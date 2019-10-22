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
     00        JMP    16-bit                        Target = @(PC+1)
     01        BGT    AC>0                          Target = IR7:0
     02        BLT    AC<0                          Target = IR7:0
     03        BNE    AC!=0                         Target = IR7:0
     04        BEQ    AC=0                          Target = IR7:0
     05        BGE    AC>=0                         Target = IR7:0
     06        BLE    AC<=0                         Target = IR7:0
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
 

 
#define MEMSIZE  1024
int M[MEMSIZE] = {
         
        0x1000,     // SET R0, 0x0000
        0x0000,      
        0x1100,     // SET R1, 0x2710    10,000
        0x2710,

        
        0xB100,     // SUB R0, R1    :10K  addr = 0x04
        0x0208,     // BLT 0x08    END10K
        0xE200,     // INC R2
        0x0004,     // BRA 0004
        0x3400,     // Store R0 in R4       addr = 0x08          
        0x2200,     // END10K    MOV R2, R0 
        0x1300,     // SET R3, 0x30
        0x0030,
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
        0x2400,     // Get R0 back from R4  
        0xA100,     // ADD R1 adds 10,000 to restore R0
        0x1100,
        0x03E8,     // R1 = 1000
        0x1200,     // SET R2,0    Reset R2
        0x0000,

        0xB100,     // SUB R0, R1    :1K  address 0x14
        0x0218,     // BLT 0x18   END1K
        0xE200,     // INC R2
        0x0014,     // BRA 0x0014
        0x3400,     // Store R0 in R4      addr = 0x18
        0x2200,     // END1K   MOV R2, R0  
        0x1300,     // SET R3, 0x30
        0x0030,
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
        0x2400,     // Get R0 back from R4  
        0xA100,     // ADD R1 adds 1000 to restore R0
        0x1100,
        0x0064,     // R1 = 100
        0x1200,     // SET R2,0    Reset R2
        0x0000,

        0xB100,     // SUB AC, R1    :100 addresss 0x24
        0x0228,     // BLT 0x28   END100
        0xE200,     // INC R2
        0x0024,     // BRA 0x0024
        0x3400,     // Store R0 in R4       address = 0x28
        0x2200,     // END100   MOV R2, R0 
        0x1300,     // SET R3, 0x30
        0x0030,
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
        0x2400,     // Get R0 back from R4  
        0xA100,     // ADD R1 adds 100 to restore R0
        0x1100,
        0x000A,     // R1 = 10
        0x1200,     // SET R2,0    Reset R2
        0x0000,

        0xB100,     // SUB AC, R1    :10 addresss 0x34
        0x0238,     // BLT 0x38   END10
        0xE200,     // INC R2
        0x0034,     // BRA 0x0034
        0x3400,     // Store R0 in R4     addr = 0x38 
        0x2200,     // END10  MOV R2, R0   
        0x1300,     // SET R3, 0x30
        0x0030,
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0
        0x2400,     // Get R0 back from R4 
        0xA100,     // ADD R1 adds 10 to restore R0
        0x1100,
        0x0001,     // R1 = 1
        0x1200,     // SET R2,0    Reset R2
        0x0000,
        
        0x1300,     // SET R3, 0x30  address = 0x44
        0x0030,
        0xA300,     // ADD R0, R3 to make a number
        0x0C00,     // putchar R0 
        0xB300,     // SUB R3 to restore accumulator

        0x1000,     // SET R0, CR
        0x000D,
        0x0C00,     // putchar R0 CR
        0x1000,     // Set R0, LF
        0x000A,
        0x0C00,     // putchar R0 LF
       
        0x1200,     // SET R2,0
        0x0000, 
        0xE500,     // INC R5
        0x2500,     // LD R0, R5
        0x0002      // BRA 0002
       
        
} ;

int A = 0 ; int PC = 0  ;   
unsigned int IR = 0  ;
int R[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;  // Zero the Registers

void setup() {  Serial.begin(921600); }

void loop() {  fetch() ; execute() ;  }
 
void fetch() {  IR = M[PC] ;    PC ++ ;  PC &= (MEMSIZE-1) ; } 
 
void execute()
{
  
/* Instruction Decode */
  
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
    case 0x6:   R[0] = M[R[n]]      ; R[n]= R[n]-1 ; break ; /* POP */
    case 0x7:   M[R[n]] = R[0]      ; R[n]= R[n]+1 ; break ; /* PSH */  
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
       case 0x7:  break ;
       case 0x8:  break ;
       case 0x9:  break ;  
       case 0xA:  break ;
       case 0xB:  R[0] =  Serial.read();  R[0] = R[0] && 0x00FF ;  break ; // IN   - Get an ascii character forn Serial and mask it with 0xFF
       case 0xC:  Serial.write(R[0]) ;  break ;                            // OUT  - output a character to the Serial port
       case 0xD:  break ;
       case 0xE:  break ;
       case 0xF:  break ; R[0] &= R[0] ;             // NOP   AND accumulator with itself
}    
    }
// Display the Output  
// Serial.print("  PC= "); Serial.print(PC) ;   Serial.print("  IR= "); Serial.print(IR,HEX) ; Serial.print("  AD= "); Serial.print(addr,HEX) ; Serial.print("  OP= "); Serial.print(op) ;      
// Serial.print("  PC= "); Serial.print(PC) ;  Serial.print("  AC= "); Serial.print(R[0]) ; Serial.print("  R1= "); Serial.print(R[1]) ;   Serial.print("  R2= "); Serial.print(R[2]) ;     Serial.print("  R3= ");  Serial.print(R[3]) ; Serial.print("  R4= ");  Serial.println(R[4]) ;  
// Serial.print("  PC= "); Serial.print(PC) ; Serial.print("  AC= "); Serial.print(R[0]) ; Serial.print("  ASCII= ") ; Serial.write(R[0]) ; Serial.println() ;
// delay(1) ;    
}
