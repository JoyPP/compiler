#include "table.h"
#include "global.h"

//定义时加入符号表
bool Table::insertTab(TabEntry entry)
{
	if (!beDef(entry)){
		table.push_back(entry);
		return true;
	}
	else return false;
}

//从尾部向上，直到找到函数名或最开头，		--》 想定义时查看是否已经定义过
bool Table::beDef(TabEntry entry)//if find ,return true
{
	
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if (entry.ekind == ENTRY_KIND::NONRTNFUNCTION || entry.ekind == ENTRY_KIND::RTNFUNCTION){
			if (strcmp(table[i].name, entry.name) == 0)
				return true;	//已经定义过
		}
		else{
			if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
				return false;
			if (_stricmp(table[i].name, entry.name) == 0)
				return true;	//已经定义过
		}
	}
	return false;
}

//使用时查看变量是否已经定义过，防止未定义使用,变量使用
TabEntry Table::canBeUse(char str[])
{
	TabEntry entry;
	entry.ekind = ENTRY_KIND::NOKIND;
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)	//到函数名，则从头开始找
		{
			for (i = 0;; i++){
				if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION){
					return entry;
					}
				if (_stricmp(table[i].name, str) == 0)
					return table[i];
			}
		}
		if (_stricmp(table[i].name, str) == 0)
			return table[i];
	}
	return entry;
}

//C语言不支持重载，找到对应的函数名就返回entry咯,函数调用
TabEntry Table::search(char str[])
{
	TabEntry entry;
	
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if ((table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
			&& (strcmp(table[i].name, str) == 0))
		{
			entry = table[i];	//找到即返回
			return entry;
		}
	}
	
	entry.ekind = ENTRY_KIND::NOKIND;
	return entry;
}

//修改函数size
void Table::Modify(int size)
{
	int i, j;
	int len = table.size();
	
	for (i = table.size() - 1, j = size; ; i--, j--)
	{
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
		{
			table[i].size = size;
			break;
		}
		else table[i].argsnum = j;
	}
}

//返回函数p在符号表中的位置
int Table::getFuncLocation(char str[])
{
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if ((table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
			&& (strcmp(table[i].name, str) == 0))
		{
			return i;	//找到即返回
		}
	}
	return -1;//假的
}

//k是函数名的位置，str是要寻找的变量，返回变量entry
TabEntry Table::getLocalVarLocation(int k, char str[])
{
	int size = table.size();
	TabEntry entry;
	entry.ekind = ENTRY_KIND::NOKIND;
	for (int i = k + 1; i < size; i++)
	{
		if (_stricmp(table[i].name, str) == 0)
			return table[i];
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)	//到函数名，则从头开始找
		{
			break;
		}
	}
	return entry;
}

TabEntry Table::getGlobalVarLocation(char str[])
{
	TabEntry entry;
	entry.ekind = ENTRY_KIND::NOKIND;
	for (int i = 0;; i++)
	{
		if (_stricmp(table[i].name, str) == 0)
			return table[i];
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)	//到函数名，则从头开始找
		{
			break;
		}
	}
	return entry;
}

//获得全局变量
void Table::setGlobalVar()
{
	TabEntry entry;
	for (int i = 0;; i++)
	{
		entry = table[i];
		if (entry.ekind == ENTRY_KIND::RTNFUNCTION || entry.ekind == ENTRY_KIND::NONRTNFUNCTION)
			break;
		if (entry.ekind == ENTRY_KIND::VAR)
		{
			if (entry.etype == ENTRY_TYPE::CHARARRAY || entry.etype == ENTRY_TYPE::INTARRAY)
			{
				fprintf(finalfile, "%s: .space %d\n", entry.name, entry.size * 4);
			}
			else{
				fprintf(finalfile, "%s: .space 4\n", entry.name);
			}
		}
	}
}