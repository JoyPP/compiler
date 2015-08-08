#include "global.h"


#ifndef _MIDCODE_H_
#define _MIDCODE_H_

void mid(char result[]);	// _Lx , _func , Endfunc
void mid(char result[], char arg1[]);	//LCALL _func  、 _tx = arg1 、 
void mid(char result[], char op[], char arg2[]);	// _tx = LCALL _func
void mid(char result[], char arg1[], char op[], char arg2[]);

void middle1(char result[], char arg1[], char op[], char arg2[]);
void middle2(char result[], char arg1[], char op[], char arg2[]);

void CreateMidtmp1();
void CreateMidtmp2();
int getnum(char p[]);
int getarrnum(char p[]);

//中间代码，结构如 result arg1 op arg2
typedef struct MidCode
{
	char result[MAX_STR_LEN];	// result
	char arg1[MAX_STR_LEN];		// arg1
	char op[MAX_STR_LEN];		// op
	char arg2[MAX_STR_LEN];		// arg2
	int block;	//划分的基本块的编号
}MidCode;

#endif