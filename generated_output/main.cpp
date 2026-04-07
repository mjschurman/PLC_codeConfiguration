// Auto-generated PLC source file
// Title  : Arduino PLC Configuration Example
// Version: 1.0
// Target : arduino mega 2560

#include <Arduino.h>
#include "plc_config.h"

ModbusEthernet mb;
byte     mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 1, 10, 101);

void setup() {
    Ethernet.begin(mac, ip);
    mb.begin();
    mb.slave(SLAVE_ID);

    // Register coils — Digital Outputs
    mb.addCoil(COIL_DO1);  // DO1 — pin D2
    mb.addCoil(COIL_DO2);  // DO2 — pin D3
    mb.addCoil(COIL_DO3);  // DO3 — pin D4
    mb.addCoil(COIL_DO4);  // DO4 — pin D5
    mb.addCoil(COIL_DO5);  // DO5 — pin D6
    mb.addCoil(COIL_DO6);  // DO6 — pin D7
    mb.addCoil(COIL_DO7);  // DO7 — pin D8

    // Register general purpose flags (coils, no pin)
    mb.addCoil(FLAG_GENERAL_PURPOSE_FLAG_1);  // General Purpose Flag 1
    mb.addCoil(FLAG_GENERAL_PURPOSE_FLAG_2);  // General Purpose Flag 2
    mb.addCoil(FLAG_GENERAL_PURPOSE_FLAG_3);  // General Purpose Flag 3
    mb.addCoil(FLAG_GENERAL_PURPOSE_FLAG_4);  // General Purpose Flag 4
    mb.addCoil(FLAG_GENERAL_PURPOSE_FLAG_5);  // General Purpose Flag 5

    // Register discrete inputs — Digital Inputs
    mb.addIsts(DI_DI1);  // DI1 — pin D22
    mb.addIsts(DI_DI2);  // DI2 — pin D23
    mb.addIsts(DI_DI3);  // DI3 — pin D24
    mb.addIsts(DI_DI4);  // DI4 — pin D25
    mb.addIsts(DI_DI5);  // DI5 — pin D26
    mb.addIsts(DI_DI6);  // DI6 — pin D27
    mb.addIsts(DI_DI7);  // DI7 — pin D28

    // Register input registers — Analog Inputs
    mb.addIreg(AI_AI1);  // AI1 — pin A0
    mb.addIreg(AI_AI2);  // AI2 — pin A1
    mb.addIreg(AI_AI3);  // AI3 — pin A2
    mb.addIreg(AI_AI4);  // AI4 — pin A3
    mb.addIreg(AI_AI5);  // AI5 — pin A4
    mb.addIreg(AI_AI6);  // AI6 — pin A5
    mb.addIreg(AI_AI7);  // AI7 — pin A6

    // Register holding registers — Analog Outputs (PWM)
    mb.addHreg(AO_AO1);  // AO1 — pin D9
    mb.addHreg(AO_AO2);  // AO2 — pin D10
    mb.addHreg(AO_AO3);  // AO3 — pin D11
    mb.addHreg(AO_AO4);  // AO4 — pin D12
    mb.addHreg(AO_AO5);  // AO5 — pin D13

    // Register general purpose holding registers (no pin)
    mb.addHreg(GENREG_GENERAL_PURPOSE_REGISTER_1);  // General Purpose Register 1
    mb.addHreg(GENREG_GENERAL_PURPOSE_REGISTER_2);  // General Purpose Register 2
    mb.addHreg(GENREG_GENERAL_PURPOSE_REGISTER_3);  // General Purpose Register 3
    mb.addHreg(GENREG_GENERAL_PURPOSE_REGISTER_4);  // General Purpose Register 4
    mb.addHreg(GENREG_GENERAL_PURPOSE_REGISTER_5);  // General Purpose Register 5

    // Digital output pin modes
    pinMode(2, OUTPUT);  // DO1
    pinMode(3, OUTPUT);  // DO2
    pinMode(4, OUTPUT);  // DO3
    pinMode(5, OUTPUT);  // DO4
    pinMode(6, OUTPUT);  // DO5
    pinMode(7, OUTPUT);  // DO6
    pinMode(8, OUTPUT);  // DO7

    // Digital input pin modes
    pinMode(22, INPUT);  // DI1
    pinMode(23, INPUT);  // DI2
    pinMode(24, INPUT);  // DI3
    pinMode(25, INPUT);  // DI4
    pinMode(26, INPUT);  // DI5
    pinMode(27, INPUT);  // DI6
    pinMode(28, INPUT);  // DI7

}

void loop() {
    mb.task();  // handle incoming MODBUS requests

    // Write coil values to digital output pins
    digitalWrite(2, mb.Coil(COIL_DO1));  // DO1
    digitalWrite(3, mb.Coil(COIL_DO2));  // DO2
    digitalWrite(4, mb.Coil(COIL_DO3));  // DO3
    digitalWrite(5, mb.Coil(COIL_DO4));  // DO4
    digitalWrite(6, mb.Coil(COIL_DO5));  // DO5
    digitalWrite(7, mb.Coil(COIL_DO6));  // DO6
    digitalWrite(8, mb.Coil(COIL_DO7));  // DO7

    // Read digital input pins into discrete input registers
    mb.Ists(DI_DI1, digitalRead(22));  // DI1
    mb.Ists(DI_DI2, digitalRead(23));  // DI2
    mb.Ists(DI_DI3, digitalRead(24));  // DI3
    mb.Ists(DI_DI4, digitalRead(25));  // DI4
    mb.Ists(DI_DI5, digitalRead(26));  // DI5
    mb.Ists(DI_DI6, digitalRead(27));  // DI6
    mb.Ists(DI_DI7, digitalRead(28));  // DI7

    // Read analog input pins into input registers
    mb.Ireg(AI_AI1, analogRead(A0));  // AI1
    mb.Ireg(AI_AI2, analogRead(A1));  // AI2
    mb.Ireg(AI_AI3, analogRead(A2));  // AI3
    mb.Ireg(AI_AI4, analogRead(A3));  // AI4
    mb.Ireg(AI_AI5, analogRead(A4));  // AI5
    mb.Ireg(AI_AI6, analogRead(A5));  // AI6
    mb.Ireg(AI_AI7, analogRead(A6));  // AI7

    // Write holding register values to PWM output pins
    // Holding registers are 16-bit (0-65535); map to 8-bit PWM (0-255)
    analogWrite(9, map(mb.Hreg(AO_AO1), 0, 65535, 0, 255));  // AO1
    analogWrite(10, map(mb.Hreg(AO_AO2), 0, 65535, 0, 255));  // AO2
    analogWrite(11, map(mb.Hreg(AO_AO3), 0, 65535, 0, 255));  // AO3
    analogWrite(12, map(mb.Hreg(AO_AO4), 0, 65535, 0, 255));  // AO4
    analogWrite(13, map(mb.Hreg(AO_AO5), 0, 65535, 0, 255));  // AO5

}
