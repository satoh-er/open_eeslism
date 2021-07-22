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

#include <stdio.h>
#include <math.h>
#include "waterandair.h"
//#include "fnlib.h"

// 水、空気の物性値の計算
// パソコンによる空気調和計算法より作成

// 空気の密度　 dblT[℃]、出力[kg/m3]
double FNarow(double dblT)
{
    return 1.293 / (1. + dblT / 273.15) ;
}

// 空気の比熱　 dblT[℃]、出力[J/kgK]
double FNac()
{
    return 1005.;
}

// 空気の熱伝導率　 dblT[℃]、出力[W/mK]
double FNalam(double dblT)
{
	double dblTemp ;

    if ( dblT > -50. && dblT < 100.)
        dblTemp = 0.0241 + 0.000077 * dblT ;
    else
        dblTemp = -999. ;

	return dblTemp ;
}

// 空気の粘性係数　 dblT[℃]、出力[Pa s]
double FNamew(double dblT)
{
    return (0.0074237 / (dblT + 390.15) * pow((dblT + 273.15) / 293.15, 1.5)) ;
}

// 空気の動粘性係数　 dblT[℃]、出力[m2/s]
double FNanew(double dblT)
{
	double FNamew(double dblT) ;
	double FNarow(double dblT) ;

    return FNamew(dblT) / FNarow(dblT) ;
}

// 空気の膨張率　 dblT[℃]、出力[1/K]
double FNabeta(double dblT)
{
    return 1. / (dblT + 273.15) ;
}

// 水の密度　 dblT[℃]、出力[kg/m3]
double FNwrow(double dblT)
{
	double dblTemp ;

    if (dblT > 0. && dblT < 100. )
        dblTemp = 1000.5 - 0.068737 * dblT - 0.0035781 * dblT * dblT ;
	else if (dblT < 200.)
        dblTemp = 1008.7 - 0.28735 * dblT - 0.0021643 * dblT * dblT ;
    else
        dblTemp = -999. ;

	return dblTemp ;
}

// 水の比熱　 dblT[℃]、出力[J/kgK]
double FNwc(double dblT)
{
	double dblTemp ;

    if (dblT > 0. && dblT < 100.)
        dblTemp = 4210.4 - 1.356 * dblT + 0.014588 * dblT * dblT ;
    else if (dblT < 200.)
        dblTemp = 4306.8 - 2.7913 * dblT + 0.018773 * dblT * dblT ;
    else
        dblTemp = -999. ;
    
	return dblTemp ;
}

// 水の熱伝導率　 dblT[℃]、出力[W/mK]
double FNwlam(double dblT)
{
	double dblTemp ;

    if (dblT > 0. && dblT < 100.)
        dblTemp = 0.56871 + 0.0018421 * dblT - 7.0427e-6 * dblT * dblT ;
    else if (dblT < 200.)
        dblTemp = 0.60791 + 0.0012032 * dblT - 4.7025e-6 * dblT * dblT ;
    else
        dblTemp = -999. ;
    
	return dblTemp ;
}

// 水の動粘性係数　 dblT[℃]、出力[m2/s]
double FNwnew(double dblT)
{
	double dblTemp ;

    if (dblT > 0. && dblT < 50.)
        dblTemp = exp(-13.233 - 0.032516 * dblT + 0.000068997 * dblT * dblT
                + 0.0000069513 * dblT * dblT * dblT - 0.00000009386 * dblT * dblT* dblT* dblT) ;
	else if (dblT < 100.)
        dblTemp = exp(-13.618 - 0.015499 * dblT - 0.000022461 * dblT * dblT
                + 0.00000036334 * dblT * dblT* dblT) ;
	else if (dblT < 200.)
        dblTemp = exp(-13.698 - 0.016782 * dblT + 0.000034425 * dblT * dblT) ;
    else
        dblTemp = -999. ;
    
	return dblTemp ;
}

// 水の膨張率　 dblT[℃]、出力[1/K]
double FNwbeta(double dblT)
{
	double dblTemp ;

    if (dblT > 0. && dblT < 50.)
        dblTemp = -0.060159 + 0.018725 * dblT - 0.00045278 * dblT * dblT 
                + 0.0000098148 * dblT * dblT * dblT  - 0.000000083333 * dblT * dblT * dblT * dblT ;
    else if (dblT < 100.)
        dblTemp = -0.46048 + 0.03104 * dblT - 0.000325 * dblT * dblT 
                + 0.0000013889 * dblT * dblT * dblT ;
    else if (dblT < 200.)
        dblTemp = 0.33381 + 0.002847 * dblT + 0.000016154 * dblT * dblT ;
    else
		dblTemp = -999. ;
    
	return dblTemp ;
}

