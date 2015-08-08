#define _CRT_SECURE_NO_WARNINGS
#include "parser.h"
#include "global.h"

//����
bool Parser::Program()
{
	bool flag = true;
	token = lexer.getToken();
	if (token.type == TOKEN_TYPE::CONSTTK)		//��������
	{ 
		if (!ConstDeclaration()){ flag = false; }
	}
	if (token.type == TOKEN_TYPE::INTTK || token.type == TOKEN_TYPE::CHARTK || token.type == TOKEN_TYPE::VOIDTK)	//����������������
	{
		TOKEN lkahead1;
		TOKEN lkahead2;
		lkahead1 = lexer.LookAhead(1);
		lkahead2 = lexer.LookAhead(2);
		if (lkahead2.type != TOKEN_TYPE::LPARENT && token.type != TOKEN_TYPE::VOIDTK)	//�������� int | char (<��ʶ��>|<��ʶ��>[�޷�������])
		{
			int num = 0;
			if (!VarDeclaration(&num)){ flag = false; }
		}

		lkahead1 = lexer.LookAhead(1);
		lkahead2 = lexer.LookAhead(2);

		while (lkahead1.type != TOKEN_TYPE::MAINTK && lkahead2.type == TOKEN_TYPE::LPARENT)
		{
			if (lkahead1.type == TOKEN_TYPE::IDEN)	//������һ����IDEN
			{
				if (token.type == TOKEN_TYPE::INTTK || token.type == TOKEN_TYPE::CHARTK)	//�з���ֵ�������� int | char function(){}
				{
				if (!RtnFuncDefinition()){ flag = false; }
				}
				else if (token.type == TOKEN_TYPE::VOIDTK)
				{
					if (!NonRtnFuncDefinition()){ flag = false; }	//�޷���ֵ�������� void function(){}
				}
				else {
					flag = false;
				}
				lkahead1 = lexer.LookAhead(1);
				lkahead2 = lexer.LookAhead(2);
			}
			else {
				while (!((token.type == TOKEN_TYPE::INTTK || token.type == TOKEN_TYPE::CHARTK || token.type == TOKEN_TYPE::VOIDTK) &&
					(lkahead1.type == TOKEN_TYPE::IDEN || lkahead1.type == TOKEN_TYPE::MAINTK) && lkahead2.type == TOKEN_TYPE::LPARENT))
				{
					token = lexer.getToken();
					lkahead1 = lexer.LookAhead(1);
					lkahead2 = lexer.LookAhead(2);
				}
			}
		}

		if (token.type == TOKEN_TYPE::CONSTTK){ error(1000); flag = false; }

		if (token.type == TOKEN_TYPE::VOIDTK && lkahead1.type == TOKEN_TYPE::MAINTK && lkahead2.type == TOKEN_TYPE::LPARENT)
		{
			if (!MainProgram()){ flag = false; }
		}
		else { error(1001); flag = false; }

		lkahead1 = lexer.LookAhead(1);
		lkahead2 = lexer.LookAhead(2);
		if (token.type == TOKEN_TYPE::VOIDTK && lkahead1.type == TOKEN_TYPE::MAINTK && lkahead2.type == TOKEN_TYPE::LPARENT)
		{
			error(1002); flag = false;
		}

		if (token.type != TOKEN_TYPE::NONE){ error(1003); flag = false; }

	}
	else { error(1001); flag = false; }

	return flag;
}

//��������  
bool Parser::ConstDeclaration()
{
	bool flag = true;
	if (token.type != TOKEN_TYPE::CONSTTK){ flag = false; } //�����ܷ�����_��
	do{
		token = lexer.getToken();
		if (!ConstDefinition()){ flag = false; }
		if (token.type != TOKEN_TYPE::SEMICN){ error(101); flag = false; }	//missing ';' , ������ֱ���ҵ���һ����;��
		token = lexer.getToken();
	} while (token.type == TOKEN_TYPE::CONSTTK);
	return flag;
}

//��������	 ��name��value����table��		
bool Parser::ConstDefinition()
{
	TabEntry entry;
	bool flag = true;

	if ((token.type != TOKEN_TYPE::INTTK) && (token.type != TOKEN_TYPE::CHARTK)){ error(201); return false; }	//missing int or char
	entry.etype = (token.type == TOKEN_TYPE::INTTK) ? ENTRY_TYPE::INT :
					(token.type == TOKEN_TYPE::CHARTK) ? ENTRY_TYPE::CHAR : ENTRY_TYPE::NOTYPE;
	entry.ekind = ENTRY_KIND::CONST;
	
	if (token.type == TOKEN_TYPE::INTTK)
	{
		do{
			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::IDEN){ error(301); return false; } //���˱�ʶ��,�ң�

			strcpy_s(entry.name, token.value.str);
			entry.declarLine = CurrentLine;

			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::ASSIGN){ error(501); return false; }	//= missing'=' ����next ';'

			token = lexer.getToken();
			int num;
			if (!Integer(&num)){ return false; } //����	 �����return false �� flag = falseһ��Ч��
			else entry.value.i = num;

			if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }	//���ű����Ѿ����ڱ�ʶ��,redefinetion

		} while (token.type == TOKEN_TYPE::COMMA);
	}
	else if (token.type == TOKEN_TYPE::CHARTK)
	{
		do{
			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::IDEN){ error(301); return false; }//��ʶ��

			strcpy_s(entry.name, token.value.str);
			entry.declarLine = CurrentLine;

			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::ASSIGN){ error(501); return false; }	//= missing'=' ����next';'

			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::CHARCON){ error(602); return false; } //�ַ� missing a char����next ';'
			entry.value.c = token.value.c;

			if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }	//���ű����Ѿ����ڱ�ʶ��

			token = lexer.getToken();
		} while (token.type == TOKEN_TYPE::COMMA);
	} 
	return flag;
}

