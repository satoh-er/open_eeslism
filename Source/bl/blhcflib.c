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

/*    bhcflib.c            */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "MODEL.h"   /*--higuchi  070918--*/
#include "fesy.h"
#include "esize.h"
#include "fnbld.h"
#include "fnlib.h"

#define  Alidmy   9.3
#define  ALITOLE  1.e-5

/* -------------------------------------------------------------- */

/*    熱伝達率に関する計算  */

void Htrcf (double *alc, double *alo, char alotype, EXSF *Exs, double Tr, int N, double *alr,
			RMSRF *Sd, char *RMmrk, WDAT *Wd )
{  
	int     n;
	double  alic, *hc;
	//double	aloc ;
	double	dT;
	extern int	dayprn ;
	
	HeapCheck("xxxxxx1");

	if(DEBUG)
		printf("Htrcf Start\n") ;

	//if ( alo != NULL )
	//	printf("llll") ;
	//if ( Wd == NULL )
	//	printf ( "dddd" ) ;
	//if ( alotype == ' ' )
	//	printf ( "wwwww" ) ;

	for ( n = 0; n < N; n++, Sd++ )
	{
		if(DEBUG)
			printf("n=%d name=%s\n", n, Sd->name) ;
		// 室内側対流熱伝達率の計算
		alic = -1.0;
		
		if (alc != NULL)
		{
			if (*alc >= 0.01)
				alic = *alc;
		}

		if(DEBUG)
			printf("alic=%lf\n", alic) ;
		
		hc = NULL ;
		HeapCheck("xxxxxx") ;
		
		if(DEBUG)
			printf("Sd->alicsch\n") ;

		//Sd->alicsch = NULL ;
		if (Sd->alicsch != NULL)
		{
			if(DEBUG)
				printf("test\n") ;

			hc = Sd->alicsch ;
			if ( *hc >= 0.00 )
				alic = *hc;
		}

		if(DEBUG)
			printf("hc set end\n") ;
		
		if ( alic < 0.0 )
		{
			dT = Sd->Ts - Tr;
			//printf("dT=%lf\n", dT ) ;
			switch ( Sd->ble )
			{
			case 'F': case 'f':
				if ( dT > 0 )
					alic = alcvup ( dT );
				else
					alic = alcvdn ( dT );
				break;
			case 'R': case 'c':
				if ( fabs(dT) <= 1.0e-3)
					alic = 0.0 ;
				else if ( dT < 0 )
					alic = alcvup ( dT );
				else
					alic = alcvdn ( dT );
				break;
			default:
				alic = alcvh ( dT );
				break;
			}
		}
		/********/
		if(DEBUG)
			printf("----- Htrcf n=%d mrk=%c alic=%lf  Sd->alic=%lf\n",
				n, Sd->mrk, alic, Sd->alic); /**********/

		//if (n == 0)
		//	printf("<Htrcf> 1 n=%d ali=%lf\n", n, Sd->ali);
		// 屋外側対流熱伝達率の計算

		if ( fabs ( alic - Sd->alic ) >= ALITOLE || Sd->mrk == '*' || Sd->PCMflg == 'Y')
		{
			*RMmrk = '*';
			Sd->mrk = '*';
			Sd->alic = alic;
			
			switch ( Sd->ble )
			{ 
			case 'W': case 'E': case 'F': case 'R':
				Sd->alo = *(Exs+Sd->exs)->alo;
				break;
			default:
				Sd->alo = Alidmy;
				break;
			}
		}
		//if (n == 0)
		//	printf("<Htrcf> 2 n=%d ali=%lf\n", n, Sd->ali);
		
		if (Sd->mrk == '*')
		{
			if ( Sd->alirsch == NULL )
				Sd->alir = *(alr + n * N + n);
			else if ( *Sd->alirsch > 0.0 )
				Sd->alir = *Sd->alirsch ;
			else
				Sd->alir = 0.0 ;

			Sd->ali = alic + Sd->alir;
		}
		//if (n == 0)
		//	printf("<Htrcf> 3 n=%d ali=%lf\n", n, Sd->ali);
		
		if ( DEBUG )
		{
			printf("----- Htrcf n=%2d ble=%c Ts=%.1lf Tr=%.1lf alic=%.3lf alir=%.3lf rmname=%s\n",
				n, Sd->ble, Sd->Ts, Tr, Sd->alic, Sd->alir, Sd->room->name);
		}
		
		if ( dayprn && ferr )
		{
			fprintf(ferr,"----- Htrcf n=%2d ble=%c Ts=%.1lf Tr=%.1lf alic=%.3lf alir=%.3lf rmname=%s\n",
				n, Sd->ble, Sd->Ts, Tr, Sd->alic, Sd->alir, Sd->room->name);           
		}
	}
	
}