// 水の粘性係数　 dblT[℃]、出力[Pa s]
double FNwmew(double dblT)
{
	double FNwnew(double dblT) ;
	double FNwrow(double dblT) ;

    return  FNwnew(dblT) / FNwrow(dblT) ;
}

// 空気の熱拡散率　 dblT[℃]、出力[m2/s]
double FNaa(double dblT)
{
	double FNalam(double dblT) ;
	double FNarow(double dblT) ;

    return  FNalam(dblT) / FNac(dblT) / FNarow(dblT) ;
}

// 水の熱拡散率　 dblT[℃]、出力[m2/s]
double FNwa(double dblT)
{
	double FNwlam(double dblT) ;
	double FNwc(double dblT) ;
	double FNwrow(double dblT) ;

    return  FNwlam(dblT) / FNwc(dblT) / FNwrow(dblT) ;
}

// プラントル数の計算
double FNPr(char strF, double dblT)
{
	double FNanew(double dblT) ;
	double FNaa(double dblT) ;
	double FNwnew(double dblT) ;
	double FNwa(double dblT) ;

	double dblTemp ;

    if (strF == 'A' || strF == 'a')
        dblTemp = FNanew(dblT) / FNaa(dblT) ;
    else if (strF == 'W' || strF == 'w')
        dblTemp = FNwnew(dblT) / FNwa(dblT) ;
    else
        dblTemp = -999. ;
    
	return dblTemp ;
}

// グラスホフ数の計算
//   dblTs:表面温度[℃]
//   dblTa:主流温度[℃]
//   dblx :代表長さ[m]
double FNGr(char strF, double dblTs, double dblTa, double dblx)
{
    double dblg, dblBeta, dblT, dbldT ;
    double n ;
	double	dmax(double a, double b);
    
    dblg = 9.80665 ;
    
    dblT = (dblTs + dblTa) / 2. ;
    
	// 温度差の計算
    dbldT = dmax(fabs(dblTs - dblTa), 0.1) ;
    
    if (strF == 'A' || strF == 'a')
	{
        dblBeta = FNabeta(dblT) ;
        n = FNanew(dblT) ;
	}
    else if (strF == 'W' || strF == 'w')
	{
        dblBeta = FNwbeta(dblT) ;
        n = FNwnew(dblT) ;
	}
    else
	{
        dblBeta = -999. ;
        n = -999. ;
	}
    
    return dblg * dblBeta * dbldT * pow(dblx, 3.) / (n * n) ;
}

// 各種定数を入力するとヌセルト数が計算される。
//   Nu = C * (Pr Gr)^m
double FNCNu(double dblC, double dblm, double dblPrGr)
{
    return dblC * pow(dblPrGr, dblm) ;
}

// 管内の強制対流熱伝達率の計算（流体は水のみ）
// dbld:配管内径[m]
// dblL:管長[m]
// dblv:管内流速[m/s]
// dblT:流体と壁面の平均温度[℃]
double FNhinpipe(double dbld, double dblL, double dblv, double dblT)
{
	double dblRe, dblPr, dblnew, dblTemp ;
	double dbldL, dblld ;

	dblnew = FNwnew(dblT) ;
	dblRe = dblv * dbld / dblnew ;
	dblPr = FNPr('W', dblT) ;
	dbldL = dbld / dblL ;
	dblld = FNwlam(dblT) / dbld ;

	if (dblRe < 2200.)
		dblTemp = (3.66 + 0.0668 * dbldL * dblRe * dblPr 
			/ (1. + 0.04 * pow(dbldL * dblRe * dblPr, 2./3.))) * dblld ;
	else
		dblTemp = 0.023 * pow(dblRe, 0.8) * pow(dblPr, 0.4) * dblld ;

	return dblTemp ;
}

// 円管外部の自然対流熱伝達率の計算（流体は水のみ）
// dbld:配管内径[m]
// dblT:流体と壁面の平均温度[℃]
double FNhoutpipe(double dbld, double dblTs, double dblTa)
{
	double dblNu, dblC, dbln, dblGr, dblPr ;

	dblC = 0.5 ;
	dbln = 0.25 ;

	dblPr = FNPr('W', (dblTs + dblTa) / 2.) ;
	dblGr = FNGr('W', dblTs, dblTa, dbld) ;

	dblNu = dblC * pow(dblPr * dblGr, dbln) ;
	return dblNu / dbld ;
}
