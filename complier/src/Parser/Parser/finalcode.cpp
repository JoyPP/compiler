#include "global.h"
#include "parser.h"

void Emit()
{
	int t[10] = {0};	//	t0 - t9		1 : 被占用	0 ：可用 
	MidCode tempmid;

	fprintf(finalfile,"\t.text\n");
	fprintf(finalfile,"\t.align 2\n");
	fprintf(finalfile,"\t.globl main\n");

	for (int i = 0;; i++)
	{
		tempmid = midcode[i];
		if (strcmp(tempmid.arg2, "") == 0)
		{
			if (strcmp(tempmid.op, "") == 0)
			{
				if (strcmp(tempmid.arg1, "") == 0)		//  result  ""  ""  ""
				{
					if (strcmp(tempmid.result, "EndFunc") != 0)	//	_Lx:  |   _func:
					{
						fprintf(finalfile, "%s\n", tempmid.result);
					}
					else	//EndFunc
					{
						fprintf(finalfile, "\t#%s\n", tempmid.result);
						fprintf(finalfile, "\tmove $sp, $fp\t%s\n", "# pop callee frame off stack");
						fprintf(finalfile, "\tlw $ra, -4($sp)\t%s\n", "# restore saved ra");
						fprintf(finalfile, "\tlw $fp, 0($fp)\t%s\n", "# restore saved fp");
						fprintf(finalfile, "\tjr $ra\t%s\n", "# return from function");
					}
				}
				/*else 		//  result  arg1  ""  ""
				{
					if (strcmp(tempmid.result, "BeginFunc") == 0)	//ok
					{
						fprintf(finalfile, "\t#%s %s\n", tempmid.result, tempmid.arg1);
						fprintf(finalfile, "\tsubu $sp, $sp, 8\t%s\n", "# decrement sp to make space to save ra,fp");
						fprintf(finalfile, "\tsw $fp, 8($sp)\t%s\n", "# save fp");
						fprintf(finalfile, "\tsw $ra, 4($sp)\t%s\n", "# save ra");
						fprintf(finalfile, "\taddiu $fp, $sp, 8\t%s\n", "# set up new fp");
						fprintf(finalfile, "\tsubu $sp, $sp, %s\t%s\n", tempmid.arg1, "# decrement sp to make space for locals/temps");
					}
					else if (strcmp(tempmid.result, "PushStack") == 0)	//unfinished
					{
						fprintf(finalfile, "\tsubu $sp, $sp, 4\t%s\n", "# decrement sp to make space for temps");
						fprintf(finalfile, "\tsw \t%s\n", "# copy value to stack");
					}
					else if (strcmp(tempmid.result, "LCALL") == 0)
					{

					}
					else if (strcmp(tempmid.result, "PopStack") == 0)	// ok
					{
						fprintf(finalfile, "\tadd $sp, $sp, %s\t%s\n", tempmid.arg1, "# pop off the stack");
					}
					else if (strcmp(tempmid.result, "GOTO") == 0)
					{

					}
					else if (strcmp(tempmid.result, "Return") == 0)
					{

					}
					else if (strcmp(tempmid.result, "VAR") == 0)
					{
						
					}
					else
					{
						if (tempmid.result[0] == '$')
						{
							if (tempmid.arg1[0] == '')
						}
					}
				}*/
			}
			else	// result arg1 op ""
			{
				if (strcmp(tempmid.arg1, "LCALL") == 0)
				{

				}
				else
				{
					fprintf(finalfile,"intermedia code is wrong\n");
				}
			}
		}
		else
		{
			if (strcmp(tempmid.op, "+\0") == 0)
			{
				
			}
			else if (strcmp(tempmid.op, "-\0") == 0)
			{

			}
			else if (strcmp(tempmid.op, "*\0") == 0)
			{

			}
			else if (strcmp(tempmid.op, "/\0") == 0)
			{

			}
		}
		
	}
}