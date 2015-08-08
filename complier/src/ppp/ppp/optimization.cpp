#define _CRT_SECURE_NO_WARNINGS
#include "optimization.h"

Optimizing::Optimizing()
{
	dagtree.nodenum = 0;
	dagtree.nodetable.size = 0;
}

//DAG���£���ʼ����
void Optimizing::DAGUpdate()
{
	for (int i = 1; i <= dagtree.nodenum; i++){
		dagtree.nodes[i].left = 0;
		dagtree.nodes[i].node = 0;
		dagtree.nodes[i].right = 0;
		strcpy(dagtree.nodes[i].op, "");
		strcpy(dagtree.nodes[i].repr, "");
		dagtree.nodes[i].ready = false;
	}
	dagtree.nodenum = 0;
	for (int j = 0; j < dagtree.nodetable.size; j++){
		dagtree.nodetable.nentry[j].node = 0;
		strcpy(dagtree.nodetable.nentry[j].name, "");
	}
	dagtree.nodetable.size = 0;
}

//����DAG
int Optimizing::genDAG(int start, int block)
{
	int end = start, nodenum = 0;
	MidCode m;
	for (;; end++)
	{
		char result[MAX_STR_LEN], arg1[MAX_STR_LEN], op[MAX_STR_LEN], arg2[MAX_STR_LEN];
		m = midcode[end];
		if (m.block == block)
		{
			if (strcmp(m.arg2, "") == 0)
			{
				if (strcmp(m.op, "") == 0)	//printf $tx   ||  IDEN $tx | IDEN | num   ||  PushStack  iden | $t  ||  PopStack  num  ||  *($t) $t �����ֹ� ||   $t  *($t)	����ĳ��ֹ�
				{
					if (strcmp(m.arg1, "") == 0){
						genOpCode();
						middle1(m.result, m.arg1, m.op, m.arg2);
						DAGUpdate();
					}
					else{
						int locnode = 0;

						//Ѱ��$tx�ڽڵ���λ��locnode��Ȼ����
						if ((strcmp(m.result, "Printf") == 0 || strcmp(m.result, "PushStack") == 0 || strcmp(m.result, "Return") == 0) && m.arg1[0] == '$' && m.arg1[1] == 't'){	//printf $t
							genOpCode();
							int i = FindOrBuild(m.arg1);
							strcpy(arg1, dagtree.nodes[i].repr);
							middle1(m.result, arg1, "", "");
							DAGUpdate();
						}
						else if (strcmp(m.result, "BeginFunc") == 0 || strcmp(m.result, "PushStack") == 0 || strcmp(m.result, "LCALL") == 0 || strcmp(m.result, "PopStack") == 0 ||
							strcmp(m.result, "GOTO") == 0 || strcmp(m.result, "Return") == 0 || strcmp(m.result, "VAR") == 0 || strcmp(m.result, "Printf") == 0 || strcmp(m.result, "Scanf") == 0){
							genOpCode();
							middle1(m.result, m.arg1, m.op, m.arg2);
							DAGUpdate();
						}
						else if (m.result[0] != '*' && m.arg1[0] != '*'){		//  IDEN $tx | IDEN | num  ||  $t num
							if (strcmp(m.arg1, midcode[end + 1].result) == 0 && strcmp(midcode[end + 1].op, "") == 0 && strcmp(midcode[end + 1].arg2, "") == 0
								&& strcmp(midcode[end + 1].arg1, midcode[end + 2].result) == 0 && strcmp(midcode[end + 2].op, "") == 0 && strcmp(midcode[end + 2].arg2, "") == 0
								&& strcmp(m.result, midcode[end + 2].arg1) == 0){	//�滻
								genOpCode();
								middle1(m.result, m.arg1, m.op, m.arg2);
								middle1(midcode[end + 1].result, midcode[end + 1].arg1, midcode[end + 1].op, midcode[end + 1].arg2);
								middle1(midcode[end + 2].result, midcode[end + 2].arg1, midcode[end + 2].op, midcode[end + 2].arg2);
								end += 2;
								DAGUpdate();
							}
							else{
								int i;
								i = FindOrBuild(m.arg1);	//��nodetable����arg1��node�����ظ�i��
								FindResult(m.result, i);	//��result���ڽڵ�i��
							}
						}
						else			//  *($t) $t �����ֹ� ||   $t  *($t)	����ĳ��ֹ������������޸�
						{
							if (m.result[0] == '*'){
								int i;
								i = FindOrBuild(m.arg1);
								strcpy(arg1, dagtree.nodes[i].repr);
								genOpCode();
								middle1(m.result, arg1, m.op, m.arg2);
								DAGUpdate();
							}
							else{
								genOpCode();
								middle1(m.result, m.arg1, m.op, m.arg2);
								DAGUpdate();
							}
						}
					}
				}
				else{
					genOpCode();
					middle1(m.result, m.arg1, m.op, m.arg2);
					DAGUpdate();
				}
			}
			else{	// result arg1 op arg2
				if (strcmp(m.result, "IFZ") != 0 && strcmp(m.op, "==") != 0 && strcmp(m.op, "!=") != 0 && strcmp(m.op, ">=") != 0 && strcmp(m.op, "<=") != 0 && strcmp(m.op, ">\0") != 0 && strcmp(m.op, "<\0") != 0){
					int i, j, k;
					i = FindOrBuild(m.arg1);
					j = FindOrBuild(m.arg2);
					k = FindOP(m.op, i, j);
					FindResult(m.result, k);
				}
				else if (strcmp(m.op, "==") == 0 || strcmp(m.op, "!=") == 0 || strcmp(m.op, ">=") == 0 || strcmp(m.op, "<=") == 0 || strcmp(m.op, ">\0") == 0 || strcmp(m.op, "<\0") == 0)
				{
					genOpCode();
					int i, j, k;
					i = FindOrBuild(m.arg1);
					j = FindOrBuild(m.arg2);
					strcpy(arg1, dagtree.nodes[i].repr);
					strcpy(arg2, dagtree.nodes[j].repr);
					middle1(m.result, arg1, m.op, arg2);
					DAGUpdate();
				}
				else if (strcmp(m.result, "IFZ") == 0 && m.arg1[0] == '$'){	//	IFZ  $t  GOTO $Lx
					genOpCode();
					int i = FindOrBuild(m.arg1);
					strcpy(arg1, dagtree.nodes[i].repr);
					middle1(m.result, arg1, m.op, m.arg2);
					DAGUpdate();
				}
				else {
					genOpCode();
					middle1(m.result, m.arg1, m.op, m.arg2);
					DAGUpdate();
				}
			}
		}
		else{
			genOpCode();
			end--;
			break;
		}
	}
	return end;
}

