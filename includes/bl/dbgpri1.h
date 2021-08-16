#pragma once

void xprtwallinit (int Nmwall, MWALL *Mw);
void xprsolrd (int Nexs, EXSF *Exs);
void xpralph (int Nroom, ROOM *Room, RMSRF *S);
void xprxas (int Nroom, ROOM *Room, RMSRF *S);
void xprtwpanel (int Nroom, ROOM *Room, double *Twp, RMSRF *Sd, MWALL *Mw);
void	xprtwsrf ( int N, RMSRF *Sd ) ;
void	xprrmsrf ( int N, RMSRF *Sd ) ;
void	xprtwall ( int Nmwall, MWALL *Mw ) ;
