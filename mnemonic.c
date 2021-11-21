#include "mnemonic.h"
#include <string.h>
#include <assert.h>

enum mnemonic mnemonic_parse_string(const char* string)
{
    enum mnemonic mnemonic = 0;
    
    if (0==strcmp("add",string)) mnemonic = MNEMONIC_ADD;
    else if (0==strcmp("adc",string)) mnemonic = MNEMONIC_ADC;
    else if (0==strcmp("adiw",string)) mnemonic = MNEMONIC_ADIW;
    else if (0==strcmp("sub",string)) mnemonic = MNEMONIC_SUB;
    else if (0==strcmp("subi",string)) mnemonic = MNEMONIC_SUBI;
    else if (0==strcmp("sbc",string)) mnemonic = MNEMONIC_SBC;
    else if (0==strcmp("sbci",string)) mnemonic = MNEMONIC_SBCI;
    else if (0==strcmp("sbiw",string)) mnemonic = MNEMONIC_SBIW;
    else if (0==strcmp("and",string)) mnemonic = MNEMONIC_AND;
    else if (0==strcmp("andi",string)) mnemonic = MNEMONIC_ANDI;
    else if (0==strcmp("or",string)) mnemonic = MNEMONIC_OR;
    else if (0==strcmp("ori",string)) mnemonic = MNEMONIC_ORI;
    else if (0==strcmp("eor",string)) mnemonic = MNEMONIC_EOR;
    else if (0==strcmp("com",string)) mnemonic = MNEMONIC_COM;
    else if (0==strcmp("neg",string)) mnemonic = MNEMONIC_NEG;
    else if (0==strcmp("sbr",string)) mnemonic = MNEMONIC_SBR;
    else if (0==strcmp("cbr",string)) mnemonic = MNEMONIC_CBR;
    else if (0==strcmp("inc",string)) mnemonic = MNEMONIC_INC;
    else if (0==strcmp("dec",string)) mnemonic = MNEMONIC_DEC;
    else if (0==strcmp("tst",string)) mnemonic = MNEMONIC_TST;
    else if (0==strcmp("clr",string)) mnemonic = MNEMONIC_CLR;
    else if (0==strcmp("ser",string)) mnemonic = MNEMONIC_SER;
    else if (0==strcmp("mul",string)) mnemonic = MNEMONIC_MUL;
    else if (0==strcmp("muls",string)) mnemonic = MNEMONIC_MULS;
    else if (0==strcmp("mulsu",string)) mnemonic = MNEMONIC_MULSU;
    else if (0==strcmp("fmul",string)) mnemonic = MNEMONIC_FMUL;
    else if (0==strcmp("fmuls",string)) mnemonic = MNEMONIC_FMULS;
    else if (0==strcmp("fmulsu",string)) mnemonic = MNEMONIC_FMULSU;
    else if (0==strcmp("des",string)) mnemonic = MNEMONIC_DES;

    else if (0==strcmp("rjmp",string)) mnemonic = MNEMONIC_RJMP;
    else if (0==strcmp("ijmp",string)) mnemonic = MNEMONIC_IJMP;
    else if (0==strcmp("eijmp",string)) mnemonic = MNEMONIC_EIJMP;
    else if (0==strcmp("jmp",string)) mnemonic = MNEMONIC_JMP;
    else if (0==strcmp("rcall",string)) mnemonic = MNEMONIC_RCALL;
    else if (0==strcmp("icall",string)) mnemonic = MNEMONIC_ICALL;
    else if (0==strcmp("eicall",string)) mnemonic = MNEMONIC_EICALL;
    else if (0==strcmp("call",string)) mnemonic = MNEMONIC_CALL;
    else if (0==strcmp("ret",string)) mnemonic = MNEMONIC_RET;
    else if (0==strcmp("reti",string)) mnemonic = MNEMONIC_RETI;
    else if (0==strcmp("cpse",string)) mnemonic = MNEMONIC_CPSE;
    else if (0==strcmp("cp",string)) mnemonic = MNEMONIC_CP;
    else if (0==strcmp("cpc",string)) mnemonic = MNEMONIC_CPC;
    else if (0==strcmp("cpi",string)) mnemonic = MNEMONIC_CPI;
    else if (0==strcmp("sbrc",string)) mnemonic = MNEMONIC_SBRC;
    else if (0==strcmp("sbrs",string)) mnemonic = MNEMONIC_SBRS;
    else if (0==strcmp("sbic",string)) mnemonic = MNEMONIC_SBIC;
    else if (0==strcmp("sbis",string)) mnemonic = MNEMONIC_SBIS;
    else if (0==strcmp("brbs",string)) mnemonic = MNEMONIC_BRBS;
    else if (0==strcmp("brbc",string)) mnemonic = MNEMONIC_BRBC;
    else if (0==strcmp("breq",string)) mnemonic = MNEMONIC_BREQ;
    else if (0==strcmp("brne",string)) mnemonic = MNEMONIC_BRNE;
    else if (0==strcmp("brcs",string)) mnemonic = MNEMONIC_BRCS;
    else if (0==strcmp("brcc",string)) mnemonic = MNEMONIC_BRCC;
    else if (0==strcmp("brsh",string)) mnemonic = MNEMONIC_BRSH;
    else if (0==strcmp("brlo",string)) mnemonic = MNEMONIC_BRLO;
    else if (0==strcmp("brmi",string)) mnemonic = MNEMONIC_BRMI;
    else if (0==strcmp("brpl",string)) mnemonic = MNEMONIC_BRPL;
    else if (0==strcmp("brge",string)) mnemonic = MNEMONIC_BRGE;
    else if (0==strcmp("brlt",string)) mnemonic = MNEMONIC_BRLT;
    else if (0==strcmp("brhs",string)) mnemonic = MNEMONIC_BRHS;
    else if (0==strcmp("brhc",string)) mnemonic = MNEMONIC_BRHC;
    else if (0==strcmp("brts",string)) mnemonic = MNEMONIC_BRTS;
    else if (0==strcmp("brtc",string)) mnemonic = MNEMONIC_BRTC;
    else if (0==strcmp("brvs",string)) mnemonic = MNEMONIC_BRVS;
    else if (0==strcmp("brvc",string)) mnemonic = MNEMONIC_BRVC;
    else if (0==strcmp("brie",string)) mnemonic = MNEMONIC_BRIE;
    else if (0==strcmp("brid",string)) mnemonic = MNEMONIC_BRID;

