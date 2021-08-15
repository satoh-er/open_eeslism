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

/*   ee_print_s.c  */

#include "common.h"
#include "fesy.h"
#include "fnbld.h"
#include "fnfio.h"
#include "fnlib.h"

/* ---------------------------------------------------------------- */ 
/* 毎時計算値出力　*/

void Eeprinth(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
			  RMVLS *Rmvls, EXSFS *Exsfst, 
			  int Nmpath, MPATH *Mpath, EQSYS *Eqsys, WDAT *Wd)
{
	FLOUT	*flo;
	char	*title;
	int		Mon, Day, i;
	double	time;
	
	if (Daytm->ddpri && Simc->dayprn[Daytm->day])
    {
		title = Simc->title;
		Mon = Daytm->Mon;
		Day = Daytm->Day;
		time = Daytm->time;
		flo = flout;
		
		/*********      for (flo = flout; flo < flout + Nflout; flo++)
		**************/
		for (i = 0; i < Nflout; i++, flo++)
		{
			if ( DEBUG )
				printf ( "Eeprinth MAX=%d flo[%d]=%s\n", Nflout, i, flo->idn ) ;
			
			// 気象データの出力
			if (strcmp(flo->idn, PRTHWD) == 0)
			{
				if (DEBUG)
					printf("<Eeprinth> xprsolrd\n");
				//xprsolrd ( Exsfst->Nexs, Exsfst->Exs);
				Wdtprint(flo->f, title, Mon, Day, time, Wd, Exsfst);
			}
			// 毎時機器の出力
			else if (strcmp(flo->idn, PRTCOMP) == 0)
			{
				//if ( Rmvls->Nrdpnl > 0 )
				//{
				Hcmpprint(flo->f, PRTCOMP, Simc, Mon, Day, time, Eqsys,
					Rmvls->Nrdpnl, Rmvls->Rdpnl);
				//}
			}
			// システム経路の温湿度出力
			else if (strcmp(flo->idn, PRTPATH) == 0)
				Pathprint(flo->f, title, Mon, Day, time, Nmpath, Mpath);
			// 蓄熱槽内温度分布の出力
			else if (strcmp(flo->idn, PRTHRSTANK) == 0)
				Hstkprint(flo->f, title, Mon, Day, time, Eqsys);

#if   SIMUL_BUILDG	 
			// 毎時室温、MRTの出力
			else if (strcmp(flo->idn, PRTREV) == 0)
				Rmevprint(flo->f, title, Rmvls->Room, Mon, Day, time);
			// 放射パネルの出力
			else if (strcmp(flo->idn, PRTHROOM) == 0)
				Rmpnlprint(flo->f, PRTHROOM, Simc, Mon, Day, time,
					Rmvls->Nroom, Rmvls->Room);
			else if (strcmp(flo->idn, PRTHELM) == 0)
				Helmprint(flo->f, PRTHELM, Simc, Mon, Day, time,
					Rmvls->Nroom, Rmvls->Room, &Rmvls->Qetotal);
			else if (strcmp(flo->idn, PRTHELMSF) == 0)
				Helmsurfprint(flo->f, PRTHELMSF, Simc, Mon, Day, time,
					Rmvls->Nroom, Rmvls->Room);
			// PMV計算結果の出力
			else if (strcmp(flo->idn, PRTPMV) == 0)
				Pmvprint(flo->f, title, Rmvls->Room, Mon, Day, time);
			// 室の熱取得要素の出力
			else if (strcmp(flo->idn, PRTQRM) == 0)
				Qrmprint(flo->f, title, Mon, Day, time,
					Rmvls->Room, Rmvls->Qrm);
			// 室内表面温度の出力
			else if (strcmp(flo->idn, PRTRSF) == 0)
				Rmsfprint(flo->f, title, Mon, Day, time,
					Rmvls->Room, Rmvls->Sd);
			// 日よけの影面積出力
			else if (strcmp(flo->idn, PRTSHD) == 0)
				Shdprint(flo->f, title, Mon, Day, time,
					Rmvls->Nsrf, Rmvls->Sd);
			// 壁体内部温度の出力
			else if (strcmp(flo->idn, PRTWAL) == 0)
				Wallprint(flo->f, title, Mon, Day, time,
					Rmvls->Nsrf, Rmvls->Sd);
			// PCM状態値の出力
			else if (strcmp(flo->idn, PRTPCM) == 0)
				PCMprint(flo->f, title, Mon, Day, time,
					Rmvls->Nsrf, Rmvls->Sd);
			// 室内表面熱流の出力
			else if (strcmp(flo->idn, PRTSFQ) == 0)
				Rmsfqprint(flo->f, title, Mon, Day, time, Rmvls->Room, Rmvls->Sd ) ;
			// 室内表面熱伝達率の出力
			else if (strcmp(flo->idn, PRTSFA) == 0)
				Rmsfaprint(flo->f, title, Mon, Day, time, Rmvls->Room, Rmvls->Sd ) ;
#endif	 
		}
	}  
}      
/* ----------------------------------------------------------- */ 
/*  日集計値出力  */

