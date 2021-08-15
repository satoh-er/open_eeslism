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

/*  eqpcat.c  */

#include <string.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnlib.h"


int eqpcat(char *catname, COMPNT *C, EQCAT *Ecat, EQSYS *Esys)
{
	int		i ;
	HCCCA	*Hccca;
	BOICA	*Boica;
	COLLCA	*Collca;
	REFACA	*Refaca;
	PIPECA	*Pipeca;
	STANKCA	*Stankca;
	HEXCA	*Hexca;
	PUMPCA	*Pumpca;
	STHEATCA	*Stheatca ;
	THEXCA	*Thexca ;
	PVCA	*PVca ;
	OMVAVCA	*OMvavca ;
	DESICA	*Desica ;
	EVACCA	*Evcaca ;
	
	/*---- Satoh Debug VAV  2000/12/5 ----*/
	VAVCA	*Vavca ;
	
	//OMVAV	*OMvav ;

	Hccca = Ecat->Hccca;
	Boica = Ecat->Boica;
	Collca = Ecat->Collca;
	Refaca = Ecat->Refaca;
	Pipeca = Ecat->Pipeca;
	Stankca = Ecat->Stankca;
	Hexca = Ecat->Hexca;
	Pumpca = Ecat->Pumpca;
	Stheatca = Ecat->stheatca ;
	Thexca = Ecat->Thexca ;
	PVca = Ecat->PVca ;
	Evcaca = Ecat->Evacca ;

	// Satoh追加　デシカント槽　2013/10/23
	Desica = Ecat->Desica ;
	
	/*---- Satoh Debug VAV  2000/12/5 ----*/
	Vavca = Ecat->Vavca ;
	
	OMvavca = Ecat->OMvavca ;
	
	C->airpathcpy = 'n';
	C->idi = C->ido = NULL;
	
	for (i = 0; i < Ecat->Nhccca; i++, Hccca++)
	{
		if (strcmp(catname, Hccca->name) == 0)
		{
			C->eqptype = stralloc(HCCOIL_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nhcc ;
			Esys->Nhcc++ ;
			C->Nout = C->Nin = 3;
			C->idi = stralloc("txW");
			C->ido = stralloc("txW");
			C->airpathcpy = 'y';
			return(0);
		}
	}
	
	for (i = 0; i < Ecat->Nboica; i++, Boica++)
	{
		if (strcmp(catname, Boica->name) == 0)
		{
			C->eqptype = stralloc(BOILER_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nboi ;
			Esys->Nboi++ ;
			C->Nout = C->Nin = 1;
			return(0);
		}
	}
	
	for (i = 0; i < Ecat->Ncollca; i++, Collca++)
	{
		if (strcmp(catname, Collca->name) == 0)
		{
			C->eqptype = stralloc(COLLECTOR_TYPE);
			C->ncat = i;
			C->neqp = Esys->Ncoll ;
			Esys->Ncoll++ ;
			C->Ac = Collca->Ac ;

			if (Collca->type == COLLECTOR_PDT)
				C->Nout = C->Nin = 1;
			else
			{
				C->Nout = C->Nin = 2;
				C->airpathcpy = 'y' ;
			}
			return(0);
		}
	}

	for (i = 0; i < Ecat->Npvca; i++, PVca++)
	{
		if (strcmp(catname, PVca->name) == 0)
		{
			C->eqptype = stralloc(PV_TYPE);
			C->ncat = i;
			C->neqp = Esys->Npv ;
			Esys->Npv++ ;
			C->PVcap = PVca->PVcap ;
			C->Nout = C->Nin = 0 ;
			C->Area = PVca->Area ;

			return(0);
		}
	}
	
	for (i = 0; i < Ecat->Nrefaca; i++, Refaca++)
	{
		if (strcmp(catname, Refaca->name) == 0)
		{
			C->eqptype = stralloc(REFACOMP_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nrefa ;
			Esys->Nrefa++ ;
			C->Nout = C->Nin = 1;
			return(0);
		}
	}
	
	for (i = 0; i < Ecat->Npipeca; i++, Pipeca++)
	{
		if (strcmp(catname, Pipeca->name) == 0)
		{
			C->eqptype = stralloc(PIPEDUCT_TYPE);
			C->ncat = i;
			C->neqp = Esys->Npipe ;
			Esys->Npipe++ ;
			
			if (Pipeca->type == PIPE_PDT)
				C->Nout = C->Nin = 1;  
			else
			{
				C->Nout = C->Nin = 2;
				C->airpathcpy = 'y';
			}
			
			return(0);
		}
	}
	
	for (i = 0; i < Ecat->Nstankca; i++, Stankca++)
	{
		if (strcmp(catname, Stankca->name) == 0)
		{
			C->eqptype = stralloc(STANK_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nstank ;
			Esys->Nstank++ ;
			
			return(0);
		}
	}
	
	for (i = 0; i < Ecat->Nhexca; i++, Hexca++)
	{
		if (strcmp(catname, Hexca->name) == 0)
		{
			C->eqptype = stralloc(HEXCHANGR_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nhex ;
			Esys->Nhex++ ;

			C->Nout = C->Nin = 2;
			C->idi = stralloc("CH");
			C->ido = stralloc("CH");
			
			return(0);
		}
	}   
	
	for (i = 0; i < Ecat->Npumpca; i++, Pumpca++)
	{
		if (strcmp(catname, Pumpca->name) == 0)
		{
			C->eqptype = stralloc(PUMP_TYPE);
			C->ncat = i;
			C->neqp = Esys->Npump ;
			Esys->Npump++ ;
			
			if ( Pumpca->pftype == PUMP_PF )
				C->Nout = C->Nin = 1;
			else
			{
				C->Nout = C->Nin = 2 ;
				C->airpathcpy = 'y' ;
			}
			
			return(0);
		}
	}
	/*---- Satoh Debug VAV  2000/12/5 ----*/
    for (i = 0; i < Ecat->Nvavca; i++, Vavca++)
	{
		if (strcmp(catname, Vavca->name) == 0)
		{
			if ( Vavca->type == VAV_PDT )
				C->eqptype = stralloc(VAV_TYPE);
			else
				C->eqptype = stralloc(VWV_TYPE) ;

			C->ncat = i;
			C->neqp = Esys->Nvav ;
			Esys->Nvav++ ;

			/*******
			C->Nout = C->Nin = 1;
			***********************/

			if (Vavca->type == VAV_PDT)
			{
				C->Nout = C->Nin = 2 ;
				// 温湿度計算のために出入り口数は2
				C->airpathcpy = 'y' ;
			}
			else
				C->Nout = C->Nin = 1 ;
			
			return(0);
		}
	}

	// Satoh OMVAV  2010/12/16
	for (i = 0; i < Ecat->Nomvavca; i++, OMvavca++)
	{
		if (strcmp(catname, OMvavca->name) == 0)
		{
			C->eqptype = stralloc(OMVAV_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nomvav ;
			Esys->Nomvav++ ;
			C->Nout = C->Nin = 0 ;

			return(0);
		}
	}

	for (i = 0; i < Ecat->Nstheatca; i++, Stheatca++)
	{
		if (strcmp(catname, Stheatca->name) == 0)
		{
			C->eqptype = stralloc(STHEAT_TYPE);
			C->ncat = i;
			C->neqp = Esys->Nstheat ;
			Esys->Nstheat++ ;

			/*******
			C->Nout = C->Nin = 1;
			***********************/

			C->Nout = C->Nin = 3 ;
			// 温湿度計算のために出入り口数は2
			C->airpathcpy = 'y' ;
			
			return(0);
		}
	}

	// Satoh追加　デシカント槽　2013/10/23
	for (i = 0; i < Ecat->Ndesica; i++, Desica++)
	{
		if (strcmp(catname, Desica->name) == 0)
		{
			C->eqptype = stralloc(DESI_TYPE);
			C->ncat = i;
			C->neqp = Esys->Ndesi ;
			Esys->Ndesi++ ;

			/*******
			C->Nout = C->Nin = 1;
			***********************/

			C->Nout = C->Nin = 2 ;
			// 温湿度計算のために出入り口数は2
			C->airpathcpy = 'y' ;
			
			return(0);
		}
	}

	// Satoh追加　気化冷却器　2013/10/27
	for ( i = 0; i < Ecat->Nevacca; i++, Evcaca++ )
	{
		if ( strcmp ( catname, Evcaca->name ) == 0 )
		{
			C->eqptype = stralloc ( EVAC_TYPE ) ;
			C->ncat = i ;
			C->neqp = Esys->Nevac ;
			Esys->Nevac++ ;
			C->airpathcpy = 'y' ;
			C->Nout = C->Nin = 4 ;
			C->idi = stralloc ( "DdWw" ) ;
			C->ido = stralloc ( "DdWw" ) ;

			return ( 0 ) ;
		}
	}

	for ( i = 0; i < Ecat->Nthexca; i++, Thexca++ )
	{
		if ( strcmp ( catname, Thexca->name ) == 0 )
		{
			C->eqptype = stralloc ( THEX_TYPE ) ;
			C->ncat = i ;
			C->neqp = Esys->Nthex ;
			Esys->Nthex++ ;
			C->airpathcpy = 'y' ;
			C->Nout = C->Nin = 4 ;
			C->idi = stralloc ( "EeOo" ) ;
			C->ido = stralloc ( "EeOo" ) ;

			return ( 0 ) ;
		}
	}

    return(1);
}
