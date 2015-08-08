#pragma once

#include "table.h"
#include "midcode.h"
#include<stack>
#include<string>

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define MAX_STR_LEN 100		//��ַ�������
#define MAX_PRO_LEN 100000	//����򳤶�
#define MAX_MID_LINE 10000	//����м��������
#define MAX_QUOTE_COUNT 50 //������ô���
#define MAX_TEMP_COUNT 1000 //�����ʱ�Ĵ�������

extern int CurrentLine ;
extern int errnum ;		//������
extern int tempnum;		//$t����
extern int labelnum;	//��ǩ��
extern int midcodeline;	//�м��������
extern int midcodeline1;//�����Ż��м��������
extern int midcodeline2;//�ֲ��Ż��м��������
extern int strnum;		//�ַ�������
extern int blocknum;	//����������

//token������
enum TOKEN_TYPE
{
	IDEN, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK, IFTK, THENTK, ELSETK,
	WHILETK, SWITCHTK, CASETK, DEFAULTTK, FORTK, SCANFTK, PRINTFTK, RETURNTK,
	PLUS, MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN,
	SEMICN, COMMA, COLON, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
	ERRORTK, NONE
};

//token�ṹ��
typedef struct TOKEN
{
	TOKEN_TYPE type;//token����
	union Value		//token��ֵ
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
extern MidCode midcode1[MAX_MID_LINE];	//�����Ż��Ժ�
extern MidCode midcode2[MAX_MID_LINE];	//�ֲ��Ż��Ժ�
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
