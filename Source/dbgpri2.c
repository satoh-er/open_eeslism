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

/*  dbgpri2.c   */

#include <stdio.h>
#include "MODEL.h" /*---------higuchi 070918------------*/
#include "fesy.h"
#include "fnbld.h"

/* ----------------------------------------- */

void	xprroom ( int Nroom, ROOM *R )

{ 
	int		i, j ;
	double	*ARN, *RMP ;
	extern int	DEBUG ;
	extern FILE	*ferr ;
	ROOM	*Room ;
	
	Room = R ;
	if ( DEBUG )
	{
		printf ( "--- xprroom\n" ) ;
		for ( i = 0; i < Nroom; i++, Room++ )
		{ 
			printf (" Room:  name=%s  MRM=%lf  GRM=%lf\n", 
				Room->name, Room->MRM, Room->GRM);
			
			printf ("     RMt=%lf", Room->RMt);
			
			ARN = Room->ARN;
			for (j = 0; j < Room->Ntr; j++, ARN++ )
				printf ( " ARN=%lf", *ARN ) ;
			
			RMP = Room->RMP;
			for ( j = 0; j < Room->Nrp; j++, RMP++ )
				printf ( " RMP=%lf", *RMP ) ;
			
			printf ( " RMC=%lf\n", Room->RMC ) ;
			
			printf ( "     RMx=%lf          RMXC=%lf\n",
				Room->RMx, Room->RMXC ) ;
		}
	}

	Room = R ;
	if ( ferr )
	{
		fprintf (ferr, "--- xprroom\n" ) ;
		for ( i = 0; i < Nroom; i++, Room++ )
		{ 
			fprintf (ferr,"Room:\tname=%s\tMRM=%.4g\tGRM=%.4g\n", 
				Room->name, Room->MRM, Room->GRM);
			
			fprintf (ferr,"\tRMt=%.4g\n", Room->RMt);
			
			ARN = Room->ARN;
			for (j = 0; j < Room->Ntr; j++, ARN++ )
				fprintf (ferr, "\tARN[%d]=%.4g", j, *ARN ) ;
			fprintf (ferr, "\n" ) ;

			RMP = Room->RMP;
			for ( j = 0; j < Room->Nrp; j++, RMP++ )
				fprintf (ferr, "\tRMP[%d]=%.4g", j, *RMP ) ;
			fprintf (ferr, "\n" ) ;

			fprintf (ferr, "\tRMC=%.4g\n", Room->RMC ) ;
			
			fprintf (ferr, "\tRMx=%.2g\t\tRMXC=%.2g\n",
				Room->RMx, Room->RMXC ) ;
		}
	}
}
/* ----------------------------------------- */

void	xprschval (int Nsch, double *val, int Nscw, char *isw)

{
	int		j ;
	
	printf ( "--- xprschval\n" ) ;

	for ( j = 0; j < Nsch; j++, val++ )
		printf ( "--- val=(%d)  %lf\n", j, *val ) ;
	
	for ( j = 0; j < Nscw; j++, isw++ )
		printf ( "--- isw=(%d) %c\n", j, *isw ) ;
}
/* --------------------------------------------- */

void	xprqin (int Nroom, ROOM *Room)

{
	int		i;
	
	printf ( "--- xprqin  Nroom=%d\n", Nroom ) ;

	for ( i = 0; i < Nroom; i++, Room++ )
		printf ( "  [%d] Hc=%lf Hr=%lf HL=%lf Lc=%lf Lr=%lf Ac=%lf Ar=%lf AL=%lf\n",
		i, Room->Hc, Room->Hr, Room->HL, Room->Lc, Room->Lr, 
		Room->Ac, Room->Ar, Room->AL);
}
/* --------------------------------------------- */

void	xprvent ( int Nroom, ROOM *R )
{
	int		i, j ;
	ACHIR	*A ;
	extern int	DEBUG ;
	extern FILE	*ferr ;
	ROOM	*Room ;
	
	Room = R ;
	if ( DEBUG )
	{
		printf ( "--- xprvent\n" ) ;
		
		for ( i = 0; i < Nroom; i++, Room++ )
		{ 
			printf ( "  [%d] %-10s  Gvent=%lf  -- Gvr:", i, Room->name, Room->Gvent ) ;
			
			A = Room->achr ;
			for ( j = 0; j < Room->Nachr; j++, A++ )
				printf ( " <%d>=%lf", A->rm, A->Gvr ) ;
			printf ( "\n" ) ;
		}
	}

	Room = R ;
	if ( ferr )
	{
		fprintf ( ferr, "\n\n--- xprvent\n" ) ;
		
		for ( i = 0; i < Nroom; i++, Room++ )
		{ 
			fprintf ( ferr, "\t[%d]\t%s\tGvent=%.3g\n\t\t", i, Room->name, Room->Gvent ) ;
			
			A = Room->achr ;
			for ( j = 0; j < Room->Nachr; j++, A++ )
				fprintf ( ferr, "\t<%d>=%.2g", A->rm, A->Gvr ) ;
			fprintf ( ferr, "\n" ) ;
		}
	}
}
