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

/* mcthex.c */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"

/*  全熱交換器  */

/*  仕様入力  */

int		Thexdata ( char *s, THEXCA *Thexca )
{
	char	*st ;
	double	dt ;
	int		id = 0 ;

	if (( st = strchr ( s, '=' )) == 0 )
		Thexca->name = stralloc ( s ) ;
	else
	{
		*st = '\0' ;
		dt = atof ( st + 1 ) ;

		if ( strcmp ( s, "et" ) == 0 )
			Thexca->et = dt ;
		else if ( strcmp ( s, "eh" ) == 0 )
			Thexca->eh = dt ;
		else
			id = 1 ;
	}

	return ( id ) ;
}

/* ------------------------------------------------------ */

void	Thexint ( int Nthex, THEX *Thex )
{
	int		i ;
	char	s[SCHAR] ;

	for ( i = 0; i < Nthex; i++, Thex++ )
	{
		if ( Thex->cat->eh < 0.0 )
		{
			Thex->type = 't' ;
			Thex->cat->eh = 0.0 ;
		}
		else
		{
			Thex->type = 'h' ;
			//Thex->cat->eh = 1.0 ;
		}

		if ( Thex->cat->et < 0.0 )
		{
			sprintf ( s, "Name=%s catname=%s et=%lf",
				Thex->name, Thex->cat->name, Thex->cat->et ) ;
			Eprint ( "<Thexint>", s ) ;
		}

		Thex->xeinold = Thex->xeoutold = Thex->xoinold = Thex->xooutold = FNXtr ( 26., 50. ) ;
	}
}

/*  全熱交換器出口空気温湿度に関する変数割当  */
void	Thexelm(int NThex, THEX *Thex)
{
	int i;
	ELOUT  *E ;
	ELIN   *elin, *elin2 ;
	
	for (i = 0; i < NThex; i++, Thex++)
	{
		E = Thex->cmp->elouts ;

		// Teinの変数割り当て
		// E:Teoutの計算式、elin2:Tein
		elin2 = E->elins ;

		// E+2:Tooutの計算式、elin:Tein
		elin = (E+2)->elins + 1 ;
		elin->upo = elin->upv = elin2->upo ;

		if ( Thex->cat->eh > 0.0 )
		{
			// E+1:xeoutの計算式、elin:
			elin = (E+1)->elins + 1 ;
			elin->upo = elin->upv = elin2->upo ;

			elin = (E+3)->elins + 3 ;
			elin->upo = elin->upv = elin2->upo ;
		}

		// Toinの変数割り当て
		elin2 = E->elins + 1 ;

		elin = (E+2)->elins ;
		elin->upo = elin->upv = elin2->upo ;

		if ( Thex->cat->eh > 0.0 )
		{
			elin = (E+1)->elins + 3 ;
			elin->upo = elin->upv = elin2->upo ;
			
			elin = (E+3)->elins + 1 ;
			elin->upo = elin->upv = elin2->upo ;

			// Teoutの変数割り当て
			elin = (E+1)->elins + 2 ;
			elin->upo = elin->upv = E ;
			
			// Tooutの割り当て
			elin = (E+3)->elins + 2 ;
			elin->upo = elin->upv = E + 2 ;
			
			// xeinの割り当て
			elin = (E+1)->elins ;
			elin2 = (E+3)->elins + 4 ;
			elin2->upo = elin2->upv = elin->upo ;
			
			// xoinの割り当て
			elin = (E+1)->elins + 4 ;
			elin2 = (E+3)->elins ;
			elin2->upo = elin2->upv = elin->upo ;
		}
	}
}

/* ------------------------------------------------------ */

