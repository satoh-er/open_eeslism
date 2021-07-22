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

/*  mcstheat.c  */
/*  電気蓄熱式暖房器 */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"


/* ------------------------------------------ */

/* 機器仕様入力　　　　　　*/

/*---- Satoh Debug 電気蓄熱式暖房器 2001/1/21 ----*/
int Stheatdata(char *s, STHEATCA *stheatca)
{
	char  *st;
	double dt;
	int   id=0;
	
	if ((st=strchr(s,'=')) == 0)
	{
		stheatca->name = stralloc(s);
		stheatca->eff = -999.0 ;
		stheatca->Q = -999.0 ;
		stheatca->Hcap = -999.0 ;
		stheatca->KA = -999.0 ;
	}
	else
	{ 
		*st = '\0';
		
		if (strcmp(s, "PCM") == 0)
			stheatca->pcmname = stralloc(st + 1);
		else
		{
			dt = atof(st + 1);

			if (strcmp(s, "Q") == 0)
				stheatca->Q = dt;
			else if (strcmp(s, "KA") == 0)
				stheatca->KA = dt;
			else if (strcmp(s, "eff") == 0)
				stheatca->eff = dt;
			else if (strcmp(s, "Hcap") == 0)
				stheatca->Hcap = dt;
			else
				id = 1;
		}
	}
	return (id);
}
/* --------------------------- */

/*  管長・ダクト長、周囲温度設定 */

void Stheatint(int Nstheat, STHEAT *stheat, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd,int Npcm, PCM *PCM)
{
	int		i;
	char	Err[SCHAR] ;
	STHEATCA	*st ;
	
	for (i = 0; i < Nstheat; i++, stheat++)
	{
	/******************
	printf ( "envnasme=%s\n", Pipe->cmp->envname ) ;
		************************/
		
		if ( stheat->cmp->envname != NULL )
			stheat->Tenv = envptr(stheat->cmp->envname, Simc, Ncompnt, Compnt, Wd, NULL);
		else
		{
			stheat->room = roomptr(stheat->cmp->roomname, Ncompnt, Compnt ) ;
			/**********************
			printf ( "Roomname=%s\n", Pipe->room->name) ;
			**************************/
		}

		if (stheat->cat->pcmname != NULL)
		{
			int j;
			for (j = 0; j < Npcm; j++, PCM++)
			{
				if (strcmp(stheat->cat->pcmname, PCM->name) == 0)
					stheat->pcm = PCM;
			}
			if (stheat->pcm == NULL)
			{
				sprintf(Err, "STHEAT %s のPCM=%sが見つかりません", stheat->name, stheat->cat->pcmname);
				Eprint(Err, "<Stheatint>");
				preexit();
			}
		}
		
		st = stheat->cat ;

		if ( st->Q < 0.0 )
		{
			sprintf ( Err, "Name=%s  Q=%.4g", stheat->name, st->Q ) ;
			Eprint ( "Stheatinit", Err ) ;
		}
		if ( stheat->pcm == NULL && st->Hcap < 0.0 )
		{
			sprintf ( Err, "Name=%s  Hcap=%.4g", stheat->name, st->Hcap ) ;
			Eprint ( "Stheatinit", Err ) ;
		}
		if ( st->KA < 0.0 )
		{
			sprintf ( Err, "Name=%s  KA=%.4g", stheat->name, st->KA ) ;
			Eprint ( "Stheatinit", Err ) ;
		}
		if ( st->eff < 0.0 )
		{
			sprintf ( Err, "Name=%s  eff=%.4g", stheat->name, st->eff ) ;
			Eprint ( "Stheatinit", Err ) ;
		}

		stheat->Tsold = atof ( stheat->cmp->tparm ) ;

		// 内臓PCMの質量
		stheat->mPCM = stheat->cmp->mPCM;
	}
}
/* --------------------------- */

/*  特性式の係数  */

