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

/*  mcvav.c  */

/*  VALV */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnlib.h"
#include "psy.h"
#include "fnesy.h"
#include "winerror.h"

/* ------------------------------------------ */

void	Valvcountreset ( int Nvalv, VALV *Valv )
{
	int	i ;

	for ( i = 0; i < Nvalv; i++, Valv++ )
		Valv->count = 0 ;
}

/***********************************************/

void	Valvcountinc ( int Nvalv, VALV *Valv )
{
	int	i ;

	for ( i = 0; i < Nvalv; i++, Valv++ )
		Valv->count++ ;
}

// 通常はバルブの上流の流量に比率を乗じるが、基準となるOMvavが指定されている場合には、この流量に対する比率とする
// OMvavが指定されているときだけの対応
void	Valvinit ( int NValv, VALV *Valv, int NMpath, MPATH *Mpath0 )
{
	int	i, j, k ;
	PLIST	*Plist ;
	VALV	*CValv ;
	MPATH	*Mpath ;
	PELM	*Pelm ;

	for ( k = 0; k < NValv; k++, Valv++ )
	{
		if ( Valv->cmp->monPlistName != NULL )
		{
			Mpath = Mpath0 ;
			for ( i = 0; i < NMpath; i++, Mpath++ )
			{
				Plist = Mpath->plist ;
				for ( j = 0; j < Mpath->Nlpath; j++, Plist++ )
				{
					if (Plist->plistname != NULL && strcmp(Valv->cmp->monPlistName, Plist->plistname) == 0)
					{
						Valv->monPlist = Plist ;

						Valv->MGo = &Plist->G ;

						if ( Valv->cmb != NULL )
						{
							CValv = (VALV *) Valv->cmb->eqp ;
							CValv->monPlist = Plist ;
							CValv->MGo = &Plist->G ;
						}
						break ;
					}
				}
			}
		}
		else
		{
			Pelm = Valv->Plist->pelm + Valv->Plist->Nelm - 1 ;
			Valv->MGo = Pelm->cmp->elouts->lpath->Go ;
			Valv->monPlist = Pelm->cmp->elouts->lpath ;

			if ( Valv->cmb != NULL )
			{
				CValv = (VALV *) Valv->cmb->eqp ;
				CValv->monPlist = Valv->monPlist ;
				CValv->MGo = &Valv->monPlist->G ;
			}
		}
	}
}

void	Valvene ( int Nvalv, VALV *Valv, int *Valvreset )
{
	int		i ;
	//double	Tset ;
	double	T1, T2 ;
	VALV	*Vcb ;
	char	*etype ;
	extern int	DEBUG ;
	double	r ;

	for ( i = 0; i < Nvalv; i++, Valv++ )
	{
		etype = Valv->cmp->eqptype ;
		if ( strcmp ( etype, TVALV_TYPE ) == 0 && Valv->org == 'y' )
		{
			if ( Valv->mon->elouts->control != OFF_SW )
				//if ( *Valv->MGo > 0.0 )
			{
				//Tset = Valv->mon->elouts->sysv ;
				T1 = *Valv->Tin ;
				Vcb = ( VALV * ) Valv->cmb->eqp ;
				T2 = *Vcb->Tin ;

				if ( fabs ( *Valv->Tout - *Valv->Tset ) >= 1.0e-3 && fabs ( T1 - T2 ) >= 1.0e-3 )
				{
					r = ( *Valv->Tset - T2 ) / ( T1 - T2 ) ;

					r = dmin ( 1.0, dmax ( r, 0.0 )) ;
					Valv->x = r ;
					Vcb->x = 1.0 - r ;

					//Valv->x = ( *Valv->Tset - T2 ) / ( T1 - T2 ) ;
					Valv->Plist->Gcalc = r * *Valv->MGo ;
					Vcb->Plist->Gcalc = ( 1.0 - r ) * *Valv->MGo ;
					(*Valvreset)++ ;

					if ( DEBUG )
					{
						printf ( "<Valvene> Valvname=%s G=%lf\n", Valv->name, Valv->Plist->G ) ;
						printf ( "    T1=%.1lf T2=%.1lf Tset=%.1lf\n", T1, T2, *Valv->Tset ) ;
					}
				}
				else
				{
					Valv->Plist->Gcalc = Valv->x * *Valv->MGo ;
					Vcb->Plist->Gcalc = ( 1.0 - Valv->x ) * *Valv->MGo ;
					(*Valvreset)++ ;
				}
			}
		}
	}
}

