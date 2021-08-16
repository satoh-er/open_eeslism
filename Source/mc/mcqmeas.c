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

/*  mcqmeas.c  */

/*  QMEAS */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"
#include "fnesy.h"
#include "winerror.h"

void	Qmeaselm ( int Nqmeas, QMEAS *Qmeas )			// 出入り口温湿度の割り当て
{
	int	i, N ;
	PLIST	*Plist ;
	PELM	*Pelm ;

	for ( i = 0; i < Nqmeas; i++, Qmeas++ )
	{
		Plist = Qmeas->PlistTh ;
		N = Qmeas->Nelmh ;
		Pelm = Plist->pelm + N ;
		if ( N > 0
			&& (strcmp(Pelm->cmp->eqptype, DIVERG_TYPE)) != 0
			&& (strcmp(Pelm->cmp->eqptype, DIVGAIR_TYPE)) != 0)
		{
			Pelm = Plist->pelm + (N-1) ;
			Qmeas->Th = &Pelm->out->sysv ;
			
			if ( Plist->plistx != NULL )
			{
				Qmeas->Plistxh = Plist->plistx ;
				Plist = Qmeas->Plistxh ;
				Pelm = Plist->pelm + (N-1) ;
				Qmeas->xh = &Pelm->out->sysv ;
			}
		}
		else
		{
			Pelm = Plist->pelm + 1 ;
			//Qmeas->Th = &Pelm->in->sysvin ;

			if ( Pelm->out != NULL )
				Qmeas->Th = &Pelm->out->elins->sysvin ;
			else
				Qmeas->Th = &Pelm->in->sysvin ;

			if ( Plist->plistx != NULL )
			{
				Qmeas->Plistxh = Plist->plistx ;
				Pelm = Qmeas->Plistxh->pelm + 1 ;

				if ( Pelm->out != NULL )
					Qmeas->xh = &Pelm->out->elins->sysvin ;
				else
					Qmeas->xh = &Pelm->in->sysvin ;
			}
		}

		Plist = Qmeas->PlistTc ;
		N = Qmeas->Nelmc ;
		Pelm = Plist->pelm + N ;
		if ( N > 0
			&& (strcmp(Pelm->cmp->eqptype, DIVERG_TYPE)) != 0
			&& (strcmp(Pelm->cmp->eqptype, DIVGAIR_TYPE)) != 0)
		{
			Pelm = Plist->pelm + (N-1) ;
			Qmeas->Tc = &Pelm->out->sysv ;
			
			if ( Plist->plistx != NULL )
			{
				Qmeas->Plistxc = Plist->plistx ;
				Plist = Qmeas->Plistxc ;
				Pelm = Plist->pelm + (N-1) ;
				Qmeas->xc = &Pelm->out->sysv ;
			}
		}
		else
		{
			Pelm = Plist->pelm + 1 ;
			//Qmeas->Th = &Pelm->in->sysvin ;

			if ( Pelm->out != NULL )
				Qmeas->Tc = &Pelm->out->elins->sysvin ;
			else
				Qmeas->Tc = &Pelm->in->sysvin ;

			if ( Plist->plistx != NULL )
			{
				Qmeas->Plistxc = Plist->plistx ;
				Pelm = Qmeas->Plistxc->pelm + 1 ;

				if ( Pelm->out != NULL )
					Qmeas->xc = &Pelm->out->elins->sysvin ;
				else
					Qmeas->xc = &Pelm->in->sysvin ;
			}
		}
	}
}

void	Qmeasene (int Nqmeas, QMEAS *Qmeas )
{
	int		i;
	PLIST	*PG, *Ph, *Pc ;
	extern double ro ;

	for ( i = 0; i < Nqmeas; i++, Qmeas++ )
	{
		PG = Qmeas->PlistG ;
		Ph = Qmeas->PlistTh ;
		Pc = Qmeas->PlistTc ;
		if ( PG->control != OFF_SW && Ph->control != OFF_SW && Pc->control != OFF_SW )
		{
			Qmeas->Qs = spcheat(PG->Mpath->fluid) * *Qmeas->G * ( *Qmeas->Th - *Qmeas->Tc ) ;

			if ( Qmeas->Plistxc != NULL )
				Qmeas->Ql = ro * *Qmeas->G * ( *Qmeas->xh - *Qmeas->xc ) ;
			else
				Qmeas->Ql = 0.0 ;
			
			Qmeas->Qt = Qmeas->Qs + Qmeas->Ql ;
		}
		else
			Qmeas->Qs = Qmeas->Ql = Qmeas->Qt = 0.0 ;
	}
}

