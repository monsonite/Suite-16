# Suite-16

Suite-16 is a project to create a working 16-bit computer system from scratch.

It will involve logic design, software simulation, modelling on FPGA in verilog, circuit and pcb design, building in TTL and a whole bunch of other skills.

I have started a project on Hackaday.io where you can follow the progress.  https://hackaday.io/project/168025-suite-16

I now have a working cpu simulator that runs on any Arduino compatible board and will print out Hello World! to the serial terminal - you can find it in the uploaded files as Suite_16_Hello_World_1.ino

Suite-16 is based on Steve Wozniak's Sweet-16 virtual 16-bit cpu which he wrote for Apple II

It has a minimum instruction set of just 31 instructions. AC is the Accumulator R0, Rn are general purpose registers R1 to R15

/* Suite-16 Instructions

Register OPS-
     0n        ---       --     Non-Register Ops
     1n        SET       Rn     Constant  (Set)         Rn = @(PC+1)
     2n        LD        Rn     (Load)                  AC = Rn
     3n        ST        Rn     (Store)                 Rn = AC
     4n        LD        @Rn    (Load Indirect)         AC = @Rn
     5n        ST        @Rn    (Store Indirect)        @Rn = AC
     6n        PUSH      @Rn    Push AC                 @Rn = AC  Rn = Rn + 1 
     7n        POP       @Rn    Pop  AC                 AC = @Rn  Rn = Rn - 1
     8n        AND       Rn     (AND)                   AC = AC & Rn 
     9n        OR        Rn     (OR)                    AC = AC | Rn 
     An        ADD       Rn     (Add)                   AC = AC + Rn
     Bn        SUB       Rn     (Sub)                   AC = AC - Rn
     Cn        INV       Rn     (Invert)                Rn = ~Rn
     Dn        DCR       Rn     (Decrement)             Rn = Rn - 1
     En        INR       Rn     (Increment)             Rn = Rn + 1
     Fn        XOR       Rn     (XOR)                   AC = AC ^ Rn
   
  
    
Non-register OPS-
     00        JMP    16-bit                        Target = @(PC+1)
     01        BGT    AC>0                          Target = IR7:0
     02        BLT    AC<0                          Target = IR7:0
     03        BNE    AC!=0                         Target = IR7:0
     04        BEQ    AC=0                          Target = IR7:0
     05        BGE    AC>=0                         Target = IR7:0
     06        BLT    AC<=0                         Target = IR7:0
     07        BRA    Always                        Target = IR7:0
     08        CALL   16-bit                        Target = @(PC+1)
     09        RET    Return
     0A        IN                                   AC = getchar()
     0B        OUT                                  putchar(AC)
     0C                         (Unassigned)  
     0D                         (Unassigned)
     0E                         (Unassigned)
     0F        NOP                                  AC &= AC
   
   */

13-9-19

Suite-16 is a design in progress for a 16-bit TTL computer based on a 4-bit bitslice.

Having studied other TTL computer projects, it seems that a 4-bit slice is a practical and maneagable way to proceed.

Many TTL ICs come in 4-bit versions - including registers, counters, adders and multiplexers - conveniently all in 16 pin DIL packages.

A 4-bit slice can be made from less than 20 ICs all in 16 pin packages, so the circuit design can be fairly compact, and easy to handle, fitting onto a 10x10 pcb. 

26-9-19

I have started with a 4-bit ALU which is based on 4 off 74xx153 multiplexers and 74xx283 4-bit adder - based on the design by Dieter Muller from his article here: http://www.6502.org/users/dieter/a1/a1_4.htm

The design allows the bitwise logical functions AND OR XOR plus ADD add SUB

The design is created as a sub-circuit using "Digital" as alu_slice_1.dig and is the starting point for the 16-bit ALU 16_bit_alu.dig

I thought it best to start off with a 4-bit cpu - which can then be extended when the instruction set and architecture have been thoroughly thrashed out.

To turn the ALU bitslice into a cpu, I have added a program counter, a ROM, a register file (16 registers of 4 bits) and some control logic. The file is 4_bit_cpu.dig

The instruction set is very minimal - just to get something running - in hex:

0x   Load accumulator with 4-bit immediate value x from ROM

Cn   Store the accumulator into register n.

8n   Add the register n to the accumulator.

00   Reset the PC to zero - and start again

The design will be extended to allow for register indirect addressing and immediate modes.

Currently the registers can only be loaded with an immediate that has been previously loaded into the accumulator. This may be improved by having an immediate mode that uses the PC to point to the next word in ROM.

Misc Notes

Logic design, schematics and code for 16-bit TTL computer based on 4-bit slice

Its name pays homage to Steve Wozniak's "Sweet-16" a virtual 16-bit machine that was used in Apple II.

The instruction set and architecture for "Suite-16" was inspired by Woz's Sweet-16

I am most indebted to Marcel van Kervinck and Dieter Muller for their shared work that has inspired this project



Sections of the design can be breadboarded to check for correct operation, or simulated using a package like "Digital"

https://github.com/hneemann/Digital/releases/tag/v0.23

Individual slices can be debugged and tested for correct operation, simplifying the bring-up process. 

If you wish to take a break after the 4-bit machine, you can always extend it later, without having to do a complete redesign.