void Eeprintd(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
			  RMVLS *Rmvls,  int Nexs, EXSF *Exs,
			  double *Soldy, EQSYS *Eqsys, WDAT *Wdd)
{ 
	FLOUT *flo;
	char  *title;
	int   Mon, Day, i;
	extern char DAYweek[][4] ;
	static int	ic = 0 ;
	
	if (Daytm->ddpri)
	{
		title = Simc->title;
		Mon = Daytm->Mon;
		Day = Daytm->Day;
		flo = flout;
		
		/***      for (flo = flout; flo < flout + Nflout; flo++)
		*************/
		for (i = 0; i < Nflout; i++, flo++)
		{
			//printf ( "idn=%s\n", flo->idn ) ;
			if (strcmp(flo->idn, PRTDWD) == 0)	 	 
				Wdtdprint(flo->f, title, Mon, Day, Wdd, Nexs, Exs, Soldy);	
			else if (strcmp(flo->idn, PRTWK) == 0)
			{
				if ( ic == 0 )
				{
					fprintf ( flo->f, "Mo Nd Day Week\n" ) ;
					ic = 1 ;
				}
				
				fprintf ( flo->f, "%2d %2d %3d %s\n", Mon, Day, Daytm->day,
					DAYweek[Simc->daywk[Daytm->day]] ) ;
				fflush(flo->f);
			}
			else if (strcmp(flo->idn, PRTDYCOMP) == 0)
				Compodyprt(flo->f, PRTDYCOMP, Simc, Mon, Day, Eqsys,
				Rmvls->Nrdpnl, Rmvls->Rdpnl);
			
#if  SIMUL_BUILDG	   
			else if (strcmp(flo->idn, PRTDYRM) == 0)
				Rmdyprint(flo->f, PRTDYRM, Simc, Mon, Day,
				Rmvls->Nroom, Rmvls->Room);	 	 
			else if (strcmp(flo->idn, PRTDYHELM) == 0)
				Helmdyprint(flo->f, PRTDYHELM, Simc, Mon, Day,
				Rmvls->Nroom, Rmvls->Room,  &Rmvls->Qetotal);	 
			else if (strcmp(flo->idn, PRTDQR) == 0)	 
				Dyqrmprint (flo->f, title, Mon, Day, 
				Rmvls->Room, Rmvls->Trdav, Rmvls->Qrmd);
			else if (strcmp(flo->idn, PRTDYSF) == 0)
				Dysfprint(flo->f, title, Mon, Day, Rmvls->Room ) ;
#endif	 	 
		}
    }
}

/* ----------------------------------------------------------- */ 
/*  月集計値出力  */

void Eeprintm(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
			  RMVLS *Rmvls,  int Nexs, EXSF *Exs,
			  double *Solmon, EQSYS *Eqsys, WDAT *Wdm)
{ 
	FLOUT *flo;
	char  *title;
	int   Mon, Day, i;
	static int	ic = 0 ;
	
	if (Daytm->ddpri)
	{
		title = Simc->title;
		Mon = Daytm->Mon;
		Day = Daytm->Day;
		flo = flout;

		for (i = 0; i < Nflout; i++, flo++)
		{
			if (strcmp(flo->idn, PRTMWD) == 0)	 	 
				Wdtmprint(flo->f, title, Mon, Day, Wdm, Nexs, Exs, Solmon);	
			else if (strcmp(flo->idn, PRTMNCOMP) == 0)
				Compomonprt(flo->f, PRTMNCOMP, Simc, Mon, Day, Eqsys,
				Rmvls->Nrdpnl, Rmvls->Rdpnl);
			else if (strcmp(flo->idn, PRTMNRM) == 0)
				Rmmonprint(flo->f, PRTMNRM, Simc, Mon, Day,
				Rmvls->Nroom, Rmvls->Room);	 	 
		}
    }
}

/* ----------------------------------------------------------- */ 
/*  月－時刻集計値出力  */

void Eeprintmt(SIMCONTL *Simc, int Nflout, FLOUT *flout, EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl)
{ 
	FLOUT *flo;
	char  *title;
	int   i;
	static int	ic = 0 ;
	
	title = Simc->title;
	flo = flout;

	for (i = 0; i < Nflout; i++, flo++)
	{
		if (strcmp(flo->idn, PRTMTCOMP) == 0)
			Compomtprt(flo->f, PRTMNCOMP, Simc, Eqsys, Nrdpnl, Rdpnl);	 
	}
}
