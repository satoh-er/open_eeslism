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

/*   binit.c   */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "esize.h"
#include "fnbld.h"
#include "lib/u_psy.h"
#include "fnlib.h"
#include "fnfio.h"

/*  壁体デ－タの入力  */

void Walldata (FILE *fi, FILE *fbmlist, char *dsn,
			   WALL **Wall, int *Nwall, DFWL *dfwl, PCM *pcm, int Npcm)
{
	static char   s[SCHAR], *st, *ce, E[SCHAR], layer[WELMMX][SCHAR];
	int    i= -1, j, jj, jw, Nlyr, k= -1, Nbm, ndat, ndiv, N, Nw ; // ic;
	double  dt;
	BMLST	*W, *Wl, *Wc ;
	char	c ;
	WALL	*Wa ;
	WELM	*w ;
	//PCM		*PCM ;
	// void Walli();
	
	W = NULL ;
	sprintf (E, ERRFMT, dsn);
	
	Nw = wbmlistcount ( fbmlist ) ;
	
	strcpy ( s, "Walldata wbmlist.efl--" ) ;
	
	if ( Nw > 0 )
	{
		Nw++ ;
		if (( W = ( BMLST * ) malloc ( Nw * sizeof ( BMLST ))) == NULL )
			Ercalloc ( Nw, s ) ;

		Wl = W ;
		for ( j = 0; j < Nw; j++, Wl++ )
		{
			Wl->mcode = NULL ;
			Wl->Cond = Wl->Cro = -999.0 ;
		}
	}
	
	k = 0 ;
	Wl = W ;
	while (fscanf(fbmlist, "%s", s) != EOF, *s != '*')
	{
		if (strcmp(s,"!") == 0)
		{
			while (fscanf(fbmlist,"%c", &c), c != '\n' )
				;
		}
		
		else
		{
		/*****************
		if ( BMLSTrealloc ( &W, k + 1, k + 2 ) == 0 )
		{
		printf("<Walldata> メモリの最確保に失敗しました。" );
		exit ( 0 ) ;
		}
			/*******************************/
			
			//if (k > WBMLSTMX)
			//	printf("%s bmlist=%d [max=%d]\n", "wbmlist xxxx", k, WBMLSTMX);
			
			//k++ ;
			//W++ ;
			
			//Wl = &W[k] ;
			
			Wl->mcode = stralloc ( s);

			Wc = W ;
			for ( j = 0; j < k - 1; j++, Wc++ )
			{
				if ( Wl->mcode != NULL && Wc->mcode != NULL
					&& strcmp ( Wl->mcode, Wc->mcode ) == 0 )
				{
					sprintf ( s, "wbmlist.efl duplicate code=<%s>",
						Wl->mcode ) ;
					Eprint ( "<Walldata>", s ) ;
				}
			}

			fscanf(fbmlist, "%lf %lf", &Wl->Cond, &Wl->Cro);
			// Cond：熱伝導率［W/mK］、Cro：容積比熱［kJ/m3K］
			
			//for ( j=0; j<k+1; j++)
			//	printf ("k=%d code=%s\n", k, (W+j)->mcode) ;
			
			/******
			printf("Walldata>> name=%s Con=%lf Cro=%lf\n", (W+k)->mcode,
			(W+k)->Cond, (W+k)->Cro ) ;
			********************/
			Wl++ ;
			k++ ;
		}
	}
	k++ ;
	Nbm = k ;
	
	/*******************************
	for ( k = 0; k < Nbm; k++ )
	printf("Walldata>> name=%s Con=%lf Cro=%lf\n", (W+k)->mcode,
	(W+k)->Cond, (W+k)->Cro ) ;
	******************************/
	
	N = Wallcount ( fi ) ;
	
	strcpy ( s, "Walldata --" ) ;
	
	if ( N > 0 )
	{
		if (( *Wall = ( WALL * ) malloc ( N * sizeof ( WALL ))) == NULL )
			Ercalloc ( N, s ) ;

		Wa = *Wall ;
		for ( j = 0; j < N; j++, Wa++ )
		{
			Wa->name = NULL ;
			Wa->ble = ' ' ;
			Wa->N = Wa->M = Wa->mp = Wa->end = 0 ;
			Wa->Ip = -1 ;
			Wa->Ei = Wa->Eo = 0.9 ;
			Wa->as = 0.7 ;
			Wa->Rwall = Wa->effpnl = -999.0 ;
			Wa->CAPwall = -999.;
			Wa->res = Wa->cap = NULL ;
			// Wa->welm = NULL ;
			Wa->tra = -999.0 ;
			Wa->Ksu = Wa->Ksd = Wa->Kc = Wa->fcu = Wa->fcd = Wa->ku = Wa->kd = Wa->Ko = Wa->Rd = Wa->Ru = -999. ;
			Wa->Kcu = Wa->Kcd = -999. ;
			Wa->air_layer_t = -999.0 ;
			Wa->dblEsd = Wa->dblEsu = 0.9 ;
			Wa->chrRinput = 'N' ;
			Wa->ColType = NULL ;
			Wa->WallType = 'N' ;
			//Wa->PVwall = 'N' ;

			// 太陽電池一体型空気集熱器のパラメータ初期化
			PVwallcatinit(&Wa->PVwallcat) ;

			w = Wa->welm ;
			for ( jj = 0; jj < WELMMX; jj++, w++ )
			{
				w->code = NULL ;
				w->L = -999.0 ;
				w->ND = 0 ;
				w->Cond = w->Cro = -999. ;
			}

			Wa->ta = Wa->ag = -999.0 ;
			Wa->Eg = Wa->Eb = 0.9 ;
			//PCM = *(Wa->PCM) ;
			for ( jj = 0; jj < WELMMX; jj++, w++ )
			{
				Wa->PCM[jj] = NULL ;
				Wa->PCMrate[jj] = -999.0 ;
			}
			Wa->PCMLyr = NULL ;
			Wa->PCMrateLyr = NULL ;
			Wa->L = NULL ;

			Wa->PCMflg = 'N' ;

			Wa->tnxt = -999.0;
		}
	}
	
	Wa = *Wall ;
	while (fscanf(fi, " %s ",s) != EOF)
	{
		//printf( "<WAlldata> 1.... s=%s\n", s ) ;
		
		if ( *s == '*' )
			break ;
		
		++i;
		
		/**************************************
		if ( WALLrealloc ( Wall, i, i+1 ) == 0 )
		printf ( "<Walldata> Wal メモリ確保に失敗\n" ) ;
		****************************/
		
		//Wa = *(Wall+i) ;
		
		if( *s == '-')
		{
			Wa->ble = s[1];
			if (s[2] == ':')
				Wa->name = stralloc ( &s[3]);
			else
			{
				switch(s[1])
				{
				case 'E':
					dfwl->E = i;
					break;
				case 'R':
					dfwl->R = i;
					break;
				case 'F':
					dfwl->F = i;
					break;
				case 'i':
					dfwl->i = i;
					break;
				case 'c':
					dfwl->c = i;
					break;
				case 'f':
					dfwl->f = i;
					break;
				}
			}
		}
		else
			Wa->name = stralloc ( s);
		
		j= -1;
		
		while (fscanf(fi, "%s", s), *s != ';')
		{
			//printf ( "<Walldata> 2..... s=%s\n", s ) ;
			
			ce = strchr ( s, ';' ) ;
			
			if ( ce )
				*ce = '\0' ;
			if (( st = strchr ( s, '=' )) != NULL ) 
			{ 
				dt = atof ( st + 1 ) ;
				if (strncmp(s,"Ei",2) == 0) 
					Wa->Ei=dt;
				else if (strncmp(s,"Eo",2) == 0)
					Wa->Eo=dt;
				else if (strncmp(s,"as",2) == 0)
					Wa->as =dt;
				else if (strncmp(s,"type",4) == 0)
					Wa->ColType = stralloc(st + 1) ;
				else if (strncmp(s,"tra",3) == 0)
					Wa->tra =dt;
				else if (strncmp(s,"Ksu",3) == 0)
					Wa->Ksu =dt;
				else if (strncmp(s,"Ksd",3) == 0)
					Wa->Ksd =dt;
				else if (strncmp(s,"Ru",2) == 0)
					Wa->Ru =dt;
				else if (strncmp(s,"Rd",2) == 0)
					Wa->Rd =dt;
				else if (strncmp(s,"fcu",3) == 0)
					Wa->fcu =dt;
				else if (strncmp(s,"fcd",3) == 0)
					Wa->fcd =dt;
				else if (strncmp(s, "Kc",2) == 0)
					Wa->Kc = dt ;
				else if (strncmp(s, "Esu",3) == 0)
					Wa->dblEsu = dt ;
				else if (strncmp(s, "Esd",3) == 0)
					Wa->dblEsd = dt ;
				else if (strncmp(s, "Eg",2) == 0)
					Wa->Eg = dt ;
				else if (strncmp(s, "Eb",2) == 0)
					Wa->Eb = dt ;
				else if (strncmp(s, "ag",2) == 0)
					Wa->ag = dt ;
				else if (strncmp(s, "ta",2) == 0)
					Wa->ta = dt / 1000. ;
				else if (strncmp(s, "tnxt",4) == 0)
					Wa->tnxt = dt;
				else if (strncmp(s, "t", 1) == 0)
					Wa->air_layer_t = dt / 1000.;
				//else if (strncmp(s,"Kdd",3) == 0)			// 集熱媒体から裏面への熱貫流率
				//	Wa->Kdd = dt ;
				//else if (strncmp(s,"Kud",3) == 0)			// 集熱媒体から表面への熱貫流率
				//	Wa->Kud = dt ;
				//else if (strncmp(s,"ta",2) == 0)			// 建材一体型空気集熱器透過体の透過率
				//	Wa->ta = dt ;
				//else if (strncmp(s,"Ku",2) == 0)			// 建材一体型空気集熱器集熱板から屋外への熱貫流率
				//	Wa->Ku = dt ;
				//else if (strncmp(s,"Kd",2) == 0)			// 建材一体型空気集熱器集熱板から裏面への熱貫流率
				//	Wa->Kd = dt ;
				//else if (strncmp(s,"Kc",2) == 0)			// 集熱器全体の熱貫流率
				//	Wa->Kc = dt ;
				//else if (strncmp(s,"PVcap",5) == 0)			// 太陽電池容量
				//{
				//	Wa->PVwallcat.PVcap = dt ;
				//	if ( dt > 0. )
				//		Wa->PVwall = 'Y' ;
				//}
				else if (strncmp(s,"KHD",3) == 0)			// 日射量年変動補正係数
					Wa->PVwallcat.KHD = dt ;
				else if (strncmp(s,"KPD",3) == 0)			// 経時変化補正係数
					Wa->PVwallcat.KPD = dt ;
				else if (strncmp(s,"KPM",3) == 0)			// アレイ負荷整合補正係数
					Wa->PVwallcat.KPM = dt ;
				else if (strncmp(s,"KPA",3) == 0)			// アレイ回路補正係数
					Wa->PVwallcat.KPA = dt ;
				else if (strncmp(s,"EffInv",6) == 0)		// インバータ実行効率
					Wa->PVwallcat.effINO = dt ;
				else if (strncmp(s,"apmax",5) == 0)			// 最大出力温度係数
					Wa->PVwallcat.apmax = dt  ;
				else if (strncmp(s,"ap",2) == 0)			// 太陽電池裏面の対流熱伝達率
					Wa->PVwallcat.ap = dt ;
				else if (strncmp(s,"Rcoloff",7) == 0)		// 太陽電池から集熱器裏面までの熱抵抗
				{
					Wa->PVwallcat.Rcoloff = dt ;
					Wa->PVwallcat.Kcoloff = 1. / Wa->PVwallcat.Rcoloff ;
				}
				else
					Eprint ( "<Walldata>", s ) ;
			}
			else
			{
				j++ ;
				strcpy(layer[j], s);
			}
			
			if (ce) break;
		}

		Nlyr = j+1;
		
		/*******************
		for (j=0; j<Nlyr; j++)
		printf(" %s\n", layer[j]);
		************************/

		/**************************
		if ( WELMrealloc ( &(Wa->welm), 0, 1 ) == 0 )
			printf ( "<Walldata> Wall->welm メモリ最確保に失敗\n" ) ;
			/***************************************/
		
		Wa->welm->code = stralloc ((char *) "ali" ) ;
		
		jw=0;
		for (j=1; j<=Nlyr; j++)
		{
			if (Wa->ble == 'R' || Wa->ble == 'c')
				jj = Nlyr-j;
			else 
				jj=j-1;
			
			if ((st=strchr(layer[jj],'-')) != 0)
			{ 
				*st='\0';
				
				jw++ ;
				
				/************************
				if ( WELMrealloc ( &(Wa->welm), jw , jw + 1 ) == 0)
					printf ( "<Walldata> Wall->ND メモリ最確保に失敗\n" ) ;
					/*******************************/
				
				w = Wa->welm +jw;
				
				w->code = stralloc (layer[jj] ) ;
				
				// 同一層内の内部分割数の計算
				ndat = sscanf(st+1, "%lf/%d", &dt, &ndiv);
				if (ndat == 1)
					w->ND = (int)(dt>=50. ? (dt-50.)/50. : 0);
				else
					w->ND = ndiv-1;
				
				w->L = dt/1000.;
				// L：部材厚さ［m］（分割前）
			}
			else if (strncmp(layer[jj],"<P",2) == 0 || strcmp(layer[jj], "<C>") == 0)
			{
				Wa->Ip=jw;
				st = &layer[jj][0] + 3;
				if (*st == ':')
					Wa->effpnl = atof(st + 1);
				else
					Wa->effpnl = 0.7;
			}
			else if (strcmp(layer[jj], "alo") != 0 
				&& strcmp(layer[jj], "ali") != 0)
			{
				jw++ ;
				
				/*************************************
				if ( WELMrealloc ( &(Wa->welm), jw , jw + 1 ) == 0)
					printf ( "<Walldata> Wall->ND メモリ最確保に失敗\n" ) ;
					/****************************************/
				
				w = Wa->welm +jw;
				w->code = stralloc ( layer[jj]);
				w->L =0.0 ;
				w->ND=0;
			}
		} 

		// 建材一体型空気集熱器の総合熱貫流率の計算、データ入力状況のチェック
		if ( Wa->Ip >= 0 )
		{
			if ( Wa->tra > 0. )
			{
				Wa->WallType = 'C' ;

				if ((Wa->Ksu > 0. && Wa->Ksd > 0.) || (Wa->Rd > 0. && (Wa->Ru >= 0. || Wa->ta > 0.)))
				{
					if (strncmp(Wa->ColType, "A", 1) == 0)
					{
						if(Wa->Ksu > 0.)
						{
							Wa->Kcu = Wa->fcu * Wa->Ksu ;
							Wa->Kcd = Wa->fcd * Wa->Ksd ;
							Wa->Kc = Wa->Kcu + Wa->Kcd ;
							Wa->ku = Wa->Kcu / Wa->Kc ;
							Wa->kd = Wa->Kcd / Wa->Kc ;
						}
						else
							Wa->chrRinput = 'Y' ;
					}
					else if(strncmp(Wa->ColType, "W", 1) == 0)
					{
						Wa->Ko = Wa->Ksu + Wa->Ksd ;
						Wa->ku = Wa->Ksu / Wa->Ko ;
						Wa->kd = Wa->Ksd / Wa->Ko ;
					}
					//Wa->Ko = Wa->Kd + Wa->Ku ;
					//Wa->KdKo = Wa->Kd / Wa->Ko ;
					//Wa->KcKu = Wa->Kc / Wa->Ku ;
					if(Wa->PVwallcat.KHD > 0.)
						PVwallPreCalc(&Wa->PVwallcat) ;
				}
				else
				{
					sprintf ( s, "ble=%c name=%s 建築一体型空気集熱の熱貫流率Ku、Kdが未定義です",
						Wa->ble, Wa->name ) ;
					Eprint ( "<Walldata>", s ) ;
				}

				if (Wa->chrRinput == 'N' && (Wa->Kc < 0. || Wa->Ksu < 0. || Wa->Ksd < 0. ))
				{
					sprintf ( s, "ble=%c name=%s 建築一体型空気集熱の熱貫流率Kc、Kdd、Kudが未定義です",
						Wa->ble, Wa->name ) ;
					Eprint ( "<Walldata>", s ) ;
				}

				if ( Wa->Ip == -1 )
				{
					sprintf ( s, "ble=%c name=%s 建築一体型空気集熱の空気流通層<P>が未定義です",
						Wa->ble, Wa->name ) ;
					Eprint ( "<Walldata>", s ) ;
				}
			}
			else
			{
				// 通常の床暖房等放射パネル
				Wa->WallType = 'P' ;
			}
		}

		Wa->N = jw+1;
		Walli(Nbm, W, Wa, pcm, Npcm) ;
		
		/*****************************
		printf ( "<Walldata>== ble=%c name=%s\n", Wa->ble, Wa->name ) ;
		/******************************/
		Wa++ ;
   }
   i++ ;
   *Nwall = Wall[0]->end = i ;
   //if (i > WALLDMX)
   //	   printf("%s wname=%d [max=%d]\n", E, i, WALLDMX);
   
   Wl = W ;
   for ( i = 0; i < Nw; i++, Wl++ )
	   free ( Wl->mcode ) ;

   free ( W ) ;
   fclose(fbmlist) ;
} 
/* ------------------------------------------------ */

