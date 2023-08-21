/* epsinput.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eps.h"
#include "fnlib.h"
#include "estr.h"

/* シミュレーション結果、標題、識別データの入力 */

void esondat(FILE *fi, ESTL *Estl)
{
	char  s[NCHAR];
	int   i, j, Nparm, Ndat ;
	CATNM *catnm;
	
	
	while (fscanf(fi, "%s", s)!=EOF && *s != '#')
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
				Estl->unit[i++] = stralloc(s);
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
			Estl->ntimeid = strlen(Estl->timeid);
		}
		else if (strcmp(s, "-cat") == 0) 
		{
			Estl->Ncatalloc = catcount ( fi ) ;

			if ((Estl->catnm = calloc(Estl->Ncatalloc, sizeof(CATNM))) == NULL)
				Ercalloc(Estl->Ncatalloc, "esondat -- catnm");
			else
			{
				catnm = Estl->catnm ;
				for ( i = 0; i < Estl->Ncatalloc; i++, catnm++ )
				{
					catnm->N = catnm->Ncdata = 0 ;
					catnm->name = NULL ;
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
		else if (strcmp(s, "-Ndata") == 0)
			fscanf(fi, " %d",  &Estl->Ndata);
		
		
		else if (s[strlen(s) - 1] == '#')
			Estl->flid = stralloc(s);
		
		else if (strcmp(s, "-ver") == 0)
			fscanf(fi, " %*s " ) ;

		else if (strcmp(s, "-dtf") == 0)
			fscanf(fi, " %*s " ) ;
			/*********************
			else
			printf("xxxx esondat xxx s=%s\n", s);
		****************************/
	}
}

/* ----------------------------------------------------------- */

/*  処理要素の選択指定 */

void rqlist(int Narg, char *arg[], FILE *fii, ESTL *Estl)
{
	RQLIST *Rq ;
	int i ;
	char s[NCHAR], *st;
	
	if ( fii != NULL )
		Estl->NRq = Rqcount ( fii ) ;
	if (Estl->NRq>0 && (Estl->Rq = calloc(Estl->NRq, sizeof(RQLIST))) == NULL)
		Ercalloc(Estl->NRq, "esondat -- Rq");
	else
	{
		Rq = Estl->Rq;
		for ( i = 0; i < Estl->NRq; i++, Rq++ )
		{
			Rq->rname = NULL ;
			Rq->name = NULL ;
			Rq->id = NULL ;
		}
	}
	
	Rq = Estl->Rq;
	Estl->Nrqlist = 0;
	Estl->Nvreq = 0;
	
	/**********
	if (Narg > 1)
	{
		Rq = Estl->Rq;
		for (i = 1; i < Narg; i++) 
		{
			if (strcmp(arg[i], "-i") == 0)
			{	    
				Rq->rname = stralloc(arg[++i]);
				Rq++;
			}
			else if (strcmp(arg[i], "-v") == 0)
				strcpy(Estl->vreq, arg[++i]);
		}
		Estl->Nrqlist = Rq - Estl->Rq;
		Estl->Nvreq = strlen(Estl->vreq);
	} 
	*********************/
	
	if (fii != NULL && (Estl->Nrqlist == 0 && Estl->Nvreq == 0))
	{ 
		while(fscanf(fii, "%s", s), *s != '*')
		{
			printf ( "%s\n", s ) ;
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
				Estl->vreq = stralloc ( s);
				Estl->Nvreq = strlen(Estl->vreq);
				
			}
		}
	}
	
	Rq = Estl->Rq;
	for (i = 0; i < Estl->Nrqlist; i++, Rq++)
	{
		if ( i > Estl->NRq )
			printf ( "MAX=%d i=%d\n", Estl->NRq, i ) ;

		if ((st = strchr(Rq->rname, '_')) != NULL)
		{
			if (*Rq->rname == *st)
			{
				Rq->name = stralloc("*");
				Rq->id = st + 1;
			}
			else
			{
				*st = '\0';
				Rq->name = Rq->rname;
				Rq->id = stralloc(st + 1);
			}
		}
		else
		{
			Rq->name = Rq->rname;
			Rq->id = stralloc("*");
		}
	}
}


/* ----------------------------------------------------------- */

/* 要素名、シミュレーション結果入力用記憶域確保 */

