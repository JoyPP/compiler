#include "global.h"
#include "table.h"


int CurrentLine = 1;
int errnum = 0;
int tempnum = 0;	//$tx
int labelnum = 0;	//$Lx
int midcodeline = 0;	//�м���������
int midcodeline1 = 0;	//�ֲ��Ż��м���������
int midcodeline2 = 0;	//�����Ż��м���������
int strnum = 0;	//�ַ����ĸ���
int blocknum = 0;

TEMP midtmpvar[MAX_TEMP_COUNT];
//TEMP midtmpvar1[MAX_TEMP_COUNT];
//TEMP midtmpvar2[MAX_TEMP_COUNT];

Table tab;
MidCode midcode[MAX_MID_LINE];
MidCode midcode1[MAX_MID_LINE];
MidCode midcode2[MAX_MID_LINE];

std::string allstring[MAX_STR_LEN];
std::stack<std::string> mystack;
std::stack<std::string> tmpstack;

FILE *midfile;
FILE *midfile1;
FILE *midfile2;
FILE *errfile;
FILE *finalfile;