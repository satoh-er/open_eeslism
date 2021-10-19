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

/*   tcomfrt.c    */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "MODEL.h" /*---------higuchi 070918------------*/
#include "fesy.h"
#include "fnbld.h"
#include "lib/u_psy.h"
#include "fnlib.h"

/*   作用温度制御時の設定室内空気温度  */

void Rmotset (int Nroom, ROOM *Room)
{
	int i, j;
	double OT, Twi, Trset, a;
	ELOUT  *Eo;
	RMLOAD *rmld;
	TRNX  *trnx;
	RPANEL *rmpnl;
	
	static int  Pint = 0;
	
	/************
	printf("-----Rmotset A\n"); **********/
	
	Fotinit(Nroom, Room);
	
	/**********
	printf("-----Rmotset B\n"); *************/
	
	for (i=0; i < Nroom; i++, Room++)
	{
		
		Eo = Room->cmp->elouts;
		if (Eo->control == LOAD_SW)
		{	 
			rmld = Room->rmld;
			
			if (rmld->tropt == 'o')
			{
				Fotf(Room);	
				
				OT = rmld->Tset;	 
				a = OT - rmld->FOC;
				
				trnx = Room->trnx;
				for (j = 0; j < Room->Ntr; j++, trnx++)
				{
				/****************
				printf("-----Rmotset  j=%d  %s next->Tr=%lf\n",
					j, trnx->nextroom->name, trnx->nextroom->Tr); *******/
					
					a -= rmld->FOTN[j] * trnx->nextroom->Tr;
				}
				
				rmpnl = Room->rmpnl;
				for (j = 0; j < Room->Nrp; j++, rmpnl++)
				{
					if (rmpnl != NULL)
					{
						if (Pint == 0)
						{
							Twi = rmpnl->sd->mw->Tw[rmpnl->sd->mw->mp];
							Pint = 1;
						}
						else
							Twi = rmpnl->pnl->Tpi;
					}
					else
						Twi = 0.0;
					
						/*
						printf("-----Rmotset  j=%d %s Twi=%lf\n",
						j, rmpnl->pnl->name, Twi);
					*/
					
					a -= rmld->FOPL[j] * Twi;
				}
				
				Trset = a / rmld->FOTr;
				Eo->sysv = Room->Tr = Trset;
				
				/***************
				printf("-----Rmotset OT=%lf  Trset=%lf\n", OT, Trset); ************/
				
				/*******
				printf("xxxx Rmotset xxxx Fot=%lf Fotw=%lf FCot=%lf\n",
				rmld->Fot, rmld->Fotw, rmld->FCot);	     
				printf("xxxx Rmotset xxxx   %s  tropt=%c, Tset=%lf Trset=%lf Twi=%lf\n",
				Room->name, rmld->tropt, rmld->Tset, Trset, Twi);
				**************/
				
				if (rmld->loadx != NULL)
				{
					Eo++;
					if (Eo->control == LOAD_SW && rmld->hmopt == 'r')
						Eo->sysv = FNXtr(Trset, rmld->Xset);
				} 
			}
		}
		
	}
}
/* -------------------------------------- */

