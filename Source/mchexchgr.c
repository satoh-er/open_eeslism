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

/* hexchgr.c */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>

#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"

/*  熱交換器  */

/*  仕様入力  */

int Hexdata(char *s, HEXCA *Hexca)
{
	char  *st;
	double dt;
	int   id=0;
	
	if ((st=strchr(s,'=')) == 0)
	{
		Hexca->name = stralloc(s);
		Hexca->eff = -999.0 ;
		Hexca->KA = -999.0 ;
	}
	else
	{ 
		*st = '\0';
		
        dt=atof(st+1);
		
        if (strcmp(s,"eff") == 0)
			Hexca->eff = dt;
		else if (strcmp(s,"KA") == 0)
			Hexca->KA = dt;
        else 
			id=1;      
	}
	return (id);
} 

/* --------------------------- */

/*  特性式の係数  */

void Hexcfv(int Nhex, HEX *Hex)
{
	ELOUT	*Eoc, *Eoh;
	double	CGc, CGh, eCGmin, *coeffin;
	int		i;
	char	Err[SCHAR] ;
	
	for (i = 0; i < Nhex; i++, Hex++)
	{
		// 計算準備
		if ( Hex->id == 0 )
		{
			/* 温度効率固定タイプと変動タイプの判定 */
			if (Hex->cat->eff > 0.0)
				Hex->etype = 'e' ;
			else if (Hex->cat->KA > 0.0)
				Hex->etype = 'k' ;
			else
			{
				printf ("Hex %s  Undefined Character eff or KA\n",
					Hex->name ) ;
				Hex->etype = '-' ;
			}

			Hex->id = 1 ;
		}

		if (Hex->cmp->control != OFF_SW)	
		{	 
			Hex->eff = Hex->cat->eff;

			if ( Hex->eff < 0.0 )
			{
				sprintf ( Err, "Name=%s  eff=%.4g", Hex->cmp->name, Hex->eff ) ;
				Eprint ( "Hexcfv", Err ) ;
			}

			Eoh = Eoc = Hex->cmp->elouts;
			Eoh++;
			Hex->CGc = CGc = spcheat(Eoc->fluid) * Eoc->G;      
			Hex->CGh = CGh = spcheat(Eoh->fluid) * Eoh->G;
			
			if ( Hex->etype == 'k' )
				Hex->eff = FNhccet ( Hex->CGc, Hex->CGh, Hex->cat->KA ) ;

			eCGmin = Hex->eff * dmin(CGc, CGh);
			Hex->eCGmin = eCGmin;
			coeffin = Eoc->coeffin;
			*coeffin = -CGc + eCGmin;
			coeffin++ ;
			*coeffin = -eCGmin;
			Eoc->coeffo = CGc;
			Eoc->Co = 0.0;
			
			coeffin = Eoh->coeffin;	 
			*coeffin = -eCGmin;
			coeffin++ ;
			*coeffin = -CGh + eCGmin;
			Eoh->coeffo = CGh;
			Eoh->Co = 0.0;
		}	
	}
}


/* --------------------------- */

/* 交換熱量の計算 */

void Hexene(int Nhex, HEX *Hex)
{
	ELIN  *Ei;
	ELOUT *Eo;
	int  i;
	
	for (i = 0; i < Nhex; i++, Hex++)
	{      
		Ei = Hex->cmp->elins;
		Hex->Tcin = Ei->sysvin;
		Ei++;
		Hex->Thin = Ei->sysvin;
		
		if (Hex->cmp->control != OFF_SW)
		{
			Eo = Hex->cmp->elouts;
			Hex->Qci = Hex->CGc * (Eo->sysv - Hex->Tcin);
			Eo++;
			Hex->Qhi = Hex->CGh * (Eo->sysv - Hex->Thin);
		}
		
		/*****	 Hex->Q = Hex->eCGmin * (Hex->Thin - Hex->Tcin); ***/
		
		else
			Hex->Qci = Hex->Qhi = 0.0;
			/*************      
			printf("Hexene  Tcin=%lf  Thin=%lf  Qci=%lf  Qhi=%lf\n", Hex->Tcin, Hex->Thin, 
			Hex->Qci, Hex->Qhi);
		*************/
	}
}

/* --------------------------- */