//�ֲ��Ż�
void Optimizing::LocalOptimize()
{
	int pointblock = 0;
	for (int i = 0;; i++)
	{
		if (midcode[i].block == pointblock){
			DAGUpdate();
			i = genDAG(i, pointblock);
		}
		else if (midcode[i].block > pointblock)
		{
			pointblock++;
			i--;
		}
		else if (pointblock == blocknum - 1){
			break;
		}
	}
	CreateMidtmp1();
	//kuikong();
}

//ȫ���Ż�
void Optimizing::GlobalOptimize()
{

}

//��nodetable����str���ҵ��˷��ؽڵ�i��δ�ҵ�����DAGͼ���½��ڵ�i���ڵ�������(str,i)
int Optimizing::FindOrBuild(char str[])
{
	bool flag = false;
	//��nodetable����str���ҵ��˷��ؽڵ�i
	for (int i = 0; i < dagtree.nodetable.size; i++)
	{
		if (strcmp(dagtree.nodetable.nentry[i].name, str) == 0){
			return dagtree.nodetable.nentry[i].node;
		}
	}
	//δ�ҵ�����DAG���½�Ҷ�ڵ�i,	op = str
	dagtree.nodenum++;
	strcpy(dagtree.nodes[dagtree.nodenum].op, str);
	dagtree.nodes[dagtree.nodenum].node = dagtree.nodenum;
	strcpy(dagtree.nodes[dagtree.nodenum].repr, str);

	//dagtree.nodes[dagtree.nodenum].reprnum++;

	//�ڵ�������(str,i)
	strcpy(dagtree.nodetable.nentry[dagtree.nodetable.size].name, str);
	dagtree.nodetable.nentry[dagtree.nodetable.size].node = dagtree.nodenum;
	dagtree.nodes[dagtree.nodenum].ready = false;
	dagtree.nodetable.size++;
	return dagtree.nodenum;
}

