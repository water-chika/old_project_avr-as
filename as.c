#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

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
	char op[8];
	uint8_t msb6bits;
	uint8_t bit_offset;
};

struct opcode_offset7 br_instructs[] = {
	{"brcc",0b111101,0},
	{"brcs",0b111100,0},
	{"breq",0b111100,1},
	{"brge",0b111101,0b100},
	{"brhc",0b111101,0b101},
	{"brhs",0b111100,0b101},
	{"brid",0b111101,0b111},
	{"brie",0b111100,0b111},
	{"brlo",0b111100,0b000},
	{"brlt",0b111100,0b100},
	{"brmi",0b111100,0b010},
	{"brne",0b111101,0b001},
	{"brpl",0b111101,0b010},
	{"brsh",0b111101,0b000},
	{"brtc",0b111101,0b110},
	{"brts",0b111100,0b110},
	{"brvc",0b111101,0b011},
	{"brvs",0b111100,0b011},
};

struct opcode_rd_rr
{
	char op[8];
	uint8_t msb6bits;
};

struct opcode_rd_rr rd_rr_instructs[] = {
	{"adc", 0b000111},
	{"add", 0b000011},
	{"and", 0b001000},
	{"cp",	0b000101},
	{"cpc",	0b000001},
	{"cpse",0b000100},
	{"eor",	0b001001},
	{"mov",	0b001011},
	{"mul",	0b100111},
	{"or",	0b001010},
	{"sbc",	0b000010},
};

struct opcode_rd
{
	char op[8];
	uint8_t msb7bits;
	uint8_t lsb4bits;
};

struct opcode_rd rd_instructs[] = {
	{"com",	0b1001010, 0b0000},
	{"dec", 0b1001010, 0b1010},
	{"inc",	0b1001010, 0b0011},
	{"lsr", 0b1001010, 0b0110},
	{"neg",	0b1001010, 0b0001},
	{"pop", 0b1001000, 0b1111},
	{"push",0b1001001, 0b1111},
	{"ror",	0b1001010, 0b0111},
};

struct opcode
{
	char op[8];
	uint16_t opcode;
};

struct opcode instructs[] = {
	{"break",	0b1001010110011000},
	{"clc",		0b1001010010001000},
	{"clh",		0b1001010011011000},
	{"cli",		0b1001010011111000},
	{"cln",		0b1001010010101000},
	{"cls",		0b1001010011001000},
	{"clt",		0b1001010011101000},
	{"clv",		0b1001010010111000},
	{"clz",		0b1001010010011000},
	{"eicall",	0b1001010100011001},
	{"eijmp",	0b1001010000011001},
	{"icall",	0b1001010100001001},
	{"ijmp",	0b1001010000001001},
	{"nop",		0b0000000000000000},
	{"ret",		0b1001010100001000},
	{"reti",	0b1001010100011000},
};

//this imm in [0, 256)
struct opcode_rd_imm
{
	char op[8];
	uint8_t msb4bits;
};

struct opcode_rd_imm rd_imm_instructs[] = {
	{"andi",	0b0111},
	{"cpi",		0b0011},
	{"ldi",		0b1110},
	{"ori",		0b0110},
	{"sbci",	0b0100},
};

struct opcode_a5_b3
{
	char op[8];
	uint8_t msb8bits;
};

struct opcode_a5_b3 a5_b3_instructs[] = {
	{"sbi",	0b10011010},
	{"sbic",0b10011001},
	{"sbis",0b10011011},
};

struct opcode_rd_iw
{
	char op[8];
	uint8_t msb8bits;
};

