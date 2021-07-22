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

/*  VAVコントローラ */

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
#include "fnbldg.h"

/* ------------------------------------------ */

/* 機器仕様入力　　　　　　*/

/*---- Satoh Debug VAV  2000/10/30 ----*/
int VAVdata (char *cattype, char *s, VAVCA *vavca)
{
	char *st;
	double dt;
	int   id=0;
	
	if (strcmp(cattype, VAV_TYPE) == 0)
		vavca->type = VAV_PDT ;
	else if(strcmp(cattype, VWV_TYPE) == 0)
		vavca->type = VWV_PDT ;

	if ((st=strchr(s,'=')) == 0)
	{
		vavca->name = stralloc(s);
		vavca->Gmax = vavca->Gmin = vavca->dTset = -999.0 ;
	}
	else
	{ 
		*st = '\0';
		dt = atof(st+1) ;
		
		if ( strcmp(s, "Gmax") == 0)
			vavca->Gmax = dt;
		else if (strcmp(s, "Gmin") == 0)
			vavca->Gmin = dt ;
		else if (strcmp(s, "dTset") == 0)
			vavca->dTset = dt ;
		else
			id=1;
    }
	return (id);
}

void VWVint(int Nvav, VAV *VAV, int Ncompnt, COMPNT *Compn)
{
	int i;
	char	s[SCHAR] ;
	
	for (i = 0; i < Nvav; i++, VAV++)
	{
		VAV->hcc = NULL ;
		VAV->hcld = NULL ;
		VAV->mon = '-' ;
		if ( VAV->cat->type == VWV_PDT )
		{
			if ( VAV->cmp->hccname != NULL )
				VAV->hcc = (HCC *) hccptr( 'c', VAV->cmp->hccname, Ncompnt, Compn, &VAV->mon ) ;
			else if ( VAV->cmp->rdpnlname != NULL )
			{
				VAV->rdpnl = (RDPNL *) rdpnlptr( VAV->cmp->rdpnlname, Ncompnt, Compn ) ;
				if (VAV->rdpnl != NULL)
					VAV->mon = 'f' ;
			}
			
			if ( VAV->mon == '-' )
				VAV->hcld = (HCLOAD *) hccptr( 'h', VAV->cmp->hccname, Ncompnt, Compn, &VAV->mon ) ;

			if ( VAV->mon == '-' )
			{
				sprintf ( s, "VWV(%s)=%s", VAV->name, VAV->cmp->hccname ) ;
				printf ( "xxxxxxxxx %s xxxxxxxxxxx\n", s ) ;
			}
		}
	}
}

/*  特性式の係数  */
/*---- Satoh Debug VAV  2000/11/8 ----*/
/*********************/
void VAVcfv(int Nvav, VAV *vav)
{
	ELOUT	*Eo;
	double	*cfin;
	int		i;
	char	Err[SCHAR] ;
	extern int	VAV_Count_MAX ;
	
	for (i = 0; i < Nvav; i++, vav++)
	{
		Eo = vav->cmp->elouts;
		
		if (vav->cmp->control != OFF_SW && Eo->control != OFF_SW )
		{
			if ( vav->cat->Gmax < 0.0 )
			{
				sprintf ( Err, "Name=%s  Gmax=%.5g", vav->name, vav->cat->Gmax ) ;
				Eprint ( "VAVcfv", Err ) ;
			}
			if ( vav->cat->Gmin < 0.0 )
			{
				sprintf ( Err, "Name=%s  Gmin=%.5g", vav->name, vav->cat->Gmin ) ;
				Eprint ( "VAVcfv", Err ) ;
			}

			//if (vav->count < VAV_Count_MAX - 1)
			if (vav->count == 0)
			{ 
				vav->G = Eo->G;
				vav->cG = spcheat(Eo->fluid) * vav->G;
				
				Eo->coeffo = vav->cG;
				Eo->Co = 0.0;
				cfin = Eo->coeffin;
				*cfin  =  -vav->cG;
			}
			else
			{
				Eo->coeffo = 1.0 ;
				Eo->Co = 0.0 ;
				*(Eo->coeffin) = -1.0 ;
			}
			
			if ( vav->cat->type == VAV_PDT )
			{
				Eo++ ;
				Eo->coeffo = 1.0 ;
				Eo->Co = 0.0 ;
				*(Eo->coeffin) = -1.0 ;
			}
		}
	}
}

/************************:/
/* ------------------------------------------ */