//��������   
bool Parser::VarDeclaration(int *num)
{
	bool flag = true;
	TOKEN lkahead;
	int varnum = 0;
	do{
		if (!VarDefinition(num,&varnum)){ flag = false; }
		if (token.type == TOKEN_TYPE::ASSIGN){ error(1100); flag = false; }
		if (token.type != TOKEN_TYPE::SEMICN){ error(101); flag = false; }	//missing a ';' , find next ';'

		token = lexer.getToken();
		if (token.type == TOKEN_TYPE::INTTK || token.type == TOKEN_TYPE::CHARTK)
		{
			lkahead = lexer.LookAhead(2);
		}
		else return flag;	//һ�����Ǳ���������
	} while (lkahead.type != TOKEN_TYPE::LPARENT);	//�����з���ֵ��������
	return flag;
}

//��������	 ��name��size����table��
bool Parser::VarDefinition(int *num, int *varnum)
{
	TabEntry entry;
	TOKEN lookahead;
	bool flag = true;
	
	if ((token.type != TOKEN_TYPE::INTTK) && (token.type != TOKEN_TYPE::CHARTK)){ error(201); return false; }	//��int char
	entry.ekind = ENTRY_KIND::VAR;
	entry.etype = (token.type == TOKEN_TYPE::INTTK) ? ENTRY_TYPE::INT : ENTRY_TYPE::CHAR;

	do{
		token = lexer.getToken();
		if (token.type != TOKEN_TYPE::IDEN){ error(301); return false; }	//��ʶ��

		strcpy_s(entry.name, token.value.str);
		entry.declarLine = CurrentLine;
		//��ǰ�������Ƿ�������
		if ((lookahead = lexer.LookAhead(1)).type == TOKEN_TYPE::LBRACK)	// [ ,������
		{
			token = lexer.getToken();	// [
			entry.etype = (entry.etype == ENTRY_TYPE::INT) ? ENTRY_TYPE::INTARRAY : ENTRY_TYPE::CHARARRAY;	//�����Ӧ����������

			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::INTCON || (token.type == TOKEN_TYPE::INTCON && token.value.i == 0)){	//�����޷�������
				error(603); flag = false;
				if (token.type != TOKEN_TYPE::COMMA)	//���Ƕ���
					break;			//����ѭ������һ��<��������>�Ѿ����
				else continue;		//�Ƕ��ţ���һѭ��
			} //�޷�������
			else entry.size = token.value.i;	//�ַ�������
			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::RBRACK){		// ]
				error(70201); flag = false;
				if (token.type != TOKEN_TYPE::COMMA)	//���Ƕ���
					break;			//����ѭ������һ��<��������>�Ѿ����
				else continue;		//�Ƕ��ţ���һѭ��
			}
			entry.varnum = (*num) + 1;
			(*num)+=entry.size;
		}
		else {
			entry.varnum = (*num) + 1;
			(*num)++;
			
		}
		if (flag){
			(*varnum)++;
			//entry.varnum = (*num);
			if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }	//insert into table
			if (flag)
			{
				genVardef(entry);
			}
		}
		
		token = lexer.getToken();
	} while (token.type == TOKEN_TYPE::COMMA);

	return flag;
}

//�з���ֵ��������	���ɺ�����ǩLable	����������������entry.name����������int|char����table
bool Parser::RtnFuncDefinition()
{
	TabEntry entry;
	bool flag = true;
	int localnum = 0;
	int rtnflag = 1;

	entry.ekind = ENTRY_KIND::RTNFUNCTION;	//����
	entry.etype = (token.type == TOKEN_TYPE::INTTK) ? ENTRY_TYPE::INT : ENTRY_TYPE::CHAR;	//����ֵ����

	token = lexer.getToken();	//һ���Ǳ�ʶ����������

	strcpy_s(entry.name, token.value.str);
	entry.declarLine = CurrentLine;
	entry.size = 0;

	if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }	//���ű����Ѿ����ڱ�ʶ��

	genLabel(entry.name);
	int line = midcodeline;//�޸ĵ�����
	int temp = tempnum;//��ǰ��ʱ�Ĵ������
	genBeginFunc();

	//(����)
	token = lexer.getToken();	//һ����'��'

	int size = 0;
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::RPARENT){
		if (!Args(&size)){ flag = false; }	//����		
		if (token.type != TOKEN_TYPE::RPARENT){ error(70401); flag = false; }	//missing a ')',find next ')' or '{'
	}

	tab.Modify(size);

	//{�������}
	token = lexer.getToken();	//�Ѿ���������������һ��{
	if (token.type != TOKEN_TYPE::LBRACE){ error(705); flag = false; }
	else token = lexer.getToken();	//��{���ȡ��һ����ȱ����ֱ���ж�
	if (token.type != TOKEN_TYPE::RBRACE){	//��}
		if (!CompoundState(&localnum)){ flag = false; }
		if (token.type != TOKEN_TYPE::RBRACE){ error(706); flag = false; }	//��}
	}

	//if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }	//���ű����Ѿ����ڱ�ʶ��

	if (flag){ genEndFunc(); }

	//temp = (tempnum - temp + localnum) * 4;
	temp = localnum * 4;
	char t[MAX_STR_LEN];
	_itoa(temp, t, 10);
	strcpy(midcode[line].arg1, t);

	token = lexer.getToken();

	return flag;
}