void	Thexcfv ( int Nthex, THEX *Thex )
{
	ELOUT	*Eoet, *Eoot, *Eoex, *Eoox ;
	//double	*coeffin ;
	double	etCGmin, ehGmin, Aeout, Aein, Aoout, Aoin ;
	int		i ;
	//char	E[SCHAR] ;
	extern double	ca, ro, cv ;
	double	*cfin ;

	for ( i = 0; i < Nthex; i++, Thex++ )
	{
		if ( Thex->cmp->control != OFF_SW )
		{
			Thex->et = Thex->cat->et ;
			Thex->eh = Thex->cat->eh ;

			Eoet = Thex->cmp->elouts ;			// 排気系統（温度）
			Eoex = Thex->cmp->elouts + 1 ;		// 排気系統（エンタルピー）
			Eoot = Thex->cmp->elouts + 2 ;		// 給気系統（温度）
			Eoox = Thex->cmp->elouts + 3 ;		// 給気系統（エンタルピー）

			Thex->Ge = Eoet->G ;
			Thex->Go = Eoot->G ;

			if ( DEBUG )
				printf ( "<Thexcfv>  %s Ge=%lf Go=%lf\n",
					Thex->cmp->name, Thex->Ge, Thex->Go ) ;

			Thex->cGe = spcheat ( Eoet->fluid ) * Thex->Ge ;
			Thex->cGo = spcheat ( Eoot->fluid ) * Thex->Go ;
			etCGmin = Thex->et * dmin ( Thex->cGe, Thex->cGo ) ;
			ehGmin = Thex->eh * dmin ( Thex->Ge, Thex->Go ) ;

			Aein = ca + cv * Thex->xeinold ;
			Aeout = ca + cv * Thex->xeoutold ;
			Aoin = ca + cv * Thex->xoinold ;
			Aoout = ca + cv * Thex->xooutold ;

			// 排気系統（温度）の熱収支
			Eoet->coeffo = Thex->cGe ;
			Eoet->Co = 0.0 ;
			cfin = Eoet->coeffin ;
			*cfin = etCGmin - Thex->cGe ;
			cfin++ ;
			*cfin = - etCGmin ;

			// 給気系統（温度）の熱収支
			Eoot->coeffo = Thex->cGo ;
			Eoot->Co = 0.0 ;
			cfin = Eoot->coeffin ;
			*cfin = etCGmin - Thex->cGo ;
			cfin++ ;
			*cfin = - etCGmin ;

			if ( Thex->type == 'h' )
			{
				// 排気系統（エンタルピー）の熱収支
				Eoex->coeffo = Thex->Ge * ro ;
				Eoex->Co = 0.0 ;
				cfin = Eoex->coeffin ;
				*cfin = ro * ( ehGmin - Thex->Ge ) ;
				cfin++ ;
				*cfin = Aein * ( ehGmin - Thex->Ge ) ;
				cfin++ ;
				*cfin = Aeout * Thex->Ge ;
				cfin++ ;
				*cfin = - ehGmin * Aoin ;
				cfin++ ;
				*cfin = - ehGmin * ro ;
				
				// 給気系統（エンタルピー）の熱収支
				Eoox->coeffo = Thex->Go * ro ;
				Eoox->Co = 0.0 ;
				cfin = Eoox->coeffin ;
				*cfin = ro * ( ehGmin - Thex->Go ) ;
				cfin++ ;
				*cfin = Aoin * ( ehGmin - Thex->Go ) ;
				cfin++ ;
				*cfin = Thex->Go * Aoout ;
				cfin++ ;
				*cfin = - ehGmin * Aein ;
				cfin++ ;
				*cfin = -ehGmin * ro ;
			}
			else
			{
				Eoex->coeffo = 1.0 ;
				*(Eoex->coeffin) = - 1.0 ;

				Eoox->coeffo = 1.0 ;
				*(Eoox->coeffin) = - 1.0 ;
			}
		}
	}
}