void	Qmeasprint(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas )
{
	int i;
	ELOUT *el ;
	
	switch (id)
	{
	case 0:
		if (Nqmeas >0)
            fprintf(fo, "%s %d\n", QMEAS_TYPE, Nqmeas);
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
				fprintf(fo, " %s 1 9\n", Qmeas->name);
			else
				fprintf(fo, " %s 1 5\n", Qmeas->name);
		}
		break;
		
	case 1:
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
			{
				fprintf(fo, "%s_ce c c %s_G m f %s_Th t f %s_Tc t f %s_xh t f %s_xc t f\n",
					Qmeas->name,  Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_Qs q f %s_Ql q f %s_Qt q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name);
			}
			else
			{
				fprintf(fo, "%s_ce c c %s_G m f %s_Th t f %s_Tc t f\n",
					Qmeas->name,  Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_Qt q f\n", Qmeas->name);
			}
		}
		break;
		
	default:
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			el = Qmeas->cmp->elouts ;
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
			{
				fprintf(fo, "%c %6.4g %4.1lf %4.1lf %.3lf %.3lf ", 
					el->control, *Qmeas->G, *Qmeas->Th, *Qmeas->Tc, *Qmeas->xh, *Qmeas->xc );
				fprintf(fo, "%.0lf %.0lf %.0lf\n",
					Qmeas->Qs, Qmeas->Ql, Qmeas->Qt ) ;
			}
			else
			{
				fprintf(fo, "%c %6.4g %4.1lf %4.1lf ", 
					el->control, *Qmeas->G, *Qmeas->Th, *Qmeas->Tc );
				fprintf(fo, "%.0lf\n", Qmeas->Qt ) ;
			}
		}
		break;
	}
} 

void	Qmeasdyint(int Nqmeas, QMEAS *Qmeas )
{
	int	i ;

	for ( i = 0; i < Nqmeas; i++, Qmeas++ )
	{
		svdyint ( &Qmeas->Tcdy ) ;
		svdyint ( &Qmeas->Thdy ) ;
		svdyint ( &Qmeas->xcdy ) ;
		svdyint ( &Qmeas->xhdy ) ;

		qdyint ( &Qmeas->Qdys ) ;
		qdyint ( &Qmeas->Qdyl ) ;
		qdyint ( &Qmeas->Qdyt ) ;
	}
}

void	Qmeasmonint(int Nqmeas, QMEAS *Qmeas )
{
	int	i ;

	for ( i = 0; i < Nqmeas; i++, Qmeas++ )
	{
		svdyint ( &Qmeas->mTcdy ) ;
		svdyint ( &Qmeas->mThdy ) ;
		svdyint ( &Qmeas->mxcdy ) ;
		svdyint ( &Qmeas->mxhdy ) ;

		qdyint ( &Qmeas->mQdys ) ;
		qdyint ( &Qmeas->mQdyl ) ;
		qdyint ( &Qmeas->mQdyt ) ;
	}
}

void	Qmeasday(int Mon, int Day, int ttmm, int Nqmeas, QMEAS *Qmeas, int Nday, int SimDayend)
{
	int i;
	
	for (i = 0; i < Nqmeas; i++, Qmeas++)
	{
		// 日集計
		svdaysum(ttmm, Qmeas->PlistG->control, *Qmeas->Th, &Qmeas->Thdy);
		svdaysum(ttmm, Qmeas->PlistG->control, *Qmeas->Tc, &Qmeas->Tcdy);

		if ( Qmeas->xh != NULL )
			svdaysum(ttmm, Qmeas->PlistG->control, *Qmeas->xh, &Qmeas->xhdy);

		if ( Qmeas->xc != NULL )
			svdaysum(ttmm, Qmeas->PlistG->control, *Qmeas->xc, &Qmeas->xcdy);

		qdaysum(ttmm, Qmeas->PlistG->control, Qmeas->Qs, &Qmeas->Qdys);
		qdaysum(ttmm, Qmeas->PlistG->control, Qmeas->Ql, &Qmeas->Qdyl);
		qdaysum(ttmm, Qmeas->PlistG->control, Qmeas->Qt, &Qmeas->Qdyt);
		
		// 月集計
		svmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, *Qmeas->Th, &Qmeas->mThdy, Nday, SimDayend);
		svmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, *Qmeas->Tc, &Qmeas->mTcdy, Nday, SimDayend);

		if ( Qmeas->xh != NULL )
			svmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, *Qmeas->xh, &Qmeas->mxhdy, Nday, SimDayend);

		if ( Qmeas->xc != NULL )
			svmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, *Qmeas->xc, &Qmeas->mxcdy, Nday, SimDayend);

		qmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, Qmeas->Qs, &Qmeas->mQdys, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, Qmeas->Ql, &Qmeas->mQdyl, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Qmeas->PlistG->control, Qmeas->Qt, &Qmeas->mQdyt, Nday, SimDayend);
	}
}

