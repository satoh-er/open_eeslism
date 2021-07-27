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

/* fnlib.h */

#pragma once

#include "simcon.h"
#include "eepath.h"
#include "eqptyp.h"
#include "control.h"
#include "sched.h"
#include "wthrd.h"
#include "eps.h"

/*  ees3lib.c  */
void	HeapCheck(char *s) ;
int		isstrdigit(char *s);
char	*stralloc(char *s);
char	*scalloc(unsigned int n, char *errkey);
int		srealloc ( char *s, unsigned int n ) ;
char	*charalloc(char c);
int		*icalloc(unsigned int n, char *errkey);
int		irealloc ( int **A, unsigned int N, unsigned int NN ) ;
double	*dcalloc(unsigned int n, char *errkey);
double	*dcallocinit(unsigned int n, double init, char *errkey);
int		drealloc ( double **A, unsigned int N, unsigned int NN ) ;
void	Errprint(int err, char *key, char *s);
void	Eprint(char *key, char *s);
void	Ercalloc(unsigned int n, char *errkey);
void	preexit ( void ) ;
double	lineardiv ( double A, double B, double dt ) ;
int		ConvertHour( int ttmm ) ;

/*  inv.c  */
/*--
void    matinv ( double *A, int n, int m) ;
--*/
void	matinv ( double *A, int n, int m, char *s );
void	gausei ( double *A, double *C, int m, int n, double *B ) ;
void	gauss ( double *A, double *C, int m, int n, double *B ) ;
void	matprint(char *fmt, int N, double *a);
void	matfprint(char *fmt, int N, double *a);
void	seqprint(char *fmt, int N, double *a, char *fmt2, double *c);
void	matmalv(double *A, double *V, int N,int n, double *T);
void	matinit ( double *A, int N ) ;
void imatinit(int *A, int N);
void	matinitx ( double *A, int N, double x ) ;
void	matfiprint ( FILE *f, char *fmt, int N, double *a ) ;
void	matcpy ( double *A, double *B, int N ) ;

/*  minmax.c  */
// double min(double a, double b);
// double max(double a, double b);
double	dmin(double a, double b);
double	dmax(double a, double b);
int		imax ( int a, int b ) ;
int		imin ( int a, int b ) ;

/*  mlib.c  */
double	spcheat(char fluid);


/* wdprint.c   */
void Wdtsum(int Mon, int Day, int Nday, int ttmm, WDAT *Wd, int Nexs, EXSF *Exs,
			WDAT *Wdd, WDAT *Wdm, double *Soldy, double *Solmon, SIMCONTL *Simc);
void	Wdtprint(FILE *fo, char *title, int Mon, int Day, double time, WDAT *Wd, EXSFS *Exs);
void	Wdtdprint(FILE *fo, char *title, int Mon, int Day, WDAT *Wdd, int Nexs, EXSF *Exs, double *Soldy);
void	Wdtmprint(FILE *fo, char *title, int Mon, int Day, WDAT *Wdm, int Nexs, EXSF *Exs, double *Solmon);

/*  wreadlib.c   */
void	monthday(int *Mon, int *Day, int mo, int dayo);

void Weatherdt(SIMCONTL *Simc, DAYTM *Daytm, LOCAT *Loc, WDAT *Wd, EXSF *Exs, char EarthSrfFlg);
void	gtsupw (FILE *fp, char *Loc, int *nmx, double *Tgrav, double *DTgr, double *Tsupw);
void	hspwdread(FILE *fp, int nday, LOCAT *Loc, int *year, int *mon, int *day, int *wkdy, double dt[7][25]);
void	dt2wdata ( WDAT *Wd, int tt, double dt[7][25] ) ;
void	WdLinearDiv ( WDAT *Wd, WDAT *WdL, WDAT *WdF, double dt ) ;
void	wdatadiv ( DAYTM *Daytm, WDAT *Wd, double dt[7][25], double dtL[7][25] ) ;
void EarthSrfTempInit(SIMCONTL *Simc, LOCAT *Loc, WDAT *Wd) ;

/* valinit.c  */
void	Simcinit ( SIMCONTL *S ) ;
void	Compinit ( int N, COMPNT *C ) ;
void	Eloutinit ( ELOUT *Eo, int N ) ;
void	Elininit ( int N, ELIN *Ei ) ;
void	Plistinit ( int N, PLIST *Pl ) ;
void	Pelminit ( int N, PELM *Pe ) ;
void	Mpathinit ( int N, MPATH *M ) ;
void	Exsfinit ( int Nd, EXSF *e ) ;
void	Syseqinit ( SYSEQ *S ) ;
void	Eqsysinit ( EQSYS *E ) ;
void	VPTRinit ( VPTR *v ) ;
void	TMDTinit ( TMDT *t ) ;
void	Locinit ( LOCAT	*L ) ;
void	Floutinit ( FLOUT *f ) ;
void	Eqcatinit ( EQCAT *E ) ;
void	Schdlinit ( SCHDL *S ) ;
void	mtEdayinit( EDAY mtEday[12][24] ) ;

// main.c
int	isEndDay(int Mon, int Day, int Dayend, int SimDayend) ;
