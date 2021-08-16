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

/*  solrcol.c  */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnbld.h"
#include "fnfio.h"
#include "fnlib.h"

/*　太陽光発電システム

機器仕様入力　　　　*/

int PVcadata (char *s, PVCA *PVca)
{
	char  *st;
	double dt;
	int   id=0;

	if ((st=strchr(s,'=')) == 0)
	{
		PVca->name = stralloc(s);
		PVca->PVcap = PVca->Area = -999.0 ;
		PVca->KHD = 1.0 ;
		PVca->KPD = 0.95 ;
		PVca->KPM = 0.94 ;
		PVca->KPA = 0.97 ;
		PVca->effINO = 0.9 ;
		PVca->A = PVca->B = -999. ;
		PVca->apmax = -0.41 ;
	}
	else
	{ 
		*st = '\0';
		
        dt=atof(st+1);
        
		if (strncmp(s,"KHD",3) == 0)			// 日射量年変動補正係数
			PVca->KHD = dt ;
		else if (strncmp(s,"KPD",3) == 0)			// 経時変化補正係数
			PVca->KPD = dt ;
		else if (strncmp(s,"KPM",3) == 0)			// アレイ負荷整合補正係数
			PVca->KPM = dt ;
		else if (strncmp(s,"KPA",3) == 0)			// アレイ回路補正係数
			PVca->KPA = dt ;
		else if (strncmp(s,"EffInv",6) == 0)		// インバータ実行効率
			PVca->effINO = dt ;
		else if (strncmp(s,"apmax",5) == 0)			// 最大出力温度係数
			PVca->apmax = dt  ;
		else if (strcmp(s, "InstallType") == 0)
		{
			PVca->InstallType = *(st+1) ;

			switch(PVca->InstallType)
			{
			case 'A':
				PVca->A = 46., PVca->B = 0.41 ;
				break ;
			case 'B':
				PVca->A = 50.0, PVca->B = 0.38 ;
				break ;
			case 'C':
				PVca->A = 57.0, PVca->B = 0.33 ;
				break ;
			}
		}
		else if (strncmp(s,"PVcap",5) == 0)			// 太陽電池容量
			PVca->PVcap = dt  ;
		else if (strncmp(s,"Area",4) == 0)			// 太陽電池容量
			PVca->Area = dt  ;
        else 
			id=1;     
	}
	return (id);
}  
/* ------------------------------------- */

/*  初期設定 */

