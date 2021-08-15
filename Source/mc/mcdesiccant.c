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

/*  mcdessicant.c  */
/*  バッチ式デシカント空調機 */

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

/* ------------------------------------------ */

// 要素方程式の変数のためのメモリの割り当て
void Desielm(int Ndesi, DESI *Desi)
{
	int i;
	ELOUT  *Eot, *Eox;
	ELIN   *elin;
	
	for (i = 0; i < Ndesi; i++, Desi++)
	{
		// 空気温度出口
		Eot = Desi->cmp->elouts;
		// 空気湿度出口
		Eox = Eot + 1;
		// 出口空気温度の要素方程式の2つめの変数は絶対湿度
		elin = Eot->elins + 1 ;
		elin->upo = elin->upv = Eox->elins->upo ;
		// 出口絶対湿度の要素方程式の2つめの変数は空気温度
		elin = Eox->elins + 1 ;
		// 空気温度の要素方程式の2つ目の変数（空気入口温度）のupo、upvに空気湿度をつなげる
		elin->upo = elin->upv = Eot->elins->upo ;
	}
}


/* 機器仕様入力　　　　　　*/

/*---- Satoh追加 2013/10/20 ----*/
int Desiccantdata(char *s, DESICA *Desica)
{
	char  *st;
	double dt;
	int   id=0;
	
	if ((st=strchr(s,'=')) == 0)
	{
		Desica->name = stralloc(s);
		Desica->r = -999. ;
		Desica->rows = -999. ;
		Desica->Uad = -999. ;
		Desica->A = -999.0 ;
		Desica->Vm = 18. ;
		Desica->eps = 0.4764 ;
		Desica->P0 = 0.4 ;
		Desica->kp = 0.0012 ;
		Desica->cps = 710. ;
		Desica->ms = -999. ;
	}
	else
	{ 
		*st = '\0';
		
        dt=atof(st+1);
		
        if (strcmp(s,"Uad") == 0)			// シリカゲル槽壁面の熱貫流率[W/m2K]
			Desica->Uad = dt;
		else if (strcmp(s, "A") == 0)		// シリカゲル槽表面積[m2]
			Desica->A = dt ;
		else if (strcmp(s, "ms") == 0)		// シリカゲル質量[g]
			Desica->ms = dt ;
		else if (strcmp(s, "r") == 0)		// シリカゲル平均直径[cm]
			Desica->r = dt ;
		else if (strcmp(s, "rows") == 0)	// シリカゲル充填密度[g/cm3]
			Desica->rows = dt ;
        else 
			id=1;      
	}
	return (id);
}
/* --------------------------- */

/*  管長・ダクト長、周囲温度設定 */

void Desiint(int NDesi, DESI *Desi, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd)
{
	int		i;
	char	Err[SCHAR] ;
	DESICA	*Desica ;
	
	for (i = 0; i < NDesi; i++, Desi++)
	{
	/******************
	printf ( "envnasme=%s\n", Pipe->cmp->envname ) ;
		************************/
		
		if ( Desi->cmp->envname != NULL )
			Desi->Tenv = envptr(Desi->cmp->envname, Simc, Ncompnt, Compnt, Wd, NULL);
		else
		{
			Desi->room = roomptr(Desi->cmp->roomname, Ncompnt, Compnt ) ;
			/**********************
			printf ( "Roomname=%s\n", Pipe->room->name) ;
			**************************/
		}
		
		Desica = Desi->cat ;

		if ( Desica->Uad < 0.0 )
		{
			sprintf ( Err, "Name=%s  Uad=%.4g", Desica->name, Desica->Uad ) ;
			Eprint ( "Desiint", Err ) ;
		}
		if ( Desica->A < 0.0 )
		{
			sprintf ( Err, "Name=%s  A=%.4g", Desica->name, Desica->A ) ;
			Eprint ( "Desiint", Err ) ;
		}
		if ( Desica->r < 0.0 )
		{
			sprintf ( Err, "Name=%s  r=%.4g", Desica->name, Desica->r ) ;
			Eprint ( "Desiint", Err ) ;
		}
		if ( Desica->rows < 0.0 )
		{
			sprintf ( Err, "Name=%s  rows=%.4g", Desica->name, Desica->rows ) ;
			Eprint ( "Desiint", Err ) ;
		}
		if ( Desica->ms < 0.0 )
		{
			sprintf ( Err, "Name=%s  ms=%.4g", Desica->name, Desica->ms ) ;
			Eprint ( "Desiint", Err ) ;
		}

		// 初期温度、出入口温度の初期化
		Desi->Tsold = 20.0 ;
		Desi->Xsold = FNXtr(Desi->Tsold, 50.) ;
		Desi->Tain = Desi->Taout = Desi->Tsold ;
		Desi->xain = Desi->xaout = Desi->Xsold ;

		// デシカント槽熱損失係数の計算
		Desi->UA = Desica->Uad * Desica->A ;

		// 吸湿量の初期化
		Desi->Pold = Desica->P0 ;

		// シリカゲルと槽内空気の熱伝達面積[m2]
		Desi->Asa = 3. * Desica->ms * 1000. * (1. - Desica->eps)/ (1.e4 * (Desica->r / 10.) * Desica->rows) ;

		// 逆行列
		Desi->UX = dcalloc(5*5, "<Desiint UX>") ;
		Desi->UXC = dcalloc(5, "<Desiint UXC>") ;
	}
}
/* --------------------------- */

