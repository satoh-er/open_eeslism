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

/*   bl_wall.c   */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MODEL.h" /*---------higuchi 070918------------*/
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"
#include "fnfio.h"

/* ------------------------------------------ */

/*  壁体後退差分計算準備   */

void Walli(int Nbm, BMLST *W, WALL *Wl, PCM *pcm, int Npcm)
{
	int     i, j, k, m, N, M;
	double  Rwall, *C, *Rw, CAPwall;
	BMLST	*BM;
	WELM	*Welm;
	PCM		*PCMcat = NULL;
	char	E[SCHAR], *st, *s, *PCMcode;
	double	*PCMrate;
	//int	Nc ;

	Wl->R = NULL;
	Wl->CAP = NULL;
	BM = NULL;

	Rwall = 0.0;
	CAPwall = 0.0;

	//printf ( "<Walli> Wl->N=%d\n", Wl->N ) ;
	//for ( i=0; i<Nbm; i++, W++ )
	//	printf("i=%d code=%s\n", i, W->mcode ) ;

	Wl->R = dcalloc(Wl->N, "<Walli R>");
	Wl->CAP = dcalloc(Wl->N, "<Walli CAP>");

	Welm = Wl->welm;
	Rw = Wl->R;
	C = Wl->CAP;
	//PCMc = *(Wl->PCM) ;
	PCMrate = Wl->PCMrate;
	for (i = 0; i < Wl->N; i++, Welm++, Rw++, C++, PCMrate++)
	{
		//	printf ( "<Walli> ----- Welm->code=%s\n", Welm->code ) ;

			// PCM内蔵建材かどうかをチェックする
		if ((st = strchr(Welm->code, '(')) != NULL)
		{
			// フォーマット　　code(PCMcode_VolRate)
			PCMcode = stralloc(st + 1);

			// PCMフラグ
			Wl->PCMflg = 'Y';

			// codeのコピー
			*st = '\0';
			s = stralloc(Welm->code);
			free(Welm->code);
			Welm->code = stralloc(s);			// 基材のコード名のコピー
			free(s);

			st = strchr(PCMcode, '_');
			*st = '\0';
			s = stralloc(PCMcode);				// PCM名称
			*PCMrate = atof(st + 1);			// PCM含有率（Vol）

			// PCMの検索
			PCMcat = pcm;
			for (k = 0; k < Npcm; k++, PCMcat++)
			{
				if (strcmp(s, PCMcat->name) == 0)
				{
					Wl->PCM[i] = PCMcat;
					break;
				}
			}
		}
		BM = W;
		for (k = 0; k < Nbm; k++, BM++)
		{
			//printf ( "\t\tBelm->code=%s\n", BM->mcode ) ;

			if (BM->mcode != NULL && strcmp(Welm->code, BM->mcode) == 0)
				break;
		}

		if (k == Nbm)
		{
			sprintf_s(E, sizeof(E), "%s", Welm->code);
			Eprint("<Walli>", E);
		}

		//printf ( "name=%s code=%s L=%g\n", Wl->name, Welm->code, Welm->L ) ;

		// 熱伝導率、容積比熱のコピー
		Welm->Cond = BM->Cond;			// 熱伝導率［W/mK］
		Welm->Cro = BM->Cro * 1000.;	// 容積比熱［J/m3K］
		if (Welm->L > 0.)
			*Rw = Welm->L / BM->Cond;	// Rw：熱抵抗[m2K/W]、L：厚さ［m］
		else
			*Rw = 1.0 / BM->Cond;

		*C = BM->Cro * 1000. * Welm->L;	// 熱容量［J/m2K］

		if (strcmp(BM->mcode, "ali") != 0 && strcmp(BM->mcode, "alo") != 0)
		{
			Rwall += *Rw;
			CAPwall += *C;
		}
	}

	N = i;
	Wl->Rwall = Rwall;
	Wl->CAPwall = CAPwall;
	//	Wl->cap = dcalloc (( unsigned int) Wl->ND[i]+1, "<Walli> Wl->cap" ) ;

	m = 0;
	C = Wl->CAP;
	Rw = Wl->R;
	Welm = Wl->welm;
	M = 0;

	for (i = 0; i < N; i++, Welm++)
	{
		for (j = 0; j <= Welm->ND; j++)
			M++;
	}

	Wl->res = dcalloc(M + 2, "<Walli> Wall->res");
	Wl->cap = dcalloc(M + 2, "<Walli> Wall->cap");
	Wl->L = dcalloc(M + 2, "<Walli> Wall->cap");

	// PCM構造体へのポインタのメモリ確保
	if ((Wl->PCMLyr = (PCM **)malloc((M + 2) * sizeof(PCM *))) == NULL)
		Ercalloc(M + 2, "<Walli> Wall->PCMLyr  alloc");
	for (i = 0; i < M + 2; i++)
		Wl->PCMLyr[i] = NULL;

	Wl->PCMrateLyr = dcalloc(M + 2, "<Walli> Wall->PCMrateLyr");

	Welm = Wl->welm;
	//PCMc = *(Wl->PCM) ;
	PCMrate = Wl->PCMrate;
	//printf("ble=%c name=%s\n", Wl->ble, Wl->name);
	for (i = 0; i < N; i++, C++, Rw++, Welm++, PCMrate++)
	{
		for (j = 0; j <= Welm->ND; j++, m++)
		{
			//			if ( drealloc( &Wl->res, m, m + 1 ) == 0 )
			//				printf ( "<Walli> Wl->res メモリ領域の最確保に失敗\n" ) ;

			//			if ( drealloc( &Wl->cap, m, m + 1 ) == 0 )
			//				printf ( "<Walli> Wl->cap メモリ領域の最確保に失敗\n" ) ;

			Wl->res[m] = *Rw / ((double)Welm->ND + 1.);
			Wl->cap[m] = *C / ((double)Welm->ND + 1.);
			if (Welm->L > 0.)
				Wl->L[m] = Welm->L / ((double)Welm->ND + 1.);
			if (Wl->PCM[i] != NULL)
			{
				Wl->PCMLyr[m] = Wl->PCM[i];
				Wl->PCMrateLyr[m] = *PCMrate;
			}
		}
	}
	Wl->M = m - 1;

	/* ========================================*

	 printf("-- Walli  N=%d\n",N);
	 for (i=0; i < N; i++)
	 printf("  %d   R=%lf    CAP=%lf\n", i, R[i], CAP[i]);
	 printf("  M=%d\n", Wl->M);

	/*========================================= */

	if (Wl->Ip > 0)
	{
		Wl->mp = Wl->Ip;
		Welm = Wl->welm;
		Welm++;
		for (i = 1; i <= Wl->Ip; i++, Welm++)
			Wl->mp += Welm->ND;
	}
	else
		Wl->mp = -1;

	//free ( R ) ;
	//free ( CAP ) ;
}
/* ------------------------------------------------- */

