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

void	Simcinit ( SIMCONTL *S ) ;
void	Compinit ( int N, COMPNT *C ) ;
void	Eloutinit ( ELOUT *Eo, int N ) ;
void	Elininit ( int N, ELIN *Ei ) ;
void	Plistinit ( int N, PLIST *Pl ) ;
void	Pelminit ( int N, PELM *Pe ) ;
void	Mpathinit ( int N, MPATH *M ) ;
void	Exsfinit ( int Nd, EXSF *e ) ;
void	Syseqinit ( SYSEQ *S ) ;
void	Eqsysinit ( EQSYS *E ) ;
void	VPTRinit ( VPTR *v ) ;
void	TMDTinit ( TMDT *t ) ;
void	Locinit ( LOCAT	*L ) ;
void	Floutinit ( FLOUT *f ) ;
void	Eqcatinit ( EQCAT *E ) ;
void	Schdlinit ( SCHDL *S ) ;
void	mtEdayinit( EDAY mtEday[12][24] ) ;
