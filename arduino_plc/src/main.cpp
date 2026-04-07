// Auto-generated PLC source file
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : arduino mega 2560

#include <Arduino.h>
#include "plc_config.h"

ModbusEthernet mb;
byte     mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 1, 10, 101);

// Coil registers and pins (Digital Outputs)
const uint16_t COIL_REGS[] = { COIL_DO1, COIL_DO2, COIL_DO3, COIL_DO4, COIL_DO5, COIL_DO6, COIL_DO7 };
const int      COIL_PINS[] = { 2, 3, 4, 5, 6, 7, 8 };
const uint8_t  COIL_COUNT  = sizeof(COIL_REGS) / sizeof(COIL_REGS[0]);

// General purpose flag registers (Coils, no pin)
const uint16_t FLAG_REGS[] = { FLAG_GENERAL_PURPOSE_FLAG_1, FLAG_GENERAL_PURPOSE_FLAG_2, FLAG_GENERAL_PURPOSE_FLAG_3, FLAG_GENERAL_PURPOSE_FLAG_4, FLAG_GENERAL_PURPOSE_FLAG_5 };
const uint8_t  FLAG_COUNT  = sizeof(FLAG_REGS) / sizeof(FLAG_REGS[0]);

// Discrete input registers and pins (Digital Inputs)
const uint16_t DI_REGS[] = { DI_DI1, DI_DI2, DI_DI3, DI_DI4, DI_DI5, DI_DI6, DI_DI7 };
const int      DI_PINS[] = { 22, 23, 24, 25, 26, 27, 28 };
const uint8_t  DI_COUNT  = sizeof(DI_REGS) / sizeof(DI_REGS[0]);

// Input registers and pins (Analog Inputs)
const uint16_t AI_REGS[] = { AI_AI1, AI_AI2, AI_AI3, AI_AI4, AI_AI5, AI_AI6, AI_AI7 };
const int      AI_PINS[] = { A0, A1, A2, A3, A4, A5, A6 };
const uint8_t  AI_COUNT  = sizeof(AI_REGS) / sizeof(AI_REGS[0]);

// Holding registers and pins (Analog Outputs / PWM)
const uint16_t AO_REGS[]      = { AO_AO1, AO_AO2, AO_AO3, AO_AO4, AO_AO5 };
const int      AO_PINS[]      = { 9, 10, 11, 12, 13 };
const uint8_t  AO_COUNT       = sizeof(AO_REGS) / sizeof(AO_REGS[0]);
const uint16_t AO_MIN[]       = { 0, 0, 0, 0, 0 };
const uint16_t AO_MAX[]       = { 255, 255, 255, 255, 255 };
uint16_t       aoLastValid[]  = { 0, 0, 0, 0, 0 };

// General purpose holding registers (no pin)
const uint16_t GENREG_REGS[]       = { GENREG_GENERAL_PURPOSE_REGISTER_1, GENREG_GENERAL_PURPOSE_REGISTER_2, GENREG_GENERAL_PURPOSE_REGISTER_3, GENREG_GENERAL_PURPOSE_REGISTER_4, GENREG_GENERAL_PURPOSE_REGISTER_5 };
const uint8_t  GENREG_COUNT        = sizeof(GENREG_REGS) / sizeof(GENREG_REGS[0]);
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

void loop() {
    mb.task();  // handle incoming MODBUS requests

    // Validate register values; revert and set invalid flag (reg 51) if out of range
    for (uint8_t i = 0; i < AO_COUNT; i++) {
        uint16_t val = mb.Hreg(AO_REGS[i]);
        if (val < AO_MIN[i] || val > AO_MAX[i]) {
            mb.Hreg(AO_REGS[i], aoLastValid[i]);
            mb.Coil(51, true);  // Register Value Invalid Flag
        } else {
            aoLastValid[i] = val;
        }
    }
    for (uint8_t i = 0; i < GENREG_COUNT; i++) {
        uint16_t val = mb.Hreg(GENREG_REGS[i]);
        if (val < GENREG_MIN[i] || val > GENREG_MAX[i]) {
            mb.Hreg(GENREG_REGS[i], genRegLastValid[i]);
            mb.Coil(51, true);  // Register Value Invalid Flag
        } else {
            genRegLastValid[i] = val;
        }
    }

    // Write coil values to digital output pins
    for (uint8_t i = 0; i < COIL_COUNT; i++) {
        digitalWrite(COIL_PINS[i], mb.Coil(COIL_REGS[i]));
    }

    // Read digital input pins into discrete input registers
    for (uint8_t i = 0; i < DI_COUNT; i++) {
        mb.Ists(DI_REGS[i], digitalRead(DI_PINS[i]));
    }

    // Read analog input pins into input registers
    for (uint8_t i = 0; i < AI_COUNT; i++) {
        mb.Ireg(AI_REGS[i], analogRead(AI_PINS[i]));
    }

    // Write holding register values to PWM output pins
    // Holding registers are 16-bit (0-65535); map to 8-bit PWM (0-255)
    for (uint8_t i = 0; i < AO_COUNT; i++) {
        analogWrite(AO_PINS[i], map(mb.Hreg(AO_REGS[i]), 0, 65535, 0, 255));
    }

}
