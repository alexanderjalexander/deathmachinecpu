# Death Machine CPU

### By Alexander Jansiewicz and Clare O'Brien

## About

The Death Machine CPU is a 16-bit ARMv8 CPU that uses C++ to parse and interpret user inputted assembly code to machine instructions and Logisim Evolution to run the parsed instructions. 

## Requirements

The only programs that are required to run the CPU are Logisim Evolution and a compiler for C++ (we recommend using gcc). 

## Included Files

Below are all the included files that are needed for the Death Machine to work.

* Death Machine CPU Manual CS 382.pdf
    * Included manual/instructions
* death_assembler.cpp
    * C++ program to turn assembly instructions into machine code for the Death Machine
* death_assembly.txt
    * User input text file
    * Read by death_assembler, turned into machine code
* Death_Machine.circ
    * Logisim Evolution Program, containing the CPU circuitry
* demo_data
    * Demo set of instructions, loaded into the DATA_MEMORY module of the CPU in Logisim
* demo_instr
    * Demo set of instructions, loaded into the INSTRUCTION_MEMORY module of the CPU in Logisim
* makefile
    * Compiles death_assembler.cpp into a runnable program

