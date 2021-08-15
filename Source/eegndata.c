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

/*    ee_gndata.c      */

#define _CRT_SECURE_NO_WARNINGS
#define FLDELIM  '/'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
/*************
#include "wthrd.h"
******************/
#include "fesy.h"
#include "fnfio.h"
#include "sun.h"
#include "fnlib.h"

/*   ファイル、計算期間、出力日の入力        */


void Gdata (FILE *fi, char *dsn, char *File, char **wfname, char **ofname,
			int *dtm, int *sttmm, int *dayxs, int *days, int *daye, double *Tini, int *pday,
			int *wdpri, int *revpri, int *pmvpri, char *helmkey, int *MaxIterate, DAYTM *Daytm, WDAT *Wd, char *perio)
{
	char	s[SCHAR], ss[SCHAR], *ce, *st, E[SCHAR], dd[SCHAR];
	int		Ms, Ds, Mxs=0, Dxs=0, Me, De, n, ns, ne, i;
	int		logprn = 0 ;
	
	*dtm = 3600;
	*sttmm = -1;
	
	*wfname = NULL;
	
	sprintf (E, ERRFMT, dsn);
	
	*ofname = stralloc( File);

	for (i=1; i<366; i++)
		pday[i] = 0;

	/*****************
	printf ( "%s\n", ofname ) ;
	*****************************/
	
	if ((st = strrchr(*ofname, '.')) != NULL)
		*st = '\0';
	
	while (fscanf(fi, "%s", s), s[0] != '*')
	{ 
		if (strcmp(s, "FILE") == 0)
		{ 
		/********
		strcpy(ofname, File);
		if ((st = strrchr(ofname, '.')) != NULL)
		*st = '\0';
			*******/
			
			while (fscanf(fi, "%s", s), s[0] != ';')
			{
				if ((ce=strchr(s, ';')) != 0)
					*ce = '\0';
				
				if ( strcmp ( s, "-skyrd" ) == 0 )			// 気象データは夜間放射量で定義されている
					Wd->RNtype = 'R' ;
				else if ( strcmp(s, "-intgtsupw" ) == 0)	// 給水温度を補間する
					Wd->Intgtsupw = 'Y' ;
				else
				{
					st=strchr(s, '=');
					*st = '\0';
					if (strcmp(s, "w") == 0)
					{
						sscanf(st+1, "%s", dd);
						*wfname= stralloc( dd ) ;
					}
					else if (strcmp(s, "out") == 0)
					{
						sscanf(st+1, "%s", ss);
						if (strrchr(ss, FLDELIM) == NULL)
						{
							free ( *ofname) ;
							
							*ofname = stralloc( File);
							
							if ((st = strrchr(*ofname, FLDELIM)) != NULL)
								strcpy(st + 1, ss);
						}
						else
						{
							free ( *ofname ) ;
							*ofname= stralloc ( ss);
						}
					}
					else
						Eprint ( "<Gdata>", s );
				}
				
				if ( ce )
					break;
			}
		}
		else if (strcmp(s, "RUN") == 0)
		{
			*Tini=15.0;
			
			while (fscanf(fi, "%s", s), s[0] != ';')
			{
				if ((ce=strchr(s, ';')) != 0)
					*ce = '\0';

				if (s[0] == 'T')
					sscanf(s, "%*[^=]=%lf", Tini);
				else if ((st = strchr(s, '=')) != NULL)
				{
					*st = '\0';
					if (strcmp(s, "dTime") == 0)
						*dtm = atoi(st + 1);
					else if (strcmp(s, "Stime") == 0)
						*sttmm = atoi(st + 1) * 100;
					else if (strcmp(s, "MaxIterate") == 0)
						*MaxIterate = atoi(st + 1);
					else if (strcmp(s, "RepeatDays") == 0)	// 周期定常計算の繰り返し日数の取得
					{
						int Ndays;
						Ndays = atoi(st + 1);
						if (*perio != 'y')
							printf("周期定常計算の指定がされていません\n");
						*daye = *days + Ndays - 1;
					}
				}
				else if (s[0] == '(')
				{
					sscanf(s, "(%d/%d)", &Mxs, &Dxs);
					*dayxs = FNNday(Mxs, Dxs);
				}
				// 周期定常計算への対応
				else if (strcmp(s, "-periodic") == 0)
				{
					*perio = 'y';			// 周期定常計算フラグの変更
					fscanf(fi, "%s", s);	// 計算する日付の読み込み
					sscanf(s, "%d/%d", &Ms, &Ds);	// 計算する日付の取得
					*days = FNNday(Ms, Ds);
					*dayxs = *days;
					Daytm->Mon = Ms;
					Daytm->Day = Ds;
				}
				else if (strchr(s,'-') != 0)
				{
					sscanf(s, "%d/%d-%d/%d", &Ms,&Ds,&Me,&De);
					*days = FNNday(Ms, Ds);
					*daye = FNNday(Me, De);
					if (Mxs == 0)
					{
						*dayxs = *days;
						Daytm->Mon = Ms;
						Daytm->Day = Ds;
					}
					else
					{
						Daytm->Mon = Mxs;
						Daytm->Day = Dxs;
					}		  
				}
				else
					Eprint ( "<Gdata>", s ) ;

				if ( ce )
					break;
			}
			printf("<<Gdata>> dtm=%d\n", *dtm);
		}
		else if (strcmp(s, "PRINT") == 0)
		{
			while (fscanf(fi, "%s", s), s[0] != ';')
			{
				if ((ce=strchr(s, ';')) != 0)
					*ce = '\0';

				if (strcmp(s, "*wd") == 0)
					*wdpri =1;
				else if (strcmp(s, "*rev") == 0)
					*revpri = 1;
				else if (strcmp(s, "*pmv") == 0)
					*pmvpri = 1;
				else if (strcmp(s, "*helm") == 0)
					*helmkey = 'y';
				else if (strcmp(s, "*log") == 0 )
					logprn = 1 ;
				else if (strcmp(s, "*debug" ) == 0 )
					DEBUG = 1 ;
				else if (strchr(s, '-') == 0)
				{
					sscanf(s, "%d/%d", &Ms, &Ds);
					pday[ FNNday(Ms, Ds) ] = 1;
				}
				else
				{
					sscanf(s, "%d/%d-%d/%d", &Ms, &Ds, &Me, &De);
					ns = FNNday(Ms, Ds);
					ne = (ns < (n=FNNday(Me, De)) ? n : n+365);
					for (n=ns; n <= ne; n++)
						pday[ (n > 365 ? n-365 : n) ] = 1;
				}
				
				if ( ce )
					break;
			}
		}
		else
			Eprint ( "<Gdata>", s );
   }

   strcat ( strcpy ( s, *ofname ), ".log" ) ;
   ferr = fopen ( s, "w" ) ;

   if ( logprn == 0 )
   {
	   fclose ( ferr ) ;
	   ferr = NULL ;
   }
}
