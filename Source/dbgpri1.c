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
#include "MODEL.h" /*---------higuchi 070918------------*/
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"

/* -------------------------------------------- */

void	xprtwallinit ( int Nmwall, MWALL *M )
{
	int		j, m ;
	double	*Told ;
	MWALL	*Mw ;
	int		Max ;

	Max = 0 ;
	Mw = M ;
	for ( j = 0; j < Nmwall; j++, Mw++ )
		Max = imax ( Mw->M, Max ) ;
	
	Mw = M ;
	if ( DEBUG )
	{
		printf ( "--- xprtwallinit\n" ) ;
		
		for ( j = 0; j < Nmwall; j++, Mw++ )
		{
			printf ( "Told  j=%2d", j ) ;
			
			Told = Mw->Told ;
			for ( m = 0; m < Mw->M; m++, Told++ )  
				printf ( "  %2d%5.1lf", m, *Told ) ;
			
			printf ( "\n" ) ;
		}
	}
	
	Mw = M ;
	if ( ferr )
	{
		fprintf (ferr, "--- xprtwallinit\n" ) ;
		fprintf (ferr, "\tNo." ) ;

		for ( j = 0; j < Max; j++ )
			fprintf (ferr, "\tT[%d]", j ) ;
		fprintf (ferr, "\n" ) ;
		
		for ( j = 0; j < Nmwall; j++, Mw++ )
		{
			fprintf (ferr, "\t%d", j ) ;
			
			Told = Mw->Told ;
			for ( m = 0; m < Mw->M; m++, Told++ )  
				fprintf (ferr, "\t%.3g", *Told ) ;
			
			fprintf (ferr, "\n" ) ;
		}
	}
}

/* -------------------------------------------- */

void	xprsolrd ( int Nexs, EXSF *E )
{
	int		i ;
	EXSF	*Exs ;
	
	Exs = E ;
	if ( DEBUG )
	{
		printf ( "--- xprsolrd\n" ) ;
		
		for ( i = 0; i < Nexs; i++, Exs++ )
		{
			printf ( "EXSF[%2d]=%s  Id=%5.0lf  Idif=%5.0lf  Iw=%5.0lf RN=%5.0lf cinc=%5.3lf\n",
				i, Exs->name, Exs->Idre, Exs->Idf, Exs->Iw, Exs->rn, Exs->cinc ) ;
		}
	}

	Exs = E ;
	if ( ferr )
	{
		fprintf (ferr, "--- xprsolrd\n" ) ;
		fprintf (ferr, "\tNo.\tName\tId\tIdif\tIw\tRN\tcinc\n" ) ;

		for ( i = 0; i < Nexs; i++, Exs++ )
		{
			fprintf (ferr, "\t%d\t%s\t%.0lf\t%.0lf\t%.0lf\t%.0lf\t%.3lf\n",
				i, Exs->name, Exs->Idre, Exs->Idf, Exs->Iw, Exs->rn, Exs->cinc ) ;
		}
	}
}

/* ---------------------------------------------------------- */

void	xpralph ( int Nroom, ROOM *Room, RMSRF *S )
{
	int		i, n, N, brs ;
	RMSRF	*Sd ;
	
	printf ( "--- xpralph\n" ) ;
	
	for ( i = 0; i < Nroom; i++, Room++ )
	{ 
		N = Room->N ;
		brs = Room->brs ;
		
		printf ( " alr(i,j)\n" ) ;
		matfprint ( "  %5.1lf", N, Room->alr ) ;
		printf ( " alph\n" ) ;
		
		Sd = S + brs ;
		for ( n = brs; n < brs + N; n++, Sd++ )
		{
			printf("  %3d  alo=%5.1lf  alir=%5.1lf alic=%5.1lf  ali=%5.1lf\n",
				n, Sd->alo, Sd->alir, Sd->alic, Sd->ali ) ;
		}
	}
}
/* ---------------------------------------------------------- */

void	xprxas ( int Nroom, ROOM *R, RMSRF *S )

