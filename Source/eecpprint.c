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

/*  ecpprint.c  */

#include <stdio.h>
#include "common.h"
#include "fesy.h"
#include "fnfio.h"


void cmpprint(int id, int N,  COMPNT *cmp, ELOUT *Elout, ELIN *Elin)
{
	const char  *name = "name", *eqptype = "eqptype",
		*envname = "envname";
	int i;
	
	if (id == 1)
		printf("COMPNT\n n %-10s %-10s %-10s -c- nca neq Nout Nin nivr Elou Elin\n",
	       name,  eqptype, envname);
	
	for (i = 0; i < N; i++, cmp++)
	{
		printf("%2d %-10s %-10s %-10s   %c %4d %3d %4d %3d %d %4ld %4ld\n",
			i, cmp->name,  cmp->eqptype, cmp->envname,
			cmp->control, cmp->ncat, cmp->neqp, cmp->Nout, cmp->Nin, cmp->nivar,
			cmp->elouts - Elout,  cmp->elins - Elin);
	}   
}

void eloutprint(int id, int N, ELOUT *E, COMPNT *cmp)
{
	int    i, j;
	double  *cfin;
	
	if (id == 1)
		printf("ELOUT\n  n name            id fld contl sysld Cmp   G      cfo    cfin\n");
	
	for (i = 0; i < N; i++, E++)
	{
		printf("%3d (%-10s)     %c   %c   %c    %c  %4ld [%5.3lf]  %6.3lf", 
			i, E->cmp->name, E->id, E->fluid, E->control, E->sysld,
			E->cmp - cmp, E->G, E->coeffo);
		
		cfin = E->coeffin;
		for (j = 0; j < E->Ni; j++, cfin++)
		{
			printf(" %6.3lf", *cfin);
			//cfin++ ;
		}
		printf(" Co=%6.4lf\n", E->Co);
	}
}

void eloutfprint(int id, int N, ELOUT *E, COMPNT *cmp)
{
	int    i, j;
	double  *cfin;
	
	if (id == 1)
		fprintf(ferr,"ELOUT\n  n         id fld contl sysld Cmp   G      cfo    cfin\n");
	
	for (i = 0; i < N; i++, E++)
	{
		fprintf(ferr,"%3d (%-6s) %c   %c   %c    %c  %4ld [%5.3lf]  %6.3lf", 
			i, E->cmp->name, E->id, E->fluid, E->control, E->sysld,
			E->cmp - cmp, E->G, E->coeffo);
		
		cfin = E->coeffin;
		for (j = 0; j < E->Ni; j++, cfin++)
		{
			fprintf(ferr," %6.3lf", *cfin);
			//cfin++ ;
		}
		fprintf(ferr," Co=%6.4lf\n", E->Co);
	}
}

void elinprint(int id, int N, COMPNT *C, ELOUT *eo, ELIN *ei)
{
	ELIN *E;
	ELOUT *Eo;
	int i, ii, j, o, v;
	
	if (id == 1)
		printf("ELIN\n  n  id   upo  upv\n");
	
	for (i = 0; i < N; i++, C++)
	{
		Eo = C->elouts;
		/***    E = C->elins; ****/
		
		for (ii = 0; ii < C->Nout; ii++, Eo++)
		{
			E = Eo->elins;	
			for (j = 0; j < Eo->Ni; j++, E++)
			{
				if ( E->upo != NULL && eo != NULL )
					o = (int)(E->upo - eo) ;
				else
					o = -999 ;

				if ( E->upv != NULL && eo != NULL )
					v = (int)(E->upv - eo) ;
				else
					v = -999 ;

				printf("%3ld (%-6s) %c   %3d   %3d", 
					E - ei, C->name, E->id, o, v ); 
				if (E->upo != NULL)
					printf(" upo=(%-6s)", E->upo->cmp->name);
				if (E->upv != NULL)
					printf(" upv=(%-6s)", E->upv->cmp->name);
				printf("\n");
			}
		}
	}    
}