struct opcode_rd_iw rd_iw_instructs[] = {
	{"sbiw",	0b10010111},
	{"adiw",	0b10010110},
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

int assemble_16_class(char* cmd, char* arguments)
{
	for (int i = 0; i < sizeof(rd_iw_instructs)/sizeof(rd_iw_instructs[0]); i++)
	{
		if (0 == strcmp(cmd, rd_iw_instructs[i].op))
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
			return 0;
		}
	}
	for (int i = 0; i < sizeof(instructs)/sizeof(instructs[0]); i++)
	{
		if (0 == strcmp(cmd, instructs[i].op))
		{
			uint16_t opcode = instructs[i].opcode;
			fwrite(&opcode, sizeof(opcode), 1, stdout);

			return 1;
		}
	}
	for (int i = 0; i < sizeof(a5_b3_instructs)/sizeof(a5_b3_instructs[0]); i++)
	{
		if (0 == strcmp(cmd, a5_b3_instructs[i].op))
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
		if (0 == strcmp(cmd, rd_imm_instructs[i].op))
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
		if (0 == strcmp(cmd, rd_instructs[i].op))
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
		if (0 == strcmp(cmd, rd_rr_instructs[i].op))
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
		if (0 == strcmp(cmd, br_instructs[i].op))
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

int assemble_16_else(char* cmd, char* arguments)
{
	if (0 == strcmp(cmd, "elpm"))
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

	if (0 == strcmp(cmd, "lpm"))
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

	uint16_t opcode;
	assert(arguments);
	if (0 == strcmp(cmd, "asr"))
	{
		assert(arguments);
		char* rd_str = strtok(arguments, " \n");
		int rd = reg_from_str(rd_str);

		opcode = pack_16_from_4(0b1001,
				0b0100|(get_bit_from_byte(rd, 4)),
				rd&0b1111,
				0b0101);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "bclr"))
	{
		assert(arguments);
		char* s_str = strtok(arguments, " \n");
		int s;
		sscanf(s_str, "%i", &s);
		assert(s>=0 && s<8);

		opcode = pack_16_from_4(0b1001,
				0b0100,
				0b1000 | s,
				0b1000);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "bld"))
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
	}
	else if (0 == strcmp(cmd, "brbc"))
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
	}
	else if (0 == strcmp(cmd, "brbs"))
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
	}
	else if (0 == strcmp(cmd, "bset"))
	{
		assert(arguments);
		int s;
		get_imm(arguments, &s);

		opcode = pack_16_from_4(
				0b1001,0b0100,s,0b1000);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "bst"))
	{
		int rd, b;
		get_rd_imm(arguments, &rd, &b);

		opcode =  get_opcode_rd_b(0b1111101, rd, 0, b);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "cbi"))
	{
		int a, b;
		get_imm_imm(arguments, &a, &b);
		opcode = pack_16_from_4(
				0b1001,0b1000,a>>1, ((a&1)<<3)|b);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "clr"))
	{
		int rd;
		get_rd(arguments, &rd);

		opcode = get_opcode_rd_rr(0b001001,
				rd,rd);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "des"))
	{
		int k;
		get_imm(arguments, &k);
		assert(k >= 0 && k <= 0x0f);

		opcode = pack_16_from_4(0b1001,
				0b0100,
				k,
				0b1011);
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "fmul"))
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
	}
	else if (0 == strcmp(cmd, "fmuls"))
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
	}
	else if (0 == strcmp(cmd, "fmulsu"))
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
	}
	else if (0 == strcmp(cmd, "in"))
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
	}
	else if (0 == strcmp(cmd, "out"))
	{
		int rr, a;
		get_addr_rr(arguments, &a, &rr);
		assert_line(0 <= a && a < 64);

		opcode = pack_16_from_4(0b1011,
				0b1000 | strip_bits(a,4,2,1) | strip_bits(rr, 4,1, 0),
				strip_bits(rr, 0, 4, 0),
				strip_bits(a, 0, 4, 0));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "lac"))
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
	}
	else if (0 == strcmp(cmd, "las"))
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
	}
	else if (0 == strcmp(cmd, "lat"))
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
	}
	else if (0 == strcmp(cmd, "movw"))
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
	}
	else if (0 == strcmp(cmd, "muls"))
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
	}
	else if (0 == strcmp(cmd, "mulsu"))
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
	}
	else if (0 == strcmp(cmd, "rcall"))
	{
		int k;
		get_imm(arguments, &k);
		assert(0 <= k && k < 0x1000);

		opcode = pack_16_from_4(0b1101,
				strip_bits(k,8,4,0),
				strip_bits(k,4,4,0),
				strip_bits(k,0,4,0));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "rjmp"))
	{
		int k;
		get_imm(arguments, &k);
		assert(0 <= k && k < 0x1000);

		opcode = pack_16_from_4(0b1100,
				strip_bits(k,8,4,0),
				strip_bits(k,4,4,0),
				strip_bits(k,0,4,0));
		fwrite(&opcode, sizeof(opcode), 1, stdout);
	}
	else if (0 == strcmp(cmd, "sbi"))
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
	}
	else if (0 == strcmp(cmd, "ld"))
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
	}
	else if (0 == strcmp(cmd, "ldd"))
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
	}
	else
	{
		return 0;
	}

	return 1;
}

int assemble_16(char* cmd, char* arguments)
{
	if (assemble_16_class(cmd, arguments))
	{
		return 1;
	}
	else if (assemble_16_else(cmd, arguments))
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

		if (assemble_16(cmd, arguments))
		{
		}
		else if (assemble_32(cmd, arguments))
		{
		}
		else
		{
			fprintf(stderr, "error cmd");
		}
	}
	return 0;
}