/*  壁体後退差分計算用係数   */


void Wallfdc(int M, int mp, double *res, double *cap, double Wp, double *UX,
	double *uo, double *um, double *Pc, char WallType, RMSRF *Sd, WDAT *Wd,
	EXSFS *Exsf, WALL *Wall, double *Told, double *Twd, PCMSTATE *pcmstate)
{
	extern double   dTM;
	double  *Ul, *Ur, C;
	int     m, j;
	double	*PCMrate;
	PCM		*PCM, *PCM1;
	WELM	*Welm;
	double	capm, capm1, resm, resm1;
	double	T, lamda, Toldn;
	double	*captempL, *captempR;
	int		PCMf = 0;
	double	Croa;				// 見かけの比熱
	double	ToldPCMave, ToldPCMNodeL, ToldPCMNodeR;

	Ul = dcalloc(M, "<Wallfdc Ul>");
	Ur = dcalloc(M, "<Wallfdc Ur>");
	captempL = dcalloc(M + 1, "<Wallfdc captemp>");
	captempR = dcalloc(M + 1, "<Wallfdc captemp>");

	// PCM体積含有率
	PCMrate = Wall->PCMrateLyr;
	// PCM構造体
	PCM = *(Wall->PCMLyr);
	PCM1 = PCM + 1;

	// 層構成
	Welm = Wall->welm;
	for (m = 0; m <= M; m++)
	{
		// PCM内蔵床暖房の計算に活用するためcapをコピーして保持
		captempL[m] = cap[m];
		captempR[m] = cap[m + 1];
	}

	for (m = 0; m < M; m++, PCMrate++, Welm++, pcmstate++)
	{
		//printf("<Wallfdc> m=%d Twd=%lf Told=%lf\n", m, Twd[m], Told[m]);
		capm = capm1 = resm = resm1 = 0.0;
		PCM = Wall->PCMLyr[m];
		PCM1 = Wall->PCMLyr[m + 1];

		// PCMなしの層
		if (PCM == NULL && PCM1 == NULL)
		{
			C = 0.5*(cap[m] + cap[m + 1]);
			Ul[m] = dTM / (C * res[m]);
			Ur[m] = dTM / (C * res[m + 1]);
		}
		// どちらかにPCMがある場合
		else
		{
			PCMf = 1;
			// 相変化温度を考慮した物性値の計算

			// m点の左にPCMがある場合
			if (PCM != NULL)
			{
				pcmstate->tempPCMave = (Twd[m - 1] + Twd[m]) * 0.5;
				pcmstate->tempPCMNodeL = Twd[m - 1];
				pcmstate->tempPCMNodeR = Twd[m];
				// PCM温度
				if (PCM->AveTemp == 'y')
				{
					T = pcmstate->tempPCMave;
					Toldn = ToldPCMave;
				}
				else
				{
					T = pcmstate->tempPCMNodeR;
					Toldn = ToldPCMNodeR;
				}
				//pcmstate->tempPCM = T;
				// m層の見かけの比熱
				if (PCM->spctype == 'm')
					Croa = FNPCMStatefun(PCM->Ctype, PCM->Cros, PCM->Crol, PCM->Ql, PCM->Ts, PCM->Tl, PCM->Tp, Toldn, T, PCM->DivTemp, PCM->PCMp);
				else
					Croa = FNPCMstate_table(&PCM->chartable[0], Toldn, T, PCM->DivTemp);
				if (Croa < 0.)
					printf("Croa=%lf\n", Croa);
				//printf("NodeR  T=%.2lf Toldn=%.2lf Croa=%.0lf\n", T, Toldn, Croa);
				pcmstate->capmR = Croa;
				//if (pcmstate->capm > PCM->Cros)
				//	printf("PCM 相変化開始\n");
				capm = Croa * Wall->L[m];
				// m層の熱抵抗（見かけの比熱特性Typeはダミー値0）
				if (PCM->condtype == 'm')
					lamda = FNPCMStatefun(0, PCM->Conds, PCM->Condl, 0., PCM->Ts, PCM->Tl, PCM->Tp, Toldn, T, PCM->DivTemp, PCM->PCMp);
				else
					lamda = FNPCMstate_table(&PCM->chartable[1], Toldn, T, PCM->DivTemp);
				pcmstate->oldlamdaR = pcmstate->lamdaR = lamda;
				resm = Wall->L[m] / lamda;
				//pcmstate->lamda = lamda;
			}

			// m点の右にPCMがある場合
			if (PCM1 != NULL)
			{
				(pcmstate + 1)->tempPCMave = (Twd[m] + Twd[m + 1]) * 0.5;
				(pcmstate + 1)->tempPCMNodeL = Twd[m];
				(pcmstate + 1)->tempPCMNodeR = Twd[m + 1];
				ToldPCMave = (Told[m - 1] + Told[m]) * 0.5;
				ToldPCMNodeL = Told[m];
				ToldPCMNodeR = Told[m + 1];
				// PCM温度
				if (PCM1->AveTemp == 'y')
				{
					T = (pcmstate + 1)->tempPCMave;
					Toldn = ToldPCMave;
				}
				else
				{
					T = (pcmstate + 1)->tempPCMNodeL;
					Toldn = ToldPCMNodeL;
				}
				// m層の見かけの比熱
				if (PCM1->spctype == 'm')
					Croa = FNPCMStatefun(PCM1->Ctype, PCM1->Cros, PCM1->Crol, PCM1->Ql, PCM1->Ts, PCM1->Tl, PCM1->Tp, Toldn, T, PCM1->DivTemp, PCM1->PCMp);
				else
					Croa = FNPCMstate_table(&PCM1->chartable[0], Toldn, T, PCM1->DivTemp);
				if (Croa < 0.)
					printf("Croa=%lf\n", Croa);
				//printf("NodeL  T=%.2lf Toldn=%.2lf Croa=%.0lf\n", T, Toldn, Croa);
				(pcmstate + 1)->capmL = Croa;
				capm1 = Croa * Wall->L[m + 1];
				//if (capm1 > PCM1->Cros*Wall->L[m + 1])
				//	printf("PCM1 相変化\n");
				// m層の熱抵抗（見かけの比熱特性Typeはダミー値0）
				if (PCM1->condtype == 'm')
					lamda = FNPCMStatefun(0, PCM1->Conds, PCM1->Condl, 0., PCM1->Ts, PCM1->Tl, PCM1->Tp, Toldn, T, PCM1->DivTemp, PCM1->PCMp);
				else
					lamda = FNPCMstate_table(&PCM1->chartable[1], Toldn, T, PCM1->DivTemp);
				(pcmstate + 1)->oldlamdaL = (pcmstate + 1)->lamdaL = lamda;
				resm1 = Wall->L[m + 1] / lamda;
				//pcmstate->lamda = lamda;
			}

			//printf("capm=%lf capm1=%lf\n", capm, capm1);
			// PCMと基材との含有率による重みづけ平均
			captempL[m] = cap[m] * (1. - *PCMrate) + capm * *PCMrate;
			captempR[m] = cap[m + 1] * (1. - *(PCMrate + 1)) + capm1 * *(PCMrate + 1);
			C = 0.5 * (captempL[m] + captempR[m]);
			//C = 0.5 * (cap[m] * (1. - *PCMrate) + capm * *PCMrate + cap[m + 1] * (1. - *(PCMrate + 1)) + capm1 * *(PCMrate + 1));
			Ul[m] = dTM / (C * (res[m] * (1. - *PCMrate) + resm * *PCMrate));
			Ur[m] = dTM / (C * (res[m + 1] * (1. - *(PCMrate + 1)) + resm1 * *(PCMrate + 1)));

			//printf("m=%d C=%lf Ul[m]=%lf Ur[m]=%lf\n", m, C, Ul[m], Ur[m]);
			//printf("PCMrate=%lf\n", *PCMrate);
		}

		/********************************/
		//printf ( "m=%d C=%lf Ur[m]=%lf Ul[m]=%lf res[m]=%lf\n",m,C, Ur[m],Ul[m],res[m] );
		//printf ( "res[m+1]=%lf Ur[m]=%lf\n",res[m+1],Ur[m] );
		/**********************************/

		/*****
		printf("----- Wallfdc  m=%d cap=%lf res=%lf\n", m, cap[m], res[m]);
		/*****/
	}

	for (m = 0; m < M; m++)
	{
		for (j = 0; j < M; j++)
		{
			UX[m*M + j] = 0.;
		}
	}

	UX[0] = 1.0 + Ul[0] + Ur[0];
	for (m = 1; m < M; m++)
	{
		UX[m*M + m] = 1.0 + Ul[m] + Ur[m];		// 対角要素
		UX[m*M + m - 1] = -Ul[m];				// 対角要素の下
		UX[(m - 1)*M + m] = -Ur[m - 1];			// 対角要素の右
	}
	if (Wp > 0.)
	{
		// 床暖房等放射パネル
		if (WallType == 'P')
		{
			//*Pc = dTM / (0.5*(cap[mp] + cap[mp + 1]));
			*Pc = dTM / (0.5*(captempL[mp] + captempR[mp]));
			UX[mp*M + mp] += Wp * *Pc;
			//printf("Pc=%lf\n", *Pc);
		}
		// 建材一体型空気集熱器の場合
		else
		{
			double ECG, ECt, CFc;
			WALL   *Wall;
			Wall = Sd->mw->wall;

			//*Pc = dTM / (0.5 * cap[mp]);
			*Pc = dTM / (0.5 * captempL[mp]);
			//UX[mp*M + mp] -= ECt * *Pc;

			// 境界条件の計算
			ECG = ECt = CFc = 0.0;
			FNBoundarySolarWall(Sd, &ECG, &ECt, &CFc);

			//// 既存配列の初期化
			//for (m = 0; m < M; m++ )
			//	UX[mp * M + m] = 0.0 ;
			UX[mp*M + mp] = 1. - *Pc * ECt + Ul[mp];

			Sd->ColCoeff = *Pc * CFc;
			//UX[mp*M + mp - 1] = - dTM / ( 0.5 * cap[M-1] * res[M-1] ) ;
		}
	}
	else
	{
		if (WallType == 'C')
		{
			double ECG, ECt, CFc;
			WALL   *Wall;

			Wall = Sd->mw->wall;
			//*Pc = dTM / (0.5 * cap[mp]);
			*Pc = dTM / (0.5 * captempL[mp]);

			FNBoundarySolarWall(Sd, &ECG, &ECt, &CFc);
			UX[mp*M + mp] = 1. - *Pc * ECt + Ul[mp];
			Sd->ColCoeff = *Pc * CFc;
			//Sd->ColCoeff = 0.0 ;

			//printf("ColCoeff=%lf\n", Sd->ColCoeff) ;
		}

		*Pc = 0.0;
	}

	*uo = Ul[0];
	*um = Ur[M - 1];

	if (PCMf == 5)
	{
		/*************/
		printf(" Wallfdc -- U --\n");
		matprint(" %12.8lf", M, UX);
		printf("\nuo=%lf   um=%lf\n", *uo, *um);
		printf("mp=%d  Pc=%lf\n", mp, *Pc);
		/***********/
	}

	matinv(UX, M, M, "<Wallfdc>");

	/*********************/
	if (PCMf == 5)
	{
		printf("\n Wallfdc-- inv(U) --\n");
		matprint("%12.8lf", M, UX);
	}
	/*********************/

	free(Ur);
	free(Ul);
	free(captempL);
	free(captempR);
}

