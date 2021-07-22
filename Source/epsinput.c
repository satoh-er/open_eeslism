//This file is part of EESLISM.
//
//Foobar is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Foobar is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Foobar.If not, see < https://www.gnu.org/licenses/>.

/* epsinput.c */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnfio.h"
#include "fnlib.h"


/* シミュレーション結果、標題、識別データの入力 */

void esondat(FILE *fi, ESTL *Estl)
{
	char  s[SCHAR];
	int   i, j, Nparm, Ndat, N, ss ;
	CATNM *catnm, *C;
	
	Estl->catnm = NULL;
	
	while (fscanf(fi, "%s", s), *s != '#')
	{
		if (strcmp(s, "-t") == 0)
		{
			fscanf(fi, " %[^;];", s);
			Estl->title = stralloc(s);
		}
		else if (strcmp(s, "-w") == 0)
		{
			fscanf(fi, "%s", s);
			Estl->wdatfile = stralloc(s);
		}	 
		else if (strcmp(s, "-tid") == 0)
		{
			fscanf(fi, " %c", &Estl->tid);
		}
		else if (strcmp(s, "-u") == 0)
		{
			i = 0;
			while (fscanf(fi, "%s", s), *s != ';')
			{
				Estl->unit[i] = stralloc(s);
				i++ ;
				Estl->Nunit = i;
			}
		}
		else if (strcmp(s, "-Ntime") == 0)
			fscanf(fi, " %d",  &Estl->Ntime);
		
		else if (strcmp(s, "-dtm") == 0)
			fscanf(fi, " %d",  &Estl->dtm);
		
		else if (strcmp(s, "-tmid") == 0)
		{
			fscanf(fi, "%s", s);
			Estl->timeid = stralloc(s);
			Estl->ntimeid = (int)strlen(Estl->timeid);
		}
		else if (strcmp(s, "-cat") == 0) 
		{
			N = CATNMMAX ;
			if (N > 0 && (Estl->catnm = (CATNM *)malloc(N * sizeof(CATNM))) == NULL)
				Ercalloc(CATNMMAX, "esondat -- catnm");

			if ( Estl->catnm != NULL )
			{
				C = Estl->catnm ;

				for ( ss = 0; ss < N; ss++, C++ )
				{
					C->name = NULL ;
					C->N = C->Ncdata = 0 ;
				}
			}

			Estl->Ndata = 0;
			catnm = Estl->catnm;
			while(fscanf(fi, "%s", s), *s != '*')
			{
				catnm->name = stralloc(s);
				catnm->Ncdata = 0;
				fscanf(fi, "%d", &catnm->N);
				for (i = 0; i < catnm->N; i++)
				{
					fscanf(fi, "%s %d %d", s, &Nparm, &Ndat);
					for (j = 0; j < Nparm - 1; j++)
						fscanf(fi, "%s", s);
					Estl->Ndata += Ndat;
					catnm->Ncdata += Ndat;
				}
				catnm++;
			}
		}
		else if (strcmp(s, "-wdloc") == 0)
		{
			fscanf(fi, "%[^;];", s);
			
			strcat(s, " ;");
			Estl->wdloc = stralloc(s);
		}
		else if (strcmp(s, "-Ndata") == 0)
			fscanf(fi, " %d",  &Estl->Ndata);
		
		
		else if (s[strlen(s) - 1] == '#')
			Estl->flid = stralloc(s);
		
		else
			Eprint("<esondat>", s);	 
	}

	if ( Estl->title != NULL )
		printf("esondat  title=%s\n", Estl->title);

	if ( Estl->title != NULL )
		printf("esondat  w=%s\n", Estl->wdatfile);

	printf("esondat  tid=%c\n", Estl->tid);
	printf("esondat  Ntime=%d\n", Estl->Ntime);
	printf("esondat  tmdt=%s\n", Estl->timeid);
	printf("esondat  flid=%s\n", Estl->flid);
}

/* ----------------------------------------------------------- */

