// A simple simulator for Suite-16 processor

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
     00        BRA    Always                        Target = IR7:0
     01        BGT    AC>0                          Target = IR7:0
     02        BLT    AC<0                          Target = IR7:0
     03        BGE    AC>=0                         Target = IR7:0
     04        BLE    AC<=0                         Target = IR7:0 
     05        BNE    AC!=0                         Target = IR7:0
     06        BEQ    AC=0                          Target = IR7:0     
     07        JMP    16-bit                        Target = @(PC+1)
     08        CALL   16-bit                        Target = @(PC+1)
     09        RET    Return
     0A        LUT    Look Up                       PC= PC + AC
     0B        IN                                   AC = getchar()
     0C        OUT                                  putchar(AC)
     0D                         (Unassigned)  
     0E                         (Unassigned)
     0F        NOP                                  AC &= AC
   
   */
 

 
#define MEMSIZE  512
unsigned int M[MEMSIZE] = {
         
   //    0x1000,     // SET R0, 0x0000
   //    0x0000, 

   // 0x0000 ---------------------------SIMPL------------------------------------------ 

    //  R0 receives the next character from the text buffer
    //  Test it against @ and [ to see if it falls into the ALPHA renge
    //  R1 points to the character in the input buffer  address 0x0110
    //  R2 contains the [ test limit  0x5B
    //  R3 contains the @ test limit  0x40
    //  R4 is temprorary storage for R0
    //  R5 contains an offset of 0x60 that allows indexing into the jump table
    //  R6 holds the execution address for the ALPHA command  
        
        0x00F0,    // BRA 0x00F0
        0x0F00,    // NOP
        0x0F00,    // NOP
        
        0x1100,     // Preload R1 with 0x110 Start of Text Input Buffer
        0x010F,
        0x1500,    // SET R5, 0x0060
        0x0060,
        
        0x1200,    // SET R2, 0x005B   Value for "["
        0x005B,
        0x1300,    // SET R3, 0x0040   Value for "@"
        0x0040,

        0xE100,     // INC R1  Get the next character
        0x4100,     // LD AC, @R1  get next character - and test to see if it is an ALPHA 
        0x3400,     // Temporary store R0 in R4
        0xB300,     // Subtract R3 Is it bigger than 0x40?
        
        0x0218,     // BLT Not an ALPHA 
       
        
        

        // 0x0010--------------------------------------------------------------------------------------


        0x2400,     // LD  R0, R4   Get the command character back in R0            
        0xB200,     // Subtract R2  0x5B      
        0x0318,     // BGE Not a ALPHA


        0x0F00,     // NOP  If you arrive here you have an ALPHA in R4 which you need to put in R0 and ADD 0x200 (Start of ALPHA handlers) Then store this in R6
        0x1000,     // Preload R0 with 0x200
        0x0200,
        0xA400,     // ADD R4, R0 
        0x3600,     // ST  R0, R6   ALPHA jump table address in R6

        
       

         // 0x0018----------------------------Not an ALPHA------------------------------------------------
         
        0x2400,     // LD  R0, R4   Get the command character back in R0
        0xA500,     // ADD R0, R5   R0 is now the address in the look up table we want to jump through - starting at 0x80 to 0xC5
        0x0A00,     // JMP @R0      Jump to the command routine   
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
       
     // 0x0020 ---------------------------------------------------------

        0x0008,     // BRA 0x0008 NEXT
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
       

 // 0x0030 ------------------------PRINTBUF---------------------------------

 //  This indexes through the text input buffer printing out each character as it finds them
 //  It stops when it finds the null terminator

        0x0F00,     // NOP 
        0x1000,     // SET R0, 0x003E
        0x003E,      
        0x0C00,     // PUT CHAR  put out > cursor      
              
        0x0B00,     // GET CHAR buffer  M(512 + 63 max)  This will pause execution until it gets some serial followed by newline
        0x1100,     // SET R1, 0x200     buffer start
        0x0200,
        0x4100,     // LD AC, @R1  get first character
        
        0x0C00,     // Output the character
        0xE100,     // INC R1 increment pointer
        0x0137,     // BNZ 0x37 loop around until you find 0 terminator
        0x0F00,     // NOP
       
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

      
     // 0x0040 --------------------------------NUMBER------------------------------------- 

     // This accepts a string of characters from the input buffer and checks if they are numerical (0-9)
     // It accumulates a 16-bit decimal number in the accumulator R0.
     // R0 used to accept characters and test them against numerical bounds
     // R1 is used as a pointer into the input buffer
     // R2 contains the constant 0x0A  decimal 10 to check upper digit bound
     // R3 contains the constant 0x30 which is used to convert ascii to number
     // R4 is used to temporarily store R0 when R0 is needed for math operations
     // R5 is used in the Times 10 multiplication routine 
     // R7 is used to accumulate the decimal powers  
     
        0x1300,     // SET R3 0x30   Preload R3 with decimal 48
        0x0030,
        0x1200,     // SET R2, 0x0A  Preload R2 with decimal 10
        0x000A,
        
        0x1100,     // SET R1, 0x0200    text buffer start
        0x0200,
        0x4100,     // LD AC, @R1  get first character from buffer
        0x3400,     // Store R0 in R4
        
        0xE100,     // INC R1
        0x4100,     // LD AC, @R1  get next character - and test to see if it is a number
        0xB300,     // Subtract R3 Is it bigger than 0x30?
        0x025A,     // BLT Not a Number
        
        0xB200,     // Subtract R2  0x0A
        0x035A,     // BGE Not a Number
        0x2400,     // Get original character  R0 back from R4
        0xB300,     // Subtract R3 to form a digit


       // 0x0050 --------------------------------------------------------------------- 
              
        0xA700,     // get the accumulating total from R7 - ready to multiply
        0x3500,     // Store R0 in R5  Copy is now in R5
        0xA500,     // ADD R5 to R0  R0 = 2 * digit  "Times 10"
        0x3500,     // Store R0 in R5   2 * Digit is now in R5
        
        0xA500,     // ADD R5   4 X
        0xA500,     // ADD R5   6 X        
        0xA500,     // ADD R5   8 X 
        0xA500,     // ADD R5   10 X 
        
        0x3700,     // Store R0 in R7   R7 is the accumulating total of all the digits multiplied by powers of 10        
        0x0046,     // BRA 0x0046 Get the next digit
        0x0F00,     // Not a number  address = 0x5A
        0x2400,     // Get last digit  R0 back from R4
               
        0xB300,     // Subtract R3
        0xA700,     // Add the accumulated sum from R7 - decimal number is now it R0
        0x1700,     // Don't forget to clear R7
        0x0000,

        

        

    // 0x0060 ---------------------------PRINTNUM_2-----------------------------------------
        
    // This prints out the 16-bit value in the accumulator as a 5-digit decimal number
    //  R1 = Decimation Value
    //  R2 = digit 
    //  R3 = 0x30
    //  R4 = temporary storage for accumulator (decimated value)
        
        0x1200,     // SET R2, 0x0000
        0x0000,
        0x1300,     // SET R3, 0x0030
        0x0030,
        
        0x1100,     // R1 =  10,000
        0x2710,
        0x0873,     // CALL decimate
       
        0x1100,     // R1 = 1000
        0x03E8,
        0x0873,     // CALL decimate
        
        0x1100,     // R1 = 100
        0x0064,
        0x0873,     // CALL decimate
        
        0x1100,     // R1 = 10
        0x000A,
        0x0873,     // CALL decimate
        
                
     // 0x0070 ---------------------------------------------------------
                      
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0 - print digit
        0x0090,     // BRA CRLF
        
     // 0x0073 ------------------------Decimate--------------------------------
      
        0xB100,     // SUB R1,     :Decimate 
        0x0277,     // BLT 0x77   
        0xE200,     // INC R2
        0x0073,     // BRA 0x073

    
        0x3400,     // Store R0 in R4   
        0x2200,     // MOV R2, R0,  
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
         
        0x2400,     // Get R0 back from R4                 
        0xA100,     // ADD R1 adds DEC value to restore R0       
        0x1200,     // SET R2,0    Reset R2
        0x0000,

        0x0900,     // RET
        
     
     // 0x0080 -----------------------------JUMP TABLE--------------------------------
  
        0x00F0,     // SPACE
        0x00F1,     // STORE !
        0x00F2,     // DUP   "
        0x00F3,     // LIT   #
        
        0x00F4,     // SWAP  $
        0x00F5,     // OVER  %
        0x00A4,     // AND   &     0x00D4
        0x00F6,     // DROP  '
        
        0x0F00,     // BEGIN (
        0x0F00,     // END   )
        0x0F00,     // MUL   *
        0x00D0,     // ADD   +     0x00D0
        
        0x0F00,     // COMMA ,
        0x00D2,     // SUB   -     0x00D2
        0x0010,     // DOT   .     Printnum  0x0010
        0x00FF,     // DIV   /

     

      // 0x0090 --------------------------------------------------------------------- 

        
        0x1000,     // SET R0, CR  Emit CRLF
        0x000D,
        0x0C00,     // putchar R0 CR      
        0x1000,     // Set R0, LF
        0x000A,
        0x0C00,     // putchar R0 LF     
        0x00E8,     // BRA 0xE8
        0x0F00,     // NOP
     
        0x0098,     // NOP
        0x0F00,     // NOP
        
        
        0x0F00,     // CALL  :
        0x0F00,     // RET   ;
        0x0F00,     // LESS  <
        0x0F00,     // EQ    =
        0x0F00,     // MORE  >
        0x0F00,     // QUERY ?

        // 0x00A0 -----------------------Jump Table----------------------------------

        0x00A0,     // FETCH @
        0x0F00,     // OPEN  [
        0x0F00,     // BACK  
        0x0F00,     // CLOSE ]
        
        0x00D8,     // XOR   ^       0x00D8
        0x0F00,     // UNDER _
        0x0F00,     // TICK  
        0x0F00,     // a
        
        0x00A8,     // b
        0x0F00,     // c
        0x0F00,     // d
        0x0F00,     // e

        0x0F00,     // f
        0x0F00,     // g
        0x0F00,     // h
        0x0F00,     // i
        
// 0x00B0 ----------------------------------------------------------------

        0x00B0,     // j
        0x0F00,     // k
        0x0F00,     // l
        0x0F00,     // m

        0x0F00,     // n
        0x0F00,     // o
        0x0F00,     // p
        0x0F00,     // q

        0x00B8,     // r
        0x0F00,     // s
        0x0F00,     // t
        0x0F00,     // u

        0x0F00,     // v
        0x0F00,     // w
        0x0F00,     // x
        0x0F00,     // y

        // 0x00C0 ----------------------------------------------------------------
       
        0x0F00,     // z
        0x0F00,     // BRACEL {
        0x00D6,     // OR     |   0x00D6
        0x0F00,     // BRACER {
        
        0x00DA,     // INV    ~   0x00DA
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
       
        0x00C8,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
              
        // 0x00D0 -----------------------ALU OPS----------------------------------

        0xA100,     // ADD R0,R1    ADD
        0x0020,     // BRA NEXT
        0xB100,     // SUB R0, R1   SUB
        0x0020,     // BRA NEXT

        0x8100,     // AND R0,R1    AND
        0x0020,     // BRA NEXT
        0x9100,     // OR  R0,R1    OR
        0x0020,     // BRA NEXT

        0xF100,     // XOR  R0,R1   XOR
        0x0020,     // BRA NEXT
        0xC000,     // INV  R0,R1   INV
        0x0020,     // BRA NEXT

        0xE000,     // INC  R0      INC
        0x0020,     // BRA NEXT
        0xD000,     // DEC  R0      DEC
        0x0020,     // BRA NEXT

        // 0x00E0 ---------------------------------------------------------

        0x1E00,     // SET R14, 0x100 as stack pointer
        0x0100,     // 
        0x1000,     // SET R0, 0x0010
        0x0010,     //

       
        
        0x7E00,     // PUSH R0, @R14  Push 16 items on stack
        0xD000,     // DEC  R0 
        0x03E4,     // BGE  0xE4
        0x0F00,     // NOP
        
        0x6E00,     // POP  @14, R0
        0x0060,     // BRA PRINTNUM  0x0060
        0x00E8,     // BRA 0xE8
        0x0F00,     // NOP
        
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        

        // 0x00F0 --------------------------------------------------------------------- 

        0x1000,     // LD R0, 0x7FFF
        0x7FFF,     // 
        0xD000,     // DEC R0
        0x03F2,     // BGE 0xF2
        
        0x0E00,     // Print micros
        0x00F0,     // BRA 0xF0
        0x0F00,     // NOP
        0x0F00,     // NOP
        
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        

     // 0x0100 ----------------------String Output----------------------------------------

        0x1200,     // SET R2,0
        0x0000, 
        0xE500,     // INC R5
        0x2500,     // LD R0, R5
        
        0x0002,     // BRA 0002
        0x1100,     // SET R1, 0xD0    Start of String
        0x00D0,
        0x4100,     // LD AC, @R1
        
        0x0C00,     // Output the character
        0xE100,     // INC R1
        0x016C,     // BNZ 0x6C
        0x0900,     // RET
         
        0x0F00,    // NOP
        0x0F00,    // NOP
        0x0F00,    // NOP
        0x0F00,    // NOP
  
        

  // 0x0110 ---------------------------Text Input  Buffer------------------------------------------ 
         
        '&',
        '|',
        '^',
        '~',
        '+',
        '-',
        '*',
        '/',
        '?',
        '@',
        '[',
        0x005C,    // Backslash
        ']',
        '{',
        '\n',
        0x0000,
        
 // 0x120 ---------------------------CRLF------------------------------------------ 
        
        
        0x1000,     // SET R0, CR
        0x000D,
        0x0C00,     // putchar R0 CR
        
        0x1000,     // Set R0, LF
        0x000A,
        0x0C00,     // putchar R0 LF     
        0x0000,     // BRA START 
        0x0F00,     // NOP 
        
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        
// 0x0130 --------------------------------------------------------------------- 

        0x1E00,     // SET R14, 0x100 as stack pointer
        0x0100,     // 
        0x1000,     // SET R0, 0x0010
        0x0010,     //
        
        0x7E00,     // PUSH R0, @R14  Push 16 items on stack
        0xD000,     // DEC  R0 
        0x03E4,     // BGE  0xE4
        0x0F00,     // NOP
        
        0x6E00,     // POP  @14, R0
        0x8060,     // CALL PRINTNUM
        0x00E8,     // BRA 0xE8
        0x0F00,     // NOP
        
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        // 0x0140 --------------------------------------------------------------------- 

        0x1100,     // SET R1, 0x5C     address = 0xB9
        0x00B9,
        0x4100,     // LD AC, @R1
        0x0C00,     // Output the character
        0xE100,     // INC R1
        0x01B4,     // BNZ 0x57   
        0x0900,     // RET 
        'S',
        'u',
        'b',
        'r',
        'o',
        'u',
        't',
        '\n',
        0x0000, 

  // 0x0150 ---------------------------SIMPL------------------------------------------ 
         
        'S',
        'I',
        'M',
        'P',
        'L',
        ' ',
        '(',
        'c',
        ')',
        ' ',
        '2',
        '0',
        '1',
        '9',
        '\n',
        0x0000,
              
        
} ;

