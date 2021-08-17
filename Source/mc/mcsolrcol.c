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


#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MODEL.h"   /*-----higuchi 070918---*/
#include "fesy.h"
#include "fnmcs.h"
#include "fnbld.h"
#include "fnfio.h"
#include "fnlib.h"


/*　太陽熱集熱器

機器仕様入力　　　　*/

int Colldata (char *type,  char *s, COLLCA *Collca)
{
	char  *st;
	double dt;
	int   id=0;
	
	if ( strcmp(type, COLLECTOR_TYPE) == 0 )
		Collca->type = COLLECTOR_PDT ;
	else
		Collca->type = ACOLLECTOR_PDT ;

	if ((st=strchr(s,'=')) == 0)
	{
		Collca->name = stralloc(s);
		Collca->b0 = Collca->b1 = Collca->Ac = Collca->Ag = -999.0 ;
	}
	else
	{ 
		*st = '\0';
		
        dt=atof(st+1);
        if (strcmp(s,"b0") == 0)
			Collca->b0 = dt;
        else if (strcmp(s,"b1") == 0)
			Collca->b1 = dt;
        else if (strcmp(s,"Fd") == 0)
			Collca->Fd = dt;
        else if (strcmp(s, "Ac") == 0)
			Collca->Ac = dt;
        else if (strcmp(s, "Ag") == 0)
			Collca->Ag= dt;
        else 
			id=1;     
	}
	return (id);
}  
/* ------------------------------------- */

/*  初期設定 */

void Collint(int Ncoll, COLL *Coll, int Nexsf, EXSF *Exs, WDAT *Wd)
{
	int		i, j;
	EXSF	*exs;
	char	Err[SCHAR] ;
	
	for (i = 0; i < Ncoll; i++, Coll++)
	{
		Coll->Ta = &Wd->T;
		Coll->sol = NULL;
		exs = Exs;
		for (j = 0; j < Nexsf; j++, exs++)
		{
			if(strcmp(Coll->cmp->exsname, exs->name) == 0)
				Coll->sol = exs;
		}
		if (Coll->sol == NULL)
			Eprint("Collint", Coll->cmp->exsname);

		if ( Coll->cat->b0 < 0.0 )
		{
			sprintf_s ( Err, sizeof(Err), "Name=%s b0=%.4g", Coll->cmp->name, Coll->cat->b0 ) ;
			Eprint("Collint", Err ) ;
		}
		if ( Coll->cat->b1 < 0.0 )
		{
			sprintf_s ( Err, sizeof(Err), "Name=%s b1=%.4g", Coll->cmp->name, Coll->cat->b1 ) ;
			Eprint("Collint", Err ) ;
		}
		if ( Coll->cat->Ac < 0.0 )
		{
			sprintf_s ( Err, sizeof(Err), "Name=%s Ac=%.4g", Coll->cmp->name, Coll->cat->Ac ) ;
			Eprint("Collint", Err ) ;
		}
		if ( Coll->cat->Ag < 0.0 )
		{
			sprintf_s ( Err, sizeof(Err), "Name=%s Ag=%.4g", Coll->cmp->name, Coll->cat->Ag ) ;
			Eprint("Collint", Err ) ;
		}

		// 総合熱損失係数[W/(m2･K)]の計算
		Coll->cat->Ko = Coll->cat->b1 / Coll->cat->Fd ;
	}
}

// 集熱器の相当外気温度を計算する（制御用）
void CalcCollTe(int Ncoll, COLL *Coll)
{
	double	tgaKo ;
	int		i ;

	for (i = 0; i < Ncoll; i++, Coll++)
	{
		tgaKo = Coll->cat->b0 / Coll->cat->b1 ;
		Coll->Te = scolte(tgaKo, Coll->sol->cinc,
			Coll->sol->Idre, Coll->sol->Idf, *Coll->Ta);
	}
}

/* ------------------------------------- */

/*  特性式の係数   */

