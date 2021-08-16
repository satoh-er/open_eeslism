#pragma once

int Stankdata(FILE *f, char *s, STANKCA *Stankca);
void Stankmemloc(char *errkey, STANK *Stank);
void Stankint(double dTM, int Nstank, STANK *Stank, 
	      SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd);
void Stankcfv(int Nstank, STANK *Stank ) ;
int stankvptr(char **key, STANK *Stank, VPTR *vptr);

void Stanktss(int Nstank, STANK *Stank, int *TKreset);
void Stankene(int Nstank, STANK *Stank);
void stankcmpprt (FILE *fo, int id, int Nstank, STANK *Stank);
void stankivprt (FILE *fo, int id, int Nstank, STANK *Stank);
void stankdyint(int Nstank, STANK *Stank);
void stankmonint(int Nstank, STANK *Stank);
void stankday(int Mon, int Day, int ttmm, int Nstank, STANK *Stank, int Nday, int SimDayend);
void stankdyprt(FILE *fo, int id, int Nstank, STANK *Stank);
void stankmonprt(FILE *fo, int id, int Nstank, STANK *Stank);