void Stheatcfv(int Nstheat, STHEAT *stheat)
{
	ELOUT *Eo;
	extern double dTM ;
	double  Te, eff, cG, KA, Tsold, d ;
	int i;
	PCM		*pcm;
	double	FNPCMState(double Ss, double Sl, double Ql, double Ts, double Tl, double T);
	
	for (i = 0; i < Nstheat; i++, stheat++)
	{
		if ( stheat->cmp->envname != NULL )
			Te = *(stheat->Tenv) ;
		else
			Te = stheat->room->Tot ;
		
		/*******
		printf("<<Stheatcfv>> i=%d %s  Tenv=%lf\n",
			i, stheat->name, Te);
		/************/
		
		Eo = stheat->cmp->elouts;
		eff = stheat->cat->eff ;
		cG = stheat->cG = spcheat(Eo->fluid) * Eo->G ;
		KA = stheat->cat->KA ;
		Tsold = stheat->Tsold ;
		pcm = stheat->pcm;
		if (pcm != NULL)
		{
			stheat->Hcap = stheat->mPCM * FNPCMState(pcm->Cros, pcm->Crol, pcm->Ql, pcm->Ts, pcm->Tl, Tsold);
		}
		else
			stheat->Hcap = stheat->cat->Hcap;

		d = stheat->Hcap / dTM + eff * cG + KA;
		//printf("Eo->control=%c cmp->control=%c\n", Eo->control, stheat->cmp->control);
		if ( stheat->cmp->control != OFF_SW )
		//if (Eo->control != OFF_SW)
			stheat->E = stheat->cat->Q ;
		else
			stheat->E = 0.0 ;
		
		// 空気が流れていれば出入口温度の関係式係数を作成する
		//if (stheat->cmp->control != OFF_SW)
		if (Eo->control != OFF_SW)
		{
			Eo->coeffo = 1.0;
			Eo->Co = eff * (stheat->Hcap / dTM * Tsold + KA * Te
				+ stheat->E) / d;
			*(Eo->coeffin) = eff - 1.0 - eff * eff * cG / d;

			Eo++;
			Eo->coeffo = 1.0;
			Eo->Co = 0.0;
			*(Eo->coeffin) = -1.0;
		}
		else
		{
			Eo->coeffo = 1.0;
			Eo->Co = 0.0;
			*(Eo->coeffin) = -1.0;

			Eo++;
			Eo->coeffo = 1.0;
			Eo->Co = 0.0;
			*(Eo->coeffin) = -1.0;
		}
	}
}
/* --------------------------- */

/* 取得熱量の計算 */

void Stheatene(int Nstheat, STHEAT *stheat)
{
	int i;
	ELOUT	*elo ;
	extern double	dTM ;
	STHEATCA	*cat ;
	double	Te ;
	
	for (i = 0; i < Nstheat; i++, stheat++)
	{
		elo = stheat->cmp->elouts ;
		stheat->Tin = elo->elins->sysvin;
		
		cat = stheat->cat ;
		
		if ( stheat->cmp->envname != NULL )
			Te = *(stheat->Tenv) ;
		else
			Te = stheat->room->Tot ;
		
		/****************************
		printf ( "envname=%s\n", stheat->room->name ) ;
		/***************************/
		stheat->Tout = elo->sysv ;
		stheat->Ts = (stheat->Hcap / dTM * stheat->Tsold
			+ cat->eff * stheat->cG * stheat->Tin
			+ cat->KA * Te + stheat->E )
			/ (stheat->Hcap / dTM + cat->eff * stheat->cG
			+ cat->KA ) ;
		
		stheat->Q = stheat->cG * ( stheat->Tout - stheat->Tin ) ;
		
		stheat->Qls = stheat->cat->KA * ( Te - stheat->Ts ) ;
		
		stheat->Qsto = stheat->Hcap / dTM
			* ( stheat->Ts - stheat->Tsold ) ;
		
		/****************************
		printf ("<Stheatene> name=%s E=%.0lf Q=%.0lf Ts=%.1lf Tsold=%.1lf\n",
			stheat->name, stheat->E, stheat->Q, stheat->Ts, stheat->Tsold ) ;
			*************************************/

		stheat->Tsold = stheat->Ts ;
		
		if ( stheat->room != NULL )
			stheat->room->Qeqp += ( - stheat->Qls ) ;
	}
}

