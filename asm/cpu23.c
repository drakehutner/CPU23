/***
 * CPU23 Assembler
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <assert.h>

#include "cpu23.h"


#define BUFFERSIZE 80
#define CHUNKSIZE 10


char * opCodeToString(OpCode o) {
	switch (o) {
		case 0x00: return "NOP";
		case 0x01: return "LD";
		case 0x02: return "STR";
		case 0x03: return "CPY";
		case 0x04: return "SET";
		case 0x05: return "RST";
		case 0x06: return "PSH";
		case 0x07: return "POP";
		case 0x08: return "INC";
		case 0x09: return "DEC";
		case 0x0A: return "ADD";
		case 0x0B: return "SUB";
		case 0x0C: return "LSL";
		case 0x0D: return "LSR";
		case 0x0E: return "AND";
		case 0x0F: return "OR";
		case 0x10: return "XOR";
		case 0x11: return "NOT";
		case 0x12: return "CMP";
		case 0x13: return "TST";
		case 0x14: return "JMP";
		case 0x15: return "BRA";
		case 0x16: return "CLL";
		case 0x17: return "BRS";
		case 0x18: return "RTS";
		case 0x19: return "DSP";
		case 0x1A: return "EMW";
		case 0x1B: return "EMR";
		default: return "ERR";
	}
}
 
InstructionWord fromInt(int cmd) {
	InstructionWord w;
	w.data[0] = cmd & 0xFF;
	w.data[1] = (cmd >> 8) & 0xFF;
	w.data[2] = (cmd >> 16) & 0xFF;
	return w;
}

 
Instruction buildInstruction(OpCode op, RegisterSelect regA, RegisterSelect regB, Constant c) {
    Instruction instr;
    instr.C = c;
    instr.B = regB.reg;
    instr.Ib = regB.disp;
    instr.A = regA.reg;
    instr.Ia = regA.disp;
    instr.opcode = op;
    instr.reserved = 0;

//	if (op == CONSTANT) {
//		return c & 0x7FFFFF;
//	} else if ( op >= 0) {
//		return (1 << 23)
//			| (op << 18)
//			| (regA.disp << 16)
//			| (regA.reg  << 12)
//			| (regB.disp << 10)
//			| (regB.reg  << 6)
//			| (c << 0)
//		;
//	} else {
//		return (1 << 23) | (NOP << 18);
//	}
    return instr;
}

HexFile * newHexFile() {
	HexFile * hexfile = malloc(sizeof(HexFile));
	hexfile->len = 0;
	hexfile->chunks = 0;
	hexfile->instructions = NULL;
	return hexfile;
}

void freeHexFile(HexFile * hexfile) {
	assert(hexfile != NULL);
	{
		if (hexfile->instructions != NULL)
			free(hexfile->instructions);
		free(hexfile);
	}
}

int addToHexFile(HexFile * hexfile, Instruction cmd) {
	assert(hexfile != NULL);
	{
		if (hexfile->chunks == 0) {
			hexfile->instructions = realloc(hexfile->instructions, sizeof(InstructionWord)*((hexfile->len)+CHUNKSIZE));
			hexfile->chunks = CHUNKSIZE;	
		}
		if (hexfile->instructions != NULL) {
			printf("\t->0x%.6X\n", *(uint32_t*)&cmd);
			hexfile->instructions[((hexfile->len)++)] = *(InstructionWord*)&cmd;
			hexfile->chunks--;
			return 0;
		} else {
			printf("ERROR: No Instruction list\n");
			return 1;
		}
	}
}

int parseOpCode(char * line, OpCode * op) {
	int i = 0;
	char cmd[4] = {0};
	for (i = 0; *line != ' ' && i < 3; (*line)++){
		cmd[i++] = *line;
	}
	for (i = 0; i < 0x1F; i++) {
		if (strcmp(cmd, opCodeToString(i))==0) {
			*op = (OpCode)i;
			break;
		}
	}
	return (*op != ERROR);
	
}

int parseRegisterSelect(char * line, RegisterSelect * reg) {
	reg->disp = NoDisplacement;
	reg->reg =  RX;
	
	return 1;
}

int parseConstant(char * line, Constant * cnst) {
	if (sscanf(line, "%i", (int *)cnst) == 0) {
		cnst = 0;
	}
	return 1;
}

int parseLine(char * line, Instruction * cmd) {
	assert(line != NULL);
	{
		OpCode 
			op = NOP
			;
		RegisterSelect 
			rA = {NoDisplacement, RX, 0}, 
			rB = {NoDisplacement, RX, 0}
			;
		Constant 
			cnst = 0
			;
		
		/* Constant */
		printf("\tline: %s\n", line);
		if (*line == '#') {
            RegisterSelect empty;
            empty.reg = 0;
            empty.disp = 0;
			int val = 0;
			op = CONSTANT;
			sscanf(line, "#%i", &val);
            *cmd = buildInstruction(op, empty, empty, val);
		} else{
			if (parseOpCode(line, &op) && parseRegisterSelect(line, &rA) && parseRegisterSelect(line, &rB) && parseConstant(&(line[0]), &cnst))
				*cmd = buildInstruction(op, rA, rB, cnst);
			else 
				cmd = NULL;
		}
		return cmd != NULL;
	}
}


int parseFile(char * fname, HexFile * hexfile) {
	FILE * fHnd = fopen(fname, "r");
	int err = 0;
	if (fHnd != NULL) {
		char buf[BUFFERSIZE+1] = {0};
		char * buf_p = &(buf[0]);
		while (!feof(fHnd) && !err) {
			*buf_p = fgetc(fHnd);
			if (*buf_p == '\n') {
				Instruction c;
				*buf_p = '\0';
				if (parseLine(&(buf[0]), &c))
					err = addToHexFile(hexfile, c);
				buf_p = &(buf[0]);
			} else {
				buf_p++;
			}
		}
		fclose(fHnd);
	}	
	return !err;
}

int saveHexFile(HexFile * hexfile, char * fname) {
	assert(hexfile != NULL);
	{
		unsigned int i = 0;
		FILE * fHnd = fopen(fname, "w");
		if (fHnd != NULL) {
			fputc(0x23, fHnd);
			fputc((hexfile->len >> 16) & 0xFF, fHnd);
			fputc((hexfile->len >> 8) & 0xFF, fHnd);
			fputc((hexfile->len >> 0) & 0xFF, fHnd);
			for (i = 0; i < hexfile->len; i++) {
				fputc(hexfile->instructions[i].data[2], fHnd);
				fputc(hexfile->instructions[i].data[1], fHnd);
				fputc(hexfile->instructions[i].data[0], fHnd);
			}
			fclose(fHnd);
			return 1;
		} else {
			return 0;
		}
	}
}


