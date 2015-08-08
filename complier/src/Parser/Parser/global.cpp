#include "global.h"
#include "table.h"


int CurrentLine = 1;
int errnum = 0;
int tempnum = 0;	//$tx
int labelnum = 0;	//$Lx
int midcodeline = 0;	//中间代码的行数
int midcodeline1 = 0;	//局部优化中间代码的行数
int midcodeline2 = 0;	//窥孔优化中间代码的行数
int strnum = 0;	//字符串的个数
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