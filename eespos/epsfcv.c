/* eosfcv.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eps.h"
#include "fnlib.h"
#include "estr.h"

/* 年、月、日、曜日、時刻の出力 */

void tmdout(int init, FILE *fo, ESTL *Estl, TMDT *Tmdt)
{
	int i;
	
	if (init == 0)
	{
		for (i = 0; i < Estl->ntimeid; i++)
		{
			switch (Estl->timeid[i])
			{
			case 'Y':
				fprintf(fo, "Yr ");
				break;
			case 'M':
				fprintf(fo, "Mon ");
				break;
			case 'D':
				fprintf(fo, "Day ");
				break;
			case 'W':
				fprintf(fo, "Wkd ");
				break;
			case 'T':
				fprintf(fo, "Time ");
				break;
			}
		}
	}
	else
	{
		for (i = 0; i < Estl->ntimeid; i++)
			fprintf(fo, "%s ", Tmdt->dat[i]);
	}
}  
/* ----------------------------------------------------------- */

/* 表計算用ファイルへの出力 */

void esoout(int init, FILE *fi, FILE *fo, int Ndata, TLIST *Tlist)
{
	int i;   
	char s[NCHAR];
	
	if (init == 0)
	{
		for (i = 0; i < Ndata; i++, Tlist++)
		{
			if (Tlist->req == 'y')
				fprintf(fo, "%s_%s ", Tlist->name, Tlist->id);
		}
		fprintf(fo, "\n");      
	}
	else
	{    
		for (i = 0; i < Ndata; i++, Tlist++)
		{
			fscanf(fi, "%s", s);
			if (Tlist->req == 'y')
				fprintf(fo, "%s ", s);
		}
		fprintf(fo, "\n");      
	}
}