void PVint(int Npv, PV *PV, int Nexsf, EXSF *Exs, WDAT *Wd)
{
	int		i, j;
	EXSF	*exs;
	char	Err[SCHAR] ;
	
	for (i = 0; i < Npv; i++, PV++)
	{
		PV->Ta = &Wd->T;
		PV->V = &Wd->Wv ;
		exs = Exs;
		for (j = 0; j < Nexsf; j++, exs++)
		{
			if(strcmp(PV->cmp->exsname, exs->name) == 0)
			{
				PV->sol = exs;
				PV->I = &PV->sol->Iw ;
			}
		}

		if (PV->sol == NULL)
			Eprint("PVint", PV->cmp->exsname);

		if (PV->sol == NULL)
			Eprint("PVint", PV->cmp->exsname);

		if ( PV->cat->KHD < 0.0 )
		{
			sprintf ( Err, "Name=%s KHD=%.4g", PV->cmp->name, PV->cat->KHD ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->cat->KPD < 0.0 )
		{
			sprintf ( Err, "Name=%s KHD=%.4g", PV->cmp->name, PV->cat->KPD ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->cat->KPM < 0.0 )
		{
			sprintf ( Err, "Name=%s KPM=%.4g", PV->cmp->name, PV->cat->KPM ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->cat->KPA < 0.0 )
		{
			sprintf ( Err, "Name=%s KPA=%.4g", PV->cmp->name, PV->cat->KPA ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->cat->effINO < 0.0 )
		{
			sprintf ( Err, "Name=%s EffInv=%.4g", PV->cmp->name, PV->cat->effINO ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->cat->apmax > 0.0 )
		{
			sprintf ( Err, "Name=%s apmax=%.4g", PV->cmp->name, PV->cat->apmax ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->PVcap < 0.0 )
		{
			sprintf ( Err, "Name=%s PVcap=%.4g", PV->cmp->name, PV->PVcap ) ;
			Eprint("PVint", Err ) ;
		}

		if ( PV->Area < 0.0 )
		{
			sprintf ( Err, "Name=%s Area=%.4g", PV->cmp->name, PV->Area ) ;
			Eprint("PVint", Err ) ;
		}

		// 計算途中で変化しない各種補正係数の積
		PV->KConst = PV->cat->KHD * PV->cat->KPD * PV->cat->KPM
			* PV->cat->KPA * PV->cat->effINO ;
	}
}

// 太陽光発電の発電量計算
/* ------------------------------------- */

/*  集熱量の計算 */

void PVene(int Npv, PV *PV)
{
	int i;
	
	for (i = 0; i < Npv; i++, PV++)
	{
		// 太陽電池アレイの計算（JIS C 8907:2005　P21による）
		PV->TPV = *PV->Ta + ( PV->cat->A / ( PV->cat->B * pow(*PV->V, 0.8) + 1.) + 2.) * *PV->I / 1000. - 2 ;
		PV->KPT = FNKPT(PV->TPV, PV->cat->apmax) ;
		PV->KTotal = PV->KConst * PV->KPT ;

		// 太陽電池入社日射量の計算
		PV->Iarea = *PV->I * PV->Area ;

		// 発電量の計算
		PV->Power = PV->KTotal * *PV->I / 1000. * PV->PVcap ;

		// 発電効率の計算
		PV->Eff = 0.0 ;
		if ( PV->Iarea > 0. )
			PV->Eff = PV->Power / PV->Iarea ;
	}
}

/* ------------------------------------------------------------- */

void PVprint(FILE *fo, int id, int Npv, PV *PV)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npv > 0)
            fprintf(fo, "%s %d\n", PV_TYPE, Npv);
		for (i = 0; i < Npv; i++, PV++)
			fprintf(fo, " %s 1 4\n", PV->name);
		break;
		
	case 1:
		for (i = 0; i < Npv; i++, PV++)
		{	    
			fprintf(fo, " %s_TPV t f %s_I e f %s_P e f %s_Eff r f \n",
				PV->name, PV->name, PV->name, PV->name);
		}
		break;
		
	default:
		for (i = 0; i < Npv; i++, PV++)
			fprintf(fo, " %4.1lf %4.0lf %3.0lf %.3lf\n",
	           PV->TPV, PV->Iarea, PV->Power, PV->Eff);
		break;
	}
}

/* --------------------------- */

/* 日積算値に関する処理 */

void PVdyint(int Npv, PV *PV)
{
	int i;
	
	for (i = 0; i < Npv; i++, PV++)
	{
		qdyint(&PV->Edy);
		edyint(&PV->Soldy);
	}
}

void PVmonint(int Npv, PV *PV)
{
	int i;
	
	for (i = 0; i < Npv; i++, PV++)
	{
		qdyint(&PV->mEdy);
		edyint(&PV->mSoldy);
	}
}

void PVday(int Mon, int Day, int ttmm, int Npv, PV *PV, int Nday, int SimDayend)
{
	int  i;
	int Mo, tt ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	for (i = 0; i < Npv; i++, PV++)
	{
		char	sw ;
		sw = OFF_SW ;
		if (PV->Power > 0.)
			sw = ON_SW ;

		// 日集計
		qdaysum(ttmm, sw, PV->Power, &PV->Edy);
		
		if (*PV->I > 0.0)
			sw = ON_SW;
		else 
			sw =OFF_SW;
		edaysum(ttmm, sw, *PV->I, &PV->Soldy);

		// 月集計
		sw = OFF_SW ;
		if (PV->Power > 0.)
			sw = ON_SW ;
		qmonsum(Mon, Day, ttmm, sw, PV->Power, &PV->mEdy, Nday, SimDayend);
		
		if (*PV->I > 0.0)
			sw = ON_SW;
		else 
			sw =OFF_SW;
		emonsum(Mon, Day, ttmm, sw, *PV->I, &PV->mSoldy, Nday, SimDayend);

		// 月・時刻のクロス集計
		emtsum(Mon, Day, ttmm, sw, PV->Power, &PV->mtEdy[Mo][tt]);
	}
}

void PVdyprt(FILE *fo, int id, int Npv, PV *PV)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npv > 0)
            fprintf(fo, "%s %d\n", PV_TYPE, Npv);
		for (i = 0; i < Npv; i++, PV++)
			fprintf(fo," %s 1 8\n", PV->name);
		break;
		
	case 1:
		for (i = 0; i < Npv; i++, PV++)
		{
			fprintf(fo, "%s_Hh H d %s_E E f \n",
				PV->name, PV->name);
			fprintf(fo, "%s_th h d %s_Em e f \n",
				PV->name, PV->name);
			fprintf(fo, "%s_He H d %s_S E f %s_te h d %s_Sm e f\n\n",
				PV->name, PV->name, PV->name, PV->name);
		}
		break;
		
	default:
		for (i = 0; i < Npv; i++, PV++)
		{
			fprintf(fo, "%1ld %3.1lf ", PV->Edy.hhr, PV->Edy.h);    
            fprintf(fo, "%1ld %2.0lf\n", PV->Edy.hmxtime, PV->Edy.hmx);
			
            fprintf(fo, "%1ld %3.1lf ", PV->Soldy.hrs, PV->Soldy.d);
            fprintf(fo, "%1ld %2.0lf\n", PV->Soldy.mxtime, PV->Soldy.mx);
		}
		break;
	}
}

