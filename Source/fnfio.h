/*  fnfio.h  */
/**** ee_files.c *****/

/*  ee_spre.c  */
void eesprera (char *File, char *Ipath);
void eespre(char *File, char *Ipath, int *key);
void	Syscheck ( FILE *fi, int *syspth, int *syscmp ) ;

/*  ee_sfiles.c  */
void eeflopen(SIMCONTL *Simc, int Nflout, FLOUT *Flout);
void eeflclose( int Nflout, FLOUT *Flout);

/*  ee_input.c   */
/*===============higuchi del INPUT.h へ移動 070918==================
void Eeinput(SIMCONTL *Simc, SCHDL *Schdl, 	    	     
	     EXSFS *Exsf, RMVLS *Rmvls, EQCAT *Eqcat, EQSYS *Eqsys, 
	     COMPNT **Compnt, int *Ncompnt, int *Ncmpalloc, ELOUT **Elout, int *Nelout, ELIN **Elin, int *Nelin, 	    
	     MPATH **Mpath, int *Nmpath, PLIST **Plist, PELM **Pelm, int *Npelm,
	     CONTL **Contl, int *Ncontl, CTLIF **Ctlif, int *Nctlif, CTLST **Ctlst, int *Nctlst,
	     FLOUT *Flout,  int *Nflout, WDAT *Wd, DAYTM *Daytm, int key, int *Nplist); 
	     ==================================================================*/   

/*  ee_print.c  */
void Eeprinth(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
	      RMVLS *Rmvls, EXSFS *Exs, 
	      int Nmpath, MPATH *Mpath, EQSYS *Eqsys, WDAT *Wd);
void Eeprintd(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
	     RMVLS *Rmvls,  int Nexs, EXSF *Exs,
	     double *Soldy, EQSYS *Eqsys, WDAT *Wdd);
void Eeprintm(DAYTM *Daytm, SIMCONTL *Simc, int Nflout, FLOUT *flout, 
			  RMVLS *Rmvls,  int Nexs, EXSF *Exs,
			  double *Solmon, EQSYS *Eqsys, WDAT *Wdm);
void Eeprintmt(SIMCONTL *Simc, int Nflout, FLOUT *flout, EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl) ;

/*  ee_gndata.c   */
void Gdata(FILE *fi, char *dsn, char *File, char **wfname, char **ofname,
	int *dtm, int *sttmm, int *dayxs, int *days, int *daye, double *Tini, int *pday,
	int *wdpri, int *revpri, int *pmvpri, char *helmkey, int *MaxIterate, DAYTM *Daytm, WDAT *Wd, char *perio);

/*  ee_pthprt.c */
void Pathprint(FILE *fo, char *title, int mon, int day, double time,
                     int Nmpath, MPATH *Mpath);

/* ee_cmpprt.c */
void Hcmpprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time, EQSYS *Eqsys,
	      int Nrdpnl, RDPNL *Rdpnl);

void Hstkprint(FILE *fo, char *title, int mon, int day, double time, EQSYS *Eqsys);


/* compoday.c */
void Compoday(int Mon, int Day, int Nday, int ttmm, EQSYS *Eqsys, int SimDayend);
void Compodyprt(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, 
		EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl);
void Compomonprt(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, 
				EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl);
void Compomtprt(FILE *fo, char *mrk, SIMCONTL *Simc, EQSYS *Eqsys, int Nrdpnl, RDPNL *Rdpnl) ;

/* ee_ecpprint.c  */
void cmpprint(int id, int N,  COMPNT *cmp, ELOUT *Elout, ELIN *Elin);
void eloutprint(int id, int N, ELOUT *E, COMPNT *cmp);
void eloutfprint(int id, int N, ELOUT *E, COMPNT *cmp);
void elinprint(int id, int N, COMPNT *C, ELOUT *eo, ELIN *ei);
void elinfprint(int id, int N, COMPNT *C, ELOUT *eo, ELIN *ei);
void plistprint(int N, MPATH *Mpath, PELM *Pe, ELOUT *Eo, ELIN *Ei);

