; This code contains  the main utility routines for Suite-16
; 
; GetText
; PrintNum
; PrintHex
; GetHex
; HexDump
; 
; use tasm -s -h -c -g0 -t16 Suite16_tasm_utils.asm out.hex

EOS:         .EQU    $0000  ;End of string

.org $0000
start:

    NOP                     ; Leave this location free for possible branch
	
GETTEXT:	
	
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
	
;------------------------------------Print Hex------------------------------------

;  Prints out a 4 digit hexadecimal number to the terminal according to contents of R0
;  Leading zeroes are not suppressed
;  R1 = Heximation Value
;  R2 = digit 
;  R3 = 0x30
;  R4 = temporary storage for accumulator (decimated value)
;  R6 = temporary store for output character

; This is based on the PrintNum routine with modifications to handle A-F characters

PRINTHEX:

		SET R2,   0
		SET R3,   $30
		
		SET R1,   4096
		CALL   heximate
		
		SET R1,   256
		CALL   heximate
		
		SET R1,   16
		CALL   heximate
		
		ADI $30				; Make R0 a number character
		
		ST R6, R0			; temporary store character in R6
		SBI $3A				; Is it bigger than ascii '9'
		
		BLT  printdec		; output as a decimal digit
		ADI $41				; modify R0 to make it a hex digit
		OUT					; output a hex digit
		BRA retn

printdec:
		
		LD	R0, R6			; get character back from R6

		OUT					; print it as a decimal digit
retn:	RET		
		
heximate:

		SUB R1				; Subtract the power of 16
		BLT tempstore
		INC R2
		BRA heximate
		
tempstore:

		ST R4, R0			; temporarily store the remainder
		LD R0, R2			; get the count from R2
		ADI $30				; ADD 0x30 to make a number
		ST R6, R0			; Temporary store to R6
		
		SBI $3A				; is it bigger than ascii '9'
		BLT printdec2		; output as decimal
		ADI $41				; make it a hex digit
		OUT
		
		BRA restore2
		
printdec2:		

		LD R0, R6
		OUT
		
restore2:

		LD R0, R4
		ADD R1				; adds the R1 value to restore sum
		SET R2, 0			; Clear the count
		RET
		
; ---------------------------HEXDUMP------------------------------------------ 

; Dump a page of memory to the screen
; Address on LHS, followed by 16 words across the screen need about 90 characters wide
; Print out 256 words over 16 lines

; R0 = value to be printed
; R11 = pointer to memory starting at 0x0000

	SET R13, $0D		; Preload with carriage return char
	SET R9,  $100		; 256 words to dump
	SET R10, $0A		; 16 words to a line
	SET R11, 0			; Begin dump from address 0

nextaddress:	
	
	
	LD R0, R11			; Get the address
	CALL PRINTHEX		; print the address
	SET R0, $3E			; a > cursor
	OUT
	SET R0, $20			; two spaces
	OUT
	OUT
	
nextword:

	LD R0, @R11			; get the next word from memory
	CALL PRINTHEX		; print the mem-word
	SET R0, $20			; a space
	OUT
	
	INC R11				; increment memory pointer
	DEC R10				; decrement word counter
	DEC R9				; decrement page address counter
	LD R0, R10			; get word counter
	BGT nextword
	
	LD R0, R9  			; get page address counter
	
	BLE nextline
	CALL CRLF
	
	SET R10, $16		; reset word counter to 16
	
	BRA nextaddress		; get address for next line

nextline:
	
	CALL CRLF			; two blank lines
	CALL CRLF
	
	BRA start			; Repeat ad nauseum

CRLF:					; Output a CRLF

	LD R0, R13			;  Get CR from R13
	OUT
	SBI 3				; Make a LF
	OUT
	RET
	
	
.end	