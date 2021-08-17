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

/* evcwdfl.c  */


#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "fesy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "lib/u_psy.h"

/* VCFILE からの気象データ入力 */

void wdflinit(SIMCONTL *Simc, ESTL *Estl, TLIST *Tlist)
{
	LOCAT *loc;
	WDPT  *wp;
	char  *s, *st, ss[SCHAR], Err[SCHAR];
	double dt, *val;
	int   id = 0, N, i, m = -1;
	
	if ((s = Estl->wdloc) == NULL)
		return;
	
	/****printf("<<wdflinit>> wdloc=%s\n", s);****/
	
	sprintf_s(Err, sizeof(Err), ERRFMT, "(wdflinit)");   

	Locinit (Simc->Loc) ;
	loc = Simc->Loc;

	m = -1;
	
	while (sscanf_s(s, "%s", ss, sizeof(ss)), *s != ';')
	{
		N = (int)strlen(ss);
		if ((st = strchr(ss, '=')) != NULL)
		{
			*st = '\0';
			dt = atof(st + 1);
			if (strcmp(ss, "Lat") == 0)
				loc->Lat = dt;
			else if (strcmp(ss, "Lon") == 0)
				loc->Lon = dt;
			else if (strcmp(ss, "Ls") == 0)
				loc->Ls = dt;
			else if (strcmp(ss, "Tgrav") == 0)
				loc->Tgrav = dt;
			else if (strcmp(ss, "DTgr") == 0)
				loc->DTgr = dt;
			else if (strcmp(ss, "daymx") == 0)
				loc->daymxert = (int)dt;
			else
				id = 1;
		}
		else
		{
			if (*ss == '-' || m >= 0)
			{
				if (strcmp(ss, "-Twsup") == 0)
					m = 0;
				else
				{
					loc->Twsup[m] = atof(ss);
					m++;
					if (m > 11)
						m = -1;
				}
			}
			else
				loc->name = stralloc ( ss);
		}
		s += N;
		while (isspace(*(++s)));
	}
	/***** printf("<wdflinit>  loc->name=%s\n", loc->name); *****/
	
	if (id)
		printf("%s %s\n", Err, ss);
	
	wp = &Simc->wdpt;
	wp->ta = NULL;
	wp->xa = NULL ;
	wp->rn = NULL ;
	wp->ihor = NULL ;
	wp->rh = NULL ;
	wp->cc = NULL ;
	wp->wv = NULL ;
	wp->wdre = NULL ;

	for (i = 0; i < Estl->Ndata; i++, Tlist++)
	{
		if (strcmp(Tlist->name, "Wd") == 0)
		{
			s = Tlist->id;
			val = Tlist->fval;
			
			if (strcmp(s, "T") == 0)
				wp->ta = val;
			else if (strcmp(s, "x") == 0)
				wp->xa = val;	 
			else if (strcmp(s, "Idn") == 0)
				wp->idn = val;
			else if (strcmp(s, "Isky") == 0)
				wp->isky= val;
			else if (strcmp(s, "Ihor") == 0)
				wp->ihor = val;
			else if (strcmp(s, "CC") == 0)
				wp->cc = val;
			else if (strcmp(s, "Wdre") == 0)
				wp->wdre = val;
			else if (strcmp(s, "Wv") == 0)
				wp->wv= val;
			else if (strcmp(s, "RH") == 0)
				wp->rh = val;
			else if (strcmp(s, "RN") == 0)
				wp->rn = val;
		}
	}
}
/* --------------------------------------------- */

void wdflinput(WDPT *wp, WDAT *Wd)
{
	extern double Sgm;
	double  Br;
	
	Wd->T = *wp->ta;
	
	Wd->Idn = *wp->idn;
	Wd->Isky = *wp->isky;
	if (wp->ihor == NULL)
		Wd->Ihor = Wd->Idn * Wd->Sh + Wd->Isky;
	
	if (wp->xa != NULL)
		Wd->x = *wp->xa;
	else
		Wd->x = -999.0;
	
	if (wp->rh != NULL)
		Wd->RH = *wp->rh;
	else
		Wd->RH = -999.0;
	
	if (wp->cc != NULL)   
		Wd->CC = *wp->cc;
	else
		Wd->CC = -999.0;
	
	if (wp->rn != NULL)
		Wd->RN = *wp->rn;
	else
		Wd->RN = -999.0;
	
	if (wp->wv != NULL)
		Wd->Wv = *wp->wv;
	else
		Wd->Wv = -999.0;
	
	if (wp->wdre != NULL)
		Wd->Wdre = *wp->wdre;
	else
		Wd->Wdre = 0.0;
	
	if (Wd->x > 0.0 && Wd->RH < 0.0)
		Wd->RH = FNRhtx(Wd->T, Wd->x);
	else if (Wd->x < 0.0 && Wd->RH > 0.0)
		Wd->x = FNXtr(Wd->T, Wd->RH);
	
	if (Wd->x > 0.0)
		Wd->h = FNH(Wd->T, Wd->x);
	
	if (Wd->x > 0.0 && Wd->CC > 0.0 || Wd->RN < 0.0 )
	{
		Br= 0.51+0.209*sqrt(FNPwx(Wd->x));
		Wd->RN= (1.0 - 0.62 * Wd->CC /10.0)*(1.-Br)*Sgm*pow(Wd->T+273.15, 4.0); 
        Wd->Rsky=((1.0-0.62*Wd->CC/10.0)*Br+0.62*Wd->CC/10.0)
                                *Sgm*pow(Wd->T+273.15,4.0) ;  /*--higuchi 070918--*/
	}
	else
		Wd->Rsky = Sgm*pow(Wd->T + 273.15,4.0) - Wd->RN ;
}

