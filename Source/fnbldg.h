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

/* fnbldg.h  */
/*** bl_files.c ****/


/*  bl_hcflib.c   */
void Htrcf (double *alc, double *alo, char alotype, EXSF *Exs, double Tr, int N, double *alr,
			RMSRF *Sd, char *RMmrk, WDAT *Wd );
void Radcf0 (double Tsav, double *alrbold, int N, RMSRF *Sd,
	     double *W, double *alr);
void radex(int N,  RMSRF *Sd, double *F, double *W);
void formfaprx(int N, double Aroom, RMSRF *Sd, double *F);
void Radshfc(int N, double FArea, double Aroom, RMSRF *Sd, double tfsol, double eqcv, char *Rmname, double *fsol);
double  alcvup(double);
double  alcvdn(double);
double  alcvh(double);

/*  bl_init.c   */
void Walldata(FILE *fi, FILE *fbmlist, char *dsn,
	WALL **Wall, int *Nwall, DFWL *dfwl, PCM *pcm, int Npcm);
void Windowdata (FILE *fi, char *dsn, WINDOW **Window, int *Nwindow);
void Snbkdata (FILE *fi, char *dsn, SNBK **Snbk);
int	BMLSTrealloc ( BMLST **W, unsigned int N, unsigned int NN ) ;
int	WELMrealloc ( WELM **W, unsigned int N, unsigned int NN ) ;
int	WINDOWrealloc ( WINDOW **W, unsigned int N, unsigned int NN ) ;
int	SNBKrealloc ( SNBK **W, unsigned int N, unsigned int NN ) ;
int	WALLrealloc ( WALL **W, unsigned int N, unsigned int NN ) ;
int		wbmlistcount ( FILE *fi ) ;
int		Wallcount ( FILE *fi ) ;


/*  bl_panel.c  */
void panelwp (RDPNL *rdpnl);
void Panelcf (RDPNL *rdpnl);
double Panelce (RDPNL *rdpnl);
int rdpnlldsptr(char *load, char **key, RDPNL *Rdpnl, VPTR *vptr, char *idmrk) ;
void rdpnlldsschd(RDPNL *Rdpnl) ;
int rdpnlvptr(char **key, RDPNL *Rdpnl, VPTR *vptr) ;

/*  bl_wall.c   */
void Walli(int Nbm, BMLST *W, WALL *Wl, PCM *pcm, int Npcm);
void Wallfdc(int M, int mp, double *res, double *cap, double Wp, double *UX,
	double *uo, double *um, double *Pc, char WallType, RMSRF *Sd, WDAT *Wd,
	EXSFS *Exsf, WALL *Wall, double *Told, double *Twd, PCMSTATE *pcmstate);
void Twall(int M, int mp, double *UX, double uo, double um,
	double Pc, double Ti, double To, double WpT, double *Told, double *Tw, RMSRF *Sd, PCM **pcm);
void Twalld(int M, int mp, double *UX, double uo, double um,
	double Pc, double Ti, double To, double WpT, double *Told, double *Tw, RMSRF *Sd);

/*   bl_roomcf.c  */
void eeroomcf(WDAT *Wd, EXSFS *Exs, RMVLS *Rmvls, int nday, int mt);
/*-----higuchi---change 070918 ---
void eeroomcf(WDAT *Wd, EXSFS *Exs, RMVLS *Rmvls);
----------------------------------*/
/*  bl_rmaceqcf.c  */
void Rmhtrcf (EXSFS *Exs,  char *Emrk,  ROOM *Room, RMSRF *Sd, WDAT *Wd); 
void Rmrdshfc(ROOM *Room, RMSRF *Sd);
void Rmhtrsmcf (int Nsrf, RMSRF *Sd);
void Rmexct (ROOM *Room, int Nsrf, RMSRF *Sd, WDAT *Wd, EXSF *Exs, SNBK *Snbk,
	     QRM *Qrm,int nday,int mt); 
/*------higuchi--change 070918 ---
void Rmexct (ROOM *Room, int Nsrf, RMSRF *Sd, WDAT *Wd, EXSF *Exs, SNBK *Snbk,
	     QRM *Qrm);   
	     ----------------------------------------*/  
void Roomcf(int Nmwall, MWALL *Mw, int Nroom, ROOM *Room, 
	                       int Nrdpnl, RDPNL *rdpnl, WDAT *Wd, EXSFS *Exsfs);
