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

#pragma once

#include "simcon.h"
#include "eepath.h"
#include "eqptyp.h"
#include "control.h"
#include "sched.h"
#include "wthrd.h"
#include "eps.h"


void	monthday(int *Mon, int *Day, int mo, int dayo);

void Weatherdt(SIMCONTL *Simc, DAYTM *Daytm, LOCAT *Loc, WDAT *Wd, EXSF *Exs, char EarthSrfFlg);
void	gtsupw (FILE *fp, char *Loc, int *nmx, double *Tgrav, double *DTgr, double *Tsupw);
void	hspwdread(FILE *fp, int nday, LOCAT *Loc, int *year, int *mon, int *day, int *wkdy, double dt[7][25]);
void	dt2wdata ( WDAT *Wd, int tt, double dt[7][25] ) ;
void	WdLinearDiv ( WDAT *Wd, WDAT *WdL, WDAT *WdF, double dt ) ;
void	wdatadiv ( DAYTM *Daytm, WDAT *Wd, double dt[7][25], double dtL[7][25] ) ;
void EarthSrfTempInit(SIMCONTL *Simc, LOCAT *Loc, WDAT *Wd) ;