/*-----------------------------------------------------*/
// 屋外側熱伝達率の計算
double	alov ( EXSF *Exs, WDAT *Wd )
{
	//int	i ;
	double	Wv, Wdre, u, Wadiff ;

	Wv = Wd->Wv ;
	Wdre = -180. + 360. / 16. * Wd->Wdre ;

	Wadiff = fabs ( Exs->Wa - Wdre ) ;
	Wadiff = ( Wadiff > 360. ) ? Wadiff - 360. : Wadiff ;
	if ( Wadiff < 45.0 )
		u = ( Wv <= 2.0 ) ? 0.5 : 0.25 * Wv ;
	else
		u = 0.3 + 0.05 * Wv ;

	return ( 3.5 + 5.6 * u ) ;
}
/* ---------------------------------------- */

/*  室内表面対流熱伝達率の計算     */


double	alcvup ( double dT )
{
	return ( 2.18 * pow ( fabs ( dT ), 0.31 ));
}

double	alcvdn ( double dT )
{
	return ( 0.138 * pow ( fabs ( dT ), 0.25 ));
}

double	alcvh ( double dT )
{
	return ( 1.78 * pow ( fabs ( dT ), 0.32 ));
}

/* --------------------------------------- */

/*  室内表面間放射熱伝達率の計算  */

void Radcf0 (double Tsav, double *alrbold, int N, RMSRF *Sd,
			 double *W, double *alr)
{
	int    n;
	double  alir,TA;
	extern double Sgm;
	
	TA = Tsav + 273.15;
	alir = 4.0 * Sgm * pow ( TA, 3.0 ) ;
	
	/*****/
	if(DEBUG)
		printf("----- Radcf0   alir=%lf  alrbold=%lf ALITOLE\n",
			alir, *alrbold);
	/*****/
	
	if ( fabs ((double)(alir - *alrbold)) >= ALITOLE)
	{
		*alrbold = alir;
		
		for (n = 0; n < N; n++, Sd++)
			Sd->mrk = '*';
		
		for (n = 0; n < N * N; n++, alr++, W++ )
		{
			*alr = alir * fabs ( *W ) ;
			
			/*****printf("----- Radcf0  n=%d alr=%lf\n",n, *alr);
            *****/
		}
	}
}
/* ------------------------------------------- */

/*  放射伝達係数の計算  */

void radex(int N,  RMSRF *Sd, double *F, double *W)
{
	int		i, j, k, l = 0, n, nn ;
	double  c, *wk, *Ff ;
	
	/****
	printf("<radex>  F[i,j]\n"); 
	matfprint(" %6.4lf", N, F);   ******/
	
	/*****************
	printf ( "\n\n N=%d\n", N ) ;
	*******************/
	
	Ff = dcalloc ( N * N, "radex" ) ;
	wk = dcalloc ( N * N, "radex" ) ;
	
	for ( l = 0, n = 0; n < N; n++ )
	{
		for ( j = 0; j < N; j++, l++ )
		{
			*(wk+l)= - *(F+l) * ( 1.0 - (Sd+j)->Ei ) / (Sd+j)->Ei ;
			
			/*************************/
			if(DEBUG)
				printf ( "j=%d F=%lf Sd=%lf wk=%lf\n", j, F[l], (Sd+j)->Ei, wk[l] ) ;
			/**************************/
			
			*(Ff+l) = - *(F+l) ;
		}
		nn = n * N + n ;
		*(wk+nn) += 1.0 / (Sd+n)->Ei;
		
		/************************/
		if(DEBUG)
			printf ( "nn=%d Sd=%lf wk=%lf\n", nn, Sd->Ei, *(wk+nn) ) ;
		/******************************/
		
		*(Ff+nn) += 1.0;
	}
	/****printf("<radex>  Ff[i,j]\n"); 
	matfprint(" %6.4lf", N, Ff); 
	/************************/
	/*******************/
	if(DEBUG)
	{
		printf("<radex>  wk\n");
		matprint(" %6.4lf", N, wk); /*********/  
	}
	
	matinv (wk, N, N, "<radex>" );
	
	/**********/
	if(DEBUG)
	{
		printf("<radex>  wkinv\n");
		matprint(" %6.4lf", N, wk);  /********/ 
	}
	
	for ( i = 0; i < N; i++ )
	{
		for ( j = 0; j < N; j++ )
		{ 
			for ( c = 0.0, k = 0; k < N; k++ )
				c += *(wk+N*i+k) * *(Ff+N*k+j) ;
			
			*(W+N*i+j) = c;
		}
	}
	HeapCheck("aaaa") ;
	free ( Ff ) ;
	HeapCheck("bbbb") ;
	free ( wk ) ;
	
	/******/
	if(DEBUG)
	{
		printf("<radex>  W[i,j]\n");
		matprint(" %6.4lf", N, W); /****/
	}
	
}
/* ------------------------------------------- */

