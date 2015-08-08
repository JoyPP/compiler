
#include "global.h"
#include "parser.h"

#ifndef _MIPS_H_
#define _MIPS_H_

//����mips���
class MIPS
{
public:
	Parser parser;	//�����﷨����
	MidCode m;	//�м����m
	int t[10];	//register t0 - t9   1 : invalid  0 : valid
	int i;		//��ǰ������Ԫʽ�ı��
	int regnum;	//��ʱ�Ĵ���t����ʼ��Ϊ-1
	int sregnum;	//��ʱ�Ĵ���s����ʼ��Ϊ-1
	int nowfuncflag;	//��ǰ���ں����ڷ��ű��е�λ��
	int ValidRegister();	//ȡ����t�Ĵ����ı��
	int ValidSRegister();	//ȡ����s�Ĵ����ı��

	MIPS();	//���캯��

	void EmitTotal(int size);	//������Ԫʽ������mips���

	void EmitLoadConstant();	//������ֵ�������
	void EmitLoadString();		//�ַ�����ֵ�������
	void EmitBeginFunc();		//������ʼ��־�������
	void EmitPushStack();		//��ջ�������
	void EmitCallNonRtnFunc();	//�����޷���ֵ�����������
	void EmitCallRtnFunc();		//�����з���ֵ�����������
	void EmitPopStack();		//��ջ�������
	void EmitIFZ();				//if�ж����������
	void EmitGoto();			//��������ת�������
	void EmitReturn();			//�������������
	void EmitEndFunc();			//����������־�������
	void EmitEndProgram();		//���������־�������
	void EmitLabel();			//������ǩ����ת��ǩ�������
	void EmitPrintf();			//��������������
	void EmitScanf();			//���뺯���������
	void EmitIDENAssign();		//��ʶ����ֵ�������
	void EmitArrayOp();			//��������������
	
	void EmitAdd();		//�ӷ��������
	void EmitSub();		//�����������
	void EmitMultDiv();		//�˳����������
	void EmitComparison();	//�Ƚ�����������

	void storereg(int t);	//��Ĵ���t��ʹ��
	int getnum(char p[]);	//ȡ������$tx����ʱ�Ĵ�����ֵx
	int getarrnum(char p[]);//ȡ������*($tx)����ʱ�Ĵ�����ֵx
	void MarkValidReg(int reg);//�ͷżĴ���t(reg)ʹ֮���Ա��ٴ�ʹ��
	void MarkInvalidReg(int reg);//ʹ�üĴ���t(reg)�������Ϊ����ʹ��
};






#endif