/* VAVコントローラ再熱部分の計算 */ 
/*---- Satoh Debug VAV  2000/11/27 ----*/
/*******************/
void VAVene(int Nvav, VAV *vav, int *VAVrest)
{
	int i, rest;
	ELOUT *elo;
	double	Tr, Go, dTset ;
	extern int	VAV_Count_MAX ;
	
	for (i = 0; i < Nvav; i++, vav++)
	{
		rest = 0 ;
		
		elo = vav->cmp->elouts;
		vav->Tin = elo->elins->sysvin;
		
		if (vav->cmp->control != OFF_SW && elo->control != OFF_SW )
		{
			//elo = vav->cmp->elouts;
			
			Go = vav->G ;
			vav->Tout = elo->sysv ;

			if (vav->cat->type == VAV_PDT)
			{
				Tr = vav->cmp->elouts->emonitr->sysv ;

				vav->Q = spcheat ( elo->fluid )
					* Go * ( vav->Tout - Tr ) ;
				
				if (fabs(vav->Tin - Tr) > 1.e-3)
					vav->G = (vav->Tout - Tr) / (vav->Tin - Tr) * Go;
				else
					vav->G = vav->cat->Gmin;

				//printf("name=%s Tr=%.2lf Q=%.0lf G=%.5lf Go=%.5lf Tin=%.2lf Tout=%.2lf\n", vav->name, Tr, vav->Q, vav->G, Go, vav->Tin, vav->Tout);
			}
			else
			{
				/***** 未完成 *****/
				//printf("本バージョンではまだサポートされていません。\n");
				//exit (EXIT_NONSPT) ;

				if ( vav->mon == 'c' && vav->count < VAV_Count_MAX - 1 )
					vav->Qrld = - vav->hcc->Qt ;
				else if ( vav->mon == 'f' && vav->count < VAV_Count_MAX - 1 )
					vav->Qrld = - vav->rdpnl->Q ;
				else if ( vav->mon == 'h' )
					vav->Qrld = vav->hcld->Qt ;

				vav->Q = vav->Qrld ;
				
				dTset = vav->cat->dTset ;

				if ( vav->mon == 'h' && dTset <= 0.0 )
				{
					printf ("<VAVene> VWV SetDifferencialTemp.=%.1lf\n",
						vav->cat->dTset ) ;
				}

				if ( vav->chmode == COOLING_SW )
					dTset = - dTset ;

				if ( vav->mon == 'h' || vav->mon == 'f' )
				{
					vav->G = vav->Q / ( spcheat ( elo->fluid ) * dTset ) ;
				}
				else if ( vav->mon == 'c' )
					vav->G = FNVWVG ( vav ) ;
				//printf ( "Name=%s  流量計算結果 G=%lf\n", vav->name, vav->G ) ;
				//printf("Tin=%.1lf Tout=%.1lf Q=%.0lf Go=%.3lf G=%.3lf Tr=%.1lf\n",
				//	vav->Tin, vav->Tout, vav->Q, Go, vav->G, Tr ) ;
			}

			/*****************************************
			printf ("<VAVene> name=%s Q=%.0lf Tin=%.1lf Tout=%.1lf Go=%.2lf G=%.2lf Tr=%.1lf\n",
				vav->name, vav->Q, vav->Tin, vav->Tout, Go, vav->G, Tr) ;
			/*********************************/

			elo->control = ON_SW;
			elo->sysld = 'n';
			
			if ( vav->mon != 'h' )
				elo->emonitr->control = ON_SW;
			
			rest = chvavswreset(vav->Q, vav->chmode, vav) ;
			
			/*****************************
			printf ("<VAVene> 修正流量 G=%.2lf\n", vav->G ) ;
			/********************************/

			/********************
			printf ("name=%s reset=%d G=%lf Go=%lf\n",
					vav->name, rest, vav->G, Go ) ;
					/*************************/

			if ( rest == 1 || fabs ( vav->G - Go ) > 1.0e-5 )
			{
				//printf ("VAV 再計算!!\n" ) ;

				(*VAVrest)++ ;
			}
		}
		else
		{
			vav->Q = 0.0;
			vav->G = vav->cat->Gmin ;

			if ( vav->count == 0 )
				(*VAVrest)++ ;
		}
	}
}

void	VAVcountreset ( int Nvav, VAV *VAV )
{
	int	i ;

	for ( i = 0; i < Nvav; i++, VAV++ )
		VAV->count = 0 ;
}

void	VAVcountinc ( int Nvav, VAV *VAV )
{
	int	i ;

	for ( i = 0; i < Nvav; i++, VAV++ )
		VAV->count++ ;
}

int		vavswptr(char **key, VAV *VAV, VPTR *vptr)
{
	int	err = 0 ;

	if ( strcmp (key[1], "chmode" ) == 0 )
	{
		vptr->ptr = &VAV->chmode ;
		vptr->type = SW_CTYPE ;
	}
	else if ( strcmp (key[1], "control" ) == 0 )
	{
		vptr->ptr = &VAV->cmp->elouts->control ;
		vptr->type = SW_CTYPE ;
	}
	else
		err = 1 ;
	
	return err ;
}

