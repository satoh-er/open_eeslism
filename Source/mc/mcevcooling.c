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

/* mcevcooling.c */


#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"
#include "waterandair.h"

/*  気化冷却器  */

/*  仕様入力  */

int		Evacdata(char *s, EVACCA *Evacca)
{
	char	*st;
	int		id = 0;

	if ((st = strchr(s, '=')) == 0)
	{
		Evacca->name = stralloc(s);
		Evacca->N = -999;
		Evacca->Nlayer = -999;
		Evacca->Awet = Evacca->Adry = Evacca->hdry = Evacca->hwet = -999.;
	}
	else
	{
		*st = '\0';

		if (strcmp(s, "Awet") == 0)
			Evacca->Awet = atof(st + 1);
		else if (strcmp(s, "Adry") == 0)
			Evacca->Adry = atof(st + 1);
		else if (strcmp(s, "hwet") == 0)
			Evacca->hwet = atof(st + 1);
		else if (strcmp(s, "hdry") == 0)
			Evacca->hdry = atof(st + 1);
		else if (strcmp(s, "N") == 0)
			Evacca->N = atoi(st + 1);
		else if (strcmp(s, "Nlayer") == 0)
			Evacca->Nlayer = atoi(st + 1);
		else
			id = 1;
	}

	return (id);
}

/* ------------------------------------------------------ */
// 初期設定（入力漏れのチェック、変数用メモリの確保）
void	Evacint(int Nevac, EVAC *Evac)
{
	int		i;
	char	s[SCHAR];
	EVACCA	*cat;
	double	Temp;
	int		N, N2;

	for (i = 0; i < Nevac; i++, Evac++)
	{
		// 入力漏れのチェック
		cat = Evac->cat;
		if (cat->N < 0)
		{
			sprintf_s(s, sizeof(s), "Name=%s catname=%s 分割数が未定義です",
				Evac->name, cat->name);
			Eprint("<Evacint>", s);
		}
		if (cat->Adry < 0. || cat->Awet < 0. || (cat->Nlayer < 0 && (cat->hdry < 0. || cat->hwet < 0.)))
		{
			sprintf_s(s, sizeof(s), "Name=%s catname=%s Adry=%.1g Awet=%.1g hdry=%.1g hwet=%.1g\n",
				Evac->name, cat->name, cat->Adry, cat->Awet, cat->hdry, cat->hwet);
			Eprint("<Evacint>", s);
		}

		// 面積を分割後の面積に変更
		cat->Adry /= (double)cat->N;
		cat->Awet /= (double)cat->N;

		// 必要なメモリ領域の確保
		if (cat->N > 0)
		{
			Temp = FNXtr(20., 50.);
			// 蒸発量のメモリ確保
			Evac->M = dcalloc(cat->N, "<Evacini> M");
			// 物質移動係数のメモリ確保
			Evac->kx = dcalloc(cat->N, "<Evacini> kx");
			// Dry側温度のメモリ確保
			Evac->Tdry = dcallocinit(cat->N, 20., "<Evacini> Tdry");
			// Wet側温度のメモリ確保
			Evac->Twet = dcallocinit(cat->N, 20., "<Evacini> Twet");
			// Dry側絶対湿度のメモリ確保
			Evac->xdry = dcallocinit(cat->N, Temp, "<Evacini> xdry");
			// Wet側絶対湿度のメモリ確保
			Evac->xwet = dcallocinit(cat->N, Temp, "<Evacini> xwet");
			// 境界層温度のメモリ確保
			Evac->Ts = dcallocinit(cat->N, 20., "<Evacini> Ts");
			// 境界層絶対湿度のメモリ確保
			Evac->xs = dcallocinit(cat->N, FNXtr(20., 100.), "<Evacini> xs");
			// Dry側相対湿度のメモリ確保
			Evac->RHdry = dcallocinit(cat->N, 50., "<Evacini> RHdry");
			// Wet側相対湿度のメモリ確保
			Evac->RHwet = dcallocinit(cat->N, 50., "<Evacini> RHwet");

			// 状態値計算用行列
			N = cat->N * 5;
			N2 = N * N;
			//Evac->UXdry = dcalloc(N2, "<Evacini> UXdry") ;
			//Evac->UXwet = dcalloc(N2, "<Evacini> UXwet") ;
			Evac->UX = dcalloc(N2, "<Evacini> UX");
			Evac->UXC = dcalloc(N, "<Evacini> UXC");
		}
	}
}

// 飽和絶対湿度の線形近似（Ts℃付近で線形回帰式を作成）
void	LinearSatx(double Ts, double *a, double *b)
{
	double	T1, T2, EPS = 0.2;
	double	x1, x2;
	double	FNXs(double T);

	// 線形近似の区間の設定（Tsを中心にEPS区間）
	T1 = Ts - EPS;
	T2 = Ts + EPS;

	// T1、T2における飽和絶対湿度の計算
	x1 = FNXs(T1);
	x2 = FNXs(T2);
	//printf("T1=%lf T2=%lf x1=%lf x2=%lf\n", T1, T2, x1, x2) ;

	// 線形回帰式の傾きの計算
	*a = (x2 - x1) / (T2 - T1);

	// 線形回帰式の切片の計算
	*b = x1 - *a * T1;
}

// 湿り空気の飽和絶対湿度の計算
double	FNXs(double T)
{
	return(4.2849E-3 * exp(6.0260E-2 * T));
}

