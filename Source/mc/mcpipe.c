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

/*  pipe.c  */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"


/*  配管・ダクト 仕様入力 */

int Pipedata(char *cattype, char *s, PIPECA *Pipeca)
{
	char  *st;
	double dt;
	int   id=0;
	
	if (strcmp(cattype, DUCT_TYPE) == 0)
		Pipeca->type = DUCT_PDT;
	else
		Pipeca->type = PIPE_PDT;
	
	if ((st=strchr(s,'=')) == 0)
	{
		Pipeca->name = stralloc(s);
		Pipeca->Ko = -999.0 ;
	}
	else
	{ 
		*st = '\0';
		
        dt=atof(st+1);
		
        if (strcmp(s,"Ko") == 0)
			Pipeca->Ko = dt;
		
        else 
			id=1;      
	}
	return (id);
}
/* --------------------------- */

/*  管長・ダクト長、周囲温度設定 */

void Pipeint(int Npipe, PIPE *Pipe, 
			 SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd)
{
	int		i;
	char	Err[SCHAR] ;
	
	for (i = 0; i < Npipe; i++, Pipe++)
	{
		/*-------  Satoh DEBUG  1998/5/15 -------------*/
		
		if ( Pipe->cmp->ivparm != NULL )
			Pipe->L = *Pipe->cmp->ivparm;
		else
			Pipe->L = -999.0 ;
		
			/******************
			printf ( "envnasme=%s\n", Pipe->cmp->envname ) ;
		************************/
		
		if ( Pipe->cmp->envname != NULL )
			Pipe->Tenv = envptr(Pipe->cmp->envname, Simc, Ncompnt, Compnt, Wd, NULL);
		else
		{
			Pipe->room = roomptr(Pipe->cmp->roomname, Ncompnt, Compnt ) ;
			/**********************
			printf ( "Roomname=%s\n", Pipe->room->name) ;
			**************************/
		}

		if ( Pipe->cat->Ko < 0.0 )
		{
			sprintf ( Err, "Name=%s  Ko=%.4g", Pipe->cmp->name, Pipe->cat->Ko ) ;
			Eprint ( "Pipeint", Err ) ;
		}
		if ( Pipe->L < 0.0 )
		{
			sprintf ( Err, "Name=%s  L=%.4g", Pipe->cmp->name, Pipe->L ) ;
			Eprint ( "Pipeint", Err ) ;
		}
	}
}
/* --------------------------- */

/*  特性式の係数  */

void Pipecfv(int Npipe, PIPE *Pipe)
{
	ELOUT *Eo;
	double  cG, Te;
	int i;
	char	s[SCHAR] ;
	
	for (i = 0; i < Npipe; i++, Pipe++)
	{
		Te = 0.0 ;
		if (Pipe->cmp->control != OFF_SW)	
		{
			if ( Pipe->cmp->envname != NULL )
				Te = *(Pipe->Tenv) ;
			else if ( Pipe->room != NULL )
				Te = Pipe->room->Tot ;
			else
			{
				sprintf ( s, "Undefined Pipe Environment  name=%s", Pipe->name ) ;
				Eprint ( "<Pipecfv>", s ) ;
			}
            
				/*******
				printf("<<Pipecfv>> i=%d %s  Tenv=%lf\n", i, Pipe->name, Te);
			/************/
			Pipe->Ko = Pipe->cat->Ko;
			
			Eo = Pipe->cmp->elouts;
			cG = spcheat(Eo->fluid) * Eo->G;	 
			Pipe->ep = 1.0 - exp((double)(-(Pipe->Ko * Pipe->L)/ cG));
			Pipe->D1 = cG * Pipe->ep;
			Pipe->Do = Pipe->D1 * Te;	 
			Eo->coeffo = cG;
			Eo->Co = Pipe->Do;
			*(Eo->coeffin) = Pipe->D1 - cG;
			
			if (Pipe->cat->type == DUCT_PDT)
			{
				Eo++;
				Eo->coeffo = 1.0 ;
				Eo->Co = 0.0;
				*(Eo->coeffin) = - 1.0 ;
			}
		}
	}
}
/* --------------------------- */

/* 取得熱量の計算 */

