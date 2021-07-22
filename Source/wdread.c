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

/*  wreadlib.c   */

//#define		DEBUG	0
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnesy.h"
#include "sun.h"
#include "psy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "winerror.h"

double	Intgtsup(int Nday, double *Tsupw ) ;

/* 月日の設定（翌日の日時を設定する） */

void monthday(int *Mon, int *Day, int mo, int dayo)
{
	*Day = dayo + 1;
	*Mon = mo;
	
	switch (mo)
	{
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		if (dayo == 31)
		{
			*Day = 1;
			if (mo == 12)
				*Mon = 1;
			else
				*Mon = mo + 1;
		}
		break;
	case 4: case 6: case 9: case 11:
		if (dayo == 30)
		{
			*Day = 1;
			*Mon = mo + 1;
		}
		break;
	case 2: 
	       if (dayo == 28)
		   {
			   *Day = 1;
			   *Mon = mo + 1;
		   }
	}
}


/*  気象デ－タの入力     */

void Weatherdt(SIMCONTL *Simc, DAYTM *Daytm, LOCAT *Loc, WDAT *Wd, EXSF *Exs, char EarthSrfFlg)
{
	extern int	DEBUG, dayprn ;
	extern FILE	*ferr ;
	extern double  Lat,Slat,Clat,Tlat,Lon,Ls,Isc;
	
	static int   ptt=25, nc=0;
	static double     decl, E, tas, timedg;
	double  Sh, Sw, Ss;
	extern double  Sgm;
//	double  Br;
	static double dt[7][25], dtL[7][25] ;
	int		tt, wkdy, Year, Mon, Day;
	char	s[SCHAR] ;
	extern double	dTM ;
	
	/**********
	Daytm->day = day;
	Daytm->time = time;
	tt = Daytm->tt = time;
	Daytm->ttmm = time*100;
	*************/
	
	tt = Daytm->tt;
	
	if (tt < ptt)		// ptt:前時刻　tt < ptt　は日付が変わった場合の処理
	{ 
		if (Simc->wdtype == 'H')			// 'H':HASP形式の気象データが指定されている場合
		{
			if ( Simc->dTm < 3600 )
			{
				hspwdread (Simc->fwdata, Daytm->day-1, Loc, &Year, &Mon, &Day, &wkdy, dtL);
				//printf("Mon=%d  Day=%d\n", Mon, Day ) ;
			}

			hspwdread (Simc->fwdata, Daytm->day, Loc, &Year, &Mon, &Day, &wkdy, dt);
			if (Daytm->Mon != Mon || Daytm->Day != Day)
			{
				sprintf(s, "loop Mon/Day=%d/%d - data Mon/Day=%d/%d",
					Daytm->Mon, Daytm->Day, Mon, Day );
				Eprint ( "<Weatherdt>", s ) ;

				preexit ( ) ;
				exit(EXIT_MOND);
			}
		}

		// 気象データ読み込みの前処理
		if (nc == 0)
		{
			Lat = Loc->Lat; 
			Lon = Loc->Lon;
			Ls  = Loc->Ls;
			Sunint();
			Psyint(); 
			if (Simc->wdtype == 'H')		// HASP形式の場合
			{
				// 給水温度の読み込み
				gtsupw (Simc->ftsupw, Loc->name,
					&(Loc->daymxert), &(Loc->Tgrav), &(Loc->DTgr), Loc->Twsup);

				// 給水温度補間の準備
				Intgtsup(1, Loc->Twsup) ;
			}
			// 地表面温度保持メモリの確保
			if(EarthSrfFlg == 'Y')
			{
				Wd->EarthSurface = dcalloc(366*25, "EarthSurface") ;
				EarthSrfTempInit(Simc, Loc, Wd) ;
			}
			nc =1;
		}
		decl = FNDecl(Daytm->day);
		E = FNE(Daytm->day);
		
		if (Wd->Intgtsupw == 'N')
			Wd->Twsup = Loc->Twsup[Daytm->Mon - 1];
		else
			Wd->Twsup = Intgtsup(Daytm->day, Loc->Twsup) ;
		//printf("%lf\n", Wd->Twsup) ;
		//Exsfter (Daytm->day, Loc->daymxert, Loc->Tgrav, Loc->DTgr, Exs, Wd, tt);  
	}
	
	Exsfter (Daytm->day, Loc->daymxert, Loc->Tgrav, Loc->DTgr, Exs, Wd, tt);  
	timedg = Daytm->tt + fmod((double)Daytm->ttmm, 100.0)/60.0;
	
	/*****
	printf("<<Weatherdt>>  timedg=%6.3lf\n", timedg);
	*****/
	
	tas = FNTtas(timedg, E);
	Solpos(tas, decl, &Sh, &Sw, &Ss, &Wd->solh, &Wd->solA);
	
	Wd->Sh = Sh;
	Wd->Sw = Sw;
	Wd->Ss = Ss;
	
	if (Simc->wdtype == 'H')
	{
		/*******************************************
		Wd->T = dt[0][tt];
		Wd->x = dt[1][tt];
		Wd->Idn =dt[2][tt]/0.86;
		Wd->Isky=dt[3][tt]/0.86;
		Wd->Ihor= Wd->Idn * Wd->Sh + Wd->Isky;
		
		if ( Wd->RNtype == 'C' )
			Wd->CC= dt[4][tt];
		else
			Wd->RN = dt[4][tt];
		
		Wd->Wdre= dt[5][tt];
		Wd->Wv=dt[6][tt];
		Wd->RH = FNRhtx(Wd->T, Wd->x);
		Wd->h = FNH(Wd->T, Wd->x);
		Br= 0.51+0.209*sqrt(FNPwx(Wd->x));
		
		if ( Wd->RNtype == 'C' )
			Wd->RN= (1.0 - 0.62 * Wd->CC /10.0)*(1.-Br)*Sgm*pow(Wd->T+273.15, 4.0);
			**************************************************/

		// 計算時間間隔が1時間未満の場合には直線補完する
		if ( Simc->dTm < 3600 )
		{
			wdatadiv ( Daytm, Wd, dt, dtL ) ;
		}
		else
			dt2wdata ( Wd, tt, dt ) ;
	}
	else
		wdflinput(&Simc->wdpt, Wd);		// VCFILE形式の気象データの読み込み
	
	if ( DEBUG )
	{
		printf ( "\n\n<Weatherdt>  ***** Wdata *****\n\n=================\n" ) ;
		printf ( "\tT=%.1lf\n\tx=%.4lf\n\tRH=%.0lf\n",
			Wd->T, Wd->x, Wd->RH ) ;
		printf ( "\tIdn=%.0lf\n\tIsky=%.0lf\n\tIhor=%.0lf\n",
			Wd->Idn, Wd->Isky, Wd->Ihor ) ;
		printf ( "\tRN=%.0lf\n\tCC=%.0lf\n\tWdre=%.0lf\n\tWv=%.1lf\n",
			Wd->RN, Wd->CC, Wd->Wdre, Wd->Wv ) ;
		printf ( "\th=%.0lf\n==================\n", Wd->h ) ;
	}

	if ( dayprn && ferr )
	{
		fprintf ( ferr, "\n\n<Weatherdt>  ***** Wdata *****\n\n=================\n" ) ;
		fprintf ( ferr, "\tT=%.1lf\n\tx=%.4lf\n\tRH=%.0lf\n",
			Wd->T, Wd->x, Wd->RH ) ;
		fprintf ( ferr, "\tIdn=%.0lf\n\tIsky=%.0lf\n\tIhor=%.0lf\n",
			Wd->Idn, Wd->Isky, Wd->Ihor ) ;
		fprintf ( ferr, "\tRN=%.0lf\n\tCC=%.0lf\n\tWdre=%.0lf\n\tWv=%.1lf\n",
			Wd->RN, Wd->CC, Wd->Wdre, Wd->Wv ) ;
		fprintf ( ferr, "\th=%.0lf\n==================\n", Wd->h ) ;
	}

	ptt = tt;
}

