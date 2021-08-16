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
#include "MODEL.h"     /*--higuchi 070918 -*/
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnesy.h"
#include "fnbld.h"


/*  システム使用機器についての出力  */

void Hcmpprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time, EQSYS *Eqsys,
			   int Nrdpnl, RDPNL *Rdpnl)
{
	int j;
	static int  id = 0;
	
	if (id == 0)
	{
		/****printf("<<Hcmpprint>> wfname=%s\n", Simc->wfname);****/
		
		ttlprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if (j == 0)
				fprintf(fo, "-cat\n");
			
			boiprint(fo, id, Eqsys->Nboi, Eqsys->Boi);
			refaprint(fo, id, Eqsys->Nrefa, Eqsys->Refa);
			collprint(fo, id, Eqsys->Ncoll, Eqsys->Coll); 
			hccprint(fo, id, Eqsys->Nhcc, Eqsys->Hcc);
			pipeprint(fo, id, Eqsys->Npipe, Eqsys->Pipe);
			hexprint(fo, id, Eqsys->Nhex, Eqsys->Hex); 	 
			stankcmpprt(fo, id, Eqsys->Nstank, Eqsys->Stank);
			pumpprint(fo, id, Eqsys->Npump, Eqsys->Pump);
			hcldprint(fo, id, Eqsys->Nhcload, Eqsys->Hcload);
			vavprint(fo, id, Eqsys->Nvav, Eqsys->vav) ;
			stheatprint(fo, id, Eqsys->Nstheat, Eqsys->stheat) ;
			Thexprint(fo, id, Eqsys->Nthex, Eqsys->Thex ) ;
			Qmeasprint(fo, id, Eqsys->Nqmeas, Eqsys->Qmeas ) ;
			PVprint(fo, id, Eqsys->Npv, Eqsys->PVcmp ) ;
			Desiprint(fo, id, Eqsys->Ndesi, Eqsys->Desi) ;
			Evacprint(fo, id, Eqsys->Nevac, Eqsys->Evac) ;
			
#if SIMUL_BUILDG
			panelprint(fo, id, Nrdpnl, Rdpnl);  
#endif	 
			if (j == 0)
				fprintf(fo ,"*\n#\n");
			
			id++;  
		}
	} 
	
	fprintf(fo, "%02d %02d %5.2lf\n", mon, day, time);
	boiprint(fo, id, Eqsys->Nboi, Eqsys->Boi);
	refaprint(fo, id, Eqsys->Nrefa, Eqsys->Refa);
	collprint(fo, id, Eqsys->Ncoll, Eqsys->Coll); 
	hccprint(fo, id, Eqsys->Nhcc, Eqsys->Hcc);   
	pipeprint(fo, id, Eqsys->Npipe, Eqsys->Pipe);
	hexprint(fo, id, Eqsys->Nhex, Eqsys->Hex);       
	stankcmpprt(fo, id, Eqsys->Nstank, Eqsys->Stank); 
	pumpprint(fo, id, Eqsys->Npump, Eqsys->Pump);
	hcldprint(fo, id, Eqsys->Nhcload, Eqsys->Hcload);
	vavprint(fo, id, Eqsys->Nvav, Eqsys->vav) ;
	stheatprint(fo, id, Eqsys->Nstheat, Eqsys->stheat) ;
	Thexprint(fo, id, Eqsys->Nthex, Eqsys->Thex ) ;
	Qmeasprint(fo, id, Eqsys->Nqmeas, Eqsys->Qmeas ) ;
	PVprint(fo, id, Eqsys->Npv, Eqsys->PVcmp ) ;
	Desiprint(fo, id, Eqsys->Ndesi, Eqsys->Desi) ;
	Evacprint(fo, id, Eqsys->Nevac, Eqsys->Evac) ;
	
#if SIMUL_BUILDG
	panelprint(fo, id, Nrdpnl, Rdpnl); 
#endif	 
	
}

/* --------------------------- */

void Hstkprint(FILE *fo, char *title, int mon, int day, double time, EQSYS *Eqsys)
{
	static int  id = 0;
	
	if (id == 0)
	{
		fprintf(fo, "%s ;\n", title); 
		stankivprt(fo, id, Eqsys->Nstank, Eqsys->Stank);
		
		id++;
	}
	if (Eqsys->Nstank)
	{
		fprintf(fo, "%02d %02d %5.2lf  ", mon, day, time);
		stankivprt(fo, id, Eqsys->Nstank, Eqsys->Stank);
	}
	fprintf(fo, " ;\n");
}