void Pipeene(int Npipe, PIPE *Pipe)
{
	int i;
	ELOUT	*Eo ;
	
	for (i = 0; i < Npipe; i++, Pipe++)
	{
		Pipe->Tin = Pipe->cmp->elins->sysvin;
		
		if (Pipe->cmp->control != OFF_SW)
		{
		/****************************
		printf ( "envname=%s\n", Pipe->room->name ) ;
			***************************/
			Eo = Pipe->cmp->elouts ;
			Pipe->Tout = Pipe->Do ;
			Pipe->Q = Pipe->Do - Pipe->D1 * Pipe->Tin;
			
			if ( Pipe->room != NULL )
				Pipe->room->Qeqp += ( -Pipe->Q ) ;

			if ( Pipe->cat->type == DUCT_PDT )
			{
				Eo++ ;
				Pipe->xout = Eo->sysv ;
				Pipe->RHout = FNRhtx ( Pipe->Tout, Pipe->xout ) ;
				Pipe->hout = FNH ( Pipe->Tout, Eo->sysv ) ;
			}
			else
				Pipe->hout = -999. ;
		}
		else
			Pipe->Q = 0.0;
	}
}
/* --------------------------- */

/* 負荷計算用設定値のポインター */

int pipeldsptr(char *load, char **key, PIPE *Pipe, VPTR *vptr, char *idmrk)
{
	int err = 0;
	
	if (strcmp(key[1], "Tout") == 0)
	{
		vptr->ptr = &Pipe->Toset;
		vptr->type = VAL_CTYPE;
		Pipe->loadt = load;
		*idmrk = 't';
	}
	else if (Pipe->cat->type == DUCT_PDT && strcmp(key[1], "xout") == 0)
	{
		vptr->ptr = &Pipe->Xoset;
		vptr->type = VAL_CTYPE;
		Pipe->loadx = load;
		*idmrk = 'x';
	}
	else
		err = 1;
	return err;
}  
/* ------------------------------------------ */  

/* 負荷計算用設定値のスケジュール設定 */

void pipeldsschd(PIPE *Pipe)
{
	ELOUT *Eo;
	
	Eo = Pipe->cmp->elouts; 
	
	if (Pipe->loadt != NULL)
	{
		if (Eo->control != OFF_SW)
		{
			if (Pipe->Toset > TEMPLIMIT)
			{
				Eo->control = LOAD_SW;
				Eo->sysv = Pipe->Toset;
			}
			else
				Eo->control = OFF_SW;
		}	 
	}
	
	if (Pipe->cat->type == DUCT_PDT && Pipe->loadx != NULL) 
	{
		Eo++; 
		if (Eo->control != OFF_SW)
		{
			if (Pipe->Xoset > 0.0)
			{
				Eo->control = LOAD_SW;
				Eo->sysv = Pipe->Xoset;
			}
			else
				Eo->control = OFF_SW;
		}	
	}
}

/* --------------------------- */

void pipeprint(FILE *fo, int id, int Npipe, PIPE *Pipe)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npipe > 0)
            fprintf(fo, "%s %d\n", PIPEDUCT_TYPE, Npipe);
		for (i = 0; i < Npipe; i++, Pipe++)
			fprintf(fo, " %s 1 5\n", Pipe->name);
		break;
		
	case 1:
		for (i = 0; i < Npipe; i++, Pipe++)
		{	 
			fprintf(fo,"%s_c c c %s_G m f %s_Ti t f %s_To t f %s_Q q f\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name, Pipe->name);
		}
		break;
		
	default:
		for (i = 0; i < Npipe; i++, Pipe++)
            fprintf(fo, "%c %6.4g %4.1lf %4.1lf %.2lf\n",
   	        Pipe->cmp->elouts->control, Pipe->cmp->elouts->G,
			Pipe->Tin, Pipe->cmp->elouts->sysv, Pipe->Q);
		break;
	}
}

/* --------------------------- */

/* 日積算値に関する処理 */

void pipedyint(int Npipe, PIPE *Pipe)
{
	int i;
	
	for (i = 0; i < Npipe; i++, Pipe++)
	{
		svdyint(&Pipe->Tidy);
		qdyint(&Pipe->Qdy);
	}
}

