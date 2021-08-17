/*    bslib.c            */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

#include "common.h"
#include "fesy.h"
#include "esize.h"
#include "fnfio.h"
#include "fnlib.h"
#include "fnesy.h"

#define  ALO  23.0

//double max(double a, double b ) ;

/*  外表面方位デ－タの入力    */

void Exsfdata (FILE *fi, char *dsn, EXSFS *Exsf, SCHDL *Schdl, SIMCONTL *Simc )
{
	char	s[SCHAR], ename[SCHAR], *st ;
	double	dt, dfrg = 0.0 ;
	double	rad; 
	double	*vall ;
	int		i = -1, j, Nd, k ;
	double  wa, wb, swa, cwa, swb, cwb ;
	EXSF	*ex, *exj ;
//	EXSF	*e ;
	char	Err[SCHAR+128];
	
	strcpy_s ( s, sizeof(s), dsn ) ;
	Nd = imax ( ExsfCount ( fi ), 1 ) ;

	vall = Schdl->val ;

	/****************/
	if ( Nd > 0 )
	{
		sprintf_s(s, sizeof(s), "%lf", ALO);
		Exsf->alosch = envptr(s,Simc,0,NULL,NULL,NULL) ;
		Exsf->alotype = 'F' ;
		Exsf->Exs = ( EXSF * ) malloc (( Nd + 1 ) * sizeof ( EXSF )) ;

		exj = Exsf->Exs ;
		if ( exj == NULL )
			Ercalloc(Nd, "<Exsfdata> Exs" ) ;
		else
			Exsfinit ( Nd + 1, exj ) ;
	}
	/**************************/
	
//	ex = NULL ;
//	ex = *Exs ;
	
//	sprintf (E, ERRFMT, dsn);
	ex = Exsf->Exs - 1 ;
	
	while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != '*')
	{
		//printf ( "s=%s\n", s ) ;

		if (strncmp(s, "alo=", 4) == 0)
		{
			st = strchr(s, '=');
			if (strcmp(st + 1, "Calc") == 0)
				Exsf->alotype = 'V';
			else if ((k = idsch(st + 1, Schdl->Sch, NULL)) >= 0)
			{
				Exsf->alosch = &vall[k];
				Exsf->alotype = 'S';
			}
			else
			{
				Exsf->alosch = envptr(st + 1, Simc, 0, NULL, NULL, NULL);
				if (Exsf->alosch != NULL)
					Exsf->alotype = 'S';
			}
		}
		else if (strncmp(s, "r=", 2) == 0)
		{
			st = strchr(s, '=');
			dfrg = atof(st + 1);

			if (dfrg<0. || dfrg>1.0)
			{
				sprintf_s(Err, sizeof(Err), "%s の設置値が不適切です", s);
				preexit();
			}
		}
		else 
		{
			ex++ ;
			i++ ;
			
			//if ( i > EXSMX )
			//	printf ( "<EXSRF> xxx i=%d  max=%d xxx\n", i, EXSMX ) ;
			//if ( Exsfrealloc ( Exs, i, i+1 ) == 0 )
			//	printf("<Exsfdata> メモリの最確保に失敗しました。" );
			//			*Exs = Exsfrealloc ( *Exs, i, i+1 ) ;
			
			//ex = &Exs[i] ;
			
			//if (( st = strchr(s, '=')) != NULL )
			//{
			//	if (strncmp(s, "alo", 3) == 0)
			//	{
			//		if ( strcmp(st+1, "Calc") == 0 )
			//			Exsf->alotype = 'V' ;
			//		else if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
			//		{
			//			Exsf->alosch = &vall[k];
			//			Exsf->alotype = 'S' ;
			//		}
			//		else
			//		{
			//			Exsf->alosch = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
			//			if ( Exsf->alosch != NULL )
			//				Exsf->alotype = 'S' ;
			//		}
			//	}
			//}
			//else
			//{
				ex->name = stralloc( s);
				ex->alotype = Exsf->alotype;
				ex->alo = Exsf->alosch;
				
				// 水平面
				if (strcmp(s, "Hor") ==0)
					ex->Wb=0.0;
				// 地表面
				else if(strcmp(s, "EarthSf") == 0)
				{
					Exsf->EarthSrfFlg = 'Y' ;
					ex->typ = 'e' ;
				}
				else
					ex->Wb=90.0, ex->Rg=dfrg;
			//}
		}
		
		while (fscanf_s(fi, " %s ", s, sizeof(s)), s[0] != ';')
		{ 
			//printf ( "s=%s\n", s ) ;

			if ( strncmp ( s, "a=", 2 ) == 0 )
			{
				if (sscanf_s(&s[2], "%lf", &dt) != 0)
					ex->Wa =dt;
				else
				{
					if (strchr(s, '+'))
						Nd = sscanf_s(&s[2], "%[^+]%lf", ename, sizeof(ename), &dt);
					else if (strchr(s, '-'))
						Nd = sscanf_s(&s[2], "%[^-]%lf", ename, sizeof(ename), &dt); 
					else
						Nd = sscanf_s(&s[2], "%s", ename, sizeof(ename));
					
					exj = Exsf->Exs ;
					for (j=0; j<i+1; j++, exj++)
					{
						if ((strcmp(exj->name, ename)) == 0)
						{ 
							ex->Wa = exj->Wa + (Nd == 2 ? dt : 0.0);
							break;
						}
					}
					if (j == i+1)
						Eprint( "<Exsfdata>", s ) ;
				}
			}
			else
			{
				st = strchr(s,'=') ;
				// dt= atof(strchr(s,'=')+1);
				if (strncmp(s, "alo", 3) == 0)
				{
					//ex->alo = dt;
					if (strcmp(st+1, "Calc") == 0)
					{
						ex->alotype = 'V' ;
					}
					else if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
					{
						ex->alo = &vall[k];
						ex->alotype = 'S' ;
					}
					else
					{
						ex->alo = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
						ex->alotype = 'S' ;
					}
				}
				else
				{
					dt = atof(st+1) ;
					switch (s[0])
					{
					case 't': ex->Wb =dt;
						break;
					case 'r': ex->Rg =dt;
						break;
					case 'Z': ex->Z =dt;
						ex->typ = 'E';
						break;
					case 'd': ex->erdff =dt;
						break;
					default :
						Eprint ( "<Exsfdata>", s ) ;
						break;
					}
				}
			}
			//ex++ ;
			if (strchr(s, ';')) break;
		}
	}
	
	i++ ;
	Exsf->Nexs = i ;
	Exsf->Exs->end = i;
	//if (i > EXSMX) 
	//	printf("%s ename=%d [max=%d]\n", E, i, EXSMX);
	
	/************************************
	ex = Exsf->Exs ;
	for (i = 0; i < Exsf->Nexs; i++, ex++)
		printf ( "Name=%s Wa=%.0lf Wb=%.0lf alo=%.2lf Rg=%.2lf\n", ex->name,
		ex->Wa, ex->Wb, *ex->alo, ex->Rg ) ;
	/***********************************/
	
	rad = PI/180.;
	
	ex = Exsf->Exs ;
	for ( i = 0; i < Exsf->Nexs; i++, ex++ )
	{
		//printf ( "i=%d\n", i ) ;

		if (ex->typ == 'S')
		{
			wa = ex->Wa*rad;
			wb = ex->Wb*rad;
			ex->cwa = cwa=cos(wa);
			ex->swa = swa=sin(wa);
			ex->Wz = cwb = cos(wb);
			ex->swb = swb = sin(wb);
			ex->Ww = swb*swa;
			ex->Ws = swb*cwa;
			ex->cbsa = cwb*swa;
			ex->cbca = cwb*cwa;
			
			ex->Fs = 0.5*(1.0+cwb);
		}
	}
}
/* ---------------------------------------- */