#if EPOS_PROG
/*  処理要素の選択指定 */

void rqlist(int Narg, char **arg, FILE *fii, ESTL *Estl)
{
	RQLIST *Rq;
	int		i, N ;
	char s[NCHAR], *st;
	
	N = NREQMAX ;
	if (N > 0 && (Estl->Rq = (RQLIST *)malloc(N * sizeof(RQLIST))) == NULL)
		Ercalloc(NREQMAX, "esondat -- Rq");

	if ( Estl->Rq != NULL )
	{
		Rq = Estl->Rq ;

		for ( i = 0; i < N; i++, Rq++ )
			Rq->rname = Rq->name = Rq->id = NULL ;
	}
	
	Rq = Estl->Rq;
	Estl->Nrqlist = 0;
	Estl->Nvreq = 0;
	
	if (Narg > 1)
	{
		Rq = Estl->Rq;
		for (i = 1; i < Narg; i++) 
		{
			//printf("i=%d arg=%s\n",i, arg[i]);
			if (strcmp(arg[i], "-i") == 0)
			{	    
				i++ ;
				Rq->rname = stralloc(arg[i]);
				Rq++;
			}
			else if (strcmp(arg[i], "-v") == 0)
			{
				i++ ;
				strcpy(Estl->vreq, arg[i]);
				printf("arg vreq = %s\n", Estl->vreq);
			}
		}
		Estl->Nrqlist = Rq - Estl->Rq;
		Estl->Nvreq = strlen(Estl->vreq);
	} 
	
	if (fii != NULL && (Estl->Nrqlist == 0 && Estl->Nvreq == 0))
	{ 
		while(fscanf(fii, "%s", s), *s != '*')
		{
			if (strcmp(s, "-i") == 0)
			{     
				Rq = Estl->Rq;
				while (fscanf(fii, "%s", s), *s != ';')
				{
					Rq->rname = stralloc(s);
					Rq++;
				}
				Estl->Nrqlist = Rq - Estl->Rq;
			}
			else if (strcmp(s, "-v") == 0)
			{
				fscanf(fii, "%s", s);
				strcpy(Estl->vreq, s);
				Estl->Nvreq = strlen(Estl->vreq);
				printf("file vreq = %s\n", Estl->vreq);
				
			}
		}
	}
	printf("Nrqlist=%d  Nvreq=%d\n", Estl->Nrqlist, Estl->Nvreq);
	
	Rq = Estl->Rq;
	for (i = 0; i < Estl->Nrqlist; i++, Rq++)
	{
		if ((st = strchr(Rq->rname, '_')) != NULL)
		{
			if (*Rq->rname == *st)
			{
				Rq->name = stralloc("*");
				Rq->id = st + 1;
			}
			else
			{
				*st = NULL;
				Rq->name = Rq->rname;
				Rq->id = stralloc(st + 1);
			}
		}
		else
		{
			Rq->name = Rq->rname;
			Rq->id = stralloc("*");
		}
		printf("Rq name=%s id=%s\n", Rq->name, Rq->id);
	}
}

#endif	       
/* ----------------------------------------------------------- */

/* 要素名、シミュレーション結果入力用記憶域確保 */