void esoint(FILE *fi, char *err, int Ntime, 
			ESTL *Estl, TLIST *Tlist) 
{
	int  i, j, n = 0;
	char nm[NCHAR], id[NCHAR], *st;
	CATNM *cat;
	RQLIST *Rq;
	
	cat = Estl->catnm;
	Rq = Estl->Rq;
	
	for (i = 0; i < Estl->Ndata; i++, Tlist++)
	{
		fscanf(fi, " %[^_]_%s %c %c", nm, id, &Tlist->vtype, &Tlist->ptype);

		//printf("%d %s_%s %c %c\n", i, nm, id, Tlist->vtype, Tlist->ptype);
		
		switch (Tlist->vtype)
		{
		case 'H': case 'Q': case 'E':
			Tlist->stype = 't';
			break;
		case 'T': case 'X': case 'R':
			Tlist->stype = 'a';
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
			/****************************
			printf("xxxx %s xxx  %s %s %c %c\n",
			err, nm, id, Tlist->vtype, Tlist->ptype);
				****************************/
				;
			}
		}
		
		if (n++ >= cat->Ncdata)
		{
			cat++;
			n = 0;
		}
		Tlist->cname = cat->name;
		Tlist->name = stralloc(nm);
		Tlist->id = stralloc(id);      
		Tlist->req = 'n';
		
		if (Estl->Nrqlist == 0 && Estl->Nvreq == 0)
			Tlist->req = 'y';
		else
		{	 
			for (j = 0; j < Estl->Nrqlist; j++)
			{
				if ((strcmp(Tlist->name, Rq[j].name) == 0 || *Rq[j].name == '*') &&
					(strcmp(Tlist->id, Rq[j].id) == 0 || *Rq[j].id == '*'))
				{
					Tlist->req = 'y';
					break;
				}
				else if ((st = strchr(Tlist->name, ':')) != NULL)
				{
					if (strncmp(Tlist->name, Rq[j].name, st - Tlist->name) == 0
						&&  *Rq[j].id == '*')
					{
						Tlist->req = 'y';
						break;
					}
				}		  
			}
			for (j = 0; j < Estl->Nvreq; j++)
			{
				if (Estl->vreq[j] == Tlist->vtype)
					Tlist->req = 'y';
			}
		}
		
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
	static char *f81="%8.1f", *f80="%8.0f", *f84="%8.4f", 
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

int tmdata(FILE *fi, ESTL *Estl, TMDT *Tmdt)
{
	int i, returnlog;
	char s[NCHAR];
	
	returnlog = fscanf(fi, "%s", s);
	if (returnlog == EOF || strcmp(s, "-999") == 0)
		return 0;
	else
	{
		for (i = 0; i < Estl->ntimeid; i++)
		{
			if (i > 0)
				fscanf(fi, "%s", s);
			
			switch (Estl->timeid[i])
			{
			case 'Y':
				strcpy(Tmdt->year, s);
				Tmdt->Year = atoi(s);
				Tmdt->dat[i] = Tmdt->year;
				break;
			case 'M':
				strcpy(Tmdt->mon, s);
				Tmdt->Mon = atoi(s);
				Tmdt->dat[i] = Tmdt->mon;
				break;
			case 'D':
				strcpy(Tmdt->day, s);
				Tmdt->Day = atoi(s);
				Tmdt->dat[i] = Tmdt->day;
				break;
			case 'W':
				strcpy(Tmdt->wkday, s);
				Tmdt->dat[i] = Tmdt->wkday;
				break;
			case 'T':
				strcpy(Tmdt->time, s);
				Tmdt->Time = (int)atof(s) * 100;
				Tmdt->dat[i] = Tmdt->time;
				break;
			}
		}
		return 1;
	}
}	       
/* ----------------------------------------------------------- */

/* シミュレーション結果データ入力 */

void esdatgt(FILE *fi, int i, int Ndata, TLIST *Tlist)
{
	char  s[NCHAR];
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


int	Rqcount ( FILE *fi )
{
	int	N ;
	long	ad ;
	char	s[NCHAR] ;
	
	N = 0 ;
	ad = ftell ( fi ) ;

	while ( fscanf ( fi, "%s", s ) != EOF )
	{
		if ( strcmp ( s, "-i" ) == 0 )
		{
			while ( fscanf ( fi, "%s", s ) != EOF )
			{
				if ( *s == ';' )
					break ;
				else
					N++ ;
			}
		}
	}

	fseek ( fi, ad, SEEK_SET ) ;
	return N ;
}

int	catcount ( FILE *fi )
{
	int	N, Ndata, i, j, Nparm ;
	long	ad ;
	char	s[NCHAR] ;

	N = 0 ;
	ad = ftell ( fi ) ;

	while ( fscanf ( fi, "%s", s ) != EOF )
	{
		if ( *s == '*' )
			break ;

		fscanf ( fi, "%d", &Ndata ) ;
		for ( i = 0; i < Ndata; i++ )
		{
			fscanf ( fi, "%*s %d %*s", &Nparm ) ;
			for ( j = 0; j < Nparm - 1; j++ )
				fscanf ( fi, "%*s" ) ;
		}
		N++ ;
	}

	fseek ( fi, ad, SEEK_SET ) ;
	return N ;
}
