#pragma once

void Roomdata (FILE *fi, char *errkey, EXSF *Exs, DFWL *dfwl, 
                             RMVLS *Rmvls, SCHDL *Schdl, SIMCONTL *Simc);

void Balloc (int N, RMSRF *Sd, WALL *Wall, MWALL **Mw, int *Nmwall);
void Tinit (double Tini, int Nroom, ROOM *Room,
            int Ns, RMSRF *S, int Nmwall, MWALL *Mw);
int	Roomrealloc ( ROOM *R, unsigned int N, unsigned int NN ) ;
void	Roominit ( int N, ROOM *Room ) ;
void	Rmvlsinit ( RMVLS *R ) ;
int		Roomcount ( FILE *fi ) ;
int		Rmsrfcount ( FILE *fi ) ;
void	Rmsrfinit ( int N, RMSRF *S ) ;
