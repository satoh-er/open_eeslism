/* eespos.c */

/* ees31データ集計 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <Windows.h>
#include "eps.h"
#include "fnlib.h"
#include "estr.h"

FILE *ferr;

int main(int Narg, char *arg[])
{
	FILE  *fi, *fii, *fo;
	char  *finame, fnc, **parm;
	// *parm[MAXPARM];
	ESTL  Estl;
	TMDT  Tmdt[366], *Tmd;
	//	TLIST *Tlist, *Plist[NPREQMAX], *T ;
	TLIST *Tlist, **Plist, *T;
	//	STATC Statc[STOTALMAX], *Pprd;
	STATC *Statc, *Pprd;
	int   i, Nstatc;
	CATNM	*C;
	RQLIST	*Rq;

	/*******************************
	printf("exfile: e,  mon/season statistics: s,  table: t\n");
	scanf("%c", &fnc);
	*******************************/
	//char cdir[255];
	//GetCurrentDirectory(255,cdir);
	//printf("Current Directory : %s\n", cdir);

	fnc = 'e';
	finame = NULL;
	parm = NULL;
	Tmd = NULL;
	Tlist = NULL;
	T = NULL;
	Pprd = NULL;
	fi = fii = fo = NULL;
	Statc = NULL;
	Plist = NULL;
	Estlinit(&Estl);

	fi = esfidat(Narg, arg, &finame);
	printf("esfidat end\n");

	fii = esfii(Narg, arg);
	printf("esfii end\n");

	if ((parm = calloc(sizeof (char **), Narg)) == NULL)
		Eprint("<main>", "parm allocate");

	for (i = 0; i < Narg; i++)
		parm[i] = arg[i];

	printf("parm copy end\n");

	/*****************
	if (fii == NULL)
	{
	printf("opt ? .... #\n");
	while (scanf("%s", s), *s != '#')
	{
	if ((c = strrchr(s,'#')) != NULL)
	{
	*c = '\n';
	break;
	}
	parm[i++] = stralloc(s);
	Narg = i;
	}
	}
	********************/

	fo = esfout(Narg, parm, IDFO, finame);
	printf("esfout end\n");

	esondat(fi, &Estl);
	printf("esondat end\n");

	printf("%d\n", Estl.Ndata);
	if ((Tlist = calloc(Estl.Ndata, sizeof(TLIST))) == NULL)
		Ercalloc(Estl.Ndata, "main -- Tlist");
	else
	{
		T = Tlist;
		for (i = 0; i < Estl.Ndata; i++, T++)
		{
			T->cname = T->name = T->id = T->unit = T->cval = T->cstat = T->fmt = NULL;
			T->fval = T->fstat = NULL;
			T->ival = T->istat = NULL;
			T->pair = NULL;
		}
	}

	if (fnc == 'e')
	{
		rqlist(Narg, parm, fii, &Estl);
		printf("rqlist end\n");

		esoint(fi, "esoint", 0, &Estl, Tlist);
		printf("esoint end\n");

		tmdout(0, fo, &Estl, Tmdt);
		printf("tmdout end\n");

		esoout(0, fi, fo, Estl.Ndata, Tlist);
		printf("esoout end\n");

		while (tmdata(fi, &Estl, Tmdt))
		{
			tmdout(1, fo, &Estl, Tmdt);
			esoout(1, fi, fo, Estl.Ndata, Tlist);
		}
	}
	else if (fnc == 's')
	{
		rqlist(Narg, parm, fii, &Estl);
		esoint(fi, "esoint", Estl.Ntime, &Estl, Tlist);

		fprintf(fo, "Mon nday ");
		esoout(0, fi, fo, Estl.Ndata, Tlist);
		Tmd = Tmdt;
		for (i = 0; i < Estl.Ntime; i++, Tmd++)
		{
			tmdata(fi, &Estl, Tmd);
			esdatgt(fi, i, Estl.Ndata, Tlist);
		}
		statmrk(Tmdt, &Statc, &Nstatc, Estl.Ntime, atoi(Tmdt[0].mon), atoi(Tmdt[0].day),
			atoi(Tmdt[Estl.Ntime - 1].mon), atoi(Tmdt[Estl.Ntime - 1].day),
			Tlist, Estl.Ndata, "statmrk");

		statcalc(Nstatc, Statc, Estl.Ntime, Tmdt, Estl.Ndata, Tlist);
		statout(fo, Nstatc, Statc, Estl.Ndata, Tlist);
	}
	else if (fnc == 't')
	{
		if ((Pprd = calloc(NPREQMAX, sizeof(STATC))) == NULL)
			Ercalloc(NPREQMAX, "main --- Pprd");

		esoint(fi, "esoint", Estl.Ntime, &Estl, Tlist);
		Tmd = Tmdt;
		for (i = 0; i < Estl.Ntime; i++, Tmd++)
		{
			tmdata(fi, &Estl, Tmd);
			esdatgt(fi, i, Estl.Ndata, Tlist);
		}
		prtreqdc(Narg, parm, fii, &Estl, Pprd);

		if ((Plist = (TLIST **)calloc(Estl.Npreq, sizeof (TLIST **))) == NULL)
			Ercalloc(Estl.Npreq, "<main>  Plist allocate");
		else
		{
			for (i = 0; i < Estl.Npreq; i++)
				Plist[i] = NULL;
		}

		prtreqlist(&Estl, Estl.Ndata, Tlist, Plist);

		prttbl(fo, &Estl, Pprd, Tmdt, Plist);
	}

	fclose(fi);
	fclose(fo);

	printf("free start\n");
	free(Estl.title);
	free(Estl.wdatfile);

	for (i = 0; i < VTYPEMAX; i++)
		free(Estl.unit[i]);
	//free ( Estl.unit ) ;

	C = Estl.catnm;
	for (i = 0; i < Estl.Ncatalloc; i++, C++)
		free(C->name);
	free(Estl.catnm);

	Rq = Estl.Rq;
	for (i = 0; i < Estl.NRq; i++, Rq++)
	{
		free(Rq->name);

		if (Rq->name != Rq->rname)
			free(Rq->rname);
		//free(Rq->id);
	}
	free(Estl.Rq);

	free(Estl.timeid);
	free(Estl.flid);
	free(Estl.vreq);

	T = Tlist;
	for (i = 0; i < Estl.Ndata; i++, T++)
	{
		//printf ( "i=%d\n", i ) ;
		free(T->name);
		free(T->id);
		free(T->fval);
		free(T->ival);
		free(T->cval);
		//free ( T->cname ) ;
		//free ( T->unit ) ;
		free(T->fstat);
		free(T->istat);
		free(T->cstat);
		//free ( T->fmt ) ;
	}
	free(Tlist);

	free(finame);

	free(parm);
	free(Pprd);
	free(Plist);
	return (0);
}

/* -------------------------------------------------------- */
