#pragma once

//#define _CRT_SECURE_NO_WARNINGS
#include "global.h"
#include "lexer.h"

#ifndef _PARSER_H_
#define _PARSER_H_


//语法分析类
class Parser	
{
public:
	TOKEN token;	//当前token
	Lexer lexer;	//调用词法分析

	void error(int errid);				//错误处理
	void error(int errid, char str[]);	//错误处理
	bool Program();				//程序
	bool ConstDeclaration();	//常量声明
	bool ConstDefinition();		//常量定义
	bool VarDeclaration(int *num);		//变量声明
	bool VarDefinition(int *num, int *varnum);		//变量定义
	bool RtnFuncDefinition();		//有返回值函数定义
	bool NonRtnFuncDefinition();	//无返回值函数定义
	bool MainProgram();				//主函数
	bool Args(int *size);				//参数
	bool CompoundState(int *num);		//复合语句
	bool StateList();			//语句列
	bool Statement();			//语句
	bool IfStatement();			//条件语句
	bool Condition(char *r, int iflabel);	//条件
	bool WhileStatement();				//循环语句
	bool CallRtnFunc(char *r);			//有返回值函数调用语句
	bool CallNonRtnFunc(char *r);		//无返回值函数调用语句
	bool ParameterList(int *size);		//值参数列
	bool AssignStatement();		//赋值语句
	bool ReadStatement();		//读语句
	bool WriteStatement();		//写语句
	bool SwitchStatement();		//情况语句
	bool ReturnStatement();		//返回语句

	bool Constant(char *con);	//常量
	bool Integer(int *num);		//整数
	bool Expression(char *r, int *arr);	//表达式
	bool Term(char *r, int *arr);		//项
	bool Factor(char *r, int *arr);		//因子
	bool CaseList(char *p, int *switchlabel);	//情况列

	void genVardef(TabEntry entry);
	void genVar(char *r, int i, bool flag);				// r = i	flag为真，则i是整数，否则是字符的ASCII码
	void genVar(char *r, char c);				// r = c
	void genALU(char *r, char op, char *p);		// r = op p
	void genALU(char *r, char *p, bool arr);				// r = p
	void genALU(char *r, char *p, char op, char *q, TEMP_TYPE ttype);	// r = p op q	ttype表示是否是字符串运算
	void genLabel(char str[]);			//生成函数标签	_func:
	void genLabel(int i);				//生成标签		_Lx:
	void genGOTOLabel(int i);			//无条件跳转		GOTO _Lx
	void genGOTOLabel(char str[]);		//无条件跳转		GOTO str
	void genCall(char str[]);					//调用无返回值的函数		LCALL _func
	void genCall(char *r, char str[], ENTRY_TYPE etype);			//调用有返回值的函数		_tx = LCALL _func
	void genAssign(char str1[], char str2[], bool arr);	//（最后的）赋值语句		str1 = str2  or *(str1) = str2
	void genRtn(char *p);				//返回语句	Return _tx
	void genCondition(char *r, char *p, int iflabel);	//	<条件> = <表达式>	IFZ %s GOTO _L%d
	void genCondition(char *r, char *p, char *op, char *q, int iflabel);	//<条件> = <表达式> <关系运算符> <表达式>
	void genCondition(char *r, char *p, char *op, char *q, char str[]);
	void genPush(std::string p);	//入栈
	void genPop(int size);	//出栈
	void genPrintf(char *p);//输出
	void genScanf(char *p);	//输入
	void genBeginFunc();	//开始函数
	void genEndFunc();		//函数结束
	void genExit();			//退出程序

	int getnum(char str[]);
	int getarrnum(char str[]);
};

#endif