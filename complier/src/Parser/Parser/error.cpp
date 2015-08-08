#include "global.h"
#include "parser.h"


void Parser::error(int errid)
{
	errnum++;
	switch (errid)
	{
	case 101: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing ';' \n", errnum, errid, CurrentLine); 	//常变量定义少了一个;  find next ';'
		while (token.type != TOKEN_TYPE::SEMICN)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 102: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing ';' \n", errnum, errid, CurrentLine); break;	//非常变量定义语句后少；

	case 201: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing 'int' or 'char' \n", errnum, errid, CurrentLine); //常量变量定义，少了一个int or char
		while (token.type != TOKEN_TYPE::SEMICN)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;	

	case 202: fprintf(errfile, "ErrorNum(%d): (%d) , Line(%d) : missing 'int' or 'char' \n", errnum, errid, CurrentLine); //函数参数声明，少了一个int or char
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::RPARENT)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 301: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an identifier \n", errnum, errid, CurrentLine); 	//少了一个标识符
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 302: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an identifier \n", errnum, errid, CurrentLine); 	//少了一个标识符
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::RPARENT){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 501: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing '=', identifier is not assigned \n", errnum, errid, CurrentLine); 	//少了=，无法赋值,寻找下一个comma or semicon
		while (token.type != TOKEN_TYPE::SEMICN){

			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 502: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a comparison operator \n", errnum, errid, CurrentLine);	//少了比较运算符
		while (token.type != TOKEN_TYPE::RPARENT)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 503: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a colon in switch-case \n", errnum, errid, CurrentLine); break;	//少了一个：

	case 601: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an integer \n", errnum, errid, CurrentLine); 	//少了整数，无法赋值
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 602: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a char \n", errnum, errid, CurrentLine); 	//少了字符，无法赋值
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 603: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an unsigned integer,declaring array error! \n", errnum, errid, CurrentLine); break;	//无符号整数，声明数组错误
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70201: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right-bracket,array error! \n", errnum, errid, CurrentLine); 	//非右中括号
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70202: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right-bracket,array error! \n", errnum, errid, CurrentLine); 	//非右中括号，赋值时结束该语句
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70203: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right-bracket,array error! \n", errnum, errid, CurrentLine); break;	//直接读

	case 70301: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left parent \n", errnum, errid, CurrentLine); break;	//少了一个左括号, 函数定义,调用
	
	case 70302: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left parent \n", errnum, errid, CurrentLine); break;	//条件 循环
	
	case 70303: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left parent \n", errnum, errid, CurrentLine);
		while (token.type != TOKEN_TYPE::SEMICN){	//next ';'
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70401: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right parent \n", errnum, errid, CurrentLine); 	//少了一个右括号
		while (token.type != TOKEN_TYPE::RPARENT && token.type != TOKEN_TYPE::LBRACE){	//next ')' or '{'
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70402: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right parent \n", errnum, errid, CurrentLine); break;	//条件 循环 ，少了一个右括号

	case 70403: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right parent \n", errnum, errid, CurrentLine);
		while (token.type != TOKEN_TYPE::SEMICN){	//next ';'
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 705: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left brace \n", errnum, errid, CurrentLine); break;	//少了一个左大括号

	case 706: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right brace \n", errnum, errid, CurrentLine); break;	//少了一个右大括号

	case 801: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a CASE in switch-case \n", errnum, errid, CurrentLine); 	//非case语句
		while (token.type != TOKEN_TYPE::CASETK && token.type != TOKEN_TYPE::LBRACE){

			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 901: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : the number of args is wrong \n", errnum, errid, CurrentLine); break;	//函数调用参数数目不对
	case 902: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : the statement of 'printf' is wrong \n", errnum, errid, CurrentLine); break;
	case 903: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : array overflow \n", errnum, errid, CurrentLine); break;
	case 904: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : 'CONST' can't be changed! \n", errnum, errid, CurrentLine); break;

	case 1000: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : 'CONST' should be ahead of var-definition \n", errnum, errid, CurrentLine); break;
	case 1001: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing the main program \n", errnum, errid, CurrentLine); break;	//少了主函数
	case 1002: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : several main programs \n", errnum, errid, CurrentLine); break;
	case 1003: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : no statements is required after the main program \n", errnum, errid, CurrentLine); break;
	
	case 1100: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : var cannot be assigned \n", errnum, errid, CurrentLine); 
		while (token.type != TOKEN_TYPE::SEMICN)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;
	default:
		break;
	}

}

//处理重定义、未定义等情况
void Parser::error(int errid, char str[])
{
	errnum++;
	switch (errid)
	{

	case 401: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : '%s' have already defined! \n", errnum, errid, CurrentLine, str); break;

	case 402: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : undefined function '%s' ! \n", errnum, errid, CurrentLine, str); break;
	case 403: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : undefined identifier '%s' ! \n", errnum, errid, CurrentLine, str); break;
	case 404: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : function '%s' return void ! \n", errnum, errid, CurrentLine, str); break;
	default:
		break;
	}

}