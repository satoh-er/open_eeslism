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

/* ee_cmpday_s.c */

#include <stdio.h>
#include "MODEL.h"     /*--higuchi 070918  -*/
#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnbld.h"


/* システム要素機器の日集計処理 */

void Compoday(int Mon, int Day, int Nday, int ttmm, EQSYS *Eqsys, int SimDayend)
{
	static int oldday = 0;
	static int oldMon = 0 ;
	
	// 日集計
	if (Nday != oldday)
	{
		boidyint(Eqsys->Nboi, Eqsys->Boi);
		refadyint(Eqsys->Nrefa, Eqsys->Refa);
		colldyint(Eqsys->Ncoll, Eqsys->Coll);
		hccdyint(Eqsys->Nhcc, Eqsys->Hcc);
		pipedyint(Eqsys->Npipe, Eqsys->Pipe);
		hexdyint(Eqsys->Nhex, Eqsys->Hex);
		stankdyint(Eqsys->Nstank, Eqsys->Stank);
		pumpdyint(Eqsys->Npump, Eqsys->Pump);
		hclddyint(Eqsys->Nhcload, Eqsys->Hcload);
		stheatdyint(Eqsys->Nstheat, Eqsys->stheat) ;
		Thexdyint(Eqsys->Nthex, Eqsys->Thex) ;
		Qmeasdyint(Eqsys->Nqmeas, Eqsys->Qmeas ) ;
		PVdyint(Eqsys->Npv, Eqsys->PVcmp ) ;
		Desidyint(Eqsys->Ndesi, Eqsys->Desi) ;
		
		oldday = Nday;
	}

	if (Mon != oldMon)
	{
		boimonint(Eqsys->Nboi, Eqsys->Boi);
		refamonint(Eqsys->Nrefa, Eqsys->Refa);
		collmonint(Eqsys->Ncoll, Eqsys->Coll);
		hccmonint(Eqsys->Nhcc, Eqsys->Hcc);
		pipemonint(Eqsys->Npipe, Eqsys->Pipe);
		hexmonint(Eqsys->Nhex, Eqsys->Hex);
		stankmonint(Eqsys->Nstank, Eqsys->Stank);
		pumpmonint(Eqsys->Npump, Eqsys->Pump);
		hcldmonint(Eqsys->Nhcload, Eqsys->Hcload);
		stheatmonint(Eqsys->Nstheat, Eqsys->stheat) ;
		Thexmonint(Eqsys->Nthex, Eqsys->Thex) ;
		Qmeasmonint(Eqsys->Nqmeas, Eqsys->Qmeas ) ;
		PVmonint(Eqsys->Npv, Eqsys->PVcmp ) ;
		
		oldMon = Mon;
	}
	
	// 日集計
	boiday(Mon, Day, ttmm, Eqsys->Nboi, Eqsys->Boi, Nday, SimDayend);
	refaday(Mon, Day, ttmm, Eqsys->Nrefa, Eqsys->Refa, Nday, SimDayend);
	collday(Mon, Day, ttmm, Eqsys->Ncoll, Eqsys->Coll, Nday, SimDayend);
	hccday(Mon, Day, ttmm, Eqsys->Nhcc, Eqsys->Hcc, Nday, SimDayend);
	pipeday(Mon, Day, ttmm, Eqsys->Npipe, Eqsys->Pipe, Nday, SimDayend);
	hexday(Mon, Day, ttmm, Eqsys->Nhex, Eqsys->Hex, Nday, SimDayend);
	stankday(Mon, Day, ttmm, Eqsys->Nstank, Eqsys->Stank, Nday, SimDayend);
	pumpday(Mon, Day, ttmm, Eqsys->Npump, Eqsys->Pump, Nday, SimDayend);
	hcldday(Mon, Day, ttmm, Eqsys->Nhcload, Eqsys->Hcload, Nday, SimDayend);
	stheatday(Mon, Day, ttmm, Eqsys->Nstheat, Eqsys->stheat, Nday, SimDayend) ;
	Thexday(Mon, Day, ttmm, Eqsys->Nthex, Eqsys->Thex, Nday, SimDayend) ;
	Qmeasday(Mon, Day, ttmm, Eqsys->Nqmeas, Eqsys->Qmeas, Nday, SimDayend) ;
	PVday(Mon, Day, ttmm, Eqsys->Npv, Eqsys->PVcmp, Nday, SimDayend ) ;
	Desiday(Mon, Day, ttmm, Eqsys->Ndesi, Eqsys->Desi, Nday, SimDayend) ;
	
	// 月集計
	//boimon(Mon, Day, ttmm, Eqsys->Nboi, Eqsys->Boi);
	//refamon(Mon, Day, ttmm, Eqsys->Nrefa, Eqsys->Refa);
	//collmon(Mon, Day, ttmm, Eqsys->Ncoll, Eqsys->Coll);
	//hccmon(Mon, Day, ttmm, Eqsys->Nhcc, Eqsys->Hcc);
	//pipemon(Mon, Day, ttmm, Eqsys->Npipe, Eqsys->Pipe);
	//hexmon(Mon, Day, ttmm, Eqsys->Nhex, Eqsys->Hex);
	//stankmon(Mon, Day, ttmm, Eqsys->Nstank, Eqsys->Stank);
	//pumpmon(Mon, Day, ttmm, Eqsys->Npump, Eqsys->Pump);
	//hcldmon(Mon, Day, ttmm, Eqsys->Nhcload, Eqsys->Hcload);
	//stheatmon(Mon, Day, ttmm, Eqsys->Nstheat, Eqsys->stheat) ;
	//Thexmon(Mon, Day, ttmm, Eqsys->Nthex, Eqsys->Thex) ;
	//Qmeasmon(Mon, Day, ttmm, Eqsys->Nqmeas, Eqsys->Qmeas) ;
	//PVmon(Mon, Day, ttmm, Eqsys->Npv, Eqsys->PVcmp ) ;
}
/* ---------------------------------- */