void Fotinit(int Nroom, ROOM *Room)
{
	int i;
	static char init = 'i';
	
	if (init == 'i')
	{
		for ( i = 0; i < Nroom; i++, Room++)	 	 
		{ 
			if (Room->rmld != NULL)
			{
			/*************
			printf("-----Fotint i=%d %s  Ntr=%d Nrp=%d\n", 
				i, Room->name, Room->Ntr, Room->Nrp); ***************/
				
				free(Room->rmld->FOTN);
				free(Room->rmld->FOPL);
				Room->rmld->FOTN = NULL;
				Room->rmld->FOPL = NULL;

				Room->rmld->FOTN = dcalloc(Room->Ntr, "Fotint");
				Room->rmld->FOPL = dcalloc(Room->Nrp, "Fotint"); 
			}
		}
		init = 'x';
	}
}
/* -------------------------------------- */	 
void Fotf(ROOM *Room)
{
	RMSRF *Sd;
	//static double r = 0.5;
	double	r ;
	double  *ft, a, C = 0.0;
	int     i, k;
	
	// 重み係数が未定義もしくは不適切な数値の場合の対処
	if ( Room->OTsetCwgt == NULL
		|| *(Room->OTsetCwgt) < 0.0 || *(Room->OTsetCwgt) > 1.0 )
		r = 0.5 ;
	else
		r = *(Room->OTsetCwgt) ;

	Sd = Room->rsrf;
	a = 0.0;
	for (i = 0; i < Room->N; i++, Sd++)
	{ 
		a += Sd->A * Sd->WSR;
		C += Sd->A * Sd->WSC;
	}
	Room->rmld->FOTr = r + (1.0 -r) * a / Room->Area ;
	Room->rmld->FOC = (1.0 - r) * C / Room->Area ;
	
	ft = Room->rmld->FOTN;
	for (k = 0; k < Room->Ntr; k++, ft++)
	{
		a = 0.0;
		Sd = Room->rsrf;
		
		for (i = 0; i < Room->N; i++, Sd++)
			a += Sd->A * Sd->WSRN[k];
		
		*ft = (1.0 - r) * a /Room->Area;
	}
	
	ft = Room->rmld->FOPL;
	for (k = 0; k < Room->Nrp; k++, ft++)
	{
		a = 0.0;
		Sd = Room->rsrf;
		
		for (i = 0; i < Room->N; i++, Sd++)
			a += Sd->A * Sd->WSPL[k];
		
		*ft = (1.0 - r) * a /Room->Area;
	}      
}

/* -------------------------------------- */

/*   各室の温熱環境指標計算　　*/


void	Rmcomfrt ( int Nroom, ROOM *Room )
{
	int      i, id;
	double   met, Icl, v;

	met = Icl = v = 0.0 ;
	
	for ( i = 0; i < Nroom; i++, Room++ )
	{
		id = 0 ;
		if ( Room->Metsch != NULL && *Room->Metsch > 0.0 )
		{
			met = *Room->Metsch ;
			id++ ;
		}
		if ( Room->Closch != NULL && *Room->Closch > 0.0 )
		{
			Icl = *Room->Closch ;
			id++ ;
		}
		if ( Room->Wvsch != NULL && *Room->Wvsch > 0.0 )
		{
			v = *Room->Wvsch ;
			id++ ;
		}

		if (id == 3)
		{
			Room->PMV = Pmv0(met, Icl, Room->Tr, Room->xr, Room->Tsav, v);
			Room->SET = SET_star(Room->Tr, Room->Tsav, v, Room->RH, met, Icl, 0., 101.3);
		}
		else 
			Room->PMV = -999.0 ;
			/*************
			printf("**** Rmcomfrt  met=%.1lf Icl=%.1lf v=%.2lf  Tr=%.1lf  xr=%.4lf Tmrt=%.1lf  PMV=%.2lf\n",
			met, Icl, v, Room->Tr, Room->xr, Room->Tsav, Room->PMV);
		*********************/
	}
}
/* ----------------------------------------------------- */

/*   PMVの計算     */


double	Pmv0 ( double met, double Icl, double Tr, double xr, double Tmrt, double v )
/* m [kcal.m2h] */
{
	double	Po = 760.0, eta = 0.0, 
		m, Pa, Tm, hc, hr, fcl, Ifcl, tcl, L;
	
	m = met * 50.0 ;
	Pa = xr * Po / ( xr + 0.62198 ) ;
	hc = 10.4 * sqrt (( double ) v ) ;
	Tm = 0.5 * ( 37.0 + 0.5 * ( Tr + Tmrt )) + 273.15 ;
	hr = 13.6e-8 * Tm * Tm * Tm ;
	fcl = ( Icl < 0.5 ? 1.0 + 0.2 * Icl : 1.05 + 0.1 * Icl ) ;
	Ifcl = 0.18 * Icl * fcl ;
	
	tcl = ( 35.7 - 0.032 * m * ( 1.0 - eta ) + Ifcl * ( hr * Tmrt + hc * Tr ))
		/ ( 1.0 + Ifcl * ( hr + hc )) ;
	
	L = m * ( 0.60135 - 0.0023 * ( 44. - Pa ) - 0.0014 * ( 34. - Tr ))
		+ 0.35 * Pa + 5.95 - 0.6013 * eta
		- fcl * ( hr * ( tcl-Tmrt ) + hc * ( tcl - Tr )) ;
	
	return (( 0.352 * exp ( -0.042 * m ) + 0.032 ) * L ) ;
}

