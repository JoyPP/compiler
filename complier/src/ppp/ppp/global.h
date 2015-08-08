#pragma once

#include "table.h"
#include "midcode.h"
#include<stack>
#include<string>

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define MAX_STR_LEN 100		//最长字符串长度
#define MAX_PRO_LEN 100000	//最长程序长度
#define MAX_MID_LINE 10000	//最大中间代码行数
#define MAX_QUOTE_COUNT 50 //最大引用次数
#define MAX_TEMP_COUNT 1000 //最大临时寄存器个数

extern int CurrentLine ;
extern int errnum ;		//错误数
extern int tempnum;		//$t数量
extern int labelnum;	//标签数
extern int midcodeline;	//中间代码行数
extern int midcodeline1;//窥孔优化中间代码行数
extern int midcodeline2;//局部优化中间代码行数
extern int strnum;		//字符串数量
extern int blocknum;	//基本块数量

//token的类型
enum TOKEN_TYPE
{
	IDEN, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK, IFTK, THENTK, ELSETK,
	WHILETK, SWITCHTK, CASETK, DEFAULTTK, FORTK, SCANFTK, PRINTFTK, RETURNTK,
	PLUS, MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN,
	SEMICN, COMMA, COLON, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
	ERRORTK, NONE
};

//token结构体
typedef struct TOKEN
{
	TOKEN_TYPE type;//token类型
	union Value		//token的值
	{
		int i;
		float f;
		char c;
		char str[MAX_STR_LEN];
		int line;	//error line
	}value;
}TOKEN;

enum TEMP_TYPE{
	NOTTYPE, INTT, CHARR, STRCONN
};

typedef struct TEMP
{
	int regnum;
	int times;
	TEMP_TYPE type;
}TEMP;

extern std::string allstring[MAX_STR_LEN];
extern TEMP midtmpvar[MAX_TEMP_COUNT];
//extern TEMP midtmpvar1[MAX_TEMP_COUNT];
//extern TEMP midtmpvar2[MAX_TEMP_COUNT];
extern Table tab;
extern MidCode midcode[MAX_MID_LINE];
extern MidCode midcode1[MAX_MID_LINE];	//窥孔优化以后
extern MidCode midcode2[MAX_MID_LINE];	//局部优化以后
extern std::stack<std::string> mystack;
extern std::stack<std::string> tmpstack;

extern FILE *midfile;
extern FILE *midfile1;
extern FILE *midfile2;
extern FILE *errfile;
extern FILE *finalfile;

#endif



/*
	char *TOKEN_TYPE_STRING[50]
	{
		"IDEN", "INTCON", "CHARCON", "STRCON", "CONSTTK", "INTTK", "FLOATTK", "CHARTK", "VOIDTK", "MAINTK", "IFTK", "THENTK", "ELSETK",
		"WHILETK", "SWITCHTK", "CASETK", "DEFAULTTK", "FORTK", "SCANFTK", "PRINTFTK", "RETURNTK",
		"PLUS", "MINU", "MULT", "DIV", "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN",
		"SEMICN", "COMMA", "PERIOD", "COLON", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE",
		"ERRORTK", "NONE"
	}

*/
