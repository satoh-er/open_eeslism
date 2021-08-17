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

/*  mcomvav.c  */

/*  OM用変風量コントローラ */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"
#include "fnesy.h"
#include "winerror.h"
#include "fnbldg.h"

/* ------------------------------------------ */

/* 機器仕様入力　　　　　　*/

/*---- Satoh OMVAV  2010/12/16 ----*/
int OMVAVdata (char *s, OMVAVCA *OMvavca)
{
	char *st;
	double dt;
	int   id=0;
	
	if ((st=strchr(s,'=')) == 0)
	{
		OMvavca->name = stralloc(s);
		OMvavca->Gmax = OMvavca->Gmin = -999.0 ;
	}
	else
	{ 
		*st = '\0';
		dt = atof(st+1) ;
		
		if ( strcmp(s, "Gmax") == 0)
			OMvavca->Gmax = dt;
		else if (strcmp(s, "Gmin") == 0)
			OMvavca->Gmin = dt ;
		else
			id=1;
    }
	return (id);
}

double	CollTout(double Tcin, double G, RMSRF *Sd)
{
	extern double	ca ;
	WALL	*Wall ;
	double	Kc ;

	Wall = Sd->mw->wall ;
	if(Wall->chrRinput == 'Y')
		Kc = Sd->dblKc ;
	else
		Kc = Wall->Kc ;

	//printf("name=%s Kc=%.2lf Tcole=%.2lf\n", Sd->name, Kc, Sd->Tcole ) ;
	return (Sd->Tcole - (Sd->Tcole - Tcin) * exp(- Kc * Sd->A / ( ca * G ))) ;
}

double	OMflowcalc(OMVAV *OMvav, WDAT *Wd)
{
	double	G, G0, dG, EPS = 0.00001, G2, dGp = 0.01 ;
	RMSRF	*omwall ;
	extern double	ca ;
	WALL	*Wall ;
	//double	Temp ;
	double	Tcin, Tcout, Tcoutset, Tcoutmin, Tcoutmax ;
	//OMVAV	*OMvav ;
	double	CollTout(double Tcin, double G, RMSRF *Sd) ;
	double	FG ;
	int		i, loop, LoopMax = 100 ;
	RMSRF	*Sd ;

	G = 0.0 ;

	if ( OMvav->Plist->control != OFF_SW )
	{
		omwall = OMvav->omwall ;
		Wall = omwall->mw->wall ;
		Tcin = Wd->T ;
		Tcoutset = omwall->rpnl->Toset ;
		// G0 = (OMvav->cat->Gmin + OMvav->cat->Gmax) / 2. ;
		// dG = OMvav->cat->Gmax * 0.0001 ;
		G0 = OMvav->cat->Gmin ;
		dG = OMvav->cat->Gmin * 0.001 ;
		G2 = OMvav->cat->Gmax ;

		/********************************************************/
		// 棟温度の計算（最小風量の場合）
		Tcin = Wd->T ;
		for ( i = 0; i < OMvav->Nrdpnl; i++ )
		{
			Sd = OMvav->rdpnl[i]->sd[0] ;
			Tcout = CollTout(Tcin, G0, Sd ) ;
				
			// 集熱器の入り口温度は上流集熱器の出口温度
			Tcin = Tcout ;
		}
		Tcoutmin = Tcout ;

		// 棟温度の計算（最大風量の場合）
		Tcin = Wd->T ;
		for ( i = 0; i < OMvav->Nrdpnl; i++ )
		{
			Sd = OMvav->rdpnl[i]->sd[0] ;
			Tcout = CollTout(Tcin, G2, Sd ) ;
				
			// 集熱器の入り口温度は上流集熱器の出口温度
			Tcin = Tcout ;
		}
		Tcoutmax = Tcout ;

		//printf("Tcoutmin=%.2lf Tcoutmax=%.2lf\n", Tcoutmin, Tcoutmax) ;
		if ( Tcoutmin < Tcoutset )
			G = G0 ;
		else if  ( Tcoutmax > Tcoutset )
			G = G2 ;
		else
		{
			//G0 = G2 ;
			//G0 = (G0 + G2) / 2. ;
			// ニュートンラプソン法のループ
			for ( loop = 0; loop < LoopMax; loop++)
			{
				// 棟温度の計算
				Tcin = Wd->T ;
				G = G0 + (double)loop * dGp * ( G2 - G0) ;
				for ( i = 0; i < OMvav->Nrdpnl; i++ )
				{
					Sd = OMvav->rdpnl[i]->sd[0] ;
					Tcout = CollTout(Tcin, G, Sd ) ;
				
					// 集熱器の入り口温度は上流集熱器の出口温度
					Tcin = Tcout ;
				}

				FG = Tcoutset - Tcout ;

				if ( FG > 0.)
					break ;
				//printf("Tcout1=%lf  ", Tcout) ;
			
				//// 棟温度の計算（微分項）
				//Tcin = Wd->T ;
				//for ( i = 0; i < OMvav->Nrdpnl; i++ )
				//{
				//	Sd = OMvav->rdpnl[i]->sd[0] ;
				//	Tcout = CollTout(Tcin, G0 + dG, Sd ) ;
				//
				//	// 集熱器の入り口温度は上流集熱器の出口温度
				//	Tcin = Tcout ;
				//}

				//FGp = Tcoutset - Tcout ;
				////printf("Tcout2=%lf\n", Tcout) ;
				//FGd = (FGp - FG) / dG ;

				//G = G0 - FG / FGd ;
				////printf("%d  G=%lf G0=%lf\n", loop, G, G0) ;
				// 収束判定
				//if(fabs(G - G0) < EPS)
				//	break ;
				//else
				//	G0 = G ;
			}			

			//printf("Tcout=%.2lf\n", Tcout) ;

			if(loop == LoopMax)
				printf("%s  風量が収束しませんでした。 G=%lf\n", OMvav->name, G) ;
		}
		/**********************************************/

		/*********************
		Tcout = omwall->rpnl->Toset ;
		if (OMvav->rdpnl[0] == NULL)
		{
			Te = omwall->Tcole ;
			Temp = ( Te - Tcout ) / ( Te - Tcin ) ;
			Kc = Wall->Kc ;
			Ac = omwall->A ;
		}
		else
		{
			//double	*A, *Kc ;
			double	TeA ;
			RMSRF	*Sd ;
			//double	A1, A2 ;
			//double	Kc1, Kc2 ;
			//RMSRF	*Sd1, *Sd2 ;
			int		i ;
			double	Ksu, AcKsu ;

			AcKsu = 0.0 ;
			Ac = 0. ;
			TeA = 0.0 ;
			Kc = 0.0 ;
			for ( i = 0; i < OMvav->Nrdpnl; i++, Sd++ )
			{
				double Kcdd ;
				WALL	*Wall ;

				Sd = OMvav->rdpnl[i]->sd[0] ;
				
				Wall = Sd->mw->wall ;
				if(Wall->chrRinput == 'Y')
				{
					Kcdd = Sd->dblKc ;
					Ksu = Sd->dblKsu ;
				}
				else
				{
					Kcdd = Wall->Kc ;
					Ksu = Wall->Ksu ;
				}

				Ac += Sd->A ;
				AcKsu += (Sd->A * Ksu) ;
				Kc += Sd->A * Kcdd ;
				TeA += (Sd->Tcole * Sd->A * Ksu) ;
			}
			//Sd1 = OMvav->rdpnl[0]->sd[0] ;
			//Sd2 = OMvav->rdpnl[1]->sd[0] ;
			//A1 = Sd1->A ;
			//A2 = Sd2->A ;
			//Ac = A1 + A2 ;
			//Kc1 = Sd1->mw->wall->Kc  ;
			//Kc2 = Sd2->mw->wall->Kc  ;
			//Kc = ( Kc1 * A1 + Kc2 * A2 ) / Ac ;
			Kc /= Ac ;
			TeA /= AcKsu ;

			//Te = ( Sd1->ColTe * A1 + Sd2->ColTe * A2 ) / Ac ;
			Temp = ( TeA - Tcout ) / ( TeA - Tcin ) ;
		}
		G = -999.0 ;
		if ( Temp > 0. )
			G = - Kc * Ac / ca / log (Temp) ;
			/**************************************/
		G = dmin(dmax(OMvav->cat->Gmin, G), OMvav->cat->Gmax) ;
	}

	OMvav->G = G ;

	//printf ("%c G=%lf\n", OMvav->Plist->control != OFF_SW, G ) ;
	return ( G ) ;
}