void esoint(FILE *fi, char *err, int Ntime, 
			ESTL *Estl, TLIST *Tlist) 
{
	int		i, j, n = 0 ;
	char	nm[SCHAR], id[SCHAR], *st, E[SCHAR] ;
	CATNM	*cat ;
	RQLIST	*Rq, *R ;
	char	*V ;
	
	cat = NULL ;
	Rq = R = NULL ;
	st = V = NULL ;

	if (Estl->catnm == NULL)
		cat = Estl->catnm;
	Rq = Estl->Rq;
	
	for (i = 0; i < Estl->Ndata; i++, Tlist++)
	{
		fscanf(fi, " %[^_]_%s %c %c", nm, id, &Tlist->vtype, &Tlist->ptype);
		
		switch (Tlist->vtype)
		{
		case 'H': case 'Q': case 'E': case 'q': case 'e': case 'm' :
			Tlist->stype = 't';
			break;
		case 'T': case 'X': case 'R': case 't': case 'x': case 'r':
			Tlist->stype = 'a';
			break;
		case 'c':
			Tlist->stype = 'c';
			break;
			
		default:
			switch(id[strlen(id)-1])
			{
			case 'n': case 'c':
				Tlist->stype = 'n';
				break;
			case 'm': case 'h': case 'e': case 'p':
				Tlist->stype = 'm';
				break;
			default:
				sprintf ( E, "xxxx %s xxx  %s %s %c %c %c\n",
					err, nm, id, id[strlen(id)-1], Tlist->vtype, Tlist->ptype);
				Eprint ( "<esoint>", E ) ;
			}
		}
		
		/****printf("<<esoint>>  cat->Ncdata=%d\n", cat->Ncdata);
		***************/
		
		if (Estl->catnm != NULL)
		{
			if (n++ >= cat->Ncdata)
			{
				cat++;
				n = 0;
			}     
			Tlist->cname = cat->name;
		}
		else
			Tlist->cname = stralloc("*");
		
		Tlist->name = stralloc(nm);
		Tlist->id = stralloc(id);      
		Tlist->req = 'n';
		
		if (Estl->Nrqlist == 0 && Estl->Nvreq == 0)
			Tlist->req = 'y';
		else
		{
			R = Estl->Rq ;
			for (j = 0; j < Estl->Nrqlist; j++, R++)
			{
				if ((strcmp(Tlist->name, R->name) == 0 || *R->name == '*') &&
					(strcmp(Tlist->id, R->id) == 0 || *R->id == '*'))
				{
					Tlist->req = 'y';
					break;
				}
				else if ((st = strchr(Tlist->name, ':')) != NULL)
				{
					if (strncmp(Tlist->name, R->name, st - Tlist->name) == 0
						&&  *R->id == '*')
					{
						Tlist->req = 'y';
						break;
					}
				}		  
			}

			V = Estl->vreq ;
			for (j = 0; j < Estl->Nvreq; j++, V++)
			{
				if (*V == Tlist->vtype)
					Tlist->req = 'y';
			}
		}
		/***** printf("<<esoint>> %s %s %s %c %c rq=%c\n", Tlist->cname, Tlist->name, Tlist->id, 
		Tlist->vtype, Tlist->ptype, Tlist->req);********/
		
		switch (Tlist->ptype)
		{
		case 'f':
			Tlist->fval = dcalloc(Ntime, err);
			break;
			
		case 'd':
			Tlist->ival = icalloc(Ntime, err);
			break;
		case 'c':
			Tlist->cval = scalloc(Ntime, err);
			break;
		}
		fofmt(Estl, Tlist);
	}
}
/* ----------------------------------------------------------- */

/* 出力時の書式指定 */

void fofmt(ESTL *Estl, TLIST *Tlist)
{
	int j;
	static char *f81="%8.1lf", *f80="%8.0lf", *f84="%8.4lf", 
		*f8d="%8d", *f4d="%04d", *fc="%c";
	
	switch (Tlist->vtype)
	{
	case 't': case 'T':
		Tlist->fmt = f81;
		break;
	case 'r': case 'R':
		Tlist->fmt = f80;
		break;	    
	case 'x': case 'X':
		Tlist->fmt = f84;
		break;
	case 'q': case 'e': 
		Tlist->fmt = f81;
		break;   
	case 'Q': case 'E':
		Tlist->fmt = f80;
		break;	    
	case 'H': 
		Tlist->fmt = f8d;
		break;	    
	case 'h': 
		Tlist->fmt = f4d;
		break;	    
	case 'c': 
		Tlist->fmt = fc;
		break;
	}
	for (j = 0; j < Estl->Nunit; j++)
	{
		if (*Estl->unit[j] == Tlist->vtype)
			Tlist->unit = Estl->unit[2];
	}
}
/* ----------------------------------------------------------- */

