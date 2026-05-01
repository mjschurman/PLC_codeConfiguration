#!/usr/bin/env python3
"""
TOML-based PLC Configuration Generator for Arduino (PlatformIO)

Parses a TOML configuration file describing MODBUS registers and generates:
  - plc_config.h  : C header with all register address definitions
  - main.cpp      : Arduino source with setup/loop for register I/O

Library: emelianov/modbus-esp8266 (supports Arduino Mega + Ethernet)
Requires Python 3.11+ (uses built-in tomllib).
"""

import sys
import tomllib
import argparse
from pathlib import Path


# ---------------------------------------------------------------------------
# Pin helpers
# ---------------------------------------------------------------------------

def parse_pin(pin_str: str) -> str:
    """Return the pin token to emit in the generated C++, verbatim.

    No transformation, no format check — whatever string the TOML provides
    ('2', 'A0', 'PD1', 'PORTB7', a board-specific macro, etc.) is forwarded
    as-is. The C++ compiler validates that the symbol resolves for the
    chosen target. Only an empty/non-string value is rejected here.
    """
    if not isinstance(pin_str, str) or not pin_str:
        raise ValueError(f"Invalid pin: {pin_str!r} (must be a non-empty string)")
    return pin_str


# ---------------------------------------------------------------------------
# Target-specific includes
# ---------------------------------------------------------------------------

# Map a substring of the TOML 'target' field (case-insensitive) to a list of
# extra C++ headers required so that target's pin-name macros resolve. An
# empty list means "stock Arduino core is sufficient" — i.e. pin numbers and
# A0..An come from <Arduino.h>, which main.cpp already includes.
# More-specific keys must come before more-general ones (first match wins).
TARGET_INCLUDE_MAP: dict[str, list[str]] = {
    'controllino': ['Controllino.h'],
    'uno':         [],
}


def target_specific_includes(target: str) -> list[str]:
    t = target.lower()
    for key, includes in TARGET_INCLUDE_MAP.items():
        if key in t:
            return includes
    return []


# ---------------------------------------------------------------------------
# Validation
# ---------------------------------------------------------------------------

def validate_toml(config: dict) -> None:
    """Raise ValueError with a descriptive message if the config is invalid."""
    for field in ('title', 'version', 'target', 'modbus', 'registers'):
        if field not in config:
            raise ValueError(f"Missing required top-level field: '{field}'")

    modbus = config['modbus']
    if 'type' not in modbus:
        raise ValueError("Missing 'modbus.type'")
    if modbus['type'] not in ('IP', 'Serial'):
        raise ValueError(
            f"Invalid modbus.type '{modbus['type']}'. Must be 'IP' or 'Serial'."
        )
    if modbus['type'] == 'IP':
        for field in ('ip_address', 'port'):
            if field not in modbus:
                raise ValueError(
                    f"Missing 'modbus.{field}' (required when modbus.type = 'IP')"
                )
        # Basic IP sanity check
        parts = modbus['ip_address'].split('.')
        if len(parts) != 4 or not all(p.isdigit() and 0 <= int(p) <= 255 for p in parts):
            raise ValueError(f"Invalid IP address: {modbus['ip_address']!r}")
    if 'slave_id' not in modbus:
        raise ValueError("Missing 'modbus.slave_id'")

    registers = config['registers']
    if not isinstance(registers, dict):
        raise ValueError("'registers' must be a TOML table")

    def check_list(entries, section, need_pin):
        if not isinstance(entries, list):
            raise ValueError(f"registers.{section} must be an array of tables")
        seen_regs = set()
        for i, entry in enumerate(entries):
            loc = f"registers.{section}[{i}]"
            if 'register' not in entry:
                raise ValueError(f"{loc}: missing 'register' field")
            if not isinstance(entry['register'], int):
                raise ValueError(f"{loc}: 'register' must be an integer")
            if entry['register'] < 0 or entry['register'] > 65535:
                raise ValueError(f"{loc}: 'register' value {entry['register']} out of range 0-65535")
            if entry['register'] in seen_regs:
                raise ValueError(f"{loc}: duplicate register address {entry['register']}")
            seen_regs.add(entry['register'])
            if need_pin and 'pin' not in entry:
                raise ValueError(f"{loc}: missing 'pin' field")
            if need_pin:
                parse_pin(entry['pin'])  # will raise if invalid
            if 'description' not in entry:
                raise ValueError(f"{loc}: missing 'description' field")

    for section in ('coils', 'discrete_inputs', 'input_registers', 'holding_registers'):
        if section in registers:
            check_list(registers[section], section, need_pin=True)
    for section in ('genRegs', 'genFlags'):
        if section in registers:
            check_list(registers[section], section, need_pin=False)

    # Validate range fields for sections that require them
    for section in ('holding_registers', 'genRegs'):
        for i, entry in enumerate(registers.get(section, [])):
            loc = f"registers.{section}[{i}]"
            if 'range' not in entry:
                raise ValueError(f"{loc}: missing 'range' field (required for value validation)")
            r = entry['range']
            if not (isinstance(r, list) and len(r) == 2 and all(isinstance(v, int) for v in r)):
                raise ValueError(f"{loc}: 'range' must be a two-element integer array [min, max]")
            if r[0] > r[1]:
                raise ValueError(f"{loc}: 'range' min ({r[0]}) must be <= max ({r[1]})")


