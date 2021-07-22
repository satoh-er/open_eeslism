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

/*  pump.c  */

/*  ポンプ   */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnlib.h"

/* 機器仕様入力　　　　　　*/

int Pumpdata (char *cattype, char *s, PUMPCA *Pumpca, int Npfcmp, PFCMP *pfcmp)
{
	char *st;
	double dt;
	int   id=0, i;
	PFCMP	*pfc ;
	
	if ((st=strchr(s,'=')) == 0)
	{
		Pumpca->name = stralloc(s);
		Pumpca->type = NULL ;
		Pumpca->Wo = Pumpca->Go = Pumpca->qef = -999.0 ;
		Pumpca->val = NULL ;
		
		if ( strcmp ( cattype, PUMP_TYPE ) == 0 )
			Pumpca->pftype = PUMP_PF ;
		else if (strcmp(cattype, FAN_TYPE) == 0)
			Pumpca->pftype = FAN_PF ;
		else
			Pumpca->pftype = OFF_SW ;
	}
	else
	{ 
		*st = '\0';
		if ( strcmp(s, "type") == 0)
		{
			Pumpca->type = stralloc(st+1);
			if ( strcmp(Pumpca->type, "P") == 0)
				Pumpca->val = dcalloc(4, "Pumpdata");

			pfc = pfcmp ;
			for (i = 0; i < Npfcmp; i++, pfc++)
			{
				if (Pumpca->pftype == pfc->pftype
					&& strcmp(Pumpca->type, pfc->type) == 0)
				{
					Pumpca->pfcmp = pfc ;
					break ;
				}
			}
		}
		else
		{ 
			dt=atof(st+1);
			if (strcmp(s,"qef") == 0)
				Pumpca->qef= dt;
			else
			{
				if (strcmp(Pumpca->type, "P") != 0)
				{
					if (strcmp(s, "Go") == 0)
						Pumpca->Go =dt;
					else if (strcmp(s, "Wo") == 0)
						Pumpca->Wo = dt;
					else 
						id = 1;
				}
				else
				{
					if (strcmp(s, "a0") == 0)
						Pumpca->val[0] = dt;
					else if (strcmp(s, "a1") == 0)
						Pumpca->val[1] = dt;
					else if (strcmp(s, "a2") == 0)
						Pumpca->val[2] = dt;
					else if (strcmp(s, "Ic") == 0)
						Pumpca->val[3] = dt;
					else
						id = 1;
				}
			}
		}
	}
	return (id);
}

/* --------------------------- */

/* 太陽電池ポンプの太陽電池パネルの方位設定　*/

void Pumpint(int Npump, PUMP *Pump, int Nexsf, EXSF *Exs)
{
	int i, j;
	EXSF *exs;
	
	for (i = 0; i < Npump; i++, Pump++)
	{
		if (strcmp(Pump->cat->type, "P") == 0)
		{
			Pump->sol = NULL;
			exs = Exs;
			for (j = 0; j < Nexsf; j++, exs++)
			{
				if(strcmp(Pump->cmp->exsname, exs->name) == 0)
					Pump->sol = exs;
			}
			if (Pump->sol == NULL)
				Eprint("Pumpint",Pump->cmp->exsname);
			
				/****printf("<Pumpint>  i=%d  sol=%s\n", i, Pump->sol->name);
			*****/
		}
	}
}
/* --------------------------- */

/* ポンプ流量設定（太陽電池ポンプのみ） */

