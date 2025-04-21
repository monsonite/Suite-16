// A simple simulator for Suite-16 processor

// Hexadecimal number entry, converted to integer and printed out in decimal in fewer than 90 words of code 
// Nov 5th 2019

// Updated April 20th 2025

// Registers R0 to R15 are now located in main memory in the first 16 words address 0x0000 to 0x000F

// Bugs fixed and tested
// Uses ADD R0, R0 for left shifting (doubling) the accumulator

// Instruction $0F yet to be developed as a PDP-8 like OPR  instruction - for clearing accumulator, left and right shifts, set and clear carry etc.

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
     0C        OUT                                  putchar(AC), port = IR7:0
     0D        IN                                   AC = getchar(), port = IR7:0
     0E        JP@                                  BRA (R0)
     0F        NOP                                  AC &= AC
   
   */
 
#define MEMSIZE  1024
unsigned int M[MEMSIZE] = {

     // Hexadecimal number entry, converted to integer and printed out in decimal

     // 0x0000 -------------------------------------------------------------------------------------   
        
        0x0000,     // R0
        0x0000,     // R1
        0x0000,     // R2
        0x0000,     // R3

        0x0000,     // R4
        0x0000,     // R5
        0x0000,     // R6
        0x0000,     // R7

        0x0000,     // R8
        0x0000,     // R9
        0x0000,     // R10
        0x0000,     // R11

        0x0000,     // R12
        0x0000,     // R13
        0x0000,     // R14
        0x0200,     // R15
    
     // 0x0010 ---------------------------------------------------------

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
        0x0F00,     // NOP
       
     // 0x0020 -----------------------------GET-TEXT---------------------------------------- 

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
        0x0127,     // BNZ 0x27 loop around until you find 0 terminator
        0x005C,     // BRA GETHEX
                
        0x0F00,    // NOP
        0x0F00,    // NOP
        0x0F00,    // NOP
        0x0F00,    // NOP

        // 0x0030 ---------------------------PRINTNUM----------------------------------------- 
        
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
        0x084C,     // CALL decimate
       
        0x1100,     // R1 = 1000
        0x03E8,
        0x084C,     // CALL decimate
        
        0x1100,     // R1 = 100
        0x0064,
        0x084C,     // CALL decimate
        
        0x1100,     // R1 = 10
        0x000A,
        0x084C,     // CALL decimate
                     
     // 0x0040 ---------------------------------------------------------
                      
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
        0xB300,     // SUB R3 to restore accumulator
        0x1000,     // SET R0, CR
        
        0x000D,
        0x0C00,     // putchar R0 CR
        0x1000,     // Set R0, LF
        0x000A,
        
        0x0C00,     // putchar R0 LF
        0x0020,     // BRA START     
        0x0F00,     // NOP
        0x0F00,     // NOP
        
     // 0x004C ------------------------Decimate--------------------------------
      
        0xB100,     // SUB R1,     :Decimate 
        0x0250,     // BLT 0x50    
        0xE200,     // INC R2
        0x004C,     // BRA 0x04C

     // 0x0050 ---------------------------------------------------------    

        0x3400,     // Store R0 in R4   
        0x2200,     // MOV R2, R0,  
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
         
        0x2400,     // Get R0 back from R4                 
        0xA100,     // ADD R1 adds DEC value to restore R0       
        0x1200,     // SET R2,0    Reset R2
        0x0000,

        0x0920,     // RET
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP

        // 0x005C -----------------------------GETHEX----------------------------

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

     // 0x0060--------------------------------------------------------------------------------------  
        
        0x4100,     // LD AC, @R1  get first character from buffer  :Getchar
        0x3400,     // Store R0 in R4
        0xE100,     // INC R1
        0x4100,     // LD AC, @R1  get next character - and test to see if it's a number or hex digit or space newline etc
     
        0x0B30,     // Subtract 0x30 Is it bigger than 0x30?
        0x0270,     // BLT 0x70 Quit  No - so must be a space or newline etc
        0x0B17,     // SBI 0x17  is it bigger than 0x47 ascii for "F" ?
        0x0370,     // BGT 0x70 Quit Not a hexadecimal digit
         
        0x0873,     // CALL 0x0073 Restore, Test and Modify R0
        0xA700,     // Add in the accumulating total from R7 - ready to multiply
        0xA000,     // ADD R0, R0  Double R0  2X
        0xA000,     // ADD R0, R0  Double R0  4X
        
        0xA000,     // ADD R0, R0  Double R0  8X
        0xA000,     // ADD R0, R0  Double R0  16X
        0x3700,     // Store R0 in R7   R7 is the accumulating total of all the digits multiplied by powers of 16
        0x0060,     // BRA 0x0060       Get the next digit

     // 0x0070-------------------------------------------------------------------------------------- 
         
        0x0873,     // CALL 0x0073      Restore, Test and modify R0
        0xA700,     // Add the accumulated sum from R7 - integer decimal number is now in R0   
        0x0030,     // BRA 0x0030  Print it in decimal     
       
     // 0x0073---------------------------------TEST R0 & MODIFY--------------------------------------

        // If R0 = 0-9 subtract 0x30 to form a number 0-9
        // If R0 = A-F subtract 0x37 to form a number 10-15

        0x2400,     // Get R0 back from R4   - we now have a hex character in the range 0-F and need to convert it to a value 0-15
        0x0B40,     // Subtract 0x40 Is it bigger than 0x40? Then subtract 0x37 else subtract 0x30
        0x0278,     // BLT Not A-F so subtract 30 and return
        0x0A09,     // ADI 0x09 (restores and corrects R0 to correct numerical value)
        0x007A,     // BRA  Return
        
        0x2400,     // LD  R0, R4   Get the character back in R0 - we know it's 0-9
        0x0B30,     // Subtract 0x30
        0x0920,     // Return
        0x0F00,     // NOP

        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
        0x0F00,     // NOP
   
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
        0x005C,     // BRA GETHEX
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
        0x0020,      // BRA START              
        
} ;

