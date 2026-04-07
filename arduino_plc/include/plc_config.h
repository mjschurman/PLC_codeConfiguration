#pragma once

// Auto-generated PLC configuration header
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : arduino mega 2560

// Register Map
// +----------+------------------+-----+-----------------------------+----------+
// | Register | Type             | Pin | Description                 | Range    |
// +----------+------------------+-----+-----------------------------+----------+
// | 1        | Coil (DO)        | D2  | DO1                         | -        |
// | 2        | Coil (DO)        | D3  | DO2                         | -        |
// | 3        | Coil (DO)        | D4  | DO3                         | -        |
// | 4        | Coil (DO)        | D5  | DO4                         | -        |
// | 5        | Coil (DO)        | D6  | DO5                         | -        |
// | 6        | Coil (DO)        | D7  | DO6                         | -        |
// | 7        | Coil (DO)        | D8  | DO7                         | -        |
// | 50       | General Flag     | -   | System Fault Flag           | -        |
// | 51       | General Flag     | -   | Invalid Register Value Flag | -        |
// | 52       | General Flag     | -   | General Purpose Flag 3      | -        |
// | 53       | General Flag     | -   | General Purpose Flag 4      | -        |
// | 54       | General Flag     | -   | General Purpose Flag 5      | -        |
// | 101      | Discrete In (DI) | D22 | DI1                         | -        |
// | 102      | Discrete In (DI) | D23 | DI2                         | -        |
// | 103      | Discrete In (DI) | D24 | DI3                         | -        |
// | 104      | Discrete In (DI) | D25 | DI4                         | -        |
// | 105      | Discrete In (DI) | D26 | DI5                         | -        |
// | 106      | Discrete In (DI) | D27 | DI6                         | -        |
// | 107      | Discrete In (DI) | D28 | DI7                         | -        |
// | 201      | Input Reg (AI)   | A0  | AI1                         | -        |
// | 202      | Input Reg (AI)   | A1  | AI2                         | -        |
// | 203      | Input Reg (AI)   | A2  | AI3                         | -        |
// | 204      | Input Reg (AI)   | A3  | AI4                         | -        |
// | 205      | Input Reg (AI)   | A4  | AI5                         | -        |
// | 206      | Input Reg (AI)   | A5  | AI6                         | -        |
// | 207      | Input Reg (AI)   | A6  | AI7                         | -        |
// | 301      | Holding Reg (AO) | D9  | AO1                         | [0, 255] |
// | 302      | Holding Reg (AO) | D10 | AO2                         | [0, 255] |
// | 303      | Holding Reg (AO) | D11 | AO3                         | [0, 255] |
// | 304      | Holding Reg (AO) | D12 | AO4                         | [0, 255] |
// | 305      | Holding Reg (AO) | D13 | AO5                         | [0, 255] |
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
#define COIL_DO1                                 1
#define COIL_DO2                                 2
#define COIL_DO3                                 3
#define COIL_DO4                                 4
#define COIL_DO5                                 5
#define COIL_DO6                                 6
#define COIL_DO7                                 7

// Discrete Input addresses (Digital Inputs)
#define DI_DI1                                   101
#define DI_DI2                                   102
#define DI_DI3                                   103
#define DI_DI4                                   104
#define DI_DI5                                   105
#define DI_DI6                                   106
#define DI_DI7                                   107

// Input Register addresses (Analog Inputs)
#define AI_AI1                                   201
#define AI_AI2                                   202
#define AI_AI3                                   203
#define AI_AI4                                   204
#define AI_AI5                                   205
#define AI_AI6                                   206
#define AI_AI7                                   207

// Holding Register addresses (Analog Outputs / PWM)
#define AO_AO1                                   301
#define AO_AO2                                   302
#define AO_AO3                                   303
#define AO_AO4                                   304
#define AO_AO5                                   305

// General Purpose Holding Registers
#define GENREG_GENERAL_PURPOSE_REGISTER_1        350
#define GENREG_GENERAL_PURPOSE_REGISTER_2        351
#define GENREG_GENERAL_PURPOSE_REGISTER_3        352
#define GENREG_GENERAL_PURPOSE_REGISTER_4        353
#define GENREG_GENERAL_PURPOSE_REGISTER_5        354

// General Purpose Coils (Flags)
#define FLAG_SYSTEM_FAULT_FLAG                   50
#define FLAG_INVALID_REGISTER_VALUE_FLAG         51
#define FLAG_GENERAL_PURPOSE_FLAG_3              52
#define FLAG_GENERAL_PURPOSE_FLAG_4              53
#define FLAG_GENERAL_PURPOSE_FLAG_5              54
