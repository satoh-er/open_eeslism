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


void Wdtsum(int Mon, int Day, int Nday, int ttmm, WDAT *Wd, int Nexs, EXSF *Exs,
			WDAT *Wdd, WDAT *Wdm, double *Soldy, double *Solmon, SIMCONTL *Simc);
void	Wdtprint(FILE *fo, char *title, int Mon, int Day, double time, WDAT *Wd, EXSFS *Exs);
void	Wdtdprint(FILE *fo, char *title, int Mon, int Day, WDAT *Wdd, int Nexs, EXSF *Exs, double *Soldy);
void	Wdtmprint(FILE *fo, char *title, int Mon, int Day, WDAT *Wdm, int Nexs, EXSF *Exs, double *Solmon);
