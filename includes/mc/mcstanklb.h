#pragma once

void stoint(double dTM, int N, double Vol, double KAside, double KAtop, double KAbtm,
	     double *dvol, double *Mdt, double *KS, double *Tss, double *Tssold,
	     int *Jva, int *Jvb);
void stofc(int N, int Nin, int *Jcin, int *Jcout, char *ihex, double *ihxeff,
	   int Jva, int Jvb, double *Mdt, double *KS, double gxr,
	   double *Tenv, double *Tssold, double *cGwin, double *EGwin,
	   double *B, double *R, double *d, double *fg);
void stotss(int N, int Nin, int *Jcin, double *B, double *R, double *EGwin, double *Twin,
	    double *Tss);
void stotsexm(int N, double *Tss, int *Jva, int *Jvb, char *dtankF, char *cfcalc);