/*  気化冷却器出口空気温湿度に関する変数割当  */
void	Evacelm(int Nevac, EVAC *Evac)
{
	int i;
	ELOUT  *EoTdry, *Eoxdry, *EoTwet, *Eoxwet;
	ELIN   *elin;

	for (i = 0; i < Nevac; i++, Evac++)
	{
		// Tdryoutの出口温度計算用
		EoTdry = Evac->cmp->elouts;
		// xdryoutの出口温度計算用
		Eoxdry = EoTdry + 1;
		// Twetoutの出口温度計算用
		EoTwet = Eoxdry + 1;
		// xwetoutの出口温度計算用
		Eoxwet = EoTwet + 1;

		// Tdryout計算式への変数割り当て
		elin = EoTdry->elins;
		// 2つめはxdryin
		elin++;
		elin->upo = elin->upv = Eoxdry->elins->upo;
		// 3つめはTwetin
		elin++;
		elin->upo = elin->upv = EoTwet->elins->upo;
		// 4つめはxwetin
		elin++;
		elin->upo = elin->upv = Eoxwet->elins->upo;

		// xdryout計算式への変数の割り当て
		elin = Eoxdry->elins;
		// 2つめはTdryin
		elin++;
		elin->upo = elin->upv = EoTdry->elins->upo;
		// 3つめはTwetin
		elin++;
		elin->upo = elin->upv = EoTwet->elins->upo;
		// 4つめはxwetin
		elin++;
		elin->upo = elin->upv = Eoxwet->elins->upo;

		// Twetout計算式への変数割り当て
		elin = EoTwet->elins;
		// 2つめはTdryin
		elin++;
		elin->upo = elin->upv = EoTdry->elins->upo;
		// 3つめはxdryin
		elin++;
		elin->upo = elin->upv = Eoxdry->elins->upo;
		// 4つめはxwetin
		elin++;
		elin->upo = elin->upv = Eoxwet->elins->upo;

		// xwetout計算式への変数割り当て
		elin = Eoxwet->elins;
		// 2つめはTdryin
		elin++;
		elin->upo = elin->upv = EoTdry->elins->upo;
		// 3つめはxdryin
		elin++;
		elin->upo = elin->upv = Eoxdry->elins->upo;
		// 4つめはTwetin
		elin++;
		elin->upo = elin->upv = EoTwet->elins->upo;
	}
}

// 風速の計算
double	Evacu(double G, double T, double H, double W, int N)
{
	double	u;

	u = G / FNarow(T) / ((double)N * H * W);

	return (u);
}

// 通気部の対流熱伝達率の計算（プログラムを解読したため詳細は不明）
double Evachcc(double de, double L, double T, double H, double W, int N, double G, int Flg)
{
	double	AR, Re, hc, u, Nu;
	double	Mgn;

	// 流路縦横比の計算
	AR = H / (W / 5.);

	// 通気部の風速の計算
	u = Evacu(G, T, H, W, N);

	// レイノルズ数の計算
	Re = u * L / FNanew(T);

	// ヌセルト数の計算
	Nu = EvacNu(AR, Re);

	// 裕度の計算
	Mgn = 0.875;
	if (Flg == 'd')
	{
		if (Re > 1000.)
			Mgn *= (0.0000128205 * Re + 1.0859);
		else
			Mgn *= (0.00083333 * Re + 0.18333);
	}

	// 対流熱伝達率の計算
	hc = Nu / de * FNalam(T) * Mgn;

	return (hc);
}

// 通気部のヌセルト数を計算する
double	EvacNu(double AR, double Re)
{
	double Nu;
	Nu = 0.0;

	if (Re <= 1000.)
		Nu = -13.042 * AR * AR * AR + 27.063 * AR * AR - 18.591 * AR + 7.54;
	else if (Re <= 2000.)
		Nu = (-0.023131 * AR * AR + 0.018229 * AR + 0.00043299) * Re
		+ (46.261 * AR * AR - 36.459 * AR + 7.0971);
	else
		Nu = 0.021 * pow(Re, 0.8) * pow(0.7, 0.4);

	return (Nu);
}

// 要素方程式の係数計算