/*  特性式の係数  */

void Desicfv(int NDesi, DESI *Desi)
{
	ELOUT *Eo;
	extern double dTM, ca, ro, roa ;
	double	h, i, j ;
	double	Te, hsa, hsad, hAsa, hdAsa;
	//double	A1, A2, A3, A4, A5, A6, B1, B2, B3, B4, B5, B6 ;
	double	*cfin ;
	int inti, N, N2, ii, jj;
	DESICA	*Desica ;
	double	*U, *C, *Cmat ;
	
	N = 5 ;
	N2 = N * N ;
	for (inti = 0; inti < NDesi; inti++, Desi++)
	{
		Desica = Desi->cat ;

		// 係数行列のメモリ確保
		U = dcalloc(N2, "<Desicfv U>") ;
		// 定数行列のメモリ確保
		C = dcalloc(N, "<Desicfv C>") ;

		if ( Desi->cmp->envname != NULL )
			Te = *(Desi->Tenv) ;
		else
			Te = Desi->room->Tot ;
		
		/*******
		printf("<<Stheatcfv>> i=%d %s  Tenv=%lf\n",
			i, stheat->name, Te);
		/************/
		
		Eo = Desi->cmp->elouts;
		// 熱容量流量の計算
		Desi->cG = spcheat(Eo->fluid) * Eo->G ;

		// シリカゲルと槽内空気の対流熱伝達率の計算
		if(Eo->cmp->control == OFF_SW)
			hsa = 4.614 ;
		else
			hsa = 40. ;

		// シリカゲルと槽内空気の湿気伝達率の計算
		hsad = hsa / ca ;

		hAsa = hsa * Desi->Asa ;
		hdAsa = hsad * Desi->Asa ;

		//D = Desica->ms * Desica->cps + hAsa * dTM ;
		//// 係数の計算
		//a = hAsa * dTM / D ;
		//b = hdAsa * ro * dTM / D ;
		//c = Desica->ms * Desica->cps / D ;
		//Desi->d = d = Eo->G * ca / hAsa ;
		//Desi->e = e = Desi->UA / hAsa ;
		//Desi->f = f = Eo->G / hdAsa ;
		//Desi->g = g = hdAsa * dTM / Desica->ms ;

		if(Desi->Pold >= 0.25)
		{
			h = 0.001319 ;
			i = 0.103335 ;
			j = - 0.05416 ;
		}
		else
		{
			h = 0.001158 ;
			i = 0.149479 ;
			j = -0.05835 ;
		}

		// 定数行列Cの作成
		Cmat = C ;
		*Cmat = Desica->ms * Desica->cps / dTM * Desi->Tsold ;
		Cmat++ ;
		*Cmat = Desi->UA * Te ;
		Cmat++ ;
		Cmat++ ;
		*Cmat = Desica->ms / dTM * Desi->Pold ;
		Cmat++ ;
		*Cmat = -j ;

		// 係数行列の作成
		U[0*N+0] = Desica->ms * Desica->cps / dTM + hAsa ;
		U[0*N+1] = -hAsa ;
		U[0*N+2] = -hdAsa * ro ;
		U[0*N+3] = hdAsa * ro ;
		U[1*N+0] = -hAsa ;
		U[1*N+1] = ca * Eo->G + hAsa + Desi->UA ;
		U[2*N+2] = Eo->G + hdAsa ;
		U[2*N+3] = -hdAsa ;
		U[3*N+2] = -hdAsa ;
		U[3*N+3] = hdAsa ;
		U[3*N+4] = Desica->ms / dTM ;
		U[4*N+0] = h ;
		U[4*N+3] = -1. ;
		U[4*N+4] = i ;

		//matprint("%lf ", N, U) ;
		//for(ii = 0; ii < N; ii++)
		//	printf("%lf\n", C[ii]) ;
		// 逆行列の計算
		matinv(U, N, N, "<Desicfv U>") ;

		// 行列のコピー
		matinit(Desi->UX, N2) ;
		matcpy(U, Desi->UX, N2) ;

		// {UXC}=[UX]*{C}の作成
		matinit(Desi->UXC, N) ;
		for(ii = 0; ii < N; ii++)
		{
			for(jj = 0; jj < N; jj++)
				*(Desi->UXC+ii) += *(Desi->UX + ii * N + jj) * *(C + jj) ;
		}
		
		
		// 出口温度の要素方程式
		Eo->coeffo = -1.0 ;
		Eo->Co = -Desi->UXC[1] ; ;
		cfin = Eo->coeffin ;
		*cfin = Desi->UX[1*N+1] * Eo->G * ca ;
		cfin++ ;
		*cfin = Desi->UX[1*N+2] * Eo->G ;
		
		// 出口湿度の要素方程式
		Eo++;
		Eo->coeffo = -1. ;
		Eo->Co = -Desi->UXC[2] ;
		cfin = Eo->coeffin ;
		*cfin = Desi->UX[2*N+2] * Eo->G ;
		cfin++ ;
		*cfin = Desi->UX[2*N+1] * Eo->G * ca ;

		//Det1 = (d + e - a + 1.) * (f + f * g * i + 1.) + b * f * h * (d + e + 1.) ;
		//Det2 = b * f ;

		//A1 = d * (f + b * f * h + f * g * i + 1.) / Det1 ;
		//A2 = b * f / Det1 ;
		//A3 = c * (f + f * g * i + 1.) / Det1 ;
		//A4 = e * (f + b * f * h + f * g * i + 1.) / Det1 ;
		//A5 = - b * f * i / Det1 ;
		//A6 = - b * f * j / Det1 ;
		//CC = a - d - e - 1 ;
		//B1 = (CC * A1 + d) / Det2 ;
		//B2 = (CC * A2 + b * f) / Det2 ;
		//B3 = (CC * A3 + c)/ Det2 ;
		//B4 = (CC * A4 + e) / Det2 ;
		//B5 = CC * A5 / Det2 ;
		//B6 = CC * A6 / Det2 ;
		//
		//// 出口温度の要素方程式
		//Eo->coeffo = -1.0 ;
		//Eo->Co = -(A3 * Desi->Tsold + A4 * Te + A5 * Desi->Pold + A6) ;
		//cfin = Eo->coeffin ;
		//*cfin = A1 ;
		//cfin++ ;
		//*cfin = A2 ;
		//
		//// 出口湿度の要素方程式
		//Eo++;
		//Eo->coeffo = -1. ;
		//Eo->Co = -(B3 * Desi->Tsold + B4 * Te + B5 * Desi->Pold + B6) ;
		//cfin = Eo->coeffin ;
		//*cfin = B2 ;
		//cfin++ ;
		//*cfin = B1 ;

		free(U) ;
		free(C) ;
	}
}
///* --------------------------- */
//
///* 取得熱量の計算 */
//
void Desiene(int NDesi, DESI *Desi)
{
	int i, ii, jj, N, N2;
	ELOUT	*elo, *elox ;
	ELIN	*elix ;
	extern double	dTM, ro, ca ;
	DESICA	*cat ;
	double	Te, Sin[5], S[5] ;
	
	N = 5;
	N2 = N * N ;
	for (i = 0; i < NDesi; i++, Desi++)
	{
		matinit(Sin, N) ;
		matinit(S, N) ;
		elo = Desi->cmp->elouts ;
		elox = elo + 1 ;
		elix = elo->elins + 1 ;
		Desi->Tain = elo->elins->sysvin;
		Desi->xain = elix->sysvin ;
		
		cat = Desi->cat ;
		
		if ( Desi->cmp->envname != NULL )
			Te = *(Desi->Tenv) ;
		else
			Te = Desi->room->Tot ;
		
		/****************************
		printf ( "envname=%s\n", stheat->room->name ) ;
		/***************************/
		Desi->Taout = elo->sysv ;
		Desi->xaout = elox->sysv ;

		// 入口状態行列Sinの作成
		Sin[1] = ca * elo->G * Desi->Tain ;
		Sin[2] = elo->G * Desi->xain ;
		// 内部状態値の計算
		for(ii = 0; ii < N; ii++)
		{
			for(jj = 0; jj < N; jj++)
				S[ii] += Desi->UX[ii*N+jj] * Sin[jj] ;
			S[ii] += Desi->UXC[ii] ;
		}
		// 変数への格納
		Desi->Tsold = S[0] ;
		Desi->Ta = S[1] ;
		Desi->Xa = S[2] ;
		Desi->Xsold = S[3] ;
		Desi->Pold = S[4] ;
		//Desi->Tsold = (Desi->d + Desi->e + 1.) * Desi->Taout - Desi->d * Desi->Tain - Desi->e * Te ;
		//Desi->Xsold = (Desi->f + 1.) * Desi->xaout - Desi->f * Desi->xain ;
		Desi->RHold = FNRhtx(Desi->Tsold, Desi->Xsold) ;
		//Desi->Pold = Desi->g * Desi->xaout - Desi->g * Desi->Xsold + Desi->Pold ;
		
		// 顕熱の計算
		Desi->Qs = Desi->cG * ( Desi->Taout - Desi->Tain ) ;
		Desi->Ql = elo->G * ro * ( Desi->xaout - Desi->xain ) ;
		Desi->Qt = Desi->Qs + Desi->Ql ;

		// デシカント槽からの熱損失の計算
		Desi->Qloss = Desi->UA * ( Te - Desi->Ta ) ;
		
		// 設置室内部発熱の計算
		if ( Desi->room != NULL )
			Desi->room->Qeqp += ( - Desi->Qloss ) ;
	}
}

