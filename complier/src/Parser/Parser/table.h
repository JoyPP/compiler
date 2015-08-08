#pragma once

#include<vector>
#define MAX_STR_LEN 100		//最长字符串长度
#define MAX_QUOTE_COUNT 50 //最大引用次数

#ifndef _TABLE_H_
#define _TABLE_H_

//符号表项的类型
enum ENTRY_TYPE
{
	INT, CHAR, INTARRAY, CHARARRAY, VOID, NOTYPE
};

//符号表项的种类
enum ENTRY_KIND
{
	CONST, VAR, ARGS, RTNFUNCTION, NONRTNFUNCTION, NOKIND
};

//符号表项
typedef struct TabEntry
{
	char name[MAX_STR_LEN];
	ENTRY_KIND ekind;	// 种类const var rtnfunction nonrtnfunction
	ENTRY_TYPE etype;	// 类型int char intarray chararray void
	int declarLine;		// 声明时的行数
	int size;			// 数组的大小，函数形参的个数
	int argsnum;	// 函数变量的形参个数
	int varnum;		// 函数变量的个数
	union Value		// 符号表项的值
	{
		int i;
		char c;
		int intarr[MAX_STR_LEN];
		char chararr[MAX_STR_LEN];
	}value;
}TabEntry;

typedef std::vector<TabEntry> TABLE;	//定义符号表的结构

//符号表
class Table{
	TABLE table;	//符号表
public:
	bool insertTab(TabEntry entry);	// 将entry插入符号表
	bool beDef(TabEntry entry);			//查找符号表看是否已经定义过entry	防止重定义
	TabEntry canBeUse(char str[]);	//查找符号表看是否已经定义过str	防止未声明
	TabEntry search(char str[]);	//查表找调用的标识符和函数
	void Modify(int size);			//修改函数形参个数
	int getFuncLocation(char str[]);	//返回str所在的位置
	TabEntry getLocalVarLocation(int k, char str[]);	//返回局部变量str所在的位置
	TabEntry getGlobalVarLocation(char str[]);	//返回全局变量str的entry
	void setGlobalVar();	//生成汇编时调用，得到所有全局变量
};


#endif