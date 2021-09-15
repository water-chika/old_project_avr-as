#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
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
	assert(hex1 <= 0xf);
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
}

void get_rd_imm(char* rd_imm_str, int* rd, int* imm)
{
	char* rd_str, * imm_str;
	rd_str = strtok(rd_imm_str, " ,\n");
	imm_str = strtok(NULL, " ,\n");

	*rd = reg_from_str(rd_str);
	sscanf(imm_str, "%i", imm);
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
};


int main(void)
{
	char* line = NULL;
	size_t line_size = 0;
	while (getline(&line, &line_size, stdin) > 0)
	{
		char* cmd;
		char* arguments;

		cmd = strtok(line, " \n");
		if (cmd == NULL)continue;
		arguments = strtok(NULL, "\n");
		if (0 == strcmp(cmd, "andi"))
		{
			int rd;
			int imm;
			get_rd_imm(arguments, &rd, &imm);
			assert(imm < 0xff && imm >= 0);

			int rd_code = rd - 16;
			uint16_t opcode = pack_16_from_4(0b0111,
					imm>>4,
					rd_code,
					imm&0b1111);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "adiw"))
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

			uint16_t opcode = pack_16_from_4(0b1001,
					0b0110,
					((imm>>2)&0b1100)|rd_code,
					imm&0b1111);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "asr"))
		{
			char* rd_str = strtok(arguments, " \n");
			int rd = reg_from_str(rd_str);

			uint16_t opcode = pack_16_from_4(0b1001,
					0b0100|(get_bit_from_byte(rd, 4)),
					rd&0b1111,
					0b0101);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "bclr"))
		{
			char* s_str = strtok(arguments, " \n");
			int s;
			sscanf(s_str, "%i", &s);
			assert(s>=0 && s<8);

			uint16_t opcode = pack_16_from_4(0b1001,
					0b0100,
					0b1000 | s,
					0b1000);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "bld"))
		{
			int rd, b;
			get_rd_imm(arguments, &rd, &b);
			assert((b&0x7)==b);
			
			uint16_t opcode = pack_16_from_4(0b1111,
					0b1000|(rd>>4),
					rd&0b1111,
					b);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "brbc"))
		{
			int s,k;
			get_imm_imm(arguments, &s, &k);
			assert((s&0b111) == s);
			assert(k>=-64 && k < 64);

			uint16_t opcode = get_opcode_offset7_bitoffset(
					0b111101,
					k&0b1111111,
					s);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "brbs"))
		{
			int s,k;
			get_imm_imm(arguments, &s, &k);
			assert((s&0b111) == s);
			assert(k>=-64 && k < 64);

			uint16_t opcode = get_opcode_offset7_bitoffset(
					0b111100,
					k&0b1111111,
					s);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "bset"))
		{
			int s;
			get_imm(arguments, &s);

			uint16_t opcode = pack_16_from_4(
					0b1001,0b0100,s,0b1000);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "bst"))
		{
			int rd, b;
			get_rd_imm(arguments, &rd, &b);

			uint16_t opcode =  get_opcode_rd_b(0b1111101, rd, 0, b);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "call"))
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
		else if (0 == strcmp(cmd, "cbi"))
		{
			int a, b;
			get_imm_imm(arguments, &a, &b);
			uint16_t opcode = pack_16_from_4(
					0b1001,0b1000,a>>1, ((a&1)<<3)|b);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "cbr"))
		{
			int rd;
			int k;
			get_rd_imm(arguments, &rd, &k);
			assert(k< 0xff && k>= 0);

			int imm = 0xff-k;
			int rd_code = rd - 16;
			uint16_t opcode = pack_16_from_4(0b0111,
					imm>>4,
					rd_code,
					imm&0b1111);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "clr"))
		{
			int rd;
			get_rd(arguments, &rd);

			uint16_t opcode = get_opcode_rd_rr(0b001001,
					rd,rd);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else
		{
			int recognize_flag = 0;
			for (int i = 0; i < sizeof(instructs)/sizeof(instructs[0]); i++)
			{
				if (0 == strcmp(cmd, instructs[i].op))
				{
					uint16_t opcode = instructs[i].opcode;
					fwrite(&opcode, sizeof(opcode), 1, stdout);

					recognize_flag = 1;
					break;
				}
			}
			if (recognize_flag)continue;
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

					recognize_flag = 1;
					break;
				}
			}

			if (recognize_flag)continue;
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

					recognize_flag = 1;
					break;
				}
			}
			if (!recognize_flag)
				fprintf(stderr,"%s do not recognize\n", cmd);
		}
	}
	return 0;
}