void pipemonint(int Npipe, PIPE *Pipe)
{
	int i;
	
	for (i = 0; i < Npipe; i++, Pipe++)
	{
		svdyint(&Pipe->mTidy);
		qdyint(&Pipe->mQdy);
	}
}

void pipeday(int Mon, int Day, int ttmm, int Npipe, PIPE *Pipe, int Nday, int SimDayend)
{
	int i;
	
	for (i = 0; i < Npipe; i++, Pipe++)
	{
		// 日集計
		svdaysum(ttmm, Pipe->cmp->control, Pipe->Tin, &Pipe->Tidy);
		qdaysum(ttmm, Pipe->cmp->control, Pipe->Q, &Pipe->Qdy);	 

		
		// 月集計
		svmonsum(Mon, Day, ttmm, Pipe->cmp->control, Pipe->Tin, &Pipe->mTidy, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Pipe->cmp->control, Pipe->Q, &Pipe->mQdy, Nday, SimDayend);	 
	}
}

void pipedyprt(FILE *fo, int id, int Npipe, PIPE *Pipe)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npipe > 0)
            fprintf(fo, "%s %d\n", PIPEDUCT_TYPE, Npipe);
		for (i = 0; i < Npipe; i++, Pipe++)
			fprintf(fo, " %s 1 14\n", Pipe->name);
		break;
		
	case 1:
		for (i = 0; i < Npipe; i++, Pipe++)
		{	 
			fprintf(fo,"%s_Ht H d %s_T T f ", Pipe->name, Pipe->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name);
		}
		break;
		
	default:
		for (i = 0; i < Npipe; i++, Pipe++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Pipe->Tidy.hrs, Pipe->Tidy.m, 
				Pipe->Tidy.mntime, Pipe->Tidy.mn, 
				Pipe->Tidy.mxtime, Pipe->Tidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Pipe->Qdy.hhr, Pipe->Qdy.h);
            fprintf(fo, "%1ld %3.1lf ", Pipe->Qdy.chr, Pipe->Qdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Pipe->Qdy.hmxtime, Pipe->Qdy.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Pipe->Qdy.cmxtime, Pipe->Qdy.cmx);
		}
		break;
	}
}

void pipemonprt(FILE *fo, int id, int Npipe, PIPE *Pipe)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npipe > 0)
            fprintf(fo, "%s %d\n", PIPEDUCT_TYPE, Npipe);
		for (i = 0; i < Npipe; i++, Pipe++)
			fprintf(fo, " %s 1 14\n", Pipe->name);
		break;
		
	case 1:
		for (i = 0; i < Npipe; i++, Pipe++)
		{	 
			fprintf(fo,"%s_Ht H d %s_T T f ", Pipe->name, Pipe->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n\n",
				Pipe->name, Pipe->name, Pipe->name, Pipe->name);
		}
		break;
		
	default:
		for (i = 0; i < Npipe; i++, Pipe++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Pipe->mTidy.hrs, Pipe->mTidy.m, 
				Pipe->mTidy.mntime, Pipe->mTidy.mn, 
				Pipe->mTidy.mxtime, Pipe->mTidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Pipe->mQdy.hhr, Pipe->mQdy.h);
            fprintf(fo, "%1ld %3.1lf ", Pipe->mQdy.chr, Pipe->mQdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Pipe->mQdy.hmxtime, Pipe->mQdy.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Pipe->mQdy.cmxtime, Pipe->mQdy.cmx);
		}
		break;
	}
}

/*  配管、ダクト内部変数のポインター  */

int pipevptr(char **key, PIPE *Pipe, VPTR *vptr)
{
	int err = 0;
	
	if (strcmp(key[1], "Tout") == 0)
	{
		vptr->ptr = &Pipe->Tout;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "hout") == 0)
	{
		vptr->ptr = &Pipe->hout;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "xout") == 0)
	{
		vptr->ptr = &Pipe->xout;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "RHout") == 0)
	{
		vptr->ptr = &Pipe->RHout;
		vptr->type = VAL_CTYPE;
	}
	else
		err = 1;
	
	return err;
}