/*  窓デ－タの入力     */


void Windowdata (FILE *fi, char *dsn, WINDOW **Window, int *Nwindow)
{
	char   s[SCHAR], E[SCHAR], Err[SCHAR], ss[SCHAR], *st ;
	int    i= -1, N, j, k ;
	//double  dt;
	WINDOW	*W, *Wc ;
	
	sprintf (E, ERRFMT, dsn);
	
	N = Wallcount ( fi ) ;

	if ( N > 0 )
	{
		if (( *Window = ( WINDOW * ) malloc ( N * sizeof ( WINDOW ))) == NULL )
			Ercalloc ( N, "Windowdata" ) ;

		W = *Window ;
		for ( j = 0; j < N; j++, W++ )
		{
			W->name = NULL ;
			W->end = 0 ;
			W->K = W->Rwall = W->tgtn = W->Bn = W->as = W->Ag = W->Ao = W->W = W->H = 0.0 ;
			W->Eo = W->Ei = 0.9 ;
			W->RStrans = 'n';
			//W->Cidtype = NULL ;
			W->Cidtype = stralloc("N") ;
			//W->AirFlowFlg = 'N' ;
			//W->AirFlowcat.aci = W->AirFlowcat.acioff = W->AirFlowcat.aco = W->AirFlowcat.acooff = -999. ;
			//W->AirFlowcat.ai = W->AirFlowcat.alr = W->AirFlowcat.ao = W->AirFlowcat.ti = W->AirFlowcat.to = -999. ;
		}
	}

	W = *Window ;
	while (fscanf(fi, "%s", s), *s != '*')
	{ 
		i++ ;
		
		/*****************************
		if ( WINDOWrealloc ( Window, i, i+1 ) == 0 )
			printf ( "<Windowdata> Window メモリ確保に失敗\n" ) ;
			*************************/
		
		//W = *(Window+i) ;
		
		W->name = stralloc ( s);

		Wc = *Window ;
		for ( k = 0; k < i - 1; k++, Wc++ )
		{
			if ( W->name != NULL && Wc->name != NULL
				&& strcmp ( W->name, Wc->name ) == 0 )
			{
				sprintf ( ss, "<WINDOW>  WindowName Already Defined  (%s)", W->name ) ;
				Eprint ( "<Windowdata>", ss ) ;
			}
		}
		
		while (fscanf (fi, "%s",s), *s != ';')
		{ 
			// 室内透過日射が窓室内側への入射日射を屋外に透過する場合'y'
			if (strcmp(s, "-RStrans") == 0)
				W->RStrans = 'y';
			else
			{
				st = strchr(s, '=') + 1;

				//if (strncmp(s,"ti",2) == 0)
				//	W->AirFlowcat.ti =dt;
				//else if (strncmp(s,"to",2) == 0)
				//	W->AirFlowcat.to =dt;
				//else if (strncmp(s,"ai",2) == 0)
				//	W->AirFlowcat.ai =dt;
				//else if (strncmp(s,"ao",2) == 0)
				//	W->AirFlowcat.ao =dt;
				if (*s == 't')
					W->tgtn = atof(st);
				else if (*s == 'B')
					W->Bn = atof(st);
				else if (*s == 'R')
					W->Rwall = atof(st);
				else if (strncmp(s, "Ei", 2) == 0)
					W->Ei = atof(st);
				else if (strncmp(s, "Eo", 2) == 0)
					W->Eo = atof(st);
				else if (strncmp(s, "Cidtype", 7) == 0)
				{
					free(W->Cidtype);
					W->Cidtype = NULL;
					if (*st == '\'')
					{
						sscanf(st + 1, "%[^']", s);
						W->Cidtype = stralloc(s);
					}
					else
						W->Cidtype = stralloc(st);
				}
				//else if (strncmp(s,"acioff",6) == 0)
				//	W->AirFlowcat.acioff =dt;
				//else if (strncmp(s,"acooff",6) == 0)
				//	W->AirFlowcat.acooff =dt;
				//else if (strncmp(s,"aci",3) == 0)
				//	W->AirFlowcat.aci =dt;
				//else if (strncmp(s,"aco",3) == 0)
				//	W->AirFlowcat.aco =dt;
				//else if (strncmp(s,"alr",3) == 0)
				//	W->AirFlowcat.alr =dt;
				else
				{
					sprintf("%s %s\n", E, s);
					Eprint("<Windowdata>", Err);
				}
			}
			
			if (strchr(s,';')) break;
		}

		// エアフローウィンドウフラグの設定
		//if ( W->AirFlowcat.aci > 0. )
		//	W->AirFlowFlg = 'Y' ;

		W++ ;
	}
	
	i++ ;
	*Nwindow = Window[0]->end = i;
	
	/*****************************
	for ( i = 0; i < *Nwindow; i++ )
	printf ( "<Windowdata> name=%s\n", Window[i]->name ) ;
	/***********************************/
	
	/***********************************
	for ( i = 0; i < *Nwindow; i++ )
	{
	printf ( "Name=%s t=%.2lf B=%.2lf R=%.2lf Ei=%.1lf Eo=%.1lf\n",
	Window[i]->name, Window[i]->tgtn, Window[i]->Bn,
	Window[i]->Rwall, Window[i]->Ei, Window[i]->Eo ) ;
	}
	/*******************************************/
	/****************************
	if (i > WINDWMX)
	printf("%s fname=%d [max=%d]\n", E, i, WINDWMX);
	********************************/
}
/* --------------------------------------------------- */

