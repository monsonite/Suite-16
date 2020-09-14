# Suite-16

Suite-16 is a project to create a working 16-bit computer system from scratch.

My interest lies in developing a novel 16-bit cpu, as an exercise to further my knowledge of cpu architectures.  

My approach has 4 separate stages, each intended to allow me to develop some new skills:

1. Develop and debug the ISA and simulate on an ARM M7 - in this case Teensy 4.x and Nucleo STM32H743 - both of which can be programmed from Arduino IDE.
2. Create an FPGA soft core of the cpu, memory and UART using verilog and open source FPGA tools
3. Use a logic simulator to model the architecture. I chose the "Digital" simulator - which is an adaptation of the original "Logisim"
4. Design and build the 16-bit in traditional TTL devices - using a 4-bit, bitslice architecture

I have made good progress on the software simulation of the instruction set, all simulated in about 60 lines of Arduino code.

The instruction set is only 31  instructions - so this can easily be simulated with a couple of swich-case structures.

The instruction set is loosely based on Steve Wozniak's "Sweet-16" and is based around 1 accumulator and 15 other general purpose registers.  All mathematical and logic operations involve the accumulator and one of the general purpose registers.  

This constraint of one of the operands and the destination always being the accumulator means that a simple register file can be created using the 74xx89 16x4bit RAM ICs, and it removes the complexity of the instruction decoding and datapaths required where you have independent source and destination registers (such as PDP-11, MSP430 etc).

I have written enough code in the native assembly language to allow numerical input in both decimal and hex, plus conversions between the two bases. The simulator works well at approximately 20 million simulated instructions per second.

This Fall, I am going to tackle the 2nd part of the project which will be developing the cpu on an FPGA, whilst continuing to write a series of routines which will allow a simplified Forth-like language to be run on the cpu.

The aim is to create a virtual machine architecture of a stack machine, which can be programmed in a slightly higher language than native assembly language. Forth has its attractions for this, but Tiny BASIC and FOCAL as used on a PDP-8 and only occupied 4K words in memory but provided a complete floating point interpreded language environment, also has some merits worthy of further investigation.

Ultimately, the final project aim is to create the architecture in TTL devices. For this to be practical, I have had to be careful with the ISA so as not to create to complex an architecture that cannot be easily created in TTL. The bitslice approach will hopefully allow me to keep the hardware simple.  I have already run the 4-bit slice through a logic simulator "Digital" which is an updated version of the original "Logisim".

It will involve logic design, software simulation, modelling the cpu in FPGA in verilog, circuit schematics and pcb design, building the processor in TTL and a whole bunch of other skills.

I have started a project on Hackaday.io where you can follow the progress.  https://hackaday.io/project/168025-suite-16

I now have a working cpu simulator that runs on an Arduino compatible ARM board. For ultimate speed it's best to use one of the Teensy 4.0 boards running at 600MHz.  I previously used a STM32H743 Nucleo board running at 400MHz.

For more information, an assembler (TASM) and software simulation running under windows on PC you might wish to check out my colleague Frank Eggink's repository:

https://github.com/frankeggink/Suite-16-Assember


Some Background:

The original aim was to create a novel 16-bit processor, as a learning exercise, and offer it as a simulation running on PC, Teensy or Nucleo board, an FPGA implementation and as a retro-computer built from real 74xx00 series TTL chips.

The instruction set and C simulator were mostly complete by early November 2019, and a few hand assembled routines were coded up to prove that decimal and hex number input and output could be reliably achieved over a serial interface.

Frank joined the project in November 2019 - and brought along a PC version of the simulation and an assembler based on TASM.
  
With his experience and an improved toolchain, plus a team-mate to bounce ideas off, we have been making steady progress in our spare time over the last couple of months.

Frank has written a hex-loader routine that allows either the PC simulator or the Arduino simulator to be loaded from an object file.

As of mid January, we have a fairly stable simulator with hexloader, which can execute hexadecimal and decimal number entry, conversion and serial printout of numbers and text-strings.

Finalising the Instruction Set

The initial concept was to have a trivial instruction set loosely based on Steve Wozniak's "Sweet 16" 16-bit virtual machine that ran on the early 6502 based Apple machines.

Several influences for the ISA came from the PDP-8, the PDP-11 and the MSP430. 

The PDP-8 is a simple accumulator architecture, where all the operands need to be loaded from and stored back to RAM. Suite-16 takes this simple architecture but adds a register file of 15 general purpose registers, which are directly addressed from a 4-bit field in the instruction word. This allows many instructions to be executed without access to the external memory, using the register file as temporary storage.

We have a reduced instruction set of just 31 basic instructions, operating on a bank of 16, 16-bit registers. I utilise a paged addressing scheme which allows up to a 24-bit wide address apace to be accessed, 16M words.

The instruction set has evolved slowly over the months - especially when I started using it to try to do real text and number handling routines.

Some new instructions have been added to make it much easier to code with, more compact and much more flexible than the original ISA concept.

We are now finalising the instruction set before we progress with the next phase of the project which will be the implementation of the processor, memory and serial UART within an opensource FPGA development board using verilog. 

Whilst FPGA hardware can achieve near miracles in logic design, I am keeping in mind that I ultimately wish to implement Suite-16 as a TTL Retro-Computer - so I am really trying to keep things simple and not too far removed from what can be done efficiently using a minimum of TTL ICs.

Some Results:

On the 400MHz STM32H743 Nucleo board (about £25), the simulator runs at about 8 million simulated instructions per second.

Using TeraTerm and Frank's hexloader, you can send a hex file to the STM32H743 simulator at 921600 baud.

https://hackaday.io/project/168025-suite-16/log/170353-benchmarking-suite-16

I have subsequently moved to the Teensy 4.0 which uses a 600MHz ARM Cortex M7 from NXP. The Teensy 4.0 is supported by Arduino, PlatformIO as well as CircuitPython and MicroPython. At about $20 it makes a very low cost platform for developing experimental cpu architectures.


The rest of the earlier project notes are now in a text file  Old_Notes_2019.txt