{  
	int		i, n, N, brs, j;
	double  *WSRN, *WSPL;
	RMSRF	*Sd ;
	ROOM	*Room ;
	
	Room = R ;
	if ( DEBUG )
	{
		printf ( "--- xprxas\n" ) ;
		
		for ( i = 0; i < Nroom; i++, Room++ )
		{
			N = Room->N ;
			brs = Room->brs ;
			
			printf ( " XA(i,j)\n" ) ;
			matprint ( "%7.4lf", N, Room->XA ) ;
			
			Sd = S + brs ;
			for ( n = brs; n < brs + N; n++, Sd++ )
			{
				printf("%2d  K=%lf  alo=%lf  FI=%lf FO=%lf FP=%lf  CF=%lf\n",
					n, Sd->K, Sd->alo, Sd->FI, Sd->FO, Sd->FP, Sd->CF);
				printf("            WSR=%lf", Sd->WSR ) ;
				
				WSRN = Sd->WSRN;
				for ( j = 0; j < Room->Ntr; j++, WSRN++ )
					printf ( " WSRN=%lf", *WSRN ) ;
				
				WSPL = Sd->WSPL ;
				for (j = 0; j < Room->Nrp; j++, WSPL++ )
					printf ( " WSPL=%lf", *WSPL ) ;
				
				printf ( "   WSC=%lf\n", Sd->WSC ) ; 
			}
		}
	}
	
	Room = R ;
	if ( ferr )
	{
		fprintf (ferr, "--- xprxas\n" ) ;
		
		for ( i = 0; i < Nroom; i++, Room++ )
		{
			N = Room->N ;
			brs = Room->brs ;
			
			fprintf (ferr, "Room=%s\tXA(i,j)\n", Room->name ) ;
			matfiprint ( ferr, "\t%.1g", N, Room->XA ) ;
			
			Sd = S + brs ;

			for ( n = brs; n < brs + N; n++, Sd++ )
			{
				fprintf(ferr,"\n\n\t%d\tK=%.2g\talo=%.2g\tFI=%.2g\tFO=%.2g\tFP=%.2g\tCF=%.2g\t",
					n, Sd->K, Sd->alo, Sd->FI, Sd->FO, Sd->FP, Sd->CF);
				fprintf(ferr,"\t\tWSR=%.3g\n\t", Sd->WSR ) ;
				
				WSRN = Sd->WSRN;
				for ( j = 0; j < Room->Ntr; j++, WSRN++ )
					fprintf (ferr, "\tWSRN[%d]=%.3g", j, *WSRN ) ;
				fprintf (ferr, "\n\t" ) ;

				WSPL = Sd->WSPL ;
				for (j = 0; j < Room->Nrp; j++, WSPL++ )
					fprintf (ferr, "\tWSPL[%d]=%.3g", j, *WSPL ) ;
				fprintf (ferr, "\n" ) ;
				
				fprintf (ferr, "\t\tWSC=%.3g\n", Sd->WSC ) ; 
			}
		}
	}
}

void	xprtwsrf ( int N, RMSRF *Sd )
{
	int		n ;
	
	printf ( "--- xprtwsrf\n" ) ;
	
	for ( n = 0; n < N; n++, Sd++ )
		printf ( "  n=%2d  rm=%d nr=%d  Ts=%6.2lf  Tmrt=%6.2lf  Te=%6.2lf  RS=%7.1lf\n",
		n, Sd->rm, Sd->n, Sd->Ts, Sd->Tmrt, Sd->Te, Sd->RS ) ;
}
/* -------------------------------------------------------------------- */

void	xprrmsrf ( int N, RMSRF *Sd )

{
	int		n ;
	
	printf ( "--- xprrmsf\n" ) ;
	
	for ( n = 0; n < N; n++, Sd++ )
		printf ( "  [%d]=%6.2lf", n, Sd->Ts ) ;
	printf ( "\n" ) ;
}
/* -------------------------------------------------------------------- */

void	xprtwall ( int Nmwall, MWALL *Mw )

{
	int		j, m ;
	double	*Tw ;
	
	printf ( "--- xprtwall\n" ) ;
	
	for ( j = 0; j < Nmwall; j++, Mw++ )
	{
		if ( Mw->Pc > 0 ) 
		{
			printf ( "Tw j=%2d", j ) ;
			
			Tw = Mw->Tw ;
			for ( m = 0; m < Mw->M; m++, Tw++ )
				printf ( "  [%d]=%6.2lf", m, *Tw ) ; 
			printf ( "\n" ) ; 
		}
	}
}
