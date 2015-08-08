#pragma once

//#define _CRT_SECURE_NO_WARNINGS
#include "global.h"

#ifndef _LEXER_H_
#define _LEXER_H_

bool isSpace(char c);	// ' '
bool isNewline(char c);	// '\n'
bool isTab(char c);		// '\t'

bool isaddop(char c);	// + -
bool ismultop(char c);	// * /

bool isDigit(char c);	//����
bool isNonZeroDigit(char c);	//��������
bool isLetter(char c);	//��ĸ
bool isChar(char c);	//�ַ�
bool isString(char *str);	//�ַ���

bool isSeparator(char c);
bool isSingleSeparator(char c);
bool isDoubleSeparator(char c);

int Ctod(char c);	//%c --> %d

//�ʷ�������
class Lexer
{
private:
	int curChar ;	//the place of current char
	//int curLine = 0;	//current line number
	
	char program[MAX_PRO_LEN];	//�������

	char NextChar();			//������һ���ַ�
	void BackChar(int mark);	//curChar�ص�mark
	TOKEN SingleSeparator(char c);	//����һ��SingleSeparator
	TOKEN DoubleSeparator(char c, char d);	//����һ��DoubleSeparator
	TOKEN Reserver(char str[]);	//Identifier or Reserver
public:
	Lexer();
	void getProgram(char filename[]);	//getProgram�������뵽�ַ���program��
	TOKEN getToken();					//��ȡ��ǰtoken
	TOKEN LookAhead(int k);				//��ȡ��k��token
};

#endif

