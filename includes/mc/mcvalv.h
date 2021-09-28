#pragma once

#include "MODEL.h"

void	Valvcountreset ( int Nvalv, VALV *Valv ) ;
void	Valvcountinc ( int Nvalv, VALV *Valv ) ;
void	Valvene ( int Nvalv, VALV *Valv, int *Valvreset ) ;
void	ValvControl ( FILE *fi,  int Ncompnt, COMPNT *Compnt, SCHDL *Schdl, SIMCONTL *Simc,
					 WDAT *Wd, VPTR *vptr ) ;
void	Valvinit ( int NValv, VALV *Valv, int NMpath, MPATH *Mpath ) ;
int		valv_vptr(char** key, VALV* Valv, VPTR* vptr);