void Collcfv(int Ncoll, COLL *Coll)
{
	ELOUT *Eo;
	double cG, Kcw;
	int   i;
	
	for (i = 0; i < Ncoll; i++, Coll++)
	{
		// 制御用の相当外気温度（現在時刻）は計算済みなのでここでは計算しない
		//tgaKo = Coll->cat->b0 / Coll->cat->b1;
		//Coll->Te = scolte(tgaKo, Coll->sol->cinc,
		//	Coll->sol->Idre, Coll->sol->Idf, *Coll->Ta);
		
		if (Coll->cmp->control != OFF_SW)
		{
			Eo = Coll->cmp->elouts;
			Kcw = Coll->cat->b1;
			cG = spcheat(Eo->fluid) * Eo->G;
			Coll->ec = 1.0 - exp((double) (-Kcw * Coll->cmp->Ac / cG));	 
			Coll->D1 = cG * Coll->ec;
			Coll->Do = Coll->D1 * Coll->Te;
			
			Eo->coeffo = cG;
			Eo->Co = Coll->Do;
			*Eo->coeffin = Coll->D1 - cG;

			if (Coll->cat->type == ACOLLECTOR_PDT)
			{
				Eo++ ;
				Eo->coeffo = 1.0 ;
				Eo->Co = 0.0 ;
				*(Eo->coeffin) = -1.0 ;
			}
		}
	}
}
/* ------------------------------------- */

/*  集熱量の計算 */

void Collene(int Ncoll, COLL *Coll)
{
	int i;
	//COLLCA *Collca ;
	
	for (i = 0; i < Ncoll; i++, Coll++)
	{
		Coll->Tin = Coll->cmp->elins->sysvin;
		
		if (Coll->cmp->control != OFF_SW)
			Coll->Q = Coll->Do - Coll->D1 * Coll->Tin;
		else
			Coll->Q = 0.0;

		// 集熱板温度の計算
		if (Coll->Q > 0.)
			Coll->Tcb = Coll->Te - Coll->Q / ( Coll->Ac * Coll->cat->Ko ) ;
		else	// 集熱ポンプ停止時は相当外気温度に等しいとする
			Coll->Tcb = Coll->Te ;
		
		Coll->Sol = Coll->sol->Iw * Coll->cmp->Ac;
	}
}
/* ------------------------------------- */

/*  集熱器到達温度　Te　　　　　　　*/

double scolte (double rtgko, double cinc, double Idre, double Idf, double Ta)
{
	double Cidf=0.91;
	
	return ( rtgko*(Glscid(cinc)*Idre + Cidf*Idf) + Ta );
	
}
/* ------------------------------------- */

/*  集熱器内部変数のポインター  */

int collvptr(char **key, COLL *Coll, VPTR *vptr)
{
	int err = 0;
	
	if (strcmp(key[1], "Te") == 0)
	{
		vptr->ptr = &Coll->Te;
		vptr->type = VAL_CTYPE;
	}
	else if (strcmp(key[1], "Tcb") == 0)
	{
		vptr->ptr = &Coll->Tcb ;
		vptr->type = VAL_CTYPE;
	}
	else
		err = 1;
	
	return err;
}

/* ------------------------------------------------------------- */

void collprint(FILE *fo, int id, int Ncoll, COLL *Coll)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Ncoll > 0)
            fprintf(fo, "%s %d\n", COLLECTOR_TYPE, Ncoll);
		for (i = 0; i < Ncoll; i++, Coll++)
			fprintf(fo, " %s 1 7\n", Coll->name);
		break;
		
	case 1:
		for (i = 0; i < Ncoll; i++, Coll++)
		{	    
			fprintf(fo, "%s_c c c %s_Ti t f %s_To t f %s_Te t f %s_Tcb t f %s_Q q f %s_S e f\n",
				Coll->name, Coll->name, Coll->name, Coll->name, Coll->name, Coll->name, Coll->name);
		}
		break;
		
	default:
		for (i = 0; i < Ncoll; i++, Coll++)
			fprintf(fo, "%c %4.1lf %4.1lf %4.1lf %4.1lf %3.0lf %3.0lf\n", 
			Coll->cmp->elouts->control,
	           Coll->Tin, Coll->cmp->elouts->sysv, Coll->Te, Coll->Tcb,
			   Coll->Q, Coll->Sol);
		break;
	}
}

/* --------------------------- */

/* 日積算値に関する処理 */

void colldyint(int Ncoll, COLL *Coll)
{
	int i;
	
	for (i = 0; i < Ncoll; i++, Coll++)
	{
		svdyint(&Coll->Tidy);
		qdyint(&Coll->Qdy);
		edyint(&Coll->Soldy);
	}
}

