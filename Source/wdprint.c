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

/* wdprint.c   */

#include <stdio.h>
#include "common.h"
#include "estr.h"
#include "fesy.h"
#include "fnfio.h"
#include "fnlib.h"

/* -----------------------------------------------------*/

/* 気象データ日集計 */

void Wdtsum(int Mon, int Day, int Nday, int ttmm, WDAT *Wd, int Nexs, EXSF *Exs,
			WDAT *Wdd, WDAT *Wdm, double *Soldy, double *Solmon, SIMCONTL *Simc)
{
	static int oldday=0, hrs, oldMon = 0, hrsm;
	static double  cffWh;
	extern double  cff_kWh;
	int		i;
	double	*s ;
	EXSF	*e ;
	//int isEndDay(int Mon, int Day) ;
	
	// 日集計の初期化
	if (Nday != oldday)
	{
		cffWh = cff_kWh * 1000.0;
		hrs = 0;
		Wdd->T = Wdd->x = Wdd->Wv = Wdd->Idn = Wdd->Isky = Wdd->RN = 0.0;

		s = Soldy ;
		for (i = 0; i < Nexs; i++, s++)
			*s =0.0;
		oldday = Nday;
		/*****
		printf("<<Wdtsum  int>> ttmm=%d hrs=%d\n cfkWh=%lf  cfWh=%lf\n", 
		ttmm, hrs, cff_kWh, cffWh);
		*****/
	}

	// 月集計の初期化
	if (Mon != oldMon)
	{
		cffWh = cff_kWh * 1000.0;
		hrsm = 0;
		Wdm->T = Wdm->x = Wdm->Wv = Wdm->Idn = Wdm->Isky = Wdm->RN = 0.0;

		s = Solmon ;
		for (i = 0; i < Nexs; i++, s++)
			*s =0.0;
		oldMon = Mon;
		/*****
		printf("<<Wdtsum  int>> ttmm=%d hrs=%d\n cfkWh=%lf  cfWh=%lf\n", 
		ttmm, hrs, cff_kWh, cffWh);
		*****/
	}
	
	// 日集計
	hrs++;        
	Wdd->T += Wd->T;
	Wdd->x += Wd->x;
	Wdd->Wv += Wd->Wv; 
	Wdd->Idn += Wd->Idn; 
	Wdd->Isky += Wd->Isky;
	Wdd->RN += Wd->RN;

	s = Soldy ;
	e = Exs ;
	for ( i = 0; i < Nexs; i++, s++, e++ )
	{
		if (e->typ != 'E' && e->typ != 'e')
            *s += e->Iw;
		else
			*s += e->Tearth;
	}
	/*****
	printf("<<Wdtsum>> ttmm=%d hrs=%d\n", ttmm, hrs);*****/
	
	// 月集計
	hrsm++;        
	Wdm->T += Wd->T;
	Wdm->x += Wd->x;
	Wdm->Wv += Wd->Wv; 
	Wdm->Idn += Wd->Idn; 
	Wdm->Isky += Wd->Isky;
	Wdm->RN += Wd->RN;

	s = Solmon ;
	e = Exs ;
	for ( i = 0; i < Nexs; i++, s++, e++ )
	{
		if (e->typ != 'E' && e->typ != 'e')
            *s += e->Iw;
		else
			*s += e->Tearth;
	}
	/*****
	printf("<<Wdtsum>> ttmm=%d hrs=%d\n", ttmm, hrs);*****/
	
	s = Soldy ;
	e = Exs ;

	if (ttmm == 2400)
	{
		Wdd->T /= hrs;
		Wdd->x /= hrs;
		Wdd->Wv /= hrs; 
		Wdd->Idn *= cffWh; 
		Wdd->Isky *= cffWh;
		Wdd->RN *= cffWh;
		for ( i = 0; i < Nexs; i++, s++, e++ )
		{ 
			if (e->typ != 'E' && e->typ != 'e')
				*s *= cffWh;
			else
				*s /= hrs; 
		}
	}
	
	s = Solmon ;
	e = Exs ;

	if (isEndDay(Mon, Day, Nday, Simc->dayend) == 1 && hrsm > 0 && ttmm == 2400)
	{
		Wdm->T /= hrsm;
		Wdm->x /= hrsm;
		Wdm->Wv /= hrsm; 
		Wdm->Idn *= cffWh; 
		Wdm->Isky *= cffWh;
		Wdm->RN *= cffWh;
		for ( i = 0; i < Nexs; i++, s++, e++ )
		{ 
			if (e->typ != 'E' && e->typ != 'e')
				*s *= cffWh;
			else
				*s /= hrsm; 
		}
	}
}
/* ----------------------------------------------------------------- */

/* 気象データ出力 */

