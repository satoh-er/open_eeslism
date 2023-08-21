/* epstbl.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eps.h"
#include "fnlib.h"
#include "estr.h"

/* 作表（項目・期間）指定データの入力 */

void prtreqdc(int Narg, char *arg[], FILE *fii, ESTL *Estl, STATC *Pprd)
{
	PRQLIST *Prq;
	STATC  *Prd;
	int i, j, jj;
	char s[NCHAR], *st;
	
	if ((Estl->Prq = calloc(NPREQMAX, sizeof(PRQLIST))) == NULL)
		Ercalloc(NPREQMAX, "prtreqdc --- Prq");
	else
	{
		Prq = Estl->Prq ;
		for ( i = 0; i < NPREQMAX; i++, Prq++ )
		{
			Prq->Npname = Prq->Npid = 0 ;
			if (( Prq->prname = calloc ( sizeof ( char ** ), Narg )) == NULL )
				Ercalloc ( Narg, "Prq->prname" ) ;
			if (( Prq->prid = calloc ( sizeof ( char ** ), Narg )) == NULL )
				Ercalloc ( Narg, "Prq->prid" ) ;
		}
	}

	Prq = Estl->Prq;
	Estl->Nrqlist = 0;
	Estl->Nvreq = 0;
	
	Prd = Pprd;
	if (Narg > 1)
	{
		for (i = 1; i < Narg; i++) 
		{
			if (*arg[i] == '-')
			{
				Prq->mark = *(arg[i]+1);
				if(Prq->mark != 'p')
				{
					j = 0, jj = 0;
					Prq->Npname = Prq->Npid = 0;
				}
			}
			else if (*arg[i] == ';')
			{
				if (Prq->mark != 'p')
				{
					Prq++;
					Estl->Npreq = Prq - Estl->Prq;
				}
			}
			else
			{
				switch (Prq->mark)
				{
				case 'c':
					if ((st = strchr(arg[i], '_')) != NULL)
					{
						*st = '\0';
						Prq->prname[j++] = arg[i];
						Prq->Npname = j;
						Prq->prid[j] = st + 1;
					}
					else
						printf("xxx prtredc arg=%s\n", arg[i]);
					break;
					
				case 'n':
					if (*arg[i] == '_')
					{
						Prq->prid[jj++] = arg[i] + 1;
						Prq->Npid = jj;
					}
					else
					{
						Prq->prname[j++] = arg[i];
						Prq->Npname = j;
					}
					break;
				case 'p':
					daystrend(arg[i], &Pprd->mostart, &Pprd->daystart, &Pprd->moend,
						&Pprd->dayend);
					Pprd++;
					Estl->Npprd = Pprd - Prd;
					break;
				}
			}
		}
	}
	
	if (fii != NULL && (Estl->Npreq == 0 && Estl->Npprd == 0))
	{
		Prq = Estl->Prq;
		i = 0;
		while (fscanf(fii, "%s", s), *s != '*')
		{
			if (*s == '-')
			{
				Prq->mark = *(s + 1);
				if(Prq->mark != 'p')
				{
					j = 0, jj = 0;
					Prq->Npname = Prq->Npid = 0;
				}
			}
			else if (*s == ';')
			{
				if (Prq->mark != 'p')
				{
					Prq++;
					Estl->Npreq = Prq - Estl->Prq;
				}
			}	 
			else
			{
				switch (Prq->mark)
				{
				case 'c':
					if ((st = strchr(s, '_')) != NULL)
					{
						*st = '\0';
						Prq->prname[j++] = stralloc(s);
						Prq->Npname = j;
						Prq->prid[j] = stralloc(st + 1);
					}
					else
						printf("xxx prtredc s=%s\n", s);
					break;
				case 'n':
					if (*s == '_')
					{
						Prq->prid[jj++] = stralloc(s + 1);
						Prq->Npid = jj;
					}
					else
					{
						Prq->prname[j++] = stralloc(s);
						Prq->Npname = j;
					}
					break;
				case 'p':
					daystrend(arg[i], &Pprd->mostart, &Pprd->daystart, &Pprd->moend,
						&Pprd->dayend);
					Pprd++;
					Estl->Npprd = Pprd - Prd;
					break;
				}
			}
		}
	}
	
}
/* -------------------------------------------------------------------- */

/* 作表（項目・期間）指定データリストの作成 */
void prtreqlist(ESTL *Estl, int Ndata, TLIST *Tlist, TLIST *plist[])
{
	int  i, j, k, kk, id;
	PRQLIST *Prq;
	
	for (i = 0; i < Ndata; i++, Tlist++)
	{
		Tlist->req = 'n';
		Prq = Estl->Prq;
		for (j = 0; j < Estl->Npreq; j++, Prq++)
		{
			id = 0;
			if (Prq->mark == 'c')
			{
				for (k = 0; k < Prq->Npname; k++)
				{
					if (strcmp(Tlist->name, Prq->prname[k]) == 0 &&
						strcmp(Tlist->id, Prq->prid[k]))
					{
						if (id == 0)
							plist[j] = Tlist;		  
						id++;
						Tlist->req = 'p';
						if (k == (Prq->Npname - 1) && Tlist->req == 'p')
							Tlist->req = 'P';
					}
				}
			}
			else if (Prq->mark == 'n')
			{
				for (k = 0; k < Prq->Npname; k++)
				{
					if (strcmp(Tlist->cname, Prq->prname[k]) == 0 ||
						strcmp(Tlist->name, Prq->prname[k]) == 0)
					{
						for(kk = 0; kk < Prq->Npid; kk++)
						{
							if (strcmp(Tlist->id, Prq->prid[kk]) == 0)
							{
								if (id == 0)
									plist[j] = Tlist;
								id++;
								Tlist->req = 'p';
								if (k == (Prq->Npname - 1) && kk == (Prq->Npid - 1)
									&& Tlist->req == 'p')
									Tlist->req = 'P';
							}
						}
					}
				}
			}
		}
	}
}
/* -------------------------------------------------------------------- */

/* 出力期間指定 */

void daystrend(char *s, int *mostart, int *daystart, int *moend, int *dayend)
{
	if (strchr(s, '-') != NULL)
		sscanf(s, "%d/%d-%d/%d", mostart, daystart, moend, dayend);
	else
	{
		sscanf(s, "%d/%d", mostart, daystart);
		moend = mostart;
		dayend = daystart;
	}
}