int chvavswreset(double Qload, char chmode, VAV *vav)
{
	if ((chmode == HEATING_SW && Qload < 0.0) ||
		(chmode == COOLING_SW && Qload > 0.0))
	{
		vav->G = vav->cat->Gmin ;
		return 1;
	}
	else
	{
		if ( vav->G < vav->cat->Gmin )
		{
			vav->G = vav->cat->Gmin ;
			return 1 ;
		}
		else if ( vav->G > vav->cat->Gmax )
		{
			vav->G = vav->cat->Gmax ;
			return 1 ;
		}
		else
			return 0;
	}
}

void vavprint(FILE *fo, int id, int Nvav, VAV *VAV)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nvav > 0)
            fprintf(fo, "%s %d\n", VAV_TYPE, Nvav);
		for (i = 0; i < Nvav; i++, VAV++)
			fprintf(fo, " %s 1 2\n", VAV->name);
		break;
		
	case 1:
		for (i = 0; i < Nvav; i++, VAV++)
		{	 
			fprintf(fo,"%s_c c c %s_G m f\n", VAV->name, VAV->name);
		}
		break;
		
	default:
		for (i = 0; i < Nvav; i++, VAV++)
            fprintf(fo, "%c %6.4g\n",
   	        VAV->cmp->elouts->control, VAV->cmp->elouts->G);
		break;
	}
}

/* VWVシステムの流量計算 */
double	FNVWVG ( VAV *VWV )
{
	double	Wa, Wwd, Tain, Twin, Q, KA, et, F, emax, emin, Gwd ;
	double	A, B ;
	extern double	cw ;
	HCC		*h ;
	int		i ;
	
	h = VWV->hcc ;
	Wa =h->cGa ;
	Q = VWV->Q ;
	KA = h->cat->KA ;
	Tain = h->Tain ;
	//Tain = VWV->cmp->elouts->emonitr->sysv ;
	Twin = VWV->Tin ;
	//Wwd = - Q / ( cw * VWV->cat->dTset ) ;
	//Wwd = cw * ( VWV->cat->Gmax + VWV->cat->Gmin ) / 2.0 ;
	//Gwd = VWV->cat->Gmin ;
	A = VWV->cat->Gmin ;
	B = VWV->cat->Gmax ;
	Gwd = ( A + B ) / 2.0 ;
	Wwd = Gwd * cw ;
	
	et = - Q / ( Wa * ( Tain - Twin )) ;
	emin = FNhccet ( Wa, cw * A, KA ) ;
	emax = FNhccet ( Wa, cw * B, KA ) ;
	
	//printf ( "必要なコイル温度効率は et=%lf\n", et ) ;
	
	/***************
	if ( et > 1.0 || et < 0.0 )
		printf ( "xxxxxx FNVWVG 必要コイル温度効率が不適当 et=%lf\n", et ) ;
		*****************************/
	
	if ( et > emax )
		return ( VWV->cat->Gmax ) ;
	else if ( et < emin )
		return ( VWV->cat->Gmin ) ;

	for ( i = 0; i < 30; i++ )
	{
		Wwd = Gwd * cw ;
		/*******************/
		F = FNhccet ( Wa, Wwd, KA ) - et ;
		//Fd = ( FNhccet ( Wa, Wwd + 1.0, KA ) - et - F ) / ( 1.0 ) ;
		//Fd = FNFd ( Wa, Wwd, KA ) ;
		//Gw = Gwd - F / Fd ;
		
		if ( fabs ( F ) < 1.0e-5 )
		{
			//printf ( "収束計算後の温度効率 et=%lf\n",
			//	FNhccet ( Wa, cw * Gwd, KA )) ;
			return ( Gwd ) ;
		}
		else if ( F > 0.0 )
			B = Gwd ;
		else if ( F < 0.0 )
			A = Gwd ;

		Gwd = ( A + B ) / 2.0 ;

		//printf ( "i=%d <%s> et=%lf  etd=%lf\n", i, VWV->name, et, F + et ) ;

		/**************************
		if ( fabs ( Gw - Gwd ) < 1.0e-5 )
		{
			printf ( "収束計算後の温度効率 et=%lf\n",
				FNhccet ( Wa, cw * Gw, KA )) ;
			return ( Gwd ) ;
		}
		else
			Gwd = Gw ;
		/********************/
	}
	
	printf ( "xxxxxx FNVWVG  収束せず\n" ) ;
	return ( Gwd ) ;
}

double FNFd ( double Wa, double Ww, double KA )
{
	double	exB, ex2B, d, n, Ww2, B ;

	B = ( 1.0 - Wa / Ww ) * KA / Wa ;
	exB = exp ( - B ) ;
	ex2B = exp ( - 2.0 * B ) ;
	Ww2 = pow ( Ww, 2.0 ) ;

	d = Ww * pow ( Ww - Wa * exB, 2.0 ) ;
	n = ( Ww2 + Ww + KA ) * ex2B - ( Ww * ( Ww2 + Wa ) - Wa * KA ) * exB ;

	return ( n / d ) ;
}
