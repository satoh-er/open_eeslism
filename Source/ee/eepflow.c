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

/*  pflow.c  */

/* 流量の設定 */
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"
#include "winerror.h"
#include "fnmcs.h"

/* --------------------------------------------------- */

/* システム要素の流量設定 */

void Pflow(int Nmpath, MPATH *Mpath, WDAT *Wd)
{
	int		m, i, j, n, NG ;
	MPATH	*mpi;
	PLIST	*Plist, *pl;
	PELM	*Pelm;
	ELIN	*eli;
	ELOUT	*elo ;
	COMPNT	*cmp ;
	VALV	*vc, *vcmb ;
	double	*A, *Y, *X, Go ;
//	PUMP	*Pump;
	double	G;
	char	Err[SCHAR], s[SCHAR]; 
	/*---- Satoh Debug VAV  2000/12/6 ----*/
	VAV		*vav;
	extern int	dayprn ;
	double G0 ;
	
	mpi = Mpath;
	X = Y = A = NULL ;
	
	if ( Nmpath > 0 )
	{
		for (m = 0; m < Nmpath; m++, Mpath++)
		{
			Plist = Mpath->plist;
			
			if ( DEBUG )
			{
				printf ( "m=%d mMAX=%d name=%s\n", m, Nmpath, Mpath->name ) ;
			}

			// 流量が既知の末端流量の初期化
			for (i = 0; i < Mpath->Nlpath; i++, Plist++)
			{
				if (Plist->Go != NULL && Plist->Nvalv == 0)
					Plist->G = *Plist->Go;
			}

			Plist = Mpath->plist;
			for (i = 0; i < Mpath->Nlpath; i++, Plist++)
			{
				Plist->G = 0.0 ;
				
				if ( DEBUG )
				{
					printf ( "i=%d iMAX=%d name=%s\n", i, Mpath->Nlpath, Plist->name ) ;
				}
				
				// 流量が既知の末端経路
				if (Plist->Go != NULL && Plist->Nvalv == 0)
					Plist->G = *Plist->Go;
				else if ( Plist->Go != NULL && Plist->Nvalv > 0 
					|| Plist->NOMVAV > 0 || ( Plist->Go == NULL && Plist->Nvalv > 0 && Plist->UnknownFlow == 1 ))
				//else if (  Plist->Nvalv > 0 )
				{
					// 二方弁の計算
					if ( Plist->Go != NULL && Plist->valv != NULL && strcmp ( Plist->valv->cmp->eqptype, VALV_TYPE ) == 0 )
					{
						Plist->G = *Plist->Go ;
						vc = Plist->valv ;
						if ( vc == NULL || vc->org == 'y' )
						{
							if ( vc->x < 0.0 )
							{
								sprintf ( s, "%s のバルブ開度 %lf が不正です。",
									vc->name, vc->x ) ;
								Eprint ( "<Pflow>", s ) ;
							}
							Plist->G = vc->x * *Plist->Go ;
						}
						else
						{
							vcmb = ( VALV * ) vc->cmb->eqp ;
							Plist->G = ( 1.0 - vcmb->x ) * *Plist->Go ;
						}
					}
					// 三方弁の計算
					else if (Plist->valv != NULL && Plist->valv->MGo != NULL
						&& *Plist->valv->MGo > 0.0 && Plist->control != OFF_SW )
					{
						//lpathscdd ( ON_SW, Plist ) ;
						vc = Plist->valv ;
						vcmb = ( VALV * ) vc->cmb->eqp ;
						
						/*********************
						if ( vc->count > 0 && vc->org == 'y' )
						{
						vc->x = ( *vc->Tset - *vcmb->Tin )
						/ ( *vc->Tin - *vcmb->Tin ) ;
						}
						/************************************/
						
						/******************************/
						//if ( strcmp( vc->cmp->eqptype, TVALV_TYPE ) != 0 )
						//{
							if ( vc->org == 'y' )
							{
								//Plist->G = vc->x * *Plist->Go ;
								Plist->G = vc->x * *vc->MGo ;
							}
							else
							{
								//Plist->G = ( 1.0 - vcmb->x ) * *Plist->Go ;
								Plist->G = ( 1.0 - vcmb->x ) * *vc->MGo ;
							}
							/**************************************/
							//printf("ValvName=%s G=%g\n", vc->cmp->name, Plist->G) ;
						//}
						//else
						//{
							//Plist->G = Plist->Gcalc ;
							
						//}

						if ( Plist->G > 0. )
							Plist->control = ON_SW ;
						
						//printf("Valv->Name=%s  G=%g\n", vc->name, Plist->G ) ;
					}
					else if (Plist->valv != NULL && Plist->valv->MGo != NULL && *Plist->valv->MGo <= 0.0 )
						Plist->G = 0.0 ;
					else if (Plist->valv != NULL && Plist->valv->count > 0 )
						Plist->G = Plist->Gcalc ;
					else if ( Plist->NOMVAV > 0 )
						Plist->G = OMflowcalc(Plist->OMvav, Wd) ;
					
					if ( Plist->G <= 0.0 )
						lpathscdd ( OFF_SW, Plist ) ;

					if ( Plist->G > 0. )
						Plist->control = ON_SW ;
				}
				//else if (Plist->Npump > 0)
				//{
					/* ポンプによる流量設定 */
					
					
					//Pelm = Plist->pelm;
					//G = -999.0;
					/************************
					for (j = 0; j < Plist->Nelm; j++, Pelm++)
					{
						if (strcmp(Pelm->cmp->eqptype, PUMP_TYPE) == 0)
						{		
						/***********************************/
							//Pump = (PUMP *) Pelm->cmp->eqp;
							//Pump = Plist->Pump ;
							
							//if (Pump == NULL )
								//printf("xxxx <Pflow> ポンプの設定がされていません。\n") ;

							//if (strcmp(Pump->cat->type, PUMP_C) == 0
								//|| strcmp(Pump->cat->type, FAN_C) == 0)
								//G = dmax(G, Pump->G);
							/*********************************
						}
					}
					/***************************************/
					//Plist->G = G;
				//}
				/*---- Satoh Debug VAV  2000/12/6 ----*/
				else if (Plist->Nvav > 0)
				{
					/* VAVユニット時の流量 */
					
					Pelm = Plist->pelm;
					G = -999.0;
					for (j = 0; j < Plist->Nelm; j++, Pelm++)
					{
						if (strcmp(Pelm->cmp->eqptype, VAV_TYPE) == 0
							|| strcmp(Pelm->cmp->eqptype, VWV_TYPE) == 0)
						{		
							vav = (VAV *) Pelm->cmp->eqp;
							
							if (vav->count == 0)
								G = dmax(G, vav->cat->Gmax);
							else
							{
								G = dmax(G, vav->G) ;
								//printf ( "%d----VAVname=%s G=%lf\n",
								//	vav->count, vav->name, G ) ;
							}
							
							/*********************
							printf ("%d---VAVname=%s G=%lf\n",
							vav->count, vav->name, G ) ;
							********************/
						}
					}
					Plist->G = G;
				}
				else if (Plist->rate != NULL)
					Plist->G = *Mpath->G0 * *Plist->rate ;
				else if (Plist->batch != 'y')
				{
					if ( Plist->Go != NULL )
						Go = *Plist->Go ;
					else
						Go = 0.0 ;
					
					if(Plist->pelm != NULL)
					{
						sprintf(Err, "Mpath=%s  lpath=%Ild  elm=%s  Go=%lf\n",
							Mpath->name, Plist - mpi->plist, 
							Plist->pelm->cmp->name, Go);
					}
				}
			}
			
			NG = Mpath->NGv ;
			
			X = dcalloc ( NG, "<eepflow>" ) ;
			Y = dcalloc ( NG, "<eepflow>" ) ;
			A = dcalloc ( NG * NG, "<eepflow>" ) ;
			
			for ( i = 0; i < NG; i++ )
			{
				if ( DEBUG )
				{
					printf ( "i=%d iMAX=%d\n", i, NG ) ;
				}
				
				cmp = Mpath->cbcmp[i] ;
				
				if ( DEBUG )
				{
					printf ( "<Pflow> Name=%s\n", cmp->name ) ;
				}
				
				eli = cmp->elins ;
				
				for ( j = 0; j < cmp->Nin; j++, eli++ )
				{
				/**************
				printf ("Pflow>> 流入　name=%s\tj=%d\tn=%d\n",
				cmp->name, j, eli->lpath->n ) ;
					/********************/
					if ( DEBUG )
					{
						printf ( "j=%d jMAX=%d\n", j, cmp->Nin ) ;
					}
					
					if ( eli->lpath->Go != NULL
						|| eli->lpath->Nvav !=0
						//|| (eli->lpath->Npump != 0 
						//   && (strcmp(eli->lpath->Pump->cat->type,PUMP_C)==0
						//   ||strcmp(eli->lpath->Pump->cat->type,FAN_C)==0))
						|| eli->lpath->Nvalv != 0 || eli->lpath->rate != NULL || eli->lpath->NOMVAV != 0 )
						Y[i] -= eli->lpath->G ;
					/*else if(eli->lpath->rate != NULL)
					{
						n = eli->lpath->n ;
						A[i*NG+n] = *eli->lpath->rate ;
					}*/
					else
					{
						n = eli->lpath->n ;
						
						/**************************************
						printf ("Pflow>> 流入　name=%s\tj=%d\tn=%d\n", cmp->name, j, n ) ;
						/******************************/
						
						if (n < 0 || n >= NG)
						{
							sprintf ( Err, "n=%d", n ) ;
							Eprint ( "<Pflow>", Err ) ;
							preexit ( ) ;
							exit (EXIT_PFLOW) ;
						}
						
						A[i*NG+n] = 1.0 ;
					}
				}
				
				////////
				elo = cmp->elouts ;
				
				for ( j = 0; j < cmp->Nout; j++, elo++ )
				{
				/*********************
				printf ("Pflow>> 流出　Nout=%d name=%s\tj=%d\n",
				cmp->Nout, cmp->name, j ) ;
				printf ("Go=%lf n=%d\n", elo->lpath->Go, elo->lpath->n ) ;
					****************************/
					
					if ( elo->lpath->Go != NULL
						|| elo->lpath->Nvav !=0
						//|| (elo->lpath->Npump != 0 && (strcmp(elo->lpath->Pump->cat->type,PUMP_C)==0
						//   ||strcmp(elo->lpath->Pump->cat->type,FAN_C)==0))
						|| elo->lpath->Nvalv != 0 || elo->lpath->rate != NULL )
						Y[i] += elo->lpath->G ;
					/*else if(elo->lpath->rate != NULL)
					{
						n = elo->lpath->n ;
						A[i*NG+n] = - *elo->lpath->rate ;
					}*/
					else
					{
						n = elo->lpath->n ;
						
						if (n < 0 || n >= NG)
						{
							sprintf ( Err, "n=%d", n ) ;
							Eprint ( "<Pflow>", Err ) ;
							preexit ( ) ;
							exit (EXIT_PFLOW) ;
						}
						
						A[i*NG+n] = -1.0 ;
					}
				}
			}
			
			if ( NG > 0 )
			{
				
				if ( DEBUG )
				{
					for ( i = 0; i < NG; i++ )
					{
						printf ( "%s\t", Mpath->cbcmp[i]->name ) ;
						
						for ( j = 0; j < NG; j++ )
							printf ( "%6.1lf", A[i*NG+j] ) ;
						
						printf ( "\t%.5lf\n", Y[i] ) ;
					}
				}
				
				if ( dayprn && ferr )
				{
					for ( i = 0; i < NG; i++ )
					{
						fprintf ( ferr, "%s\t", Mpath->cbcmp[i]->name ) ;
						
						for ( j = 0; j < NG; j++ )
							fprintf ( ferr, "\t%.1g", A[i*NG+j] ) ;
						
						fprintf ( ferr, "\t\t%.2g\n", Y[i] ) ;
					}
				}
				
				if ( NG > 1 )
				{
					matinv ( A, NG, NG, "<Pflow>" ) ;
					matmalv ( A, Y, NG, NG, X ) ;
				}
				else
					*X = *Y / *A ;
				
				if ( DEBUG )
				{
					printf ( "<Pflow>  Flow Rate\n" ) ;
					for ( i = 0; i < NG; i++ )
						printf ( "\t%6.2lf\n", X[i] ) ;
				}
				
				if ( dayprn && ferr )
				{
					for ( i = 0; i < NG; i++ )
						fprintf ( ferr, "\t\t%.2g\n", X[i] ) ;
				}
			}
			
			for ( i = 0; i < NG; i++ )
			{
				pl = Mpath->pl[i] ;
				pl->G = X[i] ;
			}
			
			Plist = Mpath->plist;
			
			for (i = 0; i < Mpath->Nlpath; i++, Plist++)
			{
				
				if ( DEBUG )
				{
					printf("<< Pflow >> e i=%d iMAX=%d control=%c G=%g\n", 
						i, Mpath->Nlpath, Plist->control, Plist->G);
				}
				
				if (Plist->control == OFF_SW)
					Plist->G = 0.0;
				
				else if (Plist->G <= 0.0)
				{
					// 負であればエラーを表示する
					//if (Plist->G < 0. )
					//	printf("<%s>  流量が負になっています %g\n", Mpath->name, Plist->G ) ;

					Plist->G = 0.0;
					Plist->control = OFF_SW;
					lpathscdd(Plist->control, Plist);
				}
				
				Pelm = Plist->pelm;
				for (j = 0; j < Plist->Nelm; j++, Pelm++)
				{
					if (Pelm->out != NULL)
						Pelm->out->G = Plist->G;
					
					if ( DEBUG )
					{
						if ( Pelm->out != NULL )
							G0 = Pelm->out->G ;
						else
							G0 = 0.0 ;
						
						printf("< Pflow > j=%d\tjMAX=%d\tPelm-G=%g\tPlist->G=%g\n",
							j, Plist->Nelm, G0, Plist->G);
					}
				}
			}
			free ( X ) ;
			free ( Y ) ;
			free ( A ) ;
      }
   }
}
