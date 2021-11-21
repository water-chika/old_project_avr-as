#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mnemonic.h"

char current_line[32];

#define assert_line(expr) assert((expr) || (fprintf(stderr, "%s",current_line),0))

int reg_from_str(char* str)
{
	int reg;
	sscanf(str, "r%d", &reg);
	return reg;
}

uint16_t pack_16_from_4(uint8_t hex3,uint8_t hex2,uint8_t hex1,uint8_t hex0)
{
	assert(hex3 <= 0xf);
	assert(hex2 <= 0xf);
	assert_line(hex1 <= 0xf);
	assert(hex0 <= 0xf);
	
	return (hex3<<12)|(hex2<<8)|(hex1<<4)|(hex0);
}

uint8_t pack_4_from_1(uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0)
{
	assert(bit3 <= 1);
	assert(bit2 <= 1);
	assert(bit1 <= 1);
	assert(bit0 <= 1);

	return (bit3<<3)|(bit2<<2)|(bit1<<1)|(bit0<<0);
}

uint8_t get_bit_from_byte(uint8_t byte, int bit)
{
	return (byte&(1<<bit)) != 0;
}

uint32_t strip_bits(uint32_t b, int start, int len, int shift)
{
	b = b << (32 - (start + len));
	b = b >> (32 - len);
	b = b << shift;
	return b;
}

void get_rd_rr(char* rd_rr_str, int* rd, int* rr)
{
	char* rd_str,* rr_str;
	rd_str = strtok(rd_rr_str, " ,");
	rr_str = strtok(NULL, " ,\n");
	*rd = reg_from_str(rd_str);
	*rr = reg_from_str(rr_str);
}
void get_rd(char* str, int* rd)
{
	char* rd_str;
	rd_str = strtok(str, " ,\n");
	*rd = reg_from_str(rd_str);
	assert(*rd >= 0 && *rd < 32);
}

void get_rd_imm(char* rd_imm_str, int* rd, int* imm)
{
	char* rd_str, * imm_str;
	rd_str = strtok(rd_imm_str, " ,\n");
	imm_str = strtok(NULL, " ,\n");

	*rd = reg_from_str(rd_str);
	sscanf(imm_str, "%i", imm);
}

void get_addr_rr(char* addr_rr_str, int* a, int* rr)
{
	char* a_str, *rr_str;
	a_str = strtok(addr_rr_str, " ,\n");
	rr_str = strtok(NULL, " ,\n");

	sscanf(a_str, "%i", a);
	*rr = reg_from_str(rr_str);
}

void get_imm(char* imm_str,int* imm)
{
	sscanf(imm_str, "%i", imm);
}

void get_imm_imm(char* arg_str, int* imm1, int* imm2)
{
	char* imm1_str, *imm2_str;
	imm1_str = strtok(arg_str, " ,\n");
	imm2_str = strtok(NULL, " ,\n");

	sscanf(imm1_str, "%i", imm1);
	sscanf(imm2_str, "%i", imm2);
}

uint16_t get_opcode_rd_rr(uint8_t msb6bits, uint8_t rd, uint8_t rr)
{
	uint16_t opcode = pack_16_from_4(msb6bits>>2,
		pack_4_from_1((msb6bits&2) != 0,msb6bits&1, get_bit_from_byte(rr, 4), get_bit_from_byte(rd, 4)),
		rd&0xf,
		rr&0xf);
	return opcode;
}

uint16_t get_opcode_offset7_bitoffset(uint8_t msb6bits, uint8_t offset7bits, uint8_t bit_offset3bits)
{
	assert((msb6bits&0b111111)==msb6bits);
	assert((offset7bits&0b1111111)==offset7bits);
	assert((bit_offset3bits&0b111)==bit_offset3bits);
	uint16_t opcode = pack_16_from_4(msb6bits>>2,
			((msb6bits&0b11)<<2)|(offset7bits>>5),
			(offset7bits>>1)&0b1111,
			((offset7bits&0b1)<<3) | bit_offset3bits);
	return opcode;
}