void	Thexene ( int Nthex, THEX *Thex )
{
	ELOUT	*Eoet, *Eoot, *Eoex, *Eoox ;
	//double	*coeffin ;
	int		i ;
	//char	E[SCHAR] ;
	extern double	ca, ro, cv ;

	for ( i = 0; i < Nthex; i++, Thex++ )
	{
		Eoet = Thex->cmp->elouts ;			// 排気系統（温度）
		Eoex = Thex->cmp->elouts + 1 ;		// 排気系統（エンタルピー）
		Eoot = Thex->cmp->elouts + 2 ;		// 給気系統（温度）
		Eoox = Thex->cmp->elouts + 3 ;		// 給気系統（エンタルピー）

		Thex->Tein = Eoet->elins->upo->sysv ;
		Thex->Teout = Eoet->sysv ;
		Thex->xein = Eoex->elins->upo->sysv ;
		Thex->xeout = Eoex->sysv ;

		Thex->Toin = Eoot->elins->upo->sysv ;
		Thex->Toout = Eoot->sysv ;
		Thex->xoin = Eoox->elins->upo->sysv ;
		Thex->xoout = Eoox->sysv ;

		Thex->hein = FNH ( Thex->Tein, Thex->xein ) ;
		Thex->heout = FNH ( Thex->Teout, Thex->xeout ) ;
		Thex->hoin = FNH ( Thex->Toin, Thex->xoin ) ;
		Thex->hoout = FNH ( Thex->Toout, Thex->xoout ) ;

		if ( Thex->cmp->control != OFF_SW )
		{
			// 交換熱量の計算
			Thex->Qes = ca * Thex->Ge * ( Thex->Teout - Thex->Tein ) ;
			Thex->Qel = ro * Thex->Ge * ( Thex->xeout - Thex->xein ) ;
			//Thex->Qet = Thex->Ge * ( Thex->heout - Thex->hein ) ;
			Thex->Qet = Thex->Qes + Thex->Qel ;

			Thex->Qos = ca * Thex->Go * ( Thex->Toout - Thex->Toin ) ;
			Thex->Qol = ro * Thex->Go * ( Thex->xoout - Thex->xoin ) ;
			//Thex->Qot = Thex->Go * ( Thex->hoout - Thex->hoin ) ;
			Thex->Qot = Thex->Qos + Thex->Qol ;

			// 前時刻の絶対湿度の入れ替え
			Thex->xeinold = Thex->xein ;
			Thex->xeoutold = Thex->xeout ;
			Thex->xoinold = Thex->xoin ;
			Thex->xooutold = Thex->xoout ;
		}
		else
		{
			Thex->Qes = Thex->Qel = Thex->Qet = Thex->Qos = Thex->Qol = Thex->Qot = 0.0 ;
			Thex->Ge = Thex->Tein = Thex->Teout = Thex->xein = Thex->xeout = 0.0 ;
			Thex->hein = Thex->heout = 0.0 ;
			Thex->Go = Thex->Toin = Thex->Toout = Thex->xoin = Thex->xoout = 0.0 ;
			Thex->hoin = Thex->hoout = 0.0 ;
		}
	}
}

void	Thexprint(FILE *fo, int id, int Nthex, THEX *Thex )
{
	int i;
	ELOUT *el ;
	
	switch (id)
	{
	case 0:
		if (Nthex >0)
            fprintf(fo, "%s %d\n", THEX_TYPE, Nthex);
		for (i = 0; i < Nthex; i++, Thex++)
			fprintf(fo, " %s 1 22\n", Thex->name);

		break;
		
	case 1:
		for (i = 0; i < Nthex; i++, Thex++)
		{
			fprintf(fo, "%s_ce c c %s_Ge m f %s_Tei t f %s_Teo t f %s_xei t f %s_xeo t f\n",
				Thex->name,  Thex->name, Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_hei h f %s_heo h f %s_Qes q f %s_Qel q f %s_Qet q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name, Thex->name);
			
			fprintf(fo, "%s_co c c %s_Go m f %s_Toi t f %s_Too t f %s_xoi t f %s_xoo t f\n",
				Thex->name,  Thex->name, Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_hoi h f %s_hoo h f %s_Qos q f %s_Qol q f %s_Qot q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name, Thex->name);
		}
		break;
		
	default:
		for (i = 0; i < Nthex; i++, Thex++)
		{
			el = Thex->cmp->elouts ;
			fprintf(fo, "%c %6.4g %4.2lf %4.2lf %.4lf %.4lf ", 
				el->control, Thex->Ge, Thex->Tein, Thex->Teout, Thex->xein, Thex->xeout );
			fprintf(fo, "%.0lf %.0lf %.2lf %.2lf %.2lf\n",
				Thex->hein, Thex->heout, Thex->Qes, Thex->Qel, Thex->Qet ) ;

			el += 2 ;
			fprintf(fo, "%c %6.4g %4.2lf %4.2lf %.4lf %.4lf ", 
				el->control, Thex->Go, Thex->Toin, Thex->Toout, Thex->xoin, Thex->xoout );
			fprintf(fo, "%.0lf %.0lf %.2lf %.2lf %.2lf\n",
				Thex->hoin, Thex->hoout, Thex->Qos, Thex->Qol, Thex->Qot ) ;
		}
		break;
	}
} 

