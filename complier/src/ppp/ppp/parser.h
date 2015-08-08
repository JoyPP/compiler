#pragma once

//#define _CRT_SECURE_NO_WARNINGS
#include "global.h"
#include "lexer.h"

#ifndef _PARSER_H_
#define _PARSER_H_


//�﷨������
class Parser	
{
public:
	TOKEN token;	//��ǰtoken
	Lexer lexer;	//���ôʷ�����

	void error(int errid);				//������
	void error(int errid, char str[]);	//������
	bool Program();				//����
	bool ConstDeclaration();	//��������
	bool ConstDefinition();		//��������
	bool VarDeclaration(int *num);		//��������
	bool VarDefinition(int *num, int *varnum);		//��������
	bool RtnFuncDefinition();		//�з���ֵ��������
	bool NonRtnFuncDefinition();	//�޷���ֵ��������
	bool MainProgram();				//������
	bool Args(int *size);				//����
	bool CompoundState(int *num);		//�������
	bool StateList();			//�����
	bool Statement();			//���
	bool IfStatement();			//�������
	bool Condition(char *r, int iflabel);	//����
	bool WhileStatement();				//ѭ�����
	bool CallRtnFunc(char *r);			//�з���ֵ�����������
	bool CallNonRtnFunc(char *r);		//�޷���ֵ�����������
	bool ParameterList(int *size);		//ֵ������
	bool AssignStatement();		//��ֵ���
	bool ReadStatement();		//�����
	bool WriteStatement();		//д���
	bool SwitchStatement();		//������
	bool ReturnStatement();		//�������

	bool Constant(char *con);	//����
	bool Integer(int *num);		//����
	bool Expression(char *r, int *arr);	//���ʽ
	bool Term(char *r, int *arr);		//��
	bool Factor(char *r, int *arr);		//����
	bool CaseList(char *p, int *switchlabel);	//�����

	void genVardef(TabEntry entry);
	void genVar(char *r, int i, bool flag);				// r = i	flagΪ�棬��i���������������ַ���ASCII��
	void genVar(char *r, char c);				// r = c
	void genALU(char *r, char op, char *p);		// r = op p
	void genALU(char *r, char *p, bool arr);				// r = p
	void genALU(char *r, char *p, char op, char *q, TEMP_TYPE ttype);	// r = p op q	ttype��ʾ�Ƿ����ַ�������
	void genLabel(char str[]);			//���ɺ�����ǩ	_func:
	void genLabel(int i);				//���ɱ�ǩ		_Lx:
	void genGOTOLabel(int i);			//��������ת		GOTO _Lx
	void genGOTOLabel(char str[]);		//��������ת		GOTO str
	void genCall(char str[]);					//�����޷���ֵ�ĺ���		LCALL _func
	void genCall(char *r, char str[], ENTRY_TYPE etype);			//�����з���ֵ�ĺ���		_tx = LCALL _func
	void genAssign(char str1[], char str2[], bool arr);	//�����ģ���ֵ���		str1 = str2  or *(str1) = str2
	void genRtn(char *p);				//�������	Return _tx
	void genCondition(char *r, char *p, int iflabel);	//	<����> = <���ʽ>	IFZ %s GOTO _L%d
	void genCondition(char *r, char *p, char *op, char *q, int iflabel);	//<����> = <���ʽ> <��ϵ�����> <���ʽ>
	void genCondition(char *r, char *p, char *op, char *q, char str[]);
	void genPush(std::string p);	//��ջ
	void genPop(int size);	//��ջ
	void genPrintf(char *p);//���
	void genScanf(char *p);	//����
	void genBeginFunc();	//��ʼ����
	void genEndFunc();		//��������
	void genExit();			//�˳�����

	int getnum(char str[]);
	int getarrnum(char str[]);
};

#endif