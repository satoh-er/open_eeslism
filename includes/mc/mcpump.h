#pragma once

int Pumpdata (char *cattype, char *s, PUMPCA *Pumpca, int Npfcmp, PFCMP *pfcmp) ;
void Pumpint(int Npump, PUMP *Pump, int Nexsf, EXSF *Exs);
void Pumpflow(int Npump, PUMP *Pump);
void Pumpcfv(int Npump, PUMP *Pump);
double	PumpFanPLC ( double XQ, PUMP *Pump ) ;
void Pumpene(int Npump, PUMP *Pump);
void pumpprint(FILE *fo, int id, int Npump, PUMP *Pump);
void pumpdyint(int Npump, PUMP *Pump);
void pumpmonint(int Npump, PUMP *Pump);
void pumpday(int Mon, int Day, int ttmm, int Npump, PUMP *Pump, int Nday, int SimDayend);
void pumpdyprt(FILE *fo, int id, int Npump, PUMP *Pump);
void pumpmonprt(FILE *fo, int id, int Npump, PUMP *Pump);
void pumpmtprt(FILE *fo, int id, int Npump, PUMP *Pump, int Mo, int tt) ;
void	PFcmpInit(int Npfcmp, PFCMP *Pfcmp) ;
void	PFcmpdata(FILE *fl, int *Npfcmp, PFCMP *pfcmp) ;