/*  外表面入射日射量の計算    */


void Exsfsol(int Nexs, WDAT *Wd, EXSF *Exs)
{
	int		i ;
	double	cinc ;
	EXSF	*ex ;

	ex = Exs ;
	for ( i = 0; i < Nexs; i++, ex++ )
	{
		if ( ex->typ == 'S' )
		{
			cinc = Wd->Sh * ex->Wz + Wd->Sw * ex->Ww + Wd->Ss * ex->Ws;

			if (cinc > 0.0)
			{
				ex->tprof= (Wd->Sh * ex->swb - Wd->Sw * ex->cbsa
					- Wd->Ss * ex->cbca) / cinc;
				// プロファイル角の計算
				ex->Prof = atan(ex->tprof) ;
				ex->tazm= (Wd->Sw * ex->cwa - Wd->Ss * ex->swa)/cinc;
				// 見かけの方位角の計算
				ex->Gamma = atan(ex->tazm) ;
				ex->cinc = cinc;
			}
			else
			{
				ex->Prof = ex->Gamma = 0.0 ;
				ex->cinc = cinc = 0.0;
			}
			
			ex->Idre = Wd->Idn * cinc;				// 外表面入射（直達）
			ex->Idf = Wd->Isky * ex->Fs 
				+ ex->Rg * Wd->Ihor * (1.0-ex->Fs); 
			ex->Iw = ex->Idre + ex->Idf;
			ex->rn = Wd->RN * ex->Fs;
		}
	}
}
/* ------------------------------------------------------------- */

