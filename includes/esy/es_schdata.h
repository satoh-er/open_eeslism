#pragma once

void Dayweek(FILE *fi, char *Ipath, int *daywk, int key);
/***************************
void Schtable (FILE *fi, char *dsn, SEASN *Seasn, WKDY *Wkdy,
	       DSCH *Dsch, DSCW *Dscw);
		   **************************/
void Schtable (FILE *fi, char *dsn, SCHDL *Schdl ) ;
void Schdata (FILE *fi, char *dsn, int *daywk, SCHDL *Schdl );
void Schname (char *Ipath, char *dsn, SCHDL *Schdl );
void	SchCount ( FILE *fi, int *ssn, int *wkd, int *vl, int *sw,
				  int *ssnmx, int *vlmx, int *swmx ) ;
int		Schcmpcount ( FILE *fi ) ;