void	Qmeasdyprt(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nqmeas > 0)
            fprintf(fo, "%s %d\n", QMEAS_TYPE, Nqmeas);
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
				fprintf(fo, " %s 1 24\n", Qmeas->name);
			else
				fprintf(fo, " %s 1 8\n", Qmeas->name ) ;
		}
		break;
		
	case 1:
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
			{
				fprintf(fo, "%s_Hhs H d %s_Qsh Q f %s_Hcs H d %s_Qsc Q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_ths h d %s_qsh q f %s_tcs h d %s_qsc q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_Hhl H d %s_Qlh Q f %s_Hcl H d %s_Qlc Q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_thl h d %s_qlh q f %s_tcl h d %s_qlc q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
			}

			fprintf(fo, "%s_Hht H d %s_Qth Q f %s_Hct H d %s_Qtc Q f\n",
				Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
			fprintf(fo, "%s_tht h d %s_qth q f %s_tct h d %s_qtc q f\n\n",
				Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
		}
		break;
		
	default:
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
			{
				fprintf(fo, "%1ld %3.1lf ", Qmeas->Qdys.hhr, Qmeas->Qdys.h);
				fprintf(fo, "%1ld %3.1lf ", Qmeas->Qdys.chr, Qmeas->Qdys.c);      
				fprintf(fo, "%1ld %2.0lf ", Qmeas->Qdys.hmxtime, Qmeas->Qdys.hmx);
				fprintf(fo, "%1ld %2.0lf\n", Qmeas->Qdys.cmxtime, Qmeas->Qdys.cmx);
				
				fprintf(fo, "%1ld %3.1lf ", Qmeas->Qdyl.hhr, Qmeas->Qdyl.h);
				fprintf(fo, "%1ld %3.1lf ", Qmeas->Qdyl.chr, Qmeas->Qdyl.c);      
				fprintf(fo, "%1ld %2.0lf ", Qmeas->Qdyl.hmxtime, Qmeas->Qdyl.hmx);
				fprintf(fo, "%1ld %2.0lf\n", Qmeas->Qdyl.cmxtime, Qmeas->Qdyl.cmx);
			}
			
            fprintf(fo, "%1ld %3.1lf ", Qmeas->Qdyt.hhr, Qmeas->Qdyt.h);
            fprintf(fo, "%1ld %3.1lf ", Qmeas->Qdyt.chr, Qmeas->Qdyt.c);      
            fprintf(fo, "%1ld %2.0lf ", Qmeas->Qdyt.hmxtime, Qmeas->Qdyt.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Qmeas->Qdyt.cmxtime, Qmeas->Qdyt.cmx);
		}
		break;
	}
}

void	Qmeasmonprt(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nqmeas > 0)
            fprintf(fo, "%s %d\n", QMEAS_TYPE, Nqmeas);
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
				fprintf(fo, " %s 1 24\n", Qmeas->name);
			else
				fprintf(fo, " %s 1 8\n", Qmeas->name ) ;
		}
		break;
		
	case 1:
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
			{
				fprintf(fo, "%s_Hhs H d %s_Qsh Q f %s_Hcs H d %s_Qsc Q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_ths h d %s_qsh q f %s_tcs h d %s_qsc q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_Hhl H d %s_Qlh Q f %s_Hcl H d %s_Qlc Q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
				fprintf(fo, "%s_thl h d %s_qlh q f %s_tcl h d %s_qlc q f\n",
					Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
			}

			fprintf(fo, "%s_Hht H d %s_Qth Q f %s_Hct H d %s_Qtc Q f\n",
				Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
			fprintf(fo, "%s_tht h d %s_qth q f %s_tct h d %s_qtc q f\n\n",
				Qmeas->name, Qmeas->name, Qmeas->name, Qmeas->name);
		}
		break;
		
	default:
		for (i = 0; i < Nqmeas; i++, Qmeas++)
		{
			if ( Qmeas->Plistxc != NULL && Qmeas->Plistxh != NULL )
			{
				fprintf(fo, "%1ld %3.1lf ", Qmeas->mQdys.hhr, Qmeas->mQdys.h);
				fprintf(fo, "%1ld %3.1lf ", Qmeas->mQdys.chr, Qmeas->mQdys.c);      
				fprintf(fo, "%1ld %2.0lf ", Qmeas->mQdys.hmxtime, Qmeas->mQdys.hmx);
				fprintf(fo, "%1ld %2.0lf\n", Qmeas->mQdys.cmxtime, Qmeas->mQdys.cmx);
				
				fprintf(fo, "%1ld %3.1lf ", Qmeas->mQdyl.hhr, Qmeas->mQdyl.h);
				fprintf(fo, "%1ld %3.1lf ", Qmeas->mQdyl.chr, Qmeas->mQdyl.c);      
				fprintf(fo, "%1ld %2.0lf ", Qmeas->mQdyl.hmxtime, Qmeas->mQdyl.hmx);
				fprintf(fo, "%1ld %2.0lf\n", Qmeas->mQdyl.cmxtime, Qmeas->mQdyl.cmx);
			}
			
            fprintf(fo, "%1ld %3.1lf ", Qmeas->mQdyt.hhr, Qmeas->mQdyt.h);
            fprintf(fo, "%1ld %3.1lf ", Qmeas->mQdyt.chr, Qmeas->mQdyt.c);      
            fprintf(fo, "%1ld %2.0lf ", Qmeas->mQdyt.hmxtime, Qmeas->mQdyt.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Qmeas->mQdyt.cmxtime, Qmeas->mQdyt.cmx);
		}
		break;
	}
}
