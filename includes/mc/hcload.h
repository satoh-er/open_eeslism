#pragma once

void Hclelm(int Nhcload, HCLOAD *Hcload);
void Hcldcfv(int Nhcload, HCLOAD *Hcload);
void Hcldene(int Nhcload, HCLOAD *Hcload, int *LDrest, WDAT *Wd);
int  hcldptr(char *load, char **key, HCLOAD *Hcload, VPTR *vptr, char *idmrk);
void hcldschd(HCLOAD *Hcload);
void hcldprint(FILE *fo, int id, int Nhcload, HCLOAD *Hcload);
void hclddyint(int Nhcload, HCLOAD *Hcload);
void hcldmonint(int Nhcload, HCLOAD *Hcload);
void hcldday(int Mon, int Day, int ttmm, int Nhcload, HCLOAD *Hcload, int Nday, int SimDayend);
void hclddyprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload);
void hcldmonprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload);
void hcldmtprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload, int Mo, int tt);
int  hcldswptr (char **key, HCLOAD *Hcload, VPTR *vptr) ;
int  chhcldswreset(double Qload, double Ql, char chmode, ELOUT *Eo) ;
void	hcldwetmdreset ( EQSYS *Eqsys ) ;
void	hcldmodeinit ( EQSYS *Eqsys ) ;
double	FNeffthc(double Tco, double Tci, double xci, double Qc, double Ec, double Go, double Gi, double cai, double cao) ;
double	FNeffthh(double Tco, double Tci, double xco, double Qc, double Eh, double Go, double Gi, double cai, double cao) ;
void rmacdat(HCLOAD *Hcld) ;
void rmacddat(HCLOAD *Hcld) ;
double	FNTcndc(double Tco, double Qc, double Ec, double cao, double Go) ;
double	FNTevpc(double Tci, double Qc, double cai, double Gi, double xci) ;
double	FNTcndh(double Tci, double Qc, double cai, double Gi) ;
double	FNTevph(double Tco, double Qc, double Eh, double cao, double Go, double xco) ;