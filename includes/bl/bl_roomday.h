#pragma once

void Roomday(int Mon, int Day, int Nday, int ttmm, int Nroom, ROOM *Rm, int Nrdpnl, RDPNL *Rdp, int SimDayend);
void Rmdyprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, int Nroom, ROOM *Rm);
void paneldyprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl);
void panelmonprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl);
void Rmmonprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, int Nroom, ROOM *Rm);
void panelmtprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl, int Mo, int tt) ;
