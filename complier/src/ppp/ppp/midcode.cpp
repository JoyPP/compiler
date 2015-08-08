#define _CRT_SECURE_NO_WARNINGS
#include "midcode.h"


void mid(char result[])
{
	strcpy(midcode[midcodeline].result, result);
	strcpy(midcode[midcodeline].arg1, "");
	strcpy(midcode[midcodeline].op, "");
	strcpy(midcode[midcodeline].arg2, "");
	midcode[midcodeline].block = blocknum;
	midcodeline++;
	
}

void mid(char result[], char arg1[])
{
	strcpy(midcode[midcodeline].result, result);
	strcpy(midcode[midcodeline].arg1, arg1);
	strcpy(midcode[midcodeline].op, "");
	strcpy(midcode[midcodeline].arg2, "");
	midcode[midcodeline].block = blocknum;
	midcodeline++;
}

void mid(char result[], char arg1[], char op[])
{
	strcpy(midcode[midcodeline].result, result);
	strcpy(midcode[midcodeline].arg1, arg1);
	strcpy(midcode[midcodeline].op, op);
	strcpy(midcode[midcodeline].arg2, "");
	midcode[midcodeline].block = blocknum;
	midcodeline++;
}

void mid(char result[], char arg1[], char op[], char arg2[])
{
	strcpy(midcode[midcodeline].result, result);
	strcpy(midcode[midcodeline].arg1, arg1);
	strcpy(midcode[midcodeline].op, op);
	strcpy(midcode[midcodeline].arg2, arg2);
	midcode[midcodeline].block = blocknum;
	midcodeline++;
}


void middle1(char result[], char arg1[], char op[], char arg2[])
{
	strcpy(midcode1[midcodeline1].result, result);
	strcpy(midcode1[midcodeline1].arg1, arg1);
	strcpy(midcode1[midcodeline1].op, op);
	strcpy(midcode1[midcodeline1].arg2, arg2);
	midcodeline1++;
}

void middle2(char result[], char arg1[], char op[], char arg2[])
{
	strcpy(midcode2[midcodeline2].result, result);
	strcpy(midcode2[midcodeline2].arg1, arg1);
	strcpy(midcode2[midcodeline2].op, op);
	strcpy(midcode2[midcodeline2].arg2, arg2);
	midcodeline2++;
}

void CreateMidtmp1()
{
	MidCode m;
	for (int i = 0;; i++){
		if (midtmpvar[i].type != TEMP_TYPE::NOTTYPE){
			midtmpvar[i].times = 0;
		}
		else break;
	}
	for (int i = 0;; i++){
		m = midcode1[i];
		if (strcmp(m.result, "") == 0)
			break;
		if (m.result[0] == '*')
		{
			midtmpvar[getarrnum(m.result)].times++;
		}
		if (m.arg1[0] == '$' && m.arg1[1] == 't')
		{
			midtmpvar[getnum(m.arg1)].times++;
		}
		if (m.op[0] == '$' && m.op[1] == 't')
		{
			midtmpvar[getnum(m.op)].times++;
		}
		if (m.arg2[0] == '$' && m.arg2[1] == 't')
		{
			midtmpvar[getnum(m.arg2)].times++;
		}
	}
}

void CreateMidtmp2()
{
	MidCode m;
	for (int i = 0;; i++){
		if (midtmpvar[i].type != TEMP_TYPE::NOTTYPE){
			midtmpvar[i].times = 0;
		}
		else break;
	}
	for (int i = 0;; i++){
		m = midcode2[i];
		if (strcmp(m.result, "") == 0)
			break;
		if (m.result[0] == '*')
		{
			midtmpvar[getarrnum(m.result)].times++;
		}
		if (m.arg1[0] == '$' && m.arg1[1] == 't')
		{
			midtmpvar[getnum(m.arg1)].times++;
		}
		if (m.op[0] == '$' && m.op[1] == 't')
		{
			midtmpvar[getnum(m.op)].times++;
		}
		if (m.arg2[0] == '$' && m.arg2[1] == 't')
		{
			midtmpvar[getnum(m.arg2)].times++;
		}
		
	}
}

//返回四元式中临时寄存器p = $tx的数字x
int getnum(char p[])
{
	int num, len = strlen(p);
	int i, j;
	char tp[10], q[10];
	sprintf(q, "%s", p);
	for (i = 2, j = 0; i < len; i++, j++)
	{
		tp[j] = p[i];
	}
	tp[j] = '\0';
	num = atoi(tp);
	return num;
}

//返回四元式中临时寄存器p = *($tx)的数字x
int getarrnum(char p[])
{
	int num, len = strlen(p);
	int i, j;
	char tp[10], q[10];
	sprintf(q, "%s", p);
	for (i = 4, j = 0; i < len - 1; i++, j++)
	{
		tp[j] = q[i];
	}
	tp[j] = '\0';
	num = atoi(tp);
	return num;
}