/* ------------------------------------------------------- */

void  gtsupw (FILE *fp, char *Loc, 
			  int *nmx, double *Tgrav, double *DTgr, double *Tsupw)
{
	static int    ic=0;
	int		m, flg = 0;
	char	s[SCHAR], E[SCHAR] ;
	
	if (ic == 0)
	{ 
		while ( fscanf ( fp, "%s", s ) != EOF )
		{ 
			//fscanf(fp, " %s", s);
			if (strcmp(s, "end") == 0)
			{
				// higuchi add 以下３行
				//Loc = "Tokyo" ;
				//printf("気象データファイルと給水温度・地中温度ファイル(supw.efl)の地名が一致しません。\n東京のsupw.eflデータを代用します。\nLocatin=%s\n",Loc) ;
				//preexit() ;
				//exit(1) ;
				//以下４行、higuchi del supw.eflと気象データの地名に整合性の無い場合は、東京で走らせる。
				sprintf ( E,"supw.eflに%sが登録されていません。", s) ;
				Eprint ( "<gtsupw>", E ) ;

				preexit () ;
				exit(EXIT_GTSUPW);
			}
			if (strcmp(s, Loc) == 0)
			{
				flg = 1;
				break;
			}
		}
		
		for (m=0; m<12; m++)
			fscanf(fp, "%lf", &Tsupw[m]);
		
		fscanf(fp, " %d %lf %lf ", nmx, Tgrav, DTgr);
		
		ic = 1;

		if (flg == 0)
		{
			sprintf(E, "supw.eflに%sが登録されていません。", s);
			Eprint("<gtsupw>", E);
			//getch();
			preexit();
		}
	}
}

