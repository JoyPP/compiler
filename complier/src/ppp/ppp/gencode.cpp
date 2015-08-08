#define _CRT_SECURE_NO_WARNINGS
#include "global.h"
#include "parser.h"

void Parser::genVardef(TabEntry entry)
{
	char temp[MAX_STR_LEN];
	if (entry.etype == ENTRY_TYPE::CHAR || entry.etype == ENTRY_TYPE::INT)
	{
		mid("VAR", entry.name, "", "");
	}
	else {
		sprintf(temp, "%s%c%d%c", entry.name, '[', entry.size, ']');
		mid("VAR", temp, "", "");
	}
}


//变量赋值	$tx = i
void Parser::genVar(char *r, int i, bool flag)
{
	char temp[MAX_STR_LEN], tpi[16];
	sprintf(r, "%s%d", "$t", tempnum);
	if (flag)
		midtmpvar[tempnum++].type = TEMP_TYPE::INTT;
	else midtmpvar[tempnum++].type = TEMP_TYPE::CHARR;
	sprintf(temp, "%s%c%d", r, '=', i);
	sprintf(tpi, "%d", i);
	mid(r, tpi, "", "");
}

//变量赋值	$tx = c
void Parser::genVar(char *r, char c)
{
	char temp[MAX_STR_LEN], tpc[5];
	sprintf(r, "%s%d", "$t", tempnum);
	midtmpvar[tempnum++].type = TEMP_TYPE::CHARR;
	sprintf(temp, "%s%c%c%c", r, '=', c, '\0');
	sprintf(tpc, "%c%c%c", '\'', c, '\'');
	mid(r, tpc, "", "");
}

//生成函数标签Label
void Parser::genLabel(char str[])
{
	char temp[MAX_STR_LEN];
	sprintf(temp, "%c%s%c", '_', str, ':');
	blocknum++;
	mid(temp, "", "", "");
}

//生成普通标签Label
void Parser::genLabel(int i)
{
	char temp[MAX_STR_LEN];
	sprintf(temp, "%s%d%c", "$L", i, ':');
	blocknum++;
	mid(temp, "", "", "");
}

//无条件转移标签Label	GOTO $Lx
void Parser::genGOTOLabel(int i)
{
	char temp[MAX_STR_LEN], tpl[6];
	sprintf(temp, "%s%d", "GOLO $L", i);
	sprintf(tpl, "%s%d", "$L", i);
	mid("GOTO", tpl, "", "");
	blocknum++;
}

//无条件转移标签Label	GOTO str
void Parser::genGOTOLabel(char str[])
{
	char temp[MAX_STR_LEN];
	sprintf(temp, "%s", str);
	mid("GOTO", temp, "", "");
	blocknum++;
}

//调用无返回值函数
void Parser::genCall(char str[])
{
	char temp[MAX_STR_LEN], tps[MAX_STR_LEN];
	sprintf(temp, "%s%s", "LCALL _", str);
	sprintf(tps, "%c%s", '_', str);
	mid("LCALL", tps, "", "");
	blocknum++;
}

//调用函数	$tx = LCall _func
void Parser::genCall(char *r, char str[], ENTRY_TYPE etype)
{
	char temp[MAX_STR_LEN], tps[MAX_STR_LEN];
	sprintf(r, "%s%d", "$t", tempnum);

	if (etype == ENTRY_TYPE::INT)
		midtmpvar[tempnum++].type = TEMP_TYPE::INTT;
	else if (etype == ENTRY_TYPE::CHAR)
		midtmpvar[tempnum++].type = TEMP_TYPE::CHARR;

	sprintf(temp, "%s%c%s%s", r, '=', "LCALL _", str);
	sprintf(tps, "%c%s", '_', str);
	mid(r, "LCALL", tps, "");
	blocknum++;
}

// r = p op q
void Parser::genALU(char *r, char *p, char op, char *q, TEMP_TYPE ttype)
{
	char temp[MAX_STR_LEN], tpop[3];
	sprintf(r, "%s%d", "$t", tempnum);

	midtmpvar[tempnum++].type = ttype;
	if (p[0] == '$')
		midtmpvar[getnum(p)].times++;
	if (q[0] == '$')
		midtmpvar[getnum(q)].times++;

	sprintf(temp, "%s%c%s%c%s", r, '=', p, op, q);
	sprintf(tpop, "%c", op);
	mid(r, p, tpop, q);
}

// r = p	$t = $str   $t = *($t)
void Parser::genALU(char *r, char *p, bool arr)
{
	char temp[MAX_STR_LEN], tpp[MAX_STR_LEN], wtf[6];
	sprintf(r, "%s%d", "$t", tempnum);
	
	if (arr){
		int len = strlen(p);
		sprintf(temp, "%s", p);
		for (int k = 2, m = 0; k < len; m++, k++)
		{
			wtf[m] = temp[k];
		}
		int num = atoi(wtf);
		midtmpvar[tempnum++].type = midtmpvar[num].type;
		if (p[0] == '$')
			midtmpvar[getnum(p)].times++;

		sprintf(temp, "%s%c%s%s%c", r, '=', "*(", p, ')');
		sprintf(tpp, "%s%s%c", "*(", p, ')');
		mid(r, tpp, "", "");
	}
	else{	//string

		midtmpvar[tempnum].type = TEMP_TYPE::STRCONN;
		midtmpvar[tempnum++].regnum = strnum;
		allstring[strnum] = p;
		//p是字符串
		sprintf(temp, "%s%d", "$str", strnum++);
		mid(r, temp, "", "");
	}
	//printf("\t%s\n", temp);
}

