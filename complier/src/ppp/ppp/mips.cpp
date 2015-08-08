#define _CRT_SECURE_NO_WARNINGS
#include "mips.h"

// $t0 - $t9	
int MIPS::ValidRegister()
{
	for (regnum = 0;; regnum++)
	{
		if (regnum == 10)	//没有未被使用过的$t
			return -1;
		if (t[regnum] == 0){	// 0 则可以使用
			MarkInvalidReg(regnum);
			return regnum;
		}
	}	
	return -1;
}

// $s0 - $s7
int MIPS::ValidSRegister()
{
	sregnum++;
	if (sregnum == 8)
		sregnum = 0;
	return sregnum;
}

//将已经用过的寄存器标记为0
void MIPS::MarkValidReg(int reg)
{
	t[reg] = 0;
//	fprintf(finalfile, "#release $t%d\n", reg);
	
}

//将刚用的reg标记为1
void MIPS::MarkInvalidReg(int reg)
{
	t[reg] = 1;
	//fprintf(finalfile, "#alloc $t%d\n", reg);
}

void MIPS::EmitTotal(int size)
{
	fprintf(finalfile, ".data\n");

	//在符号表里找到所有的全局变量
	tab.setGlobalVar();

	for (int k = 0;; k++)
	{
		m = midcode[k];
		if (m.result[0] == '_'){
			i = k;
			break;
		}
	}

	for (int k = 0; k < strnum; k++)
	{
		fprintf(finalfile, "$str%d: .asciiz %s \n", k, allstring[k].c_str());
	}

	fprintf(finalfile, ".text \n");
	fprintf(finalfile, "\tj _main \n");

	for (; i < size; i++)
	{
		m = midcode[i];	//	result arg1 op arg2
		fprintf(finalfile, "\t# %s %s %s %s \n", m.result, m.arg1, m.op, m.arg2);
		if (strcmp(m.result, "") == 0)
			break;
		if (strcmp(m.arg2, "") == 0)
		{
			if (strcmp(m.op, "") == 0)
			{
				if (strcmp(m.arg1, "") == 0)		//  result  ""  ""  ""
				{
					if (strcmp(m.result, "EndFunc") != 0 && strcmp(m.result, "EndProgram") != 0 && strcmp(m.result, "Return") != 0)	//	$Lx:  |   _func:
					{
						EmitLabel();	//生成标签
					}
					else if (strcmp(m.result, "EndFunc") == 0)	//EndFunc
					{
						EmitEndFunc();
					}
					else if (strcmp(m.result, "EndProgram") == 0)
					{
						EmitEndProgram();
					}
					else{
						EmitReturn();
					}
				}
				else 		//  result  arg1  ""  ""
				{
					if (strcmp(m.result, "BeginFunc") == 0)	//ok
					{
						EmitBeginFunc();
					}
					else if (strcmp(m.result, "PushStack") == 0)	
					{
						EmitPushStack();
					}
					else if (strcmp(m.result, "LCALL") == 0)
					{
						EmitCallNonRtnFunc();
					}
					else if (strcmp(m.result, "PopStack") == 0)	// ok
					{
						EmitPopStack();
					}
					else if (strcmp(m.result, "GOTO") == 0)
					{
						EmitGoto();
					}
					else if (strcmp(m.result, "Return") == 0)
					{
						EmitReturn();
					}
					else if (strcmp(m.result, "VAR") == 0)
					{
						continue;
					}
					else if (strcmp(m.result, "Printf") == 0)
					{
						EmitPrintf();
					}
					else if (strcmp(m.result, "Scanf") == 0)
					{
						EmitScanf();
					}
					else		// $t num ||  IDEN $t | IDEN | num  ||  $t *($t)  ||  *($t) $t | num  ||  $t $strx 
					{
						if (m.result[0] == '$')		//  $t num ||   $t *($t)  ||  $t $strx 
						{
							if (m.arg1[0] == '*')	//  $t *($t)  $t是数组的地址
							{
								EmitArrayOp();
							}
							else if (m.arg1[0] == '$')		//   $t $strx	字符串
							{
								EmitLoadString();
							}
							else {		// $t num  num = int | (int)char
								EmitLoadConstant();
							}
						}
						else if (m.result[0] == '*')	//	*($t) $t | num
						{
							EmitArrayOp();
						}
						else{		// IDEN $t  |  IDEN IDEN  |  IDEN num  
							EmitIDENAssign();
						}
					}
				}
			}
			else	// result arg1 op ""
			{
				if (strcmp(m.arg1, "LCALL") == 0)	// $t = LCALL _func
				{
					EmitCallRtnFunc();
				}
				else
				{
					fprintf(finalfile, "intermedia code is wrong\n");
				}
			}
		}
		else	// result arg1 op arg2
		{
			// + - * /
			if (strcmp(m.op, "+\0") == 0)
			{
				EmitAdd();
			}
			else if (strcmp(m.op, "-\0") == 0)
			{
				EmitSub();
			}
			else if (strcmp(m.op, "*\0") == 0 || strcmp(m.op, "/\0") == 0)
			{
				EmitMultDiv();
			}
			//关系运算符 == != > >= < <=
			else if (strcmp(m.op, "==") == 0 || strcmp(m.op, "!=") == 0 || strcmp(m.op, ">=") == 0
				|| strcmp(m.op, ">") == 0 || strcmp(m.op, "<=") == 0 || strcmp(m.op, "<") == 0)
			{
				EmitComparison();
			}
			else if (strcmp(m.result, "IFZ") == 0)
			{
				EmitIFZ();
			}
			else {
				fprintf(finalfile, "intermedia code is wrong \n");
			}
		}
	}
}

