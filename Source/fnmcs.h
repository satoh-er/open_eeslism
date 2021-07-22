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

/*  fnmcs.h  */
/*** mc_files.c ****/

/*  mc_aclib.c  */
void wcoil(char Air_SW, char Water_SW, char wet, double Gaet, double Gaeh,
	   double xai, double Twi, ACS *Et, ACS *Ex, ACS *Ew);
double Qcoils(ACS *Et, double Tai, double xai, double Twi);
double Qcoill(ACS *Ex, double Tai, double xai, double Twi);
void hstaircf(double Tw1, double Tw2, double *a, double *b);

/*  mc_reflib.c  */
void Refcmpdat(FILE *frf, int *Nrfcmp, RFCMP *Rfcmp);
void Compca(double *e, double *d, double EGex, double *Teo, double Ta, 
	    double *Ho, double *He);
void Compha(double *e, double *d, double EGex, double *Tco, double Ta,
	    double *Ho, double *He);
double Refpow(REFA *Rf, double QP);

/*  mc_boiler.c  */
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

/*  mc_solrcol.c  */
int Colldata (char *type, char *s, COLLCA *Collca);
void Collint(int Ncoll, COLL *Coll, int Nexsf, EXSF *Exs, WDAT *Wd);
void Collcfv(int Ncoll, COLL *Coll);
void Collene(int Ncoll, COLL *Coll);
void collprint(FILE *fo, int id, int Ncoll, COLL *Coll);
double scolte (double rtgko, double cinc, double Idre, double Idf, double Ta);
int collvptr(char **key, COLL *Coll, VPTR *vptr);
void colldyint(int Ncoll, COLL *Coll);
void collmonint(int Ncoll, COLL *Coll);
void collday(int Mon, int Day, int ttmm, int Ncoll, COLL *Coll, int Nday, int SimDayend);
void colldyprt(FILE *fo, int id, int Ncoll, COLL *Coll);
void collmonprt(FILE *fo, int id, int Ncoll, COLL *Coll);

/*  mcpv.c  */
int PVcadata (char *s, PVCA *PVca) ;
void PVint(int Npv, PV *PV, int Nexsf, EXSF *Exs, WDAT *Wd) ;
void PVene(int Npv, PV *PV) ;
void PVprint(FILE *fo, int id, int Npv, PV *PV) ;
void PVdyint(int Npv, PV *PV) ;
void PVmonint(int Npv, PV *PV) ;
void PVday(int Mon, int Day, int ttmm, int Npv, PV *PV, int Nday, int SimDayend) ;
void PVdyprt(FILE *fo, int id, int Npv, PV *PV) ;
void PVmonprt(FILE *fo, int id, int Npv, PV *PV) ;
void PVmtprt(FILE *fo, int id, int Npv, PV *PV, int Mo, int tt);

/*  mc_refas.c */
int Refadata (char *s, REFACA *Refaca, int Nrfcmp, RFCMP *Rfcmp);
void Refaint(int Nrefa, REFA *Refa, WDAT *Wd, int Ncompnt, COMPNT *Compnt);
void Refacfv(int Nrefa, REFA *Refa);
void refacoeff(REFA *Refa, int *err);
void Refaene(int Nrefa, REFA *Refa, int *LDreset); 
void Refaene2(int Nrefa, REFA *Refa);
int refaldptr(char *load, char **key, REFA *Refa, VPTR *vptr);
int refaswptr(char **key, REFA *Refa, VPTR *vptr);
void refaldschd(REFA *Refa);
void refaprint(FILE *fo, int id, int Nrefa, REFA *Refa);
void refadyint(int Nrefa, REFA *Refa);
void refamonint(int Nrefa, REFA *Refa);
void refaday(int Mon, int Day, int ttmm, int Nrefa, REFA *Refa, int Nday, int SimDayend);
void refadyprt(FILE *fo, int id, int Nrefa, REFA *Refa);
void refamonprt(FILE *fo, int id, int Nrefa, REFA *Refa);
void refamtprt(FILE *fo, int id, int Nrefa, REFA *Refa, int Mo, int tt) ;

/*  mc_hccoil.c  */
int Hccdata(char *s, HCCCA *Hccca);
void Hccdwint(int Nhcc, HCC *Hcc);
void Hcccfv(int Nhcc, HCC *Hcc);
void Hccdwreset(int Nhcc, HCC *Hcc, int *DWreset);
void Hccene(int Nhcc, HCC *Hcc);
void hccprint(FILE *fo, int id, int Nhcc, HCC *Hcc);
void hccdyint(int Nhcc, HCC *Hcc);
void hccmonint(int Nhcc, HCC *Hcc);
void hccday(int Mon, int Day, int ttmm, int Nhcc, HCC *Hcc, int Nday, int SimDayend);
void hccdyprt(FILE *fo, int id, int Nhcc, HCC *Hcc);
void hccmonprt(FILE *fo, int id, int Nhcc, HCC *Hcc);
double	FNhccet ( double Wa, double Ww, double KA ) ;