/* 形態係数の近似計算　　*/

void formfaprx(int N, double Aroom, RMSRF *Sd, double *F)
{ 
	int i,n;
	
	for ( n = 0; n < N; n++, Sd++ )
	{ 
		*(F+n) = Sd->A / Aroom;
		/**** printf("<formfarpx>  n=%d A=%lf F=%lf Aroom=%lf\n",
		n, Sd->A, *(F+n), Aroom);*****/
		
		for ( i = 0; i < N; i++)
			*(F+i*N+n) = *(F+n) ;
	}
}   

/* ------------------------------------------- */

/*  短波長放射吸収係数 */

void Radshfc(int N,  double FArea,  double Aroom, RMSRF *Sd0, double tfsol, double eqcv, char *Rmname, double *fsolm)
{
	int		n;
	double	flr;
	RMSRF	*Sd ;
	double	Srgchk ;
	double	Sumsrg2;
	double	dblTemp;
	ROOM	*Room;

	Sd = Sd0 ;
	flr = FArea ;		// Dummy
	Room = Sd->room;
	
	Sumsrg2 = 0.;

	// tfsol:定義済みの部位日射吸収係数の合計値
	for ( n = 0; n < N; n++, Sd++ )
	{        
		Sd->eqrd = ( 1.0 - eqcv ) * Sd->A / Aroom ;
		
		dblTemp = dmax((1.0 - tfsol), 0.) * Sd->A / Aroom;
		if (Sd->fsol != NULL)
			Sd->srg = Sd->srh = Sd->srl = Sd->sra
			= *(Sd->fsol) + dblTemp;
		else
			Sd->srg = Sd->srh = Sd->srl = Sd->sra = dblTemp;
		
		/**************************
		if (Sd->ble == 'F' || Sd->ble == 'f')
		{
			flr = flrsr * Sd->A / FArea ;
			Sd->srg += flr ;
			Sd->srh += flr ;
			Sd->srl += flr ;
			Sd->sra += flr ;
		}
		****************************/
		/******************
		printf("xxx Radshfc Room=%s  n=%d A=%lf ble=%c srg=%lf\n",Rmname, n, Sd->A, Sd->ble, Sd->srg);
		/**************************/
		Sd->srg2 = Sd->srg;
		if (Sd->RStrans == 'y')
			Sd->srg2 = 0.0;
		if (Sd->tnxt > 0.)
			Sd->srg2 = Sd->srg*(1. - Sd->tnxt);
		Sumsrg2 += Sd->srg2;
	}

	Room->Srgm2 = 0.;
	if (fsolm != NULL)
		Room->Srgm2 = *fsolm;

	// 各種部位の吸収係数のチェック

	Sd = Sd0 ;
	Srgchk = 0. ;

	// 家具の日射吸収割合を加算
	if (fsolm != NULL)
	{
		//printf("Room=%s fsolm=%lf\n", Rmname, *fsolm);
		Srgchk += *fsolm;
		Sumsrg2 += *fsolm;
	}
	
	for ( n = 0; n < N; n++, Sd++ )
	{
		//printf ( "<Radshfc>  srg=%lf\n", Sd->srg ) ;
		Srgchk += Sd->srg ;
	}
	
	if (fabs( Srgchk - 1.0) > 1.0e-3)
	{
		printf ("xxxxx (%s)  室内部位への日射吸収比率の合計が不適 %.3lf (本来、1となるべき)\n",
			Rmname, Srgchk ) ;
	}

	if (tfsol > 1.)
	{
		printf("xxxxx (%s)  室内部位への日射吸収比率を指定したものだけで合計が不適 %.3lf (本来、1未満となるべき)\n",
			Rmname, tfsol);
	}

	 //最終日射吸収比率の計算（Sumsrg2で基準化）
	Sd = Sd0;
	for (n = 0; n < N; n++, Sd++)
		Sd->srg2 /= Sumsrg2;
	if (fsolm != NULL)
		Room->Srgm2 /= Sumsrg2;
}

