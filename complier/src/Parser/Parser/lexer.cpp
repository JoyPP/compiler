#define _CRT_SECURE_NO_WARNINGS
#include "lexer.h"
#include "global.h"
#include<string.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include<fstream>

using namespace std;

bool isSpace(char c)
{
	return c == ' ';
}

bool isTab(char c)
{
	return ((c == '	')||(c == '\t'));
}

bool isNewline(char c)
{
	return c == '\n';
}

bool isaddop(char c)
{
	return ((c == '+') || (c == '-'));
}

bool ismultop(char c)
{
	return ((c == '*') || (c == '/'));
}

bool isDigit(char c)
{
	return ((c <= '9') && (c >= '0'));
}

bool isNonZeroDigit(char c)
{
	return ((c <= '9') && (c >= '1')) ;
}

bool isLetter(char c)
{
	return (((c >= 'A')&&(c <= 'Z'))||((c >= 'a')&&(c <= 'z'))||(c == '_'));
}

bool isChar(char c)
{
	return (isaddop(c)||ismultop(c)||isDigit(c)||isLetter(c));
}

bool isString(char *str)
{
	int len = strlen(str);
	if (str[len - 1] != '"')
		return false;
	for (int i = 1; i < len-1; i++)
	{
		if (str[i] >= 32 && str[i] <= 126 && str[i] != 34)
			continue;
		else return false;
	}
	return true;
}

int Ctod(char c)
{
	return (c - '0');
}

Lexer::Lexer()
{
	curChar = 0;
}

char Lexer::NextChar()
{
	curChar++;
	return Lexer::program[curChar-1];
}

void Lexer::BackChar(int mark){
	curChar = mark;
}

bool isSeparator(char c)
{
	return (isSingleSeparator(c) || isDoubleSeparator(c) || isSpace(c) || isNewline(c) || isTab(c));
}

bool isDoubleSeparator(char c)
{
	return ((c == '<') || (c == '>') || (c == '!') || (c == '='));
}

TOKEN Lexer::DoubleSeparator(char c,char d)
{
	TOKEN token;
	switch (c)
	{
	case'<':if (d == '=')
			{
				token.type = TOKEN_TYPE::LEQ;
				strcpy(token.value.str,"<=\0");
			}
			else {
				token.type = TOKEN_TYPE::LSS;
				strcpy(token.value.str, "<\0");
			}
			break;

	case'>':if (d == '=')
			{
				token.type = TOKEN_TYPE::GEQ;
				strcpy(token.value.str, ">=\0");
			}
			else {
				token.type = TOKEN_TYPE::GRE;
				strcpy(token.value.str, ">\0");
			}
			break;

	case'!':if (d == '=')
			{
				token.type = TOKEN_TYPE::NEQ;
				strcpy(token.value.str, "!=\0");
			}
			else {
				token.type = TOKEN_TYPE::ERRORTK;
				token.value.line = CurrentLine;
			}
			break;

	case'=':if (d == '=')
			{
				token.type = TOKEN_TYPE::EQL;
				strcpy(token.value.str, "==\0");
			}
			else {
				token.type = TOKEN_TYPE::ASSIGN;
				strcpy(token.value.str, "=\0");
			}
			break;

	default:
		break;
	}
	if (d != '=')
		BackChar(curChar - 1);
	return token;
}

bool isSingleSeparator(char c)
{
	return (isaddop(c) || ismultop(c) || (c == ';') || (c == ',') || (c == ':')
		 || (c == '(') || (c == ')') || (c == '[') || (c == ']') || (c == '{') || (c == '}'));
}

TOKEN Lexer::SingleSeparator(char c)
{
	TOKEN token;
	token.value.c = c;
	switch (c)
	{
	case'+':token.type = TOKEN_TYPE::PLUS;
			break;

	case'-':token.type = TOKEN_TYPE::MINU;
			break;

	case'*':token.type = TOKEN_TYPE::MULT;
			break;

	case'/':token.type = TOKEN_TYPE::DIV;
			break;
	
	case';':token.type = TOKEN_TYPE::SEMICN;
			break;

	case',':token.type = TOKEN_TYPE::COMMA;
			break;

	case':':token.type = TOKEN_TYPE::COLON;
			break;

	case'(':token.type = TOKEN_TYPE::LPARENT;
			break;

	case')':token.type = TOKEN_TYPE::RPARENT;
			break;

	case'[':token.type = TOKEN_TYPE::LBRACK;
			break;

	case']':token.type = TOKEN_TYPE::RBRACK;
			break;

	case'{':token.type = TOKEN_TYPE::LBRACE;
			break;

	case'}':token.type = TOKEN_TYPE::RBRACE;
			break;

	default: 
			break;
	}
	return token;
}