uint16_t get_opcode_rd_b(uint8_t msb7bits, uint8_t rd, uint8_t lsb4, uint8_t bit)
{
	uint16_t opcode = pack_16_from_4(msb7bits>>3,
			(msb7bits&0b111)<<1|(rd>>4),
			rd&0b1111,
			(lsb4<<3)|bit);
	return opcode;
}

uint16_t get_opcode_rd(uint8_t msb7bits, uint8_t rd, uint8_t lsb4bits)
{
	uint16_t opcode = pack_16_from_4(msb7bits>>3,
			(msb7bits & 0b111) << 1| (rd >> 4),
			(rd&0b01111),
			lsb4bits);
	return opcode;
}

struct opcode_offset7
{
    enum mnemonic mnemonic;
	uint8_t msb6bits;
	uint8_t bit_offset;
};

struct opcode_offset7 br_instructs[] = {
	{MNEMONIC_BRCC,0b111101,0},
	{MNEMONIC_BRCS,0b111100,0},
	{MNEMONIC_BREQ,0b111100,1},
	{MNEMONIC_BRGE,0b111101,0b100},
	{MNEMONIC_BRHC,0b111101,0b101},
	{MNEMONIC_BRHS,0b111100,0b101},
	{MNEMONIC_BRID,0b111101,0b111},
	{MNEMONIC_BRIE,0b111100,0b111},
	{MNEMONIC_BRLO,0b111100,0b000},
	{MNEMONIC_BRLT,0b111100,0b100},
	{MNEMONIC_BRMI,0b111100,0b010},
	{MNEMONIC_BRNE,0b111101,0b001},
	{MNEMONIC_BRPL,0b111101,0b010},
	{MNEMONIC_BRSH,0b111101,0b000},
	{MNEMONIC_BRTC,0b111101,0b110},
	{MNEMONIC_BRTS,0b111100,0b110},
	{MNEMONIC_BRVC,0b111101,0b011},
	{MNEMONIC_BRVS,0b111100,0b011},
};

struct opcode_rd_rr
{
    enum mnemonic mnemonic;
	uint8_t msb6bits;
};

struct opcode_rd_rr rd_rr_instructs[] = {
	{MNEMONIC_ADC, 0b000111},
	{MNEMONIC_ADD, 0b000011},
	{MNEMONIC_AND, 0b001000},
	{MNEMONIC_CP,	0b000101},
	{MNEMONIC_CPC,	0b000001},
	{MNEMONIC_CPSE,0b000100},
	{MNEMONIC_EOR,	0b001001},
	{MNEMONIC_MOV,	0b001011},
	{MNEMONIC_MUL,	0b100111},
	{MNEMONIC_OR,	0b001010},
	{MNEMONIC_SBC,	0b000010},
};

struct opcode_rd
{
    enum mnemonic mnemonic;
	uint8_t msb7bits;
	uint8_t lsb4bits;
};

struct opcode_rd rd_instructs[] = {
	{MNEMONIC_COM,	0b1001010, 0b0000},
	{MNEMONIC_DEC, 0b1001010, 0b1010},
	{MNEMONIC_INC,	0b1001010, 0b0011},
	{MNEMONIC_LSR, 0b1001010, 0b0110},
	{MNEMONIC_NEG,	0b1001010, 0b0001},
	{MNEMONIC_POP, 0b1001000, 0b1111},
	{MNEMONIC_PUSH,0b1001001, 0b1111},
	{MNEMONIC_ROR,	0b1001010, 0b0111},
};

struct opcode
{
    enum mnemonic mnemonic;
	uint16_t opcode;
};

struct opcode instructs[] = {
	{MNEMONIC_BREAK,	0b1001010110011000},
	{MNEMONIC_CLC,		0b1001010010001000},
	{MNEMONIC_CLH,		0b1001010011011000},
	{MNEMONIC_CLI,		0b1001010011111000},
	{MNEMONIC_CLN,		0b1001010010101000},
	{MNEMONIC_CLS,		0b1001010011001000},
	{MNEMONIC_CLT,		0b1001010011101000},
	{MNEMONIC_CLV,		0b1001010010111000},
	{MNEMONIC_CLZ,		0b1001010010011000},
	{MNEMONIC_EICALL,	0b1001010100011001},
	{MNEMONIC_EIJMP,	0b1001010000011001},
	{MNEMONIC_ICALL,	0b1001010100001001},
	{MNEMONIC_IJMP,	0b1001010000001001},
	{MNEMONIC_NOP,		0b0000000000000000},
	{MNEMONIC_RET,		0b1001010100001000},
	{MNEMONIC_RETI,	0b1001010100011000},
};

