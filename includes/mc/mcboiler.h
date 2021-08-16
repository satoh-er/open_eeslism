#pragma once

int Boidata (char *s, BOICA *Boica);
void Boicfv(int Nboi, BOI *Boi);
void Boiene(int Nboi, BOI *Boi, int *BOIreset);
int boildptr(char *load, char **key, BOI *Boi, VPTR *vptr);
void boildschd(BOI *Boi);
void boiprint(FILE *fo, int id, int Nboi, BOI *Boi);
void boidyint(int Nboi, BOI *Boi);
void boimonint(int Nboi, BOI *Boi);
void boiday(int Mon, int Day, int ttmm, int Nboi, BOI *Boi, int Nday, int SimDayend);
void boidyprt(FILE *fo, int id, int Nboi, BOI *Boi);
void boimonprt(FILE *fo, int id, int Nboi, BOI *Boi);
void boimtprt(FILE *fo, int id, int Nboi, BOI *Boi, int Mo, int tt) ;
void	Boicaint(int NBoica, BOICA *Boica,  SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl) ;
