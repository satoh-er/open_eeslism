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

/*  hcload.c  */

/*  空調負荷仮想機器  */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"
#include "fnesy.h"

/* ------------------------------------------ */

/*  コイル出口空気温湿度に関する変数割当  */

void Hclelm(int Nhcload, HCLOAD *Hcload)
{
	int i;
	ELOUT  *Eo, *elo;
	ELIN   *elini, *elin;
	
	for (i = 0; i < Nhcload; i++, Hcload++)
	{
		// 湿りコイルの場合
		if (Hcload->wet == 'y')
		{
			// 空気温度出口の計算式
			Eo = Hcload->cmp->elouts;
			// elo:空気湿度出口の計算式
			elo = Eo + 1;
			// elini:空気湿度出口の要素方程式の2つ目の変数
			elini = elo->elins + 1;
			// 空気絶対湿度の要素方程式の2つ目の変数にupo、upvに空気出口をつなげる
			elini->upo = elini->upv = Eo;
		}
		
		if (Hcload->type == 'W')
		{
			Eo = Hcload->cmp->elouts;
			elo = Eo + 2;
			elini = elo->elins + 1;
			elin = elo->elins + 3;
			
			elini->upo = elini->upv = Eo->elins->upo;
			elin->upo = elin->upv = Eo;
			elini++, elin++, Eo++;
			
			elini->upo = elini->upv = Eo->elins->upo;
			elin->upo = elin->upv = Eo;
		}
	}
}


/* -------------------------------------------------- */

/* ルームエアコン（事業主基準モデル）機器仕様の入力処理 */

