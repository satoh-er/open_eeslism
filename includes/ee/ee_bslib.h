#pragma once

void Exsfdata (FILE *fi, char *dsn, EXSFS *Exsf, SCHDL *Schdl, SIMCONTL *Simc );
void Exsfsol(int Nexs, WDAT *Wd, EXSF *Exs);
void Glasstga (double Ag, double tgtn, double Bn, double cinc, double Fsdw, 
			   double Idr, double Idf,  double *Qgt, double *Qga, char *Cidtype, double Profile, double Gamma);
double Glscid(double cinc);
int	Exsfrealloc ( EXSF **E, unsigned int N, unsigned int NN ) ;
int	ExsfCount ( FILE *fi ) ;
