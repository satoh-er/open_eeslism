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

/*   schdata.c  */


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "esize.h"
#include "lib/u_sun.h"
#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"
#include "winerror.h"

/* 曜日の設定  */

void Dayweek(FILE *fi, char *Ipath, int *daywk, int key)
{
	extern char DAYweek[][4];
	char  s[SCHAR], *ce;
	int   ds, de, dd, d, id=0, M, D;
	FILE	 *fw ;
	
	strcpy_s(s, sizeof(s), Ipath);
	strcat_s(s, sizeof(s), "week.ewk");
	if (fopen_s (&fw, s, "r" ) != 0 )
	{
		Eprint ( "<Dayweek>", "week.ewk" ) ;

		preexit ( ) ;
		exit (EXIT_WEEK) ;
	}
	
	if ( key == 0 )
		fscanf_s(fi, "%d/%d=%s", &M, &D, s, sizeof(s));
	else
	{
		fscanf_s ( fi, "%*s" ) ;
		
		fscanf_s ( fw, "%d/%d=%s", &M, &D, s, sizeof(s));
	}
	
	for ( d = 0; d < 8; d++ )
	{
		if ( strcmp ( s, DAYweek[d]) == 0 )
			id = d;
	}
	if ( id == 8 )
		Eprint ( "<Dayweek>", s ) ;
	
	ds = FNNday ( M, D ) ;
	de = ds + 365;
	
	for ( dd = ds; dd < de; dd++ )
	{
		d = ( dd > 365 ? dd - 365 : dd ) ;
		daywk[d] = id ;
		id++ ;

		if (id > 6 )
			id = 0;
	}
	while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != ';')
	{
		if ((ce=strchr(s, ';')) != 0)
			*ce = '\0';
		if (strchr(s,'/') != 0)
		{
			sscanf_s(s, "%d/%d", &M, &D);
			d = FNNday(M, D);
			daywk[d] = 7;
		}
		if (ce) break;
	}
	
	fclose ( fw ) ;
}

/* ------------------------------------------------------------ */

/*  スケジュ－ル表の入力          */


