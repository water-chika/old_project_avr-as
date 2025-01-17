#ifndef MNEMONIC_H
#define MNEMONIC_H

#define MNEMONIC_ARITHMETIC_AND_LOGIC_START 0x100
#define MNEMONIC_CHANGE_OF_FLOW_START 0x200
#define MNEMONIC_DATA_TRANSFER_START 0x300
#define MNEMONIC_BIT_AND_BIT_TEST_START 0x400
#define MNEMONIC_MCU_CONTROL_START 0x500

enum mnemonic
{
	MNEMONIC_ADD=MNEMONIC_ARITHMETIC_AND_LOGIC_START,
	MNEMONIC_ADC,
	MNEMONIC_ADIW,
	MNEMONIC_SUB,
	MNEMONIC_SUBI,
	MNEMONIC_SBC,
	MNEMONIC_SBCI,
	MNEMONIC_SBIW,
	MNEMONIC_AND,
	MNEMONIC_ANDI,
	MNEMONIC_OR,
	MNEMONIC_ORI,
	MNEMONIC_EOR,
	MNEMONIC_COM,
	MNEMONIC_NEG,
	MNEMONIC_SBR,
	MNEMONIC_CBR,
	MNEMONIC_INC,
	MNEMONIC_DEC,
	MNEMONIC_TST,
	MNEMONIC_CLR,
	MNEMONIC_SER,
	MNEMONIC_MUL,
	MNEMONIC_MULS,
	MNEMONIC_MULSU,
	MNEMONIC_FMUL,
	MNEMONIC_FMULS,
	MNEMONIC_FMULSU,
	MNEMONIC_DES,

	MNEMONIC_RJMP=MNEMONIC_CHANGE_OF_FLOW_START,
	MNEMONIC_IJMP,
	MNEMONIC_EIJMP,
	MNEMONIC_JMP,
	MNEMONIC_RCALL,
	MNEMONIC_ICALL,
	MNEMONIC_EICALL,
	MNEMONIC_CALL,
	MNEMONIC_RET,
	MNEMONIC_RETI,
	MNEMONIC_CPSE,
	MNEMONIC_CP,
	MNEMONIC_CPC,
	MNEMONIC_CPI,
	MNEMONIC_SBRC,
	MNEMONIC_SBRS,
	MNEMONIC_SBIC,
	MNEMONIC_SBIS,
	MNEMONIC_BRBS,
	MNEMONIC_BRBC,
	MNEMONIC_BREQ,
	MNEMONIC_BRNE,
	MNEMONIC_BRCS,
	MNEMONIC_BRCC,
	MNEMONIC_BRSH,
	MNEMONIC_BRLO,
	MNEMONIC_BRMI,
	MNEMONIC_BRPL,
	MNEMONIC_BRGE,
	MNEMONIC_BRLT,
	MNEMONIC_BRHS,
	MNEMONIC_BRHC,
	MNEMONIC_BRTS,
	MNEMONIC_BRTC,
	MNEMONIC_BRVS,
	MNEMONIC_BRVC,
	MNEMONIC_BRIE,
	MNEMONIC_BRID,

	MNEMONIC_MOV=MNEMONIC_DATA_TRANSFER_START,
	MNEMONIC_MOVW,
	MNEMONIC_LDI,
	MNEMONIC_LDS,
	MNEMONIC_LD,
	MNEMONIC_LDD,
	MNEMONIC_STS,
	MNEMONIC_STD,
	MNEMONIC_LPM,
	MNEMONIC_ELPM,
	MNEMONIC_SPM,
	MNEMONIC_IN,
	MNEMONIC_OUT,
	MNEMONIC_PUSH,
	MNEMONIC_POP,
	MNEMONIC_XCH,
	MNEMONIC_LAS,
	MNEMONIC_LAC,
	MNEMONIC_LAT,

	MNEMONIC_LSL=MNEMONIC_BIT_AND_BIT_TEST_START,
	MNEMONIC_LSR,
	MNEMONIC_ROL,
	MNEMONIC_ROR,
	MNEMONIC_ASR,
	MNEMONIC_SWAP,
	MNEMONIC_SBI,
	MNEMONIC_CBI,
	MNEMONIC_BST,
	MNEMONIC_BLD,
	MNEMONIC_BSET,
	MNEMONIC_BCLR,
	MNEMONIC_SEC,
	MNEMONIC_CLC,
	MNEMONIC_SEN,
	MNEMONIC_CLN,
	MNEMONIC_SEZ,
	MNEMONIC_CLZ,
	MNEMONIC_SEI,
	MNEMONIC_CLI,
	MNEMONIC_SES,
	MNEMONIC_CLS,
	MNEMONIC_SEV,
	MNEMONIC_CLV,
	MNEMONIC_SET,
	MNEMONIC_CLT,
	MNEMONIC_SEH,
	MNEMONIC_CLH,

	MNEMONIC_BREAK=MNEMONIC_MCU_CONTROL_START,
	MNEMONIC_NOP,
	MNEMONIC_SLEEP,
	MNEMONIC_WDR,
};

enum mnemonic mnemonic_parse_string(const char* string);
#endif
