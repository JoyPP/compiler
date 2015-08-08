#include "global.h"
#include "parser.h"


void Parser::error(int errid)
{
	errnum++;
	switch (errid)
	{
	case 101: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing ';' \n", errnum, errid, CurrentLine); 	//��������������һ��;  find next ';'
		while (token.type != TOKEN_TYPE::SEMICN)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 102: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing ';' \n", errnum, errid, CurrentLine); break;	//�ǳ��������������٣�

	case 201: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing 'int' or 'char' \n", errnum, errid, CurrentLine); //�����������壬����һ��int or char
		while (token.type != TOKEN_TYPE::SEMICN)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;	

	case 202: fprintf(errfile, "ErrorNum(%d): (%d) , Line(%d) : missing 'int' or 'char' \n", errnum, errid, CurrentLine); //������������������һ��int or char
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::RPARENT)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 301: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an identifier \n", errnum, errid, CurrentLine); 	//����һ����ʶ��
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 302: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an identifier \n", errnum, errid, CurrentLine); 	//����һ����ʶ��
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::RPARENT){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 501: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing '=', identifier is not assigned \n", errnum, errid, CurrentLine); 	//����=���޷���ֵ,Ѱ����һ��comma or semicon
		while (token.type != TOKEN_TYPE::SEMICN){

			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 502: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a comparison operator \n", errnum, errid, CurrentLine);	//���˱Ƚ������
		while (token.type != TOKEN_TYPE::RPARENT)
		{
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 503: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a colon in switch-case \n", errnum, errid, CurrentLine); break;	//����һ����

	case 601: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an integer \n", errnum, errid, CurrentLine); 	//�����������޷���ֵ
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 602: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a char \n", errnum, errid, CurrentLine); 	//�����ַ����޷���ֵ
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 603: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing an unsigned integer,declaring array error! \n", errnum, errid, CurrentLine); break;	//�޷��������������������
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70201: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right-bracket,array error! \n", errnum, errid, CurrentLine); 	//����������
		while (token.type != TOKEN_TYPE::COMMA && token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70202: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right-bracket,array error! \n", errnum, errid, CurrentLine); 	//���������ţ���ֵʱ���������
		while (token.type != TOKEN_TYPE::SEMICN){
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70203: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right-bracket,array error! \n", errnum, errid, CurrentLine); break;	//ֱ�Ӷ�

	case 70301: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left parent \n", errnum, errid, CurrentLine); break;	//����һ��������, ��������,����
	
	case 70302: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left parent \n", errnum, errid, CurrentLine); break;	//���� ѭ��
	
	case 70303: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left parent \n", errnum, errid, CurrentLine);
		while (token.type != TOKEN_TYPE::SEMICN){	//next ';'
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70401: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right parent \n", errnum, errid, CurrentLine); 	//����һ��������
		while (token.type != TOKEN_TYPE::RPARENT && token.type != TOKEN_TYPE::LBRACE){	//next ')' or '{'
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 70402: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right parent \n", errnum, errid, CurrentLine); break;	//���� ѭ�� ������һ��������

	case 70403: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right parent \n", errnum, errid, CurrentLine);
		while (token.type != TOKEN_TYPE::SEMICN){	//next ';'
			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 705: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a left brace \n", errnum, errid, CurrentLine); break;	//����һ���������

	case 706: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a right brace \n", errnum, errid, CurrentLine); break;	//����һ���Ҵ�����

	case 801: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing a CASE in switch-case \n", errnum, errid, CurrentLine); 	//��case���
		while (token.type != TOKEN_TYPE::CASETK && token.type != TOKEN_TYPE::LBRACE){

			token = lexer.getToken();
			if (token.type == TOKEN_TYPE::NONE)
				break;
		}
		break;

	case 901: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : the number of args is wrong \n", errnum, errid, CurrentLine); break;	//�������ò�����Ŀ����
	case 902: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : the statement of 'printf' is wrong \n", errnum, errid, CurrentLine); break;
	case 903: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : array overflow \n", errnum, errid, CurrentLine); break;
	case 904: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : 'CONST' can't be changed! \n", errnum, errid, CurrentLine); break;

	case 1000: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : 'CONST' should be ahead of var-definition \n", errnum, errid, CurrentLine); break;
	case 1001: fprintf(errfile, "ErrorNum(%d): Error(%d) , Line(%d) : missing the main program \n", errnum, errid, CurrentLine); break;	//����������
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

//�����ض��塢δ��������
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