/*  mc_pipe.c  */
int Pipedata(char *cattype, char *s, PIPECA *Pipeca);
void Pipeint(int Npipe, PIPE *Pipe, 
	     SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd);
void Pipecfv(int Npipe, PIPE *Pipe);
void Pipeene(int Npipe, PIPE *Pipe);
int pipeldsptr(char *load, char **key, PIPE *Pipe, VPTR *vptr, char *idmrk);
void pipeldsschd(PIPE *Pipe);
void pipeprint(FILE *fo, int id, int Npipe, PIPE *Pipe);
void pipedyint(int Npipe, PIPE *Pipe);
void pipemonint(int Npipe, PIPE *Pipe);
void pipeday(int Mon, int Day, int ttmm, int Npipe, PIPE *Pipe, int Nday, int SimDayend);
void pipedyprt(FILE *fo, int id, int Npipe, PIPE *Pipe);
void pipemonprt(FILE *fo, int id, int Npipe, PIPE *Pipe);
int pipevptr(char **key, PIPE *Pipe, VPTR *vptr) ;

/*  mc_stank.c */
int Stankdata(FILE *f, char *s, STANKCA *Stankca);
void Stankmemloc(char *errkey, STANK *Stank);
void Stankint(double dTM, int Nstank, STANK *Stank, 
	      SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd);
void Stankcfv(int Nstank, STANK *Stank ) ;
int stankvptr(char **key, STANK *Stank, VPTR *vptr);

void Stanktss(int Nstank, STANK *Stank, int *TKreset);
void Stankene(int Nstank, STANK *Stank);
void stankcmpprt (FILE *fo, int id, int Nstank, STANK *Stank);
void stankivprt (FILE *fo, int id, int Nstank, STANK *Stank);
void stankdyint(int Nstank, STANK *Stank);
void stankmonint(int Nstank, STANK *Stank);
void stankday(int Mon, int Day, int ttmm, int Nstank, STANK *Stank, int Nday, int SimDayend);
void stankdyprt(FILE *fo, int id, int Nstank, STANK *Stank);
void stankmonprt(FILE *fo, int id, int Nstank, STANK *Stank);


/* mc_stanklb.c  */
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

/* mc_hexchgr.c */
int Hexdata(char *s, HEXCA *Hexca);
void Hexcfv(int Nhex, HEX *Hex);
void Hexene(int Nhex, HEX *Hex);
void hexprint(FILE *fo, int id, int Nhex, HEX *Hex);
void hexdyint(int Nhex, HEX *Hex);
void hexmonint(int Nhex, HEX *Hex);
void hexday(int Mon, int Day, int ttmm, int Nhex, HEX *Hex, int Nday, int SimDayend);
void hexdyprt(FILE *fo, int id, int Nhex, HEX *Hex);
void hexmonprt(FILE *fo, int id, int Nhex, HEX *Hex);

/* mcpump.c */
int Pumpdata (char *cattype, char *s, PUMPCA *Pumpca, int Npfcmp, PFCMP *pfcmp) ;
void Pumpint(int Npump, PUMP *Pump, int Nexsf, EXSF *Exs);
void Pumpflow(int Npump, PUMP *Pump);
void Pumpcfv(int Npump, PUMP *Pump);
double	PumpFanPLC ( double XQ, PUMP *Pump ) ;
void Pumpene(int Npump, PUMP *Pump);
void pumpprint(FILE *fo, int id, int Npump, PUMP *Pump);
void pumpdyint(int Npump, PUMP *Pump);
void pumpmonint(int Npump, PUMP *Pump);
void pumpday(int Mon, int Day, int ttmm, int Npump, PUMP *Pump, int Nday, int SimDayend);
void pumpdyprt(FILE *fo, int id, int Npump, PUMP *Pump);
void pumpmonprt(FILE *fo, int id, int Npump, PUMP *Pump);
void pumpmtprt(FILE *fo, int id, int Npump, PUMP *Pump, int Mo, int tt) ;
void	PFcmpInit(int Npfcmp, PFCMP *Pfcmp) ;
void	PFcmpdata(FILE *fl, int *Npfcmp, PFCMP *pfcmp) ;

