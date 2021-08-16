#pragma once

void Contlschdlr(double dTM, int Ncontl, CONTL *Contl, int Nmpath, MPATH *Mpath, 
				 int Ncompnt, COMPNT *Compnt);
int contrlif(CTLIF *ctlif);
void mpathschd(char control, int Nlpath, PLIST *Plist);
void lpathscdd(char control, PLIST *Plist);
void lpathschd(char control, int Nelm, PELM *Pelm);
void lpathschbat(double dTM,  PLIST *Plist);
   void contlxprint(int Ncontl, CONTL *Contl);
int rmloadreset(double Qload, char loadsw, ELOUT *Eo, char SWITCH);
int chswreset(double Qloads, char chmode, ELOUT *Eo);
int	chqlreset ( HCLOAD *Hcload ) ;
int maxcapreset(double Qload, double Qmax, char chmode, ELOUT *Eo);
