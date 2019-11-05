// A simple simulator for Suite-16 processor

// Hexadecimal number entry, converted to integer and printed out in decimal in fewer than 90 words of code 
// Nov 5th 2019

// Bugs fixed and tested
// Uses ADD R0, R0 for left shifting (doubling) the accumulator

// Add and Subtract Immediate instructions ADI and SBI added at 0x0Axx and 0x0Bxx
// IN moved to 0x0D00
// JP@ - Branch to the address held in the accumulator  added at 0x0E00



/* Suite-16 Instructions

Register OPS-
     0n        ---       --     Non-Register Ops
     1n        SET       Rn     Constant  (Set)         Rn = @(PC+1)
     2n        LD        Rn     (Load)                  AC = Rn
     3n        ST        Rn     (Store)                 Rn = AC
     4n        LD        @Rn    (Load Indirect)         AC = @Rn
     5n        ST        @Rn    (Store Indirect)        @Rn = AC
     6n        POP       @Rn    Pop  AC                 AC = @Rn  Rn = Rn - 1
     7n        PUSH      @Rn    Push AC                 @Rn = AC  Rn = Rn + 1 
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
     0A        ADI    Add 8-bit Immediate           Immediate = IR7:0
     0B        SBI    Subtract 8-bit Immediate      Immediate = IR7:0
     0C        OUT                                  putchar(AC, port = IR7:0
     0D        IN                                   AC = getchar(), port = IR7:0
     0E        JP@                                  BRA (R0)
     0F        NOP                                  AC &= AC
   
   */
 

 
