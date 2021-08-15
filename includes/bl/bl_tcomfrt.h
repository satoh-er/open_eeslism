#pragma once

void Rmotset (int Nroom, ROOM *Room);
void Fotinit(int Nroom, ROOM *Room);
void Fotf(ROOM *Room);
void Rmcomfrt ( int Nroom, ROOM *Room);
double Pmv0(double met, double Icl, double Tr, double xr, double Tmrt, double v);
double SET_star(double TA, double TR, double VEL, double RH, double MET, double CLO, double WME, double PATM);