//ok
void MIPS::EmitLabel()
{
	fprintf(finalfile, "%s \n", m.result);
	if (m.result[0] == '_')
	{
		int len = strlen(m.result), i, k;
		char func[MAX_STR_LEN];
		for (i = 1, k = 0; i < len - 1; i++, k++){
			func[k] = m.result[i];
		}
		func[k] = '\0';
		nowfuncflag = tab.getFuncLocation(func);
	}
}

//ok
void MIPS::EmitLoadConstant()	
{
	int reg = ValidRegister();
	fprintf(finalfile, "\tli $t%d, %s \n", reg, m.arg1);
	storereg(reg);
}

// $t $strx		ok
void MIPS::EmitLoadString()
{
	fprintf(finalfile, "\tla $a0, %s \n", m.arg1);
}

// IDEN $t | num | IDEN		ok		不存在IDEN是array的情况
void MIPS::EmitIDENAssign()
{
	int reg;

	if (m.arg1[0] == '$'){		// IDEN $t	
		reg = midtmpvar[getnum(m.arg1)].regnum;
		midtmpvar[getnum(m.arg1)].times--;
		if (midtmpvar[getnum(m.arg1)].times == 0)
			MarkValidReg(reg);
	}
	else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-'){
		reg = ValidRegister();
		fprintf(finalfile, "\tli $t%d, %s \n", reg, m.arg1);
		MarkValidReg(reg);
	}
	else{		// IDEN IDEN
		reg = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
		if (entry.ekind != ENTRY_KIND::NOKIND){	//局部
			if (entry.ekind == ENTRY_KIND::ARGS)
			{
				fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
			}
			else{
				fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
			}
		}
		else{	//全局
			fprintf(finalfile, "\tlw $t%d, %s \n", reg, m.arg1);
		}
		MarkValidReg(reg);
	}

	TabEntry entry;
	entry = tab.getLocalVarLocation(nowfuncflag, m.result);
	
	if (entry.ekind != ENTRY_KIND::NOKIND){	//局部
		if (entry.ekind == ENTRY_KIND::ARGS)
		{
			fprintf(finalfile, "\tsw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
		}
		else{
			fprintf(finalfile, "\tsw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
		}
	}
	else{	//全局
		fprintf(finalfile, "\tsw $t%d, %s \n", reg, m.result);
	}
}

//ok
void MIPS::EmitBeginFunc()
{
	fprintf(finalfile, "\tsubu $sp, $sp, 8\t%s\n", "# decrement sp to make space to save ra,fp");
	fprintf(finalfile, "\tsw $fp, 8($sp)\t%s\n", "# save fp");
	fprintf(finalfile, "\tsw $ra, 4($sp)\t%s\n", "# save ra");
	fprintf(finalfile, "\taddiu $fp, $sp, 8\t%s\n", "# set up new fp");
	fprintf(finalfile, "\tsubu $sp, $sp, %s\t%s\n", m.arg1, "# decrement sp to make space for locals/temps");
}

//ok	PushStack  $t | IDEN
void MIPS::EmitPushStack()	
{
	fprintf(finalfile, "\tsubu $sp, $sp, 4\t%s\n", "# decrement sp to make space for temps");
	if (m.arg1[0] == '$'){
		int reg = midtmpvar[getnum(m.arg1)].regnum;
		fprintf(finalfile, "\tsw $t%d, 4($sp) \n", reg);
		midtmpvar[getnum(m.arg1)].times--;
		if (midtmpvar[getnum(m.arg1)].times == 0)
			MarkValidReg(reg);
	}
	else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || (m.arg1[0] == '-')){
		int reg = ValidRegister();
		fprintf(finalfile, "\tli $t%d, %s\n", reg, m.arg1);
		fprintf(finalfile, "\tsw $t%d, 4($sp) \n", reg);
		MarkValidReg(reg);
	}
	else{
		int reg = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
		if (entry.ekind != ENTRY_KIND::NOKIND)	//局部变量	
		{
			if (entry.ekind == ENTRY_KIND::ARGS)
			{
				fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
				fprintf(finalfile, "\tsw $t%d, 4($sp) \n", reg);
			}
			else{
				fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
				fprintf(finalfile, "\tsw $t%d, 4($sp) \n", reg);
			}
			
		}
		else{	//全局变量
			//不需要找到全局里面的entry？	-->  不需要，printf里面需要是要判断int char方便打印
			fprintf(finalfile, "\tsw %s, 4($sp) \n", m.arg1);
		}
		MarkValidReg(reg);
	}
	
}

//LCALL _func
void MIPS::EmitCallNonRtnFunc()
{
	fprintf(finalfile, "\tjal %s \n", m.arg1);
}

