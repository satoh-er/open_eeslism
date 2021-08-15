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

#include <stdio.h>
#include "common.h"
#include "fesy.h"

#include "fnbld.h"
#include "sun.h"

/* -------------------------------- */

void	dprdayweek ( int *daywk )
{  
	int		k, d, dmax = 366 ;
	extern char	DAYweek[][4];
	
	printf ( "---  Day of week -----\n   " ) ;
	for ( d = 0; d < 8; d++ )
		printf ( "  %s=%d  ", DAYweek[d], d ) ;
	printf ( "\n" ) ;
	
	k = 1 ;
	daywk++ ;
	for ( d = 1; d < dmax; d++, daywk++ )
	{
		if ( FNNday ( k, 1 ) == d )
		{
			printf ( "\n%2d - ", k ) ;
			k++ ;
		}
		printf ( "%2d", *daywk ) ;
	}
	printf ( "\n\n" ) ; 
}
/* ----------------------------------------------------------------- */

void	dprschtable ( SEASN *Ssn, WKDY *Wkd, DSCH *Dh, DSCW *Dw )
{ 
	int		is, iw, sc, sw, j, js, jsc, jsw ;
	int		Ns, Nw, Nsc, Nsw, *sday, *eday, *wday, *stime, *etime ;
	double	*val ;
	char	*mode ;
	SEASN	*Seasn ;
	WKDY	*Wkdy ;
	DSCH	*Dsch ;
	DSCW	*Dscw ;
	extern char	DAYweek[][4] ;
	
	Seasn = Ssn ;
	Wkdy = Wkd ;
	Dsch = Dh ;
	Dscw = Dw ;

	Ns = Seasn->end ;
	Nw = Wkdy->end ;
	Nsc = Dsch->end ;
	Nsw = Dscw->end ;

	if ( DEBUG )
	{
		printf("\n*** dprschtable  ***\n");
		printf("\n=== Schtable end  is=%d  iw=%d  sc=%d  sw=%d\n", Ns, Nw, Nsc, Nsw ) ;
		
		for ( is = 0; is < Ns; is++, Seasn++ )
		{
			printf ( "\n- %s", Seasn->name ) ;
			
			sday = Seasn->sday ;
			eday = Seasn->eday ;
			for ( js = 0; js < Seasn->N; js++, sday++, eday++ )
				printf ( "  %4d-%4d", *sday, *eday ) ;
		}
		
		for ( iw = 0; iw < Nw; iw++, Wkdy++ )
		{
			printf ( "\n- %s", Wkdy->name ) ;
			
			wday = Wkdy->wday ;
			for ( j = 0; j < 8; j++, wday++ )
				printf ( "   %d", *wday ) ;
		}
		
		for ( sc = 0; sc < Nsc; sc++, Dsch++ )
		{
			printf ( "\n-VL   %10s (%2d) ", Dsch->name, sc ) ;
			
			stime = Dsch->stime ;
			val = Dsch->val ;
			etime = Dsch->etime ;
			for ( jsc = 0; jsc < Dsch->N; jsc++, stime++, val++, etime++ )
				printf ( "  %4d-(%6.2lf)-%4d", *stime, *val, *etime ) ;
		} 
		
		for ( sw = 0; sw < Nsw; sw++, Dscw++ )
		{
			printf ( "\n-SW   %10s (%2d) ", Dscw->name, sw ) ;
			
			stime = Dscw->stime ;
			mode = Dscw->mode ;
			etime = Dscw->etime ;
			for ( jsw = 0; jsw < Dscw->N; jsw++, stime++, mode++, etime++ )
				printf ( "  %4d-( %c)-%4d", *stime, *mode, *etime ) ;
		} 
	}

	Seasn = Ssn ;
	Wkdy = Wkd ;
	Dsch = Dh ;
	Dscw = Dw ;
	
	if ( ferr )
	{
		fprintf(ferr,"\n*** dprschtable  ***\n");
		fprintf(ferr,"\n=== Schtable end  is=%d  iw=%d  sc=%d  sw=%d\n", Ns, Nw, Nsc, Nsw ) ;
		
		for ( is = 0; is < Ns; is++, Seasn++ )
		{
			fprintf (ferr, "\n\t%s", Seasn->name ) ;
			
			sday = Seasn->sday ;
			eday = Seasn->eday ;
			for ( js = 0; js < Seasn->N; js++, sday++, eday++ )
				fprintf (ferr, "\t%d-%d", *sday, *eday ) ;
		}
		
		for ( j = 0; j < 8; j++ )
			fprintf ( ferr, "\t%s", DAYweek[j] ) ;

		for ( iw = 0; iw < Nw; iw++, Wkdy++ )
		{
			fprintf (ferr, "\n%s", Wkdy->name ) ;
			
			wday = Wkdy->wday ;
			for ( j = 0; j < 8; j++, wday++ )
				fprintf (ferr, "\t%d", *wday ) ;
		}
		
		for ( sc = 0; sc < Nsc; sc++, Dsch++ )
		{
			fprintf (ferr, "\nVL\t%s\t[%d]", Dsch->name, sc ) ;
			
			stime = Dsch->stime ;
			val = Dsch->val ;
			etime = Dsch->etime ;
			for ( jsc = 0; jsc < Dsch->N; jsc++, stime++, val++, etime++ )
				fprintf (ferr, "\t%d-(%.2g)-%d", *stime, *val, *etime ) ;
		} 
		
		for ( sw = 0; sw < Nsw; sw++, Dscw++ )
		{
			fprintf (ferr, "\nSW\t%s\t[%d]", Dscw->name, sw ) ;
			
			stime = Dscw->stime ;
			mode = Dscw->mode ;
			etime = Dscw->etime ;
			for ( jsw = 0; jsw < Dscw->N; jsw++, stime++, mode++, etime++ )
				fprintf (ferr, "\t%d-(%c)-%d", *stime, *mode, *etime ) ;
		}

		fprintf ( ferr, "\n\n" ) ;
	}
}
/* ----------------------------------------------------------------- */