unsigned long count = 0 ; int A = 0 ; int PC = 0x20  ;   int  i=0 ; int ir =0 ; int fn = 0 ; int reg = 0 ; int data = 0 ; unsigned int IR = 0  ;
void setup() {  Serial.begin(115200); }
void loop() {  fetch() ; execute() ;  } 
void fetch() { Serial.println(); IR = M[PC] ; if (PC < 4096) { Serial.print("0");} if (PC < 256) { Serial.print("0");} if (PC < 16) { Serial.print("0");} 
    Serial.print(PC, HEX); Serial.print("  "); 
   if (M[PC] < 4096) { Serial.print("0");} if (M[PC] < 256) { Serial.print("0");} if (M[PC] < 16) { Serial.print("0");}  Serial.print(M[PC], HEX); 
   fn = ((M[PC] & 0xFF00)/ 256);
   reg = ((M[PC] & 0xFF00)/ 4096);
Serial.print("  ");  PC ++ ;  PC &= (MEMSIZE-1) ; } 
void execute(){  
  unsigned int op = IR >> 12 ;   unsigned int n  = (IR & 0xf00) >> 8 ; // op = IR 15:12      Opcode  // n = IR 11:8        Register or Condition
  int addr = IR & 0x0ff ;                 // addr = IR 7:0      Address or Displacement
      switch (op) {                               // Opcode Execute 
    case 0x0:   break ; 
    case 0x1:   M[n]= M[PC] ; PC++  ; break ;  /* SET */      
    case 0x2:   M[0] = M[n]         ; break ;  /* LD */
    case 0x3:   M[n] = M[0]         ; break ;  /* ST */
    case 0x4:   M[0] = M[M[n]]      ; break ;  /* LD@ */
    case 0x5:   M[M[n]] = M[0]      ; break ;  /* ST@ */
    case 0x6:   M[0] = M[M[n]]      ; M[n]= M[n]+1   ;  break ; /* POP with post-increment of pointer Rn  */
    case 0x7:   M[n]= M[n]-1        ; M[M[n]] = M[0] ;  break ; /* PSH with pre-decrement of pointer Rn */  
    case 0x8:   M[0] &= M[n]        ; break ;  /* AND */
    case 0x9:   M[0] |= M[n]        ; break ;  /* OR  */
    case 0xA:   M[0] += M[n]        ; break ;  /* ADD */     
    case 0xB:   M[0] -= M[n]        ; break ;  /* SUB */
    case 0xC:   M[n] = ~M[n]        ; break ;  /* INV */
    case 0xD:   M[n] =  M[n]-1      ; break ;  /* DEC */
    case 0xE:   M[n] =  M[n]+1      ; break ;  /* INC */
    case 0xF:   M[0] ^= M[n]        ; break ;  /* XOR */   }
    A = M[0] ;          // Conditional Branches and I/O Group 
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
       case 0x8:  M[15]= M[15]-1 ; M[M[15]] = PC ; PC = addr ; break ;   // CALL (zero page) use R15 as Return Stack Pointer 
       case 0x9:  PC = M[M[15]]      ; M[15]= M[15]+1   ; break ;        // RET
       case 0xA:  M[0]= M[0] + addr ; break ;        // ADI add the immediate 8-bit contained in the address field
       case 0xB:  M[0]= M[0] - addr ; break ;        // SBI subtract the immediate 8-bit contained in the address field
       case 0xC:  Serial.write(M[0]) ; if( 0<=(M[0]<= 126)){ Serial.print("  ");} break ;                            // OUT  - output a character to the Serial port
       case 0xD:  i =0 ; while (i < (63)) {          // GETCHAR - get '0' terminated character string into buffer
                  while (!Serial.available()); char ch = Serial.read(); if (ch == '\r' || ch == '\n') break;
                  if (ch >= ' ' && ch <= '~') {  M[512 + i] = ch;  i++;  } } M[512+i] = '\n' ; M[512+1+i] =   0 ;  break ; // Terminate the buffer with zero            
       case 0xE:  PC = M[0] ; M[0]= M[PC] ; break ;  // JMP @R0   - useful for indexing and table look-up ( curious but useful pipeline effect here)
       case 0xF:  break ; M[0] &= M[0] ; // NOP   AND accumulator with itself    
       }  


