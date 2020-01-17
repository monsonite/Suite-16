# Suite-16

Suite-16 is a project to create a working 16-bit computer system from scratch.

It will involve logic design, software simulation, modelling on FPGA in verilog, circuit schematics and pcb design, building the processor in TTL and a whole bunch of other skills.

I have started a project on Hackaday.io where you can follow the progress.  https://hackaday.io/project/168025-suite-16

I now have a working cpu simulator that runs on any Arduino compatible board. You can use a standard Arduino UNO or MSP430 Launchpad using Energia, but for ultimate speed an STM32F743 Nucleo board running at 400MHz.

For more information, an assembler (tasm) and software simulation on PC you migh wish to check out my colleagues Frank Eggink's repository:

https://github.com/frankeggink/Suite-16-Assember


Earlier Notes:

The original aim was to create a novel 16-bit processor, as a learning exercise, and offer it as a simulation, an FPGA implementation and as a retro-computer built from real 74xx00 series TTL.

The instruction set and C simulator were mostly complete (we thought) by early November 2019, and a few hand assembled routines were coded up to allow decimal and hex number input and output over a serial interface.

Frank joined the project on November 4th 2019 - and brought along a PC version of the  simulation and an assembler based on TASM.  We his experience and an improved toolchain, plus a team-mate to bounce ideas off, we have been making reasonable progress in our spare time over the last couple of months.

Frank has written a hex-loader routine that allows either the PC simulator or the Arduino simulator to be loaded with an object file.

As of mid January, we have a fairly stable simulator, which can execute hexadecimal and decimal number entry, conversion and serial printout of numbers and text-strings.

The rest of the earlier notes are now in a text file  Old_Notes_2019.txt


