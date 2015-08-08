#pragma once

#include<vector>
#define MAX_STR_LEN 100		//��ַ�������
#define MAX_QUOTE_COUNT 50 //������ô���

#ifndef _TABLE_H_
#define _TABLE_H_

//���ű��������
enum ENTRY_TYPE
{
	INT, CHAR, INTARRAY, CHARARRAY, VOID, NOTYPE
};

//���ű��������
enum ENTRY_KIND
{
	CONST, VAR, ARGS, RTNFUNCTION, NONRTNFUNCTION, NOKIND
};

//���ű���
typedef struct TabEntry
{
	char name[MAX_STR_LEN];
	ENTRY_KIND ekind;	// ����const var rtnfunction nonrtnfunction
	ENTRY_TYPE etype;	// ����int char intarray chararray void
	int declarLine;		// ����ʱ������
	int size;			// ����Ĵ�С�������βεĸ���
	int argsnum;	// �����������βθ���
	int varnum;		// ���������ĸ���
	union Value		// ���ű����ֵ
	{
		int i;
		char c;
		int intarr[MAX_STR_LEN];
		char chararr[MAX_STR_LEN];
	}value;
}TabEntry;

typedef std::vector<TabEntry> TABLE;	//������ű�Ľṹ

//���ű�
class Table{
	TABLE table;	//���ű�
public:
	bool insertTab(TabEntry entry);	// ��entry������ű�
	bool beDef(TabEntry entry);			//���ҷ��ű��Ƿ��Ѿ������entry	��ֹ�ض���
	TabEntry canBeUse(char str[]);	//���ҷ��ű��Ƿ��Ѿ������str	��ֹδ����
	TabEntry search(char str[]);	//����ҵ��õı�ʶ���ͺ���
	void Modify(int size);			//�޸ĺ����βθ���
	int getFuncLocation(char str[]);	//����str���ڵ�λ��
	TabEntry getLocalVarLocation(int k, char str[]);	//���ؾֲ�����str���ڵ�λ��
	TabEntry getGlobalVarLocation(char str[]);	//����ȫ�ֱ���str��entry
	void setGlobalVar();	//���ɻ��ʱ���ã��õ�����ȫ�ֱ���
};


#endif