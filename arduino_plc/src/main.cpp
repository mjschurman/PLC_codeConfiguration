// Auto-generated PLC source file
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : Controllino Mega

#include <Arduino.h>
#include "plc_config.h"

ModbusEthernet mb;
byte     mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 1, 10, 101);

// Coil pins (Digital Outputs)
const int COIL_PINS[] = { CONTROLLINO_D0, CONTROLLINO_D1, CONTROLLINO_D2, CONTROLLINO_D3, CONTROLLINO_D4, CONTROLLINO_D5, CONTROLLINO_D6 };

// Discrete input pins (Digital Inputs)
const int DI_PINS[] = { CONTROLLINO_IN1, CONTROLLINO_IN0, CONTROLLINO_I18, CONTROLLINO_I17, CONTROLLINO_I16, CONTROLLINO_A15, CONTROLLINO_A14 };

// Input register pins (Analog Inputs)
const int AI_PINS[] = { CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4, CONTROLLINO_A5, CONTROLLINO_A6 };

// Holding register pins and validation ranges (Analog Outputs / PWM)
const int      AO_PINS[]      = { CONTROLLINO_D7, CONTROLLINO_D8, CONTROLLINO_D9, CONTROLLINO_D10, CONTROLLINO_D11 };
const uint16_t AO_MIN[]       = { 0, 0, 0, 0, 0 };
const uint16_t AO_MAX[]       = { 255, 255, 255, 255, 255 };
uint16_t       aoLastValid[]  = { 0, 0, 0, 0, 0 };

// General purpose holding register validation ranges (no pin)
const uint16_t GENREG_MIN[]        = { 0, 0, 0, 0, 0 };
const uint16_t GENREG_MAX[]        = { 100, 100, 100, 100, 100 };
uint16_t       genRegLastValid[]   = { 0, 0, 0, 0, 0 };

void setup() {
    Ethernet.begin(mac, ip);
    mb.begin();
    mb.slave(SLAVE_ID);

    // Register coils and configure digital output pins
    for (uint8_t i = 0; i < COIL_COUNT; i++) {
        mb.addCoil(COIL_REGS[i]);
        pinMode(COIL_PINS[i], OUTPUT);
    }

    // Register general purpose flags (no pin)
    for (uint8_t i = 0; i < FLAG_COUNT; i++) {
        mb.addCoil(FLAG_REGS[i]);
    }

    // Register discrete inputs and configure digital input pins
    for (uint8_t i = 0; i < DI_COUNT; i++) {
        mb.addIsts(DI_REGS[i]);
        pinMode(DI_PINS[i], INPUT);
    }

    // Register input registers (analog inputs — no pinMode needed)
    for (uint8_t i = 0; i < AI_COUNT; i++) {
        mb.addIreg(AI_REGS[i]);
    }

    // Register holding registers and configure PWM output pins
    for (uint8_t i = 0; i < AO_COUNT; i++) {
        mb.addHreg(AO_REGS[i]);
        pinMode(AO_PINS[i], OUTPUT);
    }

    // Register general purpose holding registers (no pin)
    for (uint8_t i = 0; i < GENREG_COUNT; i++) {
        mb.addHreg(GENREG_REGS[i]);
    }

}

// Read digital input pins into discrete-input registers.
void updateDiscreteInputs() {
    for (uint8_t i = 0; i < DI_COUNT; i++) {
        mb.Ists(DI_REGS[i], digitalRead(DI_PINS[i]));
    }
}

// Sample analog input pins into input registers.
void updateInputRegisters() {
    for (uint8_t i = 0; i < AI_COUNT; i++) {
        mb.Ireg(AI_REGS[i], analogRead(AI_PINS[i]));
    }
}

// Validate holding-register writes (revert + set invalid flag if out of range),
// then push the current value to the PWM output pin.
// Holding registers are 16-bit (0-65535); map to 8-bit PWM (0-255).
void updateHoldingRegisters() {
    for (uint8_t i = 0; i < AO_COUNT; i++) {
        uint16_t val = mb.Hreg(AO_REGS[i]);
        if (val < AO_MIN[i] || val > AO_MAX[i]) {
            mb.Hreg(AO_REGS[i], aoLastValid[i]);
            mb.Coil(51, true);  // Register Value Invalid Flag
        } else {
            aoLastValid[i] = val;
        }
        analogWrite(AO_PINS[i], map(mb.Hreg(AO_REGS[i]), 0, 65535, 0, 255));
    }
}

// Validate general-purpose register writes; revert + set invalid flag if out of range.
void updateGenRegs() {
    for (uint8_t i = 0; i < GENREG_COUNT; i++) {
        uint16_t val = mb.Hreg(GENREG_REGS[i]);
        if (val < GENREG_MIN[i] || val > GENREG_MAX[i]) {
            mb.Hreg(GENREG_REGS[i], genRegLastValid[i]);
            mb.Coil(51, true);  // Register Value Invalid Flag
        } else {
            genRegLastValid[i] = val;
        }
    }
}

// Drive digital output pins from coil register state.
void updateCoils() {
    for (uint8_t i = 0; i < COIL_COUNT; i++) {
        digitalWrite(COIL_PINS[i], mb.Coil(COIL_REGS[i]));
    }
}

// General-purpose flags have no default per-tick behavior.
// Add custom logic here as needed.
void updateGenFlags() {
}

void loop() {
    mb.task();  // handle incoming MODBUS requests
    updateDiscreteInputs();
    updateInputRegisters();
    updateHoldingRegisters();
    updateGenRegs();
    updateCoils();
    updateGenFlags();
}