void	dprschdata ( SCH *Sh, SCH *Sw )
{ 
	int		i, k, d, dmax = 366 ;
	int		*day, Nsc, Nsw ;
	SCH		*Sch ;
	SCH		*Scw ;

	Sch = Sh ;
	Scw = Sw ;
	
	Nsc = Sch->end ;
	Nsw = Scw->end ;

	if ( DEBUG )
	{
		printf ( "\n*** dprschdata  ***\n" ) ;
		printf ( "\n== Sch.end=%d   Scw.end=%d\n", Nsc, Nsw ) ;
		
		for ( i = 0; i < Nsc; i++, Sch++ )
		{
			printf("\nSCH= %s (%2d) ", Sch->name, i);
			
			k = 1 ;
			day = Sch->day ;
			day++ ;
			for ( d = 1; d < dmax; d++, day++ )
			{
				if ( FNNday ( k, 1) == d )
				{
					printf ( "\n%2d - ", k ) ;
					k++ ;
				}
				printf ( "%2d", *day );
			}
		}
		
		for ( i = 0; i < Nsw; i++, Scw++ )
		{
			printf("\nSCW= %s (%2d) ", Scw->name, i) ;
			k = 1 ;
			day = Scw->day ;
			day++ ;
			for ( d = 1; d < dmax; d++, day++ )
			{
				if ( FNNday ( k, 1 ) == d )
				{
					printf ( "\n%2d - ", k ) ;
					k++ ;
				}
				printf ( "%2d", *day ) ;
			}
		}
		printf ( "\n" ) ;
	}

	Sch = Sh ;
	Scw = Sw ;

	if ( ferr )
	{
		fprintf (ferr, "\n*** dprschdata  ***\n" ) ;
		fprintf (ferr, "\n== Sch.end=%d   Scw.end=%d\n", Nsc, Nsw ) ;
		
		for ( i = 0; i < Nsc; i++, Sch++ )
		{
			fprintf(ferr,"\nSCH=%s\t[%d]\t", Sch->name, i);
			
			k = 1 ;
			day = Sch->day ;
			day++ ;
			for ( d = 1; d < dmax; d++, day++ )
			{
				if ( FNNday ( k, 1) == d )
				{
					fprintf (ferr,"\n%2d - ", k ) ;
					k++ ;
				}
				fprintf (ferr, "%2d", *day );
			}
		}
		
		for ( i = 0; i < Nsw; i++, Scw++ )
		{
			fprintf(ferr,"\nSCW= %s (%2d) ", Scw->name, i) ;
			k = 1 ;
			day = Scw->day ;
			day++ ;
			for ( d = 1; d < dmax; d++, day++ )
			{
				if ( FNNday ( k, 1 ) == d )
				{
					fprintf (ferr, "\n%2d - ", k ) ;
					k++ ;
				}
				fprintf (ferr, "%2d", *day ) ;
			}
		}
		fprintf (ferr, "\n" ) ;
	}
}
/* ----------------------------------------------------------------- */

