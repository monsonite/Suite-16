; 	SIMPL is a tiny Forth-like, extensible stack based language used to exercise the Suite_16 instruction set
; 	and provide useful programming debugging facilities

;	A Forth-Like Language in under 2048 bytes
; 
; 	SIMPL_Suite16_2020_5.asm

; 	Ken Boak January 2020 

; 	To assemble: use tasm -s -h -c -g0 -t16 SIMPL_Suite16_2020_5.asm out.hex

;----------------------------------------How SIMPL Works-----------------------------------------------

; SIMPL provides an interpreter framework for sequentially executing single character commands from a text buffer

; R8 is the instruction pointer, IP which points to the current character in the text buffer
; Fetch the character and use it to index into a jump table

; The jump table associates a code field address with each character
; Using this code field address we jump to the start of some action routine

; We need to determine whether the characers form part of a number
; these must be decoded separately and put on the stack - using the "number" routine

; The character is either a number, a primitive or an alpha - so look up the jump address
; If it's a primitive symbol or lowercase alpha between 0x20 and 0x2F or >$39
; point to a look-up table and fetch it's code segment address

; If the character is a numeral between ascii $30 and $39 jump to the "number" routine to process the digit
; Numbers are enummerated as 16-bit integers and put onto the stack by the number routine

; If the character is an ALPHA user word - it will execute from a fixed address stored in the jump table
; Each table entry jumps to a unique address and executes the code found at that code address

; After execution of the code block, each executed word jumps back to NEXT
; The NEXT routine fetches the next ascii character instruction from memory and the process repeats

;----------------------------------------------------------------------------------------------------------
;
; 	This code contains the framework routines and main utility routines for SIMPL coded for the Suite-16 cpu
;	Additional primitives or functions can easily be added at a later date, using this framework

;	Framework Routines

;	Text_Read   Get a character from the UART and store it in the text buffer
;	Text_Check  Examine the character to determine if a number, an alpha, a symbol, a colon or terminator  
;   Number      If numerical ascii characters are found - form a 16-bit integer and put it on the stack
;   Jump_Table  Look up the code address associated with the character and jump to it.
;   Execute     Run the action routine associated with the character - then jump back to NEXT
;   NEXT        Fetch the next character from the instruction buffer
;
;	Utility Routines
;
; 	Get_Number  Read the text buffer for decimal characters and form a 16-bit integer
; 	Print_Num   Print a 5 digit decimal number from a 16-bit integer
; 	Get_Hex     Read the text buffer for hexidecimal characters and form a 16-bit integer
; 	Print_Hex   Print a 4 digit hexadecimal number from a 16-bit integer
; 	HexDump		Display a block of memory in the usual hex dup fashion
;   Hex_Load	Load a hex file into memory

; 	To assemble: use tasm -s -h -c -g0 -t16 SIMPL_Suite16_2020_5.asm out.hex

;-------------------------------------------------------------------------------
; Memory Map - initial implementation

; $0000   Start
; $0000   Interpreter framework: Text_Read, number, next
; $004D   Utility Print_Num
; $0074	  Utility getchar
; $007C   Utility Get_Hex
; $009D   Utility Print_Hex
; $00C5	  Utility Hex_Dump
; $00F0   Utility crlf, puts, putc
; $0100   Jump Table			$60 words	
; $0160   Utility Hex_Loader	$A0 words
; $0200	  Utility Arithmetic & Logic 	 
; $0220	  Action Routines		$200 words
; $0420   User Words        	$360 words
; $0780   Text Input Buffer 	$20 words
; $07A0   RAM (stacks etc)		$60 words
; $07FF	  Top of RAM
 
;--------------------------------------------TO DO---------------------------------

; Loops, I/O, Strings and Delays to be added

; This version 921600 baud serial communications

; Input and output to UART Port P2 of Nucleo STM32H743 added with "i" and "o" commands

;-------------------------------------------------------------------------------

; Primitive Instructions - commence at $0200

; These allow basic maths and logical instructions on 16-bit integers     +  -  /  *  &  |   ^   ~
; Stack Manipulation   DUP DROP PUSH POP SWAP OVER
; Memory transfers with FETCH and STORE
; Compilation mode with : and ;
; Simple decrementing loops   (..........)
; Input and Output
; Print a string contained between underscores example: _Hello World_

