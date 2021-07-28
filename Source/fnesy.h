//This file is part of EESLISM.
//
//Foobar is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Foobar is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Foobar.If not, see < https://www.gnu.org/licenses/>.

/* fnesy.h */

#pragma once

/*** es_filees.c ****/


/*  es_schdlr.c   */
void Eeschdlr(int day, int ttmm, SCHDL *Schdl, RMVLS *Rmvls);

/*  es_idcode.c  */
int idssn (char *code, SEASN *Seasn, char *err);
int idwkd (char *code, WKDY *Wkdy, char *err);
int iddsc (char *code, DSCH *Dsch, char *err);
int iddsw (char *code, DSCW *Dscw, char *err);
int idsch (char *code, SCH *Sch, char *err);
int idscw (char *code, SCH *Scw, char *err);
int idroom (char *code, ROOM *Room, char *err);

/*  es_schdata.c  */
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

/*  es_schval.c  */
double schval (int nday, int ttmm, SCH *Sch, DSCH *Dsch);
char	scwmode (int nday, int ttmm, SCH *Scw, DSCW *Dscw);
int iswmode (char c, int N, char *mode);

/* es_cntldat.c */
void Contrldata(FILE *fi, CONTL **Ct, int *Ncontl, CTLIF **Ci, int *Nctlif,
				CTLST **Cs, int *Nctlst, 
				SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt,
				int Nmpath, MPATH *Mpath, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl);
void	ContrlCount ( FILE *fi, int *Nif, int *N ) ;
void ctifdecode(char *s, CTLIF *ctlif, 
				SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt,
				int Nmpath, MPATH *Mpath, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl);
int ctlrgtptr(char *s, CTLTYP *rgt, 
			  SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt,
			  int Nmpath, MPATH *Mpath, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl, char type);

/* es_contllb.c */
int ctlvptr(char *s, SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, int Nmpath, MPATH *Mpath,
			WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl, VPTR *vptr, VPTR *vpath);
int strkey(char *s, char **key);
int kynameptr(char *s, SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, int Nmpath, MPATH *Mpath,
			  WDAT *Wd, EXSFS *Exsf, VPTR *vptr, VPTR *vpath) ;
int pathvptr(int nk, char **key, int Nmpath, MPATH *Mpath,  VPTR *vptr, VPTR *vpath);
COMPNT	*Compntptr ( char *name, int N, COMPNT *Compnt ) ;
int compntvptr(int nk, char **key, COMPNT *Compnt, VPTR *vptr);
int loadptr(COMPNT *loadcmp, char *load, char *s, int Ncompnt, COMPNT *Compnt, VPTR *vptr);


/*  es_contlsch.c */
void Contlschdlr(double dTM, int Ncontl, CONTL *Contl, int Nmpath, MPATH *Mpath, 
				 int Ncompnt, COMPNT *Compnt);
int contrlif(CTLIF *ctlif);
void mpathschd(char control, int Nlpath, PLIST *Plist);
void lpathscdd(char control, PLIST *Plist);
void lpathschd(char control, int Nelm, PELM *Pelm);
void lpathschbat(double dTM,  PLIST *Plist);
   void contlxprint(int Ncontl, CONTL *Contl);
int rmloadreset(double Qload, char loadsw, ELOUT *Eo, char SWITCH);
int chswreset(double Qloads, char chmode, ELOUT *Eo);
int	chqlreset ( HCLOAD *Hcload ) ;
int maxcapreset(double Qload, double Qmax, char chmode, ELOUT *Eo);

