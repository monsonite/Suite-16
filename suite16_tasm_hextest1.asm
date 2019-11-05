; ### NOTE ###
; to make this program compile correctly and work
; change .ino sketch: 		case 0x7:  addr = M[PC] ;  PC = addr ; break ;  // 16-bit JMP	(remove PC++)
; change TASM16.TAB: 		CALL *        08 2 NOP 1		(call uses lsb of opcode)
; assemble with command: tasm -s -h -c -g0 -t16 hello_shift.asm out.hex
; generate MEM-array with command: binToMem.exe < hello_shift.lst > output
; paste code from 'output' in .ino sketch
; code is not really shifting, it's just transferring the msb from one register to the lsb of another register 

EOS:         .EQU    $0000            	;End of string

.org $0000
start:
    NOP                     ; Leave this location free for possible branch
	SET R0,  62             ; > cursor
	OUT						; Print cursor
	IN						; Getchar
	
	SET R1, 512			    ; R1 points to char buffer
	
loop1:	
	LD R0, @R1				; get first character
	OUT                     ; output character
	INC R1
	BGT loop1				; get characters until you find an EOS
	BRA GETHEX
	NOP
	NOP
	NOP
	NOP

PRINTNUM:

    ;Prints out a 5 digit decimal number to the terminal according to contents of R0
    ; Leading zeroes are not suppressed
    ;  R1 = Decimation Value
    ;  R2 = digit 
    ;  R3 = 0x30
    ;  R4 = temporary storage for accumulator (decimated value)

	SET R2, 0
	SET R3, 48
	
	SET R1, 10000
	CALL decimate
	
	SET R1, 1000
	CALL decimate
	
	SET R1, 100
	CALL decimate
	
	SET R1, 10
	CALL decimate
	
	ADD R3
	OUT
	SUB R4
	
	SET R0, 10				;CR
	OUT
	SET R0, 13				;LF
	OUT
	BRA start
	
	NOP
	NOP

decimate:

	SUB R1
    BLT value
    INC R2
    BRA decimate	
	
value:

	ST R4, R0
	LD R0, R2
	ADD R3 					; make R0 a printable digit
	OUT						; print a digit
	
	LD R0, R4
	ADD R1
	SET R2, 0				; reset R2
	
	RET
	NOP
	NOP
	NOP
	
GETHEX:

			; Accepts a hexadecimal number up to FFFF from terminal input buffer
			; converts it to an integer and puts it into register R0
			; It can then be printed out as a decimal using PRINTNUM - for checking integrity
			; R1 is the pointer in the text buffer - which starts at 0x0200
			; R4 is used as a temporary store for the character in the accumulator R0
			; R5 is used in the "Times 16" routine
			; R7 is used to accumulate the powers of 16 when forming the integer in R0
	
	SET R1, 512				; R1 is start of text buffer
	SET R7, 0				; Clear R7

nextchar:

	LD R0, @R1
	ST R4, R0
	INC R1
	LD R0, @R1				; get the next character
	
	SBI 48					; Is it bigger than 0x30?
	BLT quit
	SBI 23					; Is is bigger than ascii 'F'
	BGE quit
	
	CALL modify
	
	ADD R7
	ADD R0					; 2X
	ADD R0					; 4X
	ADD R0					; 8X
	ADD R0					; 16X
	
	ST  R7, R0				; R7 is the accumulating sum
	
	BRA nextchar
	
quit:	
	CALL modify
	
	ADD R7					; Add in the final digit
	BRA PRINTNUM			; Print it out in decimal
	
modify:

		; If R0 = 0-9 subtract 0x30 to form a number 0-9
		; If R0 = A-F subtract 0x37 to form a number 10-15
	
	LD R0, R4				; Get R0 back from R4
	SBI  64					; Is it bigger than 0x40?
	BLT  decimal			; must be a decimal 0-9
	ADI  9                  ; Add 9 to turn R0 into 10 -15
	BRA endmodify
	
decimal: 

	LD R0, R4				; Get R0 back from R4
	SBI 48					; convert to a decimal number
	
endmodify:

	RET
	NOP
	NOP
	NOP
	NOP
	NOP
	
	
		

	
.end	