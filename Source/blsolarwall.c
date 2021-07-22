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

// 空気集熱建材のシミュレーション

#include <stdio.h>
#include <math.h>
//#include "exs.h"
//#include "fbld.h"
//#include "wthrd.h"
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"
#include "fnfio.h"
#include "waterandair.h"

// 集熱器の放射取得熱量
double	FNScol(double ta, double I, double EffPV, double Ku, double ao, double Eo, double Fs, double RN )
{
	return (( ta - EffPV ) * I - Ku / ao * Eo * Fs * RN ) ;
}

// 建材一体型空気集熱器の相当外気温度を計算する
void  CalcSolarWallTe(RMVLS *Rmvls, WDAT *Wd, EXSFS *Exsfs)
{
	RDPNL	*rdpnl ;
	int		i ;
	
	rdpnl = Rmvls->Rdpnl ;
	for (i = 0; i < Rmvls->Nrdpnl; i++, rdpnl++)
	{
		RMSRF	*Sd ;

		Sd = rdpnl->sd[0] ;
		if (Sd->mw != NULL && Sd->mw->wall->WallType == 'C')
		{
			Sd->Tcole = FNTcoleContrl(Sd, Wd, Exsfs) ;
			//printf("Tcole=%.2lf\n", Sd->Tcole) ;
		}

		//printf ("ColTe=%lf\n", Sd->ColTe) ;
	}
}

// 集熱器相当外気温度の計算（制御用）
// 集熱器裏面温度は前時刻の値を使用する
double FNTcoleContrl(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs)
{
	double Cidf;
	WALL	*Wall ;
	EXSF	*Exs ;
	double	Glsc ;
	double	Ksu, alo, ku, kd ;

	if (Sd->mw->wall->ColType != NULL)
	{
		Wall = Sd->mw->wall;
		Exs = &Exsfs->Exs[Sd->exs];
		Glsc = 1.0;
		Cidf = 1.0;
		if (Sd->mw->wall->ColType != NULL && strncmp(Sd->mw->wall->ColType, "A2", 2) != 0 && strncmp(Sd->mw->wall->ColType, "W2", 2) != 0)
		{
			Glsc = Glscid(Exs->cinc);
			Cidf = 0.91;
		}

		// 熱貫流率等の計算
		if (Wall->chrRinput == 'Y')
		{
			FNKc(Wd, Exsfs, Sd);
			Ksu = Sd->dblKsu;
			alo = Sd->dblao;
			ku = Sd->ku;
			kd = Sd->kd;
		}
		else	// 熱貫流率が入力値の場合
		{
			Ksu = Wall->Ksu;
			alo = *Exs->alo;
			ku = Wall->ku;
			kd = Wall->kd;
		}

		//Sd->Scol = FNScol(Wall->tra, Glsc * Exs->Idre + Cidf * Exs->Idf,
		//	Sd->PVwall.Eff, Ksu, alo, Wall->Eo, Exs->Fs, Wd->RN) ;
		//printf("Ko=%g Scol=%g Ku=%g Ta=%g Kd=%g Tx=%g\n",
		//	Wall->Ko, Sd->Scol, Wall->Ku, Wd->T, Wall->Kd, Sd->oldTx ) ;

		// Satoh DEBUG 2018/2/26  壁体一体型空気集熱器への影の影響を考慮するように修正
		Sd->Tcoled = Sd->oldTx;
		double Idre, Idf, RN;
		Idre = Exs->Idre;
		Idf = Exs->Idf;
		RN = Exs->rn;
		
		Sd->dblSG = (Wall->tra - Sd->PVwall.Eff) * (Glsc * Idre + Cidf * Idf);
		if (strncmp(Sd->mw->wall->ColType, "A3", 2) == 0)
			Sd->Tcoled += Sd->dblSG / Sd->dblKsd;

		Sd->Tcoleu = Sd->dblSG / Ksu - Wall->Eo * RN / alo + Wd->T;

		//return ( Wall->ku * ( Sd->Scol / Wall->Ksu + Wd->T ) + Wall->kd * Sd->oldTx ) ;

		//printf("name=%s ku=%.2lf kd=%.2lf Tcoleu=%.2lf Tcoled=%.2lf\n", Sd->name, ku, kd, Sd->Tcoleu, Sd->Tcoled) ;
		return (ku * Sd->Tcoleu + kd * Sd->Tcoled);
	}
	else
		return 0;
}