//�޷���ֵ��������	���ɺ�����ǩLable	����������������entry.size����������void����table
bool Parser::NonRtnFuncDefinition()
{
	TabEntry entry;
	bool flag = true;
	int localnum = 0;

	entry.ekind = ENTRY_KIND::NONRTNFUNCTION;
	entry.etype = ENTRY_TYPE::VOID;

	token = lexer.getToken();	//һ���Ǳ�ʶ����������

	strcpy_s(entry.name, token.value.str);
	entry.declarLine = CurrentLine;
	entry.size = 0;

	if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }	//������ű�

	genLabel(entry.name);
	int line = midcodeline;//�޸ĵ�����
	int temp = tempnum;//��ǰ��ʱ�Ĵ������
	genBeginFunc();

	//(����)
	token = lexer.getToken();		//һ���ǡ�����

	int size = 0;
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::RPARENT){
		if (!Args(&size)){ flag = false; }	//����
		if (token.type != TOKEN_TYPE::RPARENT){ error(70401); flag = false; }	//missing a ')',find next ')' or '{'
	}

	tab.Modify(size);

	//{�������}
	token = lexer.getToken();	//�Ѿ���������������һ��{
	if (token.type != TOKEN_TYPE::LBRACE){ error(705); flag = false; }
	token = lexer.getToken();	//��{���ȡ��һ����ȱ����ֱ���ж�
	if (token.type != TOKEN_TYPE::RBRACE){
		if (!CompoundState(&localnum)){ flag = false; }
		if (token.type != TOKEN_TYPE::RBRACE){ error(706); flag = false; }	// }
	}

	if (flag){ genEndFunc(); }

	//temp = (tempnum - temp + localnum) * 4;
	temp = localnum * 4;
	char t[MAX_STR_LEN];
	_itoa(temp, t, 10);
	strcpy(midcode[line].arg1, t);

	token = lexer.getToken();

	return flag;
}

//������		���ɺ�����ǩLable	������main����������0����������void����table
bool Parser::MainProgram()
{
	TabEntry entry;
	bool flag = true;
	int localnum = 0;

	if (token.type != TOKEN_TYPE::VOIDTK){ flag = false; }
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::MAINTK){ flag = false; }

	strcpy_s(entry.name, "main");
	entry.ekind = ENTRY_KIND::NONRTNFUNCTION;
	entry.etype = ENTRY_TYPE::VOID;
	entry.declarLine = CurrentLine;
	entry.size = 0;
	if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }

	genLabel(entry.name);
	int line = midcodeline;//�޸ĵ�����
	int temp = tempnum;//��ǰ��ʱ�Ĵ������
	genBeginFunc();

	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ flag = false; }	//������
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::RPARENT){ error(70401); flag = false; }
	else token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LBRACE){ error(705); flag = false; }
	else token = lexer.getToken();
	if (!CompoundState(&localnum)){ flag = false; }
	if (token.type != TOKEN_TYPE::RBRACE){ flag = false; }

	if (flag){ 
		char str[MAX_STR_LEN];
		genLabel("exit");
		genExit(); 
		genLabel("arrerror");
		//genALU(str, "\"array overflow\"", false);
		allstring[strnum] = "\"array overflow\"";
		sprintf(str, "%s%d", "$str", strnum++);
		genPrintf(str);		// Printf str
		genGOTOLabel("_exit");
	}
	//temp = (tempnum - temp + localnum) * 4;
	temp = localnum * 4;
	char t[MAX_STR_LEN];
	_itoa(temp, t, 10);
	strcpy(midcode[line].arg1, t);

	token = lexer.getToken();
	return flag;
}

//����	
bool Parser::Args(int *size)
{
	TabEntry entry;
	bool flag = true;

	if (token.type != TOKEN_TYPE::INTTK && token.type != TOKEN_TYPE::CHARTK){ error(202); flag = false; }
	else 
	{
		(*size)++;
		entry.etype = (token.type == TOKEN_TYPE::INTTK) ? ENTRY_TYPE::INT : ENTRY_TYPE::CHAR;
		entry.ekind = ENTRY_KIND::ARGS;
		token = lexer.getToken();
		if (token.type != TOKEN_TYPE::IDEN){ error(302); flag = false; }
		entry.declarLine = CurrentLine;

		strcpy_s(entry.name, token.value.str);
		if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }
		token = lexer.getToken();
	}

	while (token.type == TOKEN_TYPE::COMMA)
	{
		token = lexer.getToken();
		if (token.type != TOKEN_TYPE::INTTK && token.type != TOKEN_TYPE::CHARTK){ error(202); flag = false; }
		else {
			entry.etype = (token.type == TOKEN_TYPE::INTTK) ? ENTRY_TYPE::INT : ENTRY_TYPE::CHAR;
			token = lexer.getToken();
			if (token.type != TOKEN_TYPE::IDEN){ error(302); flag = false; }
			entry.declarLine = CurrentLine;

			strcpy_s(entry.name, token.value.str);
			if (!tab.insertTab(entry)){ error(401, entry.name); flag = false; }
			token = lexer.getToken();
		}
		(*size)++;
	}
	return flag;
}