/*  ガラス日射熱取得の計算         */


void Glasstga (double Ag, double tgtn, double Bn, double cinc, double Fsdw, 
			   double Idr, double Idf,  double *Qgt, double *Qga, char *Cidtype, double Profile, double Gamma)
{ 
	double    Cid, Cidf=0.01;
	double	  Bid, Bidf = 0.0 ;
	double    Qt, Qb ;
	
	Cid = Bid = 0.0 ;
	Qt = Qb = 0.0 ;
	
	// 標準
	if(strcmp(Cidtype,"N") == 0)
	{
		//printf("%lf\t", cinc);
		Cid = Glscid(cinc);
		Cidf = 0.91 ;

		Bid = Cid ;
		Bidf = Cidf ;
	}
	else
	{
		printf("xxxxx <eebslib.c  CidType=%s\n", Cidtype) ;
	}

	//if(cinc>0. && (Cid <= 0. || Cid > 1.0))
	//	printf("xxxxxxx Cid=%.3lf\n", Cid) ;
	//if(Cidf <= 0. || Cidf > 1.0)
	//	printf("xxxxxxx Cidf=%.3lf\n", Cidf) ;

	//if(cinc>0. && (Bid <= 0. || Bid > 2.5))
	//	printf("xxxxxxx Bid=%.3lf\n", Bid) ;

	// 透過日射量の計算
	Qt = Ag*(Cid*Idr*(1.0-Fsdw) + Cidf*Idf);
	// 吸収日射量の計算
	Qb = Ag*(Bid*Idr*(1.0-Fsdw) + Bidf*Idf);
	
	*Qgt = Qt * tgtn ;
	*Qga = Qb * Bn ;

	//printf("%lf\t%lf\n", Ag*(Cid*Idr*(1.0 - Fsdw)) * tgtn, Ag*(Cidf*Idf) * tgtn);
}
/* ------------------------------------------------------ */

/*  ガラスの直達日射透過率標準特性　　　　*/

double Glscid(double cinc)
{
	//double max(double a, double b) ;
    return(dmax(0, cinc*(3.4167+cinc*(-4.389+cinc*(2.4948-0.5224*cinc)))) );
}
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

/*  ガラスの直達日射透過率標準特性　　　　*/
// 普通複層ガラス

double GlscidDG(double cinc)
{
	//double max(double a, double b) ;
    return(dmax(0, cinc*(0.341819+cinc*(6.070709+cinc*(-9.899236+4.495774*cinc)))) );
}

int	Exsfrealloc ( EXSF **E, unsigned int N, unsigned int NN )
{
	//	int		c ;
	unsigned int	i ;
	EXSF	*B, *Buf ;
	int		c ;
	
	Buf = B = NULL ;
	c = 1 ;
	i = NN - N ;
	//	if ( N > 0 )
	//		Buf = ( EXSF * ) realloc ( *E, ( size_t) NN * sizeof ( EXSF )) ;
	//	else
	Buf = ( EXSF * ) malloc ( i * sizeof ( EXSF )) ;
	
	if ( Buf == NULL )
	{
		c = 0 ;
		//		c = NULL ;
	}
	else
	{
		/**************/
		//		B = Buf + N ;
		B = Buf ;
		
		for ( i = N; i < NN; i++, B++ )
		{
			B->name = NULL ;
			B->typ = ' ' ;
			B->Wa = B->Wb = 0.0 ;
			B->Rg = B->Fs = B->Wz = B->Ww = B->Ws = 0.0 ;
			B->swb = B->cbsa = B->cbca = B->cwa = 0.0 ;
			B->swa = B->Z = B->erdff = B->cinc =0.0 ;
			B->tazm = B->tprof = B->Idre = B->Idf = 0.0 ;
			B->Iw = B->rn = B->Tearth = 0.0 ;
			B->end = 0 ;
			B->alo = NULL;
		}
		/********************************/
		
		//		*E = &Buf[0] ;
		
		for ( i = 0; i < NN - N; i++ )
			E[i+N] = &Buf[i] ;
	}
	return c ;
}

int	ExsfCount ( FILE *fi )
{
	int		N = 0 ;
	long	ad ;
	char	s[SCHAR] ;
	
	ad = ftell ( fi ) ;
	
	while ( fscanf_s ( fi, " %s ", s, sizeof(s) ), s[0] != '*' )
	{
		/***********************
		if ( strncmp ( s, "r=", 2 ) == 0 )
		{
			fscanf ( fi, "%*[^;] " ) ;
			fscanf ( fi, " %*s " ) ;
		}
		else ****************/
		if ( strchr ( s, ';' ))
			N++ ;
	}
	
	fseek ( fi, ad, SEEK_SET ) ;
	return ( N ) ;
}

