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

/*  syseqv.c   */

#include "common.h"
#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"
#include <string.h>
#include <stdlib.h>

//#define DPRINT 0

/* システム方程式の作成およびシステム変数の計算 */

void Syseqv(int Nelout, ELOUT *Elout, SYSEQ *Syseq)
{
	extern int	dayprn ;
	extern FILE	*ferr ;
	ELOUT	*elout, *elov, **eleq, **elosv ;
	ELIN	*elin;
	double	*a, *b, *c, *sysmcf, *syscv, *Y;
	double  *cfin;
	int		i, j, m = 0, n = 0, nn, Nsv;
	char	*mrk, *st ;
	
	//	sysmcf = Syseq->eqcf;
	//	syscv = Syseq->eqcv;
	//	Y = Syseq->Y;
	
	//	matinit ( sysmcf, SYSEQMX*SYSEQMX ) ;
	//	matinit ( syscv, SYSEQMX ) ;
	//	matinit ( Y, SYSEQMX ) ;
	
	Syseq->a = ' ' ;
	elout = elov = NULL ;
	eleq = elosv = NULL ;
	elin = NULL ;
	a = b = c = sysmcf = syscv = Y = cfin = NULL ;
	mrk = st = NULL ;

	if ( Nelout > 0 )
	{
		if (( eleq = ( ELOUT ** ) malloc ( Nelout * sizeof ( ELOUT * ))) == NULL )
			Ercalloc ( Nelout, "Syseqv  eleq alloc" ) ;
		
		for ( i = 0; i < Nelout; i++ )
			eleq[i] = NULL ;
	}
	
	if ( Nelout > 0 )
	{
		if (( elosv = ( ELOUT ** ) malloc ( Nelout * sizeof ( ELOUT * ))) == NULL )
			Ercalloc ( Nelout, "Syseqv  elosv alloc" ) ;
		
		for ( i = 0; i < Nelout; i++ )
			elosv[i] = NULL ;
	}
	
	mrk = scalloc ( Nelout, "<Syseqv> mrk alloc" ) ;
	
	for (i = 0; i < Nelout; i++, Elout++)
	{
		if ( DEBUG )
		{
			printf("xxx syseqv  Eo name=%s control=%c sysld=%c i=%d MAX=%d\n",
				Elout->cmp->name, Elout->control, Elout->sysld, i, Nelout);
		}

		if ( dayprn && ferr )
		{
			fprintf(ferr,"xxx syseqv  Eo name=%s control=%c sysld=%c i=%d MAX=%d\n",
				Elout->cmp->name, Elout->control, Elout->sysld, i, Nelout);
		}
		
		/*************************
		if ( strcmp ( Elout->cmp->name, "FH" ) == 0 )
		printf ( "xxx syseqv  Eo name=%s control=%c sysld=%c i=%d MAX=%d\n",
		Elout->cmp->name, Elout->control, Elout->sysld, i, Nelout);
		*****************************/
		
		if (Elout->control != LOAD_SW
			&& Elout->control != FLWIN_SW && Elout->control != BATCH_SW)
		{   
			Elout->sv = -1;
			Elout->sysv = 0.0;
		}

		//printf ( "control=%c [i=%d m=%d n=%d] %s  G=%lf\n",Elout->control, i, m, n, Elout->cmp->name, Elout->G ) ;
		
		if (Elout->control == ON_SW)
		{
			if ( DEBUG )
				printf ( "ON_SW = [i=%d m=%d n=%d] %s  G=%lf\n", i, m, n, Elout->cmp->name, Elout->G ) ;

			//			Syseq->eleq[m] = Elout;
			eleq[m] = Elout ;
			
			//			Syseq->elosv[n] = Elout;
			elosv[n] = Elout;
			//			Syseq->mrk[n] = SYSV_EQV;
			mrk[n] = SYSV_EQV;
			Elout->sv = n;
			Elout->sld = -1;
			n++;
			
			if (Elout->sysld == 'y')
			{
				//				Syseq->elosv[n] = Elout;
				elosv[n] = Elout;
				//				Syseq->mrk[n] = LOAD_EQV;
				mrk[n] = LOAD_EQV;
				Elout->sld = n;
				n++;
			}
			m++;     
		}
		else if (Elout->control == LOAD_SW && Elout->sysld != 'y')
		{
			//			Syseq->eleq[m] = Elout;
			eleq[m] = Elout;
			Elout->sv = -1;
			Elout->sld = -1;
			m++;
		}
	}
	Nsv = n;
	
	//	if ( Nsv > SYSEQMX )
	//	{
	//		sprintf ( Err, "syseq=%d  SYSEQMX=%d\n", Nsv, SYSEQMX ) ;
	//		Eprint ( "<Syseqv>", Err ) ;
	//	}
	
	sysmcf = dcalloc ( Nsv * Nsv, "<Syseqv> sysmcf" ) ;
	syscv = dcalloc ( Nsv, "<Syseqv> syscv" ) ;
	Y = dcalloc ( Nsv, "<Syseqv> Y" ) ;
	
	a = sysmcf ;
	b = syscv;
	
	for (i = 0; i < Nsv; i++)
	{
		//		elout = Syseq->eleq[i];
		elout = eleq[i];
		
		if ( DEBUG )
		{  
			printf("xxx syseqv Elout=%d %s Ni=%d cfo=%lf\n", 
				i, elout->cmp->name, elout->Ni, elout->coeffo);
		}

		if ( dayprn && ferr )
		{  
			fprintf(ferr,"xxx syseqv Elout=%d %s Ni=%d cfo=%lf\n", 
				i, elout->cmp->name, elout->Ni, elout->coeffo);
		}
		
		c = a;
		
		matinit ( c, Nsv ) ;
		//for (j = 0; j < Nsv; j++, c++)
		//{
		/*********************
		printf ( "j=%d  MAX=%d\n", j, Nsv ) ;
			*********************/
		//	*c = 0.0 ;
			//c++;
		//}
		
		*b = elout->Co;
		
		if ((n = elout->sv) >= 0)
		{
			*(a + n) = elout->coeffo;
			if ((nn = elout->sld) >= 0)
				*(a + nn) = -1.0;
		}
		else
			*b -= elout->coeffo * elout->sysv;
		
			/*****
			printf("xx syseqv   i=%d j=%d   a=%lf\n", i, j, *(a+i));
		**************/
		
		elin = elout->elins;   
		cfin = elout->coeffin;
		/**************
		printf ("cmp->name=%s Ni=%d\n", elout->cmp->name, elout->Ni ) ;
		********************/
		
		for (j = 0; j < elout->Ni; j++, elin++, cfin++)
		{
		/*********************
		printf ("name=%s\n", elout->cmp->name) ;
			/**********************/
			
			if ((elov = elin->upv) != NULL)
			{
				if ( DEBUG )
				{ 
					printf("xxx syseqv Elout=%d %s  in=%d elov=%s  control=%c sys=%lf\n",
						i, elout->cmp->name, j, 
						elov->cmp->name, elov->control, elov->sysv);
				}

				if ( dayprn && ferr )
				{ 
					fprintf(ferr,"xxx syseqv Elout=%d %s  in=%d elov=%s  control=%c sys=%lf\n",
						i, elout->cmp->name, j, 
						elov->cmp->name, elov->control, elov->sysv);
				}
				
				if (elov->control == ON_SW)
				{
					n = elin->upv->sv;
					*(a + n) += *cfin;
				}
				else if (elov->control == LOAD_SW 
					|| elov->control == FLWIN_SW || elov->control == BATCH_SW)
				{
					if ( DEBUG )
					{   
						printf("xxx syseqv elov=%s  control=%c sys=%lf\n",
							elov->cmp->name, elov->control, elov->sysv);
					}

					if ( dayprn && ferr )
					{   
						fprintf(ferr,"xxx syseqv elov=%s  control=%c sys=%lf\n",
							elov->cmp->name, elov->control, elov->sysv);
					}
					
					*b -= *cfin * elov->sysv;	   
				}
				
				/*********
				printf("xx syseqv   i=%d j=%d  n=%d a=%lf\n", i, j, n, *(a+n));
				************/
			}
			//elin++;
			//cfin++;
		}   
		if ( DEBUG )
		{  
			printf("xx syseqv  i=%d  b=%lf\n", i, *b);
		}
		
		a += Nsv ;
		b++;
	}
	
	/********* 連立方程式 ***********/
	
	if ( DEBUG )
	{
		seqprint("%g\t", Nsv, sysmcf, "%g", syscv);

		//for ( i = 0; i < Nsv; i++ )
		//	printf ( "%g\n", sysmcf[i+Nsv*7] ) ;
	}
	
	if (Nsv > 0)
	{
	/**********************
	matprint("%6.2lf ", Nsv, sysmcf) ;
		/**********************/
		
		matinv(sysmcf, Nsv, Nsv, "<Syseqv>" );
		matmalv(sysmcf, syscv, Nsv, Nsv, Y);
	}
	
	st = mrk ;
	for (i=0; i<Nsv; i++, st++)
	{
		//		if (Syseq->mrk[i] == SYSV_EQV)
		if ( *st == SYSV_EQV)
		{
			//			Syseq->elosv[i]->sysv =Y[i];
			elosv[i]->sysv =Y[i];
		}
		//		else if (Syseq->mrk[i] == LOAD_EQV)
		else if ( *st == LOAD_EQV)
		{
			//			Syseq->elosv[i]->load = Y[i];
			elosv[i]->load = Y[i];
		}
	}
	
	if ( DEBUG )
	{
		st = mrk ;
		for (i=0; i<Nsv; i++, st++) 
			printf("Y[%d]=%.5lf  mrk=%c  Elo=%s\n",
				i, Y[i], *st, elosv[i]->cmp->name);
		//		Syseq->mrk[i], Syseq->elosv[i]->cmp->name);
		printf("\n");
	}

	if ( dayprn && ferr )
	{
		st = mrk ;
		for (i=0; i<Nsv; i++) 
			fprintf(ferr,"Y[%d]=%6.3lf  mrk=%c  Elo=%s\n", i, Y[i], *st, elosv[i]->cmp->name);
		//		Syseq->mrk[i], Syseq->elosv[i]->cmp->name);
		fprintf(ferr,"\n");
	}
	
	free ( sysmcf ) ;
	free ( syscv ) ;
	free ( Y ) ;
	free ( mrk ) ;
	free ( eleq ) ;
	free ( elosv ) ;
}
