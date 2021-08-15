#pragma once

#include "MODEL.h"

int VAVdata (char *s, char *ss, VAVCA *vavca) ;
void VAVcfv(int Nvav, VAV *vav) ;
void VAVene(int Nvav, VAV *vav, int *VAVrest) ;
void	VAVcountreset ( int Nvav, VAV *VAV ) ;
int		vavswptr(char **key, VAV *VAV, VPTR *vptr) ;
int chvavswreset(double Qload, char chmode, VAV *VAV) ;
void vavprint(FILE *fo, int id, int Nvav, VAV *VAV) ;
void	VAVcountinc ( int Nvav, VAV *VAV ) ;
void VWVint(int Nvav, VAV *VAV, int Ncompnt, COMPNT *Compn) ;
double	FNVWVG ( VAV *VWV ) ;
double FNFd ( double Wa, double Ww, double KA ) ;