void	Evaccfv(int Nevac, EVAC *Evac)
{
	int i, ii, jj;
	ELOUT  *EoTdry, *Eoxdry, *EoTwet, *Eoxwet;
	//ELIN   *elin;
	extern double	ca,		// 乾き空気の比熱[J/kgK]
		ro,		// 水の蒸発潜熱[J/kg]
		cv;	// 水蒸気の低圧比熱[J/kgK]
	double	*U, *C;
	double	*cfin;
	extern int	DEBUG;
	int		N, N2, Row;
	double	A, *a, *b;
	double	*EvpFlg, *EF;
	double	*Ts, *xs, *xwet, *RHwet, *kx, Tsave;
	EVACCA	*cat;
	double	*Cmat;
	double	Gdry, Gwet;
	double	PreFlg;
	void	LinearSatx(double Ts, double *a, double *b);

	for (i = 0; i < Nevac; i++, Evac++)
	{
		EvpFlg = dcalloc(Evac->cat->N, "<Evaccfv> EvpFlg");

		if (Evac->cmp->control != OFF_SW)
		{
			// Tdryoutの出口温度計算用
			EoTdry = Evac->cmp->elouts;
			// xdryoutの出口温度計算用
			Eoxdry = EoTdry + 1;
			// Twetoutの出口温度計算用
			EoTwet = Eoxdry + 1;
			// xwetoutの出口温度計算用
			Eoxwet = EoTwet + 1;

			cat = Evac->cat;
			Gdry = EoTdry->G;
			Gwet = EoTwet->G;

			// 対流熱伝達率の計算
			if (cat->Nlayer > 0)
			{
				// 平均境界温度を計算
				Ts = Evac->Ts;
				Tsave = 0.;
				for (ii = 0; ii < cat->N; ii++, Ts++)
					Tsave += *Ts / (double)cat->N;
				// Dry側
				cat->hdry = Evachcc(4.3 / 1000., 4.3 / 1000., Tsave, 2.3 / 1000., 173. / 1000., cat->Nlayer, Gdry, 'd');
				// Wet側
				cat->hwet = Evachcc(6.4 / 1000., 6.4 / 1000., Tsave, 3.5 / 1000., 173. / 1000., cat->Nlayer, Gwet, 'w');
			}

			// 行列のメモリ確保
			N = cat->N * 5;
			N2 = N * N;

			//Udry = dcalloc(N2, "<Evaccfv> Udry") ;
			//Uwet = dcalloc(N2, "<Evaccfv> Uwety") ;
			U = dcalloc(N2, "<Evaccfv> U");
			C = dcalloc(N, "<Evaccfv> C");
			a = dcalloc(cat->N, "<Evaccfv> a");
			b = dcalloc(cat->N, "<Evaccfv> b");

			// 前時刻の内部状態値
			Ts = Evac->Ts + cat->N - 1;
			xwet = Evac->xwet + cat->N - 1;
			RHwet = Evac->RHwet + cat->N - 1;
			kx = Evac->kx + cat->N - 1;
			PreFlg = 1.;
			EF = EvpFlg + cat->N - 1;

			// Wet層の上流から蒸発有無を判定する（上流が飽和状態の場合、下流も飽和状態とする）
			for (ii = cat->N - 1; ii >= 0; ii--, Ts--, xwet--, RHwet--, kx--, EF--)
			{
				// 境界層温度における飽和絶対湿度計算用係数
				LinearSatx(*Ts, &a[ii], &b[ii]);
				//printf("蒸発判定 ii=%d Ts=%lf a=%lf b=%lf\n", ii, *Ts, a[ii], b[ii]) ;
				*EF = 1.;
				if (a[ii] * *Ts + b[ii] - *xwet< 0.	// 境界層温度における飽和絶対湿度よりxwetが大きい場合は蒸発しない
					|| *RHwet > 99.			// Wet側空気がすでに飽和状態の場合は蒸発しない
					|| fabs(PreFlg) <= 1.e-5)	// Wet側の上流がすでに飽和状態の場合
					*EF = 0.;

				//printf("ii=%d EF=%.0lf\n", ii, *EF) ;
				PreFlg = *EF;
			}

			// 前時刻の内部状態値
			Ts = Evac->Ts;
			xs = Evac->xs;
			xwet = Evac->xwet;
			RHwet = Evac->RHwet;
			kx = Evac->kx;
			Cmat = C;
			EF = EvpFlg;
			for (ii = 0; ii < cat->N; ii++, Ts++, xs++, xwet++, RHwet++, kx++, Cmat++, EF++)
			{
				// 境界層温度における飽和絶対湿度計算用係数
				//LinearSatx(*Ts, &a, &b) ;
				//printf("ii=%d Ts=%lf a=%lf b=%lf\n", ii, *Ts, a, b) ;

				// 物質移動係数の計算
				*kx = cat->hwet / (ca + cv * *xs) * *EF;
				// 係数の計算
				A = *kx * (ro + cv * *Ts) * *EF;

				// C行列の作成
				// Twetの状態方程式には定数項はない
				*Cmat = 0.0;
				// xwetの定数項作成
				Cmat++;
				*Cmat = -cat->Awet * *kx * b[ii];
				// Tsの定数項作成
				Cmat++;
				*Cmat = A * b[ii];
				// Tdry、xdryの係数はゼロ
				Cmat++;
				*Cmat = 0.0;
				Cmat++;
				*Cmat = 0.0;

				// U行列の作成
				// 対角行列
				// Twetの項
				*(U + N * (5 * ii + 0) + (5 * ii + 0)) = -(ca * Gwet + cat->Awet * cat->hwet);
				// xwetの項
				*(U + N * (5 * ii + 1) + (5 * ii + 1)) = -(Gwet + cat->Awet * *kx);
				// Tsの項
				*(U + N * (5 * ii + 2) + (5 * ii + 2)) = -(cat->hwet + A * a[ii] + cat->hdry);
				// Tdryの項
				*(U + N * (5 * ii + 3) + (5 * ii + 3)) = -(ca * Gdry + cat->Adry * cat->hdry);
				// xdryの項
				*(U + N * (5 * ii + 4) + (5 * ii + 4)) = 1.;

				// TwetとTsの項
				*(U + N * (5 * ii + 0) + (5 * ii + 2)) = cat->Awet * cat->hwet;
				// xwetとTsの項
				*(U + N * (5 * ii + 1) + (5 * ii + 2)) = cat->Awet * *kx * a[ii];
				// TsとTwetの項
				*(U + N * (5 * ii + 2) + (5 * ii + 0)) = cat->hwet;
				// Tsとxwetの項
				*(U + N * (5 * ii + 2) + (5 * ii + 1)) = A;
				// TsとTdryの項
				*(U + N * (5 * ii + 2) + (5 * ii + 3)) = cat->hdry;
				// TdryとTsの項
				*(U + N * (5 * ii + 3) + (5 * ii + 2)) = cat->Adry * cat->hdry;

				// Dry側上流
				if (ii > 0)
				{
					// Tdryの項
					*(U + N * (5 * ii + 3) + (5 * (ii - 1) + 3)) = ca * Gdry;
					// xdryの項
					*(U + N * (5 * ii + 4) + (5 * (ii - 1) + 4)) = -1.;
				}

				// Wet側の上流
				if (ii < cat->N - 1)
				{
					// Twetの項
					*(U + N * (5 * ii + 0) + (5 * (ii + 1) + 0)) = ca * Gwet;
					// xwetの項
					*(U + N * (5 * ii + 1) + (5 * (ii + 1) + 1)) = Gwet;
				}

				//// Udry行列の作成
				//if(ii == 0)
				//{
				//	// Tdry, xdryの項のみ入力
				//	*(Udry + N * (5 * ii + 3) + (5 * ii + 3)) = ca * Gdry ;
				//	*(Udry + N * (5 * ii + 4) + (5 * ii + 4)) = -1. ;
				//}

				//// Uwetの行列の作成
				//if(ii == cat->N - 1)
				//{
				//	// Twet, xwetの項のみ入力
				//	*(Uwet + N * (5 * ii + 0) + ( 5 * ii + 0)) = ca * Gwet ;
				//	*(Uwet + N * (5 * ii + 1) + ( 5 * ii + 1)) = Gwet ;
				//}
			}

			// 行列の表示
			//printf("matrix U\n") ;
			//matprint("%8.3lf  ", N, U) ;

			//printf("matrix C\n") ;
			//for(ii = 0; ii < N; ii++)
			//	printf("%g\n", *(C+ii)) ;

			//printf("matrix Udry\n") ;
			//matprint("%8.3lf  ", N, Udry) ;

			//printf("matrix Uwet\n") ;
			//matprint("%8.3lf  ", N, Uwet) ;

			// Uの逆行列を計算
			matinv(U, N, N, "<Evaccfv> U");
			// 行列のコピー
			matinit(Evac->UX, N2);
			matcpy(U, Evac->UX, N2);

			// 行列の表示
			//printf("matrix UX\n") ;
			//matprint("%g  ", N, Evac->UX) ;

			// [UXdry]=[UX]*[Udry]の作成
			//matinit(Evac->UXdry, N2) ;
			//matmalv(U, Udry, N, N, Evac->UXdry) ;

			//printf("matrix UXdry\n") ;
			//matprint("%8.3lf  ", N, Evac->UXdry) ;

			// [UXwet]=[UX]*[Uwet]の作成
			//matinit(Evac->UXwet, N2) ;
			//matmalv(U, Uwet, N, N, Evac->UXwet) ;

			//printf("matrix UXwet\n") ;
			//matprint("%8.3lf  ", N, Evac->UXwet) ;

			// {UXC}=[UX]*{C}の作成
			matinit(Evac->UXC, N);
			//matmalv(Evac->UX, C, 1, N, Evac->UXC) ;
			for (ii = 0; ii < N; ii++)
			{
				for (jj = 0; jj < N; jj++)
					*(Evac->UXC + ii) += *(Evac->UX + ii * N + jj) * *(C + jj);
			}

			//printf("matrix UXC\n") ;
			//for(ii = 0; ii < N; ii++)
			//	printf("%g\n", *(Evac->UXC+ii)) ;

			// 要素方程式の係数の収録
			/////// Tdryoutの要素方程式
			Row = N * (5 * (cat->N - 1) + 3);
			// 出口の項
			EoTdry->coeffo = -1.;
			// 定数の項
			EoTdry->Co = -*(Evac->UXC + (5 * (cat->N - 1) + 3));
			// 入口の項
			// Tdryinの項
			cfin = EoTdry->coeffin;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 3)) * (ca * Gdry);
			// xdryinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 4)) * -1.;
			// Twetinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 0)) * (ca * Gwet);
			// xwetinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 1)) * (Gwet);

			/////// xdryoutの要素方程式
			Row = N * (5 * (cat->N - 1) + 4);
			// 出口の項
			Eoxdry->coeffo = -1.;
			// 定数の項
			Eoxdry->Co = -*(Evac->UXC + (5 * (cat->N - 1) + 4));
			// 入口の項
			// xdryinの項
			cfin = Eoxdry->coeffin;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 4)) * -1.;
			// Tdryinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 3)) * (ca * Gdry);
			// Twetinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 0)) * (ca * Gwet);
			// xwetinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 1)) * (Gwet);

			/////// Twetoutの要素方程式
			Row = N * (5 * (1 - 1) + 0);
			// 出口の項
			EoTwet->coeffo = -1.;
			// 定数の項
			EoTwet->Co = -*(Evac->UXC + (5 * (1 - 1) + 0));
			// 入口の項
			// Twetinの項
			cfin = EoTwet->coeffin;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 0)) * (ca * Gwet);
			// Tdryinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 3)) * (ca * Gdry);
			// xdryinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 4)) * -1.;
			// xwetinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 1)) * (Gwet);

			/////// xwetoutの要素方程式
			Row = N * (5 * (1 - 1) + 1);
			// 出口の項
			Eoxwet->coeffo = -1.;
			// 定数の項
			Eoxwet->Co = -*(Evac->UXC + (5 * (1 - 1) + 1));
			// 入口の項
			// xwetinの項
			cfin = Eoxwet->coeffin;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 1)) * (Gwet);
			// Tdryinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 3)) * (ca * Gdry);
			// xdryinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (1 - 1) + 4)) * -1.;
			// Twetinの項
			cfin++;
			*cfin = -*(Evac->UX + Row + (5 * (cat->N - 1) + 0)) * (ca * Gwet);

			// 動的メモリの解放
			//free(Udry) ;
			//free(Uwet) ;
			free(U);
			free(C);
			free(a);
			free(b);
		}

		free(EvpFlg);
	}
}