//������� 
bool Parser::CompoundState(int *num)
{
	bool flag = true;
	if (token.type == TOKEN_TYPE::CONSTTK)	//��������
	{
		if (!ConstDeclaration()){ flag = false; }
	}
	if (token.type == TOKEN_TYPE::INTTK || token.type == TOKEN_TYPE::CHARTK)	//��������
	{
		if (!VarDeclaration(num)){ flag = false; }
	}
	if (!StateList()){ flag = false; }	//�����

	return flag;
}

//�����		 ��Ҳ�������
bool Parser::StateList()
{
	bool loop = false;
	bool flag = true;
	if (token.type == TOKEN_TYPE::IFTK || token.type == TOKEN_TYPE::WHILETK ||
		token.type == TOKEN_TYPE::LBRACE || token.type == TOKEN_TYPE::SCANFTK ||
		token.type == TOKEN_TYPE::PRINTFTK || token.type == TOKEN_TYPE::SWITCHTK ||
		token.type == TOKEN_TYPE::RETURNTK || token.type == TOKEN_TYPE::IDEN)
		loop = true;		//����� if �� while�� scanf �� printf�� switch�� return ��{ �� IDEN ��ѭ������
	while (loop){
		if (!Statement()){ flag = false; }
		loop = false;
		if (token.type == TOKEN_TYPE::IFTK || token.type == TOKEN_TYPE::WHILETK ||
			token.type == TOKEN_TYPE::LBRACE || token.type == TOKEN_TYPE::SCANFTK ||
			token.type == TOKEN_TYPE::PRINTFTK || token.type == TOKEN_TYPE::SWITCHTK ||
			token.type == TOKEN_TYPE::RETURNTK || token.type == TOKEN_TYPE::IDEN)
			loop = true;
	}
	return flag;
}

//���		attention://������Ҫ�ӣ� �ĸ���δ�����������ȥ����һ������
bool Parser::Statement()
{
	bool flag = true;
	char p[MAX_STR_LEN];
	strcpy(p, "");
	switch (token.type)
	{
	case TOKEN_TYPE::IFTK: if (!IfStatement()){ flag = false; }
						   break;
	case TOKEN_TYPE::WHILETK: if (!WhileStatement()){ flag = false; }
							  break;
	case TOKEN_TYPE::LBRACE: token = lexer.getToken();
							if (!StateList()){ flag = false; }
							 if (token.type != TOKEN_TYPE::RBRACE){ error(706); flag = false; }
							 else token = lexer.getToken();
							 break;
	case TOKEN_TYPE::SCANFTK: if (!ReadStatement()){ flag = false; }
							  if (token.type != TOKEN_TYPE::SEMICN){ error(102); flag = false; }
							  else token = lexer.getToken();
							  break;
	case TOKEN_TYPE::PRINTFTK: if (!WriteStatement()){ flag = false; }
							   if (token.type != TOKEN_TYPE::SEMICN){ error(102); flag = false; }
							   else token = lexer.getToken();
							   break;
	case TOKEN_TYPE::SWITCHTK: if (!SwitchStatement()){ flag = false; }
							   break;
	case TOKEN_TYPE::RETURNTK: if (!ReturnStatement()){ flag = false; }
							   if (token.type != TOKEN_TYPE::SEMICN){ error(102); flag = false; }
							   else token = lexer.getToken();
							   break;
	case TOKEN_TYPE::IDEN: TOKEN lkahead;
		lkahead = lexer.LookAhead(1);
		if (lkahead.type == TOKEN_TYPE::LPARENT)	//��������
		{
			TabEntry entry;
			entry = tab.search(token.value.str);
			//���ҷ��ű���iden���з��ػ����޷��صģ���Ȼ��������һ�������������ֵreturn�Ͳ�һ����
			if (entry.ekind == ENTRY_KIND::RTNFUNCTION){
				if (!CallRtnFunc(p)){ flag = false; }
			}
			else if (entry.ekind == ENTRY_KIND::NONRTNFUNCTION){
				if (!CallNonRtnFunc(p)){ flag = false; }
			}
			else { error(402, token.value.str); flag = false; }
		}
		else if (lkahead.type == TOKEN_TYPE::ASSIGN || lkahead.type == TOKEN_TYPE::LBRACK)	//��ֵ���
		{
			if (!AssignStatement()){ flag = false; }
		}
		if (token.type != TOKEN_TYPE::SEMICN){ error(101); flag = false; }
		else token = lexer.getToken();
		break;

	default:
		break;
	}

	return flag;
}

//�������	
bool Parser::IfStatement()
{
	bool flag = true;
	char p[MAX_STR_LEN];
	int iflabel = labelnum;
	labelnum+=2;
	strcpy(p, "");
	if (token.type != TOKEN_TYPE::IFTK){ flag = false; }
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70302); flag = false; }
	else token = lexer.getToken();
	if (!Condition(p,iflabel)){ flag = false; }

	if (token.type != TOKEN_TYPE::RPARENT){ error(70402); flag = false; }
	else token = lexer.getToken();
	if (!Statement()){ flag = false; }

	if (token.type == TOKEN_TYPE::ELSETK){
		genGOTOLabel(iflabel + 1);	//��ת��elseĩβ
		if (flag)
			genLabel(iflabel);

		token = lexer.getToken();
		if (!Statement()){ flag = false; }
		if (flag)
			genLabel(iflabel + 1);
	}
	else{
		if (flag)
			genLabel(iflabel);
	}
	return flag;
}