//��DAG��Ѱ���м�ڵ�op,��������ڵ�Ϊi���Ҳ����ڵ�Ϊj������ҵ���¼��ڵ�k��δ�ҵ�����DAG���½��ڵ�k�����ҽڵ�ֱ�Ϊi��j
int Optimizing::FindOP(char op[], int i, int j)
{
	for (int m = 1; m <= dagtree.nodenum; m++)
	{
		if (strcmp(dagtree.nodes[m].op, op) == 0 && dagtree.nodes[m].left == i && dagtree.nodes[m].right == j)
		{
			return m;
		}
	}
	//δ�ҵ�����DAG���½�Ҷ�ڵ�k,	op = op
	dagtree.nodenum++;
	dagtree.nodes[dagtree.nodenum].node = dagtree.nodenum;
	strcpy(dagtree.nodes[dagtree.nodenum].op, op);
	dagtree.nodes[dagtree.nodenum].left = i;
	dagtree.nodes[dagtree.nodenum].right = j;
	dagtree.nodes[dagtree.nodenum].ready = false;
	return dagtree.nodenum;
}

//�ڽڵ������str���ҵ��˽���ڵ�Ÿ���Ϊk;δ�ҵ����ڽڵ�����½�(str,k)
void Optimizing::FindResult(char str[], int k)
{
	for (int i = 0; i < dagtree.nodetable.size; i++)
	{
		if (strcmp(dagtree.nodetable.nentry[i].name, str) == 0)
		{
			dagtree.nodetable.nentry[i].node = k;
			//repr�ǷǱ�ʶ����str�Ǳ�ʶ��
			if ((!((dagtree.nodes[k].repr[0] >= 'A' && dagtree.nodes[k].repr[0] <= 'Z') || (dagtree.nodes[k].repr[0] >= 'a' && dagtree.nodes[k].repr[0] <= 'z') || (dagtree.nodes[k].repr[0] == '_')))
				&& (str[0] != '$' && !((str[0] >= '0'&&str[0] <= '9') || (str[0] == '-')))){
				strcpy(dagtree.nodes[k].repr, str);
			}
			return;
		}
	}

	//��IDEN | $tx | num  repr��δ��ֵ
	if (!((dagtree.nodes[k].repr[0] >= 'A' && dagtree.nodes[k].repr[0] <= 'Z') || (dagtree.nodes[k].repr[0] >= 'a' && dagtree.nodes[k].repr[0] <= 'z') || (dagtree.nodes[k].repr[0] == '_'))
		&& !(dagtree.nodes[k].repr[0] == '$') && !((dagtree.nodes[k].repr[0] >= '0' && dagtree.nodes[k].repr[0] <= '9') || (dagtree.nodes[k].repr[0] == '-')))
	{
		strcpy(dagtree.nodes[k].repr, str);
	}

	//δ�ҵ����ڽڵ�����½�(str,k)
	/*
		if ((!((dagtree.nodes[k].repr[0] >= 'A' && dagtree.nodes[k].repr[0] <= 'Z') || (dagtree.nodes[k].repr[0] >= 'a' && dagtree.nodes[k].repr[0] <= 'z') || (dagtree.nodes[k].repr[0] == '_')))
		&& (str[0] != '$' && !((str[0] >= '0'&&str[0] <= '9') || (str[0] == '-')))){
		strcpy(dagtree.nodes[k].repr, str);
		}
		*/
	strcpy(dagtree.nodetable.nentry[dagtree.nodetable.size].name, str);
	dagtree.nodetable.nentry[dagtree.nodetable.size].node = k;
	dagtree.nodetable.size++;
}