void	dprachv (int Nroom, ROOM *Room)
{
	int		i, j;
	ROOM	*Rm ;
	ACHIR	*A ;
	
	Rm = Room ;

	if ( DEBUG )
	{
		printf("\n*** dprachv***\n");
		
		for ( i = 0; i < Nroom; i++, Rm++ )
		{
			printf("to rm: %-10s   from rms(sch):", Rm->name);
			
			A = Rm->achr ;
			for (j=0; j< Rm->Nachr; j++, A++)
				printf("  %-10s (%3d)", (Room+A->rm)->name, A->sch);
			printf("\n");
		}
	}
	
	Rm = Room ;
	if ( ferr )
	{
		fprintf(ferr,"\n*** dprachv***\n");
		
		for ( i = 0; i < Nroom; i++, Rm++ )
		{
			fprintf(ferr,"to rm: %-10s   from rms(sch):", Rm->name);
			
			A = Rm->achr ;
			for (j=0; j< Rm->Nachr; j++, A++)
				fprintf(ferr,"  %-10s (%3d)", (Room+A->rm)->name, A->sch);
			fprintf(ferr,"\n");
		}
	}
}
/* ----------------------------------------------------------------- */

void	dprexsf ( EXSF *E)   
{
	int		i, N ;
	EXSF	*Exs ;
	
	if ( E != NULL )
	{
		Exs = E ;
		N = Exs->end ;
		
		if ( DEBUG )
		{
			printf("\n*** dprexsf ***\n"); 
			for ( i = 0; i < N; i++, Exs++)
			{
				printf("%2d  %-11s  typ=%c Wa=%6.2lf Wb=%5.2lf Rg=%4.2lf  z=%5.2lf edf=%6.2e\n",
					i, Exs->name, Exs->typ, Exs->Wa, Exs->Wb, Exs->Rg, Exs->Z, Exs->erdff);
			}
		}
		
		Exs = E ;
		if ( ferr )
		{
			fprintf(ferr,"\n*** dprexsf ***\n");
			fprintf(ferr,"\tNo.\tName\ttyp\tWa\tWb\tRg\tz\tedf\n" );
			
			for ( i = 0; i < N; i++, Exs++)
			{
				fprintf(ferr,"\t%d\t%s\t%c\t%.4g\t%.4g\t%.2g\t%.2g\t%.2g\n",
					i, Exs->name, Exs->typ, Exs->Wa, Exs->Wb, Exs->Rg, Exs->Z, Exs->erdff);
			}
		}
	}
}

/* ----------------------------------------------------------------- */