void Schtable (FILE *fi, char *dsn, SCHDL *Schdl )
{ 
	char		s[SCHAR], *ce, code, E[SCHAR];
	extern char	DAYweek[][4];
	static int	is = -1, js, iw = -1, j, sc = -1, jsc, sw = -1, jsw, Nmod = 0 ;
	int			Ms, Ds, Me, De, *wday ;
	int			ssn, wkd, vl, swn, N, i ;
	static int	ic = 0 ;
	int			ssnmx, vlmx, swmx ;
	SEASN		*Seasn, *Sn ;
	WKDY		*Wkdy, *Wk ;
	DSCH		*Dsch, *Dh ;
	DSCW		*Dscw, *Dw ;
	
	Seasn = Sn = NULL ;
	Wkdy = Wk = NULL ;
	Dsch = Dh = NULL ;
	Dscw = Dw = NULL ;

	sprintf_s (E, sizeof(E), ERRFMT, dsn);

	if ( ic == 0 )
	{
		Schdl->Dsch = NULL ;
		Schdl->Dscw = NULL ;
		Schdl->Seasn = NULL ;
		Schdl->Wkdy = NULL ;

		SchCount ( fi, &ssn, &wkd, &vl, &swn, &ssnmx, &vlmx, &swmx ) ;
		ssn++ ;
		wkd++ ;
		vl++ ;
		swn++ ;
		ssnmx++ ;
		vlmx++ ;
		swmx++ ;

		N = imax(1,ssn) ;
		if (N > 0 && ( Schdl->Seasn = ( SEASN * ) malloc(N * sizeof(SEASN))) == NULL)
			Ercalloc(ssn, "<Schtable> Seasn" ) ;

		if ( Schdl->Seasn != NULL )
		{
			Seasn = Schdl->Seasn ;

			for ( i = 0; i < N; i++, Seasn++ )
			{
				Seasn->name = NULL ;
				Seasn->N = Seasn->end = 0 ;

				Seasn->sday = icalloc ( ssnmx, "<Schtable>  Seasn->sday  alloc" ) ;
				Seasn->eday = icalloc ( ssnmx, "<Schtable>  Seasn->eday  alloc" ) ;
			}
		}

		N = imax(1,wkd) ;
		if (N > 0 && ( Schdl->Wkdy = ( WKDY * ) malloc(N * sizeof(WKDY))) == NULL)
			Ercalloc(wkd, "<Schtable> Wkdy" ) ;

		if ( Schdl->Wkdy != NULL )
		{
			Wkdy = Schdl->Wkdy ;

			for ( i = 0; i < N; i++, Wkdy++ )
			{
				Wkdy->name = NULL ;
				Wkdy->end = 0 ;
				
				wday = Wkdy->wday ;
				for ( j = 0; j < 8; j++, wday++ )
					*wday = 0 ;
			}
		}

		N = imax(1,vl) ;
		if (N > 0 && ( Schdl->Dsch = ( DSCH * ) malloc(N * sizeof(DSCH))) == NULL)
			Ercalloc(vl, "<Schtable> Dsch" ) ;

		if ( Schdl->Dsch != NULL )
		{
			Dsch = Schdl->Dsch ;

			for ( i = 0; i < N; i++, Dsch++ )
			{
				Dsch->name = NULL ;
				Dsch->N = Dsch->end = 0 ;
				
				Dsch->stime = icalloc ( vlmx, "<Schtable>  Dsch->stime  alloc" ) ;
				Dsch->etime = icalloc ( vlmx, "<Schtable>  Dsch->etime  alloc" ) ;
				Dsch->val = dcalloc ( vlmx, "<Schtable>  Dsch->val  alloc" ) ;
			}
		}

		N = imax(1,swn) ;
		if (N > 0 && ( Schdl->Dscw = ( DSCW * ) malloc(N * sizeof(DSCW))) == NULL)
			Ercalloc(swn, "<Schtable> Dscw" ) ;

		if ( Schdl->Dscw != NULL )
		{
			Dscw = Schdl->Dscw ;

			for ( i = 0; i < N; i++, Dscw++ )
			{
				Dscw->name = NULL ;
				Dscw->N = Dscw->end = Dscw->Nmod = 0 ;
				
				Dscw->stime = icalloc ( swmx, "<Schtable>  Dsch->stime  alloc" ) ;
				Dscw->etime = icalloc ( swmx, "<Schtable>  Dsch->etime  alloc" ) ;
				Dscw->mode = scalloc ( swmx, "<Schtable>  Dsch->mode  alloc" ) ;
			}
		}
		ic = 1 ;
	}
	
	Seasn = Schdl->Seasn ;
	Wkdy = Schdl->Wkdy ;
	Dsch = Schdl->Dsch ;
	Dscw = Schdl->Dscw ;
	
	while (fscanf_s(fi,"%s", s, sizeof(s)), s[0] != '*')
	{  
		if (strcmp(s, "-ssn") == 0 || strcmp(s, "SSN") == 0)
		{
			while (fscanf_s(fi,"%s", s, sizeof(s)), s[0] != ';')
			{
				if ((ce=strchr(s,';')) != 0)
					*ce='\0';
				
				if (strchr(s,'-') == 0)
				{
					is++ ;
					Sn = Seasn + is ;
					Sn->name = stralloc( s);
					js= -1;
				}
				else
				{ 
					sscanf_s(s, "%d/%d-%d/%d", &Ms, &Ds, &Me, &De); 
					js++ ;
					Sn->sday[js] = FNNday(Ms, Ds);
					Sn->eday[js] = FNNday(Me, De);
				}
				if (ce) break;
			}
			Sn->N= js+1;
		}
		else if (strcmp(s, "-wkd") == 0 || strcmp(s, "WKD") == 0)
		{
			j=9;
			while (fscanf_s(fi,"%s", s, sizeof(s)), s[0] != ';')
			{ 
				if ((ce=strchr(s,';')) != 0)
					*ce='\0';
				
				if (j == 9)
				{
					iw++ ;
					Wk = Wkdy + iw ;
					Wk->name = stralloc( s);
					j = 0 ;
				} 
				else
				{
					wday = Wk->wday ;
					for ( j = 0; j < 8; j++, wday++ )
						if ( strcmp ( s, DAYweek[j] ) == 0 )
						{
							*wday = 1 ;
							break;
						}
				}
				if (ce) break;
			}
		}
		else if (strcmp(s, "-v") == 0 || strcmp(s, "VL") == 0)
		{
			while (fscanf_s(fi,"%s", s, sizeof(s)), s[0] != ';')
			{
				if ((ce=strchr(s,';')) != 0)
					*ce='\0';
				
				if (strchr(s,'(') == 0)
				{
					sc++ ;
					Dh = Dsch + sc ;
					Dh->name = stralloc ( s);
					/****************************
					printf ("<Schtable> [%d]name=%s\n"
						, sc, (Dsch+sc)->name) ;
						***********************/
					jsc = -1 ;
				}
				else
				{
					jsc++;
					
//					if ( jsc > SCDAYTMMAX )
//						printf ("<Schtable> Name=%s  MAX=%d  jsc=%d\n",
//						Dh->name, SCDAYTMMAX, jsc) ;

					sscanf_s(s,"%d-(%lf)-%d", &Dh->stime[jsc], &Dh->val[jsc], &Dh->etime[jsc] ) ;
				}
				if (ce) break;
			} 
			Dh->N = ++jsc ;
		}
		else if (strcmp(s, "-s") == 0 || strcmp(s, "SW") == 0)
		{ 
			Nmod = 0;
			fscanf_s(fi, " %s ", s, sizeof(s));

			sw++ ;
			Dw = Dscw + sw ;
			Dw->name = stralloc ( s);
			jsw= -1; 
			
			while (fscanf_s(fi,"%s", s, sizeof(s)), s[0] != ';')
			{ 
				if ((ce=strchr(s,';')) != 0)
					*ce='\0';
				
				jsw++;
				sscanf_s(s,"%d-(%c)-%d", &Dw->stime[jsw], &code, 1, &Dw->etime[jsw]);
				Dw->mode[jsw] = code ;
				
				for ( j = 0; j < Nmod; j++)
				{
					if ( Dw->dcode[j] == code )
						break;
				}
				
				if ( j == Nmod )
				{
					Dw->dcode[Nmod] = code ;
					Nmod++ ;  
				}
				
				if (ce) break;
			}
			Dw->N = ++jsw ;
			Dw->Nmod = Nmod ;
		}
	}
	Schdl->Seasn->end = is + 1 ;
//	if (is > SEASNMX )
//		printf("%s SSN=%d [max=%d]\n", E, is, SEASNMX);
	Schdl->Wkdy->end= iw + 1 ;
//	if (iw >WKDYMX)
//		printf("%s WKD=%d [max=%d]\n", E, iw, WKDYMX);
	Schdl->Dsch->end= sc + 1 ;
//	if (sc > DSCHMX)
//		printf("%s VL=%d [max=%d]\n", E, sc, DSCHMX);   
	Schdl->Dscw->end= sw + 1 ;
//	if (sw > DSCHMX)
//		printf("%s SW=%d [max=%d]\n", E, sw, DSCHMX);  
}
/* ------------------------------------------------------------ */

