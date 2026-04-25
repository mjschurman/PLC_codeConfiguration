#pragma once

// Auto-generated PLC configuration header
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : arduino mega 2560

// Register Map
// +----------+------------------+-----+-----------------------------+----------+
// | Register | Type             | Pin | Description                 | Range    |
// +----------+------------------+-----+-----------------------------+----------+
// | 1        | Coil (DO)        | 2   | DO1                         | -        |
// | 2        | Coil (DO)        | 3   | DO2                         | -        |
// | 3        | Coil (DO)        | 4   | DO3                         | -        |
// | 4        | Coil (DO)        | 5   | DO4                         | -        |
// | 5        | Coil (DO)        | 6   | DO5                         | -        |
// | 6        | Coil (DO)        | 7   | DO6                         | -        |
// | 7        | Coil (DO)        | 8   | DO7                         | -        |
// | 50       | General Flag     | -   | System Fault Flag           | -        |
// | 51       | General Flag     | -   | Invalid Register Value Flag | -        |
// | 52       | General Flag     | -   | General Purpose Flag 3      | -        |
// | 53       | General Flag     | -   | General Purpose Flag 4      | -        |
// | 54       | General Flag     | -   | General Purpose Flag 5      | -        |
// | 101      | Discrete In (DI) | 22  | DI1                         | -        |
// | 102      | Discrete In (DI) | 23  | DI2                         | -        |
// | 103      | Discrete In (DI) | 24  | DI3                         | -        |
// | 104      | Discrete In (DI) | 25  | DI4                         | -        |
// | 105      | Discrete In (DI) | 26  | DI5                         | -        |
// | 106      | Discrete In (DI) | 27  | DI6                         | -        |
// | 107      | Discrete In (DI) | 28  | DI7                         | -        |
// | 201      | Input Reg (AI)   | A0  | AI1                         | -        |
// | 202      | Input Reg (AI)   | A1  | AI2                         | -        |
// | 203      | Input Reg (AI)   | A2  | AI3                         | -        |
// | 204      | Input Reg (AI)   | A3  | AI4                         | -        |
// | 205      | Input Reg (AI)   | A4  | AI5                         | -        |
// | 206      | Input Reg (AI)   | A5  | AI6                         | -        |
// | 207      | Input Reg (AI)   | A6  | AI7                         | -        |
// | 301      | Holding Reg (AO) | 9   | AO1                         | [0, 255] |
// | 302      | Holding Reg (AO) | 10  | AO2                         | [0, 255] |
// | 303      | Holding Reg (AO) | 11  | AO3                         | [0, 255] |
// | 304      | Holding Reg (AO) | 12  | AO4                         | [0, 255] |
// | 305      | Holding Reg (AO) | 13  | AO5                         | [0, 255] |
// | 350      | General Reg      | -   | General Purpose Register 1  | [0, 100] |
// | 351      | General Reg      | -   | General Purpose Register 2  | [0, 100] |
// | 352      | General Reg      | -   | General Purpose Register 3  | [0, 100] |
// | 353      | General Reg      | -   | General Purpose Register 4  | [0, 100] |
// | 354      | General Reg      | -   | General Purpose Register 5  | [0, 100] |
// +----------+------------------+-----+-----------------------------+----------+

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