// r = op p
void Parser::genALU(char *r, char op, char *p)
{
	char temp[MAX_STR_LEN], tpop[3];
	sprintf(r, "%s%d", "$t", tempnum);

	midtmpvar[tempnum++].type = TEMP_TYPE::INTT;
	if (p[0] == '$')
		midtmpvar[getnum(p)].times++;
	
	sprintf(temp, "%s%c%c%s", r, '=', op, p);
	sprintf(tpop, "%c", op);
	mid(r, "0\0", tpop, p);
	//printf("\t%s\n", temp);
}

//赋值语句 str1 = str2  or  *(str1) = str2	
void Parser::genAssign(char str1[], char str2[], bool arr)
{
	char tp1[MAX_STR_LEN];
	if (arr){
		sprintf(tp1, "%s%s%c", "*(", str1, ')');
		mid(tp1, str2, "", "");
		//printf("\t%s%c%s\n", tp1, '=', str2);
	}
	else{
		mid(str1, str2, "", "");
		//printf("\t%s%c%s\n", str1, '=', str2);
	}
	if (str1[0] == '$' && getnum(str1) < tempnum)
		midtmpvar[getnum(str1)].times++;
	if (str2[0] == '$')
		midtmpvar[getnum(str2)].times++;
}

//Return $tx
void Parser::genRtn(char str[])
{
	char temp[MAX_STR_LEN];
	sprintf(temp, "%s%s", "Return ", str);
	if (str[0] == '$')
		midtmpvar[getnum(str)].times++;

	mid("Return", str, "", "");
}

//<条件> = <表达式>
void Parser::genCondition(char *r, char *p, int iflabel)
{
	char tpl[10];
	sprintf(r, "%s", p);
	sprintf(tpl, "%s%d", "$L", iflabel);
	mid("IFZ", r, "GOTO", tpl);
	blocknum++;
	if (r[0] == '$')
		midtmpvar[getnum(r)].times++;
}

//<条件> = <表达式> <关系运算符> <表达式>
void Parser::genCondition(char *r, char *p, char *op, char *q, int iflabel)
{
	char temp[MAX_STR_LEN], tpl[10];
	sprintf(r, "%s%d", "$t", tempnum);

	midtmpvar[tempnum++].type = TEMP_TYPE::INTT;
	if (p[0] == '$')
		midtmpvar[getnum(p)].times++;
	if (q[0] == '$')
		midtmpvar[getnum(q)].times++;

	sprintf(temp, "%s%c%s%s%s", r, '=', p,op,q);
	mid(r, p, op, q);

	sprintf(tpl, "%s%d", "$L", iflabel);
	mid("IFZ", r, "GOTO", tpl);
	blocknum++;
	if (r[0] == '$')
		midtmpvar[getnum(r)].times++;
}

void Parser::genCondition(char *r, char *p, char *op, char *q, char str[])
{
	char temp[MAX_STR_LEN], tpl[10];
	sprintf(r, "%s%d", "$t", tempnum);

	midtmpvar[tempnum++].type = TEMP_TYPE::INTT;
	if (p[0] == '$')
		midtmpvar[getnum(p)].times++;
	if (q[0] == '$')
		midtmpvar[getnum(q)].times++;

	sprintf(temp, "%s%c%s%s%s", r, '=', p, op, q);
	mid(r, p, op, q);

	sprintf(tpl, "%s", str);
	mid("IFZ", r, "GOTO", tpl);
	blocknum++;
	if (r[0] == '$')
		midtmpvar[getnum(r)].times++;
}

//入栈
void Parser::genPush(std::string p)
{
	char tmp[MAX_STR_LEN];
	mystack.push(p);
	strcpy(tmp, p.c_str());

	if (tmp[0] == '$'){
		midtmpvar[getnum(tmp)].times++;
		mid("PushStack", tmp, "", "");
	}
	else
		mid("PushStack", tmp, "", "");
}

//出栈
void Parser::genPop(int size)
{
	char tmp[MAX_STR_LEN];
	for (int i = 0; i < size; i++)
	{
		mystack.pop();
	}
	sprintf(tmp, "%d", size * 4);
	mid("PopStack", tmp, "", "");
}

void Parser::genPrintf(char *p)
{
	if (p[0] == '$'){
		midtmpvar[getnum(p)].times++;
		mid("Printf", p, "", "");
	}
	else {
		mid("Printf", p, "", "");
	}
}

void Parser::genScanf(char *p)
{
	mid("Scanf", p, "", "");
	if (p[0] == '$')
		midtmpvar[getnum(p)].times++;
}

void Parser::genBeginFunc()
{
	mid("BeginFunc", "", "", "");
}

void Parser::genEndFunc()
{
	mid("EndFunc", "", "", "");
}

void Parser::genExit()
{
	mid("EndProgram", "", "", "");
}


//返回四元式中临时寄存器p = $tx的数字x
int Parser::getnum(char p[])
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
int Parser::getarrnum(char p[])
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
