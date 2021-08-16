#pragma once

int ctlvptr(char *s, SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, int Nmpath, MPATH *Mpath,
			WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl, VPTR *vptr, VPTR *vpath);
int strkey(char *s, char **key);
int kynameptr(char *s, SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, int Nmpath, MPATH *Mpath,
			  WDAT *Wd, EXSFS *Exsf, VPTR *vptr, VPTR *vpath) ;
int pathvptr(int nk, char **key, int Nmpath, MPATH *Mpath,  VPTR *vptr, VPTR *vpath);
COMPNT	*Compntptr ( char *name, int N, COMPNT *Compnt ) ;
int compntvptr(int nk, char **key, COMPNT *Compnt, VPTR *vptr);
int loadptr(COMPNT *loadcmp, char *load, char *s, int Ncompnt, COMPNT *Compnt, VPTR *vptr);