//this imm in [0, 256)
struct opcode_rd_imm
{
    enum mnemonic mnemonic;
	uint8_t msb4bits;
};

struct opcode_rd_imm rd_imm_instructs[] = {
	{MNEMONIC_ANDI,	0b0111},
	{MNEMONIC_CPI,		0b0011},
	{MNEMONIC_LDI,		0b1110},
	{MNEMONIC_ORI,		0b0110},
	{MNEMONIC_SBCI,	0b0100},
};

struct opcode_a5_b3
{
    enum mnemonic mnemonic;
	uint8_t msb8bits;
};

struct opcode_a5_b3 a5_b3_instructs[] = {
	{MNEMONIC_SBI,	0b10011010},
	{MNEMONIC_SBIC,0b10011001},
	{MNEMONIC_SBIS,0b10011011},
};

struct opcode_rd_iw
{
    enum mnemonic mnemonic;
	uint8_t msb8bits;
};

struct opcode_rd_iw rd_iw_instructs[] = {
	{MNEMONIC_SBIW,	0b10010111},
	{MNEMONIC_ADIW,	0b10010110},
};

struct opcode_rr_b
{
	char op[8];
	uint8_t msb7bits;
	uint8_t lsb3bit;
};

int assemble_32(char* cmd, char* arguments)
{
	int recognize_flag = 1;
	if (0 == strcmp(cmd, "call"))
	{
		int imm;
		get_imm(arguments, &imm);
		uint32_t addr = imm&0x3fffff;
		uint16_t opcode0 = pack_16_from_4(0b1001,
				0b0100 | (addr>>21),
				(addr>>17) & 0b1111,
				0b1110 | (addr>>16));
		uint16_t opcode1 = addr & 0xffff;
		fwrite(&opcode0, sizeof(opcode0), 1, stdout);
		fwrite(&opcode1, sizeof(opcode1), 1, stdout);
	}
	else if (0 == strcmp(cmd, "lds"))
	{
		//This is normal lds instructs
		//But when chip is AVRrc, there are another lds instruction.
		//And I do not implement AVRrc version 'lds' instruction.
		int rd, k;
		get_rd_imm(arguments, &rd, &k);
		assert_line(0 <= k && k <65536);

		uint16_t opcode0 = pack_16_from_4(0b1001,
				strip_bits(rd, 4, 1, 0),
				strip_bits(rd, 0, 4, 0),
				0b0000);
		uint16_t opcode1 = k;
		fwrite(&opcode0, sizeof(opcode0), 1, stdout);
		fwrite(&opcode1, sizeof(opcode1), 1, stdout);
	}
	else if (0 == strcmp(cmd, "jmp"))
	{
		int imm;
		get_imm(arguments, &imm);
		assert(imm < 0x400000);
		uint32_t addr = imm&0x3fffff;
		uint16_t opcode0 = pack_16_from_4(0b1001,
				0b0100 | (addr>>21),
				(addr>>17) & 0b1111,
				0b1100 | (addr>>16));
		uint16_t opcode1 = addr & 0xffff;
		fwrite(&opcode0, sizeof(opcode0), 1, stdout);
		fwrite(&opcode1, sizeof(opcode1), 1, stdout);
	}
	else recognize_flag = 0;

	return recognize_flag;
}

