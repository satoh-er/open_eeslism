/* epstbl2.c */

#include <stdio.h>

#include "eps.h"
#include "fnlib.h"

/*  表の出力 */
void prttbl(FILE *fo, ESTL *Estl, STATC *Pprd, TMDT *Tmdt, TLIST *Plist[])
{
	int   j,  p, t, Tms, Tme;  
	
	for (p = 0; p < Estl->Npprd; p++, Pprd++)
	{
		for (j = 0; j < Estl->Npreq; j++)
		{
			fprintf(fo, "%s\n\n", Estl->title);
			Tms = prtmdtstart(Estl, Tmdt, Pprd);
			Tme = prtmdtend(Estl, Tmdt, Pprd);
			tmdout(0, fo, Estl, Tmdt);
			tblout('n', fo, Plist[j], 0);
			tblout('i', fo, Plist[j], 0);
			tblout('u', fo, Plist[j], 0);
			
			for (t = Tms; t <= Tme; t++)
			{
				tmdout(1, fo, Estl, Tmdt + t);
				tblout('d', fo, Plist[j], t);
			}
		}
	}
}
/* ----------------------------------------------------- */

/* 出力開始日時の検索 */

int prtmdtstart(ESTL *Estl, TMDT *Tmdt, STATC *Pprd)
{
	int i;
	
	for (i = 0; i < Estl->Ntime; i++, Tmdt++)
	{
		if (Pprd->mostart == Tmdt->Mon && Pprd->daystart == Tmdt->Day)
		{
			if (Estl->tid == 'h')
			{
				if (Estl->dtm == 3600)
				{
					if (Tmdt->Time == 100)
						return i;
				}
				else
				{
					if (Tmdt->Time == 60 * Estl->dtm / 3600)
						return i;
				}
			}
			else
				return i;
		}
	}
	
	return 0;
}
/* ----------------------------------------------------- */

/* 出力終了日時の検索 */

int prtmdtend(ESTL *Estl, TMDT *Tmdt, STATC *Pprd)
{
	int i;
	
	for (i = 0; i < Estl->Ntime; i++, Tmdt++)
	{
		if (Pprd->moend == Tmdt->Mon && Pprd->dayend == Tmdt->Day)
		{
			if (Estl->tid == 'h')
			{
				if (Tmdt->Time == 2400)
					return i;
			}
			else
				return i;
		}	 
	}
	
	return 0;
}
/* -------------------------------------------------------- */

/* 作表用データの出力 */

void tblout(char key, FILE *fo, TLIST *Plist, int t)
{
	for( ;Plist->req == 'P'; Plist++)
	{
		if(Plist->req != 'n')
		{
			if (key == 'd')
			{
				switch (Plist->ptype)
				{
				case 'f': 
					fprintf(fo, Plist->fmt, Plist->fval[t]);
					break;
				case 'd':
					fprintf(fo, Plist->fmt, Plist->ival[t]);
					break;
				case 'c':
					fprintf(fo, Plist->fmt, Plist->cval[t]);
					break;
				}
			}
			else if (key == 'n')
				fprintf(fo, "%10s", Plist->name);
			else if (key == 'i')
				fprintf(fo, "%10s", Plist->id);
			else if (key == 'u')
				fprintf(fo, "%10s", Plist->unit);
			
		}
	}
	fprintf(fo, "\n");
}
