#include "table.h"
#include "global.h"

//����ʱ������ű�
bool Table::insertTab(TabEntry entry)
{
	if (!beDef(entry)){
		table.push_back(entry);
		return true;
	}
	else return false;
}

//��β�����ϣ�ֱ���ҵ����������ͷ��		--�� �붨��ʱ�鿴�Ƿ��Ѿ������
bool Table::beDef(TabEntry entry)//if find ,return true
{
	
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if (entry.ekind == ENTRY_KIND::NONRTNFUNCTION || entry.ekind == ENTRY_KIND::RTNFUNCTION){
			if (strcmp(table[i].name, entry.name) == 0)
				return true;	//�Ѿ������
		}
		else{
			if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
				return false;
			if (_stricmp(table[i].name, entry.name) == 0)
				return true;	//�Ѿ������
		}
	}
	return false;
}

//ʹ��ʱ�鿴�����Ƿ��Ѿ����������ֹδ����ʹ��,����ʹ��
TabEntry Table::canBeUse(char str[])
{
	TabEntry entry;
	entry.ekind = ENTRY_KIND::NOKIND;
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)	//�������������ͷ��ʼ��
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

//C���Բ�֧�����أ��ҵ���Ӧ�ĺ������ͷ���entry��,��������
TabEntry Table::search(char str[])
{
	TabEntry entry;
	
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if ((table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
			&& (strcmp(table[i].name, str) == 0))
		{
			entry = table[i];	//�ҵ�������
			return entry;
		}
	}
	
	entry.ekind = ENTRY_KIND::NOKIND;
	return entry;
}

//�޸ĺ���size
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

//���غ���p�ڷ��ű��е�λ��
int Table::getFuncLocation(char str[])
{
	for (int i = table.size() - 1; i >= 0; i--)
	{
		if ((table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)
			&& (strcmp(table[i].name, str) == 0))
		{
			return i;	//�ҵ�������
		}
	}
	return -1;//�ٵ�
}

//k�Ǻ�������λ�ã�str��ҪѰ�ҵı��������ر���entry
TabEntry Table::getLocalVarLocation(int k, char str[])
{
	int size = table.size();
	TabEntry entry;
	entry.ekind = ENTRY_KIND::NOKIND;
	for (int i = k + 1; i < size; i++)
	{
		if (_stricmp(table[i].name, str) == 0)
			return table[i];
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)	//�������������ͷ��ʼ��
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
		if (table[i].ekind == ENTRY_KIND::RTNFUNCTION || table[i].ekind == ENTRY_KIND::NONRTNFUNCTION)	//�������������ͷ��ʼ��
		{
			break;
		}
	}
	return entry;
}

//���ȫ�ֱ���
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