/*  季節、曜日によるスケジュ－ル表の組み合わせ    */


void Schdata (FILE *fi, char *dsn, int *daywk, SCHDL *Schdl )
{
	char	s[SCHAR], ss[SCHAR], dmod, *ce, sname[SCHAR],wname[SCHAR],dname[SCHAR];
	char	err[SCHAR], E[SCHAR] ; 
	int		i, j, k, N, d, ds, de, day, is, iw, sc, sw ;
	int		dmax = 366, *day1 ;
//	int		sws, vl ;
	static int	ic = 0 ;
	SEASN	*Seasn, *Sn ;
	WKDY	*Wkdy ;
	DSCH	*Dsch ;
	DSCW	*Dscw ;
	SCH		*Sch, *Scw, *S ;
	
	Seasn = Schdl->Seasn ;
	Wkdy = Schdl->Wkdy ;
	Dsch = Schdl->Dsch ;
	Dscw = Schdl->Dscw ;
	Sch	= Schdl->Sch ;
	Scw = Schdl->Scw ;

//	SchCount ( fi, &i, &i, &vl, &sws ) ;

	sprintf_s (E, sizeof(E), ERRFMT, dsn);
	i = Sch->end - 1;
	j = Scw->end - 1;
	
	while (fscanf_s(fi,"%s", s, sizeof(s)), s[0] != '*')
	{   
		if (strcmp(s, "-v") == 0 || strcmp(s, "VL") == 0)
			dmod = 'v';
		else
			dmod = 'w';
		
		fscanf_s(fi,"%s", s, sizeof(s));
		
		if (dmod == 'v')
		{
			i++ ;
			S = Sch + i ;
			S->name = stralloc( s);

			day1 = S->day ;
			for ( d = 0; d < dmax; d++, day1++ )
				*day1 = -1;
		}
		else 
		{
			j++ ;
			S = Scw + j ;
			S->name = stralloc( s);

			day1 = S->day ;
			for ( d = 0; d < dmax; d++, day1++ )
				*day1 = - 1 ;
		}
		
		while (fscanf_s(fi,"%s", s, sizeof(s)),  *s != ';')
		{
			if (( ce = strchr ( s, ';' )) != 0 )
				*ce = '\0' ;  
			*sname = *wname = '\0' ;
			is = iw = sc = sw = -1 ;
			
			sscanf_s ( s, "%[^:]:%s", dname, sizeof(dname), ss, sizeof(ss));
			if ( strchr ( ss, '-') == 0 )
				sscanf_s ( ss, "%s", sname, sizeof(sname) ) ;
			else
			{
				if ( *ss == '-' )
					sscanf_s ( &ss[1], "%s", wname, sizeof(wname));
				else
					sscanf_s(ss, "%[^-]-%s", sname, sizeof(sname), wname, sizeof(wname));
			}
			
			/************************ 
			if ((st=strchr(s,'-')) == s)
			sscanf(s, "-%[^-]-%s", wname, dname);
			else if (*(st+1) == '-')
			sscanf(s, "%[^-]--%s", sname, dname);
			else 
			sscanf(s, "%[^-]-%[^-]-%s", sname, wname, dname);
			*****************/
			/*******************
			printf("-- dname:s,w,= %s  %s  %s\n",dname,sname,wname);
			********************/
			
			if (sname[0] != '\0') {
				strcpy_s(err, sizeof(err), E);
				strcat_s(err, sizeof(err), sname);
				is= idssn(sname, Seasn, err);
			}
			if (wname[0] != '\0') {
				strcpy_s(err, sizeof(err), E);
				strcat_s(err, sizeof(err), wname);
				iw= idwkd(wname, Wkdy, err);
			}
			if (dname[0] != '\0')
			{
				if (dmod == 'v') {
					strcpy_s(err, sizeof(err), E);
					strcat_s(err, sizeof(err), dname);
					sc= iddsc(dname, Dsch, err);
				}
				else {
					strcpy_s(err, sizeof(err), E);
					strcat_s(err, sizeof(err), dname);
					sw= iddsw(dname, Dscw, err);
				}
			}
			if ( is >= 0 )
				N = (Seasn+is)->N ;
			else
				N = 1 ;

			for ( k = 0; k < N; k++ )
			{ 
				if (is >= 0)
				{
					Sn = Seasn + is ;
					ds = Sn->sday[k] ;
					de = Sn->eday[k] ;

					if ( ds > de )
						de += 365 ;
				}
				else
					ds = 1, de = dmax ;
				
				for ( day = ds; day <= de; day++ )
				{ 
					d = (day > 365 ? day-365 : day) ;

					if (iw < 0 || (Wkdy+iw)->wday[daywk[d]] == 1)
					{
						if (dmod == 'v')
							S->day[d] = sc ;
						else
							S->day[d] = sw ;
					}
				}
			}
			if (ce) break;
		}
	}
	
	Schdl->Sch->end = i + 1 ;
	
//	if (i > NSCHMX)
//		printf("%s -v=%d [max=%d]\n", E, i, NSCHMX);
	
	Schdl->Scw->end = j + 1 ;
//	if (j > NSCHMX)
//		printf("%s -s=%d [max=%d]\n", E, j, NSCHMX);
}