//����	��Ԫʽ���
bool Parser::Condition(char *r, int iflabel)
{
	char p[MAX_STR_LEN], q[MAX_STR_LEN];
	bool flag = true;
	int i = 0;
	strcpy(p, "");
	strcpy(q, "");

	if (!Expression(p, &i)){ flag = false; }
	if (token.type != TOKEN_TYPE::RPARENT)
	{
		char op[3];
		if ((token.type != TOKEN_TYPE::EQL) && (token.type != TOKEN_TYPE::LSS) && 
			(token.type != TOKEN_TYPE::LEQ) && (token.type != TOKEN_TYPE::GRE) && 
			(token.type != TOKEN_TYPE::GEQ) && (token.type != TOKEN_TYPE::NEQ))
		{
			error(502);	//�ǹ�ϵ�����
			return false;
		}
		else{
			strcpy(op, token.value.str);
			token = lexer.getToken();
			if (!Expression(q, &i)){ flag = false; }
		}
		if (flag)
			genCondition(r, p, op, q, iflabel);
	}
	else{
		genCondition(r, p, iflabel);
	}
	return flag;
}

//ѭ�����  
bool Parser::WhileStatement()
{
	bool flag = true;
	char p[MAX_STR_LEN];
	int whilelabel = labelnum;
	labelnum += 2;
	strcpy(p, "");
	genLabel(whilelabel);
	if (token.type != TOKEN_TYPE::WHILETK){ flag = false; }
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70302); flag = false; }
	else token = lexer.getToken();
	if (!Condition(p, whilelabel + 1)){ flag = false; }
	if (token.type != TOKEN_TYPE::RPARENT){ error(70402); flag = false; }
	else token = lexer.getToken();
	if (!Statement()){ flag = false; }
	if (flag){
		genGOTOLabel(whilelabel);
		genLabel(whilelabel + 1);
	}
	return flag;
}

//�з���ֵ�����������	
bool Parser::CallRtnFunc(char *r)
{
	bool flag = true;
	TabEntry entry;
	int i = 0;
	if (token.type != TOKEN_TYPE::IDEN){ flag = false; }
	entry = tab.search(token.value.str);
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70301); flag = false; }
	else token = lexer.getToken();
	if (token.type != TOKEN_TYPE::RPARENT){
		if (!ParameterList(&i)){ flag = false; }
	}

	if (i != entry.size){ error(901); flag = false; }
	if (flag)	//���ò�����Ŀ��ͬ,����tempstack�Ա����mystack
	{
		while (!tmpstack.empty())
		{
			genPush(tmpstack.top());
			tmpstack.pop();
		}
		
	}

	if (token.type != TOKEN_TYPE::RPARENT){ error(70403); flag = false; }
	else token = lexer.getToken();
	if (flag)		//���ã� ��ջ
	{
		genCall(r, entry.name, entry.etype);
		genPop(i);
	}
	return flag;
}

//�޷���ֵ�����������			the same with CallRtnFunc()
bool Parser::CallNonRtnFunc(char *r)
{
	bool flag = true;
	TabEntry entry;
	int i = 0;
	if (token.type != TOKEN_TYPE::IDEN){ flag = false; }
	entry = tab.search(token.value.str);
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70301); flag = false; }
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::RPARENT){
		if (!ParameterList(&i)){ flag = false; }
	}
	if (i != entry.size){ error(901); flag = false; }
	if (flag)	//���ò�����Ŀ��ͬ,����tempstack�Ա����mystack
	{
		while (!tmpstack.empty())
		{
			genPush(tmpstack.top());
			tmpstack.pop();
		}

	}

	if (token.type != TOKEN_TYPE::RPARENT){ error(70403); flag = false; }
	else token = lexer.getToken();
	if (flag){
		genCall(entry.name);
		genPop(i);
	}
	return flag;
}

//ֵ������			
bool Parser::ParameterList(int *size)
{
	bool flag = true;
	char p[MAX_STR_LEN], q[MAX_STR_LEN];
	int i = 0;
	strcpy(p, "");
	strcpy(q, "");
	if (!Expression(p, &i)){ flag = false; }
	if (flag){ tmpstack.push(p); }
	(*size)++;

	while (token.type == TOKEN_TYPE::COMMA)
	{
		token = lexer.getToken();
		if (!Expression(q, &i)){ flag = false; }
		if (flag){ tmpstack.push(q); }
		(*size)++;
	}
	
	return flag;
}