/* --------------------------------------------- */

/*  後退差分による壁体表面、内部温度の計算   */


void Twall(int M, int mp, double *UX, double uo, double um,
	double Pc, double Ti, double To, double WpT, double *Told, double *Tw, RMSRF *Sd, PCM **pcm)
{
	int m, j;
	double	*Ttemp;
	double	*Toldcalc;
	PCM	*PCMLyr;
	double	Tpcm, Tpcmold;

	/****************
	printf("\n Twall-- inv(U) --\n");
	matprint("%8.4lf", M, UX);
	printf("  To=%lf um=%lf\n", To, um);
	for (m = 0; m < M; m++)
	printf("  Twold=%lf", Told[m]) ;
	printf("\n");
	******************/

	// 前時刻の壁体内部温度のコピー
	Ttemp = dcalloc(M, "<Twall> Ttemp");
	Toldcalc = dcalloc(M, "<Twall> Toldcalc");
	for (m = 0; m < M; m++)
	{
		Ttemp[m] = Told[m];
		Toldcalc[m] = Told[m];
	}

	Toldcalc[0] += uo * Ti;

	if (Sd->mw->wall->WallType != 'C')
		Toldcalc[M - 1] += um * To;
	else
		Toldcalc[M - 1] += Sd->ColCoeff * To;

	if (Pc > 0.) Toldcalc[mp] += Pc * WpT;

	for (m = 0; m < M; m++)
	{
		Tw[m] = 0.;
		for (j = 0; j < M; j++)
		{
			Tw[m] += UX[m * M + j] * Toldcalc[j];
			//printf("%d Tm=%lf UX=%lf Told=%lf\n",j,Tw[m],UX[m * M + j],Told[j]);
		}
	}
	//for (m = 0; m < M; m++)
	//{
	//	Told[m] = Tw[m];
	//	Toldd[m] = Tw[m];
	//	Twd[m] = Tw[m];
	//}

	// 建材一体型集熱器の集熱器と建材の境界温度
	if (Sd->mw->wall->WallType == 'C')
		Sd->oldTx = Tw[mp];
	/**************

	 printf("-----Twall  M=%d\n", M);
	 for (m = 0; m < M; m++)
	 printf("  Tw=%lf", Tw[m]);
	 printf("\n");
***************/
// PCMの温度飛び越えのチェック
	for (m = 0; m < M; m++)
	{
		PCMLyr = pcm[m];
		if (PCMLyr != NULL)
		{
			// 現在時刻のPCM温度
			Tpcm = (Tw[m - 1] + Tw[m]) * 0.5;
			// 前時刻のPCM温度
			Tpcmold = (Ttemp[m - 1] + Ttemp[m]) * 0.5;

			// 壁体温度が潜熱領域をまたいだかチェック
			if (PCMLyr->iterate == 'n' && ((PCMLyr->Ts > Tpcmold && PCMLyr->Tl < Tpcm)
				|| (PCMLyr->Tl < Tpcmold && PCMLyr->Ts > Tpcm)))
			{
				printf("xxxx 壁体温度が潜熱領域をまたぎました Tpcm=%.1lf Tpcmold=%.1lf\n",
					Tpcm, Tpcmold);
			}
		}
	}
}