# ---------------------------------------------------------------------------
# Header file generation
# ---------------------------------------------------------------------------

def generate_register_table(registers: dict) -> list[str]:
    """Return a formatted comment block listing all registers in a table."""
    sections = [
        ('coils',             'Coil (DO)'),
        ('discrete_inputs',   'Discrete In (DI)'),
        ('input_registers',   'Input Reg (AI)'),
        ('holding_registers', 'Holding Reg (AO)'),
        ('genRegs',           'General Reg'),
        ('genFlags',          'General Flag'),
    ]

    rows = []
    for key, type_label in sections:
        for entry in registers.get(key, []):
            pin = entry.get('pin', '-')
            r = entry.get('range')
            rng = f"[{r[0]}, {r[1]}]" if r else '-'
            rows.append((entry['register'], type_label, pin, entry['description'], rng))

    if not rows:
        return []

    # Column widths
    w_reg  = max(len('Register'),    max(len(str(r[0])) for r in rows))
    w_type = max(len('Type'),        max(len(r[1]) for r in rows))
    w_pin  = max(len('Pin'),         max(len(r[2]) for r in rows))
    w_desc = max(len('Description'), max(len(r[3]) for r in rows))
    w_rng  = max(len('Range'),       max(len(r[4]) for r in rows))

    sep = f"// +{'-'*(w_reg+2)}+{'-'*(w_type+2)}+{'-'*(w_pin+2)}+{'-'*(w_desc+2)}+{'-'*(w_rng+2)}+"
    hdr = f"// | {'Register':<{w_reg}} | {'Type':<{w_type}} | {'Pin':<{w_pin}} | {'Description':<{w_desc}} | {'Range':<{w_rng}} |"

    lines = [sep, hdr, sep]
    for reg, typ, pin, desc, rng in sorted(rows, key=lambda r: r[0]):
        lines.append(f"// | {reg:<{w_reg}} | {typ:<{w_type}} | {pin:<{w_pin}} | {desc:<{w_desc}} | {rng:<{w_rng}} |")
    lines.append(sep)
    return lines


def generate_header(config: dict) -> str:
    modbus = config['modbus']
    registers = config['registers']

    out = []
    out.append('#pragma once')
    out.append('')
    out.append(f'// Auto-generated PLC configuration header')
    out.append(f'// Title  : {config["title"]}')
    out.append(f'// Version: {config["version"]}')
    out.append(f'// Target : {config["target"]}')
    out.append('')
    out.append('// Register Map')
    out.extend(generate_register_table(registers))
    out.append('')

    # Target-specific header (provides board-specific pin macros, e.g. CONTROLLINO_*).
    target_includes = target_specific_includes(config['target'])
    for inc in target_includes:
        out.append(f'#include <{inc}>')
    if target_includes:
        out.append('')

    if modbus['type'] == 'IP':
        out.append('#include <Ethernet.h>')
        out.append('#include <ModbusEthernet.h>')
        out.append('')
        out.append('extern ModbusEthernet mb;')
    else:
        out.append('#include <Modbus.h>')
        out.append('#include <ModbusRTU.h>')
        out.append('')
        out.append('extern ModbusRTU mb;')

    out.append('')
    out.append(f'#define SLAVE_ID {modbus["slave_id"]}')
    out.append('')

    sections = [
        ('coils',             'COIL',    'Coil addresses (Digital Outputs)'),
        ('discrete_inputs',   'DI',      'Discrete Input addresses (Digital Inputs)'),
        ('input_registers',   'AI',      'Input Register addresses (Analog Inputs)'),
        ('holding_registers', 'AO',      'Holding Register addresses (Analog Outputs / PWM)'),
        ('genRegs',           'GENREG',  'General Purpose Holding Registers'),
        ('genFlags',          'FLAG',    'General Purpose Coils (Flags)'),
    ]

    for key, prefix, comment in sections:
        entries = registers.get(key)
        if not entries:
            continue
        addrs = ', '.join(str(e['register']) for e in entries)
        out.append(f'// {comment}')
        out.append(f'const int     {prefix}_REGS[] = {{ {addrs} }};')
        out.append(f'const uint8_t {prefix}_COUNT  = sizeof({prefix}_REGS) / sizeof({prefix}_REGS[0]);')
        out.append('')

    return '\n'.join(out)


