# PLC Code Configuration

A TOML-driven code generator that turns a register map description into a ready-to-build PlatformIO Arduino project that exposes those registers over MODBUS.

You write the I/O layout once, in TOML. The generator produces a `plc_config.h` (register address tables) and a `main.cpp` (setup + loop) for the [emelianov/modbus-esp8266](https://github.com/emelianov/modbus-esp8266) library running on an Arduino Mega + Ethernet shield.

---

## Repository layout

```text
PLCTOML_example.toml         Example input — register map for the demo PLC
plc_codegen.py               The generator (TOML -> .h/.cpp)
main.py                      Thin entry point that calls plc_codegen.main()
arduino_plc/
  include/plc_config.h       Generated header
  src/main.cpp               Generated Arduino source
  platformio.ini             PlatformIO project config
generated_output/             Older snapshot of generated files (reference only)
```

---

## The Python script

`plc_codegen.py` is one self-contained module. The pipeline is:

1. **Parse** the TOML file with the standard library `tomllib` (Python 3.11+).
2. **Validate** the contents — see [Validation rules](#validation-rules) below. Any problem produces a single descriptive `ValueError` and a non-zero exit.
3. **Generate `plc_config.h`** — a register-map comment table plus, for each register section, a `const int <PREFIX>_REGS[]` array of MODBUS addresses and a matching `const uint8_t <PREFIX>_COUNT`. Includes the right MODBUS library header (`ModbusEthernet` for IP, `ModbusSerial` for serial) and exposes the `extern` MODBUS object.
4. **Generate `main.cpp`** — the MODBUS object instance, parallel pin / range / "last valid" arrays, plus `setup()` and `loop()` driven entirely by index loops over the arrays from the header.

### Usage

```bash
# Default: write both files to the current directory
py plc_codegen.py PLCTOML_example.toml

# Typical PlatformIO layout
py plc_codegen.py PLCTOML_example.toml \
    --src-dir arduino_plc/src \
    --include-dir arduino_plc/include
```

`-o/--output-dir` is the fallback when `--src-dir` or `--include-dir` are not given. After running, the script prints a register-count summary so you can sanity-check that everything from the TOML made it into the build.

### Dependencies

- Python 3.11+ (uses built-in `tomllib`)
- `pymodbus` is listed in `pyproject.toml` for separately testing the generated firmware from a host (it is not needed by the generator itself).

---

## TOML schema

Top-level fields:

| Field     | Type    | Meaning                                    |
|-----------|---------|--------------------------------------------|
| `title`   | string  | Free-text title, copied into both files.   |
| `version` | string  | Free-text version, copied into both files. |
| `target`  | string  | Free-text board description — see below.   |

The `target` value is matched case-insensitively as a substring against `TARGET_INCLUDE_MAP` in `plc_codegen.py` to decide whether to emit a board-specific include in the generated header. Recognised today:

- `controllino` — adds `#include <Controllino.h>` so `CONTROLLINO_*` pin macros resolve.
- `uno` — no extra include; pin numbers and `A0`–`A5` come from `<Arduino.h>` alone.

Anything else falls through with no extra include, which is fine as long as your TOML uses pin names that resolve from the stock Arduino core or from a header you've added to your build elsewhere.

`[modbus]` block:

| Field        | Type   | Required when    | Meaning                      |
|--------------|--------|------------------|------------------------------|
| `type`       | string | always           | `"IP"` or `"Serial"`.        |
| `ip_address` | string | `type = "IP"`    | Dotted-quad, validated.      |
| `port`       | int    | `type = "IP"`    | TCP port for the slave.      |
| `baud`       | int    | optional, serial | Defaults to 9600 if omitted. |
| `slave_id`   | int    | always           | MODBUS slave/unit ID.        |

`[registers]` contains six optional arrays of tables. Each entry has `register` (the MODBUS address) and `description`; pin-bound entries also have `pin`; analog/holding entries have `range = [min, max]`.

| Section             | MODBUS function        | Pin? | Range? | Header prefix |
|---------------------|------------------------|------|--------|---------------|
| `coils`             | Coils (DO)             | yes  | no     | `COIL`        |
| `discrete_inputs`   | Discrete inputs (DI)   | yes  | no     | `DI`          |
| `input_registers`   | Input registers (AI)   | yes  | no     | `AI`          |
| `holding_registers` | Holding registers (AO) | yes  | yes    | `AO`          |
| `genRegs`           | Holding registers      | no   | yes    | `GENREG`      |
| `genFlags`          | Coils                  | no   | no     | `FLAG`        |

`genRegs` and `genFlags` are scratch holding-registers and coils that aren't tied to a physical pin — useful for state, command flags, and the "Invalid Register Value" flag described below.

### Validation rules

Performed by `validate_toml()` before any code is generated:

- Top-level `title`, `version`, `target`, `modbus`, `registers` must all be present.
- `modbus.type` must be `"IP"` or `"Serial"`. If `"IP"`, `ip_address` and `port` are required and the IP must be a valid dotted-quad.
- `modbus.slave_id` is required.
- Every register entry needs a `register` int in `[0, 65535]`, and addresses must be unique within their section.
- Pin-bound sections require a non-empty `pin` string. The generator does **not** validate or transform pin names — whatever you write (`2`, `A0`, `PD1`, `PORTB7`, a board-specific macro, etc.) is emitted verbatim into the generated C++. It is your responsibility to use a name that (a) resolves for your target's toolchain and (b) supports the role you're assigning it (e.g. a holding-register/AO pin must support PWM). Pinouts and macro names vary by board — check the datasheet and your core's headers.
- `holding_registers` and `genRegs` require `range = [min, max]` with `min <= max`.

---

## How the generated Arduino code works

The generated firmware is intentionally simple — every section uses the same loop pattern:

```text
register an address with the modbus library  →  configure pin (if any)  →  in loop(), copy values between pin and register
```

### What lives where

- **`plc_config.h`** — register-map comment table, MODBUS library include, `extern` MODBUS object, `SLAVE_ID`, and one `const int <PREFIX>_REGS[]` + `const uint8_t <PREFIX>_COUNT` per section.
- **`main.cpp`** — MODBUS object instance, MAC/IP for Ethernet, `*_PINS[]` arrays, `*_MIN[]` / `*_MAX[]` / `*LastValid[]` validation arrays, `setup()`, `loop()`.

The `*_REGS[]` arrays hold the MODBUS addresses; the `*_PINS[]` arrays hold the matching Arduino pin numbers in the same order. Loops walk both in lockstep.

### `setup()`

For each section that has entries, `setup()`:

1. Brings up the transport (`Ethernet.begin(mac, ip); mb.begin(); mb.slave(SLAVE_ID)` for IP, or `Serial.begin(baud); mb.begin(&Serial, SLAVE_ID)` for serial).
2. Iterates the `*_REGS[]` array and registers each address with the library: `mb.addCoil`, `mb.addIsts`, `mb.addIreg`, or `mb.addHreg`.
3. For pin-bound sections, calls `pinMode(...)` with `OUTPUT` (coils, holding regs) or `INPUT` (discrete inputs). Analog inputs need no `pinMode`.

### Per-type update functions

Each register type gets its own `update*()` function, emitted just above `loop()`. They start as simple I/O loops — the intent is that you extend them with custom logic later. Functions are only generated for sections that have entries in the TOML.

- **`updateDiscreteInputs()`** — `mb.Ists(DI_REGS[i], digitalRead(DI_PINS[i]))` for each DI.
- **`updateInputRegisters()`** — `mb.Ireg(AI_REGS[i], analogRead(AI_PINS[i]))` for each AI.
- **`updateHoldingRegisters()`** — validates the current `mb.Hreg(AO_REGS[i])` value against `[AO_MIN[i], AO_MAX[i]]`; on a bad write it reverts to `aoLastValid[i]` and sets coil 51 (`Invalid Register Value Flag`), on a good one it updates `aoLastValid[i]`. After validation, writes the value to the PWM output: holding-register values are 16-bit, so they're mapped to the 8-bit PWM range with `map(..., 0, 65535, 0, 255)`.
- **`updateGenRegs()`** — same validation pattern as holding registers (revert + set coil 51 on out-of-range), but with no pin output.
- **`updateCoils()`** — `digitalWrite(COIL_PINS[i], mb.Coil(COIL_REGS[i]))` for each coil.
- **`updateGenFlags()`** — emitted as an empty stub. `genFlags` are MODBUS-accessible bits with no default per-tick behavior; add custom logic if you need it.

Coil 51 is sticky — only a master clearing it will reset it. This is the safety mechanism described in the [project requirements](projectRequirementsDocument.md).

### `loop()`

Runs every iteration. It just calls `mb.task()` and then each available update function in turn:

```cpp
void loop() {
    mb.task();
    updateDiscreteInputs();
    updateInputRegisters();
    updateHoldingRegisters();
    updateGenRegs();
    updateCoils();
    updateGenFlags();
}
```

### Building

The `arduino_plc/` directory is a standalone PlatformIO project — open it in PlatformIO/VS Code and build/upload as usual. Re-run the generator any time the TOML changes; the two generated files are intended to be overwritten.
