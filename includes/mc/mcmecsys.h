#pragma once

#include "MODEL.h"

void Mecsinit(double dTM, EQSYS *Eqsys,
	      SIMCONTL *Simc, int Ncompnt,COMPNT *Compnt,
		  int Nexsf, EXSF *Exsf, WDAT *Wd, RMVLS *Rmvls);
void	Mecscf(EQSYS *Eqsys);
void Mecsene(EQSYS *Eqsys);