// 建材一体型空気集熱パネルの境界条件計算
void	FNBoundarySolarWall(RMSRF *Sd, double *ECG, double *ECt, double *CFc)
{
	WALL	*Wall ;
	RDPNL	*Pnl ;
	double	Kc, Kcd, Kcu, ku, kd ;
	
	Wall = Sd->mw->wall ;
	Pnl = Sd->rpnl ;
	
	// 戻り値の初期化
	*ECG = 0.0 ;

	// 各種熱貫流率の設定
	if(Wall->chrRinput == 'Y')
	{
		Kc = Sd->dblKc ;
		Kcu = Sd->dblKcu ;
		Kcd = Sd->dblKcd ;
		ku = Sd->ku ;
		kd = Sd->kd ;
	}
	else
	{
		Kc = Wall->Kc ;
		Kcu = Wall->Kcu ;
		Kcd = Wall->Kcd ;
		ku = Wall->ku ;
		kd = Wall->kd ;
	}

	// パネル動作時
	if ( Pnl->cG > 0. )
	{
		//double ECGKo ;

		*ECG = Pnl->cG * Pnl->Ec / (Kc * Sd->A) ;
	}
	//printf("cG=%lf  EC=%lf\n", Pnl->cG, *ECG ) ;

	*ECt = Kcd * (( 1. - *ECG ) * ku -1. ) ;
	*CFc = Kcd * ( 1. - *ECG ) * kd ;

	//printf("ECG=%lf ECt=%lf Ewc=%lf CFc=%lf\n",*ECG,*ECt,*Ewc,*CFc) ;
	//}
	//else
	//{
		//*ECt = Wall->Kd * ( Wall->KdKo - 1. ) ;
		//*CFc = Wall->Kd ;
	//}
}

// 熱媒平均温度の計算
double	FNTf(double Tcin, double Tcole, double ECG)
{
	return((1. - ECG) * Tcole + ECG * Tcin) ;
}

// 外表面の総合熱伝達率の計算
double FNSolarWallao(WDAT *Wd, RMSRF *Sd, EXSFS *Exsfs)
{
	double dblac, dblar, dblao ;
	extern double Sgm, PI ;
	EXSF  *Exs ;
	double dblWdre ;
	double dblWa ;
	double dblu ;

	// 放射熱伝達率
	// 屋根の表面温度は外気温度で代用
	dblar = Sd->Eo * 4. * Sgm * pow((Wd->T + Sd->Tg) / 2.0 + 273.15, 3. ) ;

	// 対流熱伝達率
	Exs = &Exsfs->Exs[Sd->exs] ;

	// 外部風向の計算（南面0゜に換算）
	dblWdre = Wd->Wdre * 22.5 - 180. ;
	// 外表面と風向のなす角
	dblWa = (double)Exs->Wa - dblWdre ;

	// 風上の場合
	if(cos(dblWa * PI / 180.) > 0.)
	{
		if(Wd->Wv <= 2.)
			dblu = 2.0 ;
		else
			dblu = 0.25 * Wd->Wv ;
	}
	else
		dblu = 0.3 + 0.05 * Wd->Wv ;

	// 対流熱伝達率
	dblac = 3.5 + 5.6 * dblu ;

	// 総合熱伝達率
	dblao = dblar + dblac ;

	return dblao ;
}

// 通気層の放射熱伝達率の計算
double VentAirLayerar(double dblEsu, double dblEsd, double dblTsu, double dblTsd)
{
	extern double Sgm ;
	double dblEs ;

	// 放射率の計算
	dblEs = 1. / (1. / dblEsu + 1. / dblEsd - 1.) ;

	return 4.0 * dblEs * Sgm * pow((dblTsu + dblTsd) / 2. + 273.15, 3.) ;
}

// 通気層の強制対流熱伝達率（ユルゲスの式）
double FNJurgesac(RMSRF *Sd, double dblV, double a, double b)
{
	//double dblTemp ;
	double Dh, Nu, Re, lam ;

	//if(fabs(dblV) <= 1.0e-3)
	//	dblTemp = 3.0 ;
	//else if(dblV <= 5.0)
	//	dblTemp = 7.1 * pow(dblV, 0.78) ;
	//else
	//	dblTemp = 5.8 + 3.9 * dblV ;

	// 長方形断面の相当直径の計算
	Dh = 1.232 * (a * b) / ( a + b) ;
	// レイノルズ数の計算
	Re = dblV * Dh / FNanew(Sd->dblTf) ;
	// 空気の熱伝導率
	lam = FNalam(Sd->dblTf) ;
	// ヌセルト数の計算
	Nu = 0.0158 * pow(Re, 0.8) ;
	return Nu / Dh * lam ;
}