// iden | $t = LCALL _func
void MIPS::EmitCallRtnFunc()
{
	int reg = ValidRegister();
	fprintf(finalfile, "\tjal %s \n", m.op);
	if (m.result[0] == '$'){
		fprintf(finalfile, "\tmove $t%d, $v0 \n", reg);
		storereg(reg);
	}
	else{
		fprintf(finalfile, "\tmove $t%d, $v0 \n", reg);
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.result);

		if (entry.ekind != ENTRY_KIND::NOKIND){	//局部
			if (entry.ekind == ENTRY_KIND::ARGS)
			{
				fprintf(finalfile, "\tsw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
			}
			else{
				fprintf(finalfile, "\tsw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
			}
		}
		else{	//全局
			fprintf(finalfile, "\tsw $t%d, %s \n", reg, m.result);
		}

		MarkValidReg(reg);
	}
}

void MIPS::EmitPopStack()
{
	fprintf(finalfile, "\tadd $sp, $sp, %s\t%s\n", m.arg1, "# pop off the stack");
}

// IFZ $t | IDEN  | num
void MIPS::EmitIFZ()
{
	if (m.arg1[0] == '$'){
		int reg = midtmpvar[getnum(m.arg1)].regnum;
		char comparison[3], op[5];
		strcpy(comparison, midcode[i - 1].op);
		if (strcmp(comparison, "==") == 0)
			strcpy(op, "bne");
		else if (strcmp(comparison, "!=") == 0)
			strcpy(op, "beq");
		else if (strcmp(comparison, ">=") == 0)
			strcpy(op, "bltz");
		else if (strcmp(comparison, ">\0") == 0)
			strcpy(op, "blez");
		else if (strcmp(comparison, "<=") == 0)
			strcpy(op, "bgtz");
		else if (strcmp(comparison, "<\0") == 0)
			strcpy(op, "bgez");
		
		int len = strlen(op);
		if (len == 3)
			fprintf(finalfile, "\t%s, $t%d, $zero, %s \n", op, reg, m.arg2);
		else if (len == 4)
			fprintf(finalfile, "\t%s, $t%d, %s \n", op, reg, m.arg2);
		midtmpvar[getnum(m.arg1)].times--;
		if (midtmpvar[getnum(m.arg1)].times == 0)
			MarkValidReg(reg);
	}
	else if ((m.arg1[0] >= '0'&&m.arg1[0] <= '9') || m.arg1[0] == '-'){
		int num;
		num = atoi(m.arg1);
		if (num != 0){

		}
		else{
			fprintf(finalfile, "\tj %s \n", m.arg2);
		}
	}
	else{
		int reg = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
		if (entry.ekind == ENTRY_KIND::NOKIND){
			if (entry.ekind == ENTRY_KIND::ARGS){
				fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
			}
			else{
				fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
			}
		}
		else{
			fprintf(finalfile, "\tlw $t%d, %s \n", reg, m.arg1);
		}
		fprintf(finalfile, "\tbeq, $t%d, $zero, %s \n", reg, m.arg2);
		MarkValidReg(reg);
	}
}

//
void MIPS::EmitGoto()
{
	fprintf(finalfile, "\tj %s \n", m.arg1);
}

//Return $t | IDEN	| num	ok
void MIPS::EmitReturn()
{
	int reg;
	if (strcmp(m.arg1, "") != 0){
		if (m.arg1[0] == '$'){
			reg = midtmpvar[getnum(m.arg1)].regnum;
			fprintf(finalfile, "\tmove $v0, $t%d \n", reg);
			midtmpvar[getnum(m.arg1)].times--;
			if (midtmpvar[getnum(m.arg1)].times == 0)
				MarkValidReg(reg);
		}
		else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || (m.arg1[0] == '-')){
			reg = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg, m.arg1);
			fprintf(finalfile, "\tmove $v0, $t%d \n", reg);
			MarkValidReg(reg);
		}
		else{
			reg = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND){
				if (entry.ekind == ENTRY_KIND::ARGS){
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s \n", reg, m.arg1);
			}
			fprintf(finalfile, "\tmove $v0, $t%d \n", reg);
			MarkValidReg(reg);
		}
	}
	else{

	}
	EmitEndFunc();
}

//ok
void MIPS::EmitEndFunc()
{
	fprintf(finalfile, "\tmove $sp, $fp\t%s\n", "# pop callee frame off stack");
	fprintf(finalfile, "\tlw $ra, -4($fp)\t%s\n", "# restore saved ra");
	fprintf(finalfile, "\tlw $fp, 0($fp)\t%s\n", "# restore saved fp");
	fprintf(finalfile, "\tjr $ra\t%s\n", "# return from function");
}

//ok
void MIPS::EmitEndProgram()
{
	fprintf(finalfile, "\tli $v0, 10 \n");
	fprintf(finalfile, "\tsyscall \n");
}