void	Thexdyint(int Nthex, THEX *Thex)
{
	int i;
	
	for (i = 0; i < Nthex; i++, Thex++)
	{
		svdyint(&Thex->Teidy);
		svdyint(&Thex->Teody);
		svdyint(&Thex->xeidy);
		svdyint(&Thex->xeody);

		svdyint(&Thex->Toidy);
		svdyint(&Thex->Toody);
		svdyint(&Thex->xoidy);
		svdyint(&Thex->xoody);
		
		qdyint(&Thex->Qdyes);
		qdyint(&Thex->Qdyel);
		qdyint(&Thex->Qdyet);
		
		qdyint(&Thex->Qdyos);
		qdyint(&Thex->Qdyol);
		qdyint(&Thex->Qdyot);
	}
}

void	Thexmonint(int Nthex, THEX *Thex)
{
	int i;
	
	for (i = 0; i < Nthex; i++, Thex++)
	{
		svdyint(&Thex->mTeidy);
		svdyint(&Thex->mTeody);
		svdyint(&Thex->mxeidy);
		svdyint(&Thex->mxeody);

		svdyint(&Thex->mToidy);
		svdyint(&Thex->mToody);
		svdyint(&Thex->mxoidy);
		svdyint(&Thex->mxoody);
		
		qdyint(&Thex->mQdyes);
		qdyint(&Thex->mQdyel);
		qdyint(&Thex->mQdyet);
		
		qdyint(&Thex->mQdyos);
		qdyint(&Thex->mQdyol);
		qdyint(&Thex->mQdyot);
	}
}

void	Thexday(int Mon, int Day, int ttmm, int Nthex, THEX *Thex, int Nday, int SimDayend)
{
	int i;
	
	for (i = 0; i < Nthex; i++, Thex++)
	{ 
		// 日集計
		svdaysum(ttmm, Thex->cmp->control, Thex->Tein, &Thex->Teidy);
		svdaysum(ttmm, Thex->cmp->control, Thex->Teout, &Thex->Teody);
		svdaysum(ttmm, Thex->cmp->control, Thex->xein, &Thex->xeidy);
		svdaysum(ttmm, Thex->cmp->control, Thex->xeout, &Thex->xeody);
		
		svdaysum(ttmm, Thex->cmp->control, Thex->Toin, &Thex->Toidy);
		svdaysum(ttmm, Thex->cmp->control, Thex->Toout, &Thex->Toody);
		svdaysum(ttmm, Thex->cmp->control, Thex->xoin, &Thex->xoidy);
		svdaysum(ttmm, Thex->cmp->control, Thex->xoout, &Thex->xoody);
		
		qdaysum(ttmm, Thex->cmp->control, Thex->Qes, &Thex->Qdyes);
		qdaysum(ttmm, Thex->cmp->control, Thex->Qel, &Thex->Qdyel);
		qdaysum(ttmm, Thex->cmp->control, Thex->Qet, &Thex->Qdyet);
		
		qdaysum(ttmm, Thex->cmp->control, Thex->Qos, &Thex->Qdyos);
		qdaysum(ttmm, Thex->cmp->control, Thex->Qol, &Thex->Qdyol);
		qdaysum(ttmm, Thex->cmp->control, Thex->Qot, &Thex->Qdyot);
		
		// 月集計
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Tein, &Thex->mTeidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Teout, &Thex->mTeody, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xein, &Thex->mxeidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xeout, &Thex->mxeody, Nday, SimDayend);
		
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Toin, &Thex->mToidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Toout, &Thex->mToody, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xoin, &Thex->mxoidy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->xoout, &Thex->mxoody, Nday, SimDayend);
		
		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qes, &Thex->mQdyes, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qel, &Thex->mQdyel, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qet, &Thex->mQdyet, Nday, SimDayend);
		
		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qos, &Thex->mQdyos, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qol, &Thex->mQdyol, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Thex->cmp->control, Thex->Qot, &Thex->mQdyot, Nday, SimDayend);
	}
}