unsigned long count = 0 ;
int A = 0 ; int PC = 0  ;   int  i=0 ; 
unsigned int IR = 0  ;
int R[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x0200} ;  // Zero the Registers, Set the top of the return stack at 0x0200
 

void setup() {  Serial.begin(115200); 
/*
while (count <= 300) {
Serial.print (count, HEX); Serial.print ("    ") ; Serial.println(M[count], HEX);
count ++ ;
}
*/
}

void loop() {  fetch() ; execute() ;  }
 
void fetch() {  IR = M[PC] ;    PC ++ ;  PC &= (MEMSIZE-1) ; } 
 
void execute()
{
  
/* Instruction Decoder */
  
  unsigned int op = IR >> 12 ;            // op = IR 15:12      Opcode             
  unsigned int n  = (IR & 0xf00) >> 8 ;   // n = IR 11:8        Register or Condition
  int addr = IR & 0x0ff ;                 // addr = IR 7:0      Address or Displacement
 
  
  /* Opcode Execute */
   
    switch (op) {
      
    case 0x0:   break ; 
    case 0x1:   R[n]= M[PC] ; PC++  ; break ;  /* SET */      
    case 0x2:   R[0] = R[n]         ; break ;  /* LD */
    case 0x3:   R[n] = R[0]         ; break ;  /* ST */
    case 0x4:   R[0] = M[R[n]]      ; break ;  /* LD@ */
    case 0x5:   M[R[n]] = R[0]      ; break ;  /* ST@ */
    case 0x6:   R[0] = M[R[n]]      ; R[n]= R[n]+1   ;  break ; /* POP with post-increment of pointer Rn  */
    case 0x7:   R[n]= R[n]-1        ; M[R[n]] = R[0] ;  break ; /* PSH with pre-decrement of pointer Rn */  
    case 0x8:   R[0] &= R[n]        ; break ;  /* AND */
    case 0x9:   R[0] |= R[n]        ; break ;  /* OR  */
    case 0xA:   R[0] += R[n]        ; break ;  /* ADD */     
    case 0xB:   R[0] -= R[n]        ; break ;  /* SUB */
    case 0xC:   R[n] = ~R[n]        ; break ;  /* INV */
    case 0xD:   R[n] =  R[n]-1      ; break ;  /* DEC */
    case 0xE:   R[n] =  R[n]+1      ; break ;  /* INC */
    case 0xF:   R[0] ^= R[n]        ; break ;  /* XOR */        
    }

    /* Conditional Branches and I/O Group */
    
    A = R[0] ;
    if (op == 0) {      // do an unconditional jump back to enclosed address

switch (n) {

       case 0x0:  PC = addr ;              break ;   // BRA Branch Always
       case 0x1:  if(A>0)  { PC = addr ; } break ;   // BGT Branch if Greater
       case 0x2:  if(A<0)  { PC = addr ; } break ;   // BLT Branch if Less Than
       case 0x3:  if(A>=0) { PC = addr ; } break ;   // BGE Branch if Greater or Equal
       case 0x4:  if(A<=0) { PC = addr ; } break ;   // BLE Branch if Less Than or Equal
       case 0x5:  if(A!=0) { PC = addr ; } break ;   // BNE Branch if Not Equal to zero
       case 0x6:  if(A==0) { PC = addr ; } break ;   // BEQ Branch if Equal to zero
       case 0x7:  PC++ ; addr = M[PC] ;  PC = addr ; break ;  // 16-bit JMP
       case 0x8:  R[15]= R[15]-1 ; M[R[15]] = PC ; PC = addr ; break ;   // CALL (zero page) use R15 as Return Stack Pointer 
       case 0x9:  PC = M[R[15]]      ; R[15]= R[15]+1   ; break ;              // RET
       case 0xA:  PC = R[0] ; R[0]= M[PC] ; break ;  // JMP @R0   - useful for indexing and table look-up ( curious but useful pipeline effect here)
       case 0xB:  i =0 ; while (i < (63)) {          // GETCHAR - get '0' terminated character string into buffer
                  while (!Serial.available());
                  char ch = Serial.read();
                  if (ch == '\r' || ch == '\n') break;
                  if (ch >= ' ' && ch <= '~') {  M[512 + i] = ch;  i++;  }  
                  }  
                  M[512+i] = '\n' ; M[512+1+i] =   0 ; // Terminate the buffer with zero
                  break ;
       case 0xC:  Serial.write(R[0]) ;  break ;                            // OUT  - output a character to the Serial port
       case 0xD:  break ;
       case 0xE:  Serial.print(count);  Serial.print("   "); Serial.println(micros()); break ;
       case 0xF:  break ; R[0] &= R[0] ;             // NOP   AND accumulator with itself
}    
    }

// Serial.println(micros());    
 
    count++ ;    // Increment the instruction count

//    if (R[0] >= 32767) {  Serial.print("  #= "); Serial.print(count) ;}
// Display the Output  
// Serial.print("  PC= "); Serial.print(PC) ;   Serial.print("  IR= "); Serial.print(IR,HEX) ; Serial.print("  AD= "); Serial.print(addr,HEX) ; Serial.print("  OP= "); Serial.print(op) ;      
// Serial.print("  PC= "); Serial.print(PC) ;  Serial.print("  IR= "); Serial.print(IR,HEX) ; //  Serial.print("  #= "); Serial.print(count) ; 
 
// Serial.print("  AC= "); Serial.print(R[0]) ;  Serial.print("  ch "); Serial.write(R[0]) ; Serial.print("  R1= "); Serial.print(R[1]) ;
// Serial.print("  R2= "); Serial.print(R[2]) ; Serial.print("  R3= ");  Serial.print(R[3]) ; Serial.print("  R4= ");  Serial.print(R[4]) ;  Serial.print("  R5= ");  Serial.print(R[5]) ;
// Serial.print("  R6= ");  Serial.print(R[6]) ; Serial.print("  R7= ");  Serial.print(R[7]) ;
// Serial.print("  R14= ");  Serial.println(R[14]) ;
// Serial.print("  PC= "); Serial.print(PC) ; Serial.print("  AC= "); Serial.print(R[0]) ; Serial.print("  ASCII= ") ; Serial.write(R[0]) ; Serial.println() ;
// Serial.print("  M[R15]= "); Serial.print(M[R[15]]) ; Serial.print("  R15= ");  Serial.println(R[15]) ;
// delay(10) ;    
}


 
  

  
 