// Printf  $t  ||  IDEN		字符串 | 表达式		ok
void MIPS::EmitPrintf()
{
	if (m.arg1[0] == '$')
	{
		if (m.arg1[1] == 's'){	//$str
			fprintf(finalfile, "\tla $a0, %s \n", m.arg1);
			fprintf(finalfile, "\tli $v0, 4\t# print a string \n");	//打印字符串	
		}
		else{		//$t
			int k = i - 1;
			int num = getnum(m.arg1);
			//判断$t到底是什么类型
			if (midtmpvar[num].type == TEMP_TYPE::CHARR){
				fprintf(finalfile, "\tmove $a0, $t%d \n", midtmpvar[num].regnum);
				fprintf(finalfile, "\tli $v0, 11 \t# print a char \n");	//打印字符
				midtmpvar[num].times--;
				if (midtmpvar[num].times == 0)
					MarkValidReg(midtmpvar[num].regnum);
			}
			else if (midtmpvar[num].type == TEMP_TYPE::INTT){
				fprintf(finalfile, "\tmove $a0, $t%d \n", midtmpvar[num].regnum);
				fprintf(finalfile, "\tli $v0, 1\t# print an integer \n");	//打印整数
				midtmpvar[num].times--;
				if (midtmpvar[num].times == 0)
					MarkValidReg(midtmpvar[num].regnum);
			}
			else if (midtmpvar[num].type == TEMP_TYPE::STRCONN)
				fprintf(finalfile, "\tli $v0, 4\t# print a string \n");	//打印字符串	
		}
	}
	else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-'){
		fprintf(finalfile, "\tli $a0, %s \n", m.arg1);
		fprintf(finalfile, "\tli $v0, 1\t# print an integer \n");	//打印整数
	}
	else{
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
		if (entry.ekind != ENTRY_KIND::NOKIND)
		{
			if (entry.ekind == ENTRY_KIND::ARGS)	//参数 argsnum
			{
				int reg = ValidRegister();
				fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
				fprintf(finalfile, "\tmove $a0, $t%d \n", reg);
				storereg(reg);
				MarkValidReg(reg);
				if (entry.etype == ENTRY_TYPE::INT)
					fprintf(finalfile, "\tli $v0, 1 \t# print an integer\n");	//打印整数
				else fprintf(finalfile, "\tli $v0, 11 \t# print a char\n");	//打印字符
			}
			else{		//变量 varnum
				int reg = ValidRegister();
				fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
				fprintf(finalfile, "\tmove $a0, $t%d \n", reg);
				storereg(reg);
				MarkValidReg(reg);
				if (entry.etype == ENTRY_TYPE::INT)
					fprintf(finalfile, "\tli $v0, 1 \t# print an integer\n");	//打印整数
				else fprintf(finalfile, "\tli $v0, 11 \t# print a char\n");	//打印字符
			}
		}
		else{
			entry = tab.getGlobalVarLocation(m.arg1);
			//全局变量取出来
			fprintf(finalfile, "\tlw $a0, %s \n", m.arg1);
			if (entry.etype == ENTRY_TYPE::INT)
				fprintf(finalfile, "\tli $v0, 1 \t# print an integer\n");	//打印整数
			else fprintf(finalfile, "\tli $v0, 11 \t# print a char\n");	//打印字符
		}
	}
	fprintf(finalfile, "\tsyscall \n");
}

// Scanf IDEN		ok
void MIPS::EmitScanf()
{
	TabEntry entry;

	entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
	if (entry.ekind != ENTRY_KIND::NOKIND)		//是局部变量
	{
		if (entry.etype == ENTRY_TYPE::INT)
			fprintf(finalfile, "\tli $v0, 5 \t# read an integer\n");	//读取整数
		else fprintf(finalfile, "\tli $v0, 12 \t# read a char\n");	//读取字符

		fprintf(finalfile, "\tsyscall \n");

		if (entry.ekind == ENTRY_KIND::ARGS)
			fprintf(finalfile, "\tsw $v0, %d($fp) \n", entry.argsnum * 4);
		else fprintf(finalfile, "\tsw $v0, -%d($fp) \n", (entry.varnum + 1) * 4);

	}
	else{		//全局变量
		entry = tab.getGlobalVarLocation(m.arg1);
		if (entry.etype == ENTRY_TYPE::INT)
			fprintf(finalfile, "\tli $v0, 5 \t# read an integer\n");	//读取整数
		else fprintf(finalfile, "\tli $v0, 12 \t# read a char\n");	//读取字符
		fprintf(finalfile, "\tsyscall \n");
		fprintf(finalfile, "\tsw $v0, %s \n", m.arg1);
	}
	
}
/*if (m.result[0]=='$')
				storereg(reg);
			else{
				TabEntry entry;
				entry = tab.getLocalVarLocation(nowfuncflag, m.result);
				if (entry.ekind != ENTRY_KIND::NOKIND){
					if (entry.ekind == ENTRY_KIND::ARGS)
						fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
					else
						fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
				}
				else
					fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
				MarkValidReg(reg);
			}*/