/************************************************************************/

void	ValvControl ( FILE *fi,  int Ncompnt, COMPNT *Compnt, SCHDL *Schdl, SIMCONTL *Simc,
					 WDAT *Wd, VPTR *vptr )
{
	char	s[SCHAR] ; //, ss[SCHAR] ;
	VALV	*Valv, *Vb ;
	COMPNT	*Vc ;
	int		k, i ;
	long	ad ;
	ELIN	*elins ;
	//ELOUT	*elout ;
	PELM	*Pelm ;

	Vb = NULL ;
	fscanf ( fi, "%s", s ) ;
	ad = ftell ( fi ) ;

	Vc = Compntptr ( s, Ncompnt, Compnt ) ;
	if ( Vc == NULL )
		Eprint ( "<CONTRL>", s ) ;

	vptr->ptr = &Vc->control ;
	vptr->type = SW_CTYPE;

	Valv = ( VALV * ) Vc->eqp ;
	Valv->org = 'y' ;
	while ( fscanf ( fi, "%s", s ) != EOF )
	{
		if ( *s == ';' )
		{
			fseek ( fi, ad, SEEK_SET ) ;
			break ;
		}

		if ( strcmp ( s, "-init" ) == 0 )
		{
			fscanf ( fi, "%s", s ) ;
			ad = ftell ( fi ) ;

			if (( k = idsch ( s, Schdl->Sch, NULL )) >= 0 )
				Valv->xinit = &Schdl->val[k] ;
			else
				Valv->xinit = envptr ( s, Simc, Ncompnt, Compnt, Wd, NULL ) ;
		}
		/************************************
		else if ( strcmp ( s, "-in" ) == 0 )
		{
		fscanf ( fi, "%s %s", s, ss ) ;
		ad = ftell ( fi ) ;

		Valv->Tin = envptr ( s, Simc, Ncompnt, Compnt, Wd ) ;
		Vb = ( VALV * ) Valv->cmb->eqp ;
		Vb->Tin = envptr ( ss, Simc, Ncompnt, Compnt, Wd ) ;
		}
		/*******************************************/
		else if ( strcmp ( s, "-Tout" ) == 0 )
		{
			fscanf ( fi, "%s", s ) ;
			ad = ftell ( fi ) ;

			if (( k = idsch ( s, Schdl->Sch, NULL )) >= 0 )
				Valv->Tset = &Schdl->val[k] ;
			else
				Valv->Tset = envptr ( s, Simc, Ncompnt, Compnt, Wd, NULL ) ;
			//Valv->mon = Compntptr ( ss, Ncompnt, Compnt ) ;
			Pelm = Valv->Plist->pelm + Valv->Plist->Nelm - 1 ;
			Valv->mon = Pelm->cmp ;
			Valv->Tout = &Valv->mon->elouts->sysv ;
			Valv->MGo = &Pelm->cmp->elouts->lpath->G ;
			Vb = ( VALV * ) Valv->cmb->eqp ;
			Vb->MGo = &Pelm->cmp->elouts->lpath->G ;

			/*********************/
			if ( Valv->Plist->Go == NULL )
			{
				Vb = ( VALV * ) Valv->cmb->eqp ;
				Valv->Plist->Go = Valv->mon->elouts->lpath->Go ;
				Vb->Plist->Go = Valv->mon->elouts->lpath->Go ;
			}
			/**************************************/
		}
	}

	/****************************************/
	Pelm = Valv->Plist->pelm + Valv->Plist->Nelm - 1 ;
	elins = Pelm->cmp->elins ;

	for ( i = 0; i < Pelm->cmp->Nin; i++, elins++ )
	{
		if ( strcmp ( elins->lpath->valv->name, Valv->name ) == 0 )
			Valv->Tin = &elins->sysvin ;
		//Pelm = Vb->Plist->pelm + Vb->Plist->Nelm - 1 ;
		//elins = Pelm->cmp->elins ;
		//elins++ ;
		else
			Vb->Tin = &elins->sysvin ;
	}
	/*****************************************/

	/***********************************************
	Pelm = Valv->Plist->pelm + Valv->Plist->Nelm - 2 ;
	elout = Pelm->cmp->elouts ;
	Valv->Tin = &elout->sysv ;
	Pelm = Vb->Plist->pelm + Vb->Plist->Nelm - 2 ;
	elout = Pelm->cmp->elouts ;
	Vb->Tin = &elout->sysv ;
	/**********************************************/
}
