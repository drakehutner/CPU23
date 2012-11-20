/**
 * 
 *
 */
 
#ifndef __CPU_23_H__
#define __CPU_23_H__

#include <stdint.h> 

/** OpCode definition */
enum e_OpCode {
	CONSTANT = -3,
	COMMENT = -2,
	ERROR = -1,
	NOP = 0x00, LD  = 0x01, STR = 0x02, CPY = 0x03,
	SET = 0x04, RST = 0x05, PSH = 0x06, POP = 0x07,
	INC = 0x08, DEC = 0x09, ADD = 0x0A, SUB = 0x0B,
	LSL = 0x0C, LSR = 0x0D, AND = 0x0E, OR  = 0x0F,
	XOR = 0x10, NOT = 0x11, CMP = 0x12, TST = 0x13,
	JMP = 0x14, BRA = 0x15, CLL = 0x16, BRS = 0x17,
	RTS = 0x18, DSP = 0x19, EMW = 0x1A, EMR = 0x1B
};
typedef enum e_OpCode OpCode;

/** Register selection */
enum e_RegSel {
	R0 = 0x00, R1 = 0x01, R2 = 0x02, R3 = 0x03, R4 = 0x04, 
	R5 = 0x05, R6 = 0x06, R7 = 0x07, R8 = 0x08, R9 = 0x09, 
	RX = 0x0A, 
	IR = 0x0B, 
	SR = 0x0C, 
	SP = 0x0D,
	DP = 0x0E,
	PC = 0x0F
};
typedef enum e_RegSel RegSel;

/** Displacement */
enum e_Displacement {
	NoDisplacement 	= 0x00,
	WithDisplacement= 0x01,
	PostIncrement	= 0x02,
	PostDecrement	= 0x03,
	PreIncrement	= 0x04,
	PreDecrement	= 0x05,
	BigConst	= 0x07
};
typedef enum e_Displacement Displacement;

typedef uint32_t Word;
typedef uint8_t Constant;

/** Registerselection and displacement */
struct s_RegisterSelect {
	Displacement disp;
	RegSel reg;
	Constant val;
};
typedef struct s_RegisterSelect RegisterSelect;

struct s_Instruction {
    Constant C:6;
    RegSel B:4;
    Displacement Ib:3;
    RegSel A:4;
    Displacement Ia:3;
    OpCode opcode:6;
    uint8_t reserved:1;
};
typedef struct s_Instruction Instruction;

struct s_InstructionWord {
	uint8_t data[3];
};
typedef struct s_InstructionWord InstructionWord;

struct s_HexFile {
	unsigned int len;
	unsigned int chunks;
	InstructionWord *instructions;
};
typedef struct s_HexFile HexFile;

/**
 * build a Instruction word
 *
 * @param op OpCode
 * @param regA Register A
 * @param regB Register B
 * @param c Constant
 */
Instruction buildInstruction(	OpCode op, 
			RegisterSelect regA, 
			RegisterSelect regB, 
			Constant c
			);

HexFile * newHexFile();
void freeHexFile(HexFile * hexfile);
int saveHexFile(HexFile * hexfile, char * fname);


int addToHexFile(HexFile * hexfile, Instruction cmd);

int parseFile(char * fname, HexFile * hexfile);


#endif