void elinfprint(int id, int N, COMPNT *C, ELOUT *eo, ELIN *ei)
{
	ELIN *E;
	ELOUT *Eo;
	int i, ii, j, o, v;
	
	if (id == 1)
		fprintf(ferr,"ELIN\n  n  id   upo  upv\n");
	
	for (i = 0; i < N; i++, C++)
	{
		Eo = C->elouts;
		/***    E = C->elins; ****/
		
		for (ii = 0; ii < C->Nout; ii++, Eo++)
		{
			E = Eo->elins;	
			for (j = 0; j < Eo->Ni; j++, E++)
			{
				if ( E->upo != NULL && eo != NULL )
					o = (int)(E->upo - eo) ;
				else
					o = -999 ;

				if ( E->upv != NULL && eo != NULL )
					v = (int)(E->upv - eo) ;
				else
					v = -999 ;

				fprintf(ferr,"%3ld (%-6s) %c   %3d   %3d", 
					E - ei, C->name, E->id, o, v ); 
				if (E->upo != NULL)
					fprintf(ferr," upo=(%-6s)", E->upo->cmp->name);
				if (E->upv != NULL)
					fprintf(ferr," upv=(%-6s)", E->upv->cmp->name);
				fprintf(ferr,"\n");
			}
		}
	}    
}

/****************************
void catlprint(int id, int N, CATLIST *c)
{
int i;
const char *catname = "catname", *eqptype = "eqptype";

 if (id == 1)
 printf("CATLIST\n  n %-10s %-10s neqp Nin Nout nivr\n", catname, eqptype);
 
  for (i = 0; i < N; i++, c++)
  {
  printf("%3d   %-10s %-10s %4d %3d %4d %4d\n", 
  i, c->catname, c->eqptype, c->neqp, c->Nin, c->Nout, c->nivar);   
  }    
  }
********************************/
/**********************
void pelmprint(int id, int N, PELM *p, ELOUT *Eo, ELIN *Ei)
{
int i;

 if (id == 1)
 printf("PELM\n  n  co ci elin eout\n");
 
  for (i = 0; i < N; i++, p++)
  {
  printf("%3d   %c  %c %4d %4d\n", 
  i, p->co, p->ci, p->in - Ei, p->out - Eo);   
  }    
  }
  
********************************/ 


void plistprint(int Nmpath, MPATH *Mpath, 
				PELM *Pe, ELOUT *Eo, ELIN *Ei)
{
	int   i, j, ii;
	PLIST  *pl;
	PELM   *p;
    
	printf("xxx plistprint\n");
	for (i = 0; i < Nmpath; i++, Mpath++)
	{
		
		printf("\nMpath=[%d] %s sys=%c type=%c fluid=%c Nlpath= %d  Ncv=%d lvcmx=%d\n", 
			i, Mpath->name, Mpath->sys, Mpath->type, Mpath->fluid, Mpath->Nlpath,
			Mpath->Ncv, Mpath->lvcmx);
		
		pl = Mpath->plist;
		for (j = 0; j < Mpath->Nlpath; j++, pl++)
		{
			printf("PLIST\n  n type Nelm Npump Nvav lvc Pelm  G \n");	 
			printf("%3d  %c  %3d  %3d %3d %3ld %6.3lf\n",
				j, pl->type, pl->Nelm, pl->Nvav, pl->lvc,  pl->pelm - Pe, pl->G);
			
			printf("    PELM  n  co ci elin eout\n");
			p = pl->pelm;
			for (ii = 0; ii < pl->Nelm; ii++, p++)
				printf("        %3ld   %c  %c %4ld %4ld  %s\n", 
				p - pl->pelm, p->co, p->ci, p->in - Ei, p->out - Eo, p->cmp->name ); 
		}
	}    
}