// 制御で使用する内部変数
int	Desivptr(char **key, DESI *Desi, VPTR *vptr)
{
	int err = 0;
	
	if (strcmp(key[1], "Ts") == 0)
	{
		vptr->ptr = &Desi->Tsold;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "xs") == 0)
	{
		vptr->ptr = &Desi->Xsold;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "RH") == 0)
	{
		vptr->ptr = &Desi->RHold;
		vptr->type = VAL_CTYPE;
	}
	else
		err = 1;
	
	return err;
}
///* ---------------------------*/
//
void Desiprint(FILE *fo, int id, int Ndesi, DESI *Desi)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Ndesi > 0)
            fprintf(fo, "%s %d\n", DESI_TYPE, Ndesi);
		for (i = 0; i < Ndesi; i++, Desi++)
			fprintf(fo, " %s 1 14\n", Desi->name);
		break;
		
	case 1:
		for (i = 0; i < Ndesi; i++, Desi++)
		{	 
			fprintf(fo,"%s_c c c %s_G m f %s_Ts t f %s_Ti t f %s_To t f %s_Qs q f ",
				Desi->name, Desi->name, Desi->name, Desi->name, Desi->name, Desi->name);
			fprintf(fo,"%s_xs x f %s_RHs r f %s_xi x f %s_xo x f %s_Ql q f %s_Qt q f ",
				Desi->name, Desi->name, Desi->name, Desi->name, Desi->name, Desi->name);
			fprintf(fo,"%s_Qls q f %s_P m f\n", Desi->name, Desi->name);
		}
		break;
		
	default:
		for (i = 0; i < Ndesi; i++, Desi++)
		{
            fprintf(fo, "%c %6.4g %4.1lf %4.1lf %4.1lf %2.0lf  ",
				Desi->cmp->elouts->control, Desi->cmp->elouts->G,
				Desi->Tsold, Desi->Tain, Desi->Taout, Desi->Qs);
            fprintf(fo, "%.3lf %.0lf %.3lf %.3lf %2.0lf %2.0lf  ",
				Desi->Xsold, Desi->RHold, Desi->xain, Desi->xaout, Desi->Ql, Desi->Qt);
			fprintf(fo, "%.0lf %.3lf\n", Desi->Qloss, Desi->Pold);
		}
		break;
	}
}