TOKEN Lexer::Reserver(char str[])
{
	TOKEN token;
	switch (str[0])
	{
	case'c':if (strcmp(str, "case") == 0)
				token.type = TOKEN_TYPE::CASETK;
			else if (strcmp(str, "char") == 0)
				token.type = TOKEN_TYPE::CHARTK;
			else if (strcmp(str, "const") == 0)
				token.type = TOKEN_TYPE::CONSTTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'd':if (strcmp(str, "default") == 0)
				token.type = TOKEN_TYPE::DEFAULTTK;
			else
				token.type = TOKEN_TYPE::IDEN;
		break;

	case'e':if (strcmp(str, "else") == 0)
				token.type = TOKEN_TYPE::ELSETK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'f':if (strcmp(str, "for") == 0)
				token.type = TOKEN_TYPE::FORTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'i':if (strcmp(str, "if") == 0)
				token.type = TOKEN_TYPE::IFTK;
			else if (strcmp(str, "int") == 0)
				token.type = TOKEN_TYPE::INTTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'm':if (strcmp(str, "main") == 0)
				token.type = TOKEN_TYPE::MAINTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'p':if (strcmp(str, "printf") == 0)
				token.type = TOKEN_TYPE::PRINTFTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'r':if (strcmp(str, "return") == 0)
				token.type = TOKEN_TYPE::RETURNTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case's':if (strcmp(str, "scanf") == 0)
				token.type = TOKEN_TYPE::SCANFTK;
			else if (strcmp(str, "switch") == 0)
				token.type = TOKEN_TYPE::SWITCHTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case't':if (strcmp(str, "then") == 0)
				token.type = TOKEN_TYPE::THENTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'v':if (strcmp(str, "void") == 0)
				token.type = TOKEN_TYPE::VOIDTK;
			else
				token.type = TOKEN_TYPE::IDEN;
			break;

	case'w':if (strcmp(str, "while") == 0)
				token.type = TOKEN_TYPE::WHILETK;
			else 
				token.type = TOKEN_TYPE::IDEN; 
			break;

	default:token.type = TOKEN_TYPE::IDEN;
			break;

	}
	strcpy(token.value.str , str);
	return token;
}

void Lexer::getProgram(char filename[])
{
	FILE *infile;
	program[0] = '\0';
	infile = fopen(filename,"r");
	if (infile != NULL)
	{
		char scr[1000];
		cout << "FILE OPEN SUCCEED!" << endl;	//打开成功
		while (!feof(infile))	//循环将每行程序放在program[]里面
		{
			fgets(scr, 1000, infile);
			strcat(program, scr);
		}
	}
	else	//打开文件失败
	{
		cout << "FILE OPEN FAILED!" << endl;
	}
}

TOKEN Lexer::getToken()
{
	char c;
	TOKEN token;
	while ((c = NextChar()) != '\0')
	{
		while (isSpace(c) || isNewline(c) || isTab(c))
		{
			if (isNewline(c))
				CurrentLine++;
			c = NextChar();
		}
		if (isLetter(c))//Identifier or Reserver
		{
			int j = 0;
			bool flag = true;
			char temp[MAX_STR_LEN];
			while (!isSeparator(c))
			{
				if (isLetter(c) || isDigit(c))
				{
					temp[j] = (char)c;
					j++;
				}
				else flag = false;
				c = NextChar();
			}
			BackChar(curChar - 1);
			temp[j] = '\0';
			if (flag)
				return Reserver(temp);
			else{
				token.type = TOKEN_TYPE::NONE;
			}
		}
		else if (isNonZeroDigit(c))	//INTCON,非零整数
		{
			bool flag = true;
			int num = Ctod(c);
			c = NextChar();
			while (!isSeparator(c))
			{
				if (isDigit(c)){
					num = num * 10 + Ctod(c);
				}
				else flag = false;
				c = NextChar();
			}
			BackChar(curChar - 1);
			if (flag){
				token.type = TOKEN_TYPE::INTCON;
				token.value.i = num;
			}
			else token.type = TOKEN_TYPE::NONE;
		}
		else if (isDigit(c))	//0
		{
			int num = 0;
			c = NextChar();
			if (isSeparator(c))
			{
				token.type = TOKEN_TYPE::INTCON;
				token.value.i = num;
			}
			else
			{
				token.type = TOKEN_TYPE::NONE;
				while (!isNewline(c) && !isTab(c) && !isSpace(c))
					c = NextChar();
			}
			BackChar(curChar - 1);
		}
		else if (c == '\'' || c == '\"')	//CHARCON or STRCON
		{
			if (c == '\'')	//char
			{
				if (isChar(c = NextChar()))
				{
					char mark = c;
					if ((c = NextChar()) == '\'')
					{
						token.type = TOKEN_TYPE::CHARCON;
						token.value.c = mark;
					}
					else
					{
						BackChar(curChar - 1);//回退两步到第一个'的地方
					}
				}
				else {
					token.type = TOKEN_TYPE::ERRORTK;
					token.value.line = CurrentLine;
					BackChar(curChar - 1);//回退一步到第一个'的地方
				}
			}
			else	//string
			{
				char temp[MAX_STR_LEN];
				int j = 1;
				temp[0] = '\"';
				while ((c = NextChar()) != '\"')
				{
					temp[j++] = c;
					if (c == '\n')	//读到行末尾都没有“
					{
						token.type = TOKEN_TYPE::ERRORTK;
						token.value.line = CurrentLine;
					}
				}
				temp[j++] = '\"';
				temp[j] = '\0';
				if (isString(temp))
				{
					token.type = TOKEN_TYPE::STRCON;
					strcpy(token.value.str,temp);
				}
				else{
					token.type = TOKEN_TYPE::ERRORTK;
					token.value.line = CurrentLine;
				}
			}
		}
		else if (isSingleSeparator(c))
		{
			return SingleSeparator(c);
		}
		else if (isDoubleSeparator(c))
		{
			char mark = c;
			c = NextChar();
			return DoubleSeparator(mark, c);
		}
		else if (c == '\0')
		{
			token.type = TOKEN_TYPE::NONE;
		}
		else
		{
			token.type = TOKEN_TYPE::ERRORTK;
			token.value.line = CurrentLine;
		}
		return token;
	}
	token.type = TOKEN_TYPE::NONE;
	return token;
}

TOKEN Lexer::LookAhead(int k)
{
	TOKEN token;
	int mark = curChar;
	for (int i = 0; i < k; i++)
	{
		token = getToken();
	}
	BackChar(mark);
	return token;
}