/*  hcload.c  */
void Hclelm(int Nhcload, HCLOAD *Hcload);
void Hcldcfv(int Nhcload, HCLOAD *Hcload);
void Hcldene(int Nhcload, HCLOAD *Hcload, int *LDrest, WDAT *Wd);
int  hcldptr(char *load, char **key, HCLOAD *Hcload, VPTR *vptr, char *idmrk);
void hcldschd(HCLOAD *Hcload);
void hcldprint(FILE *fo, int id, int Nhcload, HCLOAD *Hcload);
void hclddyint(int Nhcload, HCLOAD *Hcload);
void hcldmonint(int Nhcload, HCLOAD *Hcload);
void hcldday(int Mon, int Day, int ttmm, int Nhcload, HCLOAD *Hcload, int Nday, int SimDayend);
void hclddyprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload);
void hcldmonprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload);
void hcldmtprt(FILE *fo, int id, int Nhcload, HCLOAD *Hcload, int Mo, int tt);
int  hcldswptr (char **key, HCLOAD *Hcload, VPTR *vptr) ;
int  chhcldswreset(double Qload, double Ql, char chmode, ELOUT *Eo) ;
void	hcldwetmdreset ( EQSYS *Eqsys ) ;
void	hcldmodeinit ( EQSYS *Eqsys ) ;
double	FNeffthc(double Tco, double Tci, double xci, double Qc, double Ec, double Go, double Gi, double cai, double cao) ;
double	FNeffthh(double Tco, double Tci, double xco, double Qc, double Eh, double Go, double Gi, double cai, double cao) ;
void rmacdat(HCLOAD *Hcld) ;
void rmacddat(HCLOAD *Hcld) ;
double	FNTcndc(double Tco, double Qc, double Ec, double cao, double Go) ;
double	FNTevpc(double Tci, double Qc, double cai, double Gi, double xci) ;
double	FNTcndh(double Tci, double Qc, double cai, double Gi) ;
double	FNTevph(double Tco, double Qc, double Eh, double cao, double Go, double xco) ;

/* mcvalv.c */
void	Valvcountreset ( int Nvalv, VALV *Valv ) ;
void	Valvcountinc ( int Nvalv, VALV *Valv ) ;
void	Valvene ( int Nvalv, VALV *Valv, int *Valvreset ) ;
void	ValvControl ( FILE *fi,  int Ncompnt, COMPNT *Compnt, SCHDL *Schdl, SIMCONTL *Simc,
					 WDAT *Wd, VPTR *vptr ) ;
void	Valvinit ( int NValv, VALV *Valv, int NMpath, MPATH *Mpath ) ;

/* mcvav.c */
int VAVdata (char *s, char *ss, VAVCA *vavca) ;
void VAVcfv(int Nvav, VAV *vav) ;
void VAVene(int Nvav, VAV *vav, int *VAVrest) ;
void	VAVcountreset ( int Nvav, VAV *VAV ) ;
int		vavswptr(char **key, VAV *VAV, VPTR *vptr) ;
int chvavswreset(double Qload, char chmode, VAV *VAV) ;
void vavprint(FILE *fo, int id, int Nvav, VAV *VAV) ;
void	VAVcountinc ( int Nvav, VAV *VAV ) ;
void VWVint(int Nvav, VAV *VAV, int Ncompnt, COMPNT *Compn) ;
double	FNVWVG ( VAV *VWV ) ;
double FNFd ( double Wa, double Ww, double KA ) ;

// mcomvav.c
int OMVAVdata (char *ss, OMVAVCA *OMvavca) ;
double	OMflowcalc(OMVAV *OMvav, WDAT *Wd) ;
int colkey(char *s, char **key) ;
void	OMvavControl(OMVAV *OMvav, COMPNT *Compnt, int NCompnt) ;
int		strCompcount(char *st, char key) ;

/* mcstheat.c */
int Stheatdata(char *s, STHEATCA *stheatca) ;
void Stheatint(int Nstheat, STHEAT *stheat, 
	SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd, int Npcm, PCM *PCM);
void Stheatcfv(int Nstheat, STHEAT *stheat) ;
void Stheatene(int Nstheat, STHEAT *stheat) ;
//void Stheatene2(int Nstheat, STHEAT *stheat);
int	stheatvptr(char **key, STHEAT *Stheat, VPTR *vptr, VPTR *vpath) ;
void stheatprint(FILE *fo, int id, int Nstheat, STHEAT *stheat) ;
void stheatdyint(int Nstheat, STHEAT *stheat) ;
void stheatmonint(int Nstheat, STHEAT *stheat) ;
void stheatday(int Mon, int Day, int ttmm, int Nstheat, STHEAT *stheat, int Nday, int SimDayend) ;
void stheatdyprt(FILE *fo, int id, int Nstheat, STHEAT *stheat) ;
void stheatmonprt(FILE *fo, int id, int Nstheat, STHEAT *stheat) ;
void stheatmtprt(FILE *fo, int id, int Nstheat, STHEAT *stheat, int Mo, int tt) ;

