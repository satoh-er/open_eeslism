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

/*  rmdata.c   */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "esize.h"
#include "fesy.h"
#include "fnbld.h"
#include "fnesy.h"
#include "fnlib.h"
#include "psy.h"
#include "fnfio.h"
//#include "control.h"

/* -------------------------------------------- */

/*  室構成部材の入力  */


void Roomdata (FILE *fi, char *errkey, EXSF *Exs, DFWL *dfwl, 
			   RMVLS *Rmvls, SCHDL *Schdl, SIMCONTL *Simc )
{
	WALL	*Wall, *w ;
	WINDOW	*Window, *W ;
	SNBK	*Snbk, *S ;   
	ROOM	*Room, *room, *Rm, *Rc, *Rmchk ;
	RDPNL	*rdpnl, *Rd ;
	//AIRFLOW	*airflow, *Af ;
	TRNX	*trnx, *Tn;
	RPANEL	*rmpnl, *Rp ;
	//RAIRFLOW	*rairflow, *Ra ;

	RMSRF	*Sd, *rsd, *nxsd, *Sdj ;   
	int		Nroom = 0, Nrdpnl = 0, Nsrf = 0;
	int		N, Nnxrm ;
	int		Nwindow, Nwall, Nexs, Nsnbk ;
	int		Nairflow = 0;
	EXSF	*e ;

	SCH		*Scw, *Sch;
	double	*vall;

	ACHIR	*Ac ;
	extern double  ca, roa; 
	extern int	DEBUG,NSTOP ;

	int		i= -1, j, jj, J, n= -1, nr, nxnm= -1, nf, brs=0, ij, bre, N2, k, l ;
	double  Area;
	int		sk, si, rc ;
	char	c, s[SCHAR], ss[SCHAR], *st, *ce, *stt,
		**nxrmname, dexsname[SCHAR], dnxrname[SCHAR],
		err[SCHAR], Er[SCHAR], sfemark, RmnameEr[SCHAR] ;
	double	X, Y ;
	double	Wi, H, D ;
	char	*ast ;

	stt = NULL;
	//sprintf(s, "No. 1") ;
	//HeapCheck(s) ;

	N = Roomcount ( fi ) ;
	nxrmname = NULL ;

	//printf ( "Nroom=%d\n", N ) ;

	if ( N > 0 )
	{
		if (( Rmvls->Room = ( ROOM * ) malloc ( N * sizeof ( ROOM ))) == NULL )
			Ercalloc ( N, "<Roomdata> Room alloc" ) ;

		Roominit ( N, Rmvls->Room ) ;
	}

	N = Rmsrfcount ( fi ) ;
	Nnxrm = N ;

	//printf ( "Nsrf=%d\n", N ) ;

	if ( N > 0 )
	{
		if (( Rmvls->Sd = ( RMSRF * ) malloc ( N * sizeof ( RMSRF ))) == NULL )
			Ercalloc ( N, "<Roomdata> Sd alloc" ) ;
		if (( nxrmname = ( char ** ) malloc ( Nnxrm * sizeof ( char * ))) == NULL )
			Ercalloc ( N, "<Roomdata> nxrmname alloc" ) ;
		else
		{
			for ( j = 0; j < Nnxrm; j++ )
				nxrmname[j] = NULL ;
		}

		Rmsrfinit ( N, Rmvls->Sd ) ;
	}

	Wall = Rmvls->Wall;
	Window = Rmvls->Window;
	W = Window ;
	Snbk = Rmvls->Snbk;
	Rm = Room = Rmvls->Room;
	/***
	Mw = Rmvls->Mw; ***/

	Sd = Rmvls->Sd;
	rdpnl = Rmvls->Rdpnl;

	Scw = Schdl->Scw ;
	Sch = Schdl->Sch;
	vall = Schdl->val;

	sprintf (Er, ERRFMT, errkey);
	Rm-- ;
	Sd-- ;

	//sprintf(s, "No. 2") ;
	//HeapCheck(s) ;
	while (fscanf(fi, "%s", s), s[0] != '*')
	{
		if ( DEBUG )
			printf ( "%s\n", s ) ;
		/*****************************/

		strcat ( strcpy ( err, Er ), s ) ; 

		i++ ;
		Rm++ ;

		//		if ( Roomrealloc ( Room, i, i + 1 ) == 0 )
		//			printf ( "<Roomdata> Room メモリの最確保に失敗\n" ) ;

		Rm->name = stralloc ( s);

		Rmchk = Rmvls->Room ;
		for ( l = 0; l < i - 1; l++, Rmchk++ )
		{
			if ( strcmp ( Rm->name, Rmchk->name ) == 0 )
			{
				sprintf ( RmnameEr, "Room=%s is already defined name", Rm->name ) ;
				Eprint ( "<Roomdata>", RmnameEr ) ;
			}
		}

		Rc = Rmvls->Room ;
		for ( rc = 0; rc < i - 1; rc++, Rc++ )
		{
			if ( Rm->name != NULL && Rc->name != NULL
				&& strcmp ( Rm->name, Rc->name ) == 0 )
			{
				sprintf ( ss, "<ROOM>  RoomName Already Defined  (%s)", Rm->name ) ;
				Eprint ( "<Roomdata>", ss ) ;
			}
		}

		nr = - 1 ;

		/*-------  Satoh DEBUG  1998/5/15 -------------*/
		/*************************
		Rm->rpnl = NULL ;
		*************************/

		sfemark = ' ' ;

		//		Rm->metsc = Rm->closc = Rm->wvsc = -1;

		while (fscanf(fi, "%s", s), s[0] != '#' && (s[0] != '*' || strlen(s) != 1))
		{
			if ( DEBUG )
				printf("Roomdata  s=%s\n", s);

			strcat(strcpy(err, Er), s); 
			if ( *s != '-' )
			{
				if (strcmp(s, "*s") == 0)
					Rm->sfpri = 'p';
				else if (strcmp(s, "*q") == 0)
					Rm->eqpri = 'p';
				else if (strcmp(s, "*sfe") == 0)
					sfemark = 'y';
				else if ((st = strchr(s,':')) != 0)
				{
					*st = '\0';
					if ( *s == '(' )
						sscanf(&s[1], "%[^)])", dnxrname);
					else
						strcpy(dexsname, s);
				}
				else if (strcmp(s, "Fij") == 0)
				{
					Rm->fij = 'F';
					fscanf(fi, "%d", &N2);
					Rm->F = dcalloc(N2 * N2, " xxxxx Roomdata F");

					ij=0;
					while (fscanf(fi, "%s", ss), ss[0] != ';')
					{
						sscanf(ss, "%lf", &Rm->F[ij]);
						ij++ ;
					}
				}

				else if (strcmp(s, "rsrnx") == 0)
					Rm->rsrnx = 'y';

				else
				{
					st = strchr(s, '=');
					*st = '\0';

					if (strcmp(s, "Vol") == 0)
					{
						if (( ast = strchr ( st + 1, '*' )) == NULL )
							Rm->VRM = atof(st+1);
						else
						{
							sscanf ( st + 1, "%lf*%lf*%lf", &Wi, &H, &D ) ;
							Rm->VRM = Wi * H * D ;
						}
					}
					else if (strcmp(s, "flrsr") == 0)
					{
						free(Rm->flrsr);
						Rm->flrsr = NULL;
						if ((k = idsch(st + 1, Schdl->Sch, NULL)) >= 0)
							Rm->flrsr = &vall[k];
						else
							Rm->flrsr = envptr(st + 1, Simc, 0, NULL, NULL, NULL);
					}
						//Rm->flrsr = atof(st+1);

					else if(strcmp(s, "alc") == 0)
					{
						if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
							Rm->alc = &vall[k];
						else
							Rm->alc = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
					}

					else if (strcmp(s, "Hcap") == 0)
						Rm->Hcap = atof(st + 1);	  
					else if (strcmp(s, "Mxcap") == 0)
						Rm->Mxcap = atof(st + 1);	  
					else if (strcmp(s, "MCAP") == 0)
					{
						// 室内家具の熱容量の設定
						if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
							Rm->MCAP = &vall[k] ;
						else
							Rm->MCAP = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
					}
					else if (strcmp(s, "CM") == 0)
					{
						// 室内家具の熱コンダクタンスの設定
						if ((k = idsch(st + 1, Schdl->Sch, NULL)) >= 0)
							Rm->CM = &vall[k];
						else
							Rm->CM = envptr(st + 1, Simc, 0, NULL, NULL, NULL);
					}
					// 家具への日射吸収割合
					else if (strcmp(s, "fsolm") == 0)
					{
						if ((k = idsch(st + 1, Schdl->Sch, NULL)) >= 0)
							Rm->fsolm = &vall[k];
						else
							Rm->fsolm = envptr(st + 1, Simc, 0, NULL, NULL, NULL);
					}
					//{
					//	Rm->fsolm = atof(st + 1);
					//}
					else if (strcmp(s, "PCMFurn") == 0)
					{
						// PCM内臓家具の場合　(PCMname,mPCM)
						int kk;
						char *PCMname,*stbuf;
						*(st + 1) = '\0';
						PCMname = st + 2;
						stbuf = stralloc(PCMname);
						st = strchr(PCMname, ',');
						*st = '\0';
						Rm->PCMfurnname = stralloc(PCMname);
						st = strchr(stbuf, ')');
						*(st + 1) = '\0';
						st = strchr(stbuf, ',');
						*(st + 1) = '\0';
						Rm->mPCM = atof(st + 2);

						PCM *PCM;
						PCM = Rmvls->PCM;
						for (kk = 0; kk < Rmvls->Npcm; kk++, PCM++)
						{
							if (strcmp(Rm->PCMfurnname, PCM->name) == 0)
								Rm->PCM = PCM;
						}
						if (Rm->PCM == NULL)
						{
							sprintf(Er, "Roomname=%s %sが見つかりません", Rm->name, Rm->PCMfurnname);
							Eprint(Er, "<Roomdata>");
							//if (NSTOP == 0)
								preexit();
						}
					}
					else if (strcmp(s, "OTc") == 0)
					{
						// 作用温度設定時の対流成分重み係数の設定
						if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
							Rm->OTsetCwgt = &vall[k] ;
						else
							Rm->OTsetCwgt = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
					}
					else
					{
						sprintf ( err, "Room=%s s=%s", Rm->name, s ) ;
						Eprint ( "<Roomdata>", err ) ;
					}
				}
			}
			else 
			{
				++n ;
				++nr;
				Sd++ ;
				//printf ( "n=%d\n", n ) ;
				/********************************
				Sd->name = NULL;
				Sd->alicsch = NULL;
				/********************************/

				Sd->ble = c = *(s+1) ;

				/*****************************
				Sd->exs = -1;
				Sd->sb = -1;
				Sd->nxrm = -1;
				Sd->nxn = -1;
				Sd->c = -1.0;
				Sd->Nfn = 0;
				Sd->wd = -1;
				Sd->rpnl = NULL;
				Sd->Te = 0.0 ;
				Sd->Tmrt = 0.0 ;
				Sd->RS = 0.0 ;

				Sd->A = -999.0;
				Sd->alic = 0.0;
				Sd->Rwall = 0.0;
				Sd->mwside = 'i';
				Sd->mwtype = 'I';
				Sd->room = Sd->nextroom = NULL;
				Sd->nxsd = NULL;
				Sd->WSC = 0.0 ;
				/*************************************/

				Sd->sfepri = sfemark ; 
				Sd->sname = NULL ;			       
				/*****************************
				for (k=0; k<4; k++)
				Sd->fnd[k] = 0 ;
				/********************************/

				while (fscanf(fi, "%s", s), s[0] != ';')
				{
					if ( DEBUG )
						printf("Roomdata1  s=%s\n", s);

					strcat(strcpy(err, Er), s); 

					//printf ( "aaaaaa\n" ) ;

					if ((ce= strchr(s, ';')) != 0)
						*ce = '\0';

					//printf ( "bbbbbbbbbbb\n" ) ;

					if ((st=strchr(s,'=')) == 0) 
					{ 
						if ( DEBUG )
							printf("Roomdata2  s=%s\n", s);
						/*******************/

						if (strcmp(s, "*p") == 0)
							Sd->wlpri = 'p';
						else if (strcmp(s, "*sfe") == 0)
							Sd->sfepri = 'y';
						// 日よけの影面積出力
						else if (strcmp(s, "*shd") == 0)
							Sd->shdpri = 'p';

						else if (( st = strchr(s, '*')) != NULL || sscanf(s, "%lf", &Sd->A) != 0 )
						{
							if ( Sd->A < 0.0 )
							{
								sscanf ( s, "%lf*%lf", &X, &Y ) ;
								Sd->A = X * Y ;
							}
						}
						// 動的にカーテンを開閉するロジックを追加  2012/2/25 Satoh
						else if ( strcmp ( s, "if" ) == 0 )
						{
							// 制御構造体のメモリ確保
							if (( Sd->ctlif = ( CTLIF * ) calloc ( 1, sizeof(CTLIF))) ==0)
								Ercalloc(1, "<Roomdata> CTLIF" ) ;

							// ifの最後まで読み込む
							fscanf(fi, " (%[^)])", ss) ;
							Sd->dynamiccode = stralloc(ss) ;

							// IF文の展開
							//ctifdecode(s, Sd->ctlif, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl);

							// Trueの時の窓の読み込み
							fscanf(fi, "%s", s) ;
							Nwindow = Window->end ;
							W = Window ;
							for (j = 0; j < Nwindow; j++, W++)
							{
								if (W->name != NULL && strcmp(W->name, s) == 0)
								{
									Sd->ifwin = W;
									Sd->Rwall = W->Rwall;
									Sd->CAPwall = 0.0;
									break;
								}
							}
							if (j == Nwindow)
							{
								sprintf(err, "Room=%s <window> %s", Rm->name, stt);
								Eprint("<Roomdata>", err);
								//getch();
								//if (NSTOP == 0)
								preexit();
							}

							// Falseの時の窓の読み込み
							//fscanf(fi, "%*s %s", s) ;
							//Nwindow = Window->end ;
							//W = Window ;
							//for ( j = 0; j < Nwindow; j++, W++ ) 
							//{
							//	if ( W->name != NULL && strcmp ( W->name, s ) == 0 )
							//	{
							//		Sd->falsewin = W ;
							//		Sd->Rwall = W->Rwall ;
							//		break; 
							//	}
							//}
							//if ( j == Nwindow )
							//{
							//	sprintf (err, "Room=%s <window> %s", Rm->name, stt ) ;
							//	Eprint ( "<Roomdata>", err ) ;
							//}
						}
						else
						{
							if ( DEBUG )
								printf("Roomdata3  s=%s  c=%c\n", s, c );
							/**********************/

							if (c == 'W')
							{
								nf = Sd->Nfn ;
								Sd->Nfn++ ;

								if ((stt=strchr(s, ':')) != 0)
								{
									stt++ ;
									Sd->fnmrk[nf]=s[0];
								}
								else
									stt=s;

								Nwindow = Window->end ;
								W = Window ;
								for (j = 0; j < Nwindow; j++, W++)
								{
									if (W->name != NULL && stt != NULL
										&& strcmp(W->name, stt) == 0)
									{
										Sd->window = W;
										Sd->fnd[nf] = j;
										Sd->Rwall = W->Rwall;
										Sd->CAPwall = 0.0;
										Sd->RStrans = W->RStrans;
										break;
									}
								}
								if (j == Nwindow)
								{
									sprintf(err, "Room=%s <window> %s", Rm->name, stt);
									Eprint("<Roomdata>", err);
									//getch();
									//if (NSTOP == 0)
									preexit();
								}
							}
							else
							{ 
								Nwall = Wall->end ;
								w = Wall ;
								//if (Sd->ble == 'E')
								//	printf("aaaa\n");
								//printf ( "Nwall=%d\n", Nwall ) ;
								for ( j = 0; j < Nwall; j++, w++ )
								{
									if ( DEBUG )
										printf ( "!!!!Wall.name=%s  s=%s!!!!\n", w->name, s ) ;
									/***************************/

									if ( w->name != NULL && strcmp ( w->name, s ) == 0
										&& w->ble == Sd->ble )
									{
										if ( DEBUG )
											printf ( "---- j=%d Wallname=%s n=%d\n", j, w->name, n ) ;
										/**************************/

										Sd->wd = j;
										Sd->Rwall = w->Rwall ;
										Sd->CAPwall = w->CAPwall;

										// PCMフラグ
										Sd->PCMflg = w->PCMflg ;
										break;
									}
								}
								if ( j == Nwall )
								{
									sprintf ( err, "Room=%s <wall> ble=%c %s Undefined in <WALL>", Rm->name, Sd->ble, s ) ;
									Eprint ( "<Roomdata>", err ) ;
									//getch();
									//if (NSTOP == 0)
										preexit();
								}
							}
						}
					}
					else
					{ 
						//printf ( "st=%s  Sd->name=%s\n", st, Sd->name ) ;

						if ( strncmp ( s, "A=", 2 ) == 0)
							Sd->A = atof ( st + 1 ) ;
						else if ( strncmp ( s, "e=", 2 ) == 0 )
						{
							Nexs = Exs->end ;
							e = Exs ;
							for ( j = 0; j < Nexs; j++, e++ )
							{ 
								if (( J = strcmp ( e->name, st+1 )) == 0 )
								{
									Sd->exs= j;
									break;
								}
							}
							if ( j == Nexs )
							{
								sprintf ( err, "Room=%s <exsrf> %s\n", Rm->name, s ) ;
								Eprint ( "<Roomdata>", err ) ;
								//getch();
								//if (NSTOP==0)
									preexit();;
							}
						}
						else if ( strncmp ( s, "sb=", 3 ) == 0 )
						{
							S = Snbk ;
							Nsnbk = S->end ;
							for ( j = 0; j < Nsnbk; j++, S++ )
							{ 
								if ( strcmp ( S->name, st + 1 ) == 0 )
								{
									Sd->sb= j;
									break;
								}
							}
							if ( j == Nsnbk ) 
							{
								sprintf ( err, "Room=%s <Snbrk> %s\n", Rm->name, s ) ;
								Eprint ( "<Roomdata>", err ) ;
								//getch();
								//if (NSTOP == 0)
									preexit();
							}
						}
						else if ( strncmp ( s, "r=", 2 ) == 0 )
						{
							nxnm++ ;
							Sd->nxrm = nxnm ;
							//strcpy(nxrmname[nxnm], st+1);
							nxrmname[nxnm] = stralloc ( st + 1 ) ;
						}
						else if ( strncmp ( s, "c=", 2 ) == 0 )
							Sd->c = atof ( st + 1 ) ;

						else if ( strncmp ( s, "sw=", 3 ) == 0 )
							Sd->fnsw = idscw ( st + 1, Scw, err ) ;

						else if ( strncmp ( s, "i=", 2 ) == 0 )
						{
							//printf ( "cccccccc\n" ) ;
							Sd->name = stralloc(st + 1);
							//printf ( "dddddddddd\n" ) ;
						}
						else if (strncmp(s, "alc=", 4) == 0)
						{
							if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
								Sd->alicsch = &vall[k];
							else
								Sd->alicsch = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
						}
						else if (strncmp(s, "alr=", 4) == 0)
						{
							if (( k = idsch ( st + 1, Schdl->Sch, NULL )) >= 0 )
								Sd->alirsch = &vall[k];
							else
								Sd->alirsch = envptr ( st + 1, Simc, 0, NULL, NULL, NULL ) ;
						}
						else if (strncmp(s, "fsol=", 5) == 0)
						{
							Rm->Nfsolfix++ ;
							Sd->ffix_flg = '*';
							if ((k = idsch(st + 1, Schdl->Sch, NULL)) >= 0)
								Sd->fsol = &vall[k];
							else
								Sd->fsol = envptr(st + 1, Simc, 0, NULL, NULL, NULL);
							//Sd->fsol = atof(st + 1) ;
						}
						/*----------higuchi 7.12------------start-*/
						else if (strncmp(s, "rmp=",4) == 0)
						{
							Sd->sname = stralloc(s + 4);
							/*------------
							printf("Sd[%d].sname=%s\n",n,Sd[n].sname) ;
							------------*/
						}
						/*----------higuchi 7.12-----------end--*/
						else if (strncmp(s, "PVcap=", 6) == 0)
						{
							Sd->PVwall.PVcap = atof(st + 1) ;
							Sd->PVwallFlg = 'Y' ;
						}
						// 屋根一体型空気集熱器の通気層上面の幅
						else if (strncmp(s, "Wsu=", 4) == 0)
							Sd->dblWsu = atof(st + 1) ;
						// 屋根一体型空気集熱器の通気層下面の幅
						else if (strncmp(s, "Wsd=", 4) == 0)
							Sd->dblWsd = atof(st + 1) ;
						else if (strncmp(s, "Ndiv=", 5) == 0)
						{
							Sd->Ndiv = atoi(st + 1) ;
							Sd->Tc = dcalloc(Sd->Ndiv,"Sd->Tc") ;
						}
						// 当該部位への入射日射の隣接空間への日射分配（連続空間の隣室への日射分配）
						else if (strncmp(s, "tnxt=", 5) == 0)
							Sd->tnxt = atoi(st + 1);
						else
						{
							sprintf ( err, "Room=%s ble=%c s=%s\n",
								Rm->name, Sd->ble, s ) ;
							Eprint ( "<Roomdata>", err ) ;
							//getch();
							preexit();
						}
					} 
					if ( ce )
						break;
				}

				Sd->rm = i;
				Sd->room = Rm ;
				Sd->n = nr;

				/***********************************
				for ( t = 0; t < 10; t++ )
				printf ( "<rmdata>2=== t=%d  Wall.name=%s  Wall.Ei=%lf\n",
				t, Wall->name, Wall->Ei ) ;
				*****************************/

				switch (Sd->ble)
				{
				case 'E':  case 'R':  case 'F': case 'W':
					if (Sd->exs == -1)
					{
						if ( Exs != NULL )
							Nexs = Exs->end ;
						else
						{
							printf ( "EXSRFが未定義です。\n" ) ;
							Nexs = 0 ;
							//getch();
							preexit();
						}

						e = Exs ;
						for ( j = 0; j < Nexs; j++, e++ )
						{ 
							if (( J = strcmp ( e->name, dexsname )) == 0 )
							{
								Sd->exs= j ;
								break; 
							}
						}
						if ( j == Nexs )
						{
							sprintf ( err, "Room=%s  (%s)\n", Rm->name, dexsname ) ;
							Eprint ( "<Roomdata>", err ) ;
							//getch();
							preexit();
						}
					}
					break ;
				case 'i':  case 'c':  case 'f':
					//printf ( "nxrm=%d Sd->c=%lf dnxrname=%s\n", Sd->nxrm, Sd->c, dnxrname ) ;
					if ( Sd->nxrm == -1 && Sd->c < 0.0)
					{ 
						nxnm++ ;
						Sd->nxrm = nxnm;
						//strcpy(nxrmname[nxnm], dnxrname);
						nxrmname[nxnm] = stralloc ( dnxrname ) ;
					}
					/*****************************/
					if(Sd->c < 0.0)
						Sd->c = 1.0;
					/*****************************/
					break;
				}
				if (Sd->ble != 'W' && Sd->wd == -1 && Sd->A > 0.0)
				{
					switch(Sd->ble)
					{
					case 'E':
						Sd->wd = dfwl->E; break;
					case 'R':
						Sd->wd = dfwl->R; break;
					case 'F':
						Sd->wd = dfwl->F; break;
					case 'i':
						Sd->wd = dfwl->i; break;
					case 'c':
						Sd->wd = dfwl->c; break;
					case 'f':
						Sd->wd = dfwl->f; break;
					}
				}   
				/********************************
				if (Sd->ble == 'F' || Sd->ble == 'f')
				Sd->srg = Sd->srh = Sd->srl = Sd->sra = 1.0;
				else
				Sd->srg = Sd->srh = Sd->srl = Sd->sra = 0.0;
				**********************************/

				// 窓の場合
				if (Sd->ble == 'W')
				{
					Sd->typ = 'W';
					Sd->wd = -1;  

					Sd->tnxt = 0.;
				}
				// 窓以外の場合
				else
				{
					j = Sd->wd;

					//printf ( "Sd->room=%s\tSd->wd=%d\tSd->exs=%d\n",
					//	Sd->room->name, Sd->wd, Sd->exs ) ;
					if (( jj = Sd->exs ) >= 0 && (Exs+jj)->typ == 'E')
						Sd->typ = 'E';
					else if (( jj = Sd->exs ) >= 0 && (Exs+jj)->typ == 'e')
						Sd->typ = 'e';
					else
						Sd->typ = 'H';

					if ( j >= 0 )
					{
						w = Wall + j ;
						//printf ( "w->name=%s\tw->ble=%c\n", w->name, w->ble ) ;
						Sd->Eo = w->Eo;
						Sd->Ei = w->Ei;
						Sd->as = w->as;
						Sd->fn = -1; 
						Sd->Rwall = w->Rwall ;
						Sd->CAPwall = w->CAPwall;
						Sd->PCMflg = w->PCMflg ;
						if (Sd->tnxt < 0.)
							Sd->tnxt = w->tnxt;
						Sd->tnxt = max(Sd->tnxt, 0.);
						//printf("tnxt=%lf\n", Sd->tnxt);
					}

					/****************************
					printf ( "<rmdata>=== n=%d  j=%d  Sd.name=%s  Sd.Ei=%lf  Wall.name=%s  Wall.Ei=%lf\n",
					n, j, Sd->name, Sd->Ei, w->name, w->Ei ) ;
					/*****************************/

					/****************
					if (w->Ip >= 0)
					{
					Rm->Nrp = 1;
					Rm->rpnl = rdpnl;
					Sd->rpnl = rdpnl;
					rdpnl->name = Sd->name;
					rdpnl->rm = Room + i;
					rdpnl->rmsf = Sd + n;
					rdpnl->effpnl = w->effpnl;
					rdpnl++;
					} 
					******************/
				}
			}
		}

		nr++ ;
		Rm->N = nr ;

		N2 = nr * nr;
		if (Rm->fij != 'F')
			Rm->F = dcalloc(N2, " xxxxx Roomdata F");
		Rm->alr = dcalloc(N2, " xxxxx Roomdata alr");
		Rm->XA = dcalloc(N2, " xxxxx Roomdata XA");
		Rm->Wradx = dcalloc(N2, " xxxxx Roomdata Wradx");

		Rm->brs = brs;
		Rm->rsrf = Rmvls->Sd + brs ;
		brs += nr;

		//      if (brs > NSMX)
		//		  printf("%s total_rmsurfs=%d [max=%d]\n", Er, brs, NSMX);

		Rm->GRM = roa * Rm->VRM + Rm->Mxcap;
		Rm->MRM = ca * roa * Rm->VRM + Rm->Hcap ;

		/***
		Rm->Area = Area;***/

		/******* 
		printf("<<Roomdata>> i=%d %s  MRM=%lf  GRM=%lf\n", 
		i, Rm->name, Rm->MRM, Rm->GRM);
		*******/ 


	}
	i++ ;
	Nroom = Rmvls->Room->end = i;
	//   if (i > ROOMMX)
	//	   printf("%s rmname=%d [max=%d]\n", Er, i, ROOMMX);

	//printf ( "Nroom=%d\n", Nroom ) ;

	//sprintf(s, "No. 3") ;
	//HeapCheck(s) ;

	Sd = Rmvls->Sd ;
	n++ ;
	Nsrf = Sd->end = n;

	//printf ( "Nsrf=%d\n", Nsrf ) ;
	Room = Rmvls->Room ;

	nxnm++;
	for ( n = 0; n < Nsrf; n++, Sd++ )
	{
		if (( J = Sd->nxrm) >= 0)
		{
			strcat ( strcpy ( err, Er ), nxrmname[J] ) ;
			Sd->nxrm = idroom ( nxrmname[J], Room, err ) ;
			Sd->nextroom = Room + Sd->nxrm ;
		}
	}

	/******* 個別内壁 *****/

	Sd = Rmvls->Sd ;
	Rm = Rmvls->Room ;
	for ( n = 0; n < Nsrf; n++, Sd++ )
	{
		if (( i = Sd->nxrm) >= 0 )
		{
			Room = Rm + i ;
			brs = Room->brs;
			bre = brs + Room->N ;
			Sdj = Rmvls->Sd + brs ;

			switch ( Sd->ble )
			{
			case 'i':
				for ( j = brs; j < bre; j++, Sdj++ )
				{
					if ( Sdj->nxrm == Sd->rm && Sdj->ble == 'i' )
						Sd->nxn = j ;
				}
				break;
			case 'c':
				for ( j = brs; j < bre; j++, Sdj++ )
				{
					if ( Sdj->nxrm == Sd->rm && Sdj->ble == 'f' )
						Sd->nxn = j ;
				}
				break;
			case 'f':
				for ( j = brs; j < bre; j++, Sdj++ )
				{
					if ( Sdj->nxrm == Sd->rm && Sdj->ble == 'c' )
						Sd->nxn = j ;
				}
				break;
			}
		}
	}

	//sprintf(s, "No. 4") ;
	//HeapCheck(s) ;
	/***** 共用内壁 ******/

	rsd = Rmvls->Sd;
	Sd = Rmvls->Sd ;
	Room = Rmvls->Room ;
	for (n = 0; n < Nsrf; n++, rsd++)
	{
		//printf ( "n=%d Nsrf=%d\n", n, Nsrf ) ;
		if ((rsd->ble == 'i' || rsd->ble == 'c' 
			|| rsd->ble == 'f') && rsd->mwtype != 'C')
		{
			/*****************
			printf("------ Roomdata   n=%d sd->name=%s  wd=%d A=%lf\n", 
			n, rsd->name, rsd->wd, rsd->A);
			/****************/

			if (rsd->name != NULL)
			{
				/*********************************************
				printf ( "wd=%d A=%lf\n", rsd->wd, rsd->A ) ;
				/**********************************************/

				if (rsd->wd >= 0 && rsd->A >0.0)
				{
					nxsd = Rmvls->Sd;
					for (i = 0; i < Nsrf; i++, nxsd++)
					{
						if (nxsd->name != NULL && nxsd->A < 0.0)
						{
							/**************
							printf("------ Roomdata  i=%d nxsd->name=%s\n", 
							i, nxsd->name);
							/**************/

							if (strcmp(rsd->name, nxsd->name) == 0 && rsd != nxsd)
							{
								/**************
								printf("------ Roomdata COMMON WALL  room=%s nxroom=%s sd=%s\n",
								rsd->room->name, nxsd->room->name, rsd->name);
								/***************/

								rsd->room->Ntr++ ;
								nxsd->room->Ntr++;

								nxsd->nextroom = rsd->room;
								nxsd->nxsd = rsd;
								nxsd->A = rsd->A;

								nxsd->Ei = rsd->Eo;
								nxsd->Eo = rsd->Ei;
								nxsd->as = rsd->as;
								nxsd->Rwall = rsd->Rwall ;
								nxsd->CAPwall = rsd->CAPwall;

								nxsd->wd = rsd->wd;
								nxsd->mwside = 'M';
								rsd->mwtype = nxsd->mwtype = 'C';		  
								nxsd->pcmpri = rsd->pcmpri;
								nxsd->PCMflg = rsd->PCMflg;

								nxsd->tnxt = rsd->tnxt;

								//PCMSTATE	*pcm1, *pcm2;
								//pcm1 = nxsd->pcmstate;
								//pcm2 = rsd->pcmstate;
								//if (rsd->PCMflg == 'Y')
								//{
								//	for (int j = 0; j < rsd->mw->M;j++,pcm1++,pcm2++)
								//		if(pcm2->name!=NULL)
								//			pcm1->name = stralloc(pcm2->name);
								//}

								/************************
								printf ( "<Roomdata>  name=%s  wd=%d  Ei=%lf\n",
								rsd->name, rsd->wd, rsd->Ei ) ;
								/**************************/

								rsd->nextroom = nxsd->room;
								rsd->nxsd = nxsd;

								if (rsd->ble == 'i')
									nxsd->ble = 'i';
								else if (rsd->ble == 'f')
									nxsd->ble = 'c'; 
								else if (rsd->ble == 'c')
									nxsd->ble = 'f'; 

								rsd->nxrm = (int)(rsd->nextroom - Room);
								//printf ( "rsd->nxrm=%d\n", rsd->nxrm ) ;
								rsd->nxn = (int)(rsd->nxsd - Sd);
								//printf ( "rsd->nxn=%d\n", rsd->nxn ) ;
								nxsd->nxrm = (int)(nxsd->nextroom - Room);
								//printf ( "nxsd->nxrm=%d\n", nxsd->nxrm ) ;
								nxsd->nxn = (int)(nxsd->nxsd - Sd);
								//printf ( "nxsd->nxn=%d\n", nxsd->nxn ) ;
								/****
								nxsd->rmw = nxsd->mw - Mw; *******/

								break;
							}
						}
					}

					if (i == Nsrf)
						printf("name=%s 共用内壁が片側しか定義されていません。\n", rsd->name);
				}
			}

			if (rsd->nxn < 0 && rsd->mwtype == 'C')
			{
				sprintf(err, "%s    room=%s  xxx  (%s):  -%c\n", Er, (Room+rsd->rm)->name,
					(Room+rsd->nxrm)->name, rsd->ble);
				Eprint ( "<Roomdata>", err ) ;
				//getch();
				preexit();
			}
		}
	}

	// 面積入力のチェック
	rsd = Rmvls->Sd ;
	for (i = 0; i < Nsrf; i++, rsd++)
	{
		if (rsd->A <= 0.0)
		{
			printf("Room=%s  ble=%c  A=%lf\n", rsd->room->name, rsd->ble, rsd->A);
			//getch();
			preexit();
		}
	}

	//sprintf(s, "No. 5") ;
	//HeapCheck(s) ;
	/***** 放射パネル総数、室ごとのパネル数 *****/

	rsd = Rmvls->Sd ;
	for (i = 0; i <Nsrf; i++, rsd++)
	{
		//printf ( "n=%d Nsrf=%d\n", i, Nsrf ) ;
		if (rsd->ble != 'W')
		{
			w = Wall + rsd->wd ;
			if (w->Ip >= 0)
			{
				/************
				printf("rsd->name=%s  wd=%d  Wall= %s Ip=%d mwside=%c\n",
				rsd->name, rsd->wd, w->name, w->Ip, rsd->mwside );
				/********************/

				rsd->room->Nrp++;

				if (rsd->mwside == 'i')
					Nrdpnl++;
			}
		}
		else
		{
			// エアフローウィンドウの総数を数える
			//if ( rsd->window->AirFlowFlg == 'Y' )
			//	rsd->room->Nairflow++ ;

			Nairflow++ ;
		}
	}
	room = Rmvls->Room;
	for (i = 0; i < Nroom; i++, room++)
	{
		//printf ( "i=%d Nroom=%d\n", i, Nroom ) ;

		N = room->Ntr ;
		if (N > 0 )
		{
			if ((room->trnx = (TRNX *)malloc(N * sizeof(TRNX))) == NULL)
				Ercalloc(room->Ntr, errkey);
		}

		if ( room->trnx != NULL )
		{
			Tn = room->trnx ;

			for ( sk = 0; sk < N; sk++, Tn++ )
			{
				Tn->nextroom = NULL ;
				Tn->sd = NULL ;
			}
		}

		room->ARN = dcalloc(room->Ntr, errkey);

		N = room->Nrp ;
		//printf("room=%s Nrp=%d\n", room->name, room->Nrp) ;
		if (N > 0 )
		{
			if ((room->rmpnl = (RPANEL *)malloc( N * sizeof(RPANEL))) == NULL)
				Ercalloc(room->Nrp, errkey);
		}

		if ( room->rmpnl != NULL )
		{
			Rp = room->rmpnl ;

			for ( sk = 0; sk < N; sk++, Rp++ )
			{
				Rp->pnl = NULL ;
				Rp->sd = NULL ;
				Rp->elinpnl = 0 ;
			}
		}

		//N = room->Nairflow ;
		//if (N > 0)
		//{
		//	if (( room->rairflow = (RAIRFLOW *)malloc(N * sizeof(RAIRFLOW))) == NULL)
		//		Ercalloc(room->Nairflow, errkey) ;
		//}

		//if (room->rairflow != NULL)
		//{
		//	Ra = room->rairflow ;

		//	for ( sk = 0; sk < N; sk++, Ra++ )
		//	{
		//		Ra->airflow = NULL ;
		//		Ra->sd = NULL ;
		//		Ra->elinairflow = 0 ;
		//	}
		//}

		room->RMP = dcalloc(room->Nrp, errkey);
	}

	//sprintf(s, "No. 5") ;
	//HeapCheck(s) ;
	N = Nrdpnl ;
	if (N > 0 )
	{
		if ((Rmvls->Rdpnl = (RDPNL *)malloc(N * sizeof(RDPNL))) == NULL)
			Ercalloc(N, errkey);
	}

	if ( Rmvls->Rdpnl != NULL )
	{
		Rd = Rmvls->Rdpnl ;

		for ( sk = 0; sk < N; sk++, Rd++ )
		{
			Rd->name = NULL ;
			Rd->cmp = NULL ;
			Rd->MC = Rd->eprmnx = Rd->epwtw = 0 ;
			Rd->loadt = NULL ;
			Rd->Toset = -999.0 ;
			Rd->cG = 0.0 ;
			Rd->Ec = 0.0 ;
			Rd->OMvav = NULL ;
			mtEdayinit(Rd->mtPVdy) ;

			for ( si = 0; si < 2; si++ )
			{
				Rd->rm[si] = NULL ;
				Rd->sd[si] = NULL ;
				Rd->Ntrm[si] = Rd->Nrp[si] = Rd->elinpnl[si] = 0 ;
			}
		}
	}

	//N = Nairflow ;
	//if (N > 0 )
	//{
	//	if ((Rmvls->airflow = (AIRFLOW *)malloc(N * sizeof(AIRFLOW))) == NULL)
	//		Ercalloc(N, errkey);
	//}

	//if ( Rmvls->airflow != NULL )
	//{
	//	Af = Rmvls->airflow ;

	//	for ( sk = 0; sk < N; sk++, Af++)
	//	{
	//		Af->name = NULL ;
	//		Af->cG = 0.0 ;
	//		Af->cmp = NULL ;
	//		Af->loadt = NULL ;
	//		Af->rm = NULL ;
	//		Af->Toset = -999. ;
	//		Af->Ew = 0. ;
	//	}
	//}

	//sprintf(s, "No. 6") ;
	//HeapCheck(s) ;
	rsd = Rmvls->Sd;
	for (i = 0; i < Nsrf; i++, rsd++)
	{   
		//printf ( "i=%d Nsrf=%d\n", i, Nsrf ) ;
		rsd->WSRN = dcalloc(rsd->room->Ntr, errkey);
		rsd->WSPL = dcalloc(rsd->room->Nrp, errkey);
	}

	rdpnl = Rmvls->Rdpnl;
	//airflow = Rmvls->airflow ;
	room = Rmvls->Room;
	for (i = 0; i < Nroom; i++, room++)
	{

		//printf ( "i=%d Nroom=%d\n", i, Nroom ) ;
		room->Nisidermpnl = 0;
		trnx = room->trnx;
		rmpnl = room->rmpnl;
		//rairflow = room->rairflow ;
		rsd = room->rsrf;
		for (n = 0; n < room->N; n++, rsd++)
		{     
			//sprintf(s, "No. 1 i=%d n=%d", i, n) ;
			//HeapCheck(s) ;

			if (rsd->mwtype == 'C')
			{
				//printf("nextroomName=%s\n", rsd->nextroom->name) ;
				trnx->nextroom = rsd->nextroom;
				trnx->sd = rsd;
				trnx++;
			}
			//sprintf(s, "No. 2 i=%d n=%d", i, n) ;
			//HeapCheck(s) ;
			if (rsd->ble != 'W')
			{
				w = Wall + rsd->wd ;
				if (w->Ip >= 0)
				{
					if (rsd->mwside == 'i')
					{  
						// 建材一体型空気集熱器の判定
						// 2009/01/26  Satoh Debug
						if ( w->tra > 0. )
							rdpnl->type = 'C' ;
						else
							rdpnl->type = 'P' ;

						/*****rsd->rpnl = rdpnl;  ****/

						//sprintf(s, "No. 3 i=%d n=%d", i, n) ;
						//HeapCheck(s) ;
						rsd->rpnl = rdpnl;
						rdpnl->name = rsd->name;
						rdpnl->effpnl = w->effpnl;
						rdpnl->MC = 1;

						rdpnl->rm[0] = rsd->room;
						rdpnl->sd[0] = rsd;
						rdpnl->Ntrm[0] = rsd->room->Ntr;
						rdpnl->Nrp[0] = rsd->room->Nrp;

						/**************	   
						printf(" --- Roomdata  panel i=%d  rsd->rm=%d rdpnl=%s\n",
						i, rsd->rm, rdpnl->name);
						**************/

						rmpnl->pnl = rdpnl;
						rmpnl->sd = rsd;

						rmpnl->elinpnl = rdpnl->elinpnl[0] = 1 + 1 + rdpnl->Ntrm[0];
						rmpnl++;
						room->Nisidermpnl++;

						//sprintf(s, "No. 4 i=%d n=%d", i, n) ;
						//HeapCheck(s) ;
						/******************   
						printf(" --- Roomdata  panel n=%d  room=%s rdpnl=%s  rsd=%d\n",
						n, (Room+rsd->rm)->name, (Room+rsd->rm)->rmpnl->pnl->name,
						(Room+rsd->rm)->rmpnl->sd - (Room+rsd->rm)->rsrf);
						/***************/




						if (rsd->mwtype == 'C')
						{
							rdpnl->MC = 2;
							nxsd = rsd->nxsd;

							rdpnl->rm[1] = nxsd->room;
							rdpnl->sd[1] = nxsd;
							rdpnl->Ntrm[1] = nxsd->room->Ntr;
							rdpnl->Nrp[1] = nxsd->room->Nrp;
							rdpnl->elinpnl[1] = 1 + 1 + rdpnl->Ntrm[0]
							+ rdpnl->Nrp[0] + 1 + rdpnl->Ntrm[1];
							/***** 
							(Room+nxsd->rm)->rmpnl->pnl = rdpnl;
							(Room+nxsd->rm)->rmpnl->sd = nxsd;
							(Room+nxsd->rm)->rmpnl->elinpnl = 

							(Room+nxsd->rm)->rmpnl++; 
							*********/

						}

						for (j = 0; j < rdpnl->MC; j++)
						{
							rdpnl->EPR[j] = dcalloc(rdpnl->Ntrm[j], errkey);
							rdpnl->EPW[j] = dcalloc(rdpnl->Nrp[j], errkey);
						}
						rdpnl++ ;
					}
				}
			}
			else
			{
				// エアフローウィンドウの計算準備
				//if ( rsd->window->AirFlowFlg == 'Y' )
				//{
				//	rsd->airflow = airflow ;
				//	airflow->name = rsd->name ;
				//	airflow->sd = rsd ;
				//	airflow->rm = rsd->room ;

				//	rairflow->airflow = airflow ;
				//	rairflow->sd = rsd ;
				//	rairflow++ ;
				//}
			}
		}
		//free(Rmvls->Qrm) ;
		//printf("Memory Alloc Test%d\n", i ) ;
		//if (( Rmvls->Qrm = ( QRM * ) malloc ( 2 * sizeof ( QRM ))) == NULL )
		//	Ercalloc( 2, "<Roomdata> Qrm alloc test_1" );
		//free(Rmvls->Qrm) ;
		//Rmvls->Qrm = NULL ;
		//sprintf(s, "No. 1  %d end", i) ;
		//HeapCheck(s) ;
	}

	//sprintf(s, "No. 7") ;
	//HeapCheck(s) ;
	//free(Rmvls->Qrm) ;
	//printf("Memory Alloc Test1\n" ) ;
	//if (( Rmvls->Qrm = ( QRM * ) malloc ( 2 * sizeof ( QRM ))) == NULL )
	//	Ercalloc( 2, "<Roomdata> Qrm alloc test1" );

	room = Room;	      
	for (i = 0; i < Nroom; i++, room++)
	{
		/****************
		printf("-----Roomdata %s Nisidermpnl=%d\n", room->name, room->Nisidermpnl);
		/***************/

		rmpnl = room->rmpnl + room->Nisidermpnl;
		rsd = room->rsrf;
		for (n = 0; n < room->N; n++, rsd++)
		{

			if ( rsd->ble != 'W' )
			{
				//printf ( "Room=%s rsd->wd=%d\n", room->name, rsd->wd ) ;
				w = Wall + rsd->wd ;
				/*****************
				printf("-----Roomdata  %s n=%d[%d] rsd->wd=%d rsd->name=%s Ip=%d mwside=%c\n", 
				room->name, n, room->N, rsd->wd, rsd->name, w->Ip, rsd->mwside);
				/****************/

				if (w->Ip > 0 && rsd->mwside == 'M')
				{
					/***************
					printf("-----Roomdata  COMMON panel  nxsd=%s  nxsd->rpnl=%s\n",
					rsd->nxsd->name, rsd->nxsd->rpnl->name);
					/******************/

					rsd->rpnl = rsd->nxsd->rpnl;
					rmpnl->pnl = rsd->rpnl;

					/****************
					printf("-----Roomdata  rmpnl->pnl=%s\n", rmpnl->pnl->name);
					/*********************/

					rmpnl->sd = rsd;
					rmpnl->elinpnl = rsd->rpnl->elinpnl[1];
					rmpnl++;
				}	
			}
		}
	}

	Rm = Rmvls->Room ;
	for (i = 0; i < Nroom; i++, Rm++)
	{
		if (Nroom > 0 )
		{
			if ((Rm->achr = (ACHIR *)malloc(Nroom * sizeof(ACHIR))) == 0)
				Ercalloc ( Nroom, "<Roomdata>  Room->achr alloc" ) ;
		}

		if ( Rm->achr != NULL )
		{
			Ac = Rm->achr ;

			for ( sk = 0; sk < Nroom; sk++, Ac++ )
			{
				Ac->rm = Ac->sch = 0 ;
				Ac->room = NULL ;
			}
		}

		Rm->Nachr = 0;

		Rm->arsp = NULL;
		Rm->rmld = NULL; 

		Area = 0.0;
		Rm->FArea = 0.0;
		rsd = Rm->rsrf;

		for (j = 0; j < Rm->N; j++, rsd++)
		{
			Area += rsd->A;

			if (rsd->ble == 'F' || rsd->ble == 'f')
			{
				Rm->Nflr++ ;
				Rm->FArea += rsd->A;
			}
		}
		Rm->Area = Area;

		//// 室内部位の日射吸収比率の計算
		//// 2017/12/25毎時計算へ変更
		//// 家具の日射吸収割合
		//Rm->tfsol = 0.0;
		//if (Rm->fsolm != NULL)
		//	Rm->tfsol = *(Rm->fsolm);

		//rsd = Rm->rsrf ;
		//for (j = 0; j < Rm->N; j++, rsd++)
		//{
		//	// 床の場合
		//	if (rsd->ble == 'F' || rsd->ble == 'f')
		//	{
		//		// どの部位も日射吸収比率が定義されていない場合
		//		if (Rm->Nfsolfix == 0)
		//		{
		//			// 床の日射吸収比率は固定
		//			rsd->ffix_flg = '*' ;
		//			rsd->fsol = dcalloc(1, "<Roomdata> dcalloc");
		//			*(rsd->fsol) = *(Rm->flrsr) * rsd->A / Rm->FArea ;
		//		}
		//	}

		//	// fsolが規定されている部位についてfsolを合計する
		//	if (rsd->ffix_flg == '*')
		//		Rm->tfsol += *(rsd->fsol) ;		// fsolの合計値計算
		//}

		if (Rm->fij != 'F')
		{
			Rm->fij = 'A';
			// 形態係数の近似計算（面積割）
			formfaprx(Rm->N, Area, Rmvls->Sd + Rm->brs, Rm->F);
		}
		/***********      
		printf(" --- Roomdata  F[i,j]\n"); 
		matprint(" %6.4lf", Rm->N, Rm->F); 
		*********************/

	}
	Rmvls->Nroom = Nroom;
	Rmvls->Nsrf = Nsrf;
	Rmvls->Sd->end = Nsrf;
	Rmvls->Trdav = dcalloc ( Rmvls->Nroom, "<Roomdata> Trdav alloc" ) ;
	Rmvls->Nrdpnl = Nrdpnl;
	//Rmvls->Nairflow = Nairflow ;

	i = _heapmin() ;
	
	N = Rmvls->Nroom ;
	//printf("N=%d i=%d\n", N, i ) ;
	if (N > 0 )
	{
		if (( Rmvls->Qrm = ( QRM * ) malloc ( N * sizeof ( QRM ))) == NULL )
			Ercalloc(Rmvls->Nroom, "<Roomdata> Qrm alloc" );
	}
	if (N > 0 )
	{
		if (( Rmvls->Qrmd = ( QRM * ) malloc ( N * sizeof ( QRM ))) == NULL )
			Ercalloc(Rmvls->Nroom, "<Roomdata> Qrmd alloc" );
	}

	Rmvls->Emrk = scalloc ( Rmvls->Nroom, "<Roomdata> Emrk alloc" ) ;
	strcpy ( Rmvls->Emrk, "" ) ;

	for ( i = 0; i < Nnxrm; i++ )
		free ( nxrmname[i] ) ;

	free ( nxrmname ) ;
	/*********
	if (Nrdpnl > RDPNLMX)
	printf("%s rdpanel=%d [max=%d]\n", Er, Nrdpnl, RDPNLMX);  ******/   

	/***************
	printf(" xxxxx Roomdata xxxxxx\n"); 
	dprroomdata (Rmvls->Room, Rmvls->Sd);
	/**************/
}

