# Suite-16

Suite-16 is a project to create a working 16-bit computer system from scratch.

It will involve logic design, software simulation, modelling on FPGA in verilog, circuit schematics and pcb design, building the processor in TTL and a whole bunch of other skills.

I have started a project on Hackaday.io where you can follow the progress.  https://hackaday.io/project/168025-suite-16

I now have a working cpu simulator that runs on an Arduino compatible ARM board. For ultimate speed it's best to use one of the STM32H7 Nucleo boards running at 400MHz or 480MHz.  I use a STM32H743 Nucleo board running at 400MHz.

For more information, an assembler (TASM) and software simulation running under windows on PC you might wish to check out my colleague Frank Eggink's repository:

https://github.com/frankeggink/Suite-16-Assember


Earlier Notes:

The original aim was to create a novel 16-bit processor, as a learning exercise, and offer it as a simulation running on PC or Nucleo board, an FPGA implementation and as a retro-computer built from real 74xx00 series TTL chips.

The instruction set and C simulator were mostly complete (we thought) by early November 2019, and a few hand assembled routines were coded up to prove that decimal and hex number input and output could be reliably achieved over a serial interface.

Frank joined the project on November 4th 2019 - and brought along a PC version of the  simulation and an assembler based on TASM.  With his experience and an improved toolchain, plus a team-mate to bounce ideas off, we have been making steady progress in our spare time over the last couple of months.

Frank has written a hex-loader routine that allows either the PC simulator or the Arduino simulator to be loaded from an object file.

As of mid January, we have a fairly stable simulator with hexloader, which can execute hexadecimal and decimal number entry, conversion and serial printout of numbers and text-strings.

Finalising the Instruction Set

The initial concept was to have a trivial instruction set loosely based on Steve Wozniak's "Sweet 16" 16-bit virtual machine that ran on the early 6502 based Apple machines.

We have a reduced instruction set of just 31 basic instructions, operating on a bank of 16, 16-bit registers. We utilise a paged addressing scheme which allows up to a 24-bit wide address apace to be accessed, 16M words.

The instruction set has evolved slowly over the months - especially when we started using it to try to do real text and number handling routines.

Some new instructions have been added to make it much easier to code with, more compact and much more flexible than the original ISA concept.

We are now finalising the instruction set before we progress with the next phase of the project which will be the implementation of the processor, memory and serial UART within an opensource FPGA development board. 

Whilst FPGA hardware can achieve near miracles in logic design, I am keeping in mind that I ultimately wish to implement Suite-16 as a TTL Retro-Computer - so I am really trying to keep things simple and not too far removed from what can be done efficiently using a minimum of TTL ICs.

Some Results:

On the 400MHz STM32H743 Nucleo board (about £25), the simulator runs at about 8 million simulated instructions per second.

https://hackaday.io/project/168025-suite-16/log/170353-benchmarking-suite-16



The rest of the earlier project notes are now in a text file  Old_Notes_2019.txt