void Pumpflow(int Npump, PUMP *Pump)
{
	int		i;
	double	S;
	extern int	DEBUG ;
	
	for (i = 0; i < Npump; i++, Pump++)
	{
		if (strcmp(Pump->cat->type, "P") == 0)
		{
			S = Pump->sol->Iw;
			
			if ( DEBUG )
			{
				printf("<Pumpflow> i=%d S=%lf Ic=%lf a0=%lf a1=%e a2=%e\n",
					i, S, Pump->cat->val[3], Pump->cat->val[0],
					Pump->cat->val[1], Pump->cat->val[2]);
			}

			if (S > Pump->cat->val[3])
				Pump->G = Pump->cat->val[0] +
				(Pump->cat->val[1] + Pump->cat->val[2] * S) * S;
			else
				Pump->G = -999.0;
			
			Pump->E = 0;
		}
		else
		{
			if (Pump->cmp->control != OFF_SW)
			{
				Pump->G = Pump->cat->Go;
				Pump->E = Pump->cat->Wo;	 
			}
			else
			{
				Pump->G = -999.0;
				Pump->E = 0.0;
			}
			
			if ( DEBUG )
			{
				printf("<Pumpflow>  control=%c G=%lf E=%lf\n", 
					Pump->cmp->control, Pump->G, Pump->E);
			}
		}      
	}
}

/* --------------------------- */

/*  特性式の係数  */

void Pumpcfv(int Npump, PUMP *Pump)
{
	ELOUT *Eo;
	double cG;
	int i;
	
	for (i = 0; i < Npump; i++, Pump++)
	{   
		if (Pump->cmp->control != OFF_SW)
		{      
			Eo = Pump->cmp->elouts;
			Pump->cG = cG = spcheat(Eo->fluid) * Eo->G;
			Eo->coeffo = cG;
			Pump->PLC = PumpFanPLC ( Eo->G / Pump->G, Pump ) ;
			Eo->Co = Pump->cat->qef * Pump->E * Pump->PLC ;
			*(Eo->coeffin) =  -cG;
			
			if ( Pump->cat->pftype == FAN_PF )
			{
				Eo++ ;
				Eo->coeffo = Eo->G ;
				Eo->Co = 0.0 ;
				*(Eo->coeffin) = - Eo->G ;
			}
		}
		else
		{
			Pump->G = 0.0 ;
			Pump->E = 0.0 ;
		}
	}
}

// -----------------------------------------
//
//  ポンプ、ファンの部分負荷特性曲線
//

double	PumpFanPLC ( double XQ, PUMP *Pump )
{
	char	Err[SCHAR] ;
	double	Buff, dQ ;
	int		i ;
	PUMPCA	*cat ;
	
	dQ = min ( 1.0, max ( XQ, 0.25 )) ;
	cat = Pump->cat ;

	if (cat->pfcmp == NULL)
	{
		sprintf ( Err, "<PumpFanPLC>  PFtype=%c  type=%s", cat->pftype, cat->type ) ;
		Eprint ( "PUMP oir FAN", Err ) ;
		Buff = 0.0 ;
	}
	else
	{
		Buff = 0.0 ;
		
		for ( i = 0; i < 5; i++ )
			Buff += cat->pfcmp->dblcoeff[i] * pow ( dQ, ( double ) i ) ;
	}
	return ( Buff ) ;
}

/* --------------------------- */

/*  供給熱量、エネルギーの計算 */

void Pumpene(int Npump, PUMP *Pump)
{
	int i;
	ELOUT *Eo;
	
	for (i = 0; i < Npump; i++, Pump++)
	{
		Pump->Tin = Pump->cmp->elins->sysvin;
		Eo = Pump->cmp->elouts;
		
		if (Eo->control != OFF_SW)
			Pump->Q = Pump->cG * (Eo->sysv - Pump->Tin);
		else
			Pump->Q = 0.0;      
	}
}

/* --------------------------- */


void pumpprint(FILE *fo, int id, int Npump, PUMP *Pump)
{
	int i;
	double G;
	
	switch (id)
	{
	case 0:
		if (Npump > 0)
            fprintf(fo, "%s  %d\n", PUMP_TYPE, Npump); 
		for (i = 0; i < Npump; i++, Pump++)
			fprintf(fo," %s 1 6\n", Pump->name);
		break;
		
	case 1:	 
		for (i = 0; i < Npump; i++, Pump++)
		{
			fprintf(fo,"%s_c c c %s_Ti t f %s_To t f ",
				Pump->name, Pump->name, Pump->name);
			fprintf(fo,"%s_Q q f  %s_E e f %s_G m f\n",
				Pump->name, Pump->name, Pump->name);
		}
		break;
		
	default:
		for (i = 0; i < Npump; i++, Pump++)
		{
			if (Pump->cmp->elouts->G > 0.0 && Pump->cmp->elouts->control != OFF_SW)
				G = Pump->cmp->elouts->G;
			else
				G = 0.0;
			
            fprintf(fo, "%c %4.1lf %4.1lf %4.0lf %4.0lf %.5g\n",
				Pump->cmp->elouts->control, 
				Pump->Tin, Pump->cmp->elouts->sysv, Pump->Q, Pump->E * Pump->PLC, G);
		}
		break;
	}
}
/* --------------------------- */

