#pragma once

void Vcfdata(FILE *fi, SIMCONTL *Simcon);
int	VCFcount ( FILE *fi ) ;
void flindat(FLIN *Flin);
void Flinint(int Nflin, FLIN *Flin, 
	     SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wdat);
int vcfptr(char **key, SIMCONTL *Simc, VPTR *vptr);
void Vcfinput(DAYTM *Daytm, int Nvcfile, VCFILE *Vcfile, char perio);
void	Flinprt ( int N, FLIN *Fl ) ;
