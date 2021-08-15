#pragma once

void Rmhtrcf (EXSFS *Exs,  char *Emrk,  ROOM *Room, RMSRF *Sd, WDAT *Wd); 
void Rmrdshfc(ROOM *Room, RMSRF *Sd);
void Rmhtrsmcf (int Nsrf, RMSRF *Sd);
void Rmexct (ROOM *Room, int Nsrf, RMSRF *Sd, WDAT *Wd, EXSF *Exs, SNBK *Snbk,
	     QRM *Qrm,int nday,int mt); 
/*------higuchi--change 070918 ---
void Rmexct (ROOM *Room, int Nsrf, RMSRF *Sd, WDAT *Wd, EXSF *Exs, SNBK *Snbk,
	     QRM *Qrm);   
	     ----------------------------------------*/  
void Roomcf(int Nmwall, MWALL *Mw, int Nroom, ROOM *Room, 
	                       int Nrdpnl, RDPNL *rdpnl, WDAT *Wd, EXSFS *Exsfs);
void Rmsurft(int Nroom, ROOM *Room, RMSRF *Sd);
void Rmsurftd(int Nroom, ROOM *Room, RMSRF *Sd);
void	Qrmsim (ROOM *Room, WDAT *Wd, QRM *Qrm) ;