///* --------------------------- */
//
///* 日積算値に関する処理 */
//
///*******************/
void Desidyint(int Ndesi, DESI *Desi)
{
	int i;
	
	for (i = 0; i < Ndesi; i++, Desi++)
	{
		svdyint(&Desi->Tidy);
		svdyint(&Desi->Tsdy);
		svdyint(&Desi->Tody);
		svdyint(&Desi->xidy);
		svdyint(&Desi->xsdy);
		svdyint(&Desi->xody);
		qdyint(&Desi->Qsdy);
		qdyint(&Desi->Qldy);
		qdyint(&Desi->Qtdy);
		qdyint(&Desi->Qlsdy);
	}
}

//void stheatmonint(int Nstheat, STHEAT *stheat)
//{
//	int i;
//	
//	for (i = 0; i < Nstheat; i++, stheat++)
//	{
//		stheat->mQlossdy = 0.0 ;
//		stheat->mQstody = 0.0 ;
//
//		svdyint(&stheat->mTidy);
//		svdyint(&stheat->mTsdy);
//		svdyint(&stheat->mTody);
//		qdyint(&stheat->mQdy);
//		edyint(&stheat->mEdy);
//	}
//}
//
void Desiday(int Mon, int Day, int ttmm, int Ndesi, DESI *Desi, int Nday, int SimDayend)
{
	int i;
	int Mo, tt ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	
	for (i = 0; i < Ndesi; i++, Desi++)
	{
		// 日集計
		svdaysum(ttmm, Desi->cmp->control, Desi->Tain, &Desi->Tidy);
		svdaysum(ttmm, Desi->cmp->control, Desi->Taout, &Desi->Tody);
		svdaysum(ttmm, Desi->cmp->control, Desi->Tsold, &Desi->Tsdy);
		svdaysum(ttmm, Desi->cmp->control, Desi->xain, &Desi->xidy);
		svdaysum(ttmm, Desi->cmp->control, Desi->xaout, &Desi->xody);
		svdaysum(ttmm, Desi->cmp->control, Desi->Xsold, &Desi->xsdy);
		qdaysum(ttmm, Desi->cmp->control, Desi->Qs, &Desi->Qsdy);
		qdaysum(ttmm, Desi->cmp->control, Desi->Ql, &Desi->Qldy);
		qdaysum(ttmm, Desi->cmp->control, Desi->Qt, &Desi->Qtdy);
		qdaysum(ttmm, Desi->cmp->control, Desi->Qloss, &Desi->Qlsdy);
		
		//// 月集計
		//stheat->mQlossdy += stheat->Qls ;
		//stheat->mQstody += stheat->Qsto ;
		//svmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Tin, &stheat->mTidy, Nday, SimDayend);
		//svmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Tout, &stheat->mTody, Nday, SimDayend);
		//svmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Ts, &stheat->mTsdy, Nday, SimDayend);
		//qmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Q, &stheat->mQdy, Nday, SimDayend);
		//emonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->E, &stheat->mEdy, Nday, SimDayend);	

		//// 月・時刻のクロス集計
		//emtsum(Mon, Day, ttmm, stheat->cmp->control, stheat->E, &stheat->mtEdy[Mo][tt]);
	}
}