/* システム要素機器の日集計結果出力 */

void Compodyprt(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, 
				EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl)
{
	int j;
	static int id = 0;
	
	if (id == 0)
	{
		ttldyprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if (j == 0)
				fprintf(fo ,"-cat\n");

			boidyprt(fo, id, Eqsys->Nboi, Eqsys->Boi);
			refadyprt(fo, id, Eqsys->Nrefa, Eqsys->Refa);
			colldyprt(fo, id, Eqsys->Ncoll, Eqsys->Coll);
			hccdyprt(fo, id, Eqsys->Nhcc, Eqsys->Hcc);
			pipedyprt(fo, id, Eqsys->Npipe, Eqsys->Pipe);
			hexdyprt(fo, id, Eqsys->Nhex, Eqsys->Hex);
			stankdyprt(fo, id, Eqsys->Nstank, Eqsys->Stank); 
			pumpdyprt(fo, id, Eqsys->Npump, Eqsys->Pump);
			hclddyprt(fo, id, Eqsys->Nhcload, Eqsys->Hcload);
			stheatdyprt(fo, id, Eqsys->Nstheat, Eqsys->stheat) ;
			Qmeasdyprt(fo, id, Eqsys->Nqmeas, Eqsys->Qmeas) ;
			Thexdyprt(fo, id, Eqsys->Nthex, Eqsys->Thex) ;
			PVdyprt(fo, id, Eqsys->Npv, Eqsys->PVcmp) ;
			Desidyprt(fo, id, Eqsys->Ndesi, Eqsys->Desi) ;
			
			paneldyprt(fo, id, Nrdpnl, Rdpnl);
			
			if (j == 0)
				fprintf(fo ,"*\n#\n");
			
			id++;
		}
	}
	
	fprintf(fo, "%02d %02d\n", mon, day);
	
	boidyprt(fo, id, Eqsys->Nboi, Eqsys->Boi);
	refadyprt(fo, id, Eqsys->Nrefa, Eqsys->Refa);
	colldyprt(fo, id, Eqsys->Ncoll, Eqsys->Coll);
	hccdyprt(fo, id, Eqsys->Nhcc, Eqsys->Hcc);
	pipedyprt(fo, id, Eqsys->Npipe, Eqsys->Pipe);
	hexdyprt(fo, id, Eqsys->Nhex, Eqsys->Hex);
	stankdyprt(fo, id, Eqsys->Nstank, Eqsys->Stank);
	pumpdyprt(fo, id, Eqsys->Npump, Eqsys->Pump);
	hclddyprt(fo, id, Eqsys->Nhcload, Eqsys->Hcload);
	stheatdyprt(fo, id, Eqsys->Nstheat, Eqsys->stheat) ;
	Qmeasdyprt(fo, id, Eqsys->Nqmeas, Eqsys->Qmeas) ;
	Thexdyprt(fo, id, Eqsys->Nthex, Eqsys->Thex) ;
	PVdyprt(fo, id, Eqsys->Npv, Eqsys->PVcmp) ;
	Desidyprt(fo, id, Eqsys->Ndesi, Eqsys->Desi) ;
	
	paneldyprt(fo, id, Nrdpnl, Rdpnl);
	
}