//�����Ż������
void Optimizing::genOpCode()
{
	//�����м�ڵ�δ�������
	while (CheckOP()){
		//������һ��û�и��ڵ���м�ڵ�
		int i, j;
		i = FindNoParentNode();
		dagtree.nodestack.push(i);
		dagtree.nodes[i].ready = true;
		j = dagtree.nodes[i].left;
		while (!isSon(j)){
			dagtree.nodestack.push(j);
			dagtree.nodes[j].ready = true;
			j = dagtree.nodes[j].left;
		}
	}
	for (int j = 1; j <= dagtree.nodenum; j++){
		if (dagtree.nodes[j].left <= 0 && dagtree.nodes[j].right <= 0){	//�ӽڵ�
			for (int x = 0; x < dagtree.nodetable.size; x++){
				if (dagtree.nodetable.nentry[x].node == j){
					if (strcmp(dagtree.nodetable.nentry[x].name, dagtree.nodes[j].repr) != 0 && dagtree.nodetable.nentry[x].name[0] != '$' && !((dagtree.nodetable.nentry[x].name[0] >= '0' && dagtree.nodetable.nentry[x].name[0] <= '9') || (dagtree.nodetable.nentry[x].name[0] == '-'))){
						middle1(dagtree.nodetable.nentry[x].name, dagtree.nodes[j].repr, "", "");
					}
				}
			}
		}
	}
	while (!dagtree.nodestack.empty()){
		int i = dagtree.nodestack.top();	//ȡtop����ֵ
		int tmpname[MAX_STR_LEN];
		dagtree.nodestack.pop();			//
		//�õ�ջ���ڵ㣬������Ԫʽ
		middle1(dagtree.nodes[i].repr, dagtree.nodes[dagtree.nodes[i].left].repr, dagtree.nodes[i].op, dagtree.nodes[dagtree.nodes[i].right].repr);
		for (int j = 0; j < dagtree.nodetable.size; j++){
			if (dagtree.nodetable.nentry[j].node == i){
				if (strcmp(dagtree.nodetable.nentry[j].name, dagtree.nodes[i].repr) != 0 && dagtree.nodetable.nentry[j].name[0] != '$' && !((dagtree.nodetable.nentry[j].name[0] >= '0' && dagtree.nodetable.nentry[j].name[0] <= '9') || (dagtree.nodetable.nentry[j].name[0] == '-'))){
					middle1(dagtree.nodetable.nentry[j].name, dagtree.nodes[i].repr, "", "");
				}
			}
		}
	}
}

//����Ƿ����м�ڵ�δ�������
bool Optimizing::CheckOP()
{
	for (int i = 1; i <= dagtree.nodenum; i++)
	{
		if (!dagtree.nodes[i].ready && dagtree.nodes[i].left > 0 && dagtree.nodes[i].right > 0)
			return true;
	}
	return false;
}

//���޸��ڵ�Ľڵ�
int Optimizing::FindNoParentNode()
{
	for (int i = dagtree.nodenum; i >= 1; i--)
	{
		if (!isSon(i) && !dagtree.nodes[i].ready){	//�ڵ�i�����ӽڵ�Ҳδ��ջ
			return i;
		}
	}
	return 0;
}

//���ڵ�i�Ƿ��������ڵ���ӽڵ�
bool Optimizing::isSon(int j)
{
	if (dagtree.nodes[j].left <= 0 && dagtree.nodes[j].right <= 0)
		return true;
	for (int i = 1; i <= dagtree.nodenum; i++)
	{
		if (!dagtree.nodes[i].ready){//δ��ջ
			if (dagtree.nodes[i].left == j || dagtree.nodes[i].right == j)
				return true;	//���ӽڵ�
		}
	}
	return false;
}


