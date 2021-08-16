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

#include "fesy.h"
#include "fnmcs.h"
#include "lib/u_psy.h"
#include "mc/mcaclib.h"

/************ #include "cmpref.h"
*************/
/*  冷媒コイルの処理熱量計算用係数   */

/********************
void	dxcoil ( char wet, HPCH *Rf, double Toa, double Gaet, double Gaeh,
				double Tai, double xai, ACS *Et, ACS *Ex )
{ 
	extern double ca, cv, ro;
	double  EGex, cs, ae,be, Do,Dt,Dx,Dc,Ee;
	EGex= Rf->eex * ca *Rf->Gex;
	
	if (Rf->mode == 'C')
		Compca(Rf->e, Rf->d, EGex, Rf->Teo, Toa, &Rf->Ho, &Rf->He);
	else
		Compha(Rf->e, Rf->d, EGex, Rf->Tco, Toa, &Rf->Ho, &Rf->He);
	
	if (wet == 'd')
	{
		Do= ca*Gaet + Rf->He;
		Dt= ca*Gaet/Do;
		Dc= Rf->Ho /Do;
		Et->w = Et->x =0.0;
		Et->t = ca*Gaet*(1.0-Dt);
		Et->C = -ca*Gaet*Dc;
		Ex->w = Ex->t = Ex->x = Ex->C =0.0; }
	else
	{
		hstaircf(Rf->Teo[0]+3.0, Rf->Teo[1],&ae,&be);
		cs=ca+cv*xai;
		Do= Gaeh*be + Rf->He;
		Dt= Gaeh*cs/Do;
		Dx= Gaeh*ro/Do;
		Dc= (Rf->Ho - Gaeh*ae)/Do;
		
		Ee= (Gaeh*be -Gaet*ca)/ro;
		Et->w = 0.0;
		Et->t = ca*Gaet*(1.0-Dt);
		Et->x = -ca*Gaet*Dx;
		Et->C = -ca*Gaet*Dc;
		
		Ex->w = 0.0;
		Ex->t = (Gaeh*cs-Gaet*ca)/ro-Ee*Dt;
		Ex->x = Gaeh-Ee*Dx; 
		Ex->C = -(Gaeh*ae/ro + Ee*Dc); }
}
/**********************************/

/* ------------------------------------------ */

/*  冷温水コイルの処理熱量計算用係数   */


void wcoil(char Air_SW, char Water_SW, char wet, double Gaet, double Gaeh,
		   double xai, double Twi, ACS *Et, ACS *Ex, ACS *Ew)
{
	extern double  ca, cv, ro;
	double  aw, bw, cs;
	if (wet == 'd' || Water_SW == OFF_SW || Air_SW == OFF_SW) 
	{
		// 片側系統が停止していたときに対応するように修正
		// Satoh Debug 2009/1/9
		if ( Water_SW != OFF_SW )
		{
			Et->w = Et->t = ca*Gaet;
			Et->x = Et->C =0.0;
		}
		else
			Et->w = Et->t = Et->x = Et->C = 0.0;
			
		Ex->w = Ex->t = Ex->x = Ex->C = 0.0; 

		if ( Air_SW != OFF_SW )
		{
			Ew->w = Ew->t = Gaet*ca;
			Ew->x = Ew->C = 0.0;
		}
		else
			Ew->w = Ew->t = Ew->x = Ew->C = 0.0;
	}
	else
	{
		hstaircf(Twi, Twi+5.0, &aw, &bw);
		cs=ca+cv*xai;
		Et->w = Et->t = ca*Gaet;
		Et->x = Et->C =0.0;
		
		Ex->w = (Gaeh*bw-Gaet*ca)/ro;
		Ex->t = (Gaeh*cs-Gaet*ca)/ro;
		Ex->x = Gaeh;
		Ex->C = -Gaeh*aw/ro; 
		
		Ew->w = Gaeh*bw;
		Ew->t = Gaeh*cs;
		Ew->x = Gaeh*ro;
		Ew->C = -Gaeh*aw;
	}
}
/* ------------------------------------------ */

/*  コイル供給顕熱  */

double Qcoils(ACS *Et, double Tai, double xai, double Twi)
{  
	return (Et->w * Twi - Et->t * Tai - Et->x *xai - Et->C) ;
}
/* ------------------------------------------ */

/*  コイル供給潜熱  */

double Qcoill(ACS *Ex, double Tai, double xai, double Twi)
{ 
	extern double ro;
	
	return (ro*(Ex->w * Twi - Ex->t * Tai - Ex->x *xai - Ex->C)) ;
}
/* ------------------------------------------ */

/*  水温に等しい飽和空気のエンタルピの一次式近似の係数 */

void hstaircf(double Tw1, double Tw2, double *a, double *b)
{
	double  h1, h2;
	
	h1 = FNH((double) Tw1, FNXtr((double) Tw1, 100.0));
	h2 = FNH((double) Tw2, FNXtr((double) Tw2, 100.0));
	*b = (h2-h1)/(Tw2-Tw1);
	*a = h1 - *b * Tw1;
	
	/* -------------
    printf("== hstaircf  Tw1,Tw2=%lf  %lf    a=%lf   b=%lf\n",Tw1,Tw2,*a,*b);
	---------------- */
}