void hexprint(FILE *fo, int id, int Nhex, HEX *Hex)
{
	ELOUT *Eo;
	int i;
	
	
	switch (id)
	{
	case 0:
		if (Nhex > 0)
			fprintf(fo, "%s %d\n", HEXCHANGR_TYPE, Nhex);
		for (i = 0; i < Nhex; i++, Hex++)
			fprintf(fo, " %s 1 9\n", Hex->name);
		break;
		
	case 1:
		for (i = 0; i < Nhex; i++, Hex++)
		{
			fprintf(fo, "%s_c c c %s:c_G m f %s:c_Ti t f %s:c_To t f %s:c_Q q f\n",
				Hex->name, Hex->name, Hex->name, Hex->name, Hex->name);
			fprintf(fo, "%s:h_G m f %s:h_Ti t f %s:h_To t f %s:h_Q q f\n",
				Hex->name, Hex->name, Hex->name, Hex->name);
		}
		break;
		
	default:
		for (i = 0; i < Nhex; i++, Hex++)
		{
			Eo = Hex->cmp->elouts;
            fprintf(fo, "%c %6.4g %4.1lf %4.1lf %2.0lf", 
				Hex->cmp->control, Eo->G, Hex->Tcin, Eo->sysv, Hex->Qci);
            Eo++;
            fprintf(fo, " %6.4g %4.1lf %4.1lf %2.0lf\n", Eo->G, Hex->Thin, Eo->sysv, Hex->Qhi);	 
		}
		break;
	}
}


/* 日積算値に関する処理 */

void hexdyint(int Nhex, HEX *Hex)
{
	int i;
	
	for (i = 0; i < Nhex; i++, Hex++)
	{
		svdyint(&Hex->Tcidy);
		svdyint(&Hex->Thidy);
		qdyint(&Hex->Qcidy);
		qdyint(&Hex->Qhidy);
	}
}

void hexmonint(int Nhex, HEX *Hex)
{
	int i;
	
	for (i = 0; i < Nhex; i++, Hex++)
	{
		svdyint(&Hex->mTcidy);
		svdyint(&Hex->mThidy);
		qdyint(&Hex->mQcidy);
		qdyint(&Hex->mQhidy);
	}
}

void hexday(int Mon, int Day, int ttmm, int Nhex, HEX *Hex, int Nday, int SimDayend)
{
	int i;
	
	for (i = 0; i < Nhex; i++, Hex++)
	{
		// 日集計
		svdaysum(ttmm, Hex->cmp->control, Hex->Tcin, &Hex->Tcidy);
		svdaysum(ttmm, Hex->cmp->control, Hex->Thin, &Hex->Thidy);
		qdaysum(ttmm, Hex->cmp->control, Hex->Qci, &Hex->Qcidy);
		qdaysum(ttmm, Hex->cmp->control, Hex->Qhi, &Hex->Qhidy);
		
		// 月集計
		svmonsum(Mon, Day, ttmm, Hex->cmp->control, Hex->Tcin, &Hex->mTcidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Hex->cmp->control, Hex->Thin, &Hex->mThidy, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Hex->cmp->control, Hex->Qci, &Hex->mQcidy, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Hex->cmp->control, Hex->Qhi, &Hex->mQhidy, Nday, SimDayend);
	}
}

void hexdyprt(FILE *fo, int id, int Nhex, HEX *Hex)
{
	int i, j;
	char c;
	
	switch (id)
	{
	case 0:
		if (Nhex > 0)
			fprintf(fo, "%s %d\n", HEXCHANGR_TYPE, Nhex);
		for (i = 0; i < Nhex; i++, Hex++)
			fprintf(fo, " %s 1 28\n", Hex->name);
		break;
		
	case 1:
		for (i = 0; i < Nhex; i++, Hex++)
		{
			for (j = 0; j < 2; j++)
			{
				if (j == 0) 
					c = 'c';
				else
					c = 'h';
				
				fprintf(fo, "%s:%c_Ht H d %s:%c_T T f ",  Hex->name, c, Hex->name, c);
				fprintf(fo,"%s:%c_ttn h d %s:%c_Tn t f %s:%c_ttm h d %s:%c_Tm t f\n",
					Hex->name, c, Hex->name, c, Hex->name, c, Hex->name, c);
				fprintf(fo, "%s:%c_Hh H d %s:%c_Qh Q f %s:%c_Hc H d %s:%c_Qc Q f\n",
					Hex->name, c, Hex->name, c, Hex->name, c, Hex->name, c);
				fprintf(fo, "%s:%c_th h d %s:%c_qh q f %s:%c_tc h d %s:%c_qc q f\n",
					Hex->name, c, Hex->name, c, Hex->name, c, Hex->name, c);
			}
			
		}
		break;
		
	default:
		for (i = 0; i < Nhex; i++, Hex++)
		{
            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				Hex->Tcidy.hrs, Hex->Tcidy.m, 
				Hex->Tcidy.mntime, Hex->Tcidy.mn, 
				Hex->Tcidy.mxtime, Hex->Tcidy.mx);
            fprintf(fo, "%1d %3.1lf ", Hex->Qcidy.hhr, Hex->Qcidy.h);
            fprintf(fo, "%1d %3.1lf ", Hex->Qcidy.chr, Hex->Qcidy.c);      
            fprintf(fo, "%1d %2.0lf ", Hex->Qcidy.hmxtime, Hex->Qcidy.hmx);
            fprintf(fo, "%1d %2.0lf ", Hex->Qcidy.cmxtime, Hex->Qcidy.cmx);
            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				Hex->Thidy.hrs, Hex->Thidy.m, 
				Hex->Thidy.mntime, Hex->Thidy.mn, 
				Hex->Thidy.mxtime, Hex->Thidy.mx);
            fprintf(fo, "%1d %3.1lf ", Hex->Qhidy.hhr, Hex->Qhidy.h);
            fprintf(fo, "%1d %3.1lf ", Hex->Qhidy.chr, Hex->Qhidy.c);      
            fprintf(fo, "%1d %2.0lf ", Hex->Qhidy.hmxtime, Hex->Qhidy.hmx);
            fprintf(fo, "%1d %2.0lf\n", Hex->Qhidy.cmxtime, Hex->Qhidy.cmx); 
		}
		break;
	}
}