/* ------------------------------------------------------------ */

/*  季節、曜日によるスケジュ－ル表の組み合わせ名へのスケジュ－ル名の追加  */


void Schname (char *Ipath, char *dsn, SCHDL *Schdl )
{
	int		sc, sw, d,  i,  j, N ;
	static int	ind=0, sco= 0, swo= 0;
	char	E[SCHAR], s[SCHAR]; 
	int		dmax = 366, *day ;
	int		vl, sws ;
	SCH		*Sch, *Scw ;
	DSCH	*Dsch ;
	DSCW	*Dscw ;
	FILE	*fi ;
	int		ssnmx, vlmx, swmx ;

	strcpy_s(s, sizeof(s), Ipath);
	strcat_s(s, sizeof(s), "schnma.ewk");
	if (fopen_s (&fi, s, "r" ) != 0 )
	{
		Eprint ( "<Schname>", "schnma.ewk" ) ;

		preexit ( ) ;
		exit(EXIT_SCHTB);
	}

	SchCount ( fi, &i, &j, &vl, &sws, &ssnmx, &vlmx, &swmx ) ;
	/*****************/
	vl++ ;
	sws++ ;
	ssnmx++ ;
	vlmx++ ;
	swmx++ ;
	/***********************/

	fclose ( fi ) ;

	Dsch = Schdl->Dsch ;
	Dscw = Schdl->Dscw ;

	if (ind == 0)
	{
		Schdl->Sch = NULL ;
		Schdl->Scw = NULL ;

		N = imax (Dsch->end + vl,1) ;
		Schdl->Sch = ( SCH * ) malloc ( N * sizeof(SCH)) ;
		if ( Schdl->Sch == NULL )
			Ercalloc(N, "<Schname> Sch" ) ;
		Sch = Schdl->Sch ;

		for ( i = 0; i < N; i++, Sch++ )
		{
			Sch->name = NULL ;
			Sch->end = 0 ;
			day = Sch->day ;
			for (j=0; j < 366; j++, day++)
				*day = 0 ;
		}

		N = imax ( Dscw->end + sws, 1) ;
		if (( Schdl->Scw = ( SCH * ) malloc( N * sizeof(SCH))) == NULL)
			Ercalloc(N, "<Schname> Scw" ) ;
		Scw = Schdl->Scw ;
		
		for (i=0; i<N; i++, Scw++)
		{
			Scw->name = NULL ;
			Scw->end = 0 ;
			day = Scw->day ;
			for (j=0; j < 366; j++, day++ )
				*day = 0 ;
		}

		ind=1;
	}


	i = Schdl->Sch->end ;

	N = Dsch->end ;
	Sch = Schdl->Sch + i ;

	sprintf_s (E, sizeof(E), ERRFMT, dsn);

	for ( sc = sco; sc < N; sc++, Sch++ )
	{
		i++ ;
		Sch->name = stralloc ( (Dsch+sc)->name ) ;

		/****************************
		printf ("<Schname> MAX=%d sc=%d  Sname=%s Dname=%s NSCHMX=%d(%d)\n",
			N, sc, Sch->name, (Dsch+sc)->name,
			NSCHMX, i ) ;
			******************************/

		day = Sch->day ;
		for ( d = 0; d < dmax; d++, day++ )
			*day = sc ;
	}
	sco = sc ;
	Schdl->Sch->end = i ;
//	if (i > NSCHMX)
//		printf("%s  -v=%d [max=%d]\n", E, i, NSCHMX);
	
	j = Schdl->Scw->end ;
	Scw = Schdl->Scw + j ;
	N = Dscw->end ;
	for ( sw = swo; sw < N; sw++, Scw++ )
	{
		j++ ;
		Scw->name = stralloc ( (Dscw+sw)->name );

		day = Scw->day ;
		for ( d = 0; d < dmax; d++, day++ )
			*day = sw ;
	}
	swo=sw;
	Schdl->Scw->end = j;
//	if (j > NSCHMX)
//		printf("%s  -s=%d [max=%d]\n", E, j, NSCHMX);   
}

