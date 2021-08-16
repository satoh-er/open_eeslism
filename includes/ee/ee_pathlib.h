#pragma once

void pelmco(char, PELM *, char *);
void pelmci(char, PELM *, char *);
void plistcpy(MPATH *Mpath, int *Npelm, PELM *Pelm, PLIST *Plist,
	      int Ncompnt, COMPNT *Compnt);    
void plevel(int Nmpath, MPATH *Mpath, int Ncnvrg, COMPNT **Cnvrg);
void	pflowstrct ( int Nmpath, MPATH *Mpath ) ;