// 内部温度、熱量の計算
void	Evacene(int Nevac, EVAC *Evac, int *Evacreset)
{
	int i, ii, N, N2, jj;
	ELOUT	*EoTdry, *Eoxdry, *EoTwet, *Eoxwet;
	EVACCA	*cat;
	double	*Sin, *Stat, *Scmp;
	extern double	ca,		// 乾き空気の比熱[J/kgK]
		ro,		// 水の蒸発潜熱[J/kg]
		cv;	// 水蒸気の低圧比熱[J/kgK]

	for (i = 0; i < Nevac; i++, Evac++)
	{
		cat = Evac->cat;
		if (Evac->cmp->control != OFF_SW)
		{
			double	Gdry, Gwet;
			double	*Tdry, *Twet, *xdry, *xwet, *Ts, *xs, *RHwet, *RHdry, *M, *kx;
			ELIN	*elin;

			// Tdryoutの出口温度計算用
			EoTdry = Evac->cmp->elouts;
			// xdryoutの出口温度計算用
			Eoxdry = EoTdry + 1;
			// Twetoutの出口温度計算用
			EoTwet = Eoxdry + 1;
			// xwetoutの出口温度計算用
			Eoxwet = EoTwet + 1;

			// 出入口状態値の取得
			elin = EoTdry->elins;
			Evac->Tdryi = elin->sysvin;
			elin++;
			Evac->xdryi = elin->sysvin;
			elin++;
			Evac->Tweti = elin->sysvin;
			elin++;
			Evac->xweti = elin->sysvin;

			Evac->Tdryo = EoTdry->sysv;
			Evac->xdryo = Eoxdry->sysv;
			Evac->Tweto = EoTwet->sysv;
			Evac->xweto = Eoxwet->sysv;

			Gdry = EoTdry->G;
			Gwet = EoTwet->G;

			// 相対湿度の計算
			Evac->RHdryi = FNRhtx(Evac->Tdryi, Evac->xdryi);
			Evac->RHdryo = FNRhtx(Evac->Tdryo, Evac->xdryo);
			Evac->RHweti = FNRhtx(Evac->Tweti, Evac->xweti);
			Evac->RHweto = FNRhtx(Evac->Tweto, Evac->xweto);

			// 熱量の計算
			Evac->Qsdry = ca * Gdry * (Evac->Tdryo - Evac->Tdryi);
			Evac->Qldry = ro * Gdry * (Evac->xdryo - Evac->xdryi);
			Evac->Qtdry = Evac->Qsdry + Evac->Qldry;
			Evac->Qswet = ca * Gwet * (Evac->Tweto - Evac->Tweti);
			Evac->Qlwet = ro * Gwet * (Evac->xweto - Evac->xweti);
			Evac->Qtwet = Evac->Qswet + Evac->Qlwet;

			N = cat->N * 5;
			N2 = N * N;

			// 入口状態値の行列作成
			Sin = dcalloc(N, "<Evacene> Sin");
			// 内部変数計算結果収録用
			Stat = dcalloc(N, "<Evacene> Stat");

			*(Sin + 5 * (1 - 1) + 3) = ca * Gdry * Evac->Tdryi;
			*(Sin + 5 * (1 - 1) + 4) = -Evac->xdryi;
			*(Sin + 5 * (cat->N - 1) + 0) = ca * Gwet * Evac->Tweti;
			*(Sin + 5 * (cat->N - 1) + 1) = Gwet * Evac->xweti;

			//matmalv(Evac->UXdry, Sdry, N, 1, UXSdry) ;
			//matmalv(Evac->UXwet, Swet, N, 1, UXSwet) ;

			for (ii = 0; ii < N; ii++)
			{
				for (jj = 0; jj < N; jj++)
				{
					// 内部変数の計算
					*(Stat + ii) += -*(Evac->UX + ii * N + jj) * *(Sin + jj);

					//printf("UX=%lf Sin=%lf\n", *(Evac->UX + ii * N + jj), *(Sin + jj)) ;
				}

				*(Stat + ii) += *(Evac->UXC + ii);
				//printf("UXC=%lf Stat=%lf\n\n", *(Evac->UXC + ii), *(Stat+ii)) ;
			}

			// 内部変数計算結果の格納
			Tdry = Evac->Tdry;
			xdry = Evac->xdry;
			Twet = Evac->Twet;
			xwet = Evac->xwet;
			Ts = Evac->Ts;
			xs = Evac->xs;
			RHdry = Evac->RHdry;
			RHwet = Evac->RHwet;
			M = Evac->M;
			Scmp = Stat;
			kx = Evac->kx;
			for (ii = 0; ii < cat->N; ii++, Tdry++, xdry++, Twet++, xwet++, Ts++, xs++, M++, RHdry++, RHwet++, kx++)
			{
				*Twet = *Scmp;
				Scmp++;
				*xwet = *Scmp;
				Scmp++;
				*Ts = *Scmp;
				Scmp++;
				*Tdry = *Scmp;
				Scmp++;
				*xdry = *Scmp;
				Scmp++;
				*xs = FNXtr(*Ts, 100.);

				// 相対湿度の計算
				*RHdry = FNRhtx(*Tdry, *xdry);
				*RHwet = FNRhtx(*Twet, *xwet);

				// 蒸発量の計算
				*M = *kx * dmax(*xs - *xwet, 0.) * cat->Awet;
			}

			// 動的メモリ解放
			free(Sin);
			//free(Swet) ;
			free(Stat);
			//free(UXSdry) ;
			//free(UXSwet) ;
		}
		else
		{
			Evac->Qsdry = Evac->Qldry = Evac->Qtdry = Evac->Qswet = Evac->Qlwet = Evac->Qtwet = 0.0;
			Evac->Tdryi = Evac->Tdryo = Evac->Tweti = Evac->Tweto
				= Evac->xdryi = Evac->xdryo = Evac->xweti = Evac->xweto = 0.;
			matinit(Evac->M, cat->N);
		}

		Evac->count++;
		if (Evac->count > 0)
			*Evacreset = 1;
	}
}