void Rmsurft(int Nroom, ROOM *Room, RMSRF *Sd);
void Rmsurftd(int Nroom, ROOM *Room, RMSRF *Sd);
void	Qrmsim (ROOM *Room, WDAT *Wd, QRM *Qrm) ;

/*  bl_rmdata.c  */
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

/*  bl_rmprint.c   */
void Rmsfprint (FILE *fo, char *title,
		int Mon, int Day, double time, ROOM *Room, RMSRF *Sd);
void Rmsfqprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd) ;
void Rmsfaprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd) ;
void Dysfprint(FILE *fo, char *title, int Mon, int Day, ROOM *Room ) ;
void	Shdprint(FILE *fo, char *title, int Mon, int Day, double time,
	int Nsrf, RMSRF *Sd);
void Wallprint(FILE *fo, char *title, int Mon, int Day, double time, 
	      int Nsrf, RMSRF *Sd);
void	PCMprint(FILE *fo, char *title, int Mon, int Day, double time,
	int Nsrf, RMSRF *Sd);
void Qrmprint (FILE *fo, char *title, int Mon, int Day, double time,
	       ROOM *Room, QRM *Qrm);
void Dyqrmprint (FILE *fo, char *title, int Mon, int Day, 
		 ROOM *Room, double *Trdav, QRM *Qrmd);
void Qrmsum (int Day, int Nroom, ROOM *Room, QRM *Qrm, 
	     double *Trdav, QRM *Qrmd);

/*  bl_rmresid.c   */
void Residata (FILE *fi, char *dsn, SCHDL *Schdl, ROOM *Room, int *pmvpri, SIMCONTL *Simc);
void Appldata (FILE *fi, char *dsn, SCHDL *Schdl, ROOM *Room, SIMCONTL *Simc);

/*  bl_rmschd.c   */
void Windowschdlr (char *isw, WINDOW *Window, int N, RMSRF *Sd);
void Qischdlr ( int Nroom, ROOM *Room);
void Vtschdlr ( int Nroom, ROOM *Room);

/*  bl_rmvent.c  */
void Ventdata(FILE *fi, char *dsn, SCHDL *Schdl, ROOM *Room, SIMCONTL *Simc );
void Aichschdlr(double *val, int Nroom, ROOM *Room);

/*  bl_room.c  */
void RMcf(ROOM *Room);
void RMrc (ROOM *Room);
void RMsrt (ROOM *Room);
void RMwlc (int Nmwall, MWALL *Mw, EXSFS *Exsfs, WDAT *Wd);
void RMwlt (int Nmwall, MWALL *Mw);
void RMwltd(int Nmwall, MWALL *Mw);
double  RTsav (int N, RMSRF *Sd);

/*  bl_snbklib.c  */
  double FNFsdw(int Ksdw, int Ksi, double Xazm, double Xprf,
	       double D, double Wr, double Hr, 
	       double Wi1, double Hi1, double Wi2, double Hi2);
  double FNAsdw1(double Da, double Dp, 
		double Wr, double Hr, double Wi1, double Hi, double Wi2);
  double FNAsdw2(double Dp, double Hr, double Wr, double Hi);
  double FNAsdw3(double Da, double Dp,
		double Wr,double Hr, double Wi1, double Hi1, double Wi2, double Hi2);
  
/*  bl_srprint.c  */
void Pmvprint (FILE *fpout, char *title, ROOM *Room, 
	       int Mon, int Day, double time);
void Rmevprint (FILE *fpout, char *title, ROOM *Room,
		int Mon, int Day, double time); 
   
/*   bl_tcomfrt.c    */
void Rmotset (int Nroom, ROOM *Room);
void Fotinit(int Nroom, ROOM *Room);
void Fotf(ROOM *Room);
void Rmcomfrt ( int Nroom, ROOM *Room);
double Pmv0(double met, double Icl, double Tr, double xr, double Tmrt, double v);
double SET_star(double TA, double TR, double VEL, double RH, double MET, double CLO, double WME, double PATM);

/* bl_helm.c */
void Helminit(char *errkey, char helmkey, int Nroom, ROOM *Room, QETOTAL *Qetotal);
void Helmroom( int Nroom, ROOM *Room, QRM *Qrm, QETOTAL *Qetotal, double dTM, double Ta, double xa);
void Helmprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
		int Nroom, ROOM *Room, QETOTAL *Qetotal);
void helmrmprint( FILE *fo, int id, int Nroom, ROOM *Room, QETOTAL *Qetotal);
void Helmsurfprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
		int Nroom, ROOM *Room);
