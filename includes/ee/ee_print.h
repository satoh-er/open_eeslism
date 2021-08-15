#pragma once

void Eeprinth(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
	      RMVLS *Rmvls, EXSFS *Exs, 
	      int Nmpath, MPATH *Mpath, EQSYS *Eqsys, WDAT *Wd);
void Eeprintd(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
	     RMVLS *Rmvls,  int Nexs, EXSF *Exs,
	     double *Soldy, EQSYS *Eqsys, WDAT *Wdd);
void Eeprintm(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
			  RMVLS *Rmvls,  int Nexs, EXSF *Exs,
			  double *Solmon, EQSYS *Eqsys, WDAT *Wdm);
void Eeprintmt(SIMCONTL *Simc, int Nflout, FLOUT *flout, EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl) ;
