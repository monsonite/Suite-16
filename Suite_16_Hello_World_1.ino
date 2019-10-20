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
     Cn        CPR       Rn     (Compare)               R13 = AC - Rn
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
     0A                         (Unassigned)
     0B                         (Unassigned)
     0C                         (Unassigned)  
     0D                         (Unassigned)
     0E                         (Unassigned)
     0F                         (Unassigned)
   
   */
 

 
#define MEMSIZE  1024
int M[MEMSIZE] = {
         
             
        0x1100,     // SET R1, 0x07
        0x0007,
        0x4100,     // LD AC, @R1
        0x0C00,     // Output the character
        0xE100,     // INC R1
        0x0102,     // BNZ 02
        0x0000,     // BRA 00
        'H',
        'e',
        'l',
        'l',
        'o',
        ' ',
        'W',
        'o',
        'r',
        'l',
        'd',
        '!',
        '\n',
        0x0000       
} ;

int A = 0 ; int PC = 0  ;   
unsigned int IR = 0  ;
int R[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;  // Zero the Registers

void setup() {  Serial.begin(115200); }

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
    case 0xC:   R[n] != R[n]        ; break ;  /* INV */
    case 0xD:   R[n] = R[n]-1       ; break ;  /* DEC */
    case 0xE:   R[n] = R[n]+1       ; break ;  /* INC */
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
// Serial.print("  AC= "); Serial.print(R[0]) ; Serial.print("  R1= "); Serial.print(R[1]) ;   Serial.print("  R2= "); Serial.print(R[2]) ;     Serial.print("  R3= "); Serial.println(R[3]) ;  
// Serial.print("  PC= "); Serial.print(PC) ; Serial.print("  AC= "); Serial.print(R[0]) ; Serial.print("  ASCII= ") ; Serial.write(R[0]) ; Serial.println() ;
// delay(1000) ;    
}



  

  
 