void	Thexdyprt(FILE *fo, int id, int Nthex, THEX *Thex)
{
	int i ;
	
	switch (id)
	{
	case 0:
		if (Nthex > 0)
            fprintf(fo, "%s %d\n", THEX_TYPE, Nthex);
		for (i = 0; i < Nthex; i++, Thex++)
			fprintf(fo, " %s 1 48\n", Thex->name);
		break;
		
	case 1:
		for (i = 0; i < Nthex; i++, Thex++)
		{	 
			fprintf(fo,"%s_Hte H d %s_Te T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_ttne h d %s_Ten t f %s_ttme h d %s_Tem t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo,"%s_Hto H d %s_To T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_ttno h d %s_Ton t f %s_ttmo h d %s_Tom t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			
			fprintf(fo,"%s_Hxe H d %s_xe T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_txne h d %s_xen t f %s_txme h d %s_xem t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo,"%s_Hxo H d %s_xo T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_txno h d %s_xon t f %s_txmo h d %s_xom t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);

			fprintf(fo, "%s_Hhs H d %s_Qsh Q f %s_Hcs H d %s_Qsc Q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_ths h d %s_qsh q f %s_tcs h d %s_qsc q f\n\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_Hhl H d %s_Qlh Q f %s_Hcl H d %s_Qlc Q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_thl h d %s_qlh q f %s_tcl h d %s_qlc q f\n\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_Hht H d %s_Qth Q f %s_Hct H d %s_Qtc Q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_tht h d %s_qth q f %s_tct h d %s_qtc q f\n\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
		}
		break;
		
	default:
		for (i = 0; i < Nthex; i++, Thex++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Thex->Teidy.hrs, Thex->Teidy.m, 
				Thex->Teidy.mntime, Thex->Teidy.mn, 
				Thex->Teidy.mxtime, Thex->Teidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf\n", 
				Thex->Toidy.hrs, Thex->Toidy.m, 
				Thex->Toidy.mntime, Thex->Toidy.mn, 
				Thex->Toidy.mxtime, Thex->Toidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Thex->xeidy.hrs, Thex->xeidy.m, 
				Thex->xeidy.mntime, Thex->xeidy.mn, 
				Thex->xeidy.mxtime, Thex->xeidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf\n", 
				Thex->xoidy.hrs, Thex->xoidy.m, 
				Thex->xoidy.mntime, Thex->xoidy.mn, 
				Thex->xoidy.mxtime, Thex->xoidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Thex->Qdyes.hhr, Thex->Qdyes.h);
            fprintf(fo, "%1ld %3.1lf ", Thex->Qdyes.chr, Thex->Qdyes.c);      
            fprintf(fo, "%1ld %2.0lf ", Thex->Qdyes.hmxtime, Thex->Qdyes.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Thex->Qdyes.cmxtime, Thex->Qdyes.cmx);

            fprintf(fo, "%1ld %3.1lf ", Thex->Qdyel.hhr, Thex->Qdyel.h);
            fprintf(fo, "%1ld %3.1lf ", Thex->Qdyel.chr, Thex->Qdyel.c);      
            fprintf(fo, "%1ld %2.0lf ", Thex->Qdyel.hmxtime, Thex->Qdyel.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Thex->Qdyel.cmxtime, Thex->Qdyel.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Thex->Qdyet.hhr, Thex->Qdyet.h);
            fprintf(fo, "%1ld %3.1lf ", Thex->Qdyet.chr, Thex->Qdyet.c);      
            fprintf(fo, "%1ld %2.0lf ", Thex->Qdyet.hmxtime, Thex->Qdyet.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Thex->Qdyet.cmxtime, Thex->Qdyet.cmx);
		}
		break;
	}
}

void	Thexmonprt(FILE *fo, int id, int Nthex, THEX *Thex)
{
	int i ;
	
	switch (id)
	{
	case 0:
		if (Nthex > 0)
            fprintf(fo, "%s %d\n", THEX_TYPE, Nthex);
		for (i = 0; i < Nthex; i++, Thex++)
			fprintf(fo, " %s 1 48\n", Thex->name);
		break;
		
	case 1:
		for (i = 0; i < Nthex; i++, Thex++)
		{	 
			fprintf(fo,"%s_Hte H d %s_Te T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_ttne h d %s_Ten t f %s_ttme h d %s_Tem t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo,"%s_Hto H d %s_To T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_ttno h d %s_Ton t f %s_ttmo h d %s_Tom t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			
			fprintf(fo,"%s_Hxe H d %s_xe T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_txne h d %s_xen t f %s_txme h d %s_xem t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo,"%s_Hxo H d %s_xo T f ", Thex->name, Thex->name);
			fprintf(fo,"%s_txno h d %s_xon t f %s_txmo h d %s_xom t f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);

			fprintf(fo, "%s_Hhs H d %s_Qsh Q f %s_Hcs H d %s_Qsc Q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_ths h d %s_qsh q f %s_tcs h d %s_qsc q f\n\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_Hhl H d %s_Qlh Q f %s_Hcl H d %s_Qlc Q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_thl h d %s_qlh q f %s_tcl h d %s_qlc q f\n\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_Hht H d %s_Qth Q f %s_Hct H d %s_Qtc Q f\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
			fprintf(fo, "%s_tht h d %s_qth q f %s_tct h d %s_qtc q f\n\n",
				Thex->name, Thex->name, Thex->name, Thex->name);
		}
		break;
		
	default:
		for (i = 0; i < Nthex; i++, Thex++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Thex->mTeidy.hrs, Thex->mTeidy.m, 
				Thex->mTeidy.mntime, Thex->mTeidy.mn, 
				Thex->mTeidy.mxtime, Thex->mTeidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf\n", 
				Thex->mToidy.hrs, Thex->mToidy.m, 
				Thex->mToidy.mntime, Thex->mToidy.mn, 
				Thex->mToidy.mxtime, Thex->mToidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Thex->mxeidy.hrs, Thex->mxeidy.m, 
				Thex->mxeidy.mntime, Thex->mxeidy.mn, 
				Thex->mxeidy.mxtime, Thex->mxeidy.mx);
			fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf\n", 
				Thex->mxoidy.hrs, Thex->mxoidy.m, 
				Thex->mxoidy.mntime, Thex->mxoidy.mn, 
				Thex->mxoidy.mxtime, Thex->mxoidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Thex->mQdyes.hhr, Thex->mQdyes.h);
            fprintf(fo, "%1ld %3.1lf ", Thex->mQdyes.chr, Thex->mQdyes.c);      
            fprintf(fo, "%1ld %2.0lf ", Thex->mQdyes.hmxtime, Thex->mQdyes.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Thex->mQdyes.cmxtime, Thex->mQdyes.cmx);

            fprintf(fo, "%1ld %3.1lf ", Thex->mQdyel.hhr, Thex->mQdyel.h);
            fprintf(fo, "%1ld %3.1lf ", Thex->mQdyel.chr, Thex->mQdyel.c);      
            fprintf(fo, "%1ld %2.0lf ", Thex->mQdyel.hmxtime, Thex->mQdyel.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Thex->mQdyel.cmxtime, Thex->mQdyel.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Thex->mQdyet.hhr, Thex->mQdyet.h);
            fprintf(fo, "%1ld %3.1lf ", Thex->mQdyet.chr, Thex->mQdyet.c);      
            fprintf(fo, "%1ld %2.0lf ", Thex->mQdyet.hmxtime, Thex->mQdyet.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Thex->mQdyet.cmxtime, Thex->mQdyet.cmx);
		}
		break;
	}
}