/* ------------------------------------------------------------- */

/*  重量壁体の計算準備      */

void Balloc (int N, RMSRF *Sd, WALL *Wall, MWALL **Mwall, int *Nmwall)
{
	RMSRF	*ssd;
	int		n, mw, id, m, M;
	char	Er[SCHAR];
	double	*res, *cap, *wres, *wcap ;
	MWALL	*mwl, *Mw ;
	WALL	*W ;
	int		sn ;
	PCMSTATE	*pcmstate;
	PCM		*PCM;

	ssd = NULL ;
	*Er = '\0' ;

	sprintf (Er, ERRFMT, "(Balloc)");

	mw = 0 ;
	M = 0 ;

	ssd = Sd ;
	for ( n = 0; n < N; n++, ssd++ )
	{
		if (( id = ssd->wd ) >= 0 && ssd->mwside == 'i' )
			mw++ ;
	}

	if ( mw > 0 )
	{
		if (( *Mwall = ( MWALL * ) malloc ( mw * sizeof ( MWALL ))) == NULL )
			Ercalloc ( mw, "<Balloc> Mw alloc" ) ;

		mwl = *Mwall ;
		for ( n = 0; n < mw; n++, mwl++ )
		{
			mwl->sd = NULL ;
			mwl->nxsd = NULL ;
			mwl->wall = NULL ;
			mwl->ns = mwl->rm = mwl->n = mwl->nxrm = mwl->nxn
				= mwl->M = mwl->mp = mwl->end = 0 ;
			mwl->UX = mwl->res = mwl->cap = mwl->Tw = mwl->Told = NULL ;
			mwl->uo = mwl->um = mwl->Pc = 0.0 ;
		}
	}

	mw = 0 ;
	ssd = Sd ;
	Mw = *Mwall ;
	for ( n = 0; n < N; n++, ssd++ )     
	{
		/**********
		printf("xxx Balloc  Sd->wd=%d  Sd->mwside=%c\n", 
		ssd->wd, ssd->mwside); ***********/

		if ( (id = ssd->wd) >= 0 && ssd->mwside == 'i')
		{ 
			/********
			printf("xxx Balloc  id=%d  Sd->mwside=%c\n", 
			id, ssd->mwside); *************/

			ssd->rmw = mw;
			mwl = Mw + mw ;
			W = Wall + id ;
			ssd->mw = mwl ;			// 壁体構造体のポインタ

			mwl->wall = W ;

			// 太陽光発電付のチェック
			sn = 0 ;
			if ( ssd->mw->wall->ColType != NULL )
				sn = (int)strlen(ssd->mw->wall->ColType) ;
			if(sn == 2 || sn == 3 && ssd->mw->wall->ColType[2] != 'P')
			{
				ssd->PVwallFlg = 'N' ;
				
				// 太陽電池の容量が入力されているときにはエラーを表示する
				if ( ssd->PVwall.PVcap > 0. )
				{
					printf("<%s> name=%s PVcap=%g ですが、WALLで太陽電池付が指定されていません\n", 
						ssd->room->name, ssd->name, ssd->PVwall.PVcap ) ;
					ssd->PVwall.PVcap = -999.0 ;
					//getch();
					preexit();
				}
			}

			mwl->sd = ssd ;
			mwl->nxsd = ssd->nxsd;

			mwl->ns = n;
			mwl->rm = ssd->rm;
			mwl->n = ssd->n;
			mwl->nxrm = ssd->nxrm;
			mwl->nxn = ssd->nxn; 
			M = mwl->M = W->M;
			mwl->mp = W->mp;

			if ( mwl->res == NULL )
				mwl->res = dcalloc ( M+2, "<Balloc> Mw->res" ) ;

			if ( mwl->cap == NULL )
				mwl->cap = dcalloc ( M+2, "<Balloc> Mw->cap" ) ;

			wres = W->res ;
			wcap = W->cap ;
			res = mwl->res ;
			cap = mwl->cap ;
			for (m = 0; m <= M; m++, wres++, wcap++, res++, cap++ )
			{
				*res = *wres ;
				*cap = *wcap ; 
			}

			if (ssd->typ == 'H')
			{
				M++ ;
				mwl->M = M;
				mwl->res[M] = mwl->cap[M] = 0.0;
			}

			mwl->UX = dcalloc(M*M, " xxxxx Balloc  UX");

			// PCM状態値を保持する構造体
			ssd->pcmstate = malloc((M+1) * sizeof(PCMSTATE));
			//PCM = *(mwl->wall->PCM);
			pcmstate = ssd->pcmstate;
			//if (ssd->name != NULL && strcmp(ssd->name, "IwhN10toA15") == 0)
			//	printf("fffff\n");
			for (m = 0; m <= M; m++, pcmstate++)
			{
				PCM = mwl->wall->PCMLyr[m];
				pcmstate->name = NULL;
				pcmstate->capmL = pcmstate->capmR = pcmstate->lamdaL = pcmstate->lamdaR = pcmstate->tempPCMave = 0.0;
				pcmstate->oldcapmL = pcmstate->oldcapmR = pcmstate->oldlamdaL = pcmstate->oldlamdaR = 0.0;
				pcmstate->tempPCMNodeL = pcmstate->tempPCMNodeR = 0.;
				if (PCM != NULL)
				{
					pcmstate->name = PCM->name;
					ssd->Npcm++;
					if (ssd->wlpri == 'p')
						ssd->pcmpri = 'y';
				}
			}

			int	prev_layer, start_layer;
			int	k;
			start_layer = prev_layer = -999;
			k = 0;
			//printf("ble=%c name=%s\n", W->ble, W->name);
			mw++;
			//			if (mw > MWALLMX)
			//				printf("%s heavy_wall = %d [max-%d]\n", Er, mw, MWALLMX);
		}
		else
			ssd->mw = NULL;

	}
	*Nmwall = Mw->end = mw;

	char	hptest[SCHAR];
	sprintf(hptest, "Main3");
	HeapCheck(hptest);
	ssd = Sd;
	for (n = 0; n < N; n++, ssd++)
	{
		if (ssd->mwside == 'M')
		{
			ssd->mw = ssd->nxsd->mw;
			M = ssd->mw->M;
			ssd->rmw = ssd->nxsd->rmw;

			ssd->PCMflg = ssd->nxsd->PCMflg;
			ssd->pcmpri = ssd->nxsd->pcmpri;
			ssd->Npcm = ssd->nxsd->Npcm;
			// PCM状態値を保持する構造体
			//ssd->pcmstate = malloc(M * sizeof(PCMSTATE));
			//PCM = *(mwl->wall->PCM);
			PCMSTATE *nxpcm,*nxpcm2;
			pcmstate = ssd->pcmstate;
			nxpcm2 = ssd->nxsd->pcmstate;
			nxpcm = nxpcm2 + M ;
			for (m = 0; m <= M; m++, pcmstate++,nxpcm--,nxpcm2++)
			{
				pcmstate = nxpcm;
				//PCM = ssd->mw->wall->PCM[m];
				//pcmstate->name = NULL;
				//pcmstate->capm = pcmstate->lamda = pcmstate->tempPCM = 0.0;
				//if (PCM != NULL)
				//{
				//	pcmstate->name = PCM->name;
				//	ssd->Npcm++;
				//	if (ssd->wlpri == 'p')
				//		ssd->pcmpri = 'y';
				//}
			}
		}
	}

	/************
	printf("xxxxx Balloc xxxxx\n");
	dprballoc(Mw, Sd); 
	**********************/
} 
/* ------------------------------------------ */      