//void Stheatene2(int Nstheat, STHEAT *stheat)
//{
//	int i;
//	ELOUT	*elo;
//	extern double	dTM;
//	STHEATCA	*cat;
//	double	Te;
//
//	for (i = 0; i < Nstheat; i++, stheat++)
//	{
//		// 前時刻の蓄熱体温度のアップデート
//		stheat->Tsold = stheat->Ts;
//
//		if (stheat->room != NULL)
//			stheat->room->Qeqp += (-stheat->Qls);
//	}
//}

int	stheatvptr(char **key, STHEAT *Stheat, VPTR *vptr, VPTR *vpath)
{
	int err = 0;
	
	if (strcmp(key[1], "Ts") == 0)
	{
		vptr->ptr = &Stheat->Tsold;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "control") == 0)
	{
		vpath->type = 's';
		vpath->ptr = Stheat;
		vptr->ptr = &Stheat->cmp->control;
		vptr->type = SW_CTYPE;
	}
	else
		err = 1;
	
	return err;
}
/* ---------------------------*/

void stheatprint(FILE *fo, int id, int Nstheat, STHEAT *stheat)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nstheat > 0)
            fprintf(fo, "%s %d\n", STHEAT_TYPE, Nstheat);
		for (i = 0; i < Nstheat; i++, stheat++)
			fprintf(fo, " %s 1 11\n", stheat->name);
		break;
		
	case 1:
		for (i = 0; i < Nstheat; i++, stheat++)
		{	 
			fprintf(fo,"%s_c c c %s_G m f %s_Ts t f %s_Ti t f %s_To t f %s_Q q f %s_Qsto q f ",
				stheat->name, stheat->name, stheat->name, stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo,"%s_Qls q f %s_Ec c c %s_E e f ",
				stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_Hcap q f\n", stheat->name);
		}
		break;
		
	default:
		for (i = 0; i < Nstheat; i++, stheat++)
		{
            fprintf(fo, "%c %6.4g %4.1lf %4.1lf %4.1lf %2.0lf %.4g ",
				stheat->cmp->elouts->control, stheat->cmp->elouts->G,
				stheat->Ts,
				stheat->Tin, stheat->Tout, stheat->Q, stheat->Qsto);
			fprintf(fo, "%.4g %c %2.0lf ",
				stheat->Qls, stheat->cmp->control, stheat->E);
			fprintf(fo, "%.0lf\n", stheat->Hcap);
		}
		break;
	}
}

/* --------------------------- */

/* 日積算値に関する処理 */

/*******************/
void stheatdyint(int Nstheat, STHEAT *stheat)
{
	int i;
	
	for (i = 0; i < Nstheat; i++, stheat++)
	{
		stheat->Qlossdy = 0.0 ;
		stheat->Qstody = 0.0 ;

		svdyint(&stheat->Tidy);
		svdyint(&stheat->Tsdy);
		svdyint(&stheat->Tody);
		qdyint(&stheat->Qdy);
		edyint(&stheat->Edy);
	}
}

void stheatmonint(int Nstheat, STHEAT *stheat)
{
	int i;
	
	for (i = 0; i < Nstheat; i++, stheat++)
	{
		stheat->mQlossdy = 0.0 ;
		stheat->mQstody = 0.0 ;

		svdyint(&stheat->mTidy);
		svdyint(&stheat->mTsdy);
		svdyint(&stheat->mTody);
		qdyint(&stheat->mQdy);
		edyint(&stheat->mEdy);
	}
}