void	dprwwdata(WALL *Wa, WINDOW *Wi)
{
	int		i, j, N ;
	WELM	*w ;
	WALL	*Wall ;
	WINDOW	*Window ;
	
	Wall = Wa ;
	Window = Wi ;

	if ( DEBUG )
	{
		printf ( "\n*** dprwwdata ***\nWALLdata\n" ) ;
		
		N = Wall->end ;
		for ( i = 0; i < N; i++, Wall++ )
		{
			printf("\nWall i=%d %s R=%5.3lf IP=%d Ei=%4.2lf Eo=%4.2lf as=%4.2lf\n", 
				i, Wall->name, Wall->Rwall, Wall->Ip, Wall->Ei, Wall->Eo, Wall->as ) ;
			
			w = Wall->welm ;
			for ( j = 0; j < Wall->N; j++, w++ )
				printf("   %2d  %-10s %5.3lf %2d\n", j, w->code, w->L, w->ND);
		}
		
		printf ( "\nWINDOWdata\n" ) ;
		
		if ( Window != NULL )
		{
			N = Window->end ;
			for ( i = 0; i < N; i++, Window++ )
			{
				printf ( "windows  %s\n", Window->name ) ;
				printf ( " R=%lf t=%lf B=%lf  Ei=%lf Eo=%lf\n", Window->Rwall,
					Window->tgtn, Window->Bn, Window->Ei, Window->Eo);
			}
		}
	}
	
	Wall = Wa ;
	Window = Wi ;
	
	if ( ferr )
	{
		fprintf (ferr, "\n*** dprwwdata ***\nWALLdata\n" ) ;
		
		N = Wall->end ;
		for ( i = 0; i < N; i++, Wall++ )
		{
			fprintf(ferr,"\nWall[%d]\t%s\tR=%.3g\tIP=%d\tEi=%.2g\tEo=%.2g\tas=%.2g\n", 
				i, Wall->name, Wall->Rwall, Wall->Ip, Wall->Ei, Wall->Eo, Wall->as ) ;
			
			fprintf(ferr,"\tNo.\tcode\tL\tND\n" );
			w = Wall->welm ;
			for ( j = 0; j < Wall->N; j++, w++ )
				fprintf(ferr,"\t%d\t%s\t%.3g\t%d\n", j, w->code, w->L, w->ND);
		}
		
		fprintf (ferr, "\nWINDOWdata\n" ) ;
		
		N = Window->end ;
		for ( i = 0; i < N; i++, Window++ )
		{
			fprintf (ferr, "windows[%d]\t%s\n", i, Window->name ) ;
			fprintf (ferr, "\tR=%.3g\tt=%.2g\tB=%.2g\tEi=%.2g\tEo=%.2g\n", Window->Rwall,
				Window->tgtn, Window->Bn, Window->Ei, Window->Eo);
		}
	}
}
/* ----------------------------------------------------------------- */