/*  壁体内部温度の初期値設定   */


void Tinit (double Tini, int Nroom, ROOM *Room,
			int Ns, RMSRF *S, int Nmwall, MWALL *Mw)
{
	int		i, j, m;
	RMSB	*rmsb;
	RMSRF	*Sd;
	MWALL	*mw;
	BHELM	*Told, *Tw;
	ROOM	*rm ;
	double	*TW, *TO, *TOd, *Twd;

	/****** printf("xxxxx Tinit  Tini=%lf\n", Tini); *******/

	rm = Room ;
	for ( i = 0; i < Nroom; i++, rm++ )
	{ 
		rm->Tr = rm->Trold = rm->Tsav = rm->Tot = Tini;
		rm->xr = rm->xrold = FNXtr(rm->Tr, 50.0);
		rm->hr = FNH(rm->Tr, rm->xr) ;
		rm->alrbold = -999.0;
		rm->mrk = '*';	
		rm->TM = rm->oldTM = Tini ;
	}

	Sd = S ;
	for ( i = 0; i < Ns; i++, Sd++ )
	{
		Sd->Ts = Tini;

		Sd->mrk = '*';
	}

	mw = Mw ;
	for ( i = 0; i < Nmwall; i++, mw++ )
	{
		mw->Tw = dcalloc ( mw->M, "<Tinit> Mw->Tw" ) ;
		mw->Told = dcalloc ( mw->M, "<Tinit> Mw->Told" ) ;
		mw->Toldd = dcalloc(mw->M, "<Tinit> Mw->Toldd");
		mw->Twd = dcalloc(mw->M, "<Tinit> Mw->Toldd");

		TW = mw->Tw ;
		TO = mw->Told ;
		TOd = mw->Toldd;
		Twd = mw->Twd;
		for (m = 0; m < mw->M; m++, TW++, TO++, TOd++)
			*TW = *TO = *TOd = *Twd = Tini;
	}

	/* ========================================= */   

	for (i = 0; i < Nroom; i++, Room++)
	{
		/******printf("Tinit xxx  Room=%d\n", i);*****/

		if (Room->rmqe != NULL)
		{
			rmsb = Room->rmqe->rmsb ;
			Sd = Room->rsrf ;
			for (j = 0; j < Room->N; j++, rmsb++, Sd++)
			{
				if ((mw = Sd->mw) != NULL)
				{
					Told = rmsb->Told;
					Tw = rmsb->Tw;
					for (m = 0; m < mw->M; m++, Told++, Tw++)
					{
						helmclear ( Told ) ;
						Told->trs = Tini;		  		 		  		  
						helmcpy ( Told, Tw ) ;		
					}
				}
			}
		}
	}
}

