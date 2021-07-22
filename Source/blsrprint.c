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

/*    srprint.c            */

#include <stdio.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"

/*  時間別PMVの出力  */

void	Pmvprint ( FILE *fpout, char *title, ROOM *Room, int Mon, int Day, double time ) 
{
	static int count=0, Nroom, Nr ;
	int		i ;
	ROOM	*Rm ;
	
	if (count == 0 && Room != NULL)
	{
		Nroom = Room->end ;
		Rm = Room ;
		for ( Nr = 0, i = 0; i < Nroom; i++, Rm++ )
		{
			if ( Rm->Metsch != NULL )
				Nr++ ;
		}
		
		fprintf ( fpout, "%s ;\n", title ) ;
		fprintf ( fpout, "%d ", Nr ) ;
		
		Rm = Room ;
		for ( i = 0; i < Nroom; i++, Rm++ )
		{
			if ( Rm->Metsch != NULL )
				fprintf(fpout, "  %s ", Rm->name);
		}
		
		fprintf ( fpout, "\n" ) ;
		
		count = 1;
	}
	/*======================================= */
	fprintf ( fpout, "%02d %02d %5.2lf ", Mon, Day, time ) ;
	
	Rm = Room ;
	for ( i = 0; i < Nroom; i++, Rm++ )
	{
		if ( Rm->Metsch != NULL )
			fprintf ( fpout, " %4.3lf ", Rm->PMV ) ;
	}
	fprintf ( fpout, "\n" ) ;
}
/* ----------------------------------------------------- */

/*   室内温・湿度、室内表面平均温度の出力             
*/

void	Rmevprint (FILE *fpout, char *title, ROOM *Room, int Mon, int Day, double time) 
{
	static int count=0, Nroom;
	int		i;
	ROOM	*Rm ;
	
	if ( Room != NULL )
	{
		if (count == 0)
		{
			Nroom = Room->end;
			
			fprintf ( fpout, "%s ;\n", title ) ;
			fprintf ( fpout, "%d室\t\t\t", Nroom ) ;
			
			Rm = Room ;
			for ( i = 0; i < Nroom; i++, Rm++ )
				fprintf ( fpout, "%s\t\t\t\t", Rm->name ) ;
			fprintf ( fpout, "\n" ) ;
			
			count = 1;
		}
		/*======================================= */
		fprintf ( fpout, "%d\t%d\t%.2lf\t", Mon, Day, time ) ;
		
		Rm = Room ;
		for ( i = 0; i < Nroom; i++, Rm++ )
			fprintf ( fpout, "%.1lf\t%.4lf\t%.1lf\t%.0lf\t", Rm->Tr, Rm->xr, Rm->Tsav, Rm->RH ) ;
		
		fprintf(fpout, "\n");
	}
}
/* ----------------------------------------------------- */