/* -------------------------------------------------------- */
/*   HASP標準気象デ－タファイルからの入力   */

void hspwdread(FILE *fp, int nday,
			   LOCAT *Loc, int *year, int *mon, int *day, int *wkdy,
			   double dt[7][25])
{
	extern int	DEBUG ;
	extern FILE	*ferr ;
	double  d, a, b, c;
	int   k, t;
	char  Data[24][4], Yr[3], Mon[3], Day[3], Wk[2], Sq[3];
	char	s[SCHAR] ;
	static int ic=0, recl=82;
	
	nday = ( nday > 365 ? nday - 365 : nday ) ;
	nday = ( nday <= 0 ? nday + 365 : nday ) ;

	if (ic == 0)
	{
		fscanf(fp,"%s %lf %lf %lf %lf %lf %lf ",
			s, &Loc->Lat, &Loc->Lon, &Loc->Ls, &a,&b,&c);
		Loc->name = stralloc ( s ) ;
		
		if ( ferr )
		{
			fprintf(ferr,"\n------> <hspwdread> \n"); 
			fprintf(ferr,"\nName=%s\tLat=%.4g\tLon=%.4g\tLs=%.4g\ta=%.4g\tb=%.4g\tc=%.4g\n",
				Loc->name, Loc->Lat, Loc->Lon, Loc->Ls, a,b,c); 
		}
	}

	if (ic != nday)
		fseek(fp,(long)recl*7*(nday-1)+recl, 0);

	if (ic > 0)
		for (k=0; k<7; k++)
			dt[k][0]=dt[k][24];
		
		for (k=0; k<7; k++)
		{
			for (t=0; t <24; t++)
				fgets(Data[t],4,fp);

			fgets(Yr,3,fp);
			fgets(Mon,3,fp);
			fgets(Day,3,fp);
			fgets(Wk,2,fp);
			fgets(Sq,3,fp);
			
               
			for (t=1; t<25; t++)
			{
				d=atof(Data[t-1]);
				switch (k)
				{
				case 0: dt[k][t]=(d-500.)*0.1;
					break;
				case 1: dt[k][t]= 0.0001*d;
					break;
				case 6: dt[k][t]= 0.1*d;
					break;
				default: dt[k][t]=d;
					break;
				}
			}
		}
		/*--higuchi--
		printf("C Mon=%s Day=%s\n",Mon,Day) ;
                --*/
		*year = atoi(Yr);
		*mon = atoi(Mon);
		*day = atoi(Day);
		*wkdy = atoi(Wk);
		
		if (ic == 0)
		{
			for (k=0; k<7; k++)
				dt[k][0]=dt[k][1];
		}
		ic=nday+1;
		
		if ( DEBUG )
		{
			for (t=0; t<25; t++)
			{
				printf("%2d %5.1lf %6.4lf %5.0lf %5.0lf %2.0lf %2.0lf %5.1lf\n",
					t, dt[0][t],dt[1][t],dt[2][t],dt[3][t],dt[4][t],
					dt[5][t],dt[6][t]); 
			}
		}
}


/*--------------------------------------------------------------------------------*/