/********************************************************************

int	Roomrealloc ( ROOM *R, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	ROOM	*Buf, *B ;

	Buf = B = NULL ;
	c = 1 ;

	if ( NN > 0 )
	{
		if ( N > 0 )
		{
			if (( R = ( ROOM * ) realloc ( R, NN * sizeof ( ROOM ))) == NULL )
			{
				c = 0 ;
				//			Buf = NULL ;
				R = NULL ;
			}
		}
		else
			R = ( ROOM * ) malloc ( NN * sizeof ( ROOM )) ;
	}

	if ( R == NULL )
	{
		c = 0 ;
		R = NULL ;
	}
	else
	{
		B = R + N ;

		for ( i = N; i < NN; i++, B++ )
		{
			B->name = NULL ;
			B->N = B->brs = B->Nachr = B->Ntr = B->Nrp = 0 ;
			B->Nisidermpnl = B->Nasup = 0 ;
			B->rsrf = NULL ;
			B->achr = NULL ;
			B->trnx = NULL ;
			B->rmpnl = NULL ;
			B->arsp = NULL ;
			B->cmp = NULL ;
			B->elinasup = NULL ;
			B->elinasupx = NULL ;
			B->rmld = NULL ;
			B->rmqe = NULL ;
			B->F = NULL ;
			B->alr = NULL ;
			B->XA = NULL ;
			B->Wradx = NULL ;
			B->rsrnx = 'n' ;
			B->fij = B->sfpri = B->eqpri = ' ' ;
			B->mrk = ' ' ;
			B->VRM = B->GRM = B->MRM = B->Area = B->FArea = 0.0 ;
			B->flrsr = 0.3 ;
			B->alrbold = B->Hcap = B->Mxcap = 0.0 ;
			B->Ltyp = ' ' ;
			B->Nhm = B->Light = B->Apsc = B->Apsr = B->Apl = 0.0 ;
			B->Gve = B->Gvi = 0.0 ;
			B->alc = NULL ;
			//B->vesc = B->visc = 0 ;
			B->Vesc = B->Visc = NULL ;
			//B->hmnsc = B->lgtsc = B->apssc = B->aplsc = 0 ;
			//B->hmwksc = B->metsc = B->closc = B->wvsc = -1 ;
			B->Hc = B->Hr = B->HL = B->Lc = B->Lr = B->Ac = B->Ar = 0.0 ;
			B->AL = 0.0 ;
			B->eqcv = 0.5 ;
			B->Qeqp = B->Gvent = B->RMt = 0.0 ;
			B->ARN = NULL ;
			B->RMP = NULL ;
			B->RMC = B->RMx = B->RMXC = B->Tr = B->Trold = B->xr = 0.0 ;
			B->xrold = B->RH = B->Tsav = B->Tot = B->PMV = 0.0 ;
			B->end = 0 ;
		}

		//		for ( i = 0; i < NN; i++ )
		//			R = Buf;
	}
	return c ;
}

/********************************************************************/