/* ee_dayslib.c */
void svdyint(SVDAY *vd);
void svdaysum(long ttmm, char control, double v, SVDAY *vd);
void svmonsum(int Mon, int Day, int time, char control, double v, SVDAY *vd, int Dayend, int SimDayend) ;
void qdyint(QDAY *Qd);
void qdaysum(int ttmm, char control, double Q, QDAY *Qd);
void qmonsum(int Mon, int Day, int time, char control, double Q, QDAY *Qd, int Dayend, int SimDayend);
void qdaysumNotOpe(int time, double Q, QDAY *Qd);
void qmonsumNotOpe(int Mon, int Day, int time, double Q, QDAY *Qd, int Dayend, int SimDayend);
void edyint(EDAY *Ed);
void edaysum(int ttmm, char control, double E, EDAY *Ed);
void emonsum(int Mon, int Day, int time, char control, double E, EDAY *Ed, int Dayend, int SimDayend);
void emtsum(int Mon, int Day, int time, char control, double E, EDAY *Ed) ;
void minmark(double *minval, long *timemin, double v, long time);
void maxmark(double *maxval, long *timemax, double v, long time);

/*  ee_ttlprt.c  */
void ttlprint(FILE *fo, char *fileid, SIMCONTL *Simc);
void ttldyprint(FILE *fo, char *fileid, SIMCONTL *Simc);
void ttlmtprint(FILE *fo, char *fileid, SIMCONTL *Simc);

/*  ee_cmpdat_s.c  */
void Compodata(FILE *f, char *errkey, RMVLS *Rmvls, EQCAT *Eqcat, 
	    COMPNT **Compnt, int *Ncompnt, EQSYS *Eqsys, int *Ncmpalooc, int ID );
int		Compntcount ( FILE *fi ) ;

/* ee_flwin.c *****************
void Flwindata(FLWIN *Flwin, int *Nflwin, WDAT *Wd);
void Flwinconst(int Nmpath, MPATH *Mpath);
void flwinxprint(int Nflwin, FLWIN *Flwin);
**************************/

/*  ee_pathdat.c   */
void Pathdata (FILE *f, char *errkey, SIMCONTL *Simc, WDAT *Wd, int  Ncompnt,  COMPNT *Compnt, SCHDL *Schdl,
	       MPATH **Mpath, int *Nmpath, PLIST **Plist, PELM **Pelm, int *Npelm, int *Nplist, int ID, EQSYS *Eqsys ) ;
int		Mpathcount ( FILE *fi, int *Pl ) ;
void	Plcount ( FILE *fi, int *N ) ;
int		Pelmcount ( FILE *fi ) ;
void	Elcount ( int N, COMPNT *C, int *Nelout, int *Nelin ) ;

/*  ee_pathlib.c   */
void pelmco(char, PELM *, char *);
void pelmci(char, PELM *, char *);
void plistcpy(MPATH *Mpath, int *Npelm, PELM *Pelm, PLIST *Plist,
	      int Ncompnt, COMPNT *Compnt);    
void plevel(int Nmpath, MPATH *Mpath, int Ncnvrg, COMPNT **Cnvrg);
void	pflowstrct ( int Nmpath, MPATH *Mpath ) ;

/*  ee_pflow.c  */

void Pflow(int Nmpath, MPATH *Mpath, WDAT *Wd);


/*  ee_sysupv.c  */
void Sysupv(int Nmpath, MPATH *Mpath, RMVLS *Rmvls);

/*  ee_syseqv.c   */
void Syseqv(int Nelout, ELOUT *Elout, SYSEQ *Syseq);

/* ee_sysvar.c  */
void Sysvar(int Ncompnt, COMPNT *Compnt);


/*  ee_pthene.c */
void Pathheat(int Nmpath, MPATH *Mpath);


/* ee_syselmalc.c    */
void Elmalloc(char *errkey, 
	      int Ncompnt, COMPNT *Compnt, EQCAT *Eqcat, EQSYS *Eqsys, 
		  ELOUT **Elout, int *Nelout, ELIN **Elin, int *Nelin);