void stheatday(int Mon, int Day, int ttmm, int Nstheat, STHEAT *stheat, int Nday, int SimDayend)
{
	int i;
	int Mo, tt ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	
	for (i = 0; i < Nstheat; i++, stheat++)
	{
		// 日集計
		stheat->Qlossdy += stheat->Qls ;
		stheat->Qstody += stheat->Qsto ;
		svdaysum(ttmm, stheat->cmp->control, stheat->Tin, &stheat->Tidy);
		svdaysum(ttmm, stheat->cmp->control, stheat->Tout, &stheat->Tody);
		svdaysum(ttmm, stheat->cmp->control, stheat->Ts, &stheat->Tsdy);
		qdaysum(ttmm, stheat->cmp->control, stheat->Q, &stheat->Qdy);
		edaysum(ttmm, stheat->cmp->control, stheat->E, &stheat->Edy);	 
		
		// 月集計
		stheat->mQlossdy += stheat->Qls ;
		stheat->mQstody += stheat->Qsto ;
		svmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Tin, &stheat->mTidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Tout, &stheat->mTody, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Ts, &stheat->mTsdy, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->Q, &stheat->mQdy, Nday, SimDayend);
		emonsum(Mon, Day, ttmm, stheat->cmp->control, stheat->E, &stheat->mEdy, Nday, SimDayend);	

		// 月・時刻のクロス集計
		emtsum(Mon, Day, ttmm, stheat->cmp->control, stheat->E, &stheat->mtEdy[Mo][tt]);
	}
}

void stheatdyprt(FILE *fo, int id, int Nstheat, STHEAT *stheat)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nstheat > 0)
            fprintf(fo, "%s %d\n", STHEAT_TYPE, Nstheat);
		for (i = 0; i < Nstheat; i++, stheat++)
			fprintf(fo, " %s 1 32\n", stheat->name);
		break;
		
	case 1:
		for (i = 0; i < Nstheat; i++, stheat++)
		{	 
			fprintf(fo,"%s_Ht H d %s_Ti T f ", stheat->name, stheat->name);
			fprintf(fo,"%s_ttn h d %s_Tin t f %s_ttm h d %s_Tim t f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo,"%s_Ht H d %s_To T f ", stheat->name, stheat->name);
			fprintf(fo,"%s_ttn h d %s_Ton t f %s_ttm h d %s_Tom t f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo,"%s_Ht H d %s_Ts T f ", stheat->name, stheat->name);
			fprintf(fo,"%s_ttn h d %s_Tsn t f %s_ttm h d %s_Tsm t f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_Qls Q f %s_Qst Q f\n\n",
				stheat->name, stheat->name);
		}
		break;
		
	default:
		for (i = 0; i < Nstheat; i++, stheat++)
		{
            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				stheat->Tidy.hrs, stheat->Tidy.m, 
				stheat->Tidy.mntime, stheat->Tidy.mn, 
				stheat->Tidy.mxtime, stheat->Tidy.mx);
			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				stheat->Tody.hrs, stheat->Tody.m, 
				stheat->Tody.mntime, stheat->Tody.mn, 
				stheat->Tody.mxtime, stheat->Tody.mx);
			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				stheat->Tsdy.hrs, stheat->Tsdy.m, 
				stheat->Tsdy.mntime, stheat->Tsdy.mn, 
				stheat->Tsdy.mxtime, stheat->Tsdy.mx);
            fprintf(fo, "%1d %3.1lf ", stheat->Qdy.hhr, stheat->Qdy.h);
            fprintf(fo, "%1d %3.1lf ", stheat->Qdy.chr, stheat->Qdy.c);      
            fprintf(fo, "%1d %2.0lf ", stheat->Qdy.hmxtime, stheat->Qdy.hmx);
            fprintf(fo, "%1d %2.0lf ", stheat->Qdy.cmxtime, stheat->Qdy.cmx);
			fprintf(fo, "%1d %3.1lf ", stheat->Edy.hrs, stheat->Edy.d);
            fprintf(fo, "%1d %2.0lf ", stheat->Edy.mxtime, stheat->Edy.mx);
			fprintf(fo, " %3.1lf %3.1lf\n", 
				stheat->Qlossdy * cff_kWh, stheat->Qstody * cff_kWh);
		}
		break;
	}
}

