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

/*  eesfiles.c  */

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "fesy.h"
#include "fnfio.h"
#include "winerror.h"
#include "winerror.h"
#include "fnlib.h"

/* ----------------------------------------------------- */

void eeflopen(SIMCONTL *Simc, int Nflout, FLOUT *Flout)
{
	FLOUT *fl;
	char  Fname[SCHAR], Err[SCHAR] ; 
	
	sprintf_s(Err, sizeof(Err), ERRFMT, "(eeflopen)");
	
	if (Simc->wdtype == 'H')
	{
		if (fopen_s(&(Simc->fwdata), Simc->wfname, "r") != 0)
		{
			Eprint ( "<eeflopen>", Simc->wfname ) ;
            //getch() ;
			preexit ( ) ;
			exit(EXIT_WFILE);
		}
		if (fopen_s(&(Simc->fwdata2), Simc->wfname, "r") != 0)
		{
			Eprint ( "<eeflopen>", Simc->wfname ) ;
            //getch() ;
			preexit ( ) ;
			exit(EXIT_WFILE);
		}
		if (fopen_s(&(Simc->ftsupw), "supw.efl", "r") != 0)
		{
			Eprint ( "<eeflopen>", "supw.efl" ) ;
            //getch() ;
			preexit ( ) ;
			exit(EXIT_SUPW);
		}
	}
	
	//strcat ( strcpy ( Fname, Simc->ofname ), ".log" ) ;
	//ferr = fopen ( Fname, "w" ) ;

	for (fl = Flout; fl < Flout+Nflout; fl++)
	{
		strcpy_s(Fname, sizeof(Fname), Simc->ofname);
		strcat_s(Fname, sizeof(Fname), fl->idn);
		strcat_s(Fname, sizeof(Fname), ".es");
		fopen_s(&(fl->f), Fname, "w");
	}
}
/* ----------------------------------------------------- */


void eeflclose(int Nflout, FLOUT *Flout)
{
	FLOUT *fl;
	
	//   fclose(Simc->fwdata);
	//   fclose(Simc->ftsupw);
	
	if ( ferr )
		fclose ( ferr ) ;
	for (fl = Flout; fl < Flout+Nflout; fl++)
	{
		fprintf(fl->f, "-999\n");
		fflush(fl->f);
		fclose(fl->f);
	}
	  	//_fcloseall() ;  
		//higuchi
}
