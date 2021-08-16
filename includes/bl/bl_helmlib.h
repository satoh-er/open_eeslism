#pragma once

void helmrmsrt(ROOM *Room, double Ta);
void helmwall( ROOM *Room, double Ta);
void helmwlsft(int i, int N, double *alr, RMSB *rmsb, BHELM *Tm );
void helmwlt ( int M, int mp, double *UX, double uo, double um, 
	       double Pc, BHELM *Tie, BHELM *Te, BHELM *Tpe,
	       BHELM *Told, BHELM *Tw );
void helmq(ROOM *Room, double dTM, double Ta, double xa );
void qelmclear(QHELM *q);
void qelmsum(QHELM *a, QHELM *b);
void helmclear(BHELM *b);
void helmsumpd(int N, double *u, BHELM *a, BHELM *b);
void helmsumpf(int N, double *u, BHELM *a, BHELM *b);
void helmdiv (BHELM *a, double c);
void helmsum(BHELM *a, BHELM *b);
void helmcpy(BHELM *a, BHELM *b);
void helmxxprint(char *s, BHELM *a);