void stheatmonprt(FILE *fo, int id, int Nstheat, STHEAT *stheat)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nstheat > 0)
            fprintf(fo, "%s %d\n", STHEAT_TYPE, Nstheat);
		for (i = 0; i < Nstheat; i++, stheat++)
			fprintf(fo, " %s 1 32\n", stheat->name);
		break;
		
	case 1:
		for (i = 0; i < Nstheat; i++, stheat++)
		{	 
			fprintf(fo,"%s_Ht H d %s_Ti T f ", stheat->name, stheat->name);
			fprintf(fo,"%s_ttn h d %s_Tin t f %s_ttm h d %s_Tim t f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo,"%s_Ht H d %s_To T f ", stheat->name, stheat->name);
			fprintf(fo,"%s_ttn h d %s_Ton t f %s_ttm h d %s_Tom t f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo,"%s_Ht H d %s_Ts T f ", stheat->name, stheat->name);
			fprintf(fo,"%s_ttn h d %s_Tsn t f %s_ttm h d %s_Tsm t f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
				stheat->name, stheat->name, stheat->name, stheat->name);
			fprintf(fo, "%s_Qls Q f %s_Qst Q f\n\n",
				stheat->name, stheat->name);
		}
		break;
		
	default:
		for (i = 0; i < Nstheat; i++, stheat++)
		{
            fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				stheat->mTidy.hrs, stheat->mTidy.m, 
				stheat->mTidy.mntime, stheat->mTidy.mn, 
				stheat->mTidy.mxtime, stheat->mTidy.mx);
			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				stheat->mTody.hrs, stheat->mTody.m, 
				stheat->mTody.mntime, stheat->mTody.mn, 
				stheat->mTody.mxtime, stheat->mTody.mx);
			fprintf(fo, "%1d %3.1lf %1d %3.1lf %1d %3.1lf ", 
				stheat->mTsdy.hrs, stheat->mTsdy.m, 
				stheat->mTsdy.mntime, stheat->mTsdy.mn, 
				stheat->mTsdy.mxtime, stheat->mTsdy.mx);
            fprintf(fo, "%1d %3.1lf ", stheat->mQdy.hhr, stheat->mQdy.h);
            fprintf(fo, "%1d %3.1lf ", stheat->mQdy.chr, stheat->mQdy.c);      
            fprintf(fo, "%1d %2.0lf ", stheat->mQdy.hmxtime, stheat->mQdy.hmx);
            fprintf(fo, "%1d %2.0lf ", stheat->mQdy.cmxtime, stheat->mQdy.cmx);
			fprintf(fo, "%1d %3.1lf ", stheat->mEdy.hrs, stheat->mEdy.d);
            fprintf(fo, "%1d %2.0lf ", stheat->mEdy.mxtime, stheat->mEdy.mx);
			fprintf(fo, " %3.1lf %3.1lf\n", 
				stheat->mQlossdy * cff_kWh, stheat->mQstody * cff_kWh);
		}
		break;
	}
}

void stheatmtprt(FILE *fo, int id, int Nstheat, STHEAT *stheat, int Mo, int tt)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nstheat > 0)
            fprintf(fo, "%s %d\n", STHEAT_TYPE, Nstheat);
		for (i = 0; i < Nstheat; i++, stheat++)
			fprintf(fo, " %s 1 1\n", stheat->name);
		break;
		
	case 1:
		for (i = 0; i < Nstheat; i++, stheat++)
		{	 
			fprintf(fo,"%s_E E f \n", stheat->name);
		}
		break;
		
	default:
		for (i = 0; i < Nstheat; i++, stheat++)
		{
			fprintf(fo, " %.2lf\n", stheat->mtEdy[Mo-1][tt-1].d * cff_kWh);
		}
		break;
	}
}