int		Roomcount ( FILE *fi )
{
	int		N ;
	long	ad ;
	char	s[SCHAR] ; // ss[SCHAR] ;

	N = 0 ;
	ad = ftell ( fi ) ;

	while ( fscanf ( fi, "%s", s ) != EOF && *s != '*' )
	{
		N++ ;

		while ( fscanf ( fi, "%s", s ) != EOF )
		{
			if ( strcmp ( s, "*" ) == 0 || strcmp ( s, "#" ) == 0 )
				break ;
		}
	}

	fseek ( fi, ad, SEEK_SET ) ;
	return ( N ) ;
}

/********************************************************************/

void	Roominit ( int N, ROOM *Room )
{
	int		i ;
	ROOM	*B ;

	B = Room ;

	for ( i = 0; i < N; i++, B++ )
	{
		B->name = NULL ;
		B->PCM = NULL;
		B->PCMfurnname = NULL;
		B->mPCM = -999.;
		B->FunHcap = -999.0;
		B->PCMQl = -999.0;
		B->N = B->brs = B->Nachr = B->Ntr = B->Nrp = 0 ;
		B->Nflr = 0 ;
		B->Nfsolfix = 0 ;
		B->Nisidermpnl = B->Nasup = 0 ;
		B->brs = B->N = 0;
		//B->Nairflow = 0 ;
		B->rsrf = NULL ;
		B->achr = NULL ;
		B->trnx = NULL ;
		B->rmpnl = NULL ;
		B->arsp = NULL ;
		B->cmp = NULL ;
		B->elinasup = NULL ;
		B->elinasupx = NULL ;
		B->rmld = NULL ;
		B->rmqe = NULL ;
		B->F = NULL ;
		B->alr = NULL ;
		B->XA = NULL ;
		B->Wradx = NULL ;
		B->rsrnx = 'n' ;
		B->fij = B->sfpri = B->eqpri = ' ' ;
		B->mrk = ' ' ;
		B->VRM = B->GRM = B->MRM = B->Area = B->FArea = 0.0 ;
		B->flrsr = dcalloc(1,"<Roominit> dcalloc") ;
		*(B->flrsr) = 0.3;
		B->tfsol = 0.0;
		B->alrbold = B->Hcap = B->Mxcap = 0.0 ;
		B->Ltyp = ' ' ;
		B->Nhm = B->Light = B->Apsc = B->Apsr = B->Apl = 0.0 ;
		B->Gve = B->Gvi = 0.0 ;
		B->alc = NULL ;
		B->Vesc = B->Visc = NULL ;
		//B->vesc = B->visc = 0 ;
		// B->hmwksc = B->hmnsc = B->lgtsc = B->apssc = B->aplsc = 0 ;
		//B->metsc = B->closc = B->wvsc = -1 ;
		B->Hc = B->Hr = B->HL = B->Lc = B->Lr = B->Ac = B->Ar = 0.0 ;
		B->AL = 0.0 ;
		B->eqcv = 0.5 ;
		B->Qeqp = B->Gvent = B->RMt = 0.0 ;
		B->ARN = NULL ;
		B->RMP = NULL ;
		B->RMC = B->RMx = B->RMXC = B->Tr = B->Trold = B->xr = 0.0 ;
		B->xrold = B->RH = B->Tsav = B->Tot = B->PMV = 0.0 ;
		B->end = 0 ;
		B->AEsch = B->AGsch = NULL ;
		B->AE = B->AG = 0.0 ;
		B->Assch = B->Alsch = B->Lightsch = NULL ;
		B->Hmsch = B->Metsch = B->Closch = B->Wvsch = NULL ;
		B->Hmwksch = NULL ;
		B->VAVcontrl = NULL ;
		B->OTsetCwgt = NULL ;		// 作用温度設定時の対流成分重み係数
		//B->rairflow = NULL ;
		B->MCAP = B->CM = NULL ;
		B->QM = 0.0;
		B->HM = 0.0 ;
		B->fsolm = NULL;
		B->Srgm2 = 0.0;
		B->TM = B->oldTM = 15.0 ;
		B->SET = -999.;
		B->setpri = 0;
	}
}