//result = arg1 + arg2	arg: IDEN | $t	|  num	IDEN要取出来放在寄存器里面才能使用
void MIPS::EmitAdd()
{
	/*
	int reg = ValidRegister();
	int reg1, reg2;

	if (((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-') && ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || m.arg2[0] == '-')){
		reg1 = atoi(m.arg1);
		reg2 = atoi(m.arg2);
		fprintf(finalfile, "\tli $t%d, %d\n", reg, reg1 + reg2);
	}
	else{
		if (m.arg2[0] == '$'){
			reg2 = midtmpvar[getnum(m.arg2)].regnum;
		}
		else if ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || m.arg2[0] == '-'){
			reg2 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s\n", reg2, m.arg2);
		}
		else{
			reg2 = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg2);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg2, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg2, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s\n", reg2, m.arg2);	//取值存入寄存器，以便计算
			}
		}

		if (m.arg1[0] == '$'){
			reg1 = midtmpvar[getnum(m.arg1)].regnum;
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d\n", reg, reg1, reg2);
		}
		else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-'){
			reg1 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg1, m.arg1);
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d\n", reg, reg1, reg2);
		}
		else{
			reg1 = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg1, entry.argsnum * 4);
					fprintf(finalfile, "\tadd $t%d, $t%d, $t%d\n", reg, reg1, reg2);
				}
				else{
					if (entry.etype != ENTRY_TYPE::CHARARRAY && entry.etype != ENTRY_TYPE::INTARRAY){
						fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg1, (entry.varnum + 1) * 4);
						fprintf(finalfile, "\tadd $t%d, $t%d, $t%d\n", reg, reg1, reg2);
					}
					else{	//m.arg1是数组
						int sreg = ValidSRegister();
						fprintf(finalfile, "\tli $t%d, %d \n", reg1, -(entry.varnum + 1) * 4);
						fprintf(finalfile, "\tadd $t%d, $t%d, $fp \n", reg1, reg1);	//$s(sreg)里面保存的就是数组头的地址
						fprintf(finalfile, "\tsub $t%d, $t%d, $t%d\n", reg, reg1, reg2);
					}
				}
				storereg(reg);
			}
			else{
				entry = tab.getGlobalVarLocation(m.arg1);
				if (entry.etype != ENTRY_TYPE::CHARARRAY && entry.etype != ENTRY_TYPE::INTARRAY){
					fprintf(finalfile, "\tla $t%d, %s \n", reg1, m.arg1);	//取地址存入寄存器，以便计算
					fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);	//arg2是偏移
					storereg(reg);
				}
				else{	//如果是数组,直接用名字，因为是表示地址，直接加上偏移量
					int sreg = ValidSRegister();
					fprintf(finalfile, "\tla $t%d, %s \n", reg1, m.arg1);
					fprintf(finalfile, "\tadd $s%d, $t%d, $t%d \n", sreg, reg1, reg2);
					storereg(sreg);
					MarkValidReg(reg);
				}
			}
		}

		MarkValidReg(reg1);
		MarkValidReg(reg2);
	}
*/

	
	if ((m.arg2[0] == '$') || ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || (m.arg2[0] == '-'))){
		int reg2;
		if (m.arg2[0] == '$'){
			reg2 = midtmpvar[getnum(m.arg2)].regnum;
			midtmpvar[getnum(m.arg2)].times--;
		}
		else {		// num
			reg2 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg2, m.arg2);
		}

		if (m.arg1[0] == '$'){		// $t + $t
			int reg = ValidRegister();
			int reg1 = midtmpvar[getnum(m.arg1)].regnum;
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
			//storereg(reg);
			if (m.result[0]=='$')
				storereg(reg);
			else{
				TabEntry entry;
				entry = tab.getLocalVarLocation(nowfuncflag, m.result);
				if (entry.ekind != ENTRY_KIND::NOKIND){
					if (entry.ekind == ENTRY_KIND::ARGS)
						fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
					else
						fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
				}
				else
					fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
				MarkValidReg(reg);
			}
			midtmpvar[getnum(m.arg1)].times--;
			if (midtmpvar[getnum(m.arg1)].times == 0)
				MarkValidReg(reg1);
		}
		else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || (m.arg1[0] == '-')){
			int reg = ValidRegister();
			int reg1 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg1, m.arg1);
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
			//storereg(reg);
			if (m.result[0] == '$')
				storereg(reg);
			else{
				TabEntry entry;
				entry = tab.getLocalVarLocation(nowfuncflag, m.result);
				if (entry.ekind != ENTRY_KIND::NOKIND){
					if (entry.ekind == ENTRY_KIND::ARGS)
						fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
					else
						fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
				}
				else
					fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
				MarkValidReg(reg);
			}
			MarkValidReg(reg1);
		}
		else{	// IDEN + $t	IDEN可能有数组
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				int reg1 = ValidRegister();
				int reg = ValidRegister();
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg1, entry.argsnum * 4);
					fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
					//storereg(reg);
					if (m.result[0] == '$')
						storereg(reg);
					else{
						TabEntry entry;
						entry = tab.getLocalVarLocation(nowfuncflag, m.result);
						if (entry.ekind != ENTRY_KIND::NOKIND){
							if (entry.ekind == ENTRY_KIND::ARGS)
								fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
							else
								fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
						}
						else
							fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
						MarkValidReg(reg);
					}
				}
				else{
					if (entry.etype != ENTRY_TYPE::CHARARRAY && entry.etype != ENTRY_TYPE::INTARRAY){
						fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg1, (entry.varnum + 1) * 4);
						fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
						//storereg(reg);
						if (m.result[0] == '$')
							storereg(reg);
						else{
							TabEntry entry;
							entry = tab.getLocalVarLocation(nowfuncflag, m.result);
							if (entry.ekind != ENTRY_KIND::NOKIND){
								if (entry.ekind == ENTRY_KIND::ARGS)
									fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
								else
									fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
							}
							else
								fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
							MarkValidReg(reg);
						}
					}
					else{	//如果是数组,则拿(entry.varnum + 1) * 4就行
						int sreg = ValidSRegister();
						fprintf(finalfile, "\tli $t%d, %d \n", reg1, -(entry.varnum + 1) * 4);
						fprintf(finalfile, "\tadd $t%d, $t%d, $fp \n", reg1, reg1);	//$s(sreg)里面保存的就是数组头的地址
						fprintf(finalfile, "\tsub $s%d, $t%d, $t%d \n", sreg, reg1, reg2);
						storereg(sreg);
						MarkValidReg(reg);
					}
				}
				MarkValidReg(reg1);
			}
			else{
				int reg1 = ValidRegister();
				int reg = ValidRegister();
				entry = tab.getGlobalVarLocation(m.arg1);	
				//全局变量 非数组, 名字即地址，load即可
				if (entry.etype != ENTRY_TYPE::CHARARRAY && entry.etype != ENTRY_TYPE::INTARRAY){
					fprintf(finalfile, "\tla $t%d, %s \n", reg1, m.arg1);	//取地址存入寄存器，以便计算
					fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);	//arg2是偏移
					//storereg(reg);
					if (m.result[0] == '$')
						storereg(reg);
					else{
						TabEntry entry;
						entry = tab.getLocalVarLocation(nowfuncflag, m.result);
						if (entry.ekind != ENTRY_KIND::NOKIND){
							if (entry.ekind == ENTRY_KIND::ARGS)
								fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
							else
								fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
						}
						else
							fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
						MarkValidReg(reg);
					}
				}
				else{	//如果是数组,直接用名字，因为是表示地址，直接加上偏移量
					int sreg = ValidSRegister();
					fprintf(finalfile, "\tla $t%d, %s \n", reg1, m.arg1);
					fprintf(finalfile, "\tadd $s%d, $t%d, $t%d \n", sreg, reg1, reg2);
					storereg(sreg);
					MarkValidReg(reg);
				}
				MarkValidReg(reg1);
			}
		}
		if (m.arg2[0] == '$'){	// arg2 = $t
			if (midtmpvar[getnum(m.arg2)].times == 0)
				MarkValidReg(reg2);
		}	// arg2 = num
		else
			MarkValidReg(reg2);
	}
	else{	//arg1 + IDEN
		int reg2 = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg2);
		if (entry.ekind != ENTRY_KIND::NOKIND){
			if (entry.ekind == ENTRY_KIND::ARGS){	//args
				fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg2, entry.argsnum * 4);	//arg2已经存入$t(reg2)
			}
			else{	//varnum
				fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg2, (entry.varnum + 1) * 4);	//arg2已经存入$t(reg2)
			}
		}
		else{
			fprintf(finalfile, "\tlw $t%d, %s \n", reg2, m.arg2);	//arg2已经存入$t(reg2)
		}
		if (m.arg1[0] == '$'){		// $t + IDEN
			int reg = ValidRegister();
			int reg1 = midtmpvar[getnum(m.arg1)].regnum;
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
			//storereg(reg);
			if (m.result[0] == '$')
				storereg(reg);
			else{
				TabEntry entry;
				entry = tab.getLocalVarLocation(nowfuncflag, m.result);
				if (entry.ekind != ENTRY_KIND::NOKIND){
					if (entry.ekind == ENTRY_KIND::ARGS)
						fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
					else
						fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
				}
				else
					fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
				MarkValidReg(reg);
			}
			midtmpvar[getnum(m.arg1)].times--;
			if (midtmpvar[getnum(m.arg1)].times == 0)
				MarkValidReg(reg1);
		}
		else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || (m.arg1[0] == '-')){
			int reg = ValidRegister();
			int reg1 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg1, m.arg1);
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
			//storereg(reg);
			if (m.result[0] == '$')
				storereg(reg);
			else{
				TabEntry entry;
				entry = tab.getLocalVarLocation(nowfuncflag, m.result);
				if (entry.ekind != ENTRY_KIND::NOKIND){
					if (entry.ekind == ENTRY_KIND::ARGS)
						fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
					else
						fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
				}
				else
					fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
				MarkValidReg(reg);
			}
			MarkValidReg(reg1);
		}
		else{	// IDEN + $t(reg)	IDEN不可能有数组
			int reg1 = ValidRegister();
			int reg = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg1, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg1, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s \n", reg1, m.arg1);	//取值存入寄存器，以便计算
			}
			fprintf(finalfile, "\tadd $t%d, $t%d, $t%d \n", reg, reg1, reg2);
			//storereg(reg);
			if (m.result[0] == '$')
				storereg(reg);
			else{
				TabEntry entry;
				entry = tab.getLocalVarLocation(nowfuncflag, m.result);
				if (entry.ekind != ENTRY_KIND::NOKIND){
					if (entry.ekind == ENTRY_KIND::ARGS)
						fprintf(finalfile, "\tsw $t%d, %d($fp)\n", reg, entry.argsnum * 4);
					else
						fprintf(finalfile, "\tsw $t%d, -%d($fp)\n", reg, (entry.varnum + 1) * 4);
				}
				else
					fprintf(finalfile, "\tsw $t%d, %s\n", reg, m.result);
				MarkValidReg(reg);
			}
			MarkValidReg(reg1);
		}
		MarkValidReg(reg2);
	}
}