# ---------------------------------------------------------------------------
# CPP file generation
# ---------------------------------------------------------------------------

def generate_cpp(config: dict) -> str:
    modbus = config['modbus']
    registers = config['registers']

    coils      = registers.get('coils', [])
    dis_inputs = registers.get('discrete_inputs', [])
    inp_regs   = registers.get('input_registers', [])
    hold_regs  = registers.get('holding_registers', [])
    gen_regs   = registers.get('genRegs', [])
    gen_flags  = registers.get('genFlags', [])

    out = []
    out.append(f'// Auto-generated PLC source file')
    out.append(f'// Title  : {config["title"]}')
    out.append(f'// Version: {config["version"]}')
    out.append(f'// Target : {config["target"]}')
    out.append('')
    out.append('#include <Arduino.h>')
    out.append('#include "plc_config.h"')
    out.append('')

    # Global modbus object
    if modbus['type'] == 'IP':
        out.append('ModbusEthernet mb;')
        ip_parts = modbus['ip_address'].split('.')
        out.append(f'byte     mac[] = {{ 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }};')
        out.append(f'IPAddress ip({", ".join(ip_parts)});')
    else:
        out.append('ModbusRTU mb;')

    out.append('')

    # ---- Global arrays -----------------------------------------------------
    # Register address arrays (COIL_REGS, DI_REGS, AI_REGS, AO_REGS, GENREG_REGS,
    # FLAG_REGS) and their *_COUNT constants are declared in plc_config.h.

    def pin_array(entries):
        return ', '.join(parse_pin(e['pin']) for e in entries)

    if coils:
        out.append('// Coil pins (Digital Outputs)')
        out.append(f'const int COIL_PINS[] = {{ {pin_array(coils)} }};')
        out.append('')

    if dis_inputs:
        out.append('// Discrete input pins (Digital Inputs)')
        out.append(f'const int DI_PINS[] = {{ {pin_array(dis_inputs)} }};')
        out.append('')

    if inp_regs:
        out.append('// Input register pins (Analog Inputs)')
        out.append(f'const int AI_PINS[] = {{ {pin_array(inp_regs)} }};')
        out.append('')

    if hold_regs:
        ao_min = ', '.join(str(e['range'][0]) for e in hold_regs)
        ao_max = ', '.join(str(e['range'][1]) for e in hold_regs)
        ao_init = ', '.join(str(e['range'][0]) for e in hold_regs)
        out.append('// Holding register pins and validation ranges (Analog Outputs / PWM)')
        out.append(f'const int      AO_PINS[]      = {{ {pin_array(hold_regs)} }};')
        out.append(f'const uint16_t AO_MIN[]       = {{ {ao_min} }};')
        out.append(f'const uint16_t AO_MAX[]       = {{ {ao_max} }};')
        out.append(f'uint16_t       aoLastValid[]  = {{ {ao_init} }};')
        out.append('')

    if gen_regs:
        gr_min = ', '.join(str(e['range'][0]) for e in gen_regs)
        gr_max = ', '.join(str(e['range'][1]) for e in gen_regs)
        gr_init = ', '.join(str(e['range'][0]) for e in gen_regs)
        out.append('// General purpose holding register validation ranges (no pin)')
        out.append(f'const uint16_t GENREG_MIN[]        = {{ {gr_min} }};')
        out.append(f'const uint16_t GENREG_MAX[]        = {{ {gr_max} }};')
        out.append(f'uint16_t       genRegLastValid[]   = {{ {gr_init} }};')
        out.append('')

    # ---- setup() -----------------------------------------------------------
    out.append('void setup() {')

    if modbus['type'] == 'IP':
        out.append('    Ethernet.begin(mac, ip);')
        out.append('    mb.begin();')
        out.append('    mb.slave(SLAVE_ID);')
    else:
        baud = modbus.get('baud', 9600)
        out.append(f'    Serial.begin({baud});')
        out.append('    mb.begin(&Serial);')
        out.append('    mb.slave(SLAVE_ID);')

    out.append('')

    if coils:
        out.append('    // Register coils and configure digital output pins')
        out.append('    for (uint8_t i = 0; i < COIL_COUNT; i++) {')
        out.append('        mb.addCoil(COIL_REGS[i]);')
        out.append('        pinMode(COIL_PINS[i], OUTPUT);')
        out.append('    }')
        out.append('')

    if gen_flags:
        out.append('    // Register general purpose flags (no pin)')
        out.append('    for (uint8_t i = 0; i < FLAG_COUNT; i++) {')
        out.append('        mb.addCoil(FLAG_REGS[i]);')
        out.append('    }')
        out.append('')

    if dis_inputs:
        out.append('    // Register discrete inputs and configure digital input pins')
        out.append('    for (uint8_t i = 0; i < DI_COUNT; i++) {')
        out.append('        mb.addIsts(DI_REGS[i]);')
        out.append('        pinMode(DI_PINS[i], INPUT);')
        out.append('    }')
        out.append('')

    if inp_regs:
        out.append('    // Register input registers (analog inputs — no pinMode needed)')
        out.append('    for (uint8_t i = 0; i < AI_COUNT; i++) {')
        out.append('        mb.addIreg(AI_REGS[i]);')
        out.append('    }')
        out.append('')

    if hold_regs:
        out.append('    // Register holding registers and configure PWM output pins')
        out.append('    for (uint8_t i = 0; i < AO_COUNT; i++) {')
        out.append('        mb.addHreg(AO_REGS[i]);')
        out.append('        pinMode(AO_PINS[i], OUTPUT);')
        out.append('    }')
        out.append('')

    if gen_regs:
        out.append('    // Register general purpose holding registers (no pin)')
        out.append('    for (uint8_t i = 0; i < GENREG_COUNT; i++) {')
        out.append('        mb.addHreg(GENREG_REGS[i]);')
        out.append('    }')
        out.append('')

    out.append('}')
    out.append('')

    # ---- Per-type update functions ----------------------------------------
    # One function per register type, called from loop(). These start out as
    # simple I/O loops; users can extend them with custom logic later.

    if dis_inputs:
        out.append('// Read digital input pins into discrete-input registers.')
        out.append('void updateDiscreteInputs() {')
        out.append('    for (uint8_t i = 0; i < DI_COUNT; i++) {')
        out.append('        mb.Ists(DI_REGS[i], digitalRead(DI_PINS[i]));')
        out.append('    }')
        out.append('}')
        out.append('')

    if inp_regs:
        out.append('// Sample analog input pins into input registers.')
        out.append('void updateInputRegisters() {')
        out.append('    for (uint8_t i = 0; i < AI_COUNT; i++) {')
        out.append('        mb.Ireg(AI_REGS[i], analogRead(AI_PINS[i]));')
        out.append('    }')
        out.append('}')
        out.append('')

    if hold_regs:
        out.append('// Validate holding-register writes (revert + set invalid flag if out of range),')
        out.append('// then push the current value to the PWM output pin.')
        out.append('// Holding registers are 16-bit (0-65535); map to 8-bit PWM (0-255).')
        out.append('void updateHoldingRegisters() {')
        out.append('    for (uint8_t i = 0; i < AO_COUNT; i++) {')
        out.append('        uint16_t val = mb.Hreg(AO_REGS[i]);')
        out.append('        if (val < AO_MIN[i] || val > AO_MAX[i]) {')
        out.append('            mb.Hreg(AO_REGS[i], aoLastValid[i]);')
        out.append('            mb.Coil(51, true);  // Register Value Invalid Flag')
        out.append('        } else {')
        out.append('            aoLastValid[i] = val;')
        out.append('        }')
        out.append('        analogWrite(AO_PINS[i], map(mb.Hreg(AO_REGS[i]), 0, 65535, 0, 255));')
        out.append('    }')
        out.append('}')
        out.append('')

    if gen_regs:
        out.append('// Validate general-purpose register writes; revert + set invalid flag if out of range.')
        out.append('void updateGenRegs() {')
        out.append('    for (uint8_t i = 0; i < GENREG_COUNT; i++) {')
        out.append('        uint16_t val = mb.Hreg(GENREG_REGS[i]);')
        out.append('        if (val < GENREG_MIN[i] || val > GENREG_MAX[i]) {')
        out.append('            mb.Hreg(GENREG_REGS[i], genRegLastValid[i]);')
        out.append('            mb.Coil(51, true);  // Register Value Invalid Flag')
        out.append('        } else {')
        out.append('            genRegLastValid[i] = val;')
        out.append('        }')
        out.append('    }')
        out.append('}')
        out.append('')

    if coils:
        out.append('// Drive digital output pins from coil register state.')
        out.append('void updateCoils() {')
        out.append('    for (uint8_t i = 0; i < COIL_COUNT; i++) {')
        out.append('        digitalWrite(COIL_PINS[i], mb.Coil(COIL_REGS[i]));')
        out.append('    }')
        out.append('}')
        out.append('')

    if gen_flags:
        out.append('// General-purpose flags have no default per-tick behavior.')
        out.append('// Add custom logic here as needed.')
        out.append('void updateGenFlags() {')
        out.append('}')
        out.append('')

    # ---- loop() ------------------------------------------------------------
    out.append('void loop() {')
    out.append('    mb.task();  // handle incoming MODBUS requests')
    if dis_inputs: out.append('    updateDiscreteInputs();')
    if inp_regs:   out.append('    updateInputRegisters();')
    if hold_regs:  out.append('    updateHoldingRegisters();')
    if gen_regs:   out.append('    updateGenRegs();')
    if coils:      out.append('    updateCoils();')
    if gen_flags:  out.append('    updateGenFlags();')
    out.append('}')
    out.append('')

    return '\n'.join(out)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description='Generate Arduino PlatformIO files from a TOML PLC configuration.'
    )
    parser.add_argument('toml_file', help='Path to the .toml configuration file')
    parser.add_argument(
        '--src-dir',
        default=None,
        help='Directory for main.cpp (default: --output-dir)',
    )
    parser.add_argument(
        '--include-dir',
        default=None,
        help='Directory for plc_config.h (default: --output-dir)',
    )
    parser.add_argument(
        '-o', '--output-dir',
        default='.',
        help='Fallback output directory when --src-dir/--include-dir are not set (default: .)',
    )
    args = parser.parse_args()

    toml_path = Path(args.toml_file)
    if not toml_path.is_file():
        print(f"Error: file not found: {toml_path}", file=sys.stderr)
        sys.exit(1)

    # Parse
    with open(toml_path, 'rb') as fh:
        try:
            config = tomllib.load(fh)
        except tomllib.TOMLDecodeError as exc:
            print(f"TOML parse error: {exc}", file=sys.stderr)
            sys.exit(1)

    # Validate
    try:
        validate_toml(config)
    except ValueError as exc:
        print(f"Configuration error: {exc}", file=sys.stderr)
        sys.exit(1)

    registers = config['registers']

    print(f"Configuration : {config['title']} v{config['version']}")
    print(f"Target        : {config['target']}")
    print(f"Modbus type   : {config['modbus']['type']}")
    print()

    # Resolve output paths
    base_dir    = Path(args.output_dir)
    src_dir     = Path(args.src_dir)     if args.src_dir     else base_dir
    include_dir = Path(args.include_dir) if args.include_dir else base_dir

    src_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)

    header_path = include_dir / 'plc_config.h'
    cpp_path    = src_dir     / 'main.cpp'

    header_path.write_text(generate_header(config), encoding='utf-8')
    cpp_path.write_text(generate_cpp(config),       encoding='utf-8')

    print(f"Generated:")
    print(f"  {header_path}")
    print(f"  {cpp_path}")

    # Summary
    counts = {
        'Coils (DO)':             len(registers.get('coils', [])),
        'Discrete Inputs (DI)':   len(registers.get('discrete_inputs', [])),
        'Input Registers (AI)':   len(registers.get('input_registers', [])),
        'Holding Registers (AO)': len(registers.get('holding_registers', [])),
        'General Registers':      len(registers.get('genRegs', [])),
        'General Flags':          len(registers.get('genFlags', [])),
    }
    total = sum(counts.values())
    print()
    print('Register summary:')
    for label, count in counts.items():
        print(f'  {label:<26}: {count}')
    print(f'  {"Total":<26}: {total}')


if __name__ == '__main__':
    main()
