#pragma once

int Hccdata(char *s, HCCCA *Hccca);
void Hccdwint(int Nhcc, HCC *Hcc);
void Hcccfv(int Nhcc, HCC *Hcc);
void Hccdwreset(int Nhcc, HCC *Hcc, int *DWreset);
void Hccene(int Nhcc, HCC *Hcc);
void hccprint(FILE *fo, int id, int Nhcc, HCC *Hcc);
void hccdyint(int Nhcc, HCC *Hcc);
void hccmonint(int Nhcc, HCC *Hcc);
void hccday(int Mon, int Day, int ttmm, int Nhcc, HCC *Hcc, int Nday, int SimDayend);
void hccdyprt(FILE *fo, int id, int Nhcc, HCC *Hcc);
void hccmonprt(FILE *fo, int id, int Nhcc, HCC *Hcc);
double	FNhccet ( double Wa, double Ww, double KA ) ;
