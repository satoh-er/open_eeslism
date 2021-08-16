#pragma once

void wcoil(char Air_SW, char Water_SW, char wet, double Gaet, double Gaeh,
	   double xai, double Twi, ACS *Et, ACS *Ex, ACS *Ew);
double Qcoils(ACS *Et, double Tai, double xai, double Twi);
double Qcoill(ACS *Ex, double Tai, double xai, double Twi);
void hstaircf(double Tw1, double Tw2, double *a, double *b);