; Note as of 22-01-2020 - not all of these are fully implemented

;   Primitives based on ascii symbols - implemented in memory $0200 to $0420:

;	ADD       +
;	SUB       -
;	SHR       /    shift right  (divide by 2)
;	SHL       *    shift left   (multiply by 2)
;	AND       &
;	OR        |
;	XOR       ^
;	INV       ~
;	DUP       “    Duplicate the top member of the stack
;	DROP      `    Drop the top member of the stack
;	PUSH      ,
;	POP       ‘
;	SWAP      $    Swap the top and 2nd members of the stack
;	OVER      %
;	FETCH     @
;	STORE     !
;	CALL      :
;	RETURN    ;
;	JMP       \
;	JE        =    Jump if top = 0
;	JGT       >    Jump if top > 0 
;	JLT       <    Jump if top < 0
;	TO-R      {    Copy top to return stack
;	FROM-R    }    Copy return stack to top
;	LOOP-Strt (    
;	LOOP-End  )
;	IN        [    Get a byte from the port addressed by top
;	OUT       ]    Send a byte from 2nd to the port addressed by top
;	KEY       ?    Get a character from serial port
;	PRINT     _    _Print the text string enclosed between the underscores_
;	NOP       Space
;   LIT       #	   Put the following literal into top

;-------------------------------------------------------------------------------

;	Lower case letters are used to execute more complex built in routines:

;	a
;	b		fetch a block of code via serial port
;	c
;	d		dump a block of code
;	e		execute code at a given block address
;	f
;	g
;	h       set port pin high
;	i       input byte from port
;	j
;	k       access the loop counter variable
;	l       set port pin low
;	m       milliseconds delay
;	n
;	o       output byte to port
;	p       print the top of stack to terminal as a 16-bit integer
;	q       print the ascii character at given RAM location
;	r       read input pin
;	s       sample the ADC
;	t
;	u       microseconds delay
;	v		define a variable
;	w
;	x
;	y
;	z

; ---------------------------------------User Defined Words----------------------------------------

;		Upper case letters A to Z are used to define 26, User "Words"

;       They are allocated fixed addresses at $20 word intervals, starting at $0420.  A = $0420, Z = $0760
;       

;		User Routines are defined by capital letters starting  with colon : and end with semicolon ;

;		eg  :F10(100mh200ml);		;	Flash the led 10 times - high for 100mS and low for 200mS

;		You can play sequences of notes (tunes) ABC etc through a small speaker  
; 		Some common notes are defined as follows

;		:A40{h1106ul1106u);			 musical note A
;		:B5{h986ul986u);			 musical note B
;       :C51{h929ul929u);			 musical note C
;       :D57{h825ul825u);			 musical note D
;       :E64{h733ul733u);			 musical note E
;       :F72{h690ul691u);			 musical note F
;       :G81{h613ul613u);			 musical note G

;       :H_Hello World, and welcome to SIMPL_;   A Banner Message

; The following 26 commands are normally available to users - shown here with starting address

;	A			$0420
;	B			$0440			
;	C			$0460
;	D			$0480
;	E			$04A0
;	F			$04C0
;	G			$04E0
;	H			$0500
;	I			$0520
;	J			$0540
;	K			$0560
;	L			$0580
;	M			$05A0
;	N			$05C0
;	O			$05E0
;	P			$0600
;	Q			$0620
;	R			$0640
;	S			$0660
;	T			$0680
;	U			$06A0
;	V			$06C0
;	W			$0700
;	X			$0720
;	Y			$0740
;	Z			$0760			

;-------------------------------------------------------------------------------

;   Examples of SIMPL phrases

; 	eg add 123 and 456 and print the result to the terminal

; 	123 456+p

;	Loop 10 times printing "Spurs are Fab!"

; 	10(_Spurs are Fab!_)

;   Flash a LED 10 times 100mS on 200mS off

;   10(h100ml200m)


;   Toggle a port pin at 1MHz   1000(hlhlhlhlhlhlhlhlhlhl)

;-------------------------------------------------------------------------------



;   SIMPL is basically a character interpreter based on a switch statement contained within a loop

; 	In assembly language the switch statement is usually best implemented as a jump table.
;   An Instruction Pointer IP, selects the next instruction from an array of characters in RAM
;   Unlike Forth that uses words stored in a dictionary, SIMPL uses single printable ascii characters
;   These characters are chosen to either be familiar symbols (+ -  * / etc) or have a high mnemonic value
;   The ascii character command is used as an index into the jump table
; 	The jump table contains the starting addresses of all the primitive commands and routines.
;   After indexing into the table, the interpreter jumps to the address it finds and then executes code
;   starting from that address.
; 	At the end of the code block, the interpreter jumps back to NEXT, to fetch the next command.

;-------------------------------------------------------------------------------------------------

; Register Usage and constants


; CPU registers - yet to be fully defined for Suite_16

; Register Usage

;	tos: 			.EQU R0			; top of stack
;	sos:			.EQU R1			; second on stack
;	temp2:			.EQU R2
;	temp3:			.EQU R3
;	temp4:			.EQU R4
;	stack: 			.EQU R5
;	scratch6:		.EQU R6
;	temp7: 			.EQU R7			; loop start
;	ip: 			.EQU R8			; instruction pointer
;	temp9: 			.EQU R9			; millisecond delay
;	temp10: 		.EQU R10        ; microsecond delay
;	temp11:			.EQU R11		; scratch
;	instr: 			.EQU R12		; instruction fetched into R12 for decoding
;	temp13:			.EQU R13		; arithmetic / scratch
;	temp14: 		.EQU R14		; used for buffer pointer
;	RSP: 			.EQU R15	    ; Return from alpha  next IP

;-------------------------------------------------------------------------------
; 	Implementing the Initialisation, TextTead, TextEval and UART routines in Suite_16 assembly language

;   The core of the SIMPL interpreter is coded as these short routines:

;	Text_Read   Get a character from the UART and store it in the text buffer
;	Text_Check  Check the character to determine it is a valid character or a terminator
;				(Valid characters are ascii $20 to $7F and may be a symbol, a number, an alpha, or a colon)
;				Colon signifies a new definition is being written, and subsequent text is treated differently   
;   Number      If numerical ascii characters are found - form a 16-bit integer and put it on the stack
;   Jump_Table  Look up the code address associated with the character and jump to it.
;   Execute     Run the action routine associated with the character - then jump back to NEXT
;   NEXT        Fetch the next character from the instruction buffer

;	SIMPL has an immediate mode - where a phrase of commands typed at the keyboard will be executed
;	immediately after the return key is pressed.

;	It also has a "colon definition" mode - If the first character in the text buffer is a colon it
;   will define a new word - based on the uppercase ALPHA character that immediately follows the colon.
;   This new definition will be stored in RAM at a fixed address.
 
;	32 bytes of storage is allocated in RAM for each ALPHA character.
;   This limitation means that definitions are kept short, concise, and can be chained together to form
; 	larger program constructs.

;----------------------------------------------------------------------------------------------------------

EOS:         	.EQU $0000      	; End of string

.org $0000
start:

;--------------------------------------Text_Read-----------------------------------------------------------
;
; Text_Read - This implements the SIMPL interpreter in Suite_16 assembly Language
;
;----------------------------------------------------------------------------------------------------------

; Get a character from the UART and store it in the input buffer starting at 0x0780

; Supports immediate mode - where characters are stored at 0x200 and executed directly
; Supports compiled mode - where string starts with a colon and the next character "NAME" is an upper case alpha
; String is stored at 0x400 plus a multiple of 32 byte offset according to the value of "NAME"

; Register Usage 

;  The input buffer - starts at $0780
;  R0 receives the next character from the text buffer pointed to by R8
;  Test it against CR, NL, unprintable characters and colon
;  If it starts with a colon - get the next character (the NAME) and store at a specific location in memory
;  If it does not start with a colon, interpret and execute each character in turn

;----------------------------------------------------------------------------------------------------------

Text_Read:

    BRA getchar             ; Get the input buffer from the UART
	
	SET R8, $780			; R8 points to char buffer
		
Text_Check:

    INC R8					; Get the next character from the buffer
	
	LD  R0, @R8				; Get it's ascii value into the accumulator
	
	ST  R4, R0			    ; Temporary store in R4
	
	SBI $0D					; Is it a carriage return?
	BEQ textEnd
	LD  R0,R4
	SBI $0A					; Is it a newline?
	BEQ textEnd
	LD  R0,R4
	SBI $20					; Is it unprintable - Too low?
	BLE nonValid
	LD  R0,R4
	SBI $7F					; Is it unprintable - Too High?
	BGE nonValid
	LD  R0,R4
	SBI $3E					; Is it a colon?
	BNE notColon
	
Colon:						; If the input character is a colon

	CALL    getchar    	    ; get the next character - which is the NAME
	SBI     $50				; subtract ascii "A" - $11 from R0 - puts it on $20 boundaries starting	at $220
	ADD		R0				; x2
	ADD		R0				; x4
	ADD		R0				; x8
	ADD     R0				; x16
	ADD		R0				; x32
	
	ADD		R8				; Add to the instruction pointer R8
	ST		R2, R0			; Temporary store this buffer address in R2
	LD		R0, R4			; Get the character back
	ST      @R2, R0			; Store it in the correct place in correct definition buffer
	
	BRA		nonValid		; move on to next step

notColon:		
	INC   R8                ; Increment buffer pointer R8
	LD	  R0, R4			; Get the character back
	ST    @R8, R0			; Store character in RAM buffer indexed by R8

nonValid:

	BRA   getchar
	
textEnd:		

	SET R0, $80
	ST	@R8, R0				; Put a null terminating (0x80) zero on the end of the buffer
	SET R8, $0780			; set ip (instruction pointer) - to start of input buffer in RAM at address 0x0200
	BRA	    NEXT			; get the next instruction

; Text_Read  43 Instruction words
	
;---------------------------------------------Number--------------------------------------------------------	
	
	
number:						; Accept up to 5 decimal digits from serial and put into memory as 16-bit integer

	SET R7, 0		    	; Clear R7	
	SET R8, $0780		    ; R8 points to char buffer
	
number1:	
	
	LD R0, @R8				; Get the first character	
	ST R4, R0				; Temporary store in R4
	
	INC R8					; Point to next character	
	LD R0, @R8				; Get the next character
	
	SBI  $30				; Is it bigger than $30 ?	
	BLT endNumber
	
	SBI $0A					; Is it less than $10	
	BGE endNumber
	
	LD R0, R4				; Get original character back
	SBI  $30				; Form an ascii digit
	
	ADD R7					; Add in accumulating total from R7
	
times10:	
	
	ST R5,R0				; Digit is now in R5	
	ADD R5					; R0 = 2 Times R0 
	ADD R0					; 4X
	ADD R0					; 8X
	ADD R5					; 9X
	ADD R5					; 10X
	
	ST R7, R0				; R7 is accumulation of all the digits multiplied by powers of 10
	
	BRA number1				; go around and process the remaining digits
	
endNumber:

	LD R0, R4				; Get the last digit back	
	ADD R7					; Add in the accumulated sum
	ST  R2,R0				; Temporary store integer number in scratch reg R2
	
	SET R7,0				; Don't forget to clear R7
	
; number  28 Instruction words	

;---------------------------------------------Next-------------------------------------------------------
; Next fetches the next instruction and decides what to do with it
; The character is either a primitive or an ALPHA - so index into jump table to get the execution address 
; Restore R to start of RAM buffer

NEXT:

	INC R8					; Point to the next character
	LD R0, @R8				; Get the next character into R0
	ADI $E0					; Add $E0 to form an address in the jump table range of $100 to $15F
	ST R3,R0				; Temporary store to R3
	
	LD R0, @R3				; Index into jump table	
	.WORD $E000             ; JMP @ R0 Jump to the command routine (if its a ascii numeral - jump to number routine)

; next 6 Instruction words	
	
;-----------------------------------------Print_Num---------------------------------------------

; Print_Num  Take the 16 bit value in top of stack register and prints it to terminal as an integer
; Method -  repeated subtraction of powers of 10

Print_Num:

    ;  Prints out the value of R0 as a 5 digit decimal number to the terminal
    ;  Leading zeroes are not suppressed
    ;  R1 = Decimation Value
    ;  R2 = current digit 
    ;  R4 = temporary storage for accumulator (decimated value)
	
	SET R2, 0
	
	SET R1, 10000
	CALL decimate
	
	SET R1, 1000
	CALL decimate
	
	SET R1, 100
	CALL decimate
	
	SET R1, 10
	CALL decimate
	
	ADI $30					; make the value an ascii numeral
	OUT						; print to terminal
	SUB R4
	
	CALL crlf				; Output a CRLF
	
	BRA NEXT
		
decimate:

	SUB R1					; subtract the power of 10 until R0 goes negative
    BLT value
    INC R2					; keep a count of how many times you subtract the decimation factor
    BRA decimate	
	
value:

	ST R4, R0
	LD R0, R2
	ADI $30				    ; Add 0x30 to make R0 a printable digit
	OUT						; print a digit
	
	LD R0, R4
	ADD R1
	SET R2, 0				; reset R2
	
	RET
	
; Print_Num   39 instruction words	
	
	
;------------------------------------------------getchar-----------------------------------------------
; 	
getchar:	
	

	IN						; Get character
	
	SET R1, $0780			; R1 points to char buffer
	
loop1:	
	LD R0, @R1				; get first character
	OUT                     ; output character
	INC R1
	BGT loop1				; get characters until you find an EOS
	BRA Text_Check		    ; Back to Text_Check when you get an EOS
	
	; getchar  8 instructions 
	
;-----------------------------------------Get_Hex---------------------------------------------
	
Get_Hex:

	; Accepts a hexadecimal number up to FFFF from terminal input buffer
	; converts it to an integer and puts it into register R0
	; It can then be printed out as a decimal using Print_Num - for checking integrity
	; R1 is the pointer in the text buffer - which starts at 0x0780
	; R4 is used as a temporary store for the character in the accumulator R0
	; R5 is used in the "Times 16" routine
	; R7 is used to accumulate the powers of 16 when forming the integer in R0
	
	SET R1, $0780				; R1 is start of text buffer
	SET R7, 0				; Clear R7

nextchar:

	LD R0, @R1				; get the first character
	ST R4, R0				; temporary store
	INC R1
	LD R0, @R1				; get the next character to see if it is numeric or terminator
	
	SBI $30					; Is it bigger than 0x30?
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
	BRA Print_Num			; Print it out in decimal
	
modify:

	; If R0 = 0-9 subtract 0x30 to form a number 0-9
	; If R0 = A-F subtract 0x37 to form a number 10-15
	
	LD R0, R4				; Get R0 back from R4
	SBI  $40				; Is it bigger than 0x40?
	BLT  decimal_num		; must be a decimal 0-9
	ADI  9                  ; Add 9 to turn R0 into 10 -15
	BRA endmodify
	
decimal_num: 

	LD R0, R4				; Get R0 back from R4
	SBI $30				    ; convert to a decimal number
	
endmodify:

	RET


; Get_Hex   33 instructions
	
	
;------------------------------------Print_Hex------------------------------------

;  Prints out a 4 digit hexadecimal number to the terminal according to contents of R0
;  Leading zeroes are not suppressed
;  R1 = Heximation Value
;  R2 = digit count
;  R4 = temporary storage for accumulator (decimated value)
;  R6 = temporary store for output character

; This is based on the Print_Num routine with modifications to handle A-F characters

Print_Hex:

		SET R2,   0
				
		SET R1,   4096
		CALL   heximate
		
		SET R1,   256
		CALL   heximate
		
		SET R1,   16
		CALL   heximate
		
		CALL printmod		; look at number and print correct character
		
		
retn:	RET		
		
heximate:

		SUB R1				; Subtract the power of 16
		BLT tempstore		; If it goes negative the decimation is finished
		INC R2				; Increment the decimation count
		BRA heximate		; repeat until negative
		
tempstore:

		ST R4, R0			; temporarily store the remainder
		LD R0, R2			; get the decimation count from R2
		CALL printmod       ; look at number and print correct character
		
		LD R0, R4			; get remainder back
		ADD R1				; adds the R1 value to restore sum
		SET R2, 0			; Clear the count
		RET
		
printmod:					; look at number and print correct character
		ADI $30				; ADD 0x30 to make it a number
		ST R6, R0			; Temporary store to R6		
		SBI $3A				; is it bigger than ascii '9'
		BLT printdec		; output as decimal digit
		ADI $41				; otherwise make it a hex digit
		OUT	
		BRA restore
		
printdec:		
		LD R0, R6
		OUT
		
restore:
		RET
		
		
;		Print_Hex  40 instructions	
	
; ---------------------------HEXDUMP------------------------------------------ 

; Dump a page of memory to the screen
; Address on LHS, followed by 16 words across the screen need about 90 characters wide
; Print out 256 words over 16 lines

; R0 = value to be printed
; R11 = pointer to memory starting at 0x0000

	SET R9,  $100		; 256 words to dump
	SET R10, $0A		; 16 words to a line
	SET R11, 0			; Begin dump from address 0
	

nextaddress:	
	
	
	LD R0, R11			; Get the address
	CALL Print_Hex		; print the address
	SET R0, $3E			; > Cursor
	OUT
	SBI $1E				; 3E - $1E  = $20  so print two spaces
	OUT
	OUT
	
nextword:

	LD R0, @R11			; get the next word from memory
	CALL Print_Hex		; print the mem-word
	SET R0, $20			; a space
	OUT
	
	INC R11				; increment memory pointer
	DEC R10				; decrement word counter
	DEC R9				; decrement page address counter
	LD R0, R10			; get word counter
	BGT nextword
	
	LD R0, R9  			; get page address counter
	
	BLE nextline
	CALL crlf
	
	SET R10, $16		; reset word counter to 16
	
	BRA nextaddress		; get address for next line

nextline:
	
	CALL crlf			; two blank lines
	
	BRA NEXT


; Hex_Dump 34 instructions

;--------------------------------------------------------------------------------------------

;
; Send a string to the serial line, HL contains the pointer to the string:
;
puts:
	CALL putc
	INC R1
	BGT puts
	RET
	
;
; Send a single character to the serial line (a contains the character):
;
putc:
	LD R0, @R1;
	OUT
	RET
	
crlf:					; Output a CR/LF pair

	SET R0, cr			; Load a cr
	OUT
	SBI 3				; Subtract 3 to make a lf
	OUT
	RET	

; Send a CR/LF pair:
;
; 	crlf:
;	SET R1, cr
;	call    putc
;	SET R1, lf
;	call    putc
;	ret


	

.org $0100					; Start of the jump table	
	
;------------------------------------------------------Jump_Table------------------------------------------

        .WORD   $00F0     ; SPACE
        .WORD   $00F1     ; STORE !
        .WORD   $00F2     ; DUP   "
        .WORD   $00F3     ; LIT   #
        
        .WORD   $00F4     ; SWAP  $
        .WORD   $00F5     ; OVER  %
        .WORD   _and      ; AND   &
        .WORD   $0F00     ; quote ' 		
        
        .WORD   $0F00     ; BEGIN (
        .WORD   $0F00     ; END   )
        .WORD   $0F00     ; MUL   *
        .WORD   _add      ; ADD   +     
        
        .WORD   $0F00     ; COMMA ,
        .WORD   _sub      ; SUB   -     
        .WORD   Print_Num ; DOT   .     Print_Num  
        .WORD   $00FF     ; DIV   /
		
		;ascii digits 0 to 9 are processed here
		
		.WORD   number    ; process numeral '0'
		.WORD   number    ; process numeral '1'
		.WORD   number    ; process numeral '2'
		.WORD   number    ; process numeral '3'
		.WORD   number    ; process numeral '4'
		.WORD   number    ; process numeral '5'
		.WORD   number    ; process numeral '6'
		.WORD   number    ; process numeral '7'
		.WORD   number    ; process numeral '8'
		.WORD   number    ; process numeral '9'
		       
 
   
;-----------------------------------------Jump Table Contd-----------------------------------------
        
        .WORD   $0F00     ; CALL  :
        .WORD   $0F00     ; RET   ;
        .WORD   $0F00     ; LESS  <
        .WORD   $0F00     ; EQ    =
		
        .WORD   $0F00     ; MORE  >
        .WORD   $0F00     ; QUERY ?
        .WORD   $00A0     ; FETCH @
		.WORD   $0420     ; A
        
        .WORD   $0440     ; B
        .WORD   $0460     ; C
        .WORD   $0480     ; D
        .WORD   $04A0     ; E

        .WORD   $04C0     ; F
        .WORD   $04E0     ; G
        .WORD   $0500     ; H
        .WORD   $0520     ; I
        
        .WORD   $0540     ; J
        .WORD   $0560     ; K
        .WORD   $0580     ; L
        .WORD   $05A0     ; M

        .WORD   $05C0     ; N
        .WORD   $05E0     ; O
        .WORD   $0600     ; P
        .WORD   $0620     ; Q

		.WORD   $0660     ; R
        .WORD   $0680     ; S
        .WORD   $06A0     ; T
        .WORD   $06C0     ; U

        .WORD   $06E0     ; V
        .WORD   $0700     ; W
        .WORD   $0720     ; X
        .WORD   $0740     ; Y
		
		.WORD   $0760     ; Z
			
        .WORD   $0F00     ; OPEN  [
		
        .WORD   $0F00     ; BACK  
        .WORD   $0F00     ; CLOSE ]        
        .WORD   _xor      ; XOR   ^      
        .WORD   $0F00     ; UNDER _
		
        .WORD   $0F00     ; TICK  
        .WORD   $0F00     ; a
        
        .WORD   $00A8     ; b
        .WORD   $0F00     ; c
        .WORD   $0F00     ; d
        .WORD   $0F00     ; e

        .WORD   $0F00     ; f
        .WORD   $0F00     ; g
        .WORD   $0F00     ; h
        .WORD   $0F00     ; i
        
        .WORD   $00B0     ; j
        .WORD   $0F00     ; k
        .WORD   $0F00     ; l
        .WORD   $0F00     ; m

        .WORD   $0F00     ; n
        .WORD   $0F00     ; o
        .WORD   $0F00     ; p
        .WORD   $0F00     ; q

        .WORD   $00B8     ; r
        .WORD   $0F00     ; s
        .WORD   $0F00     ; t
        .WORD   $0F00     ; u

        .WORD   $0F00     ; v
        .WORD   $0F00     ; w
        .WORD   $0F00     ; x
        .WORD   $0F00     ; y
    
        .WORD   $0F00     ; z
        .WORD   $0F00     ; BRACEL {
        .WORD   _or       ; OR     |   
        .WORD   $0F00     ; BRACER {
        
        .WORD   _inv      ; INV    ~   
        .WORD   $0F00     ; NOP
		
		
		; Jump_Table  96 words
                      
;--------------------------------------End of Jump Table-------------------------------------

;-----------------------------------------Hex_Loader-----------------------------------------

; To assemble use : tasm -s -h -c -g0 -t16 hexloader_new_3.asm out.hex
; Constant definitions
;
loader_start	.equ	$0160		; hexloader starts here
program_start	.equ	$0300		; hexloader jumps to here when done loading
eos				.equ	$0000		; end of string

.org	loader_start

starting_addr:
; Print a welcome message
	SET R1, hello_msg
	CALL puts
	CALL crlf
	SET R0, 62				; > cursor
	OUT						; Print cursor
	
get_colon:	
	; zero checksum
	SET R14, 0				; used to hold checksum
	; get first character 
	IN						; getchar
	SET R1, $780
	LD R0, @R1				; get character		
	SBI 58					; ':' char
	BNE get_colon

	; READ LENGTH
	call getbyte
	; value is in R7
	LD R0, R7
	ST R10, R0				; store in R[10]
	
	; READ ADDRESS
	call getword
	; value is in R7
	LD R0, R7
	ST R11, R0				; store in R[11]
	
	; READ RECORD TYPE
	call getbyte
	; value is in R7
	LD R0, R7
	ST R13, R0;				; store in R[13]
	
	; R[10] = length in bytes
	; R[11] = address 
	; R[13] = record type 
	
	SET R1, 1				; record type 01
	ST R7, R0				; store for later
	SUB R1					; check if type is 01
	BEQ endload				; record type is 01 (EOF), we are done_msg
	LD R0, R7				; get value back
	BGT invalid				; check if greater than zero (and not 01)

read_data:	
	; this is a data byte
msb_part:
	call getbyte
	; value is in R7
	LD R0, R7				; get byte in R0
	ADD R0					; shift
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ST @R11, R0				; store at memory address
	
lsb_part:
	call getbyte
	; value is in R7
	LD R0, @R11				; get what is in memory
	OR R7					; OR lsb into it
	ST @R11, R0				; store at memory address
	
	INC R11					; next memory address
	
	;decrement 2x
	DEC R10					; decrement 
	DEC R10					; decrement 
	
	LD R0, R10				; get length
	BGT read_data			; 
	
	; GET LAST BYTE
	call getbyte			; get checksum byte
	
	; CHECK CHECKSUM
	LD R0, R14				; get checksum byte
	SET R1, $00FF
	AND R1
	BNE checksum_error
	
	call crlf
	SET R0, 62				; > cursor
	OUT						; Print cursor
	BRA get_colon
	
	JMP loop				 ; TODO check
	
invalid:
	call crlf
	SET R1, invalid_msg
	call puts
	call crlf
	BRA starting_addr
	
endload:
	call getbyte			; get last checksum byte

	LD R0, R14				; get checksum byte
	SET R1, $00FF
	AND R1
	BNE checksum_error
	call crlf
	SET R1, done_msg
	call puts
	call crlf
	call crlf

	; jump to fixed memory location (loaded program)
	; loader program has to start at location "program_start" for now
	JMP program_start
	
checksum_error:
	call crlf
	SET R1, error_msg
	call puts
	call crlf
	BRA starting_addr
	
	; program should never get here !!!
loop:
	BRA loop

getnibble:
	IN				; read char from serial in
	SET R1, $780
	LD R0, @R1		; get character
	SBI $30		
	ST R2, R0		; save temporary
	SBI 10
	BLT decimal
	LD R0, R2		; restore value
	SBI 7
	BRA hexadecimal
decimal:	
	LD R0, R2		; restore value
hexadecimal:	
	ST R7, R0		; value is stored in R7
	RET
	
;
;
;
getbyte:
	call getnibble
	LD R0, R7
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ST R8, R0		; temporary store msb 
	call getnibble
	LD R0, R7		; get back lsb
	OR R8			; create word value
	ST R7, R0		; value is stored in R7
	
	; store byte for checksum
	LD R0, R14		; load checksum
	ADD R7			; add byte to checksum
	ST R14, R0
	
	RET

getword:
	call getbyte
	LD R0, R7
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ADD R0
	ST R9, R0		; CHECK IF NEEDED
	call getbyte
	LD R0, R7		; get back lsb ???
	OR R9
	ST R7, R0
	RET

	
	;---------------------------------------Arithmetic & Logic-----------------------------------
_add:	  
	  ADD R1    ; ADD
      BRA NEXT
_sub:	  
      SUB R1   ; SUB
      BRA NEXT
_and:     
      AND R1    ; AND
      BRA NEXT
_or:	  	  
      OR  R1    ; OR
      BRA NEXT
_xor:
      XOR  R1   ; XOR
      BRA NEXT
_inv:	  
      INV  R1   ; INV
      BRA NEXT
_inc:
      INC  R0   ; INC
      BRA NEXT
_dec:	  
      DEC  R0   ; DEC
      BRA NEXT
	  
	; Arithmetic and logic operations  16 instructions  
;----------------------------------------------------------------------------------------------------------	


	
; Message definitions
;
.MSFIRST
hello_msg		.WORD	"Suite16 Hex Loader", eos
error_msg		.WORD	"Checksum error", eos
invalid_msg		.WORD	"Invalid record type", eos
done_msg		.WORD	"Done loading", eos

;
; 
;
	
.MSFIRST
cr				.WORD $0d	; carriage return
lf				.WORD $0a	; line feed
space			.WORD $20	; space
.end	