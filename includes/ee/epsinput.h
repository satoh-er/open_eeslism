#pragma once

void esondat(FILE *fi, ESTL *Estl);
void esoint(FILE *fi, char *err, int Ntime, ESTL *Estl, TLIST *Tlist);
void fofmt(ESTL *Estl, TLIST *Tlist);
int tmdata(VCFILE *Vcfile, TMDT *Tmdt, DAYTM *Daytm, char perio) ;
void esdatgt(FILE *fi, int i, int Ndata, TLIST *Tlist);
