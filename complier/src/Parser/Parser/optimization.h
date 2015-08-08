#include "global.h"

#ifndef _OPTIMIZATION_H_
#define _OPTIMIZATION_H_

//�ڵ���еĽڵ���
typedef struct NodeEntry
{
	char name[MAX_STR_LEN];	//�ڵ���
	int node;		//���ڽڵ�
}NodeEntry;

//�ڵ��
typedef struct NodeTable
{
	NodeEntry nentry[MAX_STR_LEN];	//�ڵ�������
	int size;		//�ڵ���м����ڵ���
//	int nodenum;	//DAGͼ���м����ڵ�
}NodeTable;

//DAGͼ�нڵ�ṹ��
typedef struct Node
{
	int node;	//�ڼ����ڵ�
	char op[MAX_STR_LEN];	//���� or name
	char repr[MAX_STR_LEN];	//����ýڵ������
	int reprnum;	//��ʶ������
	int left;		//��ڵ�
	int right;		//�ҽڵ�
	bool ready;		//�Ƿ�����ջ��falseΪδ��ջ
}Node;

//DAGͼ�ṹ
typedef struct DAGTree
{
	Node nodes[MAX_STR_LEN];	//�ڵ�����
	NodeTable nodetable;		//�ڵ��
	std::stack<int> nodestack;	//�м�ڵ�����
	int nodenum;				//�ڵ�����
}DAGTree;

//�Ż���
class Optimizing
{
	DAGTree dagtree;
public:
	Optimizing();	//��ʼ��dagtree.nodenum=0,dagtree.nodetable.size=0

	void DAGUpdate();						//DAG���£���ʼ����
	int genDAG(int start, int block);		//����DAG
	void genOpCode();						//�����Ż������

	void kuikong();							//�����Ż�
	void LocalOptimize();					//�ֲ��Ż�
	void GlobalOptimize();					//ȫ���Ż�

	int FindOrBuild(char str[]);	//��nodetable����str���ҵ��˷��ؽڵ�i��δ�ҵ�����DAGͼ���½��ڵ�i���ڵ�������(str,i)
	int FindOP(char op[], int i, int j);	//��DAG��Ѱ���м�ڵ�op
	void FindResult(char str[], int k);	//�ڽڵ������str���ҵ��˽���ڵ�Ÿ���Ϊk;δ�ҵ����ڽڵ�����½�(str,k)

	bool CheckOP();				//����Ƿ����м�ڵ�δ�������
	int FindNoParentNode();		//���޸��ڵ�Ľڵ�
	bool isSon(int i);			//���ڵ�i�Ƿ��������ڵ���ӽڵ�
};

#endif