// カウンタのリセット
void	Evaccountreset(int Nevac, EVAC *Evac)
{
	int	i;

	for (i = 0; i < Nevac; i++, Evac++)
		Evac->count = 0;
}

// 代表日の計算結果出力
void Evacprint(FILE *fo, int id, int Nevac, EVAC *Evac)
{
	int	i, ii;
	ELOUT	*elo;
	double	*Tdry, *Twet, *xdry, *xwet, *Ts, *RHwet, *RHdry, *M;

	switch (id)
	{
	case 0:
		if (Nevac > 0)
			fprintf(fo, "%s %d\n", EVAC_TYPE, Nevac);
		for (i = 0; i < Nevac; i++, Evac++)
			fprintf(fo, " %s 1 %d\n", Evac->name, 18 + 8 * Evac->cat->N);

		break;

	case 1:
		for (i = 0; i < Nevac; i++, Evac++)
		{
			// Wet側出力
			fprintf(fo, "%s_cw c c %s_Gw m f %s_Twi t f %s_Two t f %s_xwi x f %s_xwo x f\n",
				Evac->name, Evac->name, Evac->name, Evac->name, Evac->name, Evac->name);
			fprintf(fo, "%s_Qws q f %s_Qwl q f %s_Qwt q f\n",
				Evac->name, Evac->name, Evac->name);
			// Dry側出力
			fprintf(fo, "%s_cd c c %s_Gd m f %s_Tdi t f %s_Tdo t f %s_xdi x f %s_xdo x f\n",
				Evac->name, Evac->name, Evac->name, Evac->name, Evac->name, Evac->name);
			fprintf(fo, "%s_Qds q f %s_Qdl q f %s_Qdt q f\n",
				Evac->name, Evac->name, Evac->name);
			// 内部変数
			for (ii = 0; ii < Evac->cat->N; ii++)
			{
				fprintf(fo, "%s_Tw[%d] t f %s_xw[%d] x f %s_RHw[%d] r f %s_Ts[%d] t f %s_Td[%d] t f %s_xd[%d] x f %s_RHd[%d] r f %s_M[%d] m f\n",
					Evac->name, ii, Evac->name, ii, Evac->name, ii, Evac->name, ii, Evac->name, ii, Evac->name, ii, Evac->name, ii, Evac->name, ii);
			}
		}
		break;

	default:
		for (i = 0; i < Nevac; i++, Evac++)
		{
			// Wet側出力
			elo = Evac->cmp->elouts + 2;
			fprintf(fo, "%c %g %.1lf %.1lf %.3lf %.3lf %.1lf %.1lf %.1lf\n",
				elo->control, elo->G, Evac->Tweti, Evac->Tweto, Evac->xweti, Evac->xweto, Evac->Qswet, Evac->Qlwet, Evac->Qtwet);
			// Dry側出力
			elo = Evac->cmp->elouts;
			fprintf(fo, "%c %g %.1lf %.1lf %.3lf %.3lf %.1lf %.1lf %.1lf\n",
				elo->control, elo->G, Evac->Tdryi, Evac->Tdryo, Evac->xdryi, Evac->xdryo, Evac->Qsdry, Evac->Qldry, Evac->Qtdry);
			// 内部変数
			Tdry = Evac->Tdry;
			xdry = Evac->xdry;
			Twet = Evac->Twet;
			xwet = Evac->xwet;
			Ts = Evac->Ts;
			RHdry = Evac->RHdry;
			RHwet = Evac->RHwet;
			M = Evac->M;
			for (ii = 0; ii < Evac->cat->N; ii++, Tdry++, xdry++, Twet++, xwet++, Ts++, M++, RHdry++, RHwet++)
			{
				fprintf(fo, "%.1lf %.3lf %.0lf %.1lf %.1lf %.3lf %.0lf %.3e\n",
					*Twet, *xwet, *RHwet, *Ts, *Tdry, *xdry, *RHdry, *M);
			}
		}
	}
}