//��ֵ���		����IDEN�Ƿ��Ѿ������		
bool Parser::AssignStatement()
{
	bool flag = true, arr = false;
	int i = 0;
	char r[MAX_STR_LEN], p[MAX_STR_LEN], q[MAX_STR_LEN], tmp[MAX_STR_LEN], tmp1[MAX_STR_LEN], tmp2[MAX_STR_LEN];
	TabEntry rstEntry;
	strcpy(r, "");
	strcpy(p, "");
	strcpy(q, "");
	strcpy(tmp, "");
	strcpy(tmp1, "");
	strcpy(tmp2, "");
	if (token.type != TOKEN_TYPE::IDEN){ flag = false; }	//������
	else {
		rstEntry = tab.canBeUse(token.value.str);
		if (rstEntry.ekind == ENTRY_KIND::NOKIND){ error(403,token.value.str); flag = false; }	//����IDEN�Ƿ��Ѿ������
		if (rstEntry.ekind == ENTRY_KIND::CONST){ error(904); flag = false; }
	}
	token = lexer.getToken();
	if (token.type == TOKEN_TYPE::LBRACK)		//�Ƿ�������
	{
		token = lexer.getToken();
		if (!Expression(q, &i)){ flag = false; }
		if (token.type != TOKEN_TYPE::RBRACK){ error(70202); return false; }

		if (flag){
			char tmpp[MAX_STR_LEN];
			strcpy(tmpp, "");
			//genVar(tmp1, 4, true);
			genALU(tmp2, q, '*', "4\0", TEMP_TYPE::INTT);
			genCondition(tmpp, tmp2, ">=\0", "0\0", "_arrerror");
			genVar(tmp1, rstEntry.size * 4, true);
			genCondition(tmp, tmp1, ">\0", tmp2, "_arrerror");
			if (rstEntry.etype == ENTRY_TYPE::INTARRAY)
				genALU(r, rstEntry.name, '+', tmp2, TEMP_TYPE::INTT);
			else genALU(r, rstEntry.name, '+', tmp2, TEMP_TYPE::CHARR);
			arr = true;
		}
		/*else if (atoi(q) >= rstEntry.size)
		{
			error(903); flag = false;
		}*/
		token = lexer.getToken();	
	}
	else strcpy(r, rstEntry.name);
	if (token.type != TOKEN_TYPE::ASSIGN){ error(501); return false; }
	token = lexer.getToken();
	if (!Expression(p, &i)){ flag = false; }
	else genAssign(r, p, arr);

	return flag;
}

//�����			
bool Parser::ReadStatement()
{
	bool flag = true;
	TabEntry entry;
	int size = 0;

	if (token.type != TOKEN_TYPE::SCANFTK){ flag = false; }	//������
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70303); return false; }	//next ; 
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::IDEN){ error(301); return false; }	//next ; 
	entry = tab.canBeUse(token.value.str);

	if (entry.ekind != ENTRY_KIND::NOKIND && entry.ekind != ENTRY_KIND::CONST){ genScanf(entry.name); }	//��tempջ
	else if (entry.ekind = ENTRY_KIND::CONST){ error(904); flag = false; }
	else { error(403,token.value.str); flag = false; }
	
	//�ҵ�IDEN��������ֵ��
	token = lexer.getToken();
	while (token.type == TOKEN_TYPE::COMMA)
	{
		token = lexer.getToken();
		if (token.type != TOKEN_TYPE::IDEN){ error(301); return false; }	//next ; 
		entry = tab.canBeUse(token.value.str);

		if (entry.ekind != ENTRY_KIND::NOKIND && entry.ekind != ENTRY_KIND::CONST){ genScanf(entry.name); }	//��tempջ
		else if (entry.ekind = ENTRY_KIND::CONST){ error(904); flag = false; }
		else { error(403, token.value.str); flag = false; }
		//�ҵ�IDEN��������ֵ��
		token = lexer.getToken();
	}
	
	
	if (token.type != TOKEN_TYPE::RPARENT){ error(70403); return false; }	//missing a )

/*	if (flag)
	{
		size = tmpstack.size();
		while (!tmpstack.empty())
		{
			genPush(tmpstack.top());
			tmpstack.pop();
		}
		for (int i = 0; i < size; i++)
		{
			genCall("ScanfString");
			genPop(1);
		}
	}
*/
	token = lexer.getToken();
	return flag;
}

//д���				
bool Parser::WriteStatement()
{
	bool flag = true;
	char p[MAX_STR_LEN];// str[MAX_STR_LEN];
	int size = 1;
	int i = 0;
	strcpy(p, "");
	if (token.type != TOKEN_TYPE::PRINTFTK){ flag = false; }	//������
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70303); return false; }
	token = lexer.getToken();
	if (token.type == TOKEN_TYPE::STRCON){	//�ַ���
		//genALU(str, token.value.str, false);
		allstring[strnum] = token.value.str;
		//p���ַ���
		char temp[MAX_STR_LEN];
		sprintf(temp, "%s%d", "$str", strnum++);
		token = lexer.getToken();
		//genPrintf(temp);		// Printf str
		if (token.type == TOKEN_TYPE::COMMA)	//�ַ��������ʽ
		{
			token = lexer.getToken();
			if (!Expression(p, &i)){ flag = false; }
			if (flag){	
				genPrintf(temp);
				genPrintf(p);	//Printf p
			}		
		}
		else{
			genPrintf(temp);
		}
		//genPush(str);	//<�ַ���>��ջ
		if (token.type != TOKEN_TYPE::RPARENT){ error(70403); return false; }
	}
	else if (token.type != TOKEN_TYPE::RPARENT)		//���ʽ
	{
		if (!Expression(p,&i)){ flag = false; }
		if (flag){ 		//<���ʽ>��ջ
			genPrintf(p);
		}
		if (token.type != TOKEN_TYPE::RPARENT){ error(70403); return false; }
	}
	else {
		error(902);
		flag = false;
	}

