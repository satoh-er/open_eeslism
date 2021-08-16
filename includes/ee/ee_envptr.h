#pragma once

double *envptr(char *s, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd, EXSFS *Exsf);
ROOM   *roomptr ( char *s, int Ncompnt, COMPNT *Compnt ) ;
void   *hccptr ( char c, char *s, int Ncompnt, COMPNT *Compnt, char *mon ) ;
void   *rdpnlptr ( char *s, int Ncompnt, COMPNT *Compnt ) ;
