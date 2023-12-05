#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t memory[65535] = { 0 }; // memmory array
                                
uint16_t USP = 0xfdff, SSP = 0x2fff;
uint16_t regs[10] = { 
    /* R0 - R7*/
    0, 0, 0, 0, 0, 0, 0, 0,
    /* PC register */
    0x3000,
    /* PSR */
    0x8000  // starting at user mode 
}; // array contains registers


enum registers { R0 = 0, R1, R2, R3, R4, R5, R6, R7, PC, PSR};

enum opcodes { 
	BR = 0, ADD, LD, ST, JSR, AND, LDR, 
	STR, RTI, NOT, LDI, STI, JMP, invalid,
	LEA, TRAP
};

enum trapvect {
    GETC = 0x20,
    OUT,
    PUTS,
    IN,
    PUTSP,
    HALT
};

void br(uint16_t opcode);
void add(uint16_t opcode);
void ld(uint16_t opcode);	
void st(uint16_t opcode);
void jsr(uint16_t opcode);
void and(uint16_t opcode);	
void ldr(uint16_t opcode);
void str(uint16_t opcode);
void rti(uint16_t opcode);	
void not(uint16_t opcode);
void ldi(uint16_t opcode);
void sti(uint16_t opcode);	
void jmp(uint16_t opcode);
void lea(uint16_t opcode);	
void trap(uint16_t opcode);

uint16_t sext(uint16_t value, uint8_t size);
void setcc(uint16_t DR);

void print_registers()
{
    for (int i = 0; i < 10; ++i)
    {
        printf(" %x\n", regs[i]);
    }
}

int main()
{
    memory[0xfffe] = 0x8000;
    regs[R6] = USP;
	memory[regs[PC]] = 0x2000;
    memory[regs[PC] + 1] = 0xf025;

    uint16_t opcode; 
    uint16_t instruction;

    // expected R0 = 0xf025;


    while (memory[0xfffe] & 0x8000)
    {
        opcode = memory[regs[PC]];
        instruction = (opcode & 0xf000) >> 12;
        switch (instruction)
        {
        case BR:
            {
                puts("It is BR");
            }
            break;
        case ADD:	
            {
                add(opcode);
                ++regs[PC];
            }
            break;
        case LD:
            {
                ld(opcode);
                print_registers();
                ++regs[PC];
            }
            break;
        case ST:
            break;
        case JSR:
            break;
        case AND:
            {
                and(opcode);
                ++regs[PC];
            }
            break;
        case LDR:
            break;
        case STR:
            break;
        case RTI:
            break;
        case NOT:
            {
                not(opcode);
                ++regs[PC];
            }
            break;
        case LDI:
            break;
        case STI:
            break;
        case JMP:
            break;
        case invalid:
            puts("Invalid opcode");
            exit(1);
            break;
        case LEA:
            break;
        case TRAP:
            {
                uint16_t trapvect8 = opcode & 0x00ff;
                switch (trapvect8)
                {
                case HALT:
                    memory[0xfffe] &= 0x7fff;
                    puts("halted");
                    break;
                default:
                    break;       
                }
            }
            break;
        }
    }
	return 0;
}


void br(uint16_t opcode);

void add(uint16_t opcode)
{
    uint16_t DR = (opcode & 0x0e00) >> 9;
    uint16_t SR1 = (opcode & 0x01c0) >> 6;
    if ((opcode & 0x0020) == 0)
    {
        uint16_t SR2 = (opcode & 0x0007);
        regs[DR] = regs[SR1] + regs[SR2];
    }
    else
    {
        uint16_t imm5 = (opcode & 0x001f);
        regs[DR] = regs[SR1] + sext(imm5, 5);
    }
    setcc(DR);
}

void ld(uint16_t opcode)
{
    uint16_t DR = (opcode & 0x0e00) >> 9;
    uint16_t PCoffset9 = (opcode & 0x01ff);
    regs[DR] = memory[regs[PC] + 1 + sext(PCoffset9, 9)];
    setcc(DR);
}

void st(uint16_t opcode);
void jsr(uint16_t opcode);

void and(uint16_t opcode)
{
    uint16_t DR = (opcode & 0x0e00) >> 9;
    uint16_t SR1 = (opcode & 0x01c0) >> 6;
    if ((opcode & 0x0020) == 0)
    {
        uint16_t SR2 = (opcode & 0x0007);
        regs[DR] = regs[SR1] & regs[SR2];
    }
    else
    {
        uint16_t imm5 = (opcode & 0x001f);
        regs[DR] = regs[SR1] & sext(imm5, 5);
    }
    setcc(DR);

}

void ldr(uint16_t opcode);
void str(uint16_t opcode);
void rti(uint16_t opcode);	

void not(uint16_t opcode)
{
    uint16_t DR = (opcode & 0x0e00) >> 9;
    uint16_t SR1 = (opcode & 0x01c0) >> 6;
    regs[DR] = ~regs[SR1];
    setcc(DR);
}

void ldi(uint16_t opcode);
void sti(uint16_t opcode);	
void jmp(uint16_t opcode);
void lea(uint16_t opcode);	
void trap(uint16_t opcode);

uint16_t sext(uint16_t value, uint8_t size)
{
    --size;
    if (((value >> size) & 0x1) == 1)
    {
        return ((0xffff << size) << 1) + value;
    }
    return value;

}

void setcc(uint16_t DR)
{
    if (regs[DR] == 0)
    {
        regs[PSR] = (regs[PSR] & 0xfff8) + 0x0002; // N = 0, Z = 1, P = 0
    }
    else if ((regs[DR] & 0x8000) == 0) 
    {
        regs[PSR] = (regs[PSR] & 0xfff8) + 0x0001; // N = 0, Z = 0, P = 1
    } 
    else if ((regs[DR] & 0x8000) == 0x8000)
    {
        regs[PSR] = (regs[PSR] & 0xfff8) + 0x0004; // N = 1, Z = 0, P = 0
    }
}

