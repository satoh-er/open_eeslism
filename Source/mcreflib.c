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

/*   mc_reflib.c                     */                           

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "esize.h"
#include "fesy.h"
#include "fnmcs.h"
#include "fnlib.h"
#include "psy.h"
#include <math.h>


/*  圧縮式冷凍機定格特性入力    */

void Refcmpdat(FILE *frf, int *Nrfcmp, RFCMP *Rfcmp)
{
	RFCMP  *rfc;
	char   s[SCHAR];
	int i;
	
	rfc = Rfcmp;
	
	while (fscanf(frf, "%s", s), s[0] != '*')
	{
        Rfcmp->name = stralloc(s);
		
		fscanf(frf, "%s", s);
		Rfcmp->cname = stralloc(s);
		
		for (i=0; i<4; i++)
			fscanf(frf, "%lf", &Rfcmp->e[i]);
		for (i=0; i<4; i++)
			fscanf(frf, "%lf", &Rfcmp->d[i]);
		for (i=0; i<4; i++)
			fscanf(frf, "%lf", &Rfcmp->w[i]);
		fscanf(frf, "%lf %lf %lf %lf", &Rfcmp->Teo[0], &Rfcmp->Teo[1], 	
			&Rfcmp->Tco[0], &Rfcmp->Tco[1]);
		fscanf(frf, "%lf", &Rfcmp->Meff);
		
		Rfcmp++;	
	}
	*Nrfcmp = (int)(Rfcmp - rfc);
	if ( *Nrfcmp >= RFCMPLSTMX)
		printf("xxxxxxxxx Refcfi   reflist=%d [max=%d]\n", *Nrfcmp, RFCMPLSTMX);
}
/* ----------------------------------- */

/*  冷凍機の蒸発温度と冷凍能力の一次式の係数  */

void Compca(double *e, double *d, double EGex, double *Teo, double Ta, 
			double *Ho, double *He)
{ 
	double  Tc, Te, Qo[2];
	int    i;
	
	for(i=0; i<2; i++)
	{ Te= Teo[i];
    Tc= (d[0] + d[1]*Te + EGex*Ta)
		/(EGex - d[2] - d[3]*Te);
    Qo[i]= e[0] + e[1] * Te + (e[2] + e[3] * Te)*Tc; 
    
	/* --------- 
	printf("xxxx Compca xxx Te=%lf  Tc=%lf   Qo[i]=%lf\n", Te,Tc,Qo[i]);
	---------- */
	}
	*He = (Qo[0]-Qo[1])/(Teo[1] - Teo[0]);
	*Ho = Qo[0] + *He * Teo[0];
}
/* ------------------------------------------------------------ */

/*  ヒ－トポンプの凝縮温度と冷凍能力の一次式の係数  */

void Compha(double *e, double *d, double EGex, double *Tco, double Ta,
			double *Ho, double *He)
{ 
	double  Tc, Te, Qo[2];
	int    i;
	
	for(i=0; i<2; i++)
	{ Tc= Tco[i];
    Te= (e[0] + e[2]*Tc + EGex*Ta)
		/(EGex - e[1] - e[3]*Tc);
    Qo[i]= d[0] + d[2] * Tc + (d[1] + d[3] * Tc)*Te; 
    /* -----
	printf("xxx Compha xxx Te=%lf  Tc=%lf   Qo[i]=%lf\n", Te,Tc,Qo[i]);
	------------- */
	}
	*He = (Qo[0]-Qo[1])/(Tco[1] - Tco[0]);
	*Ho = Qo[0] + *He * Tco[0];
}  
/* --------------------------------------- */

/*  冷凍機／ヒ－トポンプの軸動力の計算　　 */


double Refpow(REFA *Rf, double QP)
{
	double  W=0.0, Te=0.0, Tc=0.0;
	
	/****   Tx = (Rf->Ho - QP)/Rf->He; ****/
	
	if (fabs((double) QP) > 1.0)
	{
		if (Rf->chmode == COOLING_SW)
		{ 
			Te = QP / (Rf->cat->cool->eo * Rf->cG) + Rf->Tin;;  
			Tc =(QP - Rf->c_e[0] - Rf->c_e[1]*Te) / (Rf->c_e[2] + Rf->c_e[3]*Te);
			W = Rf->c_w[0] + Rf->c_w[1]*Te + Rf->c_w[2]*Tc + Rf->c_w[3]*Te*Tc;
		}
		else if (Rf->chmode == HEATING_SW)
		{ 
			Tc =  QP / (Rf->cat->heat->eo * Rf->cG) + Rf->Tin;
			Te =(QP - Rf->h_d[0] - Rf->h_d[2]*Tc)/(Rf->h_d[1] + Rf->h_d[3]*Tc);
			W = Rf->h_w[0] + Rf->h_w[1]*Te + Rf->h_w[2]*Tc + Rf->h_w[3]*Te*Tc;
		}
		
		/*****
		printf("xxx Refpow xxx Te=%lf  Tc=%lf   Qp=%lf\n", Te,Tc,QP); 
		*****/
		
		Rf->Te = Te;
		Rf->Tc = Tc;
	}
	else
		W = 0.0;
	
	return( W );
}