void Snbkdata (FILE *fi, char *dsn, SNBK **Snbk)
{
	char *ce, *vs, k[2], code[8], E[SCHAR*3+128],
		name[SCHAR], s[SCHAR], key[SCHAR], v[SCHAR], Er[SCHAR];
	int  i = -1, j, type, N ;
	double   val[2];
	static char *typstr[] =
	{
		"HWDTLR.", "HWDTLRB", "HWDTL.B", "HWDT.RB", "HWDT...",
			"HWD.LR.", "HWD.L..", "HWD..R.", "HWDTLRB"
	};
	
	SNBK	*S ;
	
	sprintf(Er, ERRFMT, dsn);
	type = 0 ;
	
	N = Wallcount ( fi ) ;

	if ( N > 0 )
	{
		if (( *Snbk = ( SNBK * ) malloc ( N * sizeof ( SNBK ))) == NULL )
			Ercalloc ( N, "Windowdata" ) ;

		S = *Snbk ;
		for ( j = 0; j < N; j++, S++ )
		{
			S->name = NULL ;
			S->W = S->H = 0.0 ;
			S->D = S->W1 = S->W2 = S->H1 = S->H2 = 0.0 ;
			S->end = S->type = S->ksi = 0 ;
		}
	}

	S = *Snbk ;
	while (fscanf(fi, " %s", name), *name != '*')
	{
		i++ ;
		
		/***************************
		if ( SNBKrealloc ( Snbk, i, i+1 ) == 0 )
			printf ( "<Snbkdata> Snbk メモリ確保に失敗\n" ) ;
		
		S = *(Snbk+i) ;
		/*************************************/

		S->name = stralloc ( name ) ;
		strcpy(code, ".......");
		
		while (fscanf(fi, " %s" ,s),  *s != ';')
		{
			if ((ce=strchr(s, ';')) != 0)
				*ce = '\0';

			sscanf(s, "%[^=]=%s", key, v);
			if (strcmp(key, "type") == 0)
			{
				vs = v ;
				if (*v == '-')
				{
					S->ksi = 1 ;
					vs++;
				}
				else
					S->ksi = 0;
				
				if (strcmp(vs, "H") == 0)
					type = 1;
				else if (strcmp(vs, "HL") == 0)
					type = 5;
				else if (strcmp(vs, "S") == 0)
					type = 2;
				else if (strcmp(vs, "SL") == 0)
					type = 6;
				else if (strcmp(vs, "K") == 0)
					type = 9;
				else
				{
					sprintf ( E, "%s %s %s\n", Er, name, s);
					Eprint ( "<Snbkdata>", E ) ;
				}
			}
			else if (strcmp(key, "window") == 0)
			{
				sscanf(v, "%c%lfx%c%lf", &k[0], &val[0], &k[1], &val[1]);
				for (j=0; j<2; j++)
				{
					if (k[j] == 'H')
						S->H = val [j], code[0] = 'H';
					else if (k[j] == 'W')
						S->W = val[j], code[1] = 'W';
					else
					{
						sprintf ( E, "%s %s %c\n", Er, S->name, k[j] );
						Eprint ( "<Snbkdata>", E ) ;
					}
				}
			}
			else if (strcmp(key, "D") == 0)
				S->D = atof(v), code[2] = 'D';
			else if (strcmp(key, "T") == 0)
				S->H1 = atof(v), code[3] = 'T';
			else if (strcmp(key, "L") == 0)
				S->W1 = atof(v), code[4] = 'L';
			else if (strcmp(key, "R") == 0)
				S->W2 = atof(v), code[5] = 'R';
			else if (strcmp(key, "B") == 0)
				S->H2 = atof(v), code[6] = 'B';
			else
			{
				sprintf ( E, "%s %s %s\n", Er, name, s) ;
				Eprint ( "<Snbkdata>", E ) ;
			}
			
			if (ce) break;
		}
		S->type = type;
		switch (type)
		{
		case 1: case 5: case 9:
			if (strcmp(code, typstr[type-1]) != 0)
			{
				sprintf(E, "%s %s  type=%d %s\n", Er, name, type, code);
				Eprint ( "<Snbkdata>", E ) ;
			}
			break;
		case 2: case 6:
			if (strcmp(code, typstr[type-1]) != 0)
			{
				for (j=1; j<3; j++)
				{
					if (strcmp(code, typstr[type+j-1]) == 0)
					{
						S->type = type+j;
						break;
					}   
					if (j == 3)
					{
						sprintf(E,"%s %s  type=%d %s\n", Er, name, type, code);
						Eprint ( "<Snbkdata>", E ) ;
					}
				}
			}
			break;
		}
		/*************************
		printf("** %s type=%d ksi=%d  WxH=%lf x %lf   D=%lf\n",
		S->name,S->type,S->ksi,S->W,S->H,S->D);
		printf("      W1=%lf  W2=%lf   H1=%lf   H2=%lf\n",
		S->W1,S->W2,S->H1,S->H2);
		/************************************************/

		S++ ;
	}
	i++ ;
	Snbk[0]->end = i;
}   