/* ee_envptr.c  */
double *envptr(char *s, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd, EXSFS *Exsf);
ROOM   *roomptr ( char *s, int Ncompnt, COMPNT *Compnt ) ;
void   *hccptr ( char c, char *s, int Ncompnt, COMPNT *Compnt, char *mon ) ;
void   *rdpnlptr ( char *s, int Ncompnt, COMPNT *Compnt ) ;
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


/*  ee_bslib.c  */
void Exsfdata (FILE *fi, char *dsn, EXSFS *Exsf, SCHDL *Schdl, SIMCONTL *Simc );
void Exsfsol(int Nexs, WDAT *Wd, EXSF *Exs);
void Glasstga (double Ag, double tgtn, double Bn, double cinc, double Fsdw, 
			   double Idr, double Idf,  double *Qgt, double *Qga, char *Cidtype, double Profile, double Gamma);
double Glscid(double cinc);
int	Exsfrealloc ( EXSF **E, unsigned int N, unsigned int NN ) ;
int	ExsfCount ( FILE *fi ) ;

/*  ee_rthlib.c  */
double Tearth (double Z, int n, int nmx, double Tgro, double DTg, double a);
void Exsfter (int day, int daymx, double Tgrav, double DTgr, EXSF *Exs,WDAT *Wd, int tt);

/* eevcdat.c */
void Vcfdata(FILE *fi, SIMCONTL *Simcon);
int	VCFcount ( FILE *fi ) ;
void flindat(FLIN *Flin);
void Flinint(int Nflin, FLIN *Flin, 
	     SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wdat);
int vcfptr(char **key, SIMCONTL *Simc, VPTR *vptr);
void Vcfinput(DAYTM *Daytm, int Nvcfile, VCFILE *Vcfile, char perio);
void	Flinprt ( int N, FLIN *Fl ) ;

/* evcwdfl.c  */ 
void wdflinit(SIMCONTL *Simc, ESTL *Estl, TLIST *Tlist);
void wdflinput(WDPT *wp, WDAT *Wd);

/* epsinput.c */
void esondat(FILE *fi, ESTL *Estl);
void esoint(FILE *fi, char *err, int Ntime, ESTL *Estl, TLIST *Tlist);
void fofmt(ESTL *Estl, TLIST *Tlist);
int tmdata(VCFILE *Vcfile, TMDT *Tmdt, DAYTM *Daytm, char perio) ;
void esdatgt(FILE *fi, int i, int Ndata, TLIST *Tlist);

// blsolarwall.c
double	FNScol(double ta, double I, double EffPV, double Ku, double ao, double Eo, double Fs, double RN ) ;
double FNTcoleContrl(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
void	FNBoundarySolarWall(RMSRF *Sd, double *ECG, double *ECt, double *CFc) ;
void  CalcSolarWallTe(RMVLS *Rmvls, WDAT *Wd, EXSFS *Exsfs) ;
double	FNTf(double Tcin, double Tcole, double ECG) ;
double FNSolarWallao(WDAT *Wd, RMSRF *Sd, EXSFS *Exsfs) ;
double VentAirLayerar(double dblEsu, double dblEsd, double dblTsu, double dblTsd) ;
double FNJurgesac(RMSRF *Sd, double dblV, double a, double b) ;
void	FNKc(WDAT *Wd, EXSFS *Exsfs, RMSRF *Sd) ;
void FNTsuTsd(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
double	FNVentAirLayerac(double Tsu, double Tsd, double air_layer_t, double Wb);
void	FNBoundarySolarWall(RMSRF *Sd, double *ECG, double *ECt, double *CFc) ;

void	CalcControlStatus(EQSYS *Eqsys, RMVLS *Rmvls, WDAT *Wd, EXSFS *Exsfs) ;
// 通気層の集熱停止時の熱コンダクタンス[W/m2K]
double	FNVentAirLayeracoff(double Tsu, double Tsd, double air_layer_t, double Wb) ;

// blpvsystem.c
double	FNKPT(double TPV, double apmax) ;
void	PVwallcatinit(PVWALLCAT *PVwallcat) ;
void	PVwallPreCalc(PVWALLCAT *PVwallcat) ;
double	FNTPV(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
void	CalcPowerOutput(int Nsrf, RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