void rmacdat(HCLOAD *Hcld)
{
	char s[SCHAR], *ss, *st;
	int  n = 0;
	char     Err[SCHAR];
	
	sprintf(Err, ERRFMT, "(rmacdat)");
	
	ss = Hcld->cmp->tparm;
	Hcld->Qhmax = Hcld->Qh = Hcld->COPc = Hcld->COPh = -999.0 ;
	Hcld->Qcmax = Hcld->Qc = 999. ;
	while(sscanf(ss, "%s", s), strchr(s,'*') == NULL)
	{
		ss += strlen(s);
		while(isspace(*ss))
			ss++;
		
		if ((st = strchr(s, '=')) != NULL)
		{ 
			*st = '\0';
			if (strcmp(s, "Qc") == 0)
			{
				Hcld->Qc = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qcmax") == 0)
			{
				Hcld->Qcmax = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qh") == 0)
			{
				Hcld->Qh = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qhmax") == 0)
			{
				Hcld->Qhmax = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "COPc") == 0)
			{
				Hcld->COPc = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "COPh") == 0)
			{
				Hcld->COPh = atof(stralloc(st + 1)) ;
				n++;
			}
		}
		else
		{
			Eprint ( "<rmacdat>", s ) ;
		}
	}

	if ( Hcld->Qc < 0. )
	{
		Hcld->rc = Hcld->Qcmax / Hcld->Qc ;
		Hcld->Ec = - Hcld->Qc / Hcld->COPc ;
	}
	if ( Hcld->Qh > 0. )
	{
		Hcld->rh = Hcld->Qhmax / Hcld->Qh ;
		Hcld->Eh = Hcld->Qh / Hcld->COPh ;
	}
}

/* ルームエアコン（電中研モデル）機器仕様の入力処理 */

void rmacddat(HCLOAD *Hcld)
{
	char s[SCHAR], *ss, *st;
	int  n = 0;
	char     Err[SCHAR];
	double	DBco, DBci, WBco, WBci, xco, xci ;
	double	effthr, effthmin, effthmax, cao, cai, X, Rr, Rmax ;
	double	U[9], Qc[3], R[3] ;
	extern double ca, cv ;
	int		i, j ;
	
	sprintf(Err, ERRFMT, "(rmacddat)");
	
	ss = Hcld->cmp->tparm;
	//Hcld->Qhmax = Hcld->Qh = Hcld->COPc = Hcld->COPh = -999.0 ;
	Hcld->Qcmax = Hcld->Qc = Hcld->Qcmin = 999. ;
	Hcld->Ecmax = Hcld->Ec = Hcld->Ecmin = Hcld->Qh = Hcld->Qhmax = Hcld->Qhmin 
		= Hcld->Ehmax = Hcld->Eh = Hcld->Ehmin = -999. ;
	Hcld->Gi = Hcld->Go = -999.0 ;
	while(sscanf(ss, "%s", s), strchr(s,'*') == NULL)
	{
		ss += strlen(s);
		while(isspace(*ss))
			ss++;
		
		if ((st = strchr(s, '=')) != NULL)
		{ 
			*st = '\0';
			if (strcmp(s, "Qc") == 0)
			{
				Hcld->Qc = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qcmax") == 0)
			{
				Hcld->Qcmax = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qcmin") == 0)
			{
				Hcld->Qcmin = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Ec") == 0)
			{
				Hcld->Ec = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Ecmax") == 0)
			{
				Hcld->Ecmax = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Ecmin") == 0)
			{
				Hcld->Ecmin = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qh") == 0)
			{
				Hcld->Qh = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qhmax") == 0)
			{
				Hcld->Qhmax = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Qhmin") == 0)
			{
				Hcld->Qhmin = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Eh") == 0)
			{
				Hcld->Eh = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Ehmax") == 0)
			{
				Hcld->Ehmax = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Ehmin") == 0)
			{
				Hcld->Ehmin = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Gi") == 0)
			{
				Hcld->Gi = atof(stralloc(st + 1)) ;
				n++;
			}
			else if (strcmp(s, "Go") == 0)
			{
				Hcld->Go = atof(stralloc(st + 1)) ;
				n++;
			}
			//else if (strcmp(s, "Qh") == 0)
			//{
			//	Hcld->Qh = atof(stralloc(st + 1)) ;
			//	n++;
			//}
			//else if (strcmp(s, "Qhmax") == 0)
			//{
			//	Hcld->Qhmax = atof(stralloc(st + 1)) ;
			//	n++;
			//}
			else
			{
				Eprint ( "<rmacddat>", s ) ;
			}
		}
		else
		{
			Eprint ( "<rmacddat>", s ) ;
		}
	}

	// 機器固有値の計算
	if ( Hcld->Qc < 0. )
	{
		Hcld->COPc = -Hcld->Qc / Hcld->Ec ;
		Hcld->COPcmax = -Hcld->Qcmax / Hcld->Ecmax ;
		Hcld->COPcmin = -Hcld->Qcmin / Hcld->Ecmin ;
		
		// JISにおける温湿度条件
		DBco = 35. ;
		DBci = 27.0 ;
		WBco = 24.0 ;
		WBci = 19.0 ;
		// 絶対湿度
		xco = FNXtw(DBco, WBco) ;
		xci = FNXtw(DBci, WBci) ;
		// 湿り比熱
		cao = ca + cv * xco ;
		cai = ca + cv * xci ;

		// 室内機、室外機熱交換器のバイパスファクタ
		Hcld->BFi = Hcld->BFo = 0.2 ;

		// 理論効率の計算
		// 定格条件
		effthr = FNeffthc(DBco, DBci, xci, Hcld->Qc, Hcld->Ec, (1. - Hcld->BFo) * Hcld->Go, (1. - Hcld->BFi) * Hcld->Gi, cai, cao) ;
		// 最小能力
		effthmin = FNeffthc(DBco, DBci, xci, Hcld->Qcmin, Hcld->Ecmin, (1. - Hcld->BFo) * Hcld->Go, (1. - Hcld->BFi) * Hcld->Gi, cai, cao) ;
		// 最大能力
		effthmax = FNeffthc(DBco, DBci, xci, Hcld->Qcmax, Hcld->Ecmax, (1. - Hcld->BFo) * Hcld->Go, (1. - Hcld->BFi) * Hcld->Gi, cai, cao) ;

		// ファン等動力の計算
		X = Hcld->COPcmin * Hcld->Qcmin / effthmin * effthr / (Hcld->Qc * Hcld->COPc) ;
		Hcld->Pcc = (- Hcld->Qcmin - X * (- Hcld->Qc)) / (Hcld->COPcmin - X * Hcld->COPc) ;

		// 定格条件、最小能力時の理論COPと実働COPの比R（両条件のRは等しいと仮定）
		Rr = (- Hcld->Qc * Hcld->COPc) / (effthr * (- Hcld->Qc - Hcld->Pcc * Hcld->COPc)) ;
		// 最大能力時のRを計算
		Rmax = (- Hcld->Qcmax * Hcld->COPcmax) / (effthmax * (- Hcld->Qcmax - Hcld->Pcc * Hcld->COPcmax)) ;

		// Rの回帰式係数の計算
		//U = dcalloc(3 * 3, "<rmacddat> U") ;
		//Qc = dcalloc(3, "<rmacddat> R") ;
		Qc[0] = - Hcld->Qcmin, Qc[1] = - Hcld->Qc, Qc[2] = - Hcld->Qcmax ;
		R[0] = R[1] = Rr, R[2] = Rmax ;
		// 行列Uの作成
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
				U[i*3+j] = pow(Qc[i], 2. - (double) j) ;
		}

		//printf("Matrix U\n") ;
		//matprint("%lf  ", 3, U) ;

		// Uの逆行列の計算
		matinv(U, 3, 3, "<rmacddat> UX") ;

		// 回帰係数の計算
		matinit(Hcld->Rc, 3) ;
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
				Hcld->Rc[i] += U[i*3+j] * R[j] ;
		}

		//printf("Matrix R\n") ;
		//for(i = 0; i < 3; i++)
		//	printf("%lf\n", Hcld->R[i]) ;

		//printf("aaaaa\n") ;
	}

	if ( Hcld->Qh > 0. )
	{
		Hcld->COPh = Hcld->Qh / Hcld->Eh ;
		Hcld->COPhmax = Hcld->Qhmax / Hcld->Ehmax ;
		Hcld->COPhmin = Hcld->Qhmin / Hcld->Ehmin ;
		
		// JISにおける温湿度条件
		DBco = 7. ;
		DBci = 20. ;
		WBco = 6. ;
		WBci = 15. ;
		// 絶対湿度
		xco = FNXtw(DBco, WBco) ;
		xci = FNXtw(DBci, WBci) ;
		// 湿り比熱
		cao = ca + cv * xco ;
		cai = ca + cv * xci ;

		// 室内機、室外機熱交換器のバイパスファクタ
		Hcld->BFi = Hcld->BFo = 0.2 ;

		// 理論効率の計算
		// 定格条件
		effthr = FNeffthh(DBco, DBci, xco, Hcld->Qh, Hcld->Eh, (1. - Hcld->BFo) * Hcld->Go, (1. - Hcld->BFi) * Hcld->Gi, cai, cao) ;
		// 最小能力
		effthmin = FNeffthh(DBco, DBci, xco, Hcld->Qhmin, Hcld->Ehmin, (1. - Hcld->BFo) * Hcld->Go, (1. - Hcld->BFi) * Hcld->Gi, cai, cao) ;
		// 最大能力
		effthmax = FNeffthh(DBco, DBci, xco, Hcld->Qhmax, Hcld->Ehmax, (1. - Hcld->BFo) * Hcld->Go, (1. - Hcld->BFi) * Hcld->Gi, cai, cao) ;

		// ファン等動力の計算
		X = Hcld->COPhmin * Hcld->Qhmin / effthmin * effthr / (Hcld->Qh * Hcld->COPh) ;
		Hcld->Pch = (Hcld->Qhmin - X * Hcld->Qh) / (Hcld->COPhmin - X * Hcld->COPh) ;

		// 定格条件、最小能力時の理論COPと実働COPの比R（両条件のRは等しいと仮定）
		Rr = (Hcld->Qh * Hcld->COPh) / (effthr * (Hcld->Qh - Hcld->Pch * Hcld->COPh)) ;
		// 最大能力時のRを計算
		Rmax = (Hcld->Qhmax * Hcld->COPhmax) / (effthmax * (Hcld->Qhmax - Hcld->Pch * Hcld->COPhmax)) ;

		// Rの回帰式係数の計算
		//U = dcalloc(3 * 3, "<rmacddat> U") ;
		//Qc = dcalloc(3, "<rmacddat> R") ;
		Qc[0] = Hcld->Qhmin, Qc[1] = Hcld->Qh, Qc[2] = Hcld->Qhmax ;
		R[0] = R[1] = Rr, R[2] = Rmax ;
		// 行列Uの作成
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
				U[i*3+j] = pow(Qc[i], 2. - (double) j) ;
		}

		//printf("Matrix U\n") ;
		//matprint("%lf  ", 3, U) ;

		// Uの逆行列の計算
		matinv(U, 3, 3, "<rmacddat> UX") ;

		// 回帰係数の計算
		matinit(Hcld->Rh, 3) ;
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
				Hcld->Rh[i] += U[i*3+j] * R[j] ;
		}

		//printf("Matrix R\n") ;
		//for(i = 0; i < 3; i++)
		//	printf("%lf\n", Hcld->R[i]) ;

		//printf("aaaaa\n") ;
	}
	//if ( Hcld->Qh > 0. )
	//{
	//	Hcld->rh = Hcld->Qhmax / Hcld->Qh ;
	//	Hcld->Eh = Hcld->Qh / Hcld->COPh ;
	//}
}

// 冷房時の理論COPを計算する
double	FNeffthc(double Tco, double Tci, double xci, double Qc, double Ec, double Go, double Gi, double cai, double cao)
{
	double	Tevp, Tcnd;

	// 凝縮温度の計算
	Tcnd = FNTcndc(Tco, Qc, Ec, cao, Go) ;
	// 蒸発温度の計算
	Tevp = FNTevpc(Tci, Qc, cai, Gi, xci) ;

	// 理論効率の計算
	return (Tevp + 273.15) / (Tcnd - Tevp) ;
}

// 暖房時の理論COPを計算する
double	FNeffthh(double Tco, double Tci, double xco, double Qc, double Eh, double Go, double Gi, double cai, double cao)
{
	double	Tevp, Tcnd;

	// 凝縮温度の計算
	Tcnd = FNTcndh(Tci, Qc, cai, Gi) ;
	// 蒸発温度の計算
	Tevp = FNTevph(Tco, Qc, Eh, cao, Go, xco) ;

	// 理論効率の計算
	return (Tcnd + 273.15) / (Tcnd - Tevp) ;
}

// 冷房時凝縮温度の計算
double	FNTcndc(double Tco, double Qc, double Ec, double cao, double Go)
{
	return(Tco + (- Qc + Ec) / (cao * Go)) ;
}

// 暖房時凝縮温度の計算
double	FNTcndh(double Tci, double Qc, double cai, double Gi)
{
	return(Tci + Qc / (cai * Gi)) ;
}

// 冷房時蒸発温度の計算
double	FNTevpc(double Tci, double Qc, double cai, double Gi, double xci)
{
	double	Tevp, RHi ;

	// 蒸発温度の計算
	Tevp = Tci - (- Qc) / (cai * Gi) ;
	// 室内が結露するかどうかの判定（結露時は等エンタルピー変化による飽和状態とする）
	RHi = FNRhtx(Tevp, xci) ;
	if(RHi > 100.)
		Tevp = FNDbrh(100., FNH(Tevp, xci)) ;

	return(Tevp) ;
}

// 暖房時蒸発温度の計算
double	FNTevph(double Tco, double Qc, double Eh, double cao, double Go, double xco)
{
	double	Tevp, RHo ;

	// 蒸発温度の計算
	Tevp = Tco - (Qc - Eh) / (cao * Go) ;
	// 室外が結露するかどうかの判定（結露時は等エンタルピー変化による飽和状態とする）
	RHo = FNRhtx(Tevp, xco) ;
	if(RHo > 100.)
		Tevp = FNDbrh(100., FNH(Tevp, xco)) ;

	return(Tevp) ;
}

/*  特性式の係数  */

void Hcldcfv(int Nhcload, HCLOAD *Hcload)
{
	ELOUT *Eo;
	double *cfin,
		rGa;
	int i;
	extern double ro;
	/*
	static double  f0 = -1.2e-3, f1 = 7.2e-4;  
	*/
	/* The coefficients used in the linear equation
	for a constant outlet humidity model of wet coil */

    /*---- Roh Debug for a constant outlet humidity model of wet coil  2003/4/25 ----*/
	static double  f0, f1;
	double Tout15 = 15.0,
		   Tout20 = 20.0,
		   Xout15,
		   Xout20;
	static int ic = 0 ;

	for (i = 0; i < Nhcload; i++, Hcload++)
	{
		//if ( ic == 0 )
		//{
			Xout15 = FNXtr(Tout15, Hcload->RHout);
			Xout20 = FNXtr(Tout20, Hcload->RHout);
			
			f1 = (Xout20 - Xout15) / (Tout20 - Tout15);
			f0 = Xout15 - f1*Tout15;
			ic = 1 ;
		//}
		/* The coefficients used in the linear equation
		for constant outlet humidities model of wet coil */
		
		Eo = Hcload->cmp->elouts;  
		Hcload->Ga = Eo->G ;
		
		/*****
		printf("<<Hcldcfv>>  %s  type=%c  control=%c fluid=%c\n",
		Hcload->name, Hcload->type, Eo->control, Eo->fluid);
		/******/
		
		if (Eo->control != OFF_SW)		// 空気（温度）の系統
		{ 
			Hcload->Ga = Eo->G;      
			Hcload->cGa = spcheat(Eo->fluid) * Hcload->Ga;
			
			Eo->coeffo = Hcload->cGa;
			Eo->Co = 0.0;
			cfin = Eo->coeffin;
			*cfin  =  -Hcload->cGa;
		}
		
		Eo++;
		if (Eo->control != OFF_SW)		// 空気（湿度）の系統
		{
			if (Hcload->wetmode == 'y' ) // && Hcload->chmode == COOLING_SW )	// 湿りコイルで冷房モードの時
			{
				Eo->coeffo = 1.0;
				Eo->Co = f0;
				cfin = Eo->coeffin;
				*cfin = 0.0;
				cfin++ ;
				*cfin = -f1;
			}
			else															// 乾きコイル若しくは暖房モードの時
			{
				// Satoh Debug 2010/10/25
				Eo->coeffo = Hcload->Ga ;
				Eo->Co = 0.0 ;
				cfin = Eo->coeffin ;
				*cfin = -Hcload->Ga ;
				cfin++ ;
				*cfin = 0.0;
				//*cfin = 0.0 ;

				//Eo->coeffo = 1.0e5 ;
				//Eo->Co = 0.0;
				//*(Eo->coeffin) = - 1.0e5 ;

				// Satoh DEBUG  BUG???  2001/8/10
				//cfin++ ;
				//*cfin = 0.0;
			}
		}
		
		if (Hcload->type == 'W')
		{
			Eo++;
			if (Eo->control != OFF_SW)	// 水の系統
			{
				Hcload->Gw = Eo->G;
				Hcload->cGw = spcheat(Eo->fluid) * Hcload->Gw;
				rGa = ro * Hcload->Ga;
				
				Eo->coeffo = Hcload->cGw;
				
				cfin = Eo->coeffin;
				*cfin = -Hcload->cGw;
				cfin++ ;
				*cfin = -Hcload->cGa;
				cfin++ ;
				*cfin = -rGa;
				cfin++ ;
				
				if (Hcload->wetmode == 'y' && Hcload->chmode == COOLING_SW )
				{
					*cfin = Hcload->cGa + rGa * f1;
					cfin++ ;
					*cfin = 0.0;
					Eo->Co = -rGa * f0;
				}
				else
				{
					*cfin = Hcload->cGa;
					cfin++ ;
					*cfin = rGa;
					Eo->Co = 0.0;
				}
			}
		}
		
	}
}
/* ------------------------------------------ */

/* 空調負荷の計算 */ 

void Hcldene(int Nhcload, HCLOAD *Hcload, int *LDrest, WDAT *Wd)
{
	int i, rest ;
	ELOUT *elo;
	extern double ro, ca, cv;

	rest = 0 ;
	
	for (i = 0; i < Nhcload; i++, Hcload++)
	{
		elo = Hcload->cmp->elouts;
		Hcload->Tain = elo->elins->sysvin;
		elo++;
		Hcload->xain = elo->elins->sysvin;
		Hcload->Qfusoku = 0. ;
//		Hcload->Ga = 0.0 ;
//		Hcload->Gw = 0.0 ;

		if (Hcload->type == 'W')
		{
			elo++;
			
			if (elo->elins->upv != NULL)
			{
				Hcload->Twin = elo->elins->upv->sysv;
				Hcload->Twout = elo->sysv ;
			}
			else
			{
				Hcload->Twin = -999.0 ;
				Hcload->Twout = -999.0 ;
			}
		}
		
		if (Hcload->cmp->control != OFF_SW)
		{
			elo = Hcload->cmp->elouts;
			if (elo->control == ON_SW && elo->sysld == 'y')
			{
				Hcload->Qs = elo->load;
				
				/*******************************
				rest = chswreset ( Hcload->Qs, Hcload->chmode, elo ) ;
				/********************************/
				
				/***************************
				rest = chhcldswreset ( Hcload->Qs, Hcload->Ql, Hcload->chmode, elo ) ;
				****************************/
				
				/*************************************
				if ( rest )
				{
					(*LDrest)++ ;
					Hcload->cmp->control = OFF_SW ;
				}
				/*****************************************/
			}
			else
				Hcload->Qs = Hcload->cGa * (elo->sysv - Hcload->Tain);

			rest = chswreset ( Hcload->Qs, Hcload->chmode, elo ) ;

			if ( rest )
			{
				(*LDrest)++ ;
				Hcload->cmp->control = OFF_SW ;
			}   
			
			elo++;
			if (elo->control == ON_SW && elo->sysld == 'y')
				Hcload->Ql = ro * elo->load;
			else
				Hcload->Ql = ro * Hcload->Ga * (elo->sysv - Hcload->xain);
			
			if ( chqlreset ( Hcload ))
				(*LDrest)++ ;

			Hcload->Qt = Hcload->Qs + Hcload->Ql;	 

			// エアコンの消費電力の計算（事業主基準モデル）
			if ( Hcload->RMACFlg == 'Y' )
			{
				if ( Hcload->Qt > 0. )		// 暖房運転
				{
					double qrhmax, qrhd, Temp ;
					double To_7 ;
					double Cafh, Cdf ;
					double Qhmax, Qhd ;
					double fht1, fht2, fht3 ;
					double fhtlb(double T, double x ) ;
					double Eff ;

					//Wd->T = 7. ;
					To_7 = Wd->T - 7. ;
					//Hcload->Qt = 4000. ;
					Temp = ( Hcload->rh - 1. ) / 1.8 ;
					Cafh = 0.8 ;			// 吹出風量補正係数
					//Cafh = 1.0 ;

					qrhmax = -1.e-6 * ( 1. + Temp ) * pow(To_7, 3.)
						+ 2.e-4 * ( 1. + Temp ) * To_7 * To_7
						+ ( 0.0134 + ( 0.0457 - 0.0134 ) * Temp ) * To_7 + Hcload->rh ;

					// デフロスト判定
					Cdf = 1.0 ;
					if ( Wd->T < 5. && Wd->RH >= 80. )
						Cdf = 0.9 ;

					// 最大処理暖房負荷
					Qhmax = qrhmax * Hcload->Qh * Cafh * Cdf ;
					
					// エアコン処理熱量
					if ( Qhmax > Hcload->Qt )
						Qhd = Hcload->Qt ;
					else	// 能力不足
					{
						Qhd = Qhmax ;
						Hcload->Qfusoku = Hcload->Qt - Qhmax ;
					}
					qrhd = Qhd / Hcload->Qh / Cafh / Cdf ;
					// 入力の外気温補正
					fht1 = fhtlb(Wd->T, 1. ) ;
					fht2 = fhtlb(Wd->T, qrhd * 1.9 / Hcload->rh) ;
					fht3 = fhtlb(Wd->T, 1.9 / Hcload->rh ) ;

					// 暖房消費電力の計算
					Eff = 1. / Cafh / Cdf * fht1 * fht2 / qrhd / fht3 ;
					Hcload->Ele = Eff / Hcload->COPh * Qhd ;
					Hcload->COP = Qhd / Hcload->Ele ;

					// 本計算結果のCOPを用いて未処理分を想定した際の消費電力を計算する
					Hcload->Ele = Hcload->Qt / Hcload->COP ;
				}
				else						// 冷房運転
				{
					double qrcmax, qrcd ;
					double To_35 ;
					double Cafc, Chm ;
					double Qcmax, Qcd ;
					double fct1, fct2, fct3 ;
					double fctlb(double T, double x ) ;

					//Wd->T = 35 ;
					To_35 = Wd->T - 35. ;
					Cafc = 0.85 ;			// 風量補正係数
					Chm = 1.0 ;				// 吸込湿度補正係数
					qrcmax = -1.e-5 * Hcload->rc * pow(To_35, 3.) + 2.e-4 * 0.5 * ( 1. + Hcload->rc) * pow(To_35, 2.)
						- ( 0.0147 + 0.014 * ( Hcload->rc - 1.)) * To_35 + Hcload->rc ;
					// 最大処理冷房負荷
					Qcmax = qrcmax * Hcload->Qc * ( Cafc + Chm ) / 2. ;

					// エアコン処理熱量
					if ( Qcmax < Hcload->Qt )
						Qcd = Hcload->Qt ;
					else	// 能力不足
					{
						Qcd = Qcmax ;
						Hcload->Qfusoku = Hcload->Qt - Qcmax ;
					}
					qrcd = Qcd / Hcload->Qc / (( Cafc + Chm ) / 2.) ;
					// 入力の外気温補正
					fct1 = fctlb(Wd->T, 1. ) ;
					fct2 = fctlb(Wd->T, qrcd * 1.5 / Hcload->rc) ;
					fct3 = fctlb(Wd->T, 1.5 / Hcload->rc ) ;

					// 冷房消費電力の計算
					Hcload->Ele = 1. / (( Cafc + Chm ) / 2.) * fct1 * fct2 / qrcd / fct3 / Hcload->COPc * Qcd ;
					Hcload->COP = Qcd / Hcload->Ele ;

					// 本計算結果のCOPを用いて未処理分を想定した際の消費電力を計算する
					Hcload->Ele = - Hcload->Qt / Hcload->COP ;
				}
			}
			// エアコンの消費電力の計算（電中研モデル）
			else if(Hcload->RMACFlg == 'y')
			{
				// 冷房時
				if(Hcload->Qt < 0.)
				{
					double	effth, Tevp, Tcnd, cai, cao, COP, COPd, Qc[3], R ;
					int		i, j ;

					// 湿り比熱の計算
					cao = ca + cv * Wd->x ;
					cai = ca + cv * Hcload->xain ;
					Tevp = FNTevpc(Hcload->Tain, Hcload->Qt, cai, (1. - Hcload->BFi) * Hcload->Ga, Hcload->xain) ;

					// COPの収束計算
					// 初期値は定格COP
					COPd = Hcload->COPc ;

					Qc[0] = Hcload->Qt * Hcload->Qt ;
					Qc[1] = - Hcload->Qt ;
					Qc[2] = 1.0 ;

					// 理論効率と実働効率の比Rの計算
					R = 0. ;
					for(j = 0; j < 3; j++)
						R += Hcload->Rc[j] * Qc[j] ;

					// 収束計算の上限は100回
					for(i = 0; i < 100; i++)
					{
						double	E ;

						// 消費電力の計算
						E = - Hcload->Qt / COPd ;
						
						// 凝縮温度の計算
						Tcnd = FNTcndc(Wd->T, Hcload->Qt, E, cao, (1. - Hcload->BFo) * Hcload->Go) ;

						// 理論効率の計算
						effth = (Tevp + 273.15) / (Tcnd - Tevp) ;

						// COPの計算
						COP = 1. / (1. / (R * effth) + Hcload->Pcc / -Hcload->Qt) ;

						// 収束判定
						if(fabs(COP - COPd) < 1.e-4)
						{
							Hcload->Ele = E ;
							Hcload->COP = COP ;
							break ;
						}
						else
							COPd = COP ;
					}
				}

				// 暖房時
				else
				{
					double	effth, Tevp, Tcnd, cai, cao, COP, COPd, Qc[3], R ;
					int		i, j ;

					// 湿り比熱の計算
					cao = ca + cv * Wd->x ;
					cai = ca + cv * Hcload->xain ;
					Tcnd = FNTcndh(Hcload->Tain, Hcload->Qt, cai, (1. - Hcload->BFi) * Hcload->Ga) ;

					// COPの収束計算
					// 初期値は定格COP
					COPd = Hcload->COPh ;

					Qc[0] = Hcload->Qt * Hcload->Qt ;
					Qc[1] = Hcload->Qt ;
					Qc[2] = 1.0 ;

					// 理論効率と実働効率の比Rの計算
					R = 0. ;
					for(j = 0; j < 3; j++)
						R += Hcload->Rh[j] * Qc[j] ;

					// 収束計算の上限は100回
					for(i = 0; i < 100; i++)
					{
						double	E ;

						// 消費電力の計算
						E = Hcload->Qt / COPd ;
						
						// 蒸発温度の計算
						Tevp = FNTevph(Wd->T, Hcload->Qt, E, cao, (1. - Hcload->BFo) * Hcload->Go, Wd->x) ;

						// 理論効率の計算
						effth = (Tcnd + 273.15) / (Tcnd - Tevp) ;

						// COPの計算
						COP = 1. / (1. / (R * effth) + Hcload->Pch / Hcload->Qt) ;

						// 収束判定
						if(fabs(COP - COPd) < 1.e-4)
						{
							Hcload->Ele = E ;
							Hcload->COP = COP ;
							break ;
						}
						else
							COPd = COP ;
					}
				}
			}
		}
		else
			Hcload->Qs = Hcload->Ql = Hcload->Qt = Hcload->Ele = Hcload->COP = Hcload->Qfusoku = 0.0;
	}
}

double fctlb(double T, double x)
{
	// satoh debug 2010/11/06
	// 事業主基準の計算根拠のドキュメントが修正されたため
	//double a[5], *ap ;
	double a[4], *ap ;
	double Temp ;
	long i ;

	a[3] = 0.0148 * T + 0.0089 ;
	a[2] = - 0.0153 * T + 0.1429 ;
	a[1] = 0.034 * T - 0.4963 ;
	a[0] = - 0.0012 * T + 0.288 + 0.0322 ;
	
	Temp = 0. ;
	ap = a ;
	for ( i = 0; i < 4; i++, ap++ )
		Temp += *ap * pow(x, (double)i) ;

	return Temp ;
}

double fhtlb(double T, double x )
{
	double a[5], *ap ;
	double Temp ;
	double T2 ;
	long i ;
	
	T2 = T * T ;
	//a[4] = 0.006 * T2 - 0.022 * T + 0.3051 ;
	//a[3] = - 0.0021 * T2 + 0.0757 * T - 1.0448 ;
	//a[2] = 0.0026 * T2 - 0.0981 * T + 1.6971 ;
	//a[1] = - 0.0011 * T2 + 0.0222 * T - 0.1444 ;
	//a[0] = 0.002 * T2 - 0.0069 * T + 0.3487 - 0.074 ;

	a[4] = 0.0018 * T2 - 0.0424 * T + 0.4554 ;
	a[3] = - 0.006 * T2 + 0.1347 * T - 1.5600 ;
	a[2] = 0.0063 * T2 - 0.1406 * T + 2.2902 ;
	a[1] = - 0.002 * T2 + 0.0176 * T - 0.3789 ;
	a[0] = 0.0002 * T2 - 0.0007 * T + 0.4202 ;

	Temp = 0. ;
	ap = a ;
	for ( i = 0; i < 5; i++, ap++ )
		Temp += ((*ap) * pow(x, (double)i)) ;

	return Temp ;
}

/* --------------------------- */

/* 負荷計算指定時の設定値のポインター */

int hcldptr(char *load, char **key, HCLOAD *Hcload, VPTR *vptr, char *idmrk)
{
	int err = 0;
	
	if (strcmp(key[1], "Tout") == 0 || strcmp(key[1], "Tr") == 0 || strcmp(key[1], "Tot") == 0)
	{
		vptr->ptr = &Hcload->Toset;
		vptr->type = VAL_CTYPE;
		Hcload->loadt = load;
		*idmrk = 't';
	}
	else if (strcmp(key[1], "xout") == 0)
	{
		vptr->ptr = &Hcload->Xoset;
		vptr->type = VAL_CTYPE;
		Hcload->loadx = load;
		*idmrk = 'x';
	}
	else
		err = 1;
	return err;
}  
/* ------------------------------------------ */


/* 負荷計算指定時のスケジュール設定 */

void hcldschd(HCLOAD *Hcload)
{
	ELOUT *Eo;
	
	Eo = Hcload->cmp->elouts; 
	
	if (Hcload->loadt != NULL)
	{
		if (Eo->control != OFF_SW)
		{
			if (Hcload->Toset > TEMPLIMIT)
			{
				Eo->control = LOAD_SW;
				Eo->sysv = Hcload->Toset;
			}
			else
			{
				Eo->control = OFF_SW;

				if (Hcload->wetmode == 'y' )
					(Eo+1)->control = OFF_SW ;
			}
		}
	}
	else if (Hcload->loadx != NULL)
	{
		Eo++; 
		if (Eo->control != OFF_SW)
		{
			if (Hcload->Xoset > 0.0)
			{
				Eo->control = LOAD_SW;
				Eo->sysv = Hcload->Xoset;
			}
			else
				Eo->control = OFF_SW;
		}	 
	}
	
	Eo = Hcload->cmp->elouts;
	if (Hcload->type == 'W')
	{
		if (Eo->control == OFF_SW && (Eo + 1)->control == OFF_SW)
			(Eo + 2)->control = OFF_SW ;
	}
}

/* ------------------------------------------ */

void hcldprint(FILE *fo, int id, int Nhcload, HCLOAD *Hcload)
{
	int i;
	ELOUT *el;
	double Taout;
	double  RHout;
	
	switch (id)
	{
	case 0:
		if (Nhcload >0)
            fprintf(fo, "%s %d\n", HCLOAD_TYPE, Nhcload);
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
			if (Hcload->type == 'W')
				//fprintf(fo, " %s 1 13\n", Hcload->name);
				fprintf(fo, " %s 1 15\n", Hcload->name);
			else
			{
				//fprintf(fo, " %s 1 10\n", Hcload->name);
				if ( Hcload->RMACFlg == 'Y' || Hcload->RMACFlg == 'y')
					fprintf(fo, " %s 1 14\n", Hcload->name) ;
				else
					fprintf(fo, " %s 1 11\n", Hcload->name);
			}
		}
		break;
		
	case 1:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
			
			fprintf(fo, "%s_ca c c %s_Ga m f %s_Ti t f %s_To t f %s_Qs q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_cx c c %s_xi x f %s_xo x f %s_RHo r f %s_Ql q f",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			if (Hcload->type == 'W')
				fprintf(fo, "%s_cw c c %s_G m f %s_Twi t f %s_Two t f",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
			fprintf(fo, " %s_Qt q f\n", Hcload->name);

			if (Hcload->RMACFlg == 'Y' || Hcload->RMACFlg == 'y')
				fprintf(fo, " %s_Qfusoku q f %s_Ele E f %s_COP C f\n",
				Hcload->name, Hcload->name, Hcload->name);
		}
		break;
		
	default:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
            el = Hcload->cmp->elouts;
			Taout = el->sysv;            
			fprintf(fo, "%c %6.4g %4.2lf %4.2lf %.2lf ", 
				el->control, Hcload->Ga, Hcload->Tain,
				el->sysv, Hcload->Qs);
			
            el++;
			RHout = FNRhtx(Taout, (double)el->sysv);
			if (RHout > 100.0 ) 
				RHout = 999;
			else if (RHout < 0.0)
				RHout = -99.0 ;
			
            fprintf(fo, "%c %.4lf %.4lf %3.0lf %.2lf ",
				el->control, Hcload->xain, el->sysv, RHout, Hcload->Ql);
            el++;
			if (Hcload->type == 'W')
				fprintf(fo, "%c %6.4g %4.1lf %4.1lf",
				el->control, Hcload->Gw, Hcload->Twin, el->sysv);	
            fprintf(fo, " %.2lf\n", Hcload->Qt);

			if (Hcload->RMACFlg == 'Y' || Hcload->RMACFlg == 'y')
				fprintf(fo, " %.0lf %.0lf %.2lf\n", Hcload->Qfusoku, Hcload->Ele, Hcload->COP) ;
		}
		break;
	}
} 

/* ------------------------------ */

/* 日積算値に関する処理 */   

void hclddyint(int Nhcload, HCLOAD *Hcload)
{
	int i;
	
	for (i = 0; i < Nhcload; i++, Hcload++)
	{
		svdyint(&Hcload->Taidy);
		svdyint(&Hcload->xaidy);
		
		qdyint(&Hcload->Qdys);
		qdyint(&Hcload->Qdyl);
		qdyint(&Hcload->Qdyt);
		qdyint(&Hcload->Qdyfusoku);
		qdyint(&Hcload->Edy);
	}
}

void hcldmonint(int Nhcload, HCLOAD *Hcload)
{
	int i;
	
	for (i = 0; i < Nhcload; i++, Hcload++)
	{
		svdyint(&Hcload->mTaidy);
		svdyint(&Hcload->mxaidy);
		
		qdyint(&Hcload->mQdys);
		qdyint(&Hcload->mQdyl);
		qdyint(&Hcload->mQdyt);
		qdyint(&Hcload->mQdyfusoku);
		qdyint(&Hcload->mEdy);
	}
}

void hcldday(int Mon, int Day, int ttmm, int Nhcload, HCLOAD *Hcload, int Nday, int SimDayend)
{
	int i;
	int Mo, tt ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	for (i = 0; i < Nhcload; i++, Hcload++)
	{ 
		// 日集計
		svdaysum(ttmm, Hcload->cmp->control, Hcload->Tain, &Hcload->Taidy);
		svdaysum(ttmm, Hcload->cmp->control, Hcload->xain, &Hcload->xaidy);
		
		qdaysum(ttmm, Hcload->cmp->control, Hcload->Qs, &Hcload->Qdys);
		qdaysum(ttmm, Hcload->cmp->control, Hcload->Ql, &Hcload->Qdyl);
		qdaysum(ttmm, Hcload->cmp->control, Hcload->Qt, &Hcload->Qdyt);
		
		qdaysum(ttmm, Hcload->cmp->control, Hcload->Qfusoku, &Hcload->Qdyfusoku);
		qdaysum(ttmm, Hcload->cmp->control, Hcload->Ele, &Hcload->Edy);
		
		// 月集計
		svmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Tain, &Hcload->mTaidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->xain, &Hcload->mxaidy, Nday, SimDayend);
		
		qmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Qs, &Hcload->mQdys, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Ql, &Hcload->mQdyl, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Qt, &Hcload->mQdyt, Nday, SimDayend);
		
		qmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Qfusoku, &Hcload->mQdyfusoku, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Ele, &Hcload->mEdy, Nday, SimDayend);

		// 月・時刻のクロス集計
		emtsum(Mon, Day, ttmm, Hcload->cmp->control, Hcload->Ele, &Hcload->mtEdy[Mo][tt]);
	}
}

void hclddyprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nhcload >0)         
            fprintf(fo, "%s %d\n", HCLOAD_TYPE, Nhcload);
		for (i = 0; i < Nhcload; i++, Hcload++)
			fprintf(fo, "%s 4 36 14 14 8\n", Hcload->name);
		break;
		
	case 1:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
			fprintf(fo,"%s_Ht H d %s_T T f ", Hcload->name, Hcload->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);	 
			fprintf(fo, "%s_Hsh H d %s_Qsh Q f %s_Hsc H d %s_Qsc Q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_tsh h d %s_qsh q f %s_tsc h d %s_qsc q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
			fprintf(fo,"%s_Hx H d %s_x X f ", Hcload->name, Hcload->name);
			fprintf(fo,"%s_txn h d %s_xn x f %s_txm h d %s_xm c f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);	 
			fprintf(fo, "%s_Hlh H d %s_Qlh Q f %s_Hlc H d %s_Qlc Q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_tlh h d %s_qlh q f %s_tlc h d %s_qlc q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
			fprintf(fo, "%s_Hth H d %s_Qth Q f %s_Htc H d %s_Qtc Q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_tth h d %s_qth q f %s_ttc h d %s_qtc q f\n\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
		}
		break;
		
	default:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Hcload->Taidy.hrs, Hcload->Taidy.m, 
				Hcload->Taidy.mntime, Hcload->Taidy.mn,
				Hcload->Taidy.mxtime, Hcload->Taidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Hcload->Qdys.hhr, Hcload->Qdys.h);
            fprintf(fo, "%1ld %3.1lf ", Hcload->Qdys.chr, Hcload->Qdys.c);
            fprintf(fo, "%1ld %2.0lf ", Hcload->Qdys.hmxtime, Hcload->Qdys.hmx);
            fprintf(fo, "%1ld %2.0lf ", Hcload->Qdys.cmxtime, Hcload->Qdys.cmx);
			
            fprintf(fo, "%1ld %5.4lf %1ld %5.4lf %1ld %5.4lf ",
				Hcload->xaidy.hrs, Hcload->xaidy.m, 
				Hcload->xaidy.mntime, Hcload->xaidy.mn, 
				Hcload->xaidy.mxtime, Hcload->xaidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Hcload->Qdyl.hhr, Hcload->Qdyl.h);
            fprintf(fo, "%1ld %3.1lf ", Hcload->Qdyl.chr, Hcload->Qdyl.c);
            fprintf(fo, "%1ld %2.0lf ", Hcload->Qdyl.hmxtime, Hcload->Qdyl.hmx);
            fprintf(fo, "%1ld %2.0lf ", Hcload->Qdyl.cmxtime, Hcload->Qdyl.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Hcload->Qdyt.hhr, Hcload->Qdyt.h);
            fprintf(fo, "%1ld %3.1lf ", Hcload->Qdyt.chr, Hcload->Qdyt.c);
            fprintf(fo, "%1ld %2.0lf ", Hcload->Qdyt.hmxtime, Hcload->Qdyt.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Hcload->Qdyt.cmxtime, Hcload->Qdyt.cmx);  
		}
		break;
	}
}

void hcldmonprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nhcload >0)         
            fprintf(fo, "%s %d\n", HCLOAD_TYPE, Nhcload);
		for (i = 0; i < Nhcload; i++, Hcload++)
			fprintf(fo, "%s 4 36 14 14 8\n", Hcload->name);
		break;
		
	case 1:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
			fprintf(fo,"%s_Ht H d %s_T T f ", Hcload->name, Hcload->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);	 
			fprintf(fo, "%s_Hsh H d %s_Qsh Q f %s_Hsc H d %s_Qsc Q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_tsh h d %s_qsh q f %s_tsc h d %s_qsc q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
			fprintf(fo,"%s_Hx H d %s_x X f ", Hcload->name, Hcload->name);
			fprintf(fo,"%s_txn h d %s_xn x f %s_txm h d %s_xm c f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);	 
			fprintf(fo, "%s_Hlh H d %s_Qlh Q f %s_Hlc H d %s_Qlc Q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_tlh h d %s_qlh q f %s_tlc h d %s_qlc q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
			fprintf(fo, "%s_Hth H d %s_Qth Q f %s_Htc H d %s_Qtc Q f\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			fprintf(fo, "%s_tth h d %s_qth q f %s_ttc h d %s_qtc q f\n\n",
				Hcload->name, Hcload->name, Hcload->name, Hcload->name);
			
		}
		break;
		
	default:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Hcload->mTaidy.hrs, Hcload->mTaidy.m, 
				Hcload->mTaidy.mntime, Hcload->mTaidy.mn,
				Hcload->mTaidy.mxtime, Hcload->mTaidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Hcload->mQdys.hhr, Hcload->mQdys.h);
            fprintf(fo, "%1ld %3.1lf ", Hcload->mQdys.chr, Hcload->mQdys.c);
            fprintf(fo, "%1ld %2.0lf ", Hcload->mQdys.hmxtime, Hcload->mQdys.hmx);
            fprintf(fo, "%1ld %2.0lf ", Hcload->mQdys.cmxtime, Hcload->mQdys.cmx);
			
            fprintf(fo, "%1ld %5.4lf %1ld %5.4lf %1ld %5.4lf ",
				Hcload->mxaidy.hrs, Hcload->mxaidy.m, 
				Hcload->mxaidy.mntime, Hcload->mxaidy.mn, 
				Hcload->mxaidy.mxtime, Hcload->mxaidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Hcload->mQdyl.hhr, Hcload->mQdyl.h);
            fprintf(fo, "%1ld %3.1lf ", Hcload->mQdyl.chr, Hcload->mQdyl.c);
            fprintf(fo, "%1ld %2.0lf ", Hcload->mQdyl.hmxtime, Hcload->mQdyl.hmx);
            fprintf(fo, "%1ld %2.0lf ", Hcload->mQdyl.cmxtime, Hcload->mQdyl.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Hcload->mQdyt.hhr, Hcload->mQdyt.h);
            fprintf(fo, "%1ld %3.1lf ", Hcload->mQdyt.chr, Hcload->mQdyt.c);
            fprintf(fo, "%1ld %2.0lf ", Hcload->mQdyt.hmxtime, Hcload->mQdyt.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Hcload->mQdyt.cmxtime, Hcload->mQdyt.cmx);  
		}
		break;
	}
}

void hcldmtprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload, int Mo, int tt)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nhcload > 0)
            fprintf(fo, "%s %d\n", HCLOAD_TYPE, Nhcload);
		for (i = 0; i < Nhcload; i++, Hcload++)
			fprintf(fo, " %s 1 1\n", Hcload->name);
		break;
		
	case 1:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{	 
			fprintf(fo,"%s_E E f \n", Hcload->name);
		}
		break;
		
	default:
		for (i = 0; i < Nhcload; i++, Hcload++)
		{
			fprintf(fo, " %.2lf \n", Hcload->mtEdy[Mo-1][tt-1].d * cff_kWh);
		}
		break;
	}
}

int	hcldswptr (char **key, HCLOAD *Hcload, VPTR *vptr)
{
	int	err = 0 ;
	
	if ( strcmp (key[1], "chmode" ) == 0 )
	{
		vptr->ptr = &Hcload->chmode ;
		vptr->type = SW_CTYPE ;
	}
	else
		err = 1 ;
	
	return err ;
}

int chhcldswreset(double Qload, double Ql, char chmode, ELOUT *Eo)
{
	if ((chmode == HEATING_SW && Qload < 0.0) ||
		(chmode == COOLING_SW && Ql > 0.0) ||
		(chmode == COOLING_SW && Qload > 0.0))
	{
		Eo->control = ON_SW;
		Eo->sysld = 'n';
		Eo->emonitr->control = ON_SW;
		
		return 1;
	}
	else
	{
		return 0;
	}
}

void	hcldwetmdreset ( EQSYS *Eqsys )
{
	HCLOAD	*Hcload ;
	int		Nhcload, i ;

	Hcload = Eqsys->Hcload ;
	Nhcload = Eqsys->Nhcload ;

	for ( i = 0; i < Nhcload; i++, Hcload++ )
		Hcload->wetmode = Hcload->wet ;
}