    else if (0==strcmp("mov",string)) mnemonic = MNEMONIC_MOV;
    else if (0==strcmp("movw",string)) mnemonic = MNEMONIC_MOVW;
    else if (0==strcmp("ldi",string)) mnemonic = MNEMONIC_LDI;
    else if (0==strcmp("lds",string)) mnemonic = MNEMONIC_LDS;
    else if (0==strcmp("ld",string)) mnemonic = MNEMONIC_LD;
    else if (0==strcmp("ldd",string)) mnemonic = MNEMONIC_LDD;
    else if (0==strcmp("sts",string)) mnemonic = MNEMONIC_STS;
    else if (0==strcmp("std",string)) mnemonic = MNEMONIC_STD;
    else if (0==strcmp("lpm",string)) mnemonic = MNEMONIC_LPM;
    else if (0==strcmp("elpm",string)) mnemonic = MNEMONIC_ELPM;
    else if (0==strcmp("spm",string)) mnemonic = MNEMONIC_SPM;
    else if (0==strcmp("in",string)) mnemonic = MNEMONIC_IN;
    else if (0==strcmp("out",string)) mnemonic = MNEMONIC_OUT;
    else if (0==strcmp("push",string)) mnemonic = MNEMONIC_PUSH;
    else if (0==strcmp("pop",string)) mnemonic = MNEMONIC_POP;
    else if (0==strcmp("xch",string)) mnemonic = MNEMONIC_XCH;
    else if (0==strcmp("las",string)) mnemonic = MNEMONIC_LAS;
    else if (0==strcmp("lac",string)) mnemonic = MNEMONIC_LAC;
    else if (0==strcmp("lat",string)) mnemonic = MNEMONIC_LAT;

    else if (0==strcmp("lsl",string)) mnemonic = MNEMONIC_LSL;
    else if (0==strcmp("lsr",string)) mnemonic = MNEMONIC_LSR;
    else if (0==strcmp("rol",string)) mnemonic = MNEMONIC_ROL;
    else if (0==strcmp("ror",string)) mnemonic = MNEMONIC_ROR;
    else if (0==strcmp("asr",string)) mnemonic = MNEMONIC_ASR;
    else if (0==strcmp("swap",string)) mnemonic = MNEMONIC_SWAP;
    else if (0==strcmp("sbi",string)) mnemonic = MNEMONIC_SBI;
    else if (0==strcmp("cbi",string)) mnemonic = MNEMONIC_CBI;
    else if (0==strcmp("bst",string)) mnemonic = MNEMONIC_BST;
    else if (0==strcmp("bld",string)) mnemonic = MNEMONIC_BLD;
    else if (0==strcmp("bset",string)) mnemonic = MNEMONIC_BSET;
    else if (0==strcmp("bclr",string)) mnemonic = MNEMONIC_BCLR;
    else if (0==strcmp("sec",string)) mnemonic = MNEMONIC_SEC;
    else if (0==strcmp("clc",string)) mnemonic = MNEMONIC_CLC;
    else if (0==strcmp("sen",string)) mnemonic = MNEMONIC_SEN;
    else if (0==strcmp("cln",string)) mnemonic = MNEMONIC_CLN;
    else if (0==strcmp("sez",string)) mnemonic = MNEMONIC_SEZ;
    else if (0==strcmp("clz",string)) mnemonic = MNEMONIC_CLZ;
    else if (0==strcmp("sei",string)) mnemonic = MNEMONIC_SEI;
    else if (0==strcmp("cli",string)) mnemonic = MNEMONIC_CLI;
    else if (0==strcmp("ses",string)) mnemonic = MNEMONIC_SES;
    else if (0==strcmp("cls",string)) mnemonic = MNEMONIC_CLS;
    else if (0==strcmp("sev",string)) mnemonic = MNEMONIC_SEV;
    else if (0==strcmp("clv",string)) mnemonic = MNEMONIC_CLV;
    else if (0==strcmp("set",string)) mnemonic = MNEMONIC_SET;
    else if (0==strcmp("clt",string)) mnemonic = MNEMONIC_CLT;
    else if (0==strcmp("seh",string)) mnemonic = MNEMONIC_SEH;
    else if (0==strcmp("clh",string)) mnemonic = MNEMONIC_CLH;

    else if (0==strcmp("break",string)) mnemonic = MNEMONIC_BREAK;
    else if (0==strcmp("nop",string)) mnemonic = MNEMONIC_NOP;
    else if (0==strcmp("sleep",string)) mnemonic = MNEMONIC_SLEEP;
    else if (0==strcmp("wdr",string)) mnemonic = MNEMONIC_WDR;
    assert(mnemonic != 0);
    return mnemonic;
};