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

/*  psylib                                  */
/* -----------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lib/u_psy.h"
#include "lib/u_minmax.h"

/* ==================================================================

 PSYLIB

 湿り空気の状態値計算用ライブラリ－
 （宇田川、パソコンによる空気調和計算法、プログラム3.1の C 言語版, ANSI C 版）

 --------------------------------------------------------------------- */

double R0, Ca, Cv, Rc, Cc, Cw, Pcnv, P;

void Psyint(void)
{
	extern double  R0, Ca, Cv, Rc, Cc, Cw, Pcnv, P;
	extern char    *UNIT;
	if (strcmp(UNIT, "SI") == 0)
	{
		P = 101.325;
		R0 = 2501000.;
		Ca = 1005.;
		Cv = 1846.;
		Rc = 333600.;
		Cc = 2093.;
		Cw = 4186.;
		Pcnv = 1.0;
	}
	else
	{
		P = 760.;
		R0 = 597.5;
		Ca = .24;
		Cv = .441;
		Rc = 79.7;
		Cc = .5;
		Cw = 1.0;
		Pcnv = 7.50062;
	}
}
/* ----------------------------------------- */
void Poset(double Po)
{
	extern double P;
	P = Po;
}
/* ----------------------------------------- */
double FNPo(void)
{
	extern double P;
	return (P);
}
/* ----------------------------------------- */
double FNPws(double T)
{
	extern double Pcnv;
	double Tabs, Pws, Temp;
	Tabs = T + 273.15;
	if (fabs(Tabs) < 1.e-5)
		printf("xxxx ゼロ割が発生しています Tabs=%lf\n", Tabs);

	if (T > 0.)
	{
		Temp = 6.5459673 * log(Tabs);
		Temp += -5800.2206 / Tabs;
		Temp += 1.3914993;
		Temp += Tabs * (-.048640239 +
			Tabs * (4.1764768E-5 - 1.4452093E-8 * Tabs));
		Pws = exp(Temp);
	}
	else
		Pws = exp(-5674.5359 / Tabs + 6.3925247 + Tabs*(-9.677843E-3 +
		Tabs*(6.2215701E-7 + Tabs*(2.0747825E-9 - 9.484024E-13*Tabs)))
		+ 4.1635019*log(Tabs));

	//printf("Tabs=%lf Temp=%lf Pws=%lf\n", Tabs, Temp, Pws) ;
	return  (Pcnv*Pws / 1000.);
}
/* ------------------------------------- */
double FNDp(double Pw)
{
	extern double Pcnv;
	double Pwx, Y;
	Pwx = Pw*1000. / Pcnv;
	Y = log(Pwx);
	if (Pwx >= 611.2)
		return (-77.199 + Y*(13.198 + Y*(-.63772 + .071098*Y)));
	else
		return (-60.662 + Y*(7.4624 + Y*(.20594 + .016321*Y)));
}
/* ---------------------------------------- */
double FNDbrp(double Rh, double Pw)
{
	return (FNDp(100.0 / Rh*Pw));
}
/* ---------------------------------------- */
double FNDbxr(double X, double Rh)
{
	return (FNDbrp(Rh, FNPwx(X)));
}
/* ---------------------------------------- */
double FNDbxh(double X, double H)
{
	extern double  R0, Ca, Cv;
	return ((H - R0*X) / (Ca + Cv*X));
}
/* ---------------------------------------- */
double FNDbxw(double X, double Twb)
{
	extern double  R0, Ca, Cv;
	double Hc;
	Hc = FNHc(Twb);
	return ((Ca*Twb + (Cv*Twb + R0 - Hc)*FNXp(FNPws(Twb)) - (R0 - Hc)*X)
		/ (Ca + Cv*X));
}
/* ---------------------------------------- */
double FNDbrh(double Rh, double H)
{
	double T0, F, Fd, Dbrh = 0.0;
	int    I;
	T0 = dmin(FNDbxh(0., H), 30.);
	for (I = 1; I <= 10; I++)
	{
		F = H - FNH(T0, FNXtr(T0, Rh));
		Fd = (H - FNH(T0 + .1, FNXtr(T0 + .1, Rh)) - F) / .1;
		Dbrh = T0 - F / Fd;
		if (fabs(Dbrh - T0) <= .02) return (Dbrh);
		T0 = Dbrh;
	}
	printf("XXX FNDbrh  (T-T0)=%lf\n", Dbrh - T0);
	return (Dbrh);
}
/* ---------------------------------------- */
double FNDbrw(double Rh, double Twb)
{
	double T0, F, Fd, Dbrw = 0.0;
	int    I;
	T0 = Twb;
	for (I = 1; I <= 10; I++)
	{
		F = T0 - FNDbxw(FNXtr(T0, Rh), Twb);
		Fd = (T0 + .1 - FNDbxw(FNXtr(T0 + .1, Rh), Twb) - F) / .1;
		Dbrw = T0 - F / Fd;
		if (fabs(Dbrw - T0) <= .02) return (Dbrw);
		T0 = Dbrw;
	}
	printf("XXX FNDbrw  (T-T0)=%lf\n", Dbrw - T0);
	return (Dbrw);
}
/* -------------------------------------------- */
double FNDbhw(double H, double Twb)
{
	double T0, F, Fd, Dbhw = 0.0;
	int I;
	T0 = Twb;
	for (I = 1; I <= 10; I++)
	{
		F = T0 - FNDbxw(FNXth(T0, H), Twb);
		Fd = (T0 + .1 - FNDbxw(FNXth(T0 + .1, H), Twb) - F) / .1;
		Dbhw = T0 - F / Fd;
		if (fabs(Dbhw - T0) <= .02) return (Dbhw);
		T0 = Dbhw;
	}
	printf("XXX FNDbhw  (T-T0)=%lf\n", Dbhw - T0);
	return (Dbhw);
}
/* ------------------------------------- */
double FNXp(double Pw)
{
	extern double P;
	if (fabs(P - Pw) < 1.0e-4)
		printf("xxxxx ゼロ割が発生しています P=%lf Pw=%lf\n", P, Pw);
	return (.62198*Pw / (P - Pw));
}
/* ------------------------------------- */
double FNXtr(double T, double Rh)
{
	return FNXp(FNPwtr(T, Rh));
}
/* ------------------------------------- */
double FNXth(double T, double H)
{
	extern double  R0, Ca, Cv;
	return (H - Ca*T) / (Cv*T + R0);
}
/* ------------------------------------ */
double FNXtw(double T, double Twb)
{
	extern double R0, Ca, Cv;
	double Hc;
	Hc = FNHc(Twb);
	return (((R0 + Cv*Twb - Hc)*FNXp(FNPws(Twb)) - Ca*(T - Twb)) / (Cv*T + R0 - Hc));
}
/* ------------------------------------- */
double FNPwx(double X)
{
	extern double P;
	return (X*P / (X + .62198));
}
/* ------------------------------------ */
double FNPwtr(double T, double Rh)
{
	return (Rh*FNPws(T) / 100.0);
}
/* ------------------------------------ */
double FNRhtp(double T, double Pw)
{
	return (100.0*Pw / FNPws(T));
}
/* ------------------------------------ */
double FNRhtx(double T, double X)
{
	return (FNRhtp(T, FNPwx(X)));
}
/* ------------------------------------ */
double FNH(double T, double X)
{
	extern double  R0, Ca, Cv;
	return (Ca*T + (Cv*T + R0)*X);
}
/* ------------------------------------ */
double FNWbtx(double T, double X)
{
	double Tw0, H, Xs, Xss, F, Fd, Wbtx = 0.0;
	int I;
	Tw0 = T;
	H = FNH(T, X);
	for (I = 1; I <= 10; I++)
	{
		Xs = FNXp(FNPws(Tw0));
		F = FNH(Tw0, Xs) - H - (Xs - X)*FNHc(Tw0);
		Xss = FNXp(FNPws(Tw0 + .1));
		Fd = (FNH(Tw0 + .1, Xss) - H - (Xss - X)*FNHc(Tw0 + .1) - F) / .1;
		Wbtx = Tw0 - F / Fd;
		if (fabs(Wbtx - Tw0) <= .02) return (Wbtx);
		Tw0 = Wbtx;
	}
	printf("XXX FNWbtx  (Twb-Tw0)=%lf\n", Wbtx - Tw0);
	return (Wbtx);
}
/* -------------------------------------- */
double FNHc(double Twb)
{
	extern double  Rc, Cc, Cw;
	double Hc;
	if (Twb >= 0.)
		Hc = Cw*Twb;
	else
		Hc = -Rc + Cc*Twb;
	return (Hc);
}
/* ------------------------------------ */

