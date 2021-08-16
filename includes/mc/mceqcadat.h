#pragma once

#include <stdio.h>
#include "MODEL.h"

void Eqcadata (FILE *f, char *dsn, EQCAT *Eqcat);
void	Eqpcount ( FILE *fi, int *NBOI, int *NREFA, int *NCOL, int *NSTANK, int *NHCC,
				  int *NHEX, int *NPIPE, int *NPUMP, int *NVAV, int *NSTHEAT, int *NTHEX, int *NPV, int *NOMVAV, int *NDESI, int *NEVAC ) ;
int	pflistcount ( FILE *fl ) ;