/************************************************************/

int	BMLSTrealloc ( BMLST **W, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	BMLST	*Buf, *B ;
	
	Buf = NULL ;
	c = 1 ;
	
	if ( NN > 0 )
		Buf = ( BMLST * ) realloc ( *W, NN * sizeof ( BMLST )) ;
	
	if ( Buf == NULL )
	{
		c = 0 ;
		*W = NULL ;
	}
	else
	{
		*W = Buf ;
		/***********************/
		B = *W + N ;
		
		for ( i = N; i < NN; i++, B++ )
		{
			B->mcode = NULL ;
			B->Cond = B->Cro = 0.0 ;
		}
		/************************/
	}
	
	return c ;
}

/************************************************************/

int	WELMrealloc ( WELM **W, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	WELM	*Buf, *B ;
	
	Buf = B = NULL ;
	c = 1 ;
	
	if ( NN > 0 )
	{
		if ( N > 0 )
			Buf = ( WELM * ) realloc ( *W, NN * sizeof ( WELM )) ;
		else
			Buf = ( WELM * ) malloc ( NN * sizeof ( WELM )) ;
	}
	
	if ( Buf == NULL )
	{
		c = 0 ;
		*W = NULL ;
	}
	else
	{
		B = Buf + N ;
		
		for ( i = N; i < NN; i++, B++ )
		{
			//			B->cap = 0.0 ;
			B->code = NULL ;
			B->L = -999.0 ;
			B->ND = 0 ;
			//			B->res = 0.0 ;
		}
		
		*W = Buf ;
	}
	
	return c ;
}

