#include "global.h"

#ifndef _OPTIMIZATION_H_
#define _OPTIMIZATION_H_

//节点表中的节点项
typedef struct NodeEntry
{
	char name[MAX_STR_LEN];	//节点名
	int node;		//所在节点
}NodeEntry;

//节点表
typedef struct NodeTable
{
	NodeEntry nentry[MAX_STR_LEN];	//节点项数组
	int size;		//节点表中几个节点项
//	int nodenum;	//DAG图中有几个节点
}NodeTable;

//DAG图中节点结构体
typedef struct Node
{
	int node;	//第几个节点
	char op[MAX_STR_LEN];	//符号 or name
	char repr[MAX_STR_LEN];	//代表该节点的名字
	int reprnum;	//标识符数量
	int left;		//左节点
	int right;		//右节点
	bool ready;		//是否已入栈，false为未入栈
}Node;

//DAG图结构
typedef struct DAGTree
{
	Node nodes[MAX_STR_LEN];	//节点数组
	NodeTable nodetable;		//节点表
	std::stack<int> nodestack;	//中间节点序列
	int nodenum;				//节点数量
}DAGTree;

//优化类
class Optimizing
{
	DAGTree dagtree;
public:
	Optimizing();	//初始化dagtree.nodenum=0,dagtree.nodetable.size=0

	void DAGUpdate();						//DAG更新（初始化）
	int genDAG(int start, int block);		//生成DAG
	void genOpCode();						//生成优化后代码

	void kuikong();							//窥孔优化
	void LocalOptimize();					//局部优化
	void GlobalOptimize();					//全局优化

	int FindOrBuild(char str[]);	//在nodetable中找str，找到了返回节点i；未找到则在DAG图中新建节点i，节点表中添加(str,i)
	int FindOP(char op[], int i, int j);	//在DAG中寻找中间节点op
	void FindResult(char str[], int k);	//在节点表中找str，找到了将其节点号更改为k;未找到则在节点表中新建(str,k)

	bool CheckOP();				//检查是否有中间节点未进入队列
	int FindNoParentNode();		//找无父节点的节点
	bool isSon(int i);			//检查节点i是否是其他节点的子节点
};

#endif