//*************************************************************************************************************************
// Debugger Screen
//*************************************************************************************************************************


    // for(int j = 0; j <= 15; j++)
   //  {ir = ((M[j] & 0xFF0000)/65536); fn = ((M[j] & 0xF00000)/1048576); reg = ((M[j] & 0x0F0000)/65536); 
     
      print_op(fn,reg) ;
     
     // print_mem(data) ; 
    
     

     delay(100) ;

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



/*
 // 16-bit word as a bit pattern

      if(x>=32768)  {Serial.print("1"); x =  x- 32768;} 
    delay(50) ;
   }  
  void loop() { execute() ;  }
*/
// ****************************************************************************************************************

void print_row(int x, int y) {                  // Output a 16-bit number in row of 1's and 0's
      if(x>=32768)  {Serial.print("1"); x =  x- 32768;}  else {Serial.print("0");}
      if(x>=16384)  {Serial.print("1"); x =  x- 16384;}  else {Serial.print("0");}
      if(x>=8192)   {Serial.print("1"); x =  x- 8192;}   else {Serial.print("0");}
      if(x>=4096)   {Serial.print("1  "); x =  x- 4096;} else {Serial.print("0  ");}
      if(x>=2048)   {Serial.print("1"); x =  x- 2048;}   else {Serial.print("0");}
      if(x>=1024)   {Serial.print("1"); x =  x- 1024;}   else {Serial.print("0");}
      if(x>=512)    {Serial.print("1"); x =  x- 512;}    else {Serial.print("0");}
      if(x>=256)    {Serial.print("1  "); x =  x- 256;}  else {Serial.print("0  ");}
      if(x>=128)    {Serial.print("1"); x =  x- 128;}    else {Serial.print("0");}
      if(x>=64)     {Serial.print("1"); x =  x- 64;}     else {Serial.print("0");}
      if(x>=32)     {Serial.print("1"); x =  x- 32;}     else {Serial.print("0");}
      if(x>=16)     {Serial.print("1  "); x =  x- 16;}   else {Serial.print("0  ");}
      if(x>=8)      {Serial.print("1"); x =  x- 8;}      else {Serial.print("0");}
      if(x>=4)      {Serial.print("1"); x =  x- 4;}      else {Serial.print("0");}
      if(x>=2)      {Serial.print("1"); x =  x- 2;}      else {Serial.print("0");}
      if(x>=1)      {Serial.print("1  "); x =  x- 1;}    else {Serial.print("0  ");}

      
      }

      /*

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
     0C        OUT                                  putchar(AC), port = IR7:0
     0D        IN                                   AC = getchar(), port = IR7:0
     0E        JP@                                  BRA (R0)
     0F        NOP                                  AC &= AC
   
   */

    

    void print_op(int x, int y){  // Print the opcode at PC location x and the register y
    switch (x) {
    case 0x00:  Serial.print(" ---  ");  break ;       // ---
    case 0x01:  Serial.print(" SET  ");  break ;       // SET Relative Jump 
    case 0x02:  Serial.print(" LD   ");  break ;       // LD
    case 0x03:  Serial.print(" ST   ");  break ;       // ST
    case 0x04:  Serial.print(" LD@  ");  break ;       // LD@
    case 0x05:  Serial.print(" ST@  ");  break ;       // ST@
    case 0x06:  Serial.print(" POP  ");  break ;       // POP
    case 0x07:  Serial.print(" PSH  ");  break ;       // PSH

    case 0x08:  Serial.print(" AND  ");  break ;       // AND
    case 0x09:  Serial.print(" OR   ");  break ;       // OR
    case 0x0A:  Serial.print(" ADD  ");  break ;       // ADD
    case 0x0B:  Serial.print(" SUB  ");  break ;       // SUB
    case 0x0C:  Serial.print(" INV  ");  break ;       // INV
    case 0x0D:  Serial.print(" DEC  ");  break ;       // DEC
    case 0x0E:  Serial.print(" INC  ");  break ;       // INC
    case 0x0F:  Serial.print(" XOR  ");  break ;       // XOR
    }

switch (y) {

 case 0x00:  Serial.print("R0  ");  break ;
 case 0x01:  Serial.print("R1  ");  break ;
 case 0x02:  Serial.print("R2  ");  break ;
 case 0x03:  Serial.print("R3  ");  break ;
 case 0x04:  Serial.print("R4  ");  break ;
 case 0x05:  Serial.print("R5  ");  break ;
 case 0x06:  Serial.print("R6  ");  break ;
 case 0x07:  Serial.print("R7  ");  break ;
 case 0x08:  Serial.print("R8  ");  break ;
 case 0x09:  Serial.print("R9  ");  break ;
 case 0x10:  Serial.print("R10 ");  break ;
 case 0x11:  Serial.print("R11 ");  break ;
 case 0x12:  Serial.print("R12 ");  break ;
 case 0x13:  Serial.print("R13 ");  break ;
 case 0x14:  Serial.print("R14 ");  break ;
 case 0x15:  Serial.print("R15 ");  break ;



}

    }

    void print_mem(int x) {   // Print the 3-byte word stored at location x
   // if (x < 1048576) { Serial.print("0");}
   // if (x < 65536) { Serial.print("0");} 
    if (x < 4096) { Serial.print("0");}
    if (x < 256) { Serial.print("0");}
    if (x < 16) { Serial.print("0");} 
    Serial.print(x, HEX); Serial.print("  "); 

    
    }

 
  /* 

    void print_inst(int x, int y) {  // Output a 8-bit instruction as a bit pattern
      if (y<16){Serial.print("0");} Serial.print(y, HEX); Serial.print("   ");
      int inst = x ;

      if(x>=128)    {Serial.print("1"); x =  x- 128;}    else {Serial.print("0");}
      if(x>=64)     {Serial.print("1"); x =  x- 64;}     else {Serial.print("0");}
      if(x>=32)     {Serial.print("1"); x =  x- 32;}     else {Serial.print("0");}
      if(x>=16)     {Serial.print("1"); x =  x- 16;}     else {Serial.print("0");}
      if(x>=8)      {Serial.print("1"); x =  x- 8;}      else {Serial.print("0");}
      if(x>=4)      {Serial.print("1"); x =  x- 4;}      else {Serial.print("0");}
      if(x>=2)      {Serial.print("1"); x =  x- 2;}      else {Serial.print("0");}
      if(x>=1)      {Serial.print("1 "); x =  x- 1;}     else {Serial.print("0 ");}
      if (inst<16){Serial.print("0");} Serial.print(inst, HEX); Serial.print("   "); }

*/
  
 