//result = arg1 - arg2	arg: IDEN | $t	| num	IDEN要取出来放在寄存器里面才能使用
void MIPS::EmitSub()
{
	int reg = ValidRegister();
	int reg1, reg2;

	if (((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-') && ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || m.arg2[0] == '-')){
		reg1 = atoi(m.arg1);
		reg2 = atoi(m.arg2);
		fprintf(finalfile, "\tli $t%d, %d\n", reg, reg1 - reg2);
	}
	else{
		if (m.arg1[0] == '$'){
			reg1 = midtmpvar[getnum(m.arg1)].regnum;
			midtmpvar[getnum(m.arg1)].times--;
		}
		else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-'){
			reg1 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg1, m.arg1);
		}
		else{
			reg1 = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg1, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg1, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s\n", reg1, m.arg1);	//取值存入寄存器，以便计算
			}
		}

		if (m.arg2[0] == '$'){
			reg2 = midtmpvar[getnum(m.arg2)].regnum;
			midtmpvar[getnum(m.arg2)].times--;
		}
		else if ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || m.arg2[0] == '-'){
			reg2 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s\n", reg2, m.arg2);
		}
		else{
			reg2 = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg2);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg2, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg2, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s\n", reg2, m.arg2);	//取值存入寄存器，以便计算
			}
		}

		fprintf(finalfile, "\tsub $t%d, $t%d, $t%d\n", reg, reg1, reg2);
		if (m.arg1[0] == '$'){
			if (midtmpvar[getnum(m.arg1)].times == 0)
				MarkValidReg(reg1);
		}
		else MarkValidReg(reg1);
		if (m.arg2[0] == '$'){
			if (midtmpvar[getnum(m.arg2)].times == 0)
				MarkValidReg(reg2);
		}
		else MarkValidReg(reg2);
	}
	
	storereg(reg);
}

