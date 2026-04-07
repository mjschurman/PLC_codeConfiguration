#pragma once

// Auto-generated PLC configuration header
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : arduino mega 2560

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
#define FLAG_GENERAL_PURPOSE_FLAG_1              50
#define FLAG_GENERAL_PURPOSE_FLAG_2              51
#define FLAG_GENERAL_PURPOSE_FLAG_3              52
#define FLAG_GENERAL_PURPOSE_FLAG_4              53
#define FLAG_GENERAL_PURPOSE_FLAG_5              54
