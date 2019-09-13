# Suite-16
Logic design, schematics and code for 16-bit TTL computer based on 4-bit slice

Suite-16 is a design in progress for a 16-bit TTL computer based on a 4-bit bitslice.

Its name pays homage to Steve Wozniak's "Sweet-16" a virtual 16-bit machine that was used in Apple II.

The instruction set and architecture for "Suite-16" was inspired by Woz's Sweet-16

I am most indebted to Marcel van Kervinck and Dieter Muller for their shared work that has inspired this project

Having studied other TTL computer projects, it seems that a 4-bit slice is a practical and maneagable way to proceed.

Many TTL ICs come in 4-bit versions - including registers, counters, adders and multiplexers. A 4-bit slice can be made from less than 20 ICs all in 16 pin packages, so the circuit design can be fairly compact, and easy to handle, fitting onto a 10x10 pcb. 

Sections of the design can be breadboarded to check for correct operation, or simulated using a package like "Digital"

https://github.com/hneemann/Digital/releases/tag/v0.23

Individual slices can be debugged and tested for correct operation, simplifying the bring-up process. 

If you wish to take a break after the 4-bit machine, you can always extend it later, without having to do a complete redesign.