// result = arg1 * | / arg2	 arg: IDEN | $t	| num	乘除法
void MIPS::EmitMultDiv()
{
	char op[5];
	if (strcmp(m.op, "*\0") == 0)
		strcpy(op, "mult");
	else if (strcmp(m.op, "/\0") == 0)
		strcpy(op, "div");

	int reg = ValidRegister();
	int reg1, reg2;

	if (((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-') && ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || m.arg2[0] == '-')){
		reg1 = atoi(m.arg1);
		reg2 = atoi(m.arg2);
		if (strcmp(op, "mult") == 0)
			fprintf(finalfile, "\tli $t%d, %d\n", reg, reg1*reg2);
		else fprintf(finalfile, "\tli $t%d, %d\n", reg, reg1 / reg2);
	}
	else{
		if (m.arg1[0] == '$'){
			reg1 = midtmpvar[getnum(m.arg1)].regnum;
			midtmpvar[getnum(m.arg1)].times--;
		}
		else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || m.arg1[0] == '-'){
			reg1 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg1, m.arg1);
		}
		else{
			reg1 = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg1, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg1, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s\n", reg1, m.arg1);	//取值存入寄存器，以便计算
			}
		}

		if (m.arg2[0] == '$'){
			reg2 = midtmpvar[getnum(m.arg2)].regnum;
			midtmpvar[getnum(m.arg2)].times--;
		}
		else if ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || m.arg2[0] == '-'){
			reg2 = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s\n", reg2, m.arg2);
		}
		else{
			reg2 = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg2);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg2, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg2, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s\n", reg2, m.arg2);	//取值存入寄存器，以便计算
			}
		}

		fprintf(finalfile, "\t%s $t%d, $t%d \n", op, reg1, reg2);
		fprintf(finalfile, "\tmflo $t%d\n", reg);
		
		if (m.arg1[0] == '$'){
			if (midtmpvar[getnum(m.arg1)].times == 0)
				MarkValidReg(reg1);
		}
		else MarkValidReg(reg1);
		if (m.arg2[0] == '$'){
			if (midtmpvar[getnum(m.arg2)].times == 0)
				MarkValidReg(reg2);
		}
		else MarkValidReg(reg2);
	}

	storereg(reg);

	/*
	if (m.arg2[0] == '$'){
		if (m.arg1[0] == '$'){		// $t  $t
			int reg = ValidRegister();
			int reg1 = midtmpvar[getnum(m.arg1)].regnum;
			int reg2 = midtmpvar[getnum(m.arg2)].regnum;
			fprintf(finalfile, "\t%s $t%d, $t%d\n", op, reg1, reg2);
			fprintf(finalfile, "\tmflo $t%d\n", reg);
			storereg(reg);
			MarkValidReg(reg1);
			MarkValidReg(reg2);
		}
		else{	// IDEN - $t	IDEN可能有数组
			int reg1 = ValidRegister();
			int reg = ValidRegister();
			int reg2 = midtmpvar[getnum(m.arg2)].regnum;
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){	// arg1的值放入$t(reg1)
					fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg1, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg1, (entry.varnum + 1) * 4);
				}
			}
			else{
				//全局变量 非数组, 名字即地址，load即可
				fprintf(finalfile, "\tlw $t%d, %s\n", reg1, m.arg1);	//取值存入寄存器，以便计算
			}
			fprintf(finalfile, "\t%s $t%d, $t%d\n", op, reg1, reg2);
			fprintf(finalfile, "\tmflo $t%d\n", reg);
			storereg(reg);
			MarkValidReg(reg1);
			MarkValidReg(reg2);
		}
	}
	else{	//arg1 - IDEN
		int reg2 = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg2);
		if (entry.ekind != ENTRY_KIND::NOKIND){
			if (entry.ekind == ENTRY_KIND::ARGS){	//args
				fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg2, entry.argsnum * 4);	//arg2已经存入$t(reg2)
			}
			else{	//varnum
				fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg2, (entry.varnum + 1) * 4);	//arg2已经存入$t(reg2)
			}
		}
		else{
			fprintf(finalfile, "\tlw $t%d, %s\n", reg2, m.arg2);	//arg2已经存入$t(reg2)
		}
		if (m.arg1[0] == '$'){		// $t - IDEN
			int reg = ValidRegister();
			int reg1 = midtmpvar[getnum(m.arg1)].regnum;
			fprintf(finalfile, "\t%s $t%d, $t%d\n", op, reg1, reg2);
			fprintf(finalfile, "\tmflo $t%d\n", reg);
			storereg(reg);
			MarkValidReg(reg1);
		}
		else{	// IDEN - $t(reg2)	IDEN不可能有数组
			int reg1 = ValidRegister();
			int reg = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND)	//局部
			{
				if (entry.ekind == ENTRY_KIND::ARGS){
					fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg1, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg1, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s\n", reg1, m.arg1);	//取值存入寄存器，以便计算
			}
			fprintf(finalfile, "\t%s $t%d, $t%d\n", op, reg1, reg2);
			fprintf(finalfile, "\tmflo $t%d\n", reg);
			storereg(reg);
			MarkValidReg(reg1);
		}
		MarkValidReg(reg2);
	}*/
}