void	dprroomdata(ROOM *R, RMSRF *S)
{
	int		i, j, N ;
	RMSRF	*Sdd ;
	ROOM	*Room ;
	RMSRF	*Sd ;

	Room = R ;
	Sd = S ;

	if ( DEBUG )
	{
		printf ( "\n*** dprroomdata ***\n" ) ;
		
		N = Room->end ;
		for ( i = 0; i < N; i++, Room++ )
		{  
			printf ( "\n==room=(%d)    %s   N=%d  Ntr=%d Nrp=%d  V=%8.1lf   MRM=%10.4e\n", 
				i, Room->name, Room->N, Room->Ntr, Room->Nrp, Room->VRM, Room->MRM ) ;
			printf ( "   Floor area=%6.2lf   Total surface area=%6.2lf\n",
				Room->FArea, Room->Area ) ;
				/*******************
				printf("   tasc=(%d)  tasclow=(%d)  taschi=(%d)  xasc=(%d)   \n",
				Room->tasc, Room->tasclow, Room->taschi, Room->xasc);
				printf("   tascsw=(%d)  tascswlow=(%d)  tascswhi=(%d)   xascsw=(%d)   \n",
				Room->tascsw, Room->tascswlow, Room->tascswhi, Room->xascsw);
			*************************/
			
			printf ( "   Gve=%lf    Gvi=%lf\n",
				Room->Gve, Room->Gvi ) ;
			printf ( "   Light=%lf  Ltyp=%c  ", Room->Light, Room->Ltyp ) ;
			printf ( "  Nhm=%lf\n",
				Room->Nhm ) ;
			printf ( "  Apsc=%lf  Apsr=%lf   ",
				Room->Apsc, Room->Apsr ) ;
			printf ( "  Apl=%lf \n", Room->Apl ) ;
			
			Sdd = Sd + Room->brs ;
			for ( j = Room->brs; j < Room->brs + Room->N; j++, Sdd++ ) 
			{
				printf ( " %2d  ble=%c typ=%c name=%8s exs=%2d nxrm=%2d nxn=%2d ",
					j, Sdd->ble, Sdd->typ, Sdd->name, Sdd->exs, Sdd->nxrm, Sdd->nxn ) ;
				printf ( "wd=%2d Nfn=%2d A=%5.1lf mwside=%c mwtype=%c Ei=%.2lf Eo=%.2lf\n",
					Sdd->wd, Sdd->Nfn, Sdd->A, Sdd->mwside, Sdd->mwtype,
					Sdd->Ei, Sdd->Eo ) ;
			}
		}
	}

	Room = R ;
	Sd = S ;

	if ( ferr )
	{
		fprintf (ferr, "\n*** dprroomdata ***\n" ) ;
		
		N = Room->end ;
		for ( i = 0; i < N; i++, Room++ )
		{  
			fprintf (ferr, "\n==room=(%d)\t%s\tN=%d\tNtr=%d\tNrp=%d\tV=%.3g\tMRM=%.2g\n", 
				i, Room->name, Room->N, Room->Ntr, Room->Nrp, Room->VRM, Room->MRM ) ;
			fprintf (ferr, "\tFloor_area=%.3g\tTotal_surface_area=%.2g\n",
				Room->FArea, Room->Area ) ;
				/*******************
				printf("   tasc=(%d)  tasclow=(%d)  taschi=(%d)  xasc=(%d)   \n",
				Room->tasc, Room->tasclow, Room->taschi, Room->xasc);
				printf("   tascsw=(%d)  tascswlow=(%d)  tascswhi=(%d)   xascsw=(%d)   \n",
				Room->tascsw, Room->tascswlow, Room->tascswhi, Room->xascsw);
			*************************/
			
			fprintf (ferr, "\tGve=%.2g\tGvi=%.2g\n",
				Room->Gve, Room->Gvi ) ;
			fprintf (ferr, "\tLight=%.2g\tLtyp=%c", Room->Light, Room->Ltyp ) ;
			fprintf (ferr, "\tNhm=%.2g\n",
				Room->Nhm ) ;
			fprintf (ferr, "\tApsc=%.2g\tApsr=%.2g",
				Room->Apsc, Room->Apsr ) ;
			fprintf (ferr, "\tApl=%.2g\n", Room->Apl ) ;

			fprintf (ferr, "\tNo.\tble\ttyp\tname\texs\tnxrmd\tnxn\t" ) ;
			fprintf (ferr, "wd\tNfn\tA\tmwside\tmwtype\tEi\tEo\n" ) ;
			
			Sdd = Sd + Room->brs ;
			for ( j = Room->brs; j < Room->brs + Room->N; j++, Sdd++ ) 
			{
				fprintf (ferr, "\t%d\t%c\t%c\t%s\t%d\t%d\t%d\t",
					j, Sdd->ble, Sdd->typ, Sdd->name, Sdd->exs, Sdd->nxrm, Sdd->nxn ) ;
				fprintf (ferr, "%d\t%d\t%.3g\t%c\t%c\t%.2lf\t%.2lf\n",
					Sdd->wd, Sdd->Nfn, Sdd->A, Sdd->mwside, Sdd->mwtype,
					Sdd->Ei, Sdd->Eo ) ;
			}
		}
	}
}
/* ----------------------------------------------------------------- */

void	dprballoc ( MWALL *M, RMSRF *S )
{
	int		mw, id, N ;
	MWALL	*Mw ;
	RMSRF	*Sd ;
	
	Sd = S ;
	Mw = M ;
	if ( DEBUG )
	{
		printf ( "\n*** dprballoc ***\n" ) ;
		
		N = Mw->end ;
		for ( mw = 0; mw < N; mw++, Mw++ )
		{
			id = ( Sd + Mw->ns )->wd ;
			printf ( " %2d n=%2d  rm=%2d  nxrm=%2d wd=%2d wall=%s M=%2d A=%.2lf\n",
				mw, Mw->ns, Mw->rm, Mw->nxrm, id, Mw->wall->name, Mw->M, Mw->sd->A ) ; 
		}
	}

	Sd = S ;
	Mw = M ;
	if ( ferr )
	{
		fprintf ( ferr, "\n*** dprballoc ***\n" ) ;
		fprintf (ferr, "\tNo.\tn\trm\tnxrm\twd\twall\tM\tA\n" ) ; 

		N = Mw->end ;
		for ( mw = 0; mw < N; mw++, Mw++ )
		{
			id = ( Sd + Mw->ns )->wd ;
			fprintf (ferr, "\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%.2g\n",
				mw, Mw->ns, Mw->rm, Mw->nxrm, id, Mw->wall->name, Mw->M, Mw->sd->A ) ; 
		}
	}
}