void Wdtprint(FILE *fo, char *title, int Mon, int Day, double time,
			  WDAT *Wd, EXSFS *Exsfst)
{
	static int ic=0;
	int		i;
	EXSF	*e ;
	int		Nexs ;

	Nexs = Exsfst->Nexs ;
	
	if ( DEBUG )
	{
		printf ( "N=%d\t%d/%d\t%.2lf\n", Nexs, Mon, Day, time);
		printf( "%s;\n %d\n", title, Nexs);
	}

	if (ic == 0)
	{
		ic++ ;
		fprintf(fo, "%s;\n %d\n", title, Nexs);
		fprintf(fo, "Mon\tDay\tTime\tWd_T\tWd_x\tWd_RH\tWd_Wv\t") ;
		fprintf(fo, "Wd_Wdre\tWd_RN\tWd_Idn\tWd_Isky\tsolh\tsolA\t") ;

		e = Exsfst->Exs ;
		for ( i = 0; i < Nexs; i++, e++ )
		{
			if ( DEBUG )
				printf( "%s[%c]\t", e->name, e->typ);

			fprintf(fo, "%s[%c]\t", e->name, e->typ);
		}
		fprintf(fo, "\n");
	}
	
	fprintf(fo, "%d\t%d\t%.2lf\t", Mon, Day, time);
	fprintf(fo, "%.2lf\t%.4lf\t%.0lf\t%.1lf\t%.0lf\t%.0lf\t%.0lf\t%.0lf\t%.1lf\t%.1lf\t",
		Wd->T, Wd->x, Wd->RH, Wd->Wv, Wd->Wdre,
		Wd->RN, Wd->Idn, Wd->Isky, Wd->solh, Wd->solA);

	e = Exsfst->Exs ;
	for ( i = 0; i < Nexs; i++, e++ )
	{
		if ( e->typ != 'E' && e->typ != 'e')
            fprintf(fo, "%.0lf\t", e->Iw);
		else
			fprintf(fo, "%.1lf\t", e->Tearth);
	}
	fprintf(fo, "\n");
}
/* ----------------------------------------------------------------- */

/* 気象データ日集計値出力 */

void Wdtdprint(FILE *fo, char *title, int Mon, int Day, 
			   WDAT *Wdd, int Nexs, EXSF *Exs, double *Soldy) 
{
	static int ic=0;
	int		i;
	EXSF	*e ;
	double	*s ;
	
	if (ic == 0)
	{
		ic++ ;
		fprintf(fo, "%s;\n %d\n", title, Nexs);

		fprintf(fo, "Mo\tNd\tWd_T\tWd_x\tWd_Wv\tWd_RN\tWd_Idn\tWd_Isky\t" );
		e = Exs ;
		for ( i = 0; i < Nexs; i++, e++ )
			fprintf(fo, "%s[%c]\t", e->name, e->typ);
		fprintf(fo, "\n");
	}
	
	fprintf(fo, "%d\t%d\t", Mon, Day);
	fprintf(fo, "%.1lf\t%.4lf\t%.1lf\t%.2lf\t%.2lf\t%4.2lf",
		Wdd->T, Wdd->x, Wdd->Wv, Wdd->RN / 1000., Wdd->Idn / 1000., Wdd->Isky / 1000. );

	e = Exs ;
	s = Soldy ;
	for ( i = 0; i < Nexs; i++, e++, s++ )
	{
		if ( e->typ != 'E' && e->typ != 'e')
            fprintf(fo, "\t%.2lf", *s / 1000. );
		else
			fprintf(fo, "\t%.1lf", *s );
	}
	fprintf(fo, "\n");
}     

void Wdtmprint(FILE *fo, char *title, int Mon, int Day, 
			   WDAT *Wdm, int Nexs, EXSF *Exs, double *Solmon) 
{
	static int ic=0;
	int		i;
	EXSF	*e ;
	double	*s ;
	
	if (ic == 0)
	{
		ic++ ;
		fprintf(fo, "%s;\n %d\n", title, Nexs);

		fprintf(fo, "Mo\tNd\tWd_T\tWd_x\tWd_Wv\tWd_RN\tWd_Idn\tWd_Isky\t" );
		e = Exs ;
		for ( i = 0; i < Nexs; i++, e++ )
			fprintf(fo, "%s[%c]\t", e->name, e->typ);
		fprintf(fo, "\n");
	}
	
	fprintf(fo, "%d\t%d\t", Mon, Day);
	fprintf(fo, "%.1lf\t%.4lf\t%.1lf\t%.2lf\t%.2lf\t%4.2lf",
		Wdm->T, Wdm->x, Wdm->Wv, Wdm->RN / 1000., Wdm->Idn / 1000., Wdm->Isky / 1000. );

	e = Exs ;
	s = Solmon ;
	for ( i = 0; i < Nexs; i++, e++, s++ )
	{
		if ( e->typ != 'E' && e->typ != 'e')
            fprintf(fo, "\t%.2lf", *s / 1000. );
		else
			fprintf(fo, "\t%.1lf", *s );
	}
	fprintf(fo, "\n");
}    


