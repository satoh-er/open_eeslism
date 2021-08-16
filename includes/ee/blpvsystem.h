#pragma once

double	FNKPT(double TPV, double apmax) ;
void	PVwallcatinit(PVWALLCAT *PVwallcat) ;
void	PVwallPreCalc(PVWALLCAT *PVwallcat) ;
double	FNTPV(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
void	CalcPowerOutput(int Nsrf, RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