//void	Thexprint(FILE *fo, int id, int Nthex, THEX *Thex )
//{
//	int i;
//	ELOUT *el ;
//	
//	switch (id)
//	{
//	default:
//		for (i = 0; i < Nthex; i++, Thex++)
//		{
//			el = Thex->cmp->elouts ;
//			fprintf(fo, "%c %6.4g %4.1lf %4.1lf %.3lf %.3lf ", 
//				el->control, Thex->Ge, Thex->Tein, Thex->Teout, Thex->xein, Thex->xeout );
//			fprintf(fo, "%.0lf %.0lf %.0lf %.0lf %.0lf\n",
//				Thex->hein, Thex->heout, Thex->Qes, Thex->Qel, Thex->Qet ) ;
//
//			el += 2 ;
//			fprintf(fo, "%c %6.4g %4.1lf %4.1lf %.3lf %.3lf ", 
//				el->control, Thex->Go, Thex->Toin, Thex->Toout, Thex->xoin, Thex->xoout );
//			fprintf(fo, "%.0lf %.0lf %.0lf %.0lf %.0lf\n",
//				Thex->hoin, Thex->hoout, Thex->Qos, Thex->Qol, Thex->Qot ) ;
//		}
//		break;
//	}
//} 
//
//void	Thexdyint(int Nthex, THEX *Thex)
//{
//	int i;
//	
//	for (i = 0; i < Nthex; i++, Thex++)
//	{
//		svdyint(&Thex->Teidy);
//		svdyint(&Thex->Teody);
//		svdyint(&Thex->xeidy);
//		svdyint(&Thex->xeody);
//
//		svdyint(&Thex->Toidy);
//		svdyint(&Thex->Toody);
//		svdyint(&Thex->xoidy);
//		svdyint(&Thex->xoody);
//		
//		qdyint(&Thex->Qdyes);
//		qdyint(&Thex->Qdyel);
//		qdyint(&Thex->Qdyet);
//		
//		qdyint(&Thex->Qdyos);
//		qdyint(&Thex->Qdyol);
//		qdyint(&Thex->Qdyot);
//	}
//}
//
//void	Thexmonint(int Nthex, THEX *Thex)
//{
//	int i;
//	
//	for (i = 0; i < Nthex; i++, Thex++)
//	{
//		svdyint(&Thex->mTeidy);
//		svdyint(&Thex->mTeody);
//		svdyint(&Thex->mxeidy);
//		svdyint(&Thex->mxeody);
//
//		svdyint(&Thex->mToidy);
//		svdyint(&Thex->mToody);
//		svdyint(&Thex->mxoidy);
//		svdyint(&Thex->mxoody);
//		
//		qdyint(&Thex->mQdyes);
//		qdyint(&Thex->mQdyel);
//		qdyint(&Thex->mQdyet);
//		
//		qdyint(&Thex->mQdyos);
//		qdyint(&Thex->mQdyol);
//		qdyint(&Thex->mQdyot);
//	}
//}
//
//void	Thexday(int Mon, int Day, int ttmm, int Nthex, THEX *Thex, int Nday, int SimDayend)
//{
//	int i;
//	
//	for (i = 0; i < Nthex; i++, Thex++)
//	{ 
//		// 日集計
//		svdaysum(ttmm, Thex->cmp->control, Thex->Tein, &Thex->Teidy);
//		svdaysum(ttmm, Thex->cmp->control, Thex->Teout, &Thex->Teody);
//		svdaysum(ttmm, Thex->cmp->control, Thex->xein, &Thex->xeidy);
//		svdaysum(ttmm, Thex->cmp->control, Thex->xeout, &Thex->xeody);
//		
//		svdaysum(ttmm, Thex->cmp->control, Thex->Toin, &Thex->Toidy);
//		svdaysum(ttmm, Thex->cmp->control, Thex->Toout, &Thex->Toody);
//		svdaysum(ttmm, Thex->cmp->control, Thex->xoin, &Thex->xoidy);
//		svdaysum(ttmm, Thex->cmp->control, Thex->xoout, &Thex->xoody);
//		
//		qdaysum(ttmm, Thex->cmp->control, Thex->Qes, &Thex->Qdyes);
//		qdaysum(ttmm, Thex->cmp->control, Thex->Qel, &Thex->Qdyel);
//		qdaysum(ttmm, Thex->cmp->control, Thex->Qet, &Thex->Qdyet);
//		
//		qdaysum(ttmm, Thex->cmp->control, Thex->Qos, &Thex->Qdyos);
//		qdaysum(ttmm, Thex->cmp->control, Thex->Qol, &Thex->Qdyol);
//		qdaysum(ttmm, Thex->cmp->control, Thex->Qot, &Thex->Qdyot);
//		
//		// 月集計
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Tein, &Thex->mTeidy, Nday, SimDayend);
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Teout, &Thex->mTeody, Nday, SimDayend);
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xein, &Thex->mxeidy, Nday, SimDayend);
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xeout, &Thex->mxeody, Nday, SimDayend);
//		
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Toin, &Thex->mToidy, Nday, SimDayend);
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Toout, &Thex->mToody, Nday, SimDayend);
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xoin, &Thex->mxoidy, Nday, SimDayend);
//		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xoout, &Thex->mxoody, Nday, SimDayend);
//		
//		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qes, &Thex->mQdyes, Nday, SimDayend);
//		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qel, &Thex->mQdyel, Nday, SimDayend);
//		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qet, &Thex->mQdyet, Nday, SimDayend);
//		
//		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qos, &Thex->mQdyos, Nday, SimDayend);
//		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qol, &Thex->mQdyol, Nday, SimDayend);
//		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qot, &Thex->mQdyot, Nday, SimDayend);
//	}
//}
//
//void	Thexdyprt(FILE *fo, int id, int Nthex, THEX *Thex)
//{
//	int i ;
//	
//	switch (id)
//	{
//	case 0:
//		if (Nthex > 0)
//            fprintf(fo, "%s %d\n", THEX_TYPE, Nthex);
//		for (i = 0; i < Nthex; i++, Thex++)
//			fprintf(fo, " %s 1 48\n", Thex->name);
//		break;
//		
//	case 1:
//		for (i = 0; i < Nthex; i++, Thex++)
//		{	 
//			fprintf(fo,"%s_Hte H d %s_Te T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_ttne h d %s_Ten t f %s_ttme h d %s_Tem t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo,"%s_Hto H d %s_To T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_ttno h d %s_Ton t f %s_ttmo h d %s_Tom t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			
//			fprintf(fo,"%s_Hxe H d %s_xe T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_txne h d %s_xen t f %s_txme h d %s_xem t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo,"%s_Hxo H d %s_xo T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_txno h d %s_xon t f %s_txmo h d %s_xom t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//
//			fprintf(fo, "%s_Hhs H d %s_Qsh Q f %s_Hcs H d %s_Qsc Q f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_ths h d %s_qsh q f %s_tcs h d %s_qsc q f\n\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_Hhl H d %s_Qlh Q f %s_Hcl H d %s_Qlc Q f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_thl h d %s_qlh q f %s_tcl h d %s_qlc q f\n\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_Hht H d %s_Qth Q f %s_Hct H d %s_Qtc Q f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_tht h d %s_qth q f %s_tct h d %s_qtc q f\n\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//		}
//		break;
//		
//	default:
//		for (i = 0; i < Nthex; i++, Thex++)
//		{
//            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				Thex->Teidy.hrs, Thex->Teidy.m, 
//				Thex->Teidy.mntime, Thex->Teidy.mn, 
//				Thex->Teidy.mxtime, Thex->Teidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf\n", 
//				Thex->Toidy.hrs, Thex->Toidy.m, 
//				Thex->Toidy.mntime, Thex->Toidy.mn, 
//				Thex->Toidy.mxtime, Thex->Toidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				Thex->xeidy.hrs, Thex->xeidy.m, 
//				Thex->xeidy.mntime, Thex->xeidy.mn, 
//				Thex->xeidy.mxtime, Thex->xeidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf\n", 
//				Thex->xoidy.hrs, Thex->xoidy.m, 
//				Thex->xoidy.mntime, Thex->xoidy.mn, 
//				Thex->xoidy.mxtime, Thex->xoidy.mx);
//            fprintf(fo, "%1d %3.1lf ", Thex->Qdyes.hhr, Thex->Qdyes.h);
//            fprintf(fo, "%1d %3.1lf ", Thex->Qdyes.chr, Thex->Qdyes.c);      
//            fprintf(fo, "%1d %2.0lf ", Thex->Qdyes.hmxtime, Thex->Qdyes.hmx);
//            fprintf(fo, "%1d %2.0lf\n", Thex->Qdyes.cmxtime, Thex->Qdyes.cmx);
//
//            fprintf(fo, "%1d %3.1lf ", Thex->Qdyel.hhr, Thex->Qdyel.h);
//            fprintf(fo, "%1d %3.1lf ", Thex->Qdyel.chr, Thex->Qdyel.c);      
//            fprintf(fo, "%1d %2.0lf ", Thex->Qdyel.hmxtime, Thex->Qdyel.hmx);
//            fprintf(fo, "%1d %2.0lf\n", Thex->Qdyel.cmxtime, Thex->Qdyel.cmx);
//			
//            fprintf(fo, "%1d %3.1lf ", Thex->Qdyet.hhr, Thex->Qdyet.h);
//            fprintf(fo, "%1d %3.1lf ", Thex->Qdyet.chr, Thex->Qdyet.c);      
//            fprintf(fo, "%1d %2.0lf ", Thex->Qdyet.hmxtime, Thex->Qdyet.hmx);
//            fprintf(fo, "%1d %2.0lf\n", Thex->Qdyet.cmxtime, Thex->Qdyet.cmx);
//		}
//		break;
//	}
//}
//
//void	Thexmonprt(FILE *fo, int id, int Nthex, THEX *Thex)
//{
//	int i ;
//	
//	switch (id)
//	{
//	case 0:
//		if (Nthex > 0)
//            fprintf(fo, "%s %d\n", THEX_TYPE, Nthex);
//		for (i = 0; i < Nthex; i++, Thex++)
//			fprintf(fo, " %s 1 48\n", Thex->name);
//		break;
//		
//	case 1:
//		for (i = 0; i < Nthex; i++, Thex++)
//		{	 
//			fprintf(fo,"%s_Hte H d %s_Te T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_ttne h d %s_Ten t f %s_ttme h d %s_Tem t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo,"%s_Hto H d %s_To T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_ttno h d %s_Ton t f %s_ttmo h d %s_Tom t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			
//			fprintf(fo,"%s_Hxe H d %s_xe T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_txne h d %s_xen t f %s_txme h d %s_xem t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo,"%s_Hxo H d %s_xo T f ", Thex->name, Thex->name);
//			fprintf(fo,"%s_txno h d %s_xon t f %s_txmo h d %s_xom t f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//
//			fprintf(fo, "%s_Hhs H d %s_Qsh Q f %s_Hcs H d %s_Qsc Q f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_ths h d %s_qsh q f %s_tcs h d %s_qsc q f\n\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_Hhl H d %s_Qlh Q f %s_Hcl H d %s_Qlc Q f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_thl h d %s_qlh q f %s_tcl h d %s_qlc q f\n\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_Hht H d %s_Qth Q f %s_Hct H d %s_Qtc Q f\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//			fprintf(fo, "%s_tht h d %s_qth q f %s_tct h d %s_qtc q f\n\n",
//				Thex->name, Thex->name, Thex->name, Thex->name);
//		}
//		break;
//		
//	default:
//		for (i = 0; i < Nthex; i++, Thex++)
//		{
//            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				Thex->mTeidy.hrs, Thex->mTeidy.m, 
//				Thex->mTeidy.mntime, Thex->mTeidy.mn, 
//				Thex->mTeidy.mxtime, Thex->mTeidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf\n", 
//				Thex->mToidy.hrs, Thex->mToidy.m, 
//				Thex->mToidy.mntime, Thex->mToidy.mn, 
//				Thex->mToidy.mxtime, Thex->mToidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
//				Thex->mxeidy.hrs, Thex->mxeidy.m, 
//				Thex->mxeidy.mntime, Thex->mxeidy.mn, 
//				Thex->mxeidy.mxtime, Thex->mxeidy.mx);
//			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf\n", 
//				Thex->mxoidy.hrs, Thex->mxoidy.m, 
//				Thex->mxoidy.mntime, Thex->mxoidy.mn, 
//				Thex->mxoidy.mxtime, Thex->mxoidy.mx);
//            fprintf(fo, "%1d %3.1lf ", Thex->mQdyes.hhr, Thex->mQdyes.h);
//            fprintf(fo, "%1d %3.1lf ", Thex->mQdyes.chr, Thex->mQdyes.c);      
//            fprintf(fo, "%1d %2.0lf ", Thex->mQdyes.hmxtime, Thex->mQdyes.hmx);
//            fprintf(fo, "%1d %2.0lf\n", Thex->mQdyes.cmxtime, Thex->mQdyes.cmx);
//
//            fprintf(fo, "%1d %3.1lf ", Thex->mQdyel.hhr, Thex->mQdyel.h);
//            fprintf(fo, "%1d %3.1lf ", Thex->mQdyel.chr, Thex->mQdyel.c);      
//            fprintf(fo, "%1d %2.0lf ", Thex->mQdyel.hmxtime, Thex->mQdyel.hmx);
//            fprintf(fo, "%1d %2.0lf\n", Thex->mQdyel.cmxtime, Thex->mQdyel.cmx);
//			
//            fprintf(fo, "%1d %3.1lf ", Thex->mQdyet.hhr, Thex->mQdyet.h);
//            fprintf(fo, "%1d %3.1lf ", Thex->mQdyet.chr, Thex->mQdyet.c);      
//            fprintf(fo, "%1d %2.0lf ", Thex->mQdyet.hmxtime, Thex->mQdyet.hmx);
//            fprintf(fo, "%1d %2.0lf\n", Thex->mQdyet.cmxtime, Thex->mQdyet.cmx);
//		}
//		break;
//	}
//}