void	dt2wdata ( WDAT *Wd, int tt, double dt[7][25] )
{
	double	Br ;
	extern double	Sgm ;

	Wd->T = dt[0][tt];
	Wd->x = dt[1][tt];
	Wd->Idn =dt[2][tt]/0.86;
	Wd->Isky=dt[3][tt]/0.86;
	Wd->Ihor= Wd->Idn * Wd->Sh + Wd->Isky;
	
	if ( Wd->RNtype == 'C' )
		Wd->CC= dt[4][tt];
	else
	{
		Wd->CC = -999.0 ;
		Wd->RN = dt[4][tt]/0.86;
		Wd->Rsky = Sgm*pow(Wd->T+273.15,4.0) - Wd->RN ;
	}
	
	Wd->Wdre= dt[5][tt];
	Wd->Wv=dt[6][tt];
	Wd->RH = FNRhtx(Wd->T, Wd->x);
	Wd->h = FNH(Wd->T, Wd->x);
	Br= 0.51+0.209*sqrt(FNPwx(Wd->x));
	if ( Wd->RNtype == 'C' ){
		Wd->RN= (1.0 - 0.62 * Wd->CC /10.0)*(1.-Br)*Sgm*pow(Wd->T+273.15, 4.0);
	    Wd->Rsky=((1.0-0.62*Wd->CC/10.0)*Br+0.62*Wd->CC/10.0)
             *Sgm*pow(Wd->T+273.15,4.0) ;  /*--higuchi 070918--*/
	}
}

void	wdatadiv ( DAYTM *Daytm, WDAT *Wd, double dt[7][25], double dtL[7][25] )
{
	WDAT	WdF, WdL ;
	double	r ;

	WdL.RNtype = WdF.RNtype = Wd->RNtype ;

	r = fmod (( double ) Daytm->ttmm, 100.0 ) / 60.0 ;

	if ( Daytm->ttmm % 100 == 0 )
		dt2wdata ( Wd, Daytm->tt, dt ) ;
	else if ( Daytm->ttmm > 100 )
	{
		dt2wdata ( &WdL, Daytm->tt, dt ) ;
		dt2wdata ( &WdF, Daytm->tt + 1, dt ) ;

		WdLinearDiv ( Wd, &WdL, &WdF, r ) ;
	}
	else
	{
		dt2wdata ( &WdL, 24, dtL ) ;
		dt2wdata ( &WdF, 1, dt ) ;

		WdLinearDiv ( Wd, &WdL, &WdF, r ) ;
	}
}

void	WdLinearDiv ( WDAT *Wd, WDAT *WdL, WDAT *WdF, double dt )
{
	Wd->T = lineardiv ( WdL->T, WdF->T, dt ) ;
	Wd->x = lineardiv ( WdL->x, WdF->x, dt ) ;
//	Wd->RH = lineardiv ( WdL->RH, WdF->RH, dt ) ;
	Wd->RH = FNRhtx ( Wd->T, Wd->x ) ;
//	Wd->h = lineardiv ( WdL->h, WdF->h, dt ) ;
	Wd->h = FNH ( Wd->T, Wd->x ) ;
	Wd->Idn = lineardiv ( WdL->Idn, WdF->Idn, dt ) ;
	Wd->Isky = lineardiv ( WdL->Isky, WdF->Isky, dt ) ;
//	Wd->Ihor = lineardiv ( WdL->Ihor, WdF->Ihor, dt ) ;
	Wd->Ihor= Wd->Idn * Wd->Sh + Wd->Isky;
	Wd->CC = lineardiv ( WdL->CC, WdF->CC, dt ) ;
	Wd->RN = lineardiv ( WdL->RN, WdF->RN, dt ) ;
	Wd->Wv = lineardiv ( WdL->Wv, WdF->Wv, dt ) ;
	Wd->Wdre = lineardiv ( WdL->Wdre, WdF->Wdre, dt ) ;
	
	if ( Wd->Wdre > 16. )
		Wd->Wdre -= 16.0 ;
}