// 屋根一体型空気集熱器の熱伝達率、熱貫流率の計算
void	FNKc(WDAT *Wd, EXSFS *Exsfs, RMSRF *Sd)
{
	double dblDet, dblWsuWsd, Ru, Cr, Cc ;
	extern double roa, PI ;
	WALL	*Wall ;
	EXSF		*Exs ;
	double	rad ;

	rad = PI / 180. ;
	Wall = Sd->mw->wall ;

	// 外表面の総合熱伝達率の計算
	Sd->dblao = FNSolarWallao(Wd, Sd, Exsfs) ;
	
	Exs = &Exsfs->Exs[Sd->exs] ;
	// 通気層の対流熱伝達率の計算
	if(Wall->air_layer_t < 0.)
		printf("%s  通気層の厚さが未定義です\n", Sd->name) ;
	if(Sd->rpnl->cmp->elouts->G > 0.)
		Sd->dblacc = FNJurgesac(Sd, Sd->rpnl->cmp->elouts->G / roa / ((Sd->dblWsd + Sd->dblWsu) / 2. * Wall->air_layer_t),
			(Sd->dblWsd + Sd->dblWsu) / 2., Wall->air_layer_t ) ;
	else
		Sd->dblacc = FNVentAirLayerac(Sd->dblTsu, Sd->dblTsd, Wall->air_layer_t, Exs->Wb * rad) ;

	if(fabs(Sd->dblacc) > 100. || Sd->dblacc < 0.)
		printf("xxxxxx <FNKc> name=%s acc=%lf\n", Sd->name, Sd->dblacc) ;
	// 通気層の放射熱伝達率の計算
	Sd->dblacr = VentAirLayerar(Wall->dblEsu, Wall->dblEsd, Sd->dblTsu, Sd->dblTsd) ;
	//printf("ac=%lf ar=%lf Tsu=%lf Tsd=%lf\n", Sd->dblacc, Sd->dblacr, Sd->dblTsu, Sd->dblTsd) ;
	if(fabs(Sd->dblacr) > 100. || Sd->dblacr < 0.)
		printf("xxxxxx <FNKc> name=%s acr=%lf\n", Sd->name, Sd->dblacr) ;

	// 通気層上面、下面から境界までの熱貫流率の計算
	if(Wall->Ru >= 0.)
		Ru = Wall->Ru ;
	else
	{
		// 空気層のコンダクタンスの計算
		// 放射成分
		Cr = VentAirLayerar(Wall->Eg, Wall->Eb, Sd->Tg, Sd->dblTsu) ;
		// 対流成分
		Cc = FNVentAirLayerac(Sd->Tg, Sd->dblTsu, Wall->ta, Exs->Wb * rad ) ;
		Ru = 1. / ( Cc + Cr ) ;
		Sd->ras = Ru ;
		//printf("Tg=%lf Tb=%lf Cr=%lf Cc=%lf\n", Sd->Tg, Sd->dblTsu, Cr, Cc) ;
	}
	Sd->dblKsu = 1. / (Ru + 1. / Sd->dblao) ;
	Sd->dblKsd = 1. / Wall->Rd ;

	dblWsuWsd = Sd->dblWsu / Sd->dblWsd ;

	dblDet = (Sd->dblacr * dblWsuWsd + Sd->dblacc + Sd->dblKsd) * (Sd->dblacr + Sd->dblacc + Sd->dblKsu) - Sd->dblacr * Sd->dblacr * dblWsuWsd ;
	Sd->dblb11 = (Sd->dblacr + Sd->dblacc + Sd->dblKsu) / dblDet ;
	Sd->dblb12 = Sd->dblacr / dblDet ;
	Sd->dblb21 = Sd->dblacr * dblWsuWsd / dblDet ;
	Sd->dblb22 = (Sd->dblacr * dblWsuWsd + Sd->dblacc + Sd->dblKsd) / dblDet ;

	Sd->dblfcu = Sd->dblacc / dblWsuWsd * Sd->dblb12 + Sd->dblacc * dblWsuWsd * Sd->dblb22 ;
	Sd->dblfcd = Sd->dblacc / dblWsuWsd * Sd->dblb11 + Sd->dblacc * dblWsuWsd * Sd->dblb21 ; 

	Sd->dblKcu = Sd->dblKsu * Sd->dblfcu ;
	Sd->dblKcd = Sd->dblKsd * Sd->dblfcd ;

	// 集熱器の総合熱貫流率の計算
	Sd->dblKc = Sd->dblKcu + Sd->dblKcd ;

	Sd->ku = Sd->dblKcu / Sd->dblKc  ;
	Sd->kd = Sd->dblKcd / Sd->dblKc  ;

	//printf("ao=%.2lf acc=%.2lf acr=%.2lf Ksu=%.3lf Ksd=%.3lf Kcu=%.3lf Kcd=%.3lf Kc=%.3lf\n",
	//	Sd->dblao, Sd->dblacc, Sd->dblacr, Sd->dblKsu, Sd->dblKsd, Sd->dblKcu, Sd->dblKcd, Sd->dblKc) ;
}

