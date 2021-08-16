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

/* mcstanklb.c  */


#include <math.h>
#include "fesy.h"
#include "fnlib.h"

#define TSTOLE  0.04


/*  蓄熱槽仮想分割　*/


void stoint(double dTM, int N, double Vol, double KAside, double KAtop, double KAbtm,
			double *dvol, double *Mdt, double *KS, double *Tss, double *Tssold,
			int *Jva, int *Jvb)
{
	extern double  row, cw;
	int		i;
	double	*K ;
	
	K = KS ;
	for (i = 0; i < N; i++, dvol++, Mdt++, K++, Tss++, Tssold++ )
	{
		*dvol = Vol / N;
		*Mdt = (cw * row * Vol / N) / dTM;
		*K = KAside / N;
		
		*Tss = *Tssold;
	}
	*KS += KAtop;
	*(KS+N-1) += KAbtm;
	
	*Jva = *Jvb = 0;
	
}
/* ----------------------------------------------------------- */

void stofc(int N, int Nin, int *Jcin, int *Jcout, 
		   char *ihex, double *ihxeff, int Jva, int Jvb, double *Mdt, double *KS,
		   double gxr, double *Tenv, double *Tssold, double *cGwin, double *EGwin,
		   double *B, double *R, double *d, double *fg)
{
	int  i, j, k, N2, Jin, Jo;
	
	/**********
	printf("stofc  === N=%d Nin=%d\n", N, Nin);
	for (i = 0; i < Nin; i++)
	printf("stofc  ===  i=%d   cGwin=%lf\n", i, cGwin[i]);
	***********/
	
	N2 = N*N;
	for (j = 0; j < N2; j++)
		B[j] = 0.0;
	
	for (j = 0; j < N; j++)
	{  
		B[j*N+j] = Mdt[j] + KS[j];
		R[j] = Mdt[j] * Tssold[j] + KS[j] * *Tenv;
	}
	
	for (j = 0; j < N-1; j++)
	{
		B[j*N+j+1] = -Mdt[j] * gxr;
		B[(j+1)*N+j] = -Mdt[j+1] * gxr;
    }
	
	if (Jva >= 0)
	{ 
		for (j=Jva; j<=Jvb; j++)
		{ 
			B[j*N+j+1] = -Mdt[j]*1.0e6;
			B[(j+1)*N+j]= -Mdt[j]*1.0e6;
		}
	}
	
	for (i = 0; i < Nin; i++)
	{
		Jin = Jcin[i];
		if (cGwin[i] > 0.0)
		{
			B[Jin*N+Jin] += EGwin[i];
			
			if (Jin < Jcout[i])
			{
				for (j = Jin + 1; j <= Jcout[i]; j++)
					B[j*N+j-1] -= cGwin[i];
			}
			else if (Jin > Jcout[i])
			{ 
				for (j = Jcout[i]; j < Jin; j++)
					B[j*N+j+1] -= cGwin[i];
			}
		}
	}
    
	for (j = 1; j < N-1; j++)
		B[j*N+j] += fabs(B[j*N+j-1]) + fabs(B[j*N+j+1]);
	
	B[0] += fabs(B[1]);
	B[N*N-1] += fabs(B[N*N-2]);
	
	
	/***************** 
	printf("--- stocf   N=%d\n", N);
	seqprint (" %11.4e", N, B, "%10.3lf" ,R);
	*******************/
	
	matinv(B, N, N, "<stofc>" );
	matmalv(B, R, N, N, d);
	
	/********************* 
	printf("--- stocf  inv(B)   N=%d\n", N);   
	seqprint (" %11.4e", N, B, "%10.3lf" ,d); 
	********************/
	
	for (k = 0; k < Nin; k++)
	{     
		Jo = Jcout[k];
		if (ihex[k] == 'y')
		{
			d[Jo] *= ihxeff[k];
			for (i = 0; i < Nin; i++)
			{
				Jin = Jcin[i];
				//*fg = B[Jo * N + Jin] * EGwin[i];
				*fg = B[Jo * N + Jin] * EGwin[i] * ihxeff[k];
				if (k == i)
					*fg += (1.0 - ihxeff[k]);  	   
				fg++;
			}
		}
		else
		{
			for (i = 0; i < Nin; i++)
			{
				Jin = Jcin[i];
				*fg = B[Jo * N + Jin] * EGwin[i];
				fg++;
			}
		}      
	}
	
	
}

/* -------------------------------------------------------------- */

/*  蓄熱槽水温の計算　*/

void stotss(int N, int Nin, int *Jcin, double *B, double *R, double *EGwin, double *Twin,
			double *Tss)
{
	int  i;
	
	for (i = 0; i < Nin; i++, Jcin++, EGwin++, Twin++)
		*(R+ *Jcin) += *EGwin * *Twin;
	
	matmalv(B, R, N, N, Tss);
	
	/******  printf("stotss ---\n");  
	for (i = 0; i < N; i++)
	printf("stotss  i=%d Tss=%lf\n", i, Tss[i]);     
	***************/
	/************************ 
	for (i = 0; i < N; i++)
	Tssold[i] = Tss[i];
	**************************/
}

/* -------------------------------------------------------------- */ 

/*  蓄熱槽水温分布の検討　*/

void stotsexm(int N, double *Tss, int *Jva, int *Jvb, char *dtankF, char *cfcalc)
{
	int j;
	
	*Jvb= -1;
	*Jva= -1;
	for(j = N-2; j >= 0; --j)
	{
		if (dtankF[j] == TANK_FULL)
		{
			if(Tss[j + 1] > (Tss[j] + TSTOLE))
				*Jvb = j;
			if(*Jvb >= 0)
				break;
		}
	}
	if( *Jvb >= 0)
	{
		for(j= *Jvb - 1; j >= 0; --j)
		{
		/***printf("<<stotsexm>> j=%d jvb=%d f=%c Tss[jvb+1]=%lf  Tss[j]=%lf\n",
			j, *Jvb, dtankF[j], Tss[*Jvb + 1], Tss[j]);***/
			
			if (dtankF[j] == TANK_FULL)
			{
				if(Tss[*Jvb + 1] > (Tss[j] + TSTOLE)) 
					*Jva=j;
			}
		}
		if( *Jva == -1)  *Jva = *Jvb;
	}  
	if (*Jva < 0)
		*cfcalc = 'n';
	else
		*cfcalc = 'y';
}
/*-----------------------------------------------------------------*/