int assemble_16_class(enum mnemonic mnemonic, char* arguments)
{
	for (int i = 0; i < sizeof(rd_iw_instructs)/sizeof(rd_iw_instructs[0]); i++)
	{
		if (mnemonic == rd_iw_instructs[i].mnemonic)
		{
			int rd, imm;
			get_rd_imm(arguments, &rd, &imm);
			assert(imm < 64 && imm >= 0);

			int rd_code;
			if (rd == 24) rd_code = 0;
			else if (rd == 26) rd_code = 1;
			else if (rd == 28) rd_code = 2;
			else if (rd == 30) rd_code = 3;
			else fprintf(stderr, "adiw:error r%d\n", rd);

			uint16_t opcode = pack_16_from_4(
					strip_bits(rd_iw_instructs[i].msb8bits, 4, 4, 0),
					strip_bits(rd_iw_instructs[i].msb8bits, 0, 4, 0),
					((imm>>2)&0b1100)|rd_code,
					imm&0b1111);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
			return 1;
		}
	}
	for (int i = 0; i < sizeof(instructs)/sizeof(instructs[0]); i++)
	{
		if (mnemonic == instructs[i].mnemonic)
		{
			uint16_t opcode = instructs[i].opcode;
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}
	for (int i = 0; i < sizeof(a5_b3_instructs)/sizeof(a5_b3_instructs[0]); i++)
	{
		if (mnemonic == a5_b3_instructs[i].mnemonic)
		{
			int a,b;
			get_imm_imm(arguments, &a, &b);
			assert_line(0<= a && a < 32);
			assert_line(0<= b && b < 8);

			uint16_t opcode = pack_16_from_4(
					strip_bits(a5_b3_instructs[i].msb8bits, 4, 4, 0),
					strip_bits(a5_b3_instructs[i].msb8bits, 0, 4, 0),
					strip_bits(a, 1, 4, 0),
					strip_bits(a, 0, 1, 3) | b);
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}

	for (int i = 0; i < sizeof(rd_imm_instructs)/sizeof(rd_imm_instructs[0]); i++)
	{
		if (mnemonic == rd_imm_instructs[i].mnemonic)
		{
			int rd, k;
			get_rd_imm(arguments, &rd, &k);
			assert(16 <= rd && rd < 32);
			assert(0 <= k && k < 256);

			int rd_code = rd - 16;
			uint16_t opcode = pack_16_from_4(rd_imm_instructs[i].msb4bits,
					strip_bits(k,4,4,0),
					rd_code,
					strip_bits(k,0,4,0));
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}

	for (int i = 0; i < sizeof(rd_instructs)/sizeof(rd_instructs[0]); i++)
	{
		if (mnemonic == rd_instructs[i].mnemonic)
		{
			int rd;
			get_rd(arguments, &rd);

			uint16_t opcode = pack_16_from_4(rd_instructs[i].msb7bits>>3,
					((rd_instructs[i].msb7bits & 0b111) << 1) | (rd >> 4),
					rd & 0b1111,
					rd_instructs[i].lsb4bits);
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}

	for (int i = 0; i < sizeof(rd_rr_instructs)/sizeof(rd_rr_instructs[0]); i++)
	{
		if (mnemonic == rd_rr_instructs[i].mnemonic)
		{
			int rd,rr;
			get_rd_rr(arguments, &rd, &rr);
			uint16_t opcode = get_opcode_rd_rr(
					rd_rr_instructs[i].msb6bits,
					rd, rr);
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}

	for (int i = 0; i < sizeof(br_instructs)/sizeof(br_instructs[0]); i++)
	{
		if (mnemonic == br_instructs[i].mnemonic)
		{
			int k;
			get_imm(arguments, &k);
			assert(k>=-64 && k < 64);
			uint16_t opcode = get_opcode_offset7_bitoffset(
					br_instructs[i].msb6bits,
					k&0b1111111,
					br_instructs[i].bit_offset);
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}
	return 0;
}

int assemble_16_else(enum mnemonic mnemonic, char* arguments)
{
    uint16_t opcode;
    switch (mnemonic)
    {
        case MNEMONIC_ELPM:
	{
		if (arguments == NULL)
		{
			uint16_t opcode = 0b1001010111011000;
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else
		{
			char* rd_str,* z;
			rd_str = strtok(arguments, " ,");
			z = strtok(NULL, " ,\n");
			if (0 == strcmp(z, "z"))
			{
				int rd;
				get_rd(rd_str, &rd);
				uint16_t opcode = get_opcode_rd(0b1001000,
						rd,
						0b0110);
				fwrite(&opcode, sizeof(opcode), 1, stdout);
			}
			else if (0 == strcmp(z, "z+"))
			{
				int rd;
				get_rd(rd_str, &rd);
				uint16_t opcode = get_opcode_rd(0b1001000,
						rd,
						0b0111);
				fwrite(&opcode, sizeof(opcode), 1, stdout);
			}
			else
			{
				fprintf(stderr, "elpm: error %s\n", z);
			}
		}
		return 1;
	}

        case MNEMONIC_LPM:
	{
		if (arguments == NULL)
		{
			uint16_t opcode = 0b1001010111001000;
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else
		{
			char* rd_str,* z;
			rd_str = strtok(arguments, " ,");
			z = strtok(NULL, " ,\n");
			if (0 == strcmp(z, "z"))
			{
				int rd;
				get_rd(rd_str, &rd);
				uint16_t opcode = get_opcode_rd(0b1001000,
						rd,
						0b0100);
				fwrite(&opcode, sizeof(opcode), 1, stdout);
			}
			else if (0 == strcmp(z, "z+"))
			{
				int rd;
				get_rd(rd_str, &rd);
				uint16_t opcode = get_opcode_rd(0b1001000,
						rd,
						0b0101);
				fwrite(&opcode, sizeof(opcode), 1, stdout);
			}
			else
			{
				fprintf(stderr, "elpm: error %s\n", z);
			}
		}
		return 1;
	}

        case MNEMONIC_ASR:
	{
		assert(arguments);
		char* rd_str = strtok(arguments, " \n");
		int rd = reg_from_str(rd_str);

		uint16_t opcode = pack_16_from_4(0b1001,
				0b0100|(get_bit_from_byte(rd, 4)),
				rd&0b1111,
				0b0101);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_BCLR:
	{
		assert(arguments);
		char* s_str = strtok(arguments, " \n");
		int s;
		sscanf(s_str, "%i", &s);
		assert(s>=0 && s<8);

		uint16_t opcode = pack_16_from_4(0b1001,
				0b0100,
				0b1000 | s,
				0b1000);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_BLD:
	{
		assert(arguments);
		int rd, b;
		get_rd_imm(arguments, &rd, &b);
		assert((b&0x7)==b);
		
		opcode = pack_16_from_4(0b1111,
				0b1000|(rd>>4),
				rd&0b1111,
				b);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_BRBC:
	{
		assert(arguments);
		int s,k;
		get_imm_imm(arguments, &s, &k);
		assert((s&0b111) == s);
		assert(k>=-64 && k < 64);

		opcode = get_opcode_offset7_bitoffset(
				0b111101,
				k&0b1111111,
				s);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_BRBS:
	{
		assert(arguments);
		int s,k;
		get_imm_imm(arguments, &s, &k);
		assert((s&0b111) == s);
		assert(k>=-64 && k < 64);

		opcode = get_opcode_offset7_bitoffset(
				0b111100,
				k&0b1111111,
				s);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_BSET:
	{
		assert(arguments);
		int s;
		get_imm(arguments, &s);

		opcode = pack_16_from_4(
				0b1001,0b0100,s,0b1000);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_BST:
	{
		int rd, b;
		get_rd_imm(arguments, &rd, &b);

		opcode =  get_opcode_rd_b(0b1111101, rd, 0, b);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_CBI:
	{
		int a, b;
		get_imm_imm(arguments, &a, &b);
		opcode = pack_16_from_4(
				0b1001,0b1000,a>>1, ((a&1)<<3)|b);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_CLR:
	{
		int rd;
		get_rd(arguments, &rd);

		opcode = get_opcode_rd_rr(0b001001,
				rd,rd);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_DES:
	{
		int k;
		get_imm(arguments, &k);
		assert(k >= 0 && k <= 0x0f);

		opcode = pack_16_from_4(0b1001,
				0b0100,
				k,
				0b1011);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_FMUL:
	{
		int rd,rr;
		get_rd_rr(arguments, &rd, &rr);
		assert(rd >= 16 && rd < 32);
		assert(rr >= 16 && rr < 32);

		int rd_code = rd - 16;
		int rr_code = rr - 16;
		opcode = pack_16_from_4(0b0000,
				0b0011,
				0b0000 | rd_code,
				0b1000 | rr_code);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_FMULS:
	{
		int rd,rr;
		get_rd_rr(arguments, &rd, &rr);
		assert(rd >= 16 && rd < 32);
		assert(rr >= 16 && rr < 32);

		int rd_code = rd - 16;
		int rr_code = rr - 16;
		opcode = pack_16_from_4(0b0000,
				0b0011,
				0b1000 | rd_code,
				0b0000 | rr_code);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_FMULSU:
	{
		int rd,rr;
		get_rd_rr(arguments, &rd, &rr);
		assert(rd >= 16 && rd < 32);
		assert(rr >= 16 && rr < 32);

		int rd_code = rd - 16;
		int rr_code = rr - 16;
		opcode = pack_16_from_4(0b0000,
				0b0011,
				0b1000 | rd_code,
				0b1000 | rr_code);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_IN:
	{
		int rd, a;
		get_rd_imm(arguments, &rd, &a);
		assert(rd >= 0 && rd < 32);
		assert(0 <= a && a <64);

		opcode = pack_16_from_4(0b1011,
				((a>>3)&0b110)|(rd >> 4),
				rd & 0b1111,
				a & 0b1111);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_OUT:
	{
		int rr, a;
		get_addr_rr(arguments, &a, &rr);
		assert_line(0 <= a && a < 64);

		opcode = pack_16_from_4(0b1011,
				0b1000 | strip_bits(a,4,2,1) | strip_bits(rr, 4,1, 0),
				strip_bits(rr, 0, 4, 0),
				strip_bits(a, 0, 4, 0));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_LAC:
	{
		char* z_str, *rd_str;
		z_str = strtok(arguments, " ,");
		rd_str = strtok(NULL, " ,\n");

		assert_line(0 == strcmp(z_str, "z"));
		int rd;
		get_rd(rd_str, &rd);
		assert(0 <= rd && rd < 32);

		opcode = get_opcode_rd(0b1001001,
				rd,
				0b0110);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_LAS:
	{
		char* z_str, *rd_str;
		z_str = strtok(arguments, " ,");
		rd_str = strtok(NULL, " ,\n");

		assert_line(0 == strcmp(z_str, "z"));
		int rd;
		get_rd(rd_str, &rd);
		assert(0 <= rd && rd < 32);

		opcode = get_opcode_rd(0b1001001,
				rd,
				0b0101);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_LAT:
	{
		char* z_str, *rd_str;
		z_str = strtok(arguments, " ,");
		rd_str = strtok(NULL, " ,\n");

		assert_line(0 == strcmp(z_str, "z"));
		int rd;
		get_rd(rd_str, &rd);
		assert(0 <= rd && rd < 32);

		opcode = get_opcode_rd(0b1001001,
				rd,
				0b0111);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_MOVW:
	{
		int rd, rr;
		get_rd_rr(arguments, &rd, &rr);
		assert(rd % 2 == 0);
		assert(rr % 2 == 0);

		int rd_code = rd >> 1;
		int rr_code = rr >> 1;
		opcode = pack_16_from_4(0b0000,
				0b0001,
				rd_code,
				rr_code);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_MULS:
	{
		int rd, rr;
		get_rd_rr(arguments, &rd, &rr);
		assert(rd >= 16);
		assert_line(rr >= 16);

		int rd_code = rd - 16;
		int rr_code = rr - 16;
		opcode = pack_16_from_4(0b0000,
				0b0010,
				rd_code,
				rr_code);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_MULSU:
	{
		int rd, rr;
		get_rd_rr(arguments, &rd, &rr);
		assert(16 <= rd && rd < 24);
		assert_line(16 <= rr && rr < 24);

		int rd_code = rd - 16;
		int rr_code = rr - 16;
		opcode = pack_16_from_4(0b0000,
				0b0011,
				rd_code,
				rr_code);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_RCALL:
	{
		int k;
		get_imm(arguments, &k);
		assert(0 <= k && k < 0x1000);

		opcode = pack_16_from_4(0b1101,
				strip_bits(k,8,4,0),
				strip_bits(k,4,4,0),
				strip_bits(k,0,4,0));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_RJMP:
	{
		int k;
		get_imm(arguments, &k);
		assert(0 <= k && k < 0x1000);

		opcode = pack_16_from_4(0b1100,
				strip_bits(k,8,4,0),
				strip_bits(k,4,4,0),
				strip_bits(k,0,4,0));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_SBI:
	{
		int a,b;
		get_imm_imm(arguments, &a, &b);
		assert(0 <= a && a <32);
		assert(0 <= b && b < 8);

		opcode = pack_16_from_4(0b1001,
				0b1010,
				strip_bits(a,1,4,0),
				strip_bits(a,1,1,3)|b);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_LD:
	{
		char* rd_str, *xyz_str;
		rd_str = strtok(arguments, " ,");
		xyz_str = strtok(NULL, " ,\n");

		int rd;
		get_rd(rd_str, &rd);
		uint8_t msb7bits;
		uint8_t lsb4bits;
		if (0 == strcmp(xyz_str,"x"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b1100;
		}
		else if (0 == strcmp(xyz_str, "x+"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b1101;
		}
		else if (0 == strcmp(xyz_str, "-x"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b1110;
		}
		else if (0 == strcmp(xyz_str, "y"))
		{
			msb7bits = 0b1000000;
			lsb4bits = 0b1000;
		}
		else if (0 == strcmp(xyz_str, "y+"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b1001;
		}
		else if (0 == strcmp(xyz_str, "-y"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b1010;
		}
		else if (0 == strcmp(xyz_str, "z"))
		{
			msb7bits = 0b1000000;
			lsb4bits = 0b0000;
		}
		else if (0 == strcmp(xyz_str, "z+"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b0001;
		}
		else if (0 == strcmp(xyz_str, "-z"))
		{
			msb7bits = 0b1001000;
			lsb4bits = 0b0010;
		}
		opcode = pack_16_from_4(msb7bits >>3,
				((msb7bits&0b111)<<1) | (rd>>4),
				rd&0b1111,
				lsb4bits);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        case MNEMONIC_LDD:
	{
		char* rd_str, *yz_str, *q_str;
		rd_str = strtok(arguments, ", ");
		yz_str = strtok(NULL, " +");
		q_str = strtok(NULL, " +\n");
		int rd;
		get_rd(rd_str, &rd);
		int lsb3bit;
		if (0 == strcmp(yz_str, "y"))lsb3bit = 1;
		else if (0 == strcmp(yz_str, "z"))lsb3bit = 0;
		else assert(0);
		int q;
		get_imm(q_str, &q);
		assert(0 <= q && q < 64);

		opcode = pack_16_from_4(0b1000 | ((q>>4)&0b10),
				((q&0b011000)>>1) | (rd>>4),
				rd&0b01111,
				(lsb3bit << 3)| (q & 0b000111));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
        break;
	}
        default:
	{
		return 0;
	}
    }
	return 1;
}

int assemble_16(enum mnemonic mnemonic, char* arguments)
{
	if (assemble_16_class(mnemonic, arguments))
	{
		return 1;
	}
	else if (assemble_16_else(mnemonic, arguments))
	{
		return 1;
	}
}

int main(void)
{
	char* line = NULL;
	size_t line_size = 0;
	while (getline(&line, &line_size, stdin) > 0)
	{
		strcpy(current_line, line);
		char* cmd;
		char* arguments;

		cmd = strtok(line, " \n");
		if (cmd == NULL)continue;
		arguments = strtok(NULL, "\n"); //this delemiters should not contain space.
        enum mnemonic mnemonic = mnemonic_parse_string(cmd);

		if (assemble_16(mnemonic, arguments))
		{
		}
		else if (assemble_32(cmd, arguments))
		{
		}
		else
		{
			fprintf(stderr, "error cmd:%s\n", cmd);
		}
	}
	return 0;
}
