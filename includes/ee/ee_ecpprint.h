#pragma once

void cmpprint(int id, int N,  COMPNT *cmp, ELOUT *Elout, ELIN *Elin);
void eloutprint(int id, int N, ELOUT *E, COMPNT *cmp);
void eloutfprint(int id, int N, ELOUT *E, COMPNT *cmp);
void elinprint(int id, int N, COMPNT *C, ELOUT *eo, ELIN *ei);
void elinfprint(int id, int N, COMPNT *C, ELOUT *eo, ELIN *ei);
void plistprint(int N, MPATH *Mpath, PELM *Pe, ELOUT *Eo, ELIN *Ei);