void PVmonprt(FILE *fo, int id, int Npv, PV *PV)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Npv > 0)
            fprintf(fo, "%s %d\n", PV_TYPE, Npv);
		for (i = 0; i < Npv; i++, PV++)
			fprintf(fo," %s 1 8\n", PV->name);
		break;
		
	case 1:
		for (i = 0; i < Npv; i++, PV++)
		{
			fprintf(fo, "%s_Hh H d %s_E E f \n",
				PV->name, PV->name);
			fprintf(fo, "%s_th h d %s_Em e f \n",
				PV->name, PV->name);
			fprintf(fo, "%s_He H d %s_S E f %s_te h d %s_Sm e f\n\n",
				PV->name, PV->name, PV->name, PV->name);
		}
		break;
		
	default:
		for (i = 0; i < Npv; i++, PV++)
		{
			fprintf(fo, "%1ld %3.1lf ", PV->mEdy.hhr, PV->mEdy.h);    
            fprintf(fo, "%1ld %2.0lf\n", PV->mEdy.hmxtime, PV->mEdy.hmx);
			
            fprintf(fo, "%1ld %3.1lf ", PV->mSoldy.hrs, PV->mSoldy.d);
            fprintf(fo, "%1ld %2.0lf\n", PV->mSoldy.mxtime, PV->mSoldy.mx);
		}
		break;
	}
}

void PVmtprt(FILE *fo, int id, int Npv, PV *PV, int Mo, int tt)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Npv > 0)
            fprintf(fo, "%s %d\n", PV_TYPE, Npv);
		for (i = 0; i < Npv; i++, PV++)
			fprintf(fo, " %s 1 1\n", PV->name);
		break;
		
	case 1:
		for (i = 0; i < Npv; i++, PV++)
		{	 
			fprintf(fo,"%s_E E f \n", PV->name);
		}
		break;
		
	default:
		for (i = 0; i < Npv; i++, PV++)
		{
			fprintf(fo, " %.2lf \n", PV->mtEdy[Mo-1][tt-1].d * cff_kWh);
		}
		break;
	}
}
