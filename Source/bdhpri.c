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

/****************

 熱損失係数出力ルーチン
 1997.11.18
	FILE=bdhpri.c
	
****************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnesy.h"
#include "fnlib.h"


void	bdhpri ( char *ofile, RMVLS rmvls, EXSFS *exs )
{
	int		i, j ;
	FILE	*fp ;
	char	file[SCHAR] ;
	ROOM	*room ;
	int		Nroom ;
	char	E[SCHAR] ;
	RMSRF	*r ;
	EXSF	*e, *er ;
	
	Nroom = rmvls.Nroom ;
	e = exs->Exs ;
	
	if ( Nroom > 0 )
		room = rmvls.Room ;
	else
		room = NULL ;
	
	strcpy ( file, ofile ) ;
	strcat ( file, "_bdh.es" ) ;
	fp = fopen ( file, "w" ) ;
	
	fprintf ( fp, "%d\n", Nroom ) ;
	
	for ( i = 0; i < Nroom; i++, room++ )
	{
		fprintf ( fp, "%s\t", room->name ) ;
		fprintf ( fp, "%d\t", room->N ) ;
		fprintf ( fp, "%.3lf\t", room->FArea ) ;
		fprintf ( fp, "%.3lf\t#\n", room->VRM ) ;
		
		r = room->rsrf ;
		for ( j = 0; j < room->N; j++, r++ )
		{
			if ( r->ble == 'E' || r->ble == 'W' || r->ble == 'R' || r->ble == 'F' )
			{
				er = e + r->exs ;
				fprintf ( fp, "\t%s", er->name ) ;
			}
			else if ( r->nextroom != NULL && r->nextroom->name != NULL )
				fprintf ( fp, "\t%s", r->nextroom->name ) ;
			else
				fprintf ( fp, "\t-" ) ;

			if ( r->name != NULL
				&& strlen ( r->name ) > 0 )
				fprintf ( fp, "\t%s\t", r->name ) ;
			else
				fprintf ( fp, "\t-\t" ) ;
			
			fprintf ( fp, "%c\t", r->ble ) ;
			fprintf ( fp, "%.3lf\t", r->A ) ;
			
			if ( r->A < 0.0 )
			{
				sprintf ( E, "RmName=%s Ble=%c A=%.3lf\n", room->name, r->ble, r->A ) ;
				Errprint ( 1, "<bdhpri>", E ) ;
			}
			
			if ( r->Rwall >= 0.0 )
				fprintf ( fp, "%.3lf\t%.2lf\t;\n", r->Rwall, r->CAPwall ) ;
			else
			{
				sprintf ( E, "Rmname=%s Ble=%c  Not Defined", room->name, r->ble ) ;
				//Errprint ( 1, "<bdhpri>", E ) ;
				fprintf(fp, "\t\t;\n");
			}
		}
		fprintf ( fp, "*\n" ) ;
	}
	fclose ( fp ) ;
//	free ( file ) ;
}