void Desidyprt(FILE *fo, int id, int Ndesi, DESI *Desi)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Ndesi > 0)
            fprintf(fo, "%s %d\n", DESI_TYPE, Ndesi);
		for (i = 0; i < Ndesi; i++, Desi++)
			fprintf(fo, " %s 1 68\n", Desi->name);
		break;
		
	case 1:
		for (i = 0; i < Ndesi; i++, Desi++)
		{	 
			fprintf(fo,"%s_Ht H d %s_Ti T f ", Desi->name, Desi->name);
			fprintf(fo,"%s_ttn h d %s_Tin t f %s_ttm h d %s_Tim t f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo,"%s_Ht H d %s_To T f ", Desi->name, Desi->name);
			fprintf(fo,"%s_ttn h d %s_Ton t f %s_ttm h d %s_Tom t f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo,"%s_Ht H d %s_Ts T f ", Desi->name, Desi->name);
			fprintf(fo,"%s_ttn h d %s_Tsn t f %s_ttm h d %s_Tsm t f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);
			
			fprintf(fo,"%s_Ht H d %s_xi T f ", Desi->name, Desi->name);
			fprintf(fo,"%s_ttn h d %s_xin t f %s_ttm h d %s_xim t f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo,"%s_Ht H d %s_xo T f ", Desi->name, Desi->name);
			fprintf(fo,"%s_ttn h d %s_xon t f %s_ttm h d %s_xom t f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo,"%s_Ht H d %s_xs T f ", Desi->name, Desi->name);
			fprintf(fo,"%s_ttn h d %s_xsn t f %s_ttm h d %s_xsm t f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo, "%s_Hsh H d %s_Qsh Q f %s_Hsc H d %s_Qsc Q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);
			fprintf(fo, "%s_tsh h d %s_qsh q f %s_tsc h d %s_qsc q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo, "%s_Hlh H d %s_Qlh Q f %s_Hlc H d %s_Qlc Q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);
			fprintf(fo, "%s_tlh h d %s_qlh q f %s_tlc h d %s_qlc q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo, "%s_Hth H d %s_Qth Q f %s_Htc H d %s_Qtc Q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);
			fprintf(fo, "%s_tth h d %s_qth q f %s_ttc h d %s_qtc q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);

			fprintf(fo, "%s_Hlsh H d %s_Qlsh Q f %s_Hlsc H d %s_Qlsc Q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);
			fprintf(fo, "%s_tlsh h d %s_qlsh q f %s_tlsc h d %s_qlsc q f\n",
				Desi->name, Desi->name, Desi->name, Desi->name);
		}
		break;
		
	default:
		for (i = 0; i < Ndesi; i++, Desi++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Desi->Tidy.hrs, Desi->Tidy.m, 
				Desi->Tidy.mntime, Desi->Tidy.mn, 
				Desi->Tidy.mxtime, Desi->Tidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Desi->Tody.hrs, Desi->Tody.m, 
				Desi->Tody.mntime, Desi->Tody.mn, 
				Desi->Tody.mxtime, Desi->Tody.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Desi->Tsdy.hrs, Desi->Tsdy.m, 
				Desi->Tsdy.mntime, Desi->Tsdy.mn, 
				Desi->Tsdy.mxtime, Desi->Tsdy.mx);

            fprintf(fo, "%1ld %.4lf %1ld %.4lf %1ld %.4lf ", 
				Desi->xidy.hrs, Desi->xidy.m, 
				Desi->xidy.mntime, Desi->xidy.mn, 
				Desi->xidy.mxtime, Desi->xidy.mx);
			fprintf(fo, "%1ld %.4lf %1ld %.4lf %1ld %.4lf ", 
				Desi->xody.hrs, Desi->xody.m, 
				Desi->xody.mntime, Desi->xody.mn, 
				Desi->xody.mxtime, Desi->xody.mx);
			fprintf(fo, "%1ld %.4lf %1ld %.4lf %1ld %.4lf ", 
				Desi->xsdy.hrs, Desi->xsdy.m, 
				Desi->xsdy.mntime, Desi->xsdy.mn, 
				Desi->xsdy.mxtime, Desi->xsdy.mx);

            fprintf(fo, "%1ld %3.1lf ", Desi->Qsdy.hhr, Desi->Qsdy.h);
            fprintf(fo, "%1ld %3.1lf ", Desi->Qsdy.chr, Desi->Qsdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Desi->Qsdy.hmxtime, Desi->Qsdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Desi->Qsdy.cmxtime, Desi->Qsdy.cmx);

            fprintf(fo, "%1ld %3.1lf ", Desi->Qldy.hhr, Desi->Qldy.h);
            fprintf(fo, "%1ld %3.1lf ", Desi->Qldy.chr, Desi->Qldy.c);      
            fprintf(fo, "%1ld %2.0lf ", Desi->Qldy.hmxtime, Desi->Qldy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Desi->Qldy.cmxtime, Desi->Qldy.cmx);

            fprintf(fo, "%1ld %3.1lf ", Desi->Qtdy.hhr, Desi->Qtdy.h);
            fprintf(fo, "%1ld %3.1lf ", Desi->Qtdy.chr, Desi->Qtdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Desi->Qtdy.hmxtime, Desi->Qtdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Desi->Qtdy.cmxtime, Desi->Qtdy.cmx);

            fprintf(fo, "%1ld %3.1lf ", Desi->Qlsdy.hhr, Desi->Qlsdy.h);
            fprintf(fo, "%1ld %3.1lf ", Desi->Qlsdy.chr, Desi->Qlsdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Desi->Qlsdy.hmxtime, Desi->Qlsdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Desi->Qlsdy.cmxtime, Desi->Qlsdy.cmx);
		}
		break;
	}
}

//void stheatmonprt(FILE *fo, int id, int Nstheat, STHEAT *stheat)
//{
//	int i;
//	extern double  cff_kWh;
//	
//	switch (id)
//	{
//	case 0:
//		if (Nstheat > 0)
//            fprintf(fo, "%s %d\n", STHEAT_TYPE, Nstheat);
//		for (i = 0; i < Nstheat; i++, stheat++)
//			fprintf(fo, " %s 1 32\n", stheat->name);
//		break;
//		
//	case 1:
//		for (i = 0; i < Nstheat; i++, stheat++)
//		{	 
//			fprintf(fo,"%s_Ht H d %s_Ti T f ", stheat->name, stheat->name);
//			fprintf(fo,"%s_ttn h d %s_Tin t f %s_ttm h d %s_Tim t f\n",
//				stheat->name, stheat->name, stheat->name, stheat->name);
//			fprintf(fo,"%s_Ht H d %s_To T f ", stheat->name, stheat->name);
//			fprintf(fo,"%s_ttn h d %s_Ton t f %s_ttm h d %s_Tom t f\n",
//				stheat->name, stheat->name, stheat->name, stheat->name);
//			fprintf(fo,"%s_Ht H d %s_Ts T f ", stheat->name, stheat->name);
//			fprintf(fo,"%s_ttn h d %s_Tsn t f %s_ttm h d %s_Tsm t f\n",
//				stheat->name, stheat->name, stheat->name, stheat->name);
//			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
//				stheat->name, stheat->name, stheat->name, stheat->name);
//			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
//				stheat->name, stheat->name, stheat->name, stheat->name);
//			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
//				stheat->name, stheat->name, stheat->name, stheat->name);
//			fprintf(fo, "%s_Qls Q f %s_Qst Q f\n\n",
//				stheat->name, stheat->name);
//		}
//		break;
//		
//	default:
//		for (i = 0; i < Nstheat; i++, stheat++)
//		{
//            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				stheat->mTidy.hrs, stheat->mTidy.m, 
//				stheat->mTidy.mntime, stheat->mTidy.mn, 
//				stheat->mTidy.mxtime, stheat->mTidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				stheat->mTody.hrs, stheat->mTody.m, 
//				stheat->mTody.mntime, stheat->mTody.mn, 
//				stheat->mTody.mxtime, stheat->mTody.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				stheat->mTsdy.hrs, stheat->mTsdy.m, 
//				stheat->mTsdy.mntime, stheat->mTsdy.mn, 
//				stheat->mTsdy.mxtime, stheat->mTsdy.mx);
//            fprintf(fo, "%1d %3.1lf ", stheat->mQdy.hhr, stheat->mQdy.h);
//            fprintf(fo, "%1d %3.1lf ", stheat->mQdy.chr, stheat->mQdy.c);      
//            fprintf(fo, "%1d %2.0lf ", stheat->mQdy.hmxtime, stheat->mQdy.hmx);
//            fprintf(fo, "%1d %2.0lf ", stheat->mQdy.cmxtime, stheat->mQdy.cmx);
//			fprintf(fo, "%1d %3.1lf ", stheat->mEdy.hrs, stheat->mEdy.d);
//            fprintf(fo, "%1d %2.0lf ", stheat->mEdy.mxtime, stheat->mEdy.mx);
//			fprintf(fo, " %3.1lf %3.1lf\n", 
//				stheat->mQlossdy * cff_kWh, stheat->mQstody * cff_kWh);
//		}
//		break;
//	}
//}
//
//void stheatmtprt(FILE *fo, int id, int Nstheat, STHEAT *stheat, int Mo, int tt)
//{
//	int i;
//	extern double  cff_kWh;
//	
//	switch (id)
//	{
//	case 0:
//		if (Nstheat > 0)
//            fprintf(fo, "%s %d\n", STHEAT_TYPE, Nstheat);
//		for (i = 0; i < Nstheat; i++, stheat++)
//			fprintf(fo, " %s 1 1\n", stheat->name);
//		break;
//		
//	case 1:
//		for (i = 0; i < Nstheat; i++, stheat++)
//		{	 
//			fprintf(fo,"%s_E E f \n", stheat->name);
//		}
//		break;
//		
//	default:
//		for (i = 0; i < Nstheat; i++, stheat++)
//		{
//			fprintf(fo, " %.2lf\n", stheat->mtEdy[Mo-1][tt-1].d * cff_kWh);
//		}
//		break;
//	}
//}