// 制御対象の集熱器の名前を取得する

int colkey(char *s, char **key)
{
	char *st;
	char ss[SCHAR];
	int  i, nk;
	
	if ((i = (int)strlen(s)) == 0)
		return 0;
	else
	{  
		strcpy_s(ss, sizeof(ss), s); 
		
		key[0] = ss;
		nk = 1;
		for (st = ss + 1; st <ss + i; st++)
		{	 
			if (*st == '-')
			{
				*st = '\0';
				st++ ;
				key[nk] = st;
				nk++ ;
			}
		}
		/************* 
		printf("   strkey  nk=%d", nk);
		for (i = 0; i < nk; i++)
		printf(" key[%d]=%s",i, key[i]);
		printf("\n");
		****************/
		
		return nk;
	}
}

void	OMvavControl(OMVAV *OMvav, COMPNT *Compnt, int NCompnt)
{
	char	*st ;
	//char	col1[SCHAR], col2[SCHAR] ;
	char	*colname[SCHAR] ;
	int		i, j ;
	COMPNT	*cmp ;
	int		Ncomp ;

	st = OMvav->cmp->omparm ;
	//Ncomp = strCompcount(st, '-' ) + 1 ;
	//for ( i = 0; i < Ncomp; i++ )
	//	sscanf(st, "%[^-]- ", colname[i]) ;
	Ncomp = colkey ( st, colname ) ;
	OMvav->Nrdpnl = Ncomp ;

	for ( j = 0; j < Ncomp; j++ )
	{
		cmp = Compnt ;
		for (i = 0; i < NCompnt; i++, cmp++)
		{
			if(strcmp(colname[j], cmp->name) ==0)
			{
				OMvav->rdpnl[j] = cmp->eqp ;
				break ;
			}
		}
	}

	//cmp = Compnt ;
	//for (i = 0; i < NCompnt; i++, cmp++)
	//{
	//	if(strcmp(col2, cmp->name) == 0)
	//		OMvav->rdpnl[1] = cmp->eqp ;
	//}
}

int		strCompcount(char *st, char key)
{
	int i, count ;
	count = 0 ;

	for ( i = 0; i < (int)strlen(st); i++, st++ )
	{
		if ( *st == key ) count++ ;
	}

	return ( count ) ;
}