void Compomonprt(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, 
				EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl)
{
	int j;
	static int id = 0;
	
	if (id == 0)
	{
		ttldyprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if (j == 0)
				fprintf(fo ,"-cat\n");

			boimonprt(fo, id, Eqsys->Nboi, Eqsys->Boi);
			refamonprt(fo, id, Eqsys->Nrefa, Eqsys->Refa);
			collmonprt(fo, id, Eqsys->Ncoll, Eqsys->Coll);
			hccmonprt(fo, id, Eqsys->Nhcc, Eqsys->Hcc);
			pipemonprt(fo, id, Eqsys->Npipe, Eqsys->Pipe);
			hexmonprt(fo, id, Eqsys->Nhex, Eqsys->Hex);
			stankmonprt(fo, id, Eqsys->Nstank, Eqsys->Stank); 
			pumpmonprt(fo, id, Eqsys->Npump, Eqsys->Pump);
			hcldmonprt(fo, id, Eqsys->Nhcload, Eqsys->Hcload);
			stheatmonprt(fo, id, Eqsys->Nstheat, Eqsys->stheat) ;
			Qmeasmonprt(fo, id, Eqsys->Nqmeas, Eqsys->Qmeas) ;
			Thexmonprt(fo, id, Eqsys->Nthex, Eqsys->Thex) ;
			PVmonprt(fo, id, Eqsys->Npv, Eqsys->PVcmp) ;
			
			panelmonprt(fo, id, Nrdpnl, Rdpnl);
			
			if (j == 0)
				fprintf(fo ,"*\n#\n");
			
			id++;
		}
	}
	
	fprintf(fo, "%02d %02d\n", mon, day);
	
	boimonprt(fo, id, Eqsys->Nboi, Eqsys->Boi);
	refamonprt(fo, id, Eqsys->Nrefa, Eqsys->Refa);
	collmonprt(fo, id, Eqsys->Ncoll, Eqsys->Coll);
	hccmonprt(fo, id, Eqsys->Nhcc, Eqsys->Hcc);
	pipemonprt(fo, id, Eqsys->Npipe, Eqsys->Pipe);
	hexmonprt(fo, id, Eqsys->Nhex, Eqsys->Hex);
	stankmonprt(fo, id, Eqsys->Nstank, Eqsys->Stank);
	pumpmonprt(fo, id, Eqsys->Npump, Eqsys->Pump);
	hcldmonprt(fo, id, Eqsys->Nhcload, Eqsys->Hcload);
	stheatmonprt(fo, id, Eqsys->Nstheat, Eqsys->stheat) ;
	Qmeasmonprt(fo, id, Eqsys->Nqmeas, Eqsys->Qmeas) ;
	Thexmonprt(fo, id, Eqsys->Nthex, Eqsys->Thex) ;
	PVmonprt(fo, id, Eqsys->Npv, Eqsys->PVcmp) ;
	
	panelmonprt(fo, id, Nrdpnl, Rdpnl);
	
}

void Compomtprt(FILE *fo, char *mrk, SIMCONTL *Simc, EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl)
{
	int j;
	static int id = 0;
	int	mo, tt ;
	
	if (id == 0)
	{
		ttlmtprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if (j == 0)
				fprintf(fo ,"-cat\n");

			stheatmtprt(fo, id, Eqsys->Nstheat, Eqsys->stheat, 0, 0) ;
			boimtprt(fo, id, Eqsys->Nboi, Eqsys->Boi, 0, 0) ;
			refamtprt(fo, id, Eqsys->Nrefa, Eqsys->Refa, 0, 0) ;
			pumpmtprt(fo, id, Eqsys->Npump, Eqsys->Pump, 0, 0) ;
			PVmtprt(fo, id, Eqsys->Npv, Eqsys->PVcmp, 0, 0) ;
			hcldmtprt(fo, id, Eqsys->Nhcload, Eqsys->Hcload, 0, 0) ;
			panelmtprt(fo, id, Nrdpnl, Rdpnl, 0, 0) ;
			
			if (j == 0)
				fprintf(fo ,"*\n#\n");
			
			id++;
		}
	}

	for ( mo = 1; mo <= 12; mo++ )
	{
		for ( tt = 1; tt <= 24; tt++ )
		{
			fprintf(fo, "%02d %02d\n", mo, tt);
			stheatmtprt(fo, id, Eqsys->Nstheat, Eqsys->stheat, mo, tt) ;
			boimtprt(fo, id, Eqsys->Nboi, Eqsys->Boi, mo, tt) ;
			refamtprt(fo, id, Eqsys->Nrefa, Eqsys->Refa, mo, tt) ;
			pumpmtprt(fo, id, Eqsys->Npump, Eqsys->Pump, mo, tt) ;
			PVmtprt(fo, id, Eqsys->Npv, Eqsys->PVcmp, mo, tt) ;
			hcldmtprt(fo, id, Eqsys->Nhcload, Eqsys->Hcload, mo, tt) ;
			panelmtprt(fo, id, Nrdpnl, Rdpnl, mo, tt) ;
		}
	}
}