/************************************************************/

int	WINDOWrealloc ( WINDOW **W, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	WINDOW	*Buf, *B ;
	
	Buf = B = NULL ;
	c = 1 ;
	
	if ( NN > 0 )
	{
		if ( N > 0 )
			Buf = ( WINDOW * ) realloc ( *W, NN * sizeof ( WINDOW )) ;
		else
			Buf = ( WINDOW * ) malloc ( NN * sizeof ( WINDOW )) ;
	}
	
	if ( Buf == NULL )
	{
		c = 0 ;
		*W = NULL ;
	}
	else
	{
		B = Buf + N ;
		
		for ( i = N; i < NN; i++, B++ )
		{
			B->name = NULL ;
			B->Ei = B->Eo = 0.9 ;
			B->Ag = B->Ao = B->as = B->Bn = B->H = 0.0 ;
			B->K = B->Rwall = B->tgtn = B->W = 0.0 ;
			B->end = 0 ;
		}
		
		for ( i = 0; i < NN; i++ )
			W[i] = &Buf[i];
	}
	return c ;
}

/************************************************************/

int	SNBKrealloc ( SNBK **W, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	SNBK	*Buf, *B ;
	
	Buf = B = NULL ;
	c = 1 ;
	
	if ( NN > 0 )
	{
		if ( N > 0 )
			Buf = ( SNBK * ) realloc ( *W, NN * sizeof ( SNBK )) ;
		else
			Buf = ( SNBK * ) malloc ( NN * sizeof ( SNBK )) ;
	}
	
	if ( Buf == NULL )
	{
		c = 0 ;
		*W = NULL ;
	}
	else
	{
		B = Buf + N ;
		
		for ( i = N; i < NN; i++, B++ )
		{
			B->name = NULL ;
			B->W = B->H = 0.0 ;
			B->D = B->W1 = B->W2 = B->H1 = B->H2 = 0.0 ;
			B->end = B->type = B->ksi = 0 ;
		}
		
		for ( i = 0; i < NN; i++ )
			W[i] = &Buf[i];
	}
	return c ;
}