void collmonint(int Ncoll, COLL *Coll)
{
	int i;
	
	for (i = 0; i < Ncoll; i++, Coll++)
	{
		svdyint(&Coll->mTidy);
		qdyint(&Coll->mQdy);
		edyint(&Coll->mSoldy);
	}
}

void collday(int Mon, int Day, int ttmm, int Ncoll, COLL *Coll, int Nday, int SimDayend)
{
	int  i;
	char sw;
	
	for (i = 0; i < Ncoll; i++, Coll++)
	{
		// 日集計
		svdaysum(ttmm, Coll->cmp->control, Coll->Tin, &Coll->Tidy);
		qdaysum(ttmm, Coll->cmp->control, Coll->Q, &Coll->Qdy);
		
		if (Coll->Sol > 0.0)
			sw = ON_SW;
		else 
			sw =OFF_SW;
		edaysum(ttmm, sw, Coll->Sol, &Coll->Soldy);

		// 月集計
		svmonsum(Mon, Day, ttmm, Coll->cmp->control, Coll->Tin, &Coll->mTidy, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Coll->cmp->control, Coll->Q, &Coll->mQdy, Nday, SimDayend);
		
		if (Coll->Sol > 0.0)
			sw = ON_SW;
		else 
			sw =OFF_SW;
		emonsum(Mon, Day, ttmm, sw, Coll->Sol, &Coll->mSoldy, Nday, SimDayend);
	}
}

void colldyprt(FILE *fo, int id, int Ncoll, COLL *Coll)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Ncoll > 0)
            fprintf(fo, "%s %d\n", COLLECTOR_TYPE, Ncoll);
		for (i = 0; i < Ncoll; i++, Coll++)
			fprintf(fo," %s 1 18\n", Coll->name);
		break;
		
	case 1:
		for (i = 0; i < Ncoll; i++, Coll++)
		{
			fprintf(fo,"%s_Ht H d %s_T T f ", Coll->name, Coll->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
			fprintf(fo, "%s_He H d %s_S E f %s_te h d %s_Sm e f\n\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
		}
		break;
		
	default:
		for (i = 0; i < Ncoll; i++, Coll++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Coll->Tidy.hrs, Coll->Tidy.m, 
				Coll->Tidy.mntime, Coll->Tidy.mn, 
				Coll->Tidy.mxtime, Coll->Tidy.mx);
			fprintf(fo, "%1ld %3.1lf ", Coll->Qdy.hhr, Coll->Qdy.h);
            fprintf(fo, "%1ld %3.1lf ", Coll->Qdy.chr, Coll->Qdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Coll->Qdy.hmxtime, Coll->Qdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Coll->Qdy.cmxtime, Coll->Qdy.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Coll->Soldy.hrs, Coll->Soldy.d);
            fprintf(fo, "%1ld %2.0lf\n", Coll->Soldy.mxtime, Coll->Soldy.mx);
		}
		break;
	}
}

void collmonprt(FILE *fo, int id, int Ncoll, COLL *Coll)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Ncoll > 0)
            fprintf(fo, "%s %d\n", COLLECTOR_TYPE, Ncoll);
		for (i = 0; i < Ncoll; i++, Coll++)
			fprintf(fo," %s 1 18\n", Coll->name);
		break;
		
	case 1:
		for (i = 0; i < Ncoll; i++, Coll++)
		{
			fprintf(fo,"%s_Ht H d %s_T T f ", Coll->name, Coll->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
			fprintf(fo, "%s_He H d %s_S E f %s_te h d %s_Sm e f\n\n",
				Coll->name, Coll->name, Coll->name, Coll->name);
		}
		break;
		
	default:
		for (i = 0; i < Ncoll; i++, Coll++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Coll->mTidy.hrs, Coll->mTidy.m, 
				Coll->mTidy.mntime, Coll->mTidy.mn, 
				Coll->mTidy.mxtime, Coll->mTidy.mx);
			fprintf(fo, "%1ld %3.1lf ", Coll->mQdy.hhr, Coll->mQdy.h);
            fprintf(fo, "%1ld %3.1lf ", Coll->mQdy.chr, Coll->mQdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Coll->mQdy.hmxtime, Coll->mQdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Coll->mQdy.cmxtime, Coll->mQdy.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Coll->mSoldy.hrs, Coll->mSoldy.d);
            fprintf(fo, "%1ld %2.0lf\n", Coll->mSoldy.mxtime, Coll->mSoldy.mx);
		}
		break;
	}
}