void hexmonprt(FILE *fo, int id, int Nhex, HEX *Hex)
{
	int i, j;
	char c;
	
	switch (id)
	{
	case 0:
		if (Nhex > 0)
			fprintf(fo, "%s %d\n", HEXCHANGR_TYPE, Nhex);
		for (i = 0; i < Nhex; i++, Hex++)
			fprintf(fo, " %s 1 28\n", Hex->name);
		break;
		
	case 1:
		for (i = 0; i < Nhex; i++, Hex++)
		{
			for (j = 0; j < 2; j++)
			{
				if (j == 0) 
					c = 'c';
				else
					c = 'h';
				
				fprintf(fo, "%s:%c_Ht H d %s:%c_T T f ",  Hex->name, c, Hex->name, c);
				fprintf(fo,"%s:%c_ttn h d %s:%c_Tn t f %s:%c_ttm h d %s:%c_Tm t f\n",
					Hex->name, c, Hex->name, c, Hex->name, c, Hex->name, c);
				fprintf(fo, "%s:%c_Hh H d %s:%c_Qh Q f %s:%c_Hc H d %s:%c_Qc Q f\n",
					Hex->name, c, Hex->name, c, Hex->name, c, Hex->name, c);
				fprintf(fo, "%s:%c_th h d %s:%c_qh q f %s:%c_tc h d %s:%c_qc q f\n",
					Hex->name, c, Hex->name, c, Hex->name, c, Hex->name, c);
			}
			
		}
		break;
		
	default:
		for (i = 0; i < Nhex; i++, Hex++)
		{
            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				Hex->mTcidy.hrs, Hex->mTcidy.m, 
				Hex->mTcidy.mntime, Hex->mTcidy.mn, 
				Hex->mTcidy.mxtime, Hex->mTcidy.mx);
            fprintf(fo, "%1d %3.1lf ", Hex->mQcidy.hhr, Hex->mQcidy.h);
            fprintf(fo, "%1d %3.1lf ", Hex->mQcidy.chr, Hex->mQcidy.c);      
            fprintf(fo, "%1d %2.0lf ", Hex->mQcidy.hmxtime, Hex->mQcidy.hmx);
            fprintf(fo, "%1d %2.0lf ", Hex->mQcidy.cmxtime, Hex->mQcidy.cmx);
            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				Hex->mThidy.hrs, Hex->mThidy.m, 
				Hex->mThidy.mntime, Hex->mThidy.mn, 
				Hex->mThidy.mxtime, Hex->mThidy.mx);
            fprintf(fo, "%1d %3.1lf ", Hex->mQhidy.hhr, Hex->mQhidy.h);
            fprintf(fo, "%1d %3.1lf ", Hex->mQhidy.chr, Hex->mQhidy.c);      
            fprintf(fo, "%1d %2.0lf ", Hex->mQhidy.hmxtime, Hex->mQhidy.hmx);
            fprintf(fo, "%1d %2.0lf\n", Hex->mQhidy.cmxtime, Hex->mQhidy.cmx); 
		}
		break;
	}
}