/*  年、月、日、曜日、時刻の入力 */

int tmdata(VCFILE *Vcfile, TMDT *Tmdt, DAYTM *Daytm, char perio)
{
	int		i, r, D ;
	char	s[SCHAR], *st;
	FILE	*fi ;
	ESTL	*Estl ;

	fi = Vcfile->fi ;
	Estl = &Vcfile->Estl ;
	r = 1 ;
	
	while ( fscanf ( fi, "%s", s ) != EOF )
	{
		//printf ( "s=%s  ", s ) ;

		if ( strcmp ( s, "-999" ) == 0 || strcmp ( s, "end" ) == 0 )
		{
			// 周期定常計算の場合は何回でも読み込む
			if ( Vcfile->ic != 2 || perio == 'y')
			{
				fseek ( fi, Vcfile->ad, SEEK_SET ) ;
				(Vcfile->ic)++ ;
			}
			else
				return 0 ;
		}
		else
		{
			D = 0 ;
			for ( i = 0; i < Estl->ntimeid; i++ )
			{
				if ( i > 0 )
					fscanf ( fi, "%s", s ) ;
				
				/*****printf("<<tmdata>> i=%d s=%s\n", i, s);/*****/
				
				switch (Estl->timeid[i])
				{
				case 'Y':
					strcpy(Tmdt->year, s);
					Tmdt->Year = atoi(s);
					Tmdt->dat[i] = Tmdt->year;

					if ( Tmdt->Year == Daytm->Year )
						D++ ;

					break;
				case 'M':
					strcpy(Tmdt->mon, s);
					Tmdt->Mon = atoi(s);
					Tmdt->dat[i] = Tmdt->mon;

					if ( Tmdt->Mon == Daytm->Mon )
						D++ ;

					break;
				case 'D':
					strcpy(Tmdt->day, s);
					Tmdt->Day = atoi(s);
					Tmdt->dat[i] = Tmdt->day;

					if ( Tmdt->Day == Daytm->Day )
						D++ ;

					break;
				case 'W':
					strcpy(Tmdt->wkday, s);
					Tmdt->dat[i] = Tmdt->wkday;
					break;
				case 'T':
					strcpy(Tmdt->time, s);
					if ((st = strchr(s, ':')) != NULL)
						*st = '.';
					Tmdt->Time = (int)(atof(s) * 100.0 + 0.5);
					Tmdt->dat[i] = Tmdt->time;

					if ( Tmdt->Time - ( int ) ( 100. * ( Daytm->time ) + 0.5 ) == 0 )
						D++ ;
					break;
				}
			}

			if ( D == i )
				return 1 ;
			else
			{
				for ( i = 0; i < Estl->Ndata; i++ )
					fscanf ( fi, "%*s" ) ;
			}
		}
	}

	return r ;
}	       
/* ----------------------------------------------------------- */

/* シミュレーション結果データ入力 */

void esdatgt(FILE *fi, int i, int Ndata, TLIST *Tlist)
{
	char  s[SCHAR];
	int j;
	TLIST *Tml;
	
	for (j = 0; j < Ndata; j++, Tlist++)
	{
		fscanf(fi, "%s", s);
		if (Tlist->req == 'y' || Tlist->vtype == 'h' || Tlist->vtype == 'H')
		{
			switch (Tlist->ptype)
			{
			case 'f':
				Tlist->fval[i] = atof(s);
				/***printf("<<esdatgt>> j=%d (data=%s)  %s %s [%d]=%lf\n",
				j, s, Tlist->name, Tlist->id, i, Tlist->fval[i]);
				*****/
				break;
			case 'd':
				Tlist->ival[i] = atoi(s);
				break;
			case 'c':
				Tlist->cval[i] = *s;
				break;
			}
			Tml = Tlist - 1;
			if (Tml != NULL)
			{	    
				if (Tml->vtype == 'h' || Tml->vtype == 'H')
					Tlist->pair = Tml;
			}
		}
	}
}