/****  スケジュールの数を数える  ****/

void	SchCount ( FILE *fi, int *ssn, int *wkd, int *vl, int *sw,
				  int *ssnmx, int *vlmx, int *swmx )
{
	char	s[SCHAR] ;
	long	a ;
	int		i ;

	*ssnmx = *vlmx = *swmx = 0 ;

	a = ftell ( fi ) ;

	if ( a != 0L )
		rewind ( fi ) ;

	*ssn = *wkd = *vl = *sw = 0 ;

	while ( fscanf_s ( fi, "%s", s, sizeof(s) ), *s != '*' )
	{
		if ( strcmp ( s, "-ssn" ) == 0 || strcmp ( s, "SSN" ) == 0 )
		{
			(*ssn)++ ;
			i = Schcmpcount ( fi ) ;
			*ssnmx = imax ( i, *ssnmx ) ;
		}
		else if ( strcmp ( s, "-wkd" ) == 0 || strcmp ( s, "WKD" ) == 0 )
			(*wkd)++ ;
		else if ( strcmp ( s, "-v" ) == 0 || strcmp ( s, "VL" ) == 0 )
		{
			(*vl)++ ;
			i = Schcmpcount ( fi ) ;
			*vlmx = imax ( i, *vlmx ) ;
		}
		else if ( strcmp ( s, "-s" ) == 0 || strcmp ( s, "SW" ) == 0 )
		{
			(*sw)++ ;
			i = Schcmpcount ( fi ) ;
			*swmx = imax ( i, *swmx ) ;
		}
	}

	rewind ( fi ) ;
}

/***************************************************************************/

int		Schcmpcount ( FILE *fi )
{
	int		N ;
	char	s[SCHAR] ;
	
	N = 0 ;
	while ( fscanf_s ( fi, "%s", s, sizeof(s) ) != EOF )
	{
		if ( *s == ';' )
			break ;
		N++ ;
	}

	return ( N -1 ) ;
}
