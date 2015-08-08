#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdlib.h"
#include "global.h"
#include "lexer.h"
#include "parser.h"
#include "mips.h"
#include "optimization.h"
#include<iostream>
#include<fstream>

using namespace std;


int main()
{
	MIPS mips = MIPS();
	Optimizing optimizing;
	//Parser parser;
	char filename[300];
	int a, size;
	cout << "请输入文件名:" << endl;
	cin >> filename;
	
	errfile = fopen("errfile.txt", "w");
	midfile = fopen("midfile.txt", "w");
	midfile1 = fopen("midfile1.txt", "w");
	midfile2 = fopen("midfile2.txt", "w");
	finalfile = fopen("finalfile.txt", "w");

	mips.parser.lexer.getProgram(filename);
	if (mips.parser.Program()){
		cout << "NO ERRORS!" << endl;
		for (int i = 0;; i++)
		{
			if (strcmp(midcode[i].result, "") != 0)
			{
				fprintf(midfile, "%-10s\t%-10s\t%-10s\t%-10s\t%-10d\n", midcode[i].result, midcode[i].arg1, midcode[i].op, midcode[i].arg2, midcode[i].block);
			}
			else {
				size = i;
				break;
			}
		}
/*		optimizing.LocalOptimize();
		for (int i = 0;; i++)
		{
			if (strcmp(midcode1[i].result, "") != 0)
			{
				fprintf(midfile1, "%-10s\t%-10s\t%-10s\t%-10s\n", midcode1[i].result, midcode1[i].arg1, midcode1[i].op, midcode1[i].arg2);
			}
			else {
				size = i;
				break;
			}
		}*/
		mips.EmitTotal(size);
	}
	else
		cout << "The program has " << errnum << " errors !" << endl;

	cin >> a;
	fclose(errfile);
	return 0;
}
