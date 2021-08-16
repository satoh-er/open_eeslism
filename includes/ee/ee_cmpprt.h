#pragma once

void Hcmpprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time, EQSYS *Eqsys,
	      int Nrdpnl, RDPNL *Rdpnl);

void Hstkprint(FILE *fo, char *title, int mon, int day, double time, EQSYS *Eqsys);
