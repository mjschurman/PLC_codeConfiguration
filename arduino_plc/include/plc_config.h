#pragma once

// Auto-generated PLC configuration header
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : Controllino Mega

// Register Map
// +----------+------------------+-----------------+-----------------------------+----------+
// | Register | Type             | Pin             | Description                 | Range    |
// +----------+------------------+-----------------+-----------------------------+----------+
// | 1        | Coil (DO)        | CONTROLLINO_D0  | DO1                         | -        |
// | 2        | Coil (DO)        | CONTROLLINO_D1  | DO2                         | -        |
// | 3        | Coil (DO)        | CONTROLLINO_D2  | DO3                         | -        |
// | 4        | Coil (DO)        | CONTROLLINO_D3  | DO4                         | -        |
// | 5        | Coil (DO)        | CONTROLLINO_D4  | DO5                         | -        |
// | 6        | Coil (DO)        | CONTROLLINO_D5  | DO6                         | -        |
// | 7        | Coil (DO)        | CONTROLLINO_D6  | DO7                         | -        |
// | 50       | General Flag     | -               | System Fault Flag           | -        |
// | 51       | General Flag     | -               | Invalid Register Value Flag | -        |
// | 52       | General Flag     | -               | General Purpose Flag 3      | -        |
// | 53       | General Flag     | -               | General Purpose Flag 4      | -        |
// | 54       | General Flag     | -               | General Purpose Flag 5      | -        |
// | 101      | Discrete In (DI) | CONTROLLINO_IN1 | DI1                         | -        |
// | 102      | Discrete In (DI) | CONTROLLINO_IN0 | DI2                         | -        |
// | 103      | Discrete In (DI) | CONTROLLINO_I18 | DI3                         | -        |
// | 104      | Discrete In (DI) | CONTROLLINO_I17 | DI4                         | -        |
// | 105      | Discrete In (DI) | CONTROLLINO_I16 | DI5                         | -        |
// | 106      | Discrete In (DI) | CONTROLLINO_A15 | DI6                         | -        |
// | 107      | Discrete In (DI) | CONTROLLINO_A14 | DI7                         | -        |
// | 201      | Input Reg (AI)   | CONTROLLINO_A0  | AI1                         | -        |
// | 202      | Input Reg (AI)   | CONTROLLINO_A1  | AI2                         | -        |
// | 203      | Input Reg (AI)   | CONTROLLINO_A2  | AI3                         | -        |
// | 204      | Input Reg (AI)   | CONTROLLINO_A3  | AI4                         | -        |
// | 205      | Input Reg (AI)   | CONTROLLINO_A4  | AI5                         | -        |
// | 206      | Input Reg (AI)   | CONTROLLINO_A5  | AI6                         | -        |
// | 207      | Input Reg (AI)   | CONTROLLINO_A6  | AI7                         | -        |
// | 301      | Holding Reg (AO) | CONTROLLINO_D7  | AO1                         | [0, 255] |
// | 302      | Holding Reg (AO) | CONTROLLINO_D8  | AO2                         | [0, 255] |
// | 303      | Holding Reg (AO) | CONTROLLINO_D9  | AO3                         | [0, 255] |
// | 304      | Holding Reg (AO) | CONTROLLINO_D10 | AO4                         | [0, 255] |
// | 305      | Holding Reg (AO) | CONTROLLINO_D11 | AO5                         | [0, 255] |
// | 350      | General Reg      | -               | General Purpose Register 1  | [0, 100] |
// | 351      | General Reg      | -               | General Purpose Register 2  | [0, 100] |
// | 352      | General Reg      | -               | General Purpose Register 3  | [0, 100] |
// | 353      | General Reg      | -               | General Purpose Register 4  | [0, 100] |
// | 354      | General Reg      | -               | General Purpose Register 5  | [0, 100] |
// +----------+------------------+-----------------+-----------------------------+----------+

#include <Controllino.h>

#include <Ethernet.h>
#include <ModbusEthernet.h>

extern ModbusEthernet mb;

#define SLAVE_ID 1

// Coil addresses (Digital Outputs)
const int     COIL_REGS[] = { 1, 2, 3, 4, 5, 6, 7 };
const uint8_t COIL_COUNT  = sizeof(COIL_REGS) / sizeof(COIL_REGS[0]);

// Discrete Input addresses (Digital Inputs)
const int     DI_REGS[] = { 101, 102, 103, 104, 105, 106, 107 };
const uint8_t DI_COUNT  = sizeof(DI_REGS) / sizeof(DI_REGS[0]);

// Input Register addresses (Analog Inputs)
const int     AI_REGS[] = { 201, 202, 203, 204, 205, 206, 207 };
const uint8_t AI_COUNT  = sizeof(AI_REGS) / sizeof(AI_REGS[0]);

// Holding Register addresses (Analog Outputs / PWM)
const int     AO_REGS[] = { 301, 302, 303, 304, 305 };
const uint8_t AO_COUNT  = sizeof(AO_REGS) / sizeof(AO_REGS[0]);

// General Purpose Holding Registers
const int     GENREG_REGS[] = { 350, 351, 352, 353, 354 };
const uint8_t GENREG_COUNT  = sizeof(GENREG_REGS) / sizeof(GENREG_REGS[0]);

// General Purpose Coils (Flags)
const int     FLAG_REGS[] = { 50, 51, 52, 53, 54 };
const uint8_t FLAG_COUNT  = sizeof(FLAG_REGS) / sizeof(FLAG_REGS[0]);