void Optimizing::kuikong()
{
	MidCode m;
	int i = 0;
	char ifendflag[MAX_STR_LEN], elsestartflag[MAX_STR_LEN];
	for (;; i++){
		m = midcode1[i];
		if (strcmp(m.arg1, "LCALL") == 0 && strcmp(m.result, midcode[i + 1].arg1) == 0 && strcmp(midcode[i + 1].op, "") == 0 && strcmp(midcode[i + 1].arg2, "") == 0){
			middle2(midcode[i + 1].result, m.arg1, m.op, m.arg2);
			i++;
		}
		else if (strcmp(m.result, "IFZ") == 0 && ((m.arg1[0] >= '0' &&m.arg1[0] <= '9') || (m.arg1[0] == '-'))){
			int num;
			num = atoi(m.arg1);
			if (num != 0){
				strcpy(elsestartflag, m.arg2);
				int len = strlen(elsestartflag);
				elsestartflag[len] = ':';
				elsestartflag[len + 1] = '\0';
				for (int j = i + 1;; j++){
					if (strcmp(elsestartflag, midcode1[j].result) == 0){
						strcpy(ifendflag, midcode1[j - 1].arg1);
						int lenif = strlen(ifendflag);
						ifendflag[lenif] = ':';
						ifendflag[lenif + 1] = '\0';
						while (strcmp(ifendflag, midcode1[j].result) != 0)
						{
							strcpy(midcode1[j].result, "VAR");
							strcpy(midcode1[j].arg1, "NONE");
							strcpy(midcode1[j].op, "");
							strcpy(midcode1[j].arg2, "");
							j++;
						}
						break;
					}
				}
			}
			else{
				char tmp[MAX_STR_LEN];
				strcpy(tmp, m.arg2);
				int len = strlen(tmp);
				tmp[len] = ':';
				tmp[len + 1] = '\0';
				for (int j = i;; j++){
					
					if (strcmp(midcode1[j].result, tmp) == 0){
						i = j - 1;
						break;
					}
				}
			}
		}
		else if (strcmp(m.result, "IFZ") != 0 && strcmp(m.arg2, "") != 0 && ((m.arg1[0] >= '0' && m.arg1[0] <= '9') || (m.arg1[0] == '-')) && ((m.arg2[0] >= '0' && m.arg2[0] <= '9') || (m.arg2[0] == '-'))){
			if (strcmp(m.op, "+\0") == 0 || strcmp(m.op, "-\0") == 0 || strcmp(m.op, "*\0") == 0 || strcmp(m.op, "/\0") == 0){
				int num1, num2, num;
				char temp[MAX_STR_LEN];
				num1 = atoi(m.arg1);
				num2 = atoi(m.arg2);
				if (strcmp(m.op, "+\0") == 0)
					num = num1 + num2;
				else if (strcmp(m.op, "-\0") == 0)
					num = num1 - num2;
				else if (strcmp(m.op, "*\0") == 0)
					num = num1 * num2;
				else if (strcmp(m.op, "/\0") == 0)
					num = num1 / num2;
				sprintf(temp, "%d", num);
				middle2(m.result, temp, "", "");
			}
			else{
				int num1, num2, num;
				char temp[MAX_STR_LEN];
				num1 = atoi(m.arg1);
				num2 = atoi(m.arg2);
				if (strcmp(m.op, "==") == 0)
					num = (num1 == num2) ? 1 : 0;
				else if (strcmp(m.op, "!=") == 0)
					num = (num1 != num2) ? 1 : 0;
				else if (strcmp(m.op, ">=") == 0)
					num = (num1 >= num2) ? 1 : 0;
				else if (strcmp(m.op, "<=") == 0)
					num = (num1 <= num2) ? 1 : 0;
				else if (strcmp(m.op, ">\0") == 0)
					num = (num1 > num2) ? 1 : 0;
				else if (strcmp(m.op, "<\0") == 0)
					num = (num1 < num2) ? 1 : 0;
				if (num == 1){
					i++;
					strcpy(elsestartflag, midcode1[i].arg2);
					int len = strlen(elsestartflag);
					elsestartflag[len] = ':';
					elsestartflag[len + 1] = '\0';
					for (int j = i + 1;; j++){
						if (strcmp(elsestartflag, midcode1[j].result) == 0){
							strcpy(ifendflag, midcode1[j - 1].arg1);
							int lenif = strlen(ifendflag);
							ifendflag[lenif] = ':';
							ifendflag[lenif + 1] = '\0';
							while (strcmp(ifendflag,midcode1[j].result)!=0)
							{
								strcpy(midcode1[j].result, "VAR");
								strcpy(midcode1[j].arg1, "NONE");
								strcpy(midcode1[j].op, "");
								strcpy(midcode1[j].arg2, "");
								j++;
							}
							break;
						}
					}
				}
				else{
					char tmp[MAX_STR_LEN];
					strcpy(tmp, midcode1[i + 1].arg2);
					int len = strlen(tmp);
					tmp[len] = ':';
					tmp[len + 1] = '\0';
					for (int j = i;; j++){
						if (strcmp(midcode1[j].result, tmp) == 0){
							i = j - 1;
							break;
						}
					}
				}
			}
		}
		else if (strcmp(m.result, "") == 0){
			break;
		}
		else{
			middle2(m.result, m.arg1, m.op, m.arg2);
		}
	}
	CreateMidtmp2();
}