/* mcthex.c */
int		Thexdata ( char *s, THEXCA *Thexca ) ;
void	Thexint ( int Nthex, THEX *Thex ) ;
void	Thexcfv ( int Nthex, THEX *Thex ) ;
void	Thexelm(int NThex, THEX *Thex) ;
void	Thexene ( int Nthex, THEX *Thex ) ;
void	Thexprint(FILE *fo, int id, int Nthex, THEX *Thex ) ;
void	Thexdyint(int Nthex, THEX *Thex) ;
void	Thexmonint(int Nthex, THEX *Thex) ;
void	Thexday(int Mon, int Day, int ttmm, int Nthex, THEX *Thex, int Nday, int SimDayend) ;
void	Thexdyprt(FILE *fo, int id, int Nthex, THEX *Thex) ;
void	Thexmonprt(FILE *fo, int id, int Nthex, THEX *Thex) ;

/* mc_cnvrg.c  */
void Cnvrgcfv(int Ncnvrg, COMPNT **Cnvrg);

/*  mc_eqcadat.c  */
void Eqcadata (FILE *f, char *dsn, EQCAT *Eqcat);
void	Eqpcount ( FILE *fi, int *NBOI, int *NREFA, int *NCOL, int *NSTANK, int *NHCC,
				  int *NHEX, int *NPIPE, int *NPUMP, int *NVAV, int *NSTHEAT, int *NTHEX, int *NPV, int *NOMVAV, int *NDESI, int *NEVAC ) ;
int	pflistcount ( FILE *fl ) ;

/*  mc_eqpcat.c  */
int eqpcat(char *catname, COMPNT *C, EQCAT *Ecat, EQSYS *Esys);


/*  mc_mecsys.c  */ 
void Mecsinit(double dTM, EQSYS *Eqsys,
	      SIMCONTL *Simc, int Ncompnt,COMPNT *Compnt,
		  int Nexsf, EXSF *Exsf, WDAT *Wd, RMVLS *Rmvls);
void	Mecscf(EQSYS *Eqsys);
void Mecsene(EQSYS *Eqsys);


/* mc_xprmcs.c */
void mecsxprint(EQSYS *Eqsys);
void boixprint(int Nboi, BOI *Boi);
void hccxprint(int Nhcc, HCC *Hcc);
void pipexprint(int Npipe, PIPE *Pipe);
void refaxprint(int Nrefa, REFA *Refa);
void collxprint(int Ncoll, COLL *Coll);

/* mc_qmeas.c */
void	Qmeasene (int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeaselm ( int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasprint(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasdyint(int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasmonint(int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasday(int Mon, int Day, int ttmm, int Nqmeas, QMEAS *Qmeas, int Nday, int SimDayend) ;
void	Qmeasdyprt(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas) ;
void	Qmeasmonprt(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas) ;

// mcdesiccant.c  Satoh追加 2013/10/20
int Desiccantdata(char *s, DESICA *Desica) ;
void Desiint(int NDesi, DESI *Desi, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd) ;
void Desicfv(int NDesi, DESI *Desi) ;
void Desiene(int NDesi, DESI *Desi) ;
int	Desivptr(char **key, DESI *Desi, VPTR *vptr) ;
void Desielm(int Ndesi, DESI *Desi) ;
void Desiprint(FILE *fo, int id, int Ndesi, DESI *Desi) ;
void Desidyprt(FILE *fo, int id, int Ndesi, DESI *Desi) ;
void Desiday(int Mon, int Day, int ttmm, int Ndesi, DESI *Desi, int Nday, int SimDayend) ;
void Desidyint(int Ndesi, DESI *Desi) ;


// mcevcooling.c Satoh追加 2013/10/26
int		Evacdata ( char *s, EVACCA *Evacca ) ;
void	Evacint ( int Nevac, EVAC *Evac ) ;
void	LinearSatx(double Ts, double *a, double *b) ;
void	Evacelm(int Nevac, EVAC *Evac) ;
void	Evacelm(int Nevac, EVAC *Evac) ;
void	Evaccfv(int Nevac, EVAC *Evac) ;
void	Evacene(int Nevac, EVAC *Evac, int *Evacreset) ;
void Evacprint(FILE *fo, int id, int Nevac, EVAC *Evac) ;
void	Evaccountreset(int Nevac, EVAC *Evac) ;
double	Evacu(double G, double T, double H, double W, int N) ;
double Evachcc(double de, double L, double T, double H, double W, int N, double G, int Flg) ;
double	EvacNu(double AR, double Re) ;
