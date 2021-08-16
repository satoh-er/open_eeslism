#pragma once

void Compoday(int Mon, int Day, int Nday, int ttmm, EQSYS *Eqsys, int SimDayend);
void Compodyprt(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, 
		EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl);
void Compomonprt(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, 
				EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl);
void Compomtprt(FILE *fo, char *mrk, SIMCONTL *Simc, EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl) ;