/*	if (flag){
		for (int i = 0; i < size; i++)
		{
			genCall("PrintString");
			genPop(1);
		}
	}
*/
	token = lexer.getToken();
	
	return flag;
}

//������	
bool Parser::SwitchStatement()
{
	bool flag = true;
	char p[MAX_STR_LEN];
	int switchlabel;
	int i = 0;
	strcpy(p, "");
	if (token.type != TOKEN_TYPE::SWITCHTK){ flag = false; }
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LPARENT){ error(70302); flag = false; }	//�ǣ� ֱ�Ӷ�
	else token = lexer.getToken();

	if (!Expression(p, &i)){ flag = false; }	//���Ƚϵı��ʽp

	if (token.type != TOKEN_TYPE::RPARENT){ error(70401); flag = false; }	//�ǡ��������� �� or {
	token = lexer.getToken();
	if (token.type != TOKEN_TYPE::LBRACE){ error(705); flag = false; }	//��{ ,ֱ�Ӷ�
	else token = lexer.getToken();

	if (!CaseList(p, &switchlabel)){ flag = false; }

	if (token.type != TOKEN_TYPE::RBRACE){ error(706); flag = false; }	//��}

	if (flag){ genLabel(switchlabel); }

	token = lexer.getToken();
	return flag;
}

//����б�	
bool Parser::CaseList(char *p, int *label)
{
	bool flag = true;
	char con[MAX_STR_LEN], r[MAX_STR_LEN];
	int switchlabel = labelnum, i = 0;
	int lasecaselabel = labelnum + 1;
	*label = switchlabel;
	labelnum+=2;
	strcpy(con, "");
	strcpy(r, "");

	if (token.type != TOKEN_TYPE::CASETK){ error(801); flag = false; }
	if (token.type == TOKEN_TYPE::LBRACE){ return flag; }	//��case
	do{
		genLabel(lasecaselabel);
		token = lexer.getToken();
		if (!Constant(con)){ token = lexer.getToken(); flag = false; }			//���������ַ�ʱ���������Ѿ�����������
		else{
			genCondition(r, p, "==", con, labelnum++);
			lasecaselabel = labelnum - 1;
			if (token.type != TOKEN_TYPE::COLON){ error(503); flag = false; }	//���ˣ� ���Ŷ�
			else token = lexer.getToken();
			if (!Statement()){ flag = false; }
			if (flag){ genGOTOLabel(switchlabel); }	//����switch-case���Ľ�β��
		}
	} while (token.type == TOKEN_TYPE::CASETK);
	genLabel(lasecaselabel);
	return flag;
}

//����	
bool Parser::Constant(char *con)
{
	bool flag = true;
	char temp[3];
	if (token.type == TOKEN_TYPE::CHARCON){
		temp[0] = token.value.c;
		temp[1] = '\0';
		strcpy(con, temp);
		token = lexer.getToken();
	}
	else {
		int num;
		if (!Integer(&num)) { flag = false; }	//��������
		else {
			_itoa(num,con,10);
		}
	}
	return flag;
}

//�������		���÷��ر�ǩ
bool Parser::ReturnStatement()
{
	bool flag = true;
	char p[MAX_STR_LEN];
	strcpy(p, "");
	int i = 0;
	if (token.type != TOKEN_TYPE::RETURNTK){ flag = false; }
	token = lexer.getToken();
	if (token.type == TOKEN_TYPE::LPARENT)
	{
		token = lexer.getToken();
		if (!Expression(p, &i)){ flag = false; }
		if (token.type != TOKEN_TYPE::RPARENT){ error(70403); return false; }
		else token = lexer.getToken();
	}
	else if (token.type == TOKEN_TYPE::SEMICN){
		strcpy(p, "");
	}
	else{ error(70303); flag = false; }

	if (flag){ genRtn(p); }

	return flag;
}

//����	
bool Parser::Integer(int *num)
{
	bool flag = true;
	if (token.type == TOKEN_TYPE::PLUS || token.type == TOKEN_TYPE::MINU)
	{
		token = lexer.getToken();
		if (token.type != TOKEN_TYPE::INTCON || (token.type == TOKEN_TYPE::INTCON && token.value.i == 0)){ error(601); return false; }
		*num = (token.type == TOKEN_TYPE::PLUS) ? token.value.i : (0 - token.value.i);
	}
	else
	{
		if (token.type != TOKEN_TYPE::INTCON){ error(601); return false; }
		*num = token.value.i;
	}
	token = lexer.getToken();
	return flag;
}

//���ʽ
bool Parser::Expression(char *r, int *arr)
{
	bool flag = true;
	char p[MAX_STR_LEN], q[MAX_STR_LEN];
	char op = '+';
	strcpy(p, "");
	strcpy(q, "");

	if (token.type == TOKEN_TYPE::PLUS || token.type == TOKEN_TYPE::MINU)
	{
		op = (token.type == TOKEN_TYPE::PLUS) ? '+' : '-';
		token = lexer.getToken();
	}

	if (!Term(p, arr)){ flag = false; }
	if (op == '-' && flag)
	{
		genALU(r, op, p);
		strcpy(p, r);
		*arr = 0;
	}
	
	while (token.type == TOKEN_TYPE::PLUS || token.type == TOKEN_TYPE::MINU)
	{
		*arr = 0;
		op = (token.type == TOKEN_TYPE::PLUS) ? '+' : '-';
		token = lexer.getToken();
		if (!Term(q, arr)){ flag = false; }
		else genALU(r, p, op, q, TEMP_TYPE::INTT);
		strcpy(p, r);
	}
	strcpy(r, p);
	return flag;
}