void EarthSrfTempInit(SIMCONTL *Simc, LOCAT *Loc, WDAT *Wd)
{
	double	decl, E, ac, Te, tas, Sh, Sw, Ss ;
	int		year, nday, tt ;
	int		wkdy, Year, Mon, Day;
	double dt[7][25] ;
	double	*U, *T, *oldT, a, u, *dblTemp, *dbloldTemp ;
	int		i, j;
	double	Ihol ;
	double	Soic, Soil ;

	// 地表面温度の計算
	printf("地表面温度の計算開始\n") ;

	// 土壌の容積比熱[J/m3K]
	Soic = 3.34e6 ;
	// 土壌の熱伝導率[W/mK]
	Soil = 1.047 ;
	// 土壌の熱拡散率
	a = Soil / Soic ;
	ac = 23. ;
	u = Simc->dTm * a / (0.5 * 0.5) ;
	U = dcalloc(20*20, "<EarthSrfTempInit>" ) ;
	T = dcalloc(20*20, "<EarthSrfTempInit>" ) ;
	oldT = dcalloc(20*20, "<EarthSrfTempInit>" ) ;
	//matinit(U, 20*20) ;
	//matinit(T, 20) ;
	// 地中温度の初期温度は平均外気温度
	matinitx(oldT, 20, Loc->Tgrav) ;

	// 地中温度計算用の行列の作成
	for(i = 0; i < 20; i++)
	{
		U[i*20+i] = 1. + 2. * u ;
		if(i > 0)
			U[i*20+i-1] = - u ;
		if(i < 20 - 1)
			U[i*20+i+1] = -u ;

		if(i == 0)
		{
			U[i*20+i] = 1. + Simc->dTm / (0.5 * ( 0. + Soic * 0.5) * 1. / ac) + Simc->dTm / ( 0.5 * (0. + Soic * 0.5) * 0.5 / Soil);
			U[i*20+i+1] = - Simc->dTm / ( 0.5 * (0. + Soic * 0.5) * 0.5 / Soil) ;
		}
		if(i == 19)
		{
			U[i*20+i] = 1. + Simc->dTm / ( 0.5 * (0. + Soic * 0.5) * 0.5 / Soil) ;
			U[i*20+i-1] = - Simc->dTm / ( 0.5 * (0. + Soic * 0.5) * 0.5 / Soil) ;
		}
	}
	//matfprint("%.2lf ", 20,U ) ;
	// Uの逆行列の計算
	matinv(U, 20, 20, "<EarthSrfTempInit>" ) ;

	// １年目は助走期間
	for(year = 0; year < 2; year++)
	{
		for(nday = 1; nday <= 365; nday++)
		{
			decl = FNDecl(nday) ;
			E = FNE(nday) ;

			// 気象データの読み込み（24時間分）
			hspwdread (Simc->fwdata2, nday, Loc, &Year, &Mon, &Day, &wkdy, dt);

			for(tt = 1; tt <=24; tt++)
			{
				dblTemp = T ;
				dbloldTemp = oldT ;

				matinit(T, 20) ;

				tas = FNTtas(tt, E);
				// 太陽位置の計算
				Solpos(tas, decl, &Sh, &Sw, &Ss, &Wd->solh, &Wd->solh);

				// 読み込んだ気象データから気象要素の取り出し
				dt2wdata ( Wd, tt, dt ) ;

				// 水平面全天日射量の計算
				Ihol = Wd->Idn * Sh + Wd->Isky ;

				// 地表面相当外気温度の計算
				Te = Wd->T + (0.7 * Ihol - Wd->RN) / ac ;

				// 境界条件の設定
				*dbloldTemp += (Te * Simc->dTm / ( 0.5 * Soic * 0.5 / ac ))  ;

				// 地中温度の計算
				matmalv(U, oldT, 20, 20, T) ;

				// 助走計算ではない場合
				if(year == 1)
				{
					//printf("nday=%d tt=%d EarthSurface=%lf\n", nday, tt, *dblTemp) ;
					Wd->EarthSurface[nday*24+tt] = *dblTemp ;
				}

				// 前時刻の温度の置き換え
				for(j = 0; j < 20; j++, dbloldTemp++, dblTemp++)
				{
					//printf("nday=%d tt=%d %.1lf ", nday, tt, *dblTemp) ;
					*dbloldTemp = *dblTemp ;
				}
				//printf("\n" ) ;
			}
		}
	}

	free(U) ;
	free(T) ;
	free(oldT) ;
	fclose(Simc->fwdata2) ;
	
	printf("地表面温度の計算終了\n") ;
}