/************************************************************************/

int		Rmsrfcount ( FILE *fi )
{
	int		N ;
	long	ad ;
	char	s[SCHAR] ;

	N = 0 ;
	ad = ftell ( fi ) ;

	while ( fscanf ( fi, "%s", s ) != EOF )
	{
		if ( strcmp ( s, "*" ) == 0 || strcmp ( s, "#" ) == 0 )
			break ;

		while ( fscanf ( fi, "%s", s ) != EOF )
		{
			if ( *s == '-' )
				N++ ;
			if ( strcmp ( s, "*" ) == 0 || strcmp ( s, "#" ) == 0 )
				break ;
		}
	}

	fseek ( fi, ad, SEEK_SET ) ;
	return ( N ) ;
}

/************************************************************************/

void	Rmsrfinit ( int N, RMSRF *S )
{
	int		i, j ;
	int		*f, *g ;

	for ( i = 0; i < N; i++, S++ )
	{
		S->ctlif = NULL;
		S->ifwin = NULL;
		S->name = NULL ;
		S->room = S->nextroom = NULL ;
		S->dynamiccode = NULL;
		S->nxsd = NULL ;
		S->mw = NULL ;
		S->rpnl = NULL ;
		S->pcmstate = NULL;
		S->Npcm = 0;
		S->Nfn = 0;
		S->pcmpri = ' ';
		S->Rwall = -999.0 ;
		S->CAPwall = -999.;
		S->A = S->Eo = S->as = S->c = S->tgtn = S->Bn = S->srg = S->srh = S->srl = 0.0 ;
		S->sra = S->alo = S->ali = S->alic = S->alir = S->K = S->FI = 0.0 ;
		S->FO = S->FP = S->CF = S->WSR = S->WSC = S->RS = S->RSsol = S->RSin = S->RSli = 0.0 ;
		S->Qi = 0.0 ;
		S->Qga = S->Qgt = 0.;
		S->TeEsol = S->TeErn = S->Te = S->Tmrt = S->Ei = S->Ts = S->eqrd = 0.0 ;
		S->alicsch = S->WSRN = S->WSPL = NULL ;

		S->exs = S->sb = S->nxrm = S->nxn = S->wd = - 1 ;
		S->fn = -1 ;
		S->c = - 1.0 ;
		S->A = - 999.0 ;
		//		S->Rwall = 0.0 ;
		S->mwside = 'i' ;
		S->mwtype = 'I' ;
		strcpy ( S->fnmrk, "" ) ;
		S->alirsch = NULL ;
		S->ffix_flg = '!' ;
		S->fsol = NULL ;

		S->ColCoeff = -999. ;
		S->oldTx = 20.0 ;
		S->Iw = 0.0 ;
		//S->Scol = 0.0 ;
		S->PVwall.Eff = 0.0 ;
		S->PVwallFlg = 'N' ;
		S->PVwall.PVcap = -999. ;
		S->Ndiv = 0 ;
		S->Tc = NULL ;
		S->dblWsd = S->dblWsu = -999.0 ;
		S->dblTf = S->dblTsd = S->dblTsu = 20.0 ;
		S->ras = -999. ;
		S->Tg = 20. ;

		S->tnxt = -999.;
		S->RStrans = 'n';

		S->wlpri = S->shdpri = ' ';
		S->Iwall = 0.0;
		S->fnsw = 0;
		f = S->direct_heat_gain;
		g = S->fnd;
		for (j = 0; j < 10; j++, f++, g++)
		{
			*f = 0;
			*g = 0;
		}
	}
}
