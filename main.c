#include <stdio.h> 
#include <stdlib.h>

// 8-bit registers
unsigned char A, B, C, D, E;

// program counter
unsigned char* pc;

typedef struct ConditionCodes {
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
} ConditionCodes;

typedef struct State8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    uint8_t int_enable;
} State8080;

/*
    Disassemble 8080 code: 
    1. Read the code into a buffer
    2. Get a pointer to the beginning of the buffer
    3. Use the byte at the pointer to determine the opcode
    4. Print out the name of the opcode using the bytes after the opcode as data, if applicable
    5. Advance the pointer the number of bytes used by that instruction (1, 2, or 3 bytes)
    6. If not at the end of the buffer, go to step 3
*/

/* 
 * codebuffer is a valid pointer to 8080 assembly code
 pc is the current offset into the code

 returns the number of bytes of the op
 */

void UnimplementedInstruction(State8080* state)
{
    // pc will have advanced one - undo 
    printf("Error: Unimplemented instruction\n");
    exit(1);
}

int Emulate8080Op(State8080* state){
    unsigned char *opcode = &state->memory[state->pc];
    
    switch(*opcode)
    {
        case 0x00:      // NOP
            break;    
        case 0x01:      // LXI  B,word
            state->c = opcode[1];
            state->b = opcode[2];
            state->pc += 2; // advance 2 additional bytes
            break;    
        case 0x02: UnimplementedInstruction(state); break;    
        case 0x03: UnimplementedInstruction(state); break;    
        case 0x04: UnimplementedInstruction(state); break;    
        /*....*/    
        case 0x41:      // MOV  B,C
            state->b = state->c;
            break;
        case 0x42:      // MOV  B,D
            state->b = state->d;
            break;
        case 0x43:      // MOV  B,E
            state->b = state->e;
            break;
    }
    state->pc+=1; // for next
}

int Disassemble8080Op(unsigned char *codebuffer, int pc)
{
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf("%04x ", pc);
    switch(*code)
    {
    case 0x00: 
        printf("NOP"); 
        break;
    case 0x01:
        printf("LXI     B,#$%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x02:
        printf("STAX    B");
        break;
    case 0x03:
        printf("INX     B");
        break;
    case 0x04:
        printf("INR     B");
        opbytes = 3;
        break;
    case 0x05:
        printf("DCR     B");
        break;
    case 0x06:
        printf("MVI     B, #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x07:
        printf("RLC");
        break;
    case 0x08:
        printf("NOP");
        break;
    case 0x09:
        printf("DAD     B"); 
        break;
    case 0x0a:
        printf("LDAX    B"); 
        break;
    case 0x0b: 
        printf("DCX     B");
        break;
    case 0x0c: 
        printf("INCR    C");
        break;
    case 0x0d:
        printf("DCR     C");
        break;
    case 0x0e: 
        printf("MVI     C, #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x0f: 
        printf("RRC");
        break;
    case 0x3e:
        printf("MVI     A,#0x%02x", code[1]); 
        opbytes = 2;
        break;
    case 0xc3:
        printf("JMP     $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    default: 
        printf("Unrecognized instruction");
        break;
    }

    printf("\n");

    return opbytes;
}

 int main(int argc, char **argv)
 {
     FILE *f = fopen(argv[1], "rb");
     if (f==NULL)
     {
         printf("error: Couldn't open %s\n", argv[1]);
         exit(1);
     }
    
     // seek end of file
     fseek(f, 0L, SEEK_END); 
     // see how far you can read (file size)
     int fsize = ftell(f);   
     // seek back to beginning 
     fseek(f, 0L, SEEK_SET); 

     unsigned char *buffer = malloc(fsize);
     fread(buffer, fsize, 1, f); 
     fclose(f);

     int pc = 0; 
     while (pc < fsize)
     {
         // Read the program counter, decode instruction, advance by opcode bytes. 
         pc += Disassemble8080Op(buffer, pc); 
     }

    return 0;

 }