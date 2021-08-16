#pragma once

void Htrcf (double *alc, double *alo, char alotype, EXSF *Exs, double Tr, int N, double *alr,
			RMSRF *Sd, char *RMmrk, WDAT *Wd );
void Radcf0 (double Tsav, double *alrbold, int N, RMSRF *Sd,
	     double *W, double *alr);
void radex(int N,  RMSRF *Sd, double *F, double *W);
void formfaprx(int N, double Aroom, RMSRF *Sd, double *F);
void Radshfc(int N, double FArea, double Aroom, RMSRF *Sd, double tfsol, double eqcv, char *Rmname, double *fsol);
double  alcvup(double);
double  alcvdn(double);
double  alcvh(double);