/************************************************************/

int	WALLrealloc ( WALL **W, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	WALL	*Buf, *B ;
	
	Buf = B = NULL ;
	c = 1 ;
	
	if ( NN > 0 )
	{
		if ( N > 0 )
			Buf = ( WALL * ) realloc ( *W, NN * sizeof ( WALL )) ;
		else
			Buf = ( WALL * ) malloc ( NN * sizeof ( WALL )) ;
	}
	
	if ( Buf == NULL )
	{
		c = 0 ;
		*W = NULL ;
	}
	else
	{
		B = Buf + N ;
		
		for ( i = N; i < NN; i++, B++ )
		{
			B->name = NULL ;
			B->ble = ' ' ;
			B->N = B->M = B->mp = B->end = 0 ;
			B->Ip = -1 ;
//			B->welm = NULL ;
			B->Ei = B->Eo = 0.9 ;
			B->as = 0.7 ;
			B->Rwall = B->effpnl = 0.0 ;
			B->CAPwall = -999.0;
			B->res = B->cap = NULL ;
		}
		
		for ( i = 0; i < NN; i++ )
			W[i] = &Buf[i];
	}
	return c ;
}

/************************************************************/

int		wbmlistcount ( FILE *fi )
{
	int		N = 0 ;
	char	s[SCHAR], c ;
	
	while ( fscanf ( fi, "%s", s ) != EOF, *s != '*' )
	{
		if ( strcmp(s,"!") == 0)
		{
			while ( fscanf ( fi,"%c", &c), c != '\n' )
				;
		}
		
		else
		{
			N++ ;
			fscanf ( fi, "%*s %*s" ) ;
		}
	}
	
	rewind ( fi ) ;
	return ( N ) ;
}

/************************************************************/

int		Wallcount ( FILE *fi )
{
	int		N = 0 ;
	long	add ;
	char	s[SCHAR] ;
	
	add = ftell ( fi ) ;
	
	while ( fscanf ( fi, " %s ", s ) != EOF )
	{
		if ( *s == '*' )
			break ;
		if ( *s == ';' )
			N++ ;
	}
	
	fseek ( fi, add, SEEK_SET ) ;
	return ( N ) ;
}