//��	term  
bool Parser::Term(char *r, int *arr)
{
	bool flag = true;
	char p[MAX_STR_LEN], q[MAX_STR_LEN];
	char op ;
	strcpy(p, "");
	strcpy(q, "");

	if (!Factor(p, arr)){ flag = false; }
	while (token.type == TOKEN_TYPE::MULT || token.type == TOKEN_TYPE::DIV)
	{
		*arr = 0;
		op = (token.type == TOKEN_TYPE::MULT) ? '*' : '/';
		token = lexer.getToken();
		if (!Factor(q, arr)){ flag = false; }
		else genALU(r, p, op, q, TEMP_TYPE::INTT);
		strcpy(p, r);
	}
	strcpy(r, p);
	return flag;
}

//���� 	����Ƿ��Ѿ�������
bool Parser::Factor(char *r, int *arr)
{
	bool flag = true;
	int i = 0;
	if (token.type == TOKEN_TYPE::LPARENT)	//(���ʽ)
	{
		token = lexer.getToken();
		if (!Expression(r, &i)){ flag = false; }
		if (token.type != TOKEN_TYPE::RPARENT){ error(70402); return false; }	//����ֱ�ӷ���
		token = lexer.getToken();
	}
	else if (token.type == TOKEN_TYPE::PLUS || token.type == TOKEN_TYPE::MINU || token.type == TOKEN_TYPE::INTCON)	//����
	{
		int num;
		if (!Integer(&num)){ flag = false; }
		else sprintf(r, "%d", num);			//������ֵ
	}
	else if (token.type == TOKEN_TYPE::CHARCON)	//�ַ�
	{
		genVar(r, (int)token.value.c, false);			//�ַ���ֵ
		//sprintf(r, "%d", (int)token.value.c);
		token = lexer.getToken();
	}
	else if (token.type == TOKEN_TYPE::IDEN)	//����ʶ����������ʶ������[�������ʽ����]�� | ���з���ֵ����������䣾		����Ƿ��Ѿ�������
	{
		TabEntry idenentry,funcentry;
		idenentry = tab.canBeUse(token.value.str);
		funcentry = tab.search(token.value.str);
		char tokenname[MAX_STR_LEN];
		strcpy_s(tokenname, token.value.str);
		TOKEN lkahead;
		lkahead = lexer.LookAhead(1);
		if ((funcentry.ekind == ENTRY_KIND::RTNFUNCTION || funcentry.ekind == ENTRY_KIND::NONRTNFUNCTION) && lkahead.type == TOKEN_TYPE::LPARENT){
			if (funcentry.ekind == ENTRY_KIND::RTNFUNCTION){
				if (!CallRtnFunc(r)){ flag = false; }
			}
			else{
				error(404, funcentry.name);
				flag = false;
			}
		}
		else if ((idenentry.etype == ENTRY_TYPE::CHARARRAY || idenentry.etype == ENTRY_TYPE::INTARRAY) && (lkahead.type == TOKEN_TYPE::LBRACK)){		//��[	����ʶ������[�������ʽ����]��
			char q[MAX_STR_LEN], tmp[MAX_STR_LEN], tmpp[MAX_STR_LEN], tmp1[MAX_STR_LEN], tmp2[MAX_STR_LEN], tmp3[MAX_STR_LEN];
			strcpy(q, "");
			strcpy(tmp, "");
			strcpy(tmpp, "");
			strcpy(tmp1, "");
			strcpy(tmp2, "");
			strcpy(tmp3, "");

			token = lexer.getToken();		// [
			token = lexer.getToken();
			if (!Expression(q, &i)){ flag = false; }
			if (token.type != TOKEN_TYPE::RBRACK){ error(70203); return false; }	//����ֱ�ӷ���
			if (flag){
				//genVar(tmp1, 4, true);
				genALU(tmp2, q, '*', "4\0", TEMP_TYPE::INTT);
				genCondition(tmpp, tmp2, ">=\0", "0\0", "_arrerror");
				genVar(tmp1, idenentry.size * 4, true);
				genCondition(tmp, tmp1, ">\0", tmp2, "_arrerror");
				if (idenentry.etype == ENTRY_TYPE::INTARRAY)
					genALU(tmp3, idenentry.name, '+', tmp2, TEMP_TYPE::INTT);
				else genALU(tmp3, idenentry.name, '+', tmp2, TEMP_TYPE::CHARR);
				genALU(r, tmp3, true);
			}
			token = lexer.getToken();
		}
		else if (idenentry.etype == ENTRY_TYPE::CHAR || idenentry.etype == ENTRY_TYPE::INT){
			if (idenentry.ekind == ENTRY_KIND::CONST){	//����
				if (idenentry.etype == ENTRY_TYPE::CHAR){
					*arr = 1;
					//sprintf(r, "%d", (int)idenentry.value.c);
					genVar(r, (int)idenentry.value.c, false);
				}
				else //sprintf(r, "%d", idenentry.value.i);
					genVar(r, idenentry.value.i, true);
			}
			else{
				strcpy(r, tokenname);			//��ʶ�� �Ǳ���
			}
			token = lexer.getToken();
		}
		else { error(403,tokenname); flag = false; }
	}
	else flag = false;
	return flag;
}