// 空気集熱器の通気層上面、下面表面温度の計算
void FNTsuTsd(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs)
{
	double dblTf ;		// 集熱空気の平均温度
	RDPNL	*Rdpnl ;
	double	cG, ECG, Kc, Ksu, Ksd ;
	WALL	*Wall ;
	EXSF	*Exs ;

	Rdpnl = Sd->rpnl ;
	Wall = Sd->mw->wall ;
	Exs = &Exsfs->Exs[Sd->exs] ;
	cG = Sd->rpnl->cG ;
	dblTf = 0.0 ;
	if( Wall->chrRinput == 'Y')
	{
		Kc = Sd->dblKc ;
		Ksu = Sd->dblKsu ;
		Ksd = Sd->dblKsd ;
		//}
		//else
		//{
		//	Kc = Wall->Kc ;
		//	Ksu = Wall->Ksu ;
		//	Ksd = Wall->Ksd ;
		//}

		// 集熱空気の平均温度
		//Sd->dblTf = (Rdpnl->Tpi + Rdpnl->Tpo) / 2.0 ;
		ECG = cG * Rdpnl->Ec / (Kc * Sd->A) ;
		Sd->dblTf = ( 1. - ECG ) * Sd->Tcole + ECG * Rdpnl->Tpi ;
		Sd->dblTsd = Sd->dblb11 * Ksd * (Sd->Tcoled - Sd->dblTf) 
			+ Sd->dblb12 * Ksu * (Sd->Tcoleu - Sd->dblTf) + Sd->dblTf ;
		Sd->dblTsu = Sd->dblb21 * Ksd * (Sd->Tcoled - Sd->dblTf) 
			+ Sd->dblb22 * Ksu * (Sd->Tcoleu - Sd->dblTf) + Sd->dblTf ;

		if(Sd->dblTsd < -100)
			printf("Error\n") ;

		// 空気層の熱抵抗が未定義の場合はガラスの温度を計算する
		if(Wall->Ru < 0.)
		{
			Sd->Tg = (Sd->dblao * Wd->T + 1. / Sd->ras * Sd->dblTsu + Wall->ag * Exs->Iw - Wall->Eo * Exs->Fs * Wd->RN) / ( Sd->dblao + 1. / Sd->ras) ;
			//if(Sd->Tg < 0.)
			//	printf("xxxxxxxx\n") ;
		}
	}
}

// 通気層の集熱停止時の熱コンダクタンス[W/m2K]
double	FNVentAirLayerac(double Tsu, double Tsd, double air_layer_t, double Wb)
{
	double Tas, Ra, anew, a, dblTemp, RacosWb, lama ;
	extern double g ;
	double max(double a, double b) ;
	double	Tsud ;

	if(fabs(Tsu - Tsd) < 1.e-4)
		Tsud = Tsu + 0.1 ;
	else
		Tsud = Tsu ;
	
	// 通気層の温度
	Tas = ( Tsud + Tsd ) / 2.0 ;
	// 空気の動粘性係数
	anew = FNanew(Tas) ;
	// 空気の熱拡散率
	a = FNaa(Tas) ;
	// 空気の熱伝導率
	lama = FNalam(Tas) ;
	// レーリー数
	Ra = g * (1. / Tas) * fabs(Tsud - Tsd) * pow(air_layer_t, 3.0) / (anew * a) ;

	RacosWb = Ra * cos(Wb) ;

	dblTemp = (1. + 1.44 * max(0., 1. - 1708./RacosWb) 
		* (1. - (pow(sin(1.8*Wb),1.6) * 1708. / RacosWb)) 
		+ max(pow(RacosWb / 5830.,1./3.)-1.,0.)) * air_layer_t / lama ;

	return ( dblTemp ) ;
}