//  result arg1 == | != | > | >= | < | <= arg2	args : IDEN | $t | num
void MIPS::EmitComparison()
{
	int reg = ValidRegister();
	int reg1, reg2;

	if (m.arg1[0] == '$'){
		reg1 = midtmpvar[getnum(m.arg1)].regnum;
		midtmpvar[getnum(m.arg1)].times--;
	}
	else if ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || (m.arg1[0] == '-')){
		reg1 = ValidRegister();
		fprintf(finalfile, "\tli $t%d, %s\n", reg1, m.arg1);
	}
	else{
		reg1 = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
		if (entry.ekind != ENTRY_KIND::NOKIND){
			if (entry.ekind == ENTRY_KIND::ARGS){
				fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg1, entry.argsnum * 4);
			}
			else{
				fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg1, (entry.varnum + 1) * 4);
			}
		}
		else{
			fprintf(finalfile, "\tlw $t%d, %s\n", reg1, m.arg1);
		}
	}

	if (m.arg2[0] == '$'){
		reg2 = midtmpvar[getnum(m.arg2)].regnum;
		midtmpvar[getnum(m.arg2)].times--;
	}
	else if ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || (m.arg2[0] == '-')){
		reg2 = ValidRegister();
		fprintf(finalfile, "\tli $t%d, %s\n", reg2, m.arg2);
	}
	else{
		reg2 = ValidRegister();
		TabEntry entry;
		entry = tab.getLocalVarLocation(nowfuncflag, m.arg2);
		if (entry.ekind != ENTRY_KIND::NOKIND){
			if (entry.ekind == ENTRY_KIND::ARGS){
				fprintf(finalfile, "\tlw $t%d, %d($fp)\n", reg2, entry.argsnum * 4);
			}
			else{
				fprintf(finalfile, "\tlw $t%d, -%d($fp)\n", reg2, (entry.varnum + 1) * 4);
			}
		}
		else{
			fprintf(finalfile, "\tlw $t%d, %s\n", reg2, m.arg2);
		}
	}

	fprintf(finalfile, "\tsub $t%d, $t%d, $t%d\n", reg, reg1, reg2);
	storereg(reg);
	if (m.arg1[0] == '$'){
		if (midtmpvar[getnum(m.arg1)].times == 0)
			MarkValidReg(reg1);
	}
	else MarkValidReg(reg1);
	if (m.arg2[0] == '$'){
		if (midtmpvar[getnum(m.arg2)].times == 0)
			MarkValidReg(reg2);
	}
	else MarkValidReg(reg2);
}

//数组操作  *($t) $t  ||  $t *($t)
void MIPS::EmitArrayOp()
{
	//*($t) $t | num | IDEN
	if (m.result[0] == '*'){
		if (m.arg1[0] == '$'){
			fprintf(finalfile, "\tsw $t%d, 0($s%d)\n", midtmpvar[getnum(m.arg1)].regnum, midtmpvar[getarrnum(m.result)].regnum);
			midtmpvar[getarrnum(m.result)].times--;
			if (midtmpvar[getarrnum(m.result)].times == 0)
				MarkValidReg(midtmpvar[getarrnum(m.result)].regnum);
			midtmpvar[getnum(m.arg1)].times--;
			if (midtmpvar[getnum(m.arg1)].times == 0)
				MarkValidReg(midtmpvar[getnum(m.arg1)].regnum);
		}
		else if ((m.arg1[0] >= '0'&& m.arg1[0] <= '9') || m.arg1[0] == '-'){
			int reg = ValidRegister();
			fprintf(finalfile, "\tli $t%d, %s \n", reg, m.arg1);
			fprintf(finalfile, "\tsw $t%d, 0($s%d)\n", reg, midtmpvar[getarrnum(m.result)].regnum);
			MarkValidReg(reg);
		}
		else{
			int reg = ValidRegister();
			TabEntry entry;
			entry = tab.getLocalVarLocation(nowfuncflag, m.arg1);
			if (entry.ekind != ENTRY_KIND::NOKIND){
				if (entry.ekind == ENTRY_KIND::ARGS){
					fprintf(finalfile, "\tlw $t%d, %d($fp) \n", reg, entry.argsnum * 4);
				}
				else{
					fprintf(finalfile, "\tlw $t%d, -%d($fp) \n", reg, (entry.varnum + 1) * 4);
				}
			}
			else{
				fprintf(finalfile, "\tlw $t%d, %s \n", reg, m.arg1);
			}
			fprintf(finalfile, "\tsw $t%d, 0($s%d)\n", reg, midtmpvar[getarrnum(m.result)].regnum);
			MarkValidReg(reg);
		}
	}
	else{		// $t  | iden *($t)
		int reg = ValidRegister();
		fprintf(finalfile, "\tlw $t%d, 0($s%d)\n", reg, midtmpvar[getarrnum(m.arg1)].regnum);
		storereg(reg);
		midtmpvar[getarrnum(m.arg1)].times--;
		if (midtmpvar[getarrnum(m.arg1)].times == 0)
			MarkValidReg(midtmpvar[getarrnum(m.arg1)].regnum);
	}
}



MIPS::MIPS()
{
	regnum = -1;
	sregnum = -1;
	nowfuncflag = 0;
	for (int i = 0; i < 10; i++)
		t[i] = 0;
}

//给midtvar(四元式中的temp)记录他们对应的寄存器 t0 - t9
void MIPS::storereg(int t)
{
	int num, len = strlen(m.result);
	char tp[10];
	for (int i = 2, j = 0; i < len; i++, j++)
	{
		tp[j] = m.result[i];
	}
	num = atoi(tp);
	midtmpvar[num].regnum = t;
}

//返回四元式中临时寄存器p = $tx的数字x
int MIPS::getnum(char p[])
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
int MIPS::getarrnum(char p[])
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

