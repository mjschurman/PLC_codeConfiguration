# TOML Based PLC Configuration for Arduino

This document defines the requirements for python code that parses a TOML file which contains information on MODBUS registers for an arduino project and converts that TOML file into arduino code that sets up the MODBUS registers.  An example TOML file is also included called: PLCTOML_example.toml.

## Libraries used

### Python

* pymodbus: for communicating with the PLC for testing
* UV: for setting up the python environment and managing libraries
* tomllib: for handling toml files

### Arduino

For arduino the modbus functionality shall be handled by the library: emelianov/modbus-esp8266@^4.1.0.  This will define the modbus code used on the target

## Code

### A Python script that:

1) parses the TOML file and ensures the format is correct
2) After parsing, generates an arduino .ccp file for platformIO that builds all the registers, brings in all libraries, etc.
3) creates a .h file that has all of the modbus definitions required by the arduino library.  This file should also have a table (as a comment) that has the following headings: register#, type, pin, description, and Range. Values taken from toml file.
4) Configures digital pins as input or output as required by TOML file

### Arduino patterns to use

* put the registers in arrays
* put the pins in arrays
* when pin directions are defined do it in a loop
* when registers are used, do it in a loop

### Register Value Validation

For holding registers and general registers implement a value validation.  For this validation:

* only allow values withing the ranges indicated in the toml file
* Create two parallel arrays that hold the values of the holding and gen regs ("last valid" arrays).  These arrays will be used to hold the last VALID value in the register
* CHECK all register values before writing (after mb.task()).  If a value in a register is out side of the valid range, revert to old register value stored in "last valid" array.  Change the value of register 51 "Register Value invalid Flag" to TRUE.  This register can only be manually cleared by user.
* if the new register value is VALID, then update the "last valid" array with new value