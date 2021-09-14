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

void get_rd_imm(char* rd_imm_str, int* rd, int* imm)
{
	char* rd_str, * imm_str;
	rd_str = strtok(rd_imm_str, " ,\n");
	imm_str = strtok(NULL, " ,\n");

	*rd = reg_from_str(rd_str);
	sscanf(imm_str, "%d", imm);
}

uint16_t get_opcode_rd_rr(uint8_t msb6bits, uint8_t rd, uint8_t rr)
{
	uint16_t opcode = pack_16_from_4(msb6bits>>2,
		pack_4_from_1(msb6bits&2!=0,msb6bits&1, get_bit_from_byte(rr, 4), get_bit_from_byte(rd, 4)),
		rd&0xf,
		rr&0xf);
	return opcode;
}

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
		if (0 == strcmp(cmd, "adc"))
		{
			int rd,rr;
			get_rd_rr(arguments, &rd, &rr);
			uint16_t opcode = get_opcode_rd_rr(0b000111, rd, rr);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "add"))
		{
			int rd, rr;
			get_rd_rr(arguments, &rd, &rr);
			uint16_t opcode = get_opcode_rd_rr(0b000011, rd, rr);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "and"))
		{
			int rd, rr;
			get_rd_rr(arguments, &rd, &rr);
			uint16_t opcode = get_opcode_rd_rr(0b001000, rd, rr);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else if (0 == strcmp(cmd, "andi"))
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
			sscanf(s_str, "%d", &s);
			assert(s>=0 && s<8);

			uint16_t opcode = pack_16_from_4(0b1001,
					0b0100,
					0b1000 | s,
					0b1000);
			fwrite(&opcode, sizeof(opcode), 1, stdout);
		}
		else
		{
			fprintf(stderr,"%s do not recognize\n", cmd);
		}
	}
	return 0;
}