/* --------------------------------------------- */

/*  後退差分による壁体表面、内部温度の計算   */
// PCM収束計算過程のチェック用

void Twalld(int M, int mp, double *UX, double uo, double um,
	double Pc, double Ti, double To, double WpT, double *Told, double *Twd, RMSRF *Sd)
{
	int m, j;
	double	*Toldtemp;

	// 収束計算過程なので、前時刻の計算結果が変わらないようにバックアップ
	Toldtemp = dcalloc(M, "<Twalld> Toldtemp");
	matcpy(Told, Toldtemp, M);

	Toldtemp[0] += uo * Ti;

	if (Sd->mw->wall->WallType != 'C')
		Toldtemp[M - 1] += um * To;
	else
		Toldtemp[M - 1] += Sd->ColCoeff * To;

	if (Pc > 0.) Toldtemp[mp] += Pc * WpT;

	for (m = 0; m < M; m++, Twd++)
	{
		*Twd = 0.;
		for (j = 0; j < M; j++)
		{
			*Twd += UX[m * M + j] * Toldtemp[j];
			//printf("%d Tm=%lf UX=%lf Told=%lf\n",j,Tw[m],UX[m * M + j],Told[j]);
		}
	}
	// 収束計算過程なので、解の入れ替えはしない
	//for (m = 0; m < M; m++)
	//	Told[m] = Tw[m];

	free(Toldtemp);
}

/* ---------------------------------------------- */