/* 日積算値に関する処理 */

void pumpdyint(int Npump, PUMP *Pump)
{
	int i;
	
	for (i = 0; i < Npump; i++, Pump++)
	{
		
		edyint(&Pump->Qdy);
		edyint(&Pump->Edy);
		edyint(&Pump->Gdy);
	}
}

void pumpmonint(int Npump, PUMP *Pump)
{
	int i;
	
	for (i = 0; i < Npump; i++, Pump++)
	{
		
		edyint(&Pump->mQdy);
		edyint(&Pump->mEdy);
		edyint(&Pump->mGdy);
	}
}

void pumpday(int Mon, int Day, int ttmm, int Npump, PUMP *Pump, int Nday, int SimDayend)
{
	int i;
	int Mo, tt ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	for (i = 0; i < Npump; i++, Pump++)
	{
		// 日集計
		edaysum(ttmm, Pump->cmp->control, Pump->Q, &Pump->Qdy);
		edaysum(ttmm, Pump->cmp->control, Pump->E, &Pump->Edy);
		edaysum(ttmm, Pump->cmp->control, Pump->G, &Pump->Gdy);
		
		// 月集計
		emonsum(Mon, Day, ttmm, Pump->cmp->control, Pump->Q, &Pump->mQdy, Nday, SimDayend);
		emonsum(Mon, Day, ttmm, Pump->cmp->control, Pump->E, &Pump->mEdy, Nday, SimDayend);
		emonsum(Mon, Day, ttmm, Pump->cmp->control, Pump->G, &Pump->mGdy, Nday, SimDayend);

		// 月・時刻のクロス集計
		emtsum(Mon, Day, ttmm, Pump->cmp->control, Pump->E, &Pump->mtEdy[Mo][tt]);
	}
}

void pumpdyprt(FILE *fo, int id, int Npump, PUMP *Pump)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npump > 0)
            fprintf(fo, "%s  %d\n",PUMP_TYPE, Npump); 	 
		for (i = 0; i < Npump; i++, Pump++)
			fprintf(fo," %s 1 12\n", Pump->name);
		break;
		
	case 1:	    
		for (i = 0; i < Npump; i++, Pump++)
		{	    
			fprintf(fo, "%s_Hq H d %s_Q Q f %s_tq h d %s_Qm q f\n",
				Pump->name, Pump->name, Pump->name, Pump->name);
			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
				Pump->name, Pump->name, Pump->name, Pump->name);
			fprintf(fo, "%s_Hg H d %s_G M f %s_tg h d %s_Gm m f\n\n",
				Pump->name, Pump->name, Pump->name, Pump->name);
		}
		break;
		
	default:
		for (i = 0; i < Npump; i++, Pump++)
		{      
            fprintf(fo, "%1d %3.1lf ", Pump->Qdy.hrs, Pump->Qdy.d);
            fprintf(fo, "%1d %2.0lf ", Pump->Qdy.mxtime, Pump->Qdy.mx);
			
            fprintf(fo, "%1d %3.1lf ", Pump->Edy.hrs, Pump->Edy.d);
            fprintf(fo, "%1d %2.0lf ", Pump->Edy.mxtime, Pump->Edy.mx);
			
            fprintf(fo, "%1d %3.1lf ", Pump->Gdy.hrs, Pump->Gdy.d);      
            fprintf(fo, "%1d %2.0lf\n", Pump->Gdy.mxtime, Pump->Gdy.mx);
		} 
		break;
	}
}