//	SET*の計算
double SET_star(double TA, double TR, double VEL, double RH, double MET, double CLO, double WME, double PATM)
{
	//Input doubleiables ? TA (air temperature): °C, TR (mean radiant temperature): °C, VEL (air velocity): m/s,
	//RH (relative humidity): %, MET: met unit, CLO: clo unit, WME (external work): W/m2, PATM (atmospheric pressure): kPa
	double FindSaturatedVaporPressureTorr(double T);
	double KCLO = 0.25;
	double BODYWEIGHT = 69.9; //kg
	double BODYSURFACEAREA = 1.8258; //m2
	double METFACTOR = 58.2; //W/m2
	double SBC = 0.000000056697; //Stefan-Boltzmann constant (W/m2K4)
	double CSW = 170.0;
	double CDIL = 120.0;
	double CSTR = 0.5;
	int LTIME = 60;
	double PS = FindSaturatedVaporPressureTorr(TA);
	double VaporPressure = RH * PS / 100.0;
	double AirVelocity = dmax(VEL, 0.1);
	double TempSkinNeutral = 33.7;
	double TempCoreNeutral = 36.49;
	double TempBodyNeutral = 36.49;
	double SkinBloodFlowNeutral = 6.3;
	double TempSkin = TempSkinNeutral; //Initial values
	double TempCore = TempCoreNeutral;
	double SkinBloodFlow = SkinBloodFlowNeutral;
	double MSHIV = 0.0;
	double ALFA = 0.1;
	double ESK = 0.1 * MET;
	// 桁落ちによる誤差を避けるため換算係数を変更
	//double PressureInAtmospheres = PATM * 0.009869;
	double PressureInAtmospheres = PATM / 101.3;
	double RCL = 0.155 * CLO;
	double FACL = 1.0 + 0.15 * CLO;
	double LR = 2.2 / PressureInAtmospheres; //Lewis Relation is 2.2 at sea level
	double RM = MET * METFACTOR;
	double M = MET * METFACTOR;
	double ICL, WCRIT;
	if (CLO <= 0) {
		WCRIT = 0.38 * pow(AirVelocity, -0.29);
		ICL = 1.0;
	}
	else {
		WCRIT = 0.59 * pow(AirVelocity, -0.08);
		ICL = 0.45;
	}
	double CHC = 3.0 * pow(PressureInAtmospheres, 0.53);
	double CHCV = 8.600001 * pow((AirVelocity * PressureInAtmospheres), 0.53);
	CHC = dmax(CHC, CHCV);
	double CHR = 4.7;
	double CTC = CHR + CHC;
	double RA = 1.0 / (FACL * CTC); //Resistance of air layer to dry heat transfer
	double TOP = (CHR * TR + CHC * TA) / CTC;
	double TCL = TOP + (TempSkin - TOP) / (CTC * (RA + RCL));
	//TCL and CHR are solved iteratively using: H(Tsk - TOP) = CTC(TCL - TOP),
	//where H = 1/(RA + RCL) and RA = 1/FACL*CTC
	double TCL_OLD = TCL;
	int flag = 1;
	double DRY, HFCS, ERES, CRES, SCR, SSK, TCSK, TCCR, DTSK, DTCR, TB, SKSIG, WARMS, COLDS, CRSIG, WARMC,
		COLDC, BDSIG, WARMB, REGSW, EMAX = 0.0;
	double PWET = 0.0;
	for (int TIM = 1; TIM <= LTIME; TIM++) { //Begin iteration
		for(int i=0;i<100;i++)
		{
			if (flag) {
				TCL_OLD = TCL;
				CHR = 4.0 * SBC * pow(((TCL + TR) / 2.0 + 273.15), 3.0) * 0.72;
				CTC = CHR + CHC;
				RA = 1.0 / (FACL * CTC); //Resistance of air layer to dry heat transfer
				TOP = (CHR * TR + CHC * TA) / CTC;
			}
			TCL = (RA * TempSkin + RCL * TOP) / (RA + RCL);
			flag = 1;
			//double diff = fabs(TCL - TCL_OLD);
			if (fabs(TCL - TCL_OLD) <= 0.01)
				break;
		}
		flag = 0;
		DRY = (TempSkin - TOP) / (RA + RCL);
		HFCS = (TempCore - TempSkin) * (5.28 + 1.163 * SkinBloodFlow);
		ERES = 0.0023 * M * (44.0 - VaporPressure);
		CRES = 0.0014 * M * (34.0 - TA);
		SCR = M - HFCS - ERES - CRES - WME;
		SSK = HFCS - DRY - ESK;
		TCSK = 0.97 * ALFA * BODYWEIGHT;
		TCCR = 0.97 * (1. - ALFA) * BODYWEIGHT;
		DTSK = (SSK * BODYSURFACEAREA) / (TCSK * 60.0); //°C/min
		DTCR = SCR * BODYSURFACEAREA / (TCCR * 60.0); //°C/min
		TempSkin = TempSkin + DTSK;
		TempCore = TempCore + DTCR;
		TB = ALFA * TempSkin + (1. - ALFA) * TempCore;
		SKSIG = TempSkin - TempSkinNeutral;
		WARMS = (SKSIG > 0) * SKSIG;
		COLDS = ((- 1.0 * SKSIG) > 0) * (- 1.0 * SKSIG);
		CRSIG = (TempCore - TempCoreNeutral);
		WARMC = (CRSIG > 0) * CRSIG;
		COLDC = ((- 1.0 * CRSIG) > 0) * (- 1.0 * CRSIG);
		BDSIG = TB - TempBodyNeutral;
		WARMB = (BDSIG > 0) * BDSIG;
		SkinBloodFlow = (SkinBloodFlowNeutral + CDIL *WARMC) / (1. + CSTR * COLDS);
		SkinBloodFlow = dmax(0.5, dmin(90.0, SkinBloodFlow));
		REGSW = CSW *WARMB * exp(WARMS / 10.7);
		REGSW = dmin(REGSW, 500.0);
		double ERSW = 0.68 * REGSW;
		double REA = 1.0 / (LR * FACL * CHC); //Evaporative resistance of air layer
		double RECL = RCL / (LR * ICL); //Evaporative resistance of clothing (icl=.45)
		EMAX = (FindSaturatedVaporPressureTorr(TempSkin) - VaporPressure) / (REA + RECL);
		double PRSW = ERSW / EMAX;
		PWET = 0.06 + 0.94 * PRSW;
		double EDIF = PWET * EMAX - ERSW;
		ESK = ERSW + EDIF;
		//if (TIM == 60)
		//	printf("aqa\n");
		if (PWET > WCRIT)
		{
			PWET = WCRIT;
			PRSW = WCRIT / 0.94;
			ERSW = PRSW * EMAX;
			EDIF = 0.06 * (1.0 - PRSW) * EMAX;
			ESK = ERSW + EDIF;
		}
		if (EMAX < 0) {
			EDIF = 0;
			ERSW = 0;
			PWET = WCRIT;
			PRSW = WCRIT;
			ESK = EMAX;
		}
		ESK = ERSW + EDIF;
		//printf("%d\t%lf\n", TIM, ESK);
		MSHIV = 19.4 * COLDS * COLDC;
		M = RM + MSHIV;
		ALFA = 0.0417737 + 0.7451833 / (SkinBloodFlow + 0.585417);
	} //End iteration
	double HSK = DRY + ESK; //Total heat loss from skin
	double RN = M - WME; //Net metabolic heat production
	double ECOMF = 0.42 * (RN - (1. * METFACTOR));
	if (ECOMF < 0.0) ECOMF = 0.0; //From Fanger
	EMAX = EMAX * WCRIT;
	double W = PWET;
	double PSSK = FindSaturatedVaporPressureTorr(TempSkin);
	double CHRS = CHR; //Definition of ASHRAE standard environment
	//... denoted “S”
	double CHCS;
	if (MET < 0.85) {
		CHCS = 3.0;
	}
	else {
		CHCS = 5.66 * pow(((MET - 0.85)), 0.39);
		CHCS = dmax(CHCS, 3.0);
	}
	double CTCS = CHCS + CHRS;
	double RCLOS = 1.52 / ((MET - WME / METFACTOR) + 0.6944) - 0.1835;
	double RCLS = 0.155 * RCLOS;
	double FACLS = 1.0 + KCLO * RCLOS;
	double FCLS = 1.0 / (1.0 + 0.155 * FACLS * CTCS * RCLOS);
	double IMS = 0.45;
	double ICLS = IMS * CHCS / CTCS * (1. - FCLS) / (CHCS / CTCS - FCLS * IMS);
	double RAS = 1.0 / (FACLS * CTCS);
	double REAS = 1.0 / (LR * FACLS * CHCS);
	double RECLS = RCLS / (LR * ICLS);
	double HD_S = 1.0 / (RAS + RCLS);
	double HE_S = 1.0 / (REAS + RECLS);
	//SET determined using Newton’s iterative solution
	double DELTA = .0001;
	double dx = 100.0;
	double SET, ERR1, ERR2;
	double SET_OLD = TempSkin - HSK / HD_S; //Lower bound for SET
	while (fabs(dx) > .01) {
		ERR1 = (HSK - HD_S * (TempSkin - SET_OLD) - W * HE_S * (PSSK - 0.5 *
			FindSaturatedVaporPressureTorr(SET_OLD)));
		ERR2 = (HSK - HD_S * (TempSkin - (SET_OLD + DELTA)) - W * HE_S * (PSSK - 0.5 *
			FindSaturatedVaporPressureTorr((SET_OLD + DELTA))));
		SET = SET_OLD - DELTA * ERR1 / (ERR2 - ERR1);
		dx = SET - SET_OLD;
		SET_OLD = SET;
	}
	return SET;
}