void helmsfprint( FILE *fo, int id, int Nroom, ROOM *Room);
void Helmdy (int day, int Nroom, ROOM *Room, QETOTAL *Qetotal);
void helmdyint ( int Nroom, ROOM *Room, QETOTAL *Qetotal);
void Helmdyprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day,
		int Nroom, ROOM *Room, QETOTAL *Qetotal);
void helmrmdyprint( FILE *fo, int id, int Nroom, ROOM *Room, QETOTAL *Qetotal);

/* bl_helmlib.c */
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

/*  bl_roomvar.c  */
void Roomelm(int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl);
void Roomvar(int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl);

/* bl_roomene.c */
void Roomene(RMVLS *Rmvls, int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl, EXSFS *Exsfs, WDAT *Wd);
void	PCMwlchk(int i, RMVLS *Rmvls, EXSFS *Exsfs, WDAT *Wd, int *LDreset);
void	PCMfunchk(int Nroom, ROOM *Room, WDAT *Wd, int *LDreset);
void Roomload(int Nroom, ROOM *Room, int *LDreset);
void rmqaprint(FILE *fo, int id, int Nroom, ROOM *Room);
void panelprint(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl);

/*  bl_roomvptr.c  */
int roomvptr(int Nk, char **key, ROOM *Room, VPTR *vptr);
int roomldptr(char *load, char **key, ROOM *Room, VPTR *vptr, char *idmrk);
void roomldschd(ROOM *Room);


/* bl_rmqprt.c */
void Rmpnlprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
		int Nroom, ROOM *Room);

/* bl_rzone.c */
void Rzonedata(FILE *fi, char *dsn, int Nroom, ROOM *Room, int *Nrzone, RZONE *Rzone);
void Rzonetotal(int Nrzone, RZONE *Rzone);


/* bl_roomday.c */
void Roomday(int Mon, int Day, int Nday, int ttmm, int Nroom, ROOM *Rm, int Nrdpnl, RDPNL *Rdp, int SimDayend);
void Rmdyprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, int Nroom, ROOM *Rm);
void paneldyprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl);
void panelmonprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl);
void Rmmonprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, int Nroom, ROOM *Rm);
void panelmtprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl, int Mo, int tt) ;

/* dbgpri1.c  */
void xprtwallinit (int Nmwall, MWALL *Mw);
void xprsolrd (int Nexs, EXSF *Exs);
void xpralph (int Nroom, ROOM *Room, RMSRF *S);
void xprxas (int Nroom, ROOM *Room, RMSRF *S);
void xprtwpanel (int Nroom, ROOM *Room, double *Twp, RMSRF *Sd, MWALL *Mw);
void	xprtwsrf ( int N, RMSRF *Sd ) ;
void	xprrmsrf ( int N, RMSRF *Sd ) ;
void	xprtwall ( int Nmwall, MWALL *Mw ) ;

/*  dbgpri2.c   */
void xprroom (int Nroom, ROOM *Room);
void xprschval (int Nsch, double *val, int Nscw, char *isw);
void xprqin (int Nroom, ROOM *Room);
void xprvent (int Nroom, ROOM *Room);

/* dbgprip1.c */
void dprdayweek(int *daywk);
void dprschtable (SEASN *Seasn, WKDY *Wkdy, DSCH *Dsch, DSCW *Dscw);
void dprschdata (SCH *Sch, SCH *Scw);
void dprachv (int Nroom, ROOM *Room);
void dprexsf(EXSF *Exs);
void dprwwdata(WALL *Wall, WINDOW *Window);
void dprroomdata(ROOM *Room, RMSRF *Sd);
void dprballoc (MWALL *Mw, RMSRF *Sd);

void PCMdata(FILE *fi, char *dsn, PCM **pcm, int *Npcm, char *pcmiterate);
int		PCMcount ( FILE *fi ) ;
double	FNPCMState(int Ctype, double Ss, double Sl, double Ql, double Ts, double Tl, double Tp, double T, PCMPARAM PCMp);
double	FNPCMStatefun(int Ctype, double Ss, double Sl, double Ql, double Ts, double Tl, double Tp, double oldT, double T, int DivTemp, PCMPARAM PCMp);
void TableRead(CHARTABLE *ct);
//double FNPCMspcheat_outtemp(CHARTABLE *ct, char flg);
double FNPCMenthalpy_table_lib(CHARTABLE *ct, double T);
double FNPCMstate_table(CHARTABLE *ct, double Told, double T, int Ndiv);