void pumpmonprt(FILE *fo, int id, int Npump, PUMP *Pump)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npump > 0)
            fprintf(fo, "%s  %d\n",PUMP_TYPE, Npump); 	 
		for (i = 0; i < Npump; i++, Pump++)
			fprintf(fo," %s 1 12\n", Pump->name);
		break;
		
	case 1:	    
		for (i = 0; i < Npump; i++, Pump++)
		{	    
			fprintf(fo, "%s_Hq H d %s_Q Q f %s_tq h d %s_Qm q f\n",
				Pump->name, Pump->name, Pump->name, Pump->name);
			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
				Pump->name, Pump->name, Pump->name, Pump->name);
			fprintf(fo, "%s_Hg H d %s_G M f %s_tg h d %s_Gm m f\n\n",
				Pump->name, Pump->name, Pump->name, Pump->name);
		}
		break;
		
	default:
		for (i = 0; i < Npump; i++, Pump++)
		{      
            fprintf(fo, "%1d %3.1lf ", Pump->mQdy.hrs, Pump->mQdy.d);
            fprintf(fo, "%1d %2.0lf ", Pump->mQdy.mxtime, Pump->mQdy.mx);
			
            fprintf(fo, "%1d %3.1lf ", Pump->mEdy.hrs, Pump->mEdy.d);
            fprintf(fo, "%1d %2.0lf ", Pump->mEdy.mxtime, Pump->mEdy.mx);
			
            fprintf(fo, "%1d %3.1lf ", Pump->mGdy.hrs, Pump->mGdy.d);      
            fprintf(fo, "%1d %2.0lf\n", Pump->mGdy.mxtime, Pump->mGdy.mx);
		} 
		break;
	}
}

void pumpmtprt(FILE *fo, int id, int Npump, PUMP *Pump, int Mo, int tt)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Npump > 0)
            fprintf(fo, "%s %d\n", PUMP_TYPE, Npump);
		for (i = 0; i < Npump; i++, Pump++)
			fprintf(fo, " %s 1 1\n", Pump->name);
		break;
		
	case 1:
		for (i = 0; i < Npump; i++, Pump++)
		{	 
			fprintf(fo,"%s_E E f \n", Pump->name);
		}
		break;
		
	default:
		for (i = 0; i < Npump; i++, Pump++)
		{
			fprintf(fo, " %.2lf \n", Pump->mtEdy[Mo-1][tt-1].d * cff_kWh);
		}
		break;
	}
}

void	PFcmpInit(int Npfcmp, PFCMP *Pfcmp)
{
	int	i ;
	
	for ( i=0; i < Npfcmp; i++, Pfcmp++ )
	{
		Pfcmp->pftype = ' ' ;
		Pfcmp->type = NULL ;
		matinit(Pfcmp->dblcoeff, 5 ) ;
	}
}

void	PFcmpdata(FILE *fl, int *Npfcmp, PFCMP *pfcmp)
{
	int		i ;
	char	s[SCHAR], c ;
	PFCMP	*Cmp ;

	Cmp = pfcmp ;
	while ( fscanf(fl, "%s", s) != EOF, s[0] != '*' )
	{
		if (strcmp(s, "!") == 0)
		{
			while ( fscanf ( fl,"%c", &c), c != '\n' )
				;
		}
		else
		{
			if (strcmp(s, PUMP_TYPE) == 0)
				pfcmp->pftype = PUMP_PF ;
			else if (strcmp(s, FAN_TYPE) == 0)
				pfcmp->pftype = FAN_PF ;
			else
				Eprint("<pumpfanlst.efl>", s) ;
			
			fscanf(fl, "%s", s) ;
			pfcmp->type = stralloc(s) ;
			
			i = 0 ;
			while (fscanf(fl, "%s", s) != EOF, s[0] != ';' )
			{
				pfcmp->dblcoeff[i] = atof(s) ;
				i++ ;
			}
			
			pfcmp++ ;
		}
	}

	*Npfcmp = (int)(pfcmp - Cmp) ;
}
