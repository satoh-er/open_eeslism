#pragma once

int Refadata (char *s, REFACA *Refaca, int Nrfcmp, RFCMP *Rfcmp);
void Refaint(int Nrefa, REFA *Refa, WDAT *Wd, int Ncompnt, COMPNT *Compnt);
void Refacfv(int Nrefa, REFA *Refa);
void refacoeff(REFA *Refa, int *err);
void Refaene(int Nrefa, REFA *Refa, int *LDreset); 
void Refaene2(int Nrefa, REFA *Refa);
int refaldptr(char *load, char **key, REFA *Refa, VPTR *vptr);
int refaswptr(char **key, REFA *Refa, VPTR *vptr);
void refaldschd(REFA *Refa);
void refaprint(FILE *fo, int id, int Nrefa, REFA *Refa);
void refadyint(int Nrefa, REFA *Refa);
void refamonint(int Nrefa, REFA *Refa);
void refaday(int Mon, int Day, int ttmm, int Nrefa, REFA *Refa, int Nday, int SimDayend);
void refadyprt(FILE *fo, int id, int Nrefa, REFA *Refa);
void refamonprt(FILE *fo, int id, int Nrefa, REFA *Refa);
void refamtprt(FILE *fo, int id, int Nrefa, REFA *Refa, int Mo, int tt) ;