#define MEMSIZE  1024
unsigned int M[MEMSIZE] = {

     // Hexadecimal number entry, converted to integer and printed out in decimal
 
     // 0x0000 -----------------------------GET-TEXT---------------------------------------- 

        0x0F00,     // NOP  Keep this location spare for inserting a branch
        0x1800,     // SET R8, 0x0300  Use R8 as a destination pointer to store the hex in RAM
        0x0300,     // 
        0x1000,     // SET R0, 0x003E   > Cursor
        
        0x003E,      
        0x0C00,     // PUT CHAR  put out > cursor      
        0x0D00,     // GET CHAR buffer  M(512 + 63 max)  This will pause execution until it gets some serial followed by newline
        0x1100,     // SET R1, 0x200     buffer start
        
        0x0200,
        0x4100,     // LD R0, @R1  get first character
        0x0C00,     // Output the character
        0xE100,     // INC R1 increment pointer
        
        0x0109,     // BNZ 0x09 loop around until you find 0 terminator
        0x003C,     // BRA GETHEX
        0x0F00,     // NOP
        0x0F00,     // NOP

     // 0x0010 ---------------------------PRINTNUM----------------------------------------- 
        
    //  Prints out a 5 digit decimal number to the terminal according to contents of R0
    //  Leading zeroes are not suppressed
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
        0x082C,     // CALL decimate
       
        0x1100,     // R1 = 1000
        0x03E8,
        0x082C,     // CALL decimate
        
        0x1100,     // R1 = 100
        0x0064,
        0x082C,     // CALL decimate
        
        0x1100,     // R1 = 10
        0x000A,
        0x082C,     // CALL decimate
        
                
     // 0x0020 ---------------------------------------------------------
                      
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
        0xB300,     // SUB R3 to restore accumulator
        0x1000,     // SET R0, CR
        
        0x000D,
        0x0C00,     // putchar R0, CR
        0x1000,     // Set R0, LF
        0x000A,
        
        0x0C00,     // putchar R0, LF
        0x0003,     // BRA START     
        0x0F00,     // NOP
        0x0F00,     // NOP
        
     // 0x002C ------------------------Decimate--------------------------------
      
        0xB100,     // SUB R1,     :Decimate 
        0x0230,     // BLT 0x30    
        0xE200,     // INC R2
        0x002C,     // BRA 0x02C

     // 0x0030 ---------------------------------------------------------    

        0x3400,     // Store R0 in R4   
        0x2200,     // MOV R2, R0,  
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
         
        0x2400,     // Get R0 back from R4                 
        0xA100,     // ADD R1 adds DEC value to restore R0       
        0x1200,     // SET R2,0    Reset R2
        0x0000,

        0x0900,     // RET
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        // 0x003C -----------------------------GETHEX----------------------------

        // Accepts a hexadecimal number up to FFFF from terminal input buffer
        // converts it to an integer and puts it into register R0
        // It can then be printed out as a decimal using PRINTNUM - for checking integrity
        // R1 is the pointer in the text buffer - which starts at 0x0200
        // R4 is used as a temporary store for the character in the accumulator R0
        // R5 is used in the "Times 16" routine
        // R7 is used to accumulate the powers of 16 when forming the integer in R0
        
        
        0x1100,     // SET R1, 0x0200    text buffer start
        0x0200,
        0x1700,     // Don't forget to clear R7 
        0x0000, 

     // 0x0040--------------------------------------------------------------------------------------  
        
        0x4100,     // LD AC, @R1  get first character from buffer  :Getchar
        0x3400,     // Store R0 in R4
        0xE100,     // INC R1
        0x4100,     // LD AC, @R1  get next character - and test to see if it's a number or hex digit or space newline etc
     
        0x0B30,     // Subtract 0x30 Is it bigger than 0x30?
        0x0250,     // BLT 0x50 Quit  No - so must be a space or newline etc
        0x0B17,     // SBI 0x17  is it bigger than 0x47 ascii for "F" ?
        0x0350,     // BGT 0x50 Quit Not a hexadecimal digit
         
        0x0853,     // CALL 0x0053 Restore, Test and Modify R0
        0xA700,     // Add in the accumulating total from R7 - ready to multiply
        0xA000,     // ADD R0, R0  Double R0  2X
        0xA000,     // ADD R0, R0  Double R0  4X
        
        0xA000,     // ADD R0, R0  Double R0  8X
        0xA000,     // ADD R0, R0  Double R0  16X
        0x3700,     // Store R0 in R7   R7 is the accumulating total of all the digits multiplied by powers of 16
        0x0040,     // BRA 0x0040       Get the next digit

     // 0x0050-------------------------------------------------------------------------------------- 
         
        0x0853,     // CALL 0x0053      Restore, Test and modify R0
        0xA700,     // Add the accumulated sum from R7 - integer decimal number is now in R0   
        0x0060,     // BRA 0x60 Store digit  
       
     // 0x0053---------------------------------TEST R0 & MODIFY--------------------------------------

        // If R0 = 0-9 subtract 0x30 to form a number 0-9
        // If R0 = A-F subtract 0x37 to form a number 10-15

        0x2400,     // Get R0 back from R4   - we now have a hex character in the range 0-F and need to convert it to a value 0-15
        0x0B40,     // Subtract 0x40 Is it bigger than 0x40? Then subtract 0x37 else subtract 0x30
        0x0258,     // BLT Not A-F so subtract 30 and return
        0x0A09,     // ADI 0x09 (restores and corrects R0 to correct numerical value)
        0x005A,     // BRA  Return
        
        0x2400,     // LD  R0, R4   Get the character back in R0 - we know it's 0-9
        0x0B30,     // Subtract 0x30
        0x0900,     // RET
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

       // 0x0060 -------------------------------------------------------------------------------------   
        
        0x5800,     // ST R0, @R8
        0x4800,     // LD R0, @R8   Readback to confirm
        0xE800,     // INC R8 
        0x0070,     // BRA PRINTHEX - print it out in hex

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
        
        
     

     // 0x0070 ---------------------------PRINTHEX----------------------------------------- 
        
    //  Prints out a 4 digit hexadecimal number to the terminal according to contents of R0
    //  Leading zeroes are not suppressed
    //  R1 = Heximation Value
    //  R2 = digit 
    //  R3 = 0x30
    //  R4 = temporary storage for accumulator (decimated value)
    //  R6 = temporary store for output character
        
        0x1200,     // SET R2, 0x0000
        0x0000,
        0x1300,     // SET R3, 0x0030
        0x0030,
                   
        0x1100,     // R1 = 4096
        0x1000,
        0x088C,     // CALL heximate
        
        0x1100,     // R1 = 256
        0x0100,
        0x088C,     // CALL hexcimate
        
        0x1100,     // R1 = 16
        0x0010,
        0x088C,     // CALL heximate

        0x0A30,     // ADI 0x30 to make a number
        0x3600,     // Store in R6
        0x0B3A,     // SBI  0x3A  - is it bigger than ascii 9

     // 0x0080 ---------------------------------------------------------
        
        0x0284,     // BLT 0x84 -  Print decimal digit
        0x0A41,     // ADDI 0x41 - make it a hex digit 
        0x0C00,     // putchar R0
        0x0086,     // BRA CRLF

        0x2600,     // LD from R6
        0x0C00,     // putchar R0
        0x1000,     // SET R0, CR
        0x000D,
        
        0x0C00,     // putchar R0, CR
        0x0B03,     // SBI 0x03 Set R0, LF   
        0x0C00,     // putchar R0, LF
        0x0003,     // BRA START     
        
        
     // 0x008C ------------------------Heximate--------------------------------
      
        0xB100,     // SUB R1,     :Heximate 
        0x0290,     // BLT 0x90    
        0xE200,     // INC R2
        0x008C,     // BRA 0x08C

     // 0x0090 ---------------------------------------------------------    

        0x3400,     // Store R0 in R4   temporary store the remainder
        0x2200,     // MOV R2, R0  get the count from R2
        0x0A30,     // ADI 0x30 to make a number
        0x3600,     // ST R0, R6  - temporary save to R6
        
        0x0B3A,     // SBI  0x3A  - is it bigger than ascii 9
        0x0299,     // BLT 0x99 Print decimal digit
        0x0A41,     // ADI 0x41 - make it a hex digit 
        0x0C00,     // putchar R0

        0x009B,     // BRA 0x9B   Restore R0
        0x2600,     // Get R0 back from R6
        0x0C00,     // putchar R0 Print it as a decimal digit
        0x2400,     // Get R0 back from R4

        0xA100,     // ADD R1 adds DEC value to restore R0       
        0x1200,     // SET R2,0    Reset R2
        0x0000,
        0x0900,     // RET
             
       
       
          
     // 0x0080 ---------------------------GETNUM------------------------------------------ 
          
        0x0F00,     // NOP
        0x1000,     // SET R0, 0x003E   > Cursor
        0x003E,      
        0x0C00,     // PUT CHAR  put out > cursor      
              
        0x0D00,     // GET CHAR buffer  M(512 + 63 max)  This will pause execution until it gets some serial followed by newline
        0x1100,     // SET R1, 0x200     buffer start
        0x0200,
        0x4100,     // LD AC, @R1  get first character
        
        0x0C00,     // Output the character
        0xE100,     // INC R1 increment pointer
        0x0187,     // BNZ 0x87 loop around until you find 0 terminator
        0x003C,     // BRA GETHEX
 //       0x0F00,     // NOP
       
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

     // 0x0090 --------------------------------NUMBER------------------------------------- 

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
        0xB300,     // Subtract R3  Is it bigger than 0x30?
        0x02AA,     // BLT Not a Number
        
        0xB200,     // Subtract R2  0x0A
        0x03AA,     // BGE Not a Number
        0x2400,     // Get original character  R0 back from R4
        0xB300,     // Subtract R3  to form a digit


       // 0x00A0 --------------------------------------------------------------------- 
              
        0xA700,     // get the accumulating total  from R7 - ready to multiply
        0x3500,     // Store R0 in R5  Digit is now in R5
        0xA500,     // ADD R5 to R0  R0 = 2 * digit  "Times 10"
        0x3600,     // Store R0 in R6 2 *
       
        0xA600,     // ADD R6   4 X
        0xA600,     // ADD R6   6 X        
        0xA600,     // ADD R6   8 X 
        0xA600,     // ADD R6   10 X 
        
        0x3700,     // Store R0 in R7   R7 is the accumulating total of all the digits multiplied by powers of 10        
        0x0096,     // BRA 0x0096 Get the next digit
        0x0F00,     // Not a number  address = 0xAA
        0x2400,     // Get last digit  R0 back from R4
               
        0xB300,     // Subtract R3
        0xA700,     // Add the accumulated sum from R7 - decimal number is now it R0
        0x1700,     // Don't forget to clear R7
        0x0000,

        // 0x00B0 --------------------------------------------------------------------- 
       
        0x0010,      // BRA 0x0010  PRINTNUM_2
        0x0000,      // BRA START
              
        
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
       case 0x9:  PC = M[R[15]]      ; R[15]= R[15]+1   ; break ;        // RET
       case 0xA:  R[0]= R[0] + addr ; break ;        // ADI add the immediate 8-bit contained in the address field
       case 0xB:  R[0]= R[0] - addr ; break ;        // SBI subtract the immediate 8-bit contained in the address field
       case 0xC:  Serial.write(R[0]) ;  break ;                            // OUT  - output a character to the Serial port
       case 0xD:  i =0 ; while (i < (63)) {          // GETCHAR - get '0' terminated character string into buffer
                  while (!Serial.available());
                  char ch = Serial.read();
                  if (ch == '\r' || ch == '\n') break;
                  if (ch >= ' ' && ch <= '~') {  M[512 + i] = ch;  i++;  }  
                  }  
                  M[512+i] = '\n' ; M[512+1+i] =   0 ; // Terminate the buffer with zero
                  break ;
       case 0xE:  PC = R[0] ; R[0]= M[PC] ; break ;  // JMP @R0   - useful for indexing and table look-up ( curious but useful pipeline effect here)
       case 0xF:  break ; R[0] &= R[0] ;             // NOP   AND accumulator with itself
}    
    }

// Serial.println(micros());    
 
//    count++ ;    // Increment the instruction count

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
// delay(100) ;    
}


 
  

  
 
