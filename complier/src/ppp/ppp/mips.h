
#include "global.h"
#include "parser.h"

#ifndef _MIPS_H_
#define _MIPS_H_

//生成mips汇编
class MIPS
{
public:
	Parser parser;	//调用语法分析
	MidCode m;	//中间代码m
	int t[10];	//register t0 - t9   1 : invalid  0 : valid
	int i;		//当前读到四元式的标号
	int regnum;	//临时寄存器t，初始化为-1
	int sregnum;	//临时寄存器s，初始化为-1
	int nowfuncflag;	//当前所在函数在符号表中的位置
	int ValidRegister();	//取可用t寄存器的标号
	int ValidSRegister();	//取可用s寄存器的标号

	MIPS();	//构造函数

	void EmitTotal(int size);	//遍历四元式，生成mips汇编

	void EmitLoadConstant();	//常量赋值汇编生成
	void EmitLoadString();		//字符串赋值汇编生成
	void EmitBeginFunc();		//函数开始标志汇编生成
	void EmitPushStack();		//入栈汇编生成
	void EmitCallNonRtnFunc();	//调用无返回值函数汇编生成
	void EmitCallRtnFunc();		//调用有返回值函数汇编生成
	void EmitPopStack();		//出栈汇编生成
	void EmitIFZ();				//if判断语句汇编生成
	void EmitGoto();			//无条件跳转汇编生成
	void EmitReturn();			//返回语句汇编生成
	void EmitEndFunc();			//函数结束标志汇编生成
	void EmitEndProgram();		//程序结束标志汇编生成
	void EmitLabel();			//函数标签、跳转标签汇编生成
	void EmitPrintf();			//输出函数汇编生成
	void EmitScanf();			//输入函数汇编生成
	void EmitIDENAssign();		//标识符赋值汇编生成
	void EmitArrayOp();			//数组操作汇编生成
	
	void EmitAdd();		//加法汇编生成
	void EmitSub();		//减法汇编生成
	void EmitMultDiv();		//乘除法汇编生成
	void EmitComparison();	//比较运算汇编生成

	void storereg(int t);	//存寄存器t被使用
	int getnum(char p[]);	//取得形如$tx的临时寄存器的值x
	int getarrnum(char p[]);//取得形如*($tx)的临时寄存器的值x
	void MarkValidReg(int reg);//释放寄存器t(reg)使之可以被再次使用
	void MarkInvalidReg(int reg);//使用寄存器t(reg)，标记其为不可使用
};






#endif