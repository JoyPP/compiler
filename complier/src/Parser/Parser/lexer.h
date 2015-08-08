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

bool isDigit(char c);	//数字
bool isNonZeroDigit(char c);	//非零数字
bool isLetter(char c);	//字母
bool isChar(char c);	//字符
bool isString(char *str);	//字符串

bool isSeparator(char c);
bool isSingleSeparator(char c);
bool isDoubleSeparator(char c);

int Ctod(char c);	//%c --> %d

//词法分析类
class Lexer
{
private:
	int curChar ;	//the place of current char
	//int curLine = 0;	//current line number
	
	char program[MAX_PRO_LEN];	//读入程序

	char NextChar();			//读入下一个字符
	void BackChar(int mark);	//curChar回到mark
	TOKEN SingleSeparator(char c);	//返回一个SingleSeparator
	TOKEN DoubleSeparator(char c, char d);	//返回一个DoubleSeparator
	TOKEN Reserver(char str[]);	//Identifier or Reserver
public:
	Lexer();
	void getProgram(char filename[]);	//getProgram程序输入到字符串program中
	TOKEN getToken();					//获取当前token
	TOKEN LookAhead(int k);				//获取下k个token
};

#endif