double FindSaturatedVaporPressureTorr(double T)
{
	//Helper function for pierceSET calculates Saturated Vapor Pressure (Torr) at Temperature T (°C)
	return exp(18.6686 - 4030.183 / (T + 235.0));
}

//double	SET_star(double Hgt, double Wgt, double met, double Icl, double Tr, double xr, double Tmrt, double v)
//{
//	const int dTm = 60;
//	extern double Sgm;
//	// 体表面積(4)
//	double Sfarea;
//	Sfarea = 0.202*pow(Wgt, 0.425)*pow(Hgt, 0.725);
//	// 水蒸気圧
//	double Pa;
//	Pa = FNPwx(xr);				//[kPa]
//	// 着衣の熱抵抗(41)
//	double	Rcl;
//	Rcl = 0.155*Icl;			// [m2K/W]
//	// 着衣面積割合（Acl/AD）(48)
//	double	fcl;
//	fcl = 1. + 0.3*Icl;			// [-]
//	// ルイス係数[K/kPa]　(27)の下
//	double	LR = 16.5;
//
//	// 対流熱伝達率の計算[W/m2K](Table 6)
//	double	SET_hc(double met, double v);
//	double	hc = SET_hc(met, v);
//	// 放射熱伝達率[W/m2K]（初期値：(36)）
//	double	hr = 4.7;			// 田辺研プログラムによる（放射率1.0相当）
//
//	double	SkBF = 6.3,			// 皮膚血流量の初期値
//			Tb = 36.5,			// 体温の初期値
//			Tsk = 33.7,			// 皮膚温度の初期値
//			Tcr = 36.8;			// コア温度の初期値
//	double	K = 5.28,			// 皮膚とコア間の熱コンダクタンス[W/m2K]
//			Cpb = 3490.,		// 人体の比熱[J/kgK]
//			Cbl = 4190.,		// 血液の比熱[J/kgK]
//			Ccr = 3500.,		// コアの比熱[J/kgK]
//			Csk = 3500.;		// 皮膚の比熱[J/kgK]
//	double	Tcl;				// 着衣表面温度
//	double	Esk = 0.1*met;		// 皮膚からの蒸発熱損失（初期値）[W/m2]
//	double	alp = 0.1;			// 体重の内の皮膚の割合（初期値）[-]
//	Tcl = (Tsk + Tcr) / 2.;
//	// 60分間放置したときの計算をする
//	for (int Time = 0; Time < 3600; Time+=dTm)
//	{
//		double	Tcld;			// 着衣表面温度
//		double	OT;				// 作用温度
//		double	Ra;				// 着衣の外表面から周囲環境までの熱伝達抵抗
//		// 着衣表面温度と放射熱伝達率の収束計算
//		for (int i = 0; i < 20; i++)
//		{
//			// 総合熱伝達率[W/m2K]
//			double	hi = hc + hr;
//
//			// 着衣の外表面から周囲環境までの熱伝達抵抗
//			Ra = 1. / (fcl*hi);
//
//			// 作用温度の計算
//			OT = (hr*Tmrt + hc*Tr) / hi;
//			// 着衣表面温度の計算（熱収支(Tsk-Tcl)/(Ra+Rcl)=hi(Tcl-OT)）
//			Tcld = (Ra*Tsk + Rcl*OT) / (Ra + Rcl);
//
//			// 放射熱伝達率の計算(35)
//			hr = 4.*Sgm*pow((Tcld + Tmrt) / 2. + 273.15, 3.0)*0.72;
//
//			// 収束判定
//			if (abs(Tcl - Tcld) <= 0.01)
//				break;
//			Tcl = Tcld;
//		}
//
//		// 皮膚からの顕熱損失量[W/m2](11)
//		double	CR = (Tsk - OT) / (Ra + Rcl);
//		// 皮膚とコア間の熱伝達量[W/m2](73)
//		double	qcr_sk = (K + SkBF*Cbl)*(Tcr - Tsk);
//		// 呼吸による潜熱損失[W/m2](26)
//		double	Eres = 0.0173*met*(5.87 - Pa);
//		// 呼吸による顕熱損失[W/m2](25)
//		double	Cres = 0.0014*met*(34.0 - Tr);
//
//		// コアの蓄熱量（熱収支）[W/m2](73)
//		double	Scr = met - qcr_sk - Eres - Cres;
//		// 皮膚の蓄熱量[W/m2](1)
//		double	Ssk = qcr_sk - CR - Esk;
//
//		// コアの熱容量[J/K](2)
//		double	Hcapcr = Cpb*Wgt*(1. - alp);
//		// 皮膚の熱容量[J/K](3)
//		double	Hcapsk = Cpb*Wgt*alp;
//
//		// コアの1分間の温度変化[K]
//		double	dTcr = (Scr*Sfarea) / Hcapcr / (double)dTm;
//		// 皮膚の1分間の温度変化[K]
//		double	dTsk = (Ssk*Sfarea) / Hcapsk / (double)dTm;
//		// 次の時刻のコア、皮膚温度の計算（前進差分）
//		Tsk = Tsk + dTsk*(double)dTm;
//		Tcr = Tcr + dTcr*(double)dTm;
//		// 体温の計算
//		Tb = alp*Tsk + (1. - alp)*Tcr;
//	}
//
//	// Standard着衣面積率用
//	double stdclo = 0.25;
//}
//
//double	SET_hc(double met, double v)
//{
//	// 対流熱伝達率の計算[W/m2K]
//	double	hc = 3.1;			// 椅座状態（V<0.2）
//	double	hcact = 5.7*pow(met - 0.8, 0.39);	// 静止空気中での活動状態
//	double	hcwlk = 8.6*pow(v, 0.52);			// 静止空気中での徒歩
//	if (met < 0.85)
//		hcact = 0.;
//	// 最大値を採用する（田辺研のプログラムの方法）
//	return(dmax(hc, dmax(hcact, hcwlk)));
//}

