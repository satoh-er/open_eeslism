#pragma once

int PVcadata (char *s, PVCA *PVca) ;
void PVint(int Npv, PV *PV, int Nexsf, EXSF *Exs, WDAT *Wd) ;
void PVene(int Npv, PV *PV) ;
void PVprint(FILE *fo, int id, int Npv, PV *PV) ;
void PVdyint(int Npv, PV *PV) ;
void PVmonint(int Npv, PV *PV) ;
void PVday(int Mon, int Day, int ttmm, int Npv, PV *PV, int Nday, int SimDayend) ;
void PVdyprt(FILE *fo, int id, int Npv, PV *PV) ;
void PVmonprt(FILE *fo, int id, int Npv, PV *PV) ;
void PVmtprt(FILE *fo, int id, int Npv, PV *PV, int Mo, int tt);
