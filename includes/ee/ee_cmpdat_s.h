#pragma once

void Compodata(FILE *f, char *errkey, RMVLS *Rmvls, EQCAT *Eqcat, 
	    COMPNT **Compnt, int *Ncompnt, EQSYS *Eqsys, int *Ncmpalooc, int ID );
int		Compntcount ( FILE *fi ) ;
