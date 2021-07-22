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

/*   ee_cmpdat_s.c    */

/*   システム要素の入力 */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnlib.h"
#include "fnesy.h"
#include "winerror.h"
#include "fnfio.h"
#include "psy.h"

void Compodata(FILE *f, char *errkey, RMVLS *Rmvls, EQCAT *Eqcat, 
			   COMPNT **Cmp, int *Ncompnt, EQSYS *Eqsys, int *Ncmpalloc, int ID )
{
	int		Nroom=0;
	ROOM	*Room; 	    
	int		Nrdpnl=0; 
	int		Nairflow = 0 ;
	RDPNL	*Rdpnl;    
	//AIRFLOW	*AirFlow ;
	COMPNT	*cmp, *Crm, *cm, *C, *Compnt, *cp ;
	ROOM	*room;
	int		i, j, Ni=0, No=0, Ncrm = 0, ncmp=0 ;
	char	s[SCHAR], *st, cio, *ps, idi[SCHAR], ido[SCHAR] ; //, ss[SCHAR] ;
	int		N ;
	HCC		*Hcc ;
	BOI		*Boi ;
	COLL	*Coll ;
	REFA	*Rf ;
	PIPE	*Pi ;
	STANK	*St ;
	HEX		*Hx ;
	PUMP	*Pp ;
	FLIN	*Fl ;
	HCLOAD	*Hl ;
	VAV		*V ;
	STHEAT	*Sh ;
	THEX	*Thex ;
	VALV	*Valv ;
	QMEAS	*Qmeas ;
	PV		*PV ;
	int		Ncmp, D ;
	OMVAV	*OMvav ;
	DESI	*Desi ;
	EVAC	*Evac ;

	D = 0 ;

	Nroom = Rmvls->Nroom;

	if ( Nroom > 0 )
		Room = Rmvls->Room;
	else
		Room = NULL ;

	Nrdpnl = Rmvls->Nrdpnl;

	if ( Nrdpnl > 0 )
		Rdpnl =Rmvls->Rdpnl;
	else
		Rdpnl = NULL ;

	//Nairflow = Rmvls->Nairflow ;
	//if ( Nairflow > 0)
	//	AirFlow = Rmvls->airflow ;
	//else
	//	AirFlow = NULL ;

	Ncmp = Compntcount ( f ) ;
	Ncmp += 2 + Nroom + Nrdpnl ; // + Nairflow ;
	if (( *Cmp = ( COMPNT * ) malloc ( Ncmp * sizeof ( COMPNT ))) == NULL )
		Ercalloc ( Ncmp, "<Compodata>  Compnt alloc" ) ;
	else
		Compinit ( Ncmp, *Cmp ) ;

	//printf ( "<Compodata> Compnt Alloc=%d\n", Ncmp ) ;
	*Ncmpalloc = Ncmp ;
	cmp = *Cmp ;
	Compnt = *Cmp ;

	/***if ((fi=fopen("bdata.ewk", "r")) == 0) ***/
	if (ferror(f))
	{
		Eprint ( "bdata.ewk", "<Compodata>" ) ;

		preexit ( ) ;
		exit(EXIT_BDATA); 
	}

	/* 給水温度設定 */ 
	Compnt->name = stralloc(CITYWATER_NAME);
	Compnt->eqptype = stralloc(FLIN_TYPE);
	Compnt->tparm = stralloc(CITYWATER_PARM);
	Compnt->Nin = Compnt->Nout = 1 ;
	Compnt++;
	Eqsys->Nflin++;
	D++ ;

	/* 取り入れ外気設定 */
	Compnt->name = stralloc(OUTDRAIR_NAME);
	Compnt->eqptype = stralloc(FLIN_TYPE);
	Compnt->tparm = stralloc(OUTDRAIR_PARM);
	Compnt->Nin = Compnt->Nout = 2 ;
	Compnt++;
	Eqsys->Nflin++;
	D++ ;

	/* 室およびパネル用     */

#if SIMUL_BUILDG /****************/

	/*****printf("<<Compodata>>  Nroom=%d\n", Nroom);*****/

	for (i = 0; i < Nroom; i++, Room++, Compnt++, D++)
	{
		Compnt->name = Room->name;
		Compnt->eqptype = stralloc(ROOM_TYPE);
		Compnt->neqp = i;
		Compnt->eqp = Room;
		Compnt->Nout = 2;       
		Compnt->Nin = 2 * Room->Nachr + Room->Ntr + Room->Nrp;
		Compnt->nivar = 0;

		Compnt->ivparm = NULL;
		Compnt->airpathcpy = 'y';
		//Compnt++;
		//D++ ;
	}
	Ncrm = (int)(Compnt - cmp);

	/****printf("<<Compodata>>  Nrdpnl=%d\n", Nrdpnl);****/

	for (i = 0; i < Nrdpnl; i++, Rdpnl++, Compnt++, D++)
	{
		Compnt->name = Rdpnl->name;
		Compnt->eqptype = stralloc(RDPANEL_TYPE);
		Compnt->neqp = i;
		Compnt->eqp = Rdpnl;

		Compnt->Nout = 2;    

		Compnt->Nin = 3 + Rdpnl->Ntrm[0] + Rdpnl->Ntrm[1] 
		+ Rdpnl->Nrp[0] + Rdpnl->Nrp[1] + 1;
		Compnt->nivar = 0;

		Compnt->airpathcpy = 'y';

		//Compnt++;
		//D++ ;
	} 

	// エアフローウィンドウの機器メモリ
	//for ( i = 0; i < Nairflow; i++, AirFlow++, Compnt++)
	//{
	//	Compnt->name = AirFlow->name ;
	//	Compnt->eqptype = stralloc(AIRFLOW_TYPE) ;
	//	Compnt->neqp = i ;
	//	Compnt->eqp = AirFlow ;

	//	Compnt->Nout = 2 ;
	//	Compnt->Nin = 3 ;
	//	Compnt->nivar = 0;
	//	Compnt->airpathcpy = 'y' ;
	//}
#endif /***********************/

	cp = NULL ;

	if ( ID == 0 )
	{
		while (fscanf(f, "%s", s), *s != '*')
		{
			//Ncrm = Compnt - cmp ;
			cio=' '; 
			Crm = cmp;

			if ( *s == '(' )
			{
				if ( strlen ( s ) == 1 )
					fscanf ( f, "%s", s ) ;
				else
					sscanf ( s, "(%s", s ) ;

				Crm = NULL ;
				Compnt->name = stralloc ( s ) ;
				Compnt++ ;

				fscanf ( f, "%s", s ) ;

				if (( st = strchr ( s, ')' )) != NULL )
				{
					//st-- ;
					*st = '\0' ;
				}

				Compnt->name = stralloc ( s ) ;

				Compnt->valvcmp = Compnt - 1 ;
				(Compnt-1)->valvcmp = Compnt ;

				Eqsys->Nvalv++ ;
				//Compnt++ ;
			}

			if ( Crm )
			{
				for (i = 0; i < Ncrm; i++, Crm++)
				{
					if (Crm->name != NULL && strcmp(s, Crm->name) == 0)
						break;
				}
				if (i == Ncrm)
				{
					Crm = NULL;
					Compnt->name = stralloc(s);
					Compnt->ivparm = NULL;
					Compnt->tparm = Compnt->envname = NULL;
					Compnt->roomname = NULL ;
					Compnt->nivar = 0;
				}
			}

			while (fscanf(f, "%s", s), *s != ';')
			{
				if (*s == '-')
				{ 
					/********************************/
					if (cio == 'i')
					{
						Compnt->Nin = Ni;
						idi[Ni] = '\0';
						Compnt->idi = stralloc(idi);
					}
					else if (cio == 'o')
					{
						Compnt->Nout = No;
						ido[No] = '\0';
						Compnt->ido = stralloc(ido);
						if (Compnt->eqptype != NULL && strcmp(Compnt->eqptype, DIVERG_TYPE) == 0)
						{
							Compnt->nivar = No;
							Compnt->ivparm = dcalloc(No, errkey);
						}
					}
					/********************************/

					ps=s+1;

					if (strcmp(ps, "c") == 0)
						cio = 'c';
					else if (strcmp(ps, "type") == 0)
						cio = 't';
					else if (strcmp(ps, "Nin") == 0)
						cio = 'I';
					else if (strcmp(ps, "Nout") == 0)
						cio = 'O';
					else if (strcmp(ps, "in") == 0)
						cio = 'i', Ni=0;
					else if (strcmp(ps, "out") == 0)
						cio = 'o', No=0;
					else if (strcmp(ps, "L") == 0)
					{
						cio = 'L';
						Compnt->nivar = 1;
						Compnt->ivparm = dcalloc(1, errkey);
					}
					else if (strcmp(ps, "env") == 0)
						cio = 'e';
					else if (strcmp(ps, "room") == 0)
						cio = 'r' ;
					else if (strcmp(ps, "roomheff") == 0)
						cio = 'R' ;
					else if (strcmp(ps, "exs") == 0)
						cio = 's';
					else if (strcmp(ps, "S") == 0)
						cio = 'S';
					else if (strcmp(ps, "Tinit") == 0)
						cio = 'T';
					else if (strcmp(ps, "V") == 0)
						cio = 'V';
					else if (strcmp(ps, "hcc") == 0)
						cio = 'h' ;
					else if (strcmp(ps, "pfloor") == 0)
						cio = 'f' ;
					else if (strcmp(ps, "wet") == 0)
					{
						Compnt->wetparm = stralloc(ps);

						/*---- Roh Debug for a constant outlet humidity model of wet coil  2003/4/25 ----*/
						cio = 'w' ;
						Compnt->ivparm = dcalloc(1, errkey);
						*(Compnt->ivparm) = 90.0;
					}
					else if (strcmp(ps, "control") == 0 )
					{
						cio = 'M' ;
					}
					// Satoh Debug
					else if (strcmp(ps, "monitor") == 0)
					{
						cio = 'm';
					}
					// 電気蓄熱暖房器の潜熱蓄熱材重量（kg）
					else if (strcmp(ps, "PCMweight") == 0)
						cio = 'P';
					else
						Eprint(errkey, s);
				}
				else if ( cio != 'V' && cio != 'S' && (st = strchr(s, '=')) != 0 )
				{
					if (strncmp(s, "Ac", 2) == 0)
						Compnt->Ac = atof(st+1) ;
					else if (strncmp(s, "PVcap", 5) == 0)
						Compnt->PVcap = atof(st+1) ;
					else if (strncmp(s, "Area", 4) == 0)
						Compnt->Area = atof(st+1) ;
				}
				else
				{
					switch (cio)
					{
					case 'c':
						if (eqpcat(s, Compnt, Eqcat, Eqsys))
							Eprint(errkey, s);
						break;	   
					case 't':
						Compnt->eqptype = stralloc(s);

						if ( Compnt->valvcmp )
							Compnt->valvcmp->eqptype = stralloc (s) ;

						Compnt->neqp = 0;
						Compnt->ncat = 0;

						/***
						if (strcmp(s, DIVERG_TYPE) == 0)
						Compnt->Nin = 1;

						else if (strcmp(s, CONVRG_TYPE) == 0)
						{
						Compnt->Nout = 1;
						Compnt->airpathcpy = 'y';
						Eqsys->Ncnvrg++;
						}****/

						if (strcmp(s, CONVRG_TYPE) == 0 || 
							strcmp(s, CVRGAIR_TYPE) == 0)
						{
							Eqsys->Ncnvrg++;
							Compnt->Nout = 1 ;
						}

						else if (strcmp(s, DIVERG_TYPE ) == 0 ||
							strcmp ( s, DIVGAIR_TYPE ) == 0 )
							Compnt->Nin = 1 ;

						else if (strcmp(s, FLIN_TYPE) == 0)
							Eqsys->Nflin++;

						else if (strcmp(s, HCLOAD_TYPE) == 0 ||
							strcmp(s, HCLOADW_TYPE) == 0 || strcmp(s, RMAC_TYPE) == 0 || strcmp(s, RMACD_TYPE) == 0)
							Eqsys->Nhcload++;

						/*************************/
						else if (strcmp(s, VALV_TYPE) == 0
							|| strcmp(s, TVALV_TYPE) == 0 )
							Eqsys->Nvalv++ ;
						/**********************************/

						else if (strcmp(s, QMEAS_TYPE) == 0 )
							Eqsys->Nqmeas++ ;
						else if (strcmp(s, DIVERG_TYPE) != 0 && 
							strcmp(s, DIVGAIR_TYPE) != 0)	      	      	      
							Eprint(errkey, s);

						break;

					case 'i':
						idi[Ni] = *s;
						Ni++ ;
						break;

					case 'o':
						ido[No] = *s;
						No++ ;
						break;

					case 'I':
						/*---------- Satoh DEBUG 1998/5/15 ---------*/
						if ( Crm != NULL )
						{
							if (strcmp(Crm->eqptype, ROOM_TYPE) == 0)
#if SIMUL_BUILDG /****************/
							{
								room = (ROOM *)Crm->eqp;
								room->Nasup = atoi(s);

								N = room->Nasup ;
								if (N > 0 )
								{
									if ((room->arsp = (AIRSUP *)malloc(N * sizeof(AIRSUP))) == NULL)
										Ercalloc(room->Nasup, errkey); 
								}

								Crm->Nin += 2 * room->Nasup;
							}
						}
#else
								;
#endif

							else
								Compnt->Nin = atoi(s);

							for (i = 0; i <Compnt->Nin; i++)
								idi[i] = ' ';
							idi[i] = '\0';
							Compnt->idi = stralloc(idi);
							break;

					case 'O':
						Compnt->Nout = atoi(s);
						for (i = 0; i <Compnt->Nout; i++)
							ido[i] = ' ';
						ido[i] = '\0';
						Compnt->ido = stralloc(ido);
						break;

					case 'L':
						*(Compnt->ivparm) = atof(s);
						break;

					case 'e':
						Compnt->envname = stralloc(s);
						break;

					case 'r':
						Compnt->roomname = stralloc(s) ;
						break ;

					case 'h':
						Compnt->hccname = stralloc(s) ;
						break ;

					case 'f':
						Compnt->rdpnlname = stralloc(s) ;
						break ;

					case 'R':
						Compnt->roomname = stralloc(s) ;
						fscanf ( f, "%lf", &Compnt->eqpeff ) ;
						break ;

					case 's':
						Compnt->exsname = stralloc(s);
						break;

					case 'M':
						Compnt->omparm = stralloc(s) ;
						break ;
					case 'S': case 'V':
						strcat(s, "  ");
						st = s + strlen(s);

						fscanf(f, "%[^*]*", st); 

						strcat(s, " *"); 

						Compnt->tparm = stralloc(s);

						break;

					case 'T':
						if (*s == '(')
						{	  
							strcat(s, " ");
							st = s + strlen(s);
							fscanf(f, "%[^)])", st);
							Compnt->tparm = stralloc(s);
						}
						else
							Compnt->tparm = stralloc(s);
						/*****	    
						Compnt->ivparm = dcalloc(1, errkey);
						*(Compnt->ivparm) = atof(s + 1);
						*************/	  
						break; 

						/*---- Roh Debug for a constant outlet humidity model of wet coil  2003/4/25 ----*/
					case 'w':
						*(Compnt->ivparm) = atof(s);
						break;

					// OM変風量でモニターするファンの名称
					case 'm':
						Compnt->monPlistName = stralloc(s) ;
						Compnt->valvcmp->monPlistName = stralloc(s) ;
						break ;

					// 電気蓄熱暖房器内臓のPCM重量
					case 'P':
						Compnt->mPCM = atof(s);
						break;
						}
					}
				}

				if (Crm == NULL)
				{
					Compnt++ ;
					D++;
				}
			}
		}

		ncmp = (int)(Compnt - cmp);
		for (cm = cmp; cm < cmp + ncmp; cm++)
		{
			if (strcmp(cm->eqptype, DIVGAIR_TYPE) == 0)
			{
				strcpy ( s, cm->name ) ;
				strcat ( s, ".x" ) ;
				Compnt->name = stralloc ( s ) ;

				/**********************
				Compnt->name = stralloc(cm->name);
				strcat(Compnt->name, ".x");
				/****************************/

				Compnt->eqptype = stralloc ( cm->eqptype ) ;
				Compnt->Nout = cm->Nout;

				Compnt->ido = stralloc(cm->ido);

				/****for (i = 0; i <Compnt->Nout; i++)
				Compnt->ido[i] = cm->ido[i];****/
				Compnt++;
			}
			else if (strcmp(cm->eqptype, CVRGAIR_TYPE) == 0)
			{
				strcpy ( s, cm->name ) ;
				strcat ( s, ".x" ) ;
				Compnt->name = stralloc ( s ) ;

				/*********************
				Compnt->name = stralloc(cm->name);
				strcat(Compnt->name, ".x");
				/******************************/

				Compnt->eqptype = stralloc ( cm->eqptype ) ;
				Compnt->Nin = cm->Nin;

				Compnt->idi = stralloc(cm->idi);

				/****for (i = 0; i < Compnt->Nin; i++)
				Compnt->idi[i] = cm->idi[i];****/

				Eqsys->Ncnvrg++;
				Compnt++;
			}
		}
		*Ncompnt = (int)(Compnt - cmp);


		//printf("<<Compodata>>  Ncompnt = %d\n", *Ncompnt);

		N = Eqsys->Nvalv ;
		if ( N > 0 )
		{
			if (( Eqsys->Valv = (VALV *) malloc(N * sizeof(VALV))) == NULL)
				Ercalloc(Eqsys->Nvalv, errkey) ;
		}

		if ( Eqsys->Valv != NULL )
		{
			Valv = Eqsys->Valv ;
			for ( i = 0; i < N; i++, Valv++ )
			{
				Valv->name = NULL ;
				Valv->cmp = NULL ;
				Valv->cmb = NULL ;
				Valv->org = 'n' ;
				Valv->x = -999.0 ;
				Valv->xinit = NULL ;
				Valv->Tin = NULL ;
				Valv->Tset = NULL ;
				Valv->mon = NULL ;
				Valv->Plist = NULL ;
				Valv->MGo = NULL ;
				Valv->Tout = NULL ;
				Valv->monPlist = NULL ;
				//Valv->OMfan = NULL ;
				//Valv->OMfanName = NULL ;
			}
		}

		N = Eqsys->Nqmeas ;
		if ( N > 0 )
		{
			if (( Eqsys->Qmeas = (QMEAS *) malloc(N * sizeof(QMEAS))) == NULL)
				Ercalloc(N, errkey) ;
		}

		if ( Eqsys->Qmeas != NULL )
		{
			Qmeas = Eqsys->Qmeas ;
			for ( i = 0; i < N; i++, Qmeas++ )
			{
				Qmeas->name = NULL ;
				Qmeas->cmp = NULL ;
				Qmeas->Th = Qmeas->Tc = Qmeas->G = NULL ;
				Qmeas->PlistG = Qmeas->PlistTc = Qmeas->PlistTh = Qmeas->Plistxc = Qmeas->Plistxh = NULL ;
				Qmeas->xc = Qmeas->xh = NULL ;
				Qmeas->id = 0 ;
				Qmeas->Nelmc = Qmeas->Nelmh = -999 ;
				//Qmeas->Pelmc = Qmeas->Pelmh = NULL ;
			}
		}

		N = Eqsys->Nhcc ;
		Eqsys->Hcc = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Hcc = (HCC *)malloc(N * sizeof(HCC))) == NULL)
				Ercalloc(Eqsys->Nhcc, errkey);
		}

		if ( Eqsys->Hcc != NULL )
		{
			Hcc = Eqsys->Hcc ;

			for ( i = 0; i < N; i++, Hcc++ )
			{
				Hcc->name = NULL ;
				Hcc->cmp = NULL ;
				Hcc->cat = NULL ;
				// 入口水温、入口空気絶対湿度を初期化
				Hcc->Twin = 5. ;
				Hcc->xain = FNXtr(25., 50. ) ;
			}
		}

		N = Eqsys->Nboi ;
		Eqsys->Boi = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Boi = (BOI *)malloc(N * sizeof(BOI))) == NULL)
				Ercalloc(Eqsys->Nboi, errkey);
		}

		if ( Eqsys->Boi != NULL )
		{
			Boi = Eqsys->Boi ;

			for ( i = 0; i < N; i++, Boi++ )
			{
				Boi->name = NULL ;
				Boi->cmp = NULL ;
				Boi->cat = NULL ;
				Boi->load = NULL ;
				Boi->mode = 'M' ;
				mtEdayinit(Boi->mtEdy) ;
				mtEdayinit(Boi->mtPhdy) ;
			}
		}

		N = Eqsys->Ncoll ;
		Eqsys->Coll = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Coll = (COLL *)malloc(N * sizeof(COLL))) == NULL)
				Ercalloc(Eqsys->Ncoll, errkey);
		}

		if ( Eqsys->Coll != NULL )
		{
			Coll = Eqsys->Coll ;

			for ( i = 0; i < N; i++, Coll++ )
			{
				Coll->name = NULL ;
				Coll->cmp = NULL ;
				Coll->cat = NULL ;
				Coll->sol = NULL ;
				Coll->Te = Coll->Tcb = 0.0 ;
				//Coll->Fd = 0.9 ;
			}
		}

		N = Eqsys->Npv ;
		Eqsys->PVcmp = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->PVcmp = (struct pv *)malloc(N * sizeof(struct pv))) == NULL)
				Ercalloc(Eqsys->Npv, errkey);
		}

		if ( Eqsys->PVcmp != NULL )
		{
			PV = Eqsys->PVcmp ;

			for ( i = 0; i < N; i++, PV++ )
			{
				PV->PVcap = PV->Area = -999. ;
				PV->name = NULL ;
				PV->cmp = NULL ;
				PV->cat = NULL ;
				PV->sol = NULL ;
				PV->Ta = PV->V = PV->I = NULL ;
				mtEdayinit(PV->mtEdy) ;
			}
		}

		// Satoh OMVAV  2010/12/16
		N = Eqsys->Nomvav ;
		Eqsys->OMvav = NULL ;
		if ( N > 0 )
		{
			if ((Eqsys->OMvav = (struct omvav *)malloc(N * sizeof(struct omvav))) == NULL)
				Ercalloc(Eqsys->Nomvav, errkey);
		}

		if ( Eqsys->OMvav != NULL )
		{
			OMvav = Eqsys->OMvav ;

			for ( i = 0; i < N; i++, OMvav++ )
			{
				OMvav->name = NULL ;
				OMvav->cmp = NULL ;
				OMvav->cat = NULL ;
				OMvav->omwall = NULL ;
				OMvav->Plist = NULL ;
				OMvav->Nrdpnl = 0 ;

				for ( j = 0; j < 4; j++)
					OMvav->rdpnl[j] = NULL ;
			}
		}

		N = Eqsys->Nrefa ;
		Eqsys->Refa = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Refa = (REFA *)malloc(N * sizeof(REFA))) == NULL)
				Ercalloc(Eqsys->Nrefa, errkey);
		}

		if ( Eqsys->Refa != NULL )
		{
			Rf = Eqsys->Refa ;

			for ( i = 0; i < N; i++, Rf++ )
			{
				Rf->name = NULL ;
				Rf->cmp = NULL ;
				Rf->cat = NULL ;
				Rf->load = NULL ;
				Rf->room = NULL ;
				Rf->Do = Rf->D1 = Rf->Tin = Rf->Te = Rf->Tc = 0.0 ;
				mtEdayinit(Rf->mtEdy) ;
				mtEdayinit(Rf->mtPhdy) ;
			}
		}

		N = Eqsys->Npipe ;
		Eqsys->Pipe = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Pipe = (PIPE *)malloc(N * sizeof(PIPE))) == NULL)
				Ercalloc(Eqsys->Npipe, errkey); 
		}

		if ( Eqsys->Pipe != NULL )
		{
			Pi = Eqsys->Pipe ;

			for ( i = 0; i < N; i++, Pi++ )
			{
				Pi->name = NULL ;
				Pi->cmp = NULL ;
				Pi->cat = NULL ;
				Pi->loadt = NULL ;
				Pi->loadx = NULL ;
				Pi->room = NULL ;
			}
		}

		N = Eqsys->Nstank ;
		Eqsys->Stank = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Stank = (STANK *)malloc(N * sizeof(STANK))) == NULL)
				Ercalloc(Eqsys->Nstank, errkey); 
		}

		if ( Eqsys->Stank != NULL )
		{
			St = Eqsys->Stank ;

			for ( i = 0; i < N; i++, St++ )
			{
				St->name = NULL ;
				St->cmp = NULL ;
				St->cat = NULL ;
				St->Jin = St->Jout = NULL ;
				St->pthcon = St->ihex = NULL ;
				St->batchcon = NULL ;
				St->B = St->R = St->Fg = St->d = St->Tss = NULL ;
				St->dtankF = NULL ;
				St->Tssold = St->dvol = St->Mdt = St->KS = St->ihxeff = St->KA = NULL ;
				St->KAinput = NULL ;
				St->cGwin = St->EGwin = St->Twin = St->Q = St->Tenv = NULL ;
				St->stkdy = NULL ;
				St->mstkdy = NULL ;
				St->mQlossdy = 0.0 ;
				St->mQstody = 0.0 ;
				St->Ncalcihex = 0 ;
			}
		}

		N = Eqsys->Nhex ;
		Eqsys->Hex = NULL ;
		if (N > 0 && (Eqsys->Hex = (HEX *)malloc(N * sizeof(HEX))) == NULL)
			Ercalloc(Eqsys->Nhex, errkey); 

		if ( Eqsys->Hex != NULL )
		{
			Hx = Eqsys->Hex ;

			for ( i = 0; i < N; i++, Hx++ )
			{
				Hx->name = NULL ;
				Hx->cmp = NULL ;
				Hx->cat = NULL ;
				Hx->id = 0 ;
			}
		}

		N = Eqsys->Npump ;
		Eqsys->Pump = NULL ;
		if (N > 0 && (Eqsys->Pump = (PUMP *)malloc(N * sizeof(PUMP))) == NULL)
			Ercalloc(Eqsys->Npump, errkey); 

		if ( Eqsys->Pump != NULL )
		{
			Pp = Eqsys->Pump ;

			for ( i = 0; i < N; i++, Pp++ )
			{
				Pp->name = NULL ;
				Pp->cmp = NULL ;
				Pp->cat = NULL ;
				Pp->sol = NULL ;
				mtEdayinit(Pp->mtEdy) ;
			}
		}

		N = Eqsys->Ncnvrg ;
		Eqsys->Cnvrg = NULL ;
		if (N > 0 )
		{
			if ((Eqsys->Cnvrg = (COMPNT **)malloc(N * sizeof(COMPNT *))) == NULL)
				Ercalloc(Eqsys->Ncnvrg, errkey);

			C = *Eqsys->Cnvrg ;
			for ( i = 0; i < N; i++, C++ )
				C = NULL ;
		}

		N = Eqsys->Nflin ;
		Eqsys->Flin = NULL ;
		if (N > 0 && (Eqsys->Flin = (FLIN *)malloc(N * sizeof(FLIN))) == NULL)
			Ercalloc(Eqsys->Nflin, errkey);

		if ( Eqsys->Flin != NULL )
		{
			Fl = Eqsys->Flin ;

			for ( i = 0; i < N; i++,Fl++ )
			{
				Fl->name = Fl->namet = Fl->namex = NULL ;
				Fl->vart = Fl->varx = NULL ;
				Fl->cmp = NULL ;
			}
		}

		N = Eqsys->Nhcload ;
		Eqsys->Hcload = NULL ;
		if (N > 0 && (Eqsys->Hcload = (HCLOAD *)malloc(N * sizeof(HCLOAD))) == NULL)
			Ercalloc(Eqsys->Nhcload, errkey); 

		if ( Eqsys->Hcload != NULL )
		{
			Hl = Eqsys->Hcload ;

			for ( i = 0; i < N; i++, Hl++ )
			{
				Hl->name = Hl->loadt = Hl->loadx = NULL ;
				Hl->cmp = NULL ;
				Hl->RMACFlg = 'N' ;
				mtEdayinit(Hl->mtEdy) ;
				Hl->Ga = Hl->Gw = 0. ;
				Hl->RHout = 50.0;
			}
		}

		/*---- Satoh Debug VAV  2000/10/30 ----*/
		N = Eqsys->Nvav ;
		Eqsys->vav = NULL ;
		if (N > 0 && (Eqsys->vav = (VAV *)malloc(N * sizeof(VAV))) == NULL)
			Ercalloc(Eqsys->Nvav, errkey); 

		if ( Eqsys->vav != NULL )
		{
			V = Eqsys->vav ;

			for ( i = 0; i < N; i++, V++ )
			{
				V->name = NULL ;
				V->cat = NULL ;
				V->hcc = NULL ;
				V->hcld = NULL ;
				V->cmp = NULL ;
			}
		}

		N = Eqsys->Nstheat ;
		Eqsys->stheat = NULL ;
		if (N > 0 && (Eqsys->stheat = (STHEAT *)malloc(N * sizeof(STHEAT))) == NULL)
			Ercalloc(Eqsys->Nstheat, errkey);

		if ( Eqsys->stheat != NULL )
		{
			Sh = Eqsys->stheat ;

			for ( i = 0; i < N; i++, Sh++ )
			{
				Sh->name = NULL ;
				Sh->cat = NULL ;
				Sh->cmp = NULL ;
				Sh->room = NULL ;
				Sh->pcm = NULL;
				mtEdayinit ( Sh->mtEdy ) ;
			}
		}

		// Satoh追加　デシカント槽 2013/10/23
		N = Eqsys->Ndesi ;
		Eqsys->Desi = NULL ;
		if (N > 0 && (Eqsys->Desi = (DESI *)malloc(N * sizeof(DESI))) == NULL)
			Ercalloc(Eqsys->Ndesi, errkey);

		if ( Eqsys->Desi != NULL )
		{
			Desi = Eqsys->Desi ;

			for ( i = 0; i < N; i++, Desi++ )
			{
				Desi->name = NULL ;
				Desi->cat = NULL ;
				Desi->cmp = NULL ;
				Desi->room = NULL ;
				Desi->Tenv = NULL ;
			}
		}

		// Satoh追加　気化冷却器 2013/10/26
		N = Eqsys->Nevac ;
		Eqsys->Evac = NULL ;
		if (N > 0 && (Eqsys->Evac = (EVAC *)malloc(N * sizeof(EVAC))) == NULL)
			Ercalloc(Eqsys->Nevac, errkey);

		if ( Eqsys->Evac != NULL )
		{
			Evac = Eqsys->Evac ;

			for ( i = 0; i < N; i++, Evac++ )
			{
				Evac->name = NULL ;
				Evac->cat = NULL ;
				Evac->cmp = NULL ;
				Evac->M = NULL ;
				Evac->kx = NULL ;
				Evac->Tdry = NULL ;
				Evac->Twet = NULL ;
				Evac->xdry = NULL ;
				Evac->xwet = NULL ;
				Evac->Ts = NULL ;
				Evac->xs = NULL ;
				Evac->RHdry = NULL ;
				Evac->RHwet = NULL ;
				Evac->UXC = NULL ;
				Evac->UX = NULL ;
				//Evac->UXdry = NULL ;
				//Evac->UXwet = NULL ;
			}
		}

		N = Eqsys->Nthex ;
		Eqsys->Thex = NULL ;
		if ( N > 0 && (Eqsys->Thex = ( THEX * ) malloc ( N * sizeof ( THEX ))) == NULL )
			Ercalloc ( Eqsys->Nthex, errkey ) ;

		if ( Eqsys->Thex != NULL )
		{
			Thex = Eqsys->Thex ;

			for ( i = 0; i < N; i++, Thex++ )
			{
				Thex->name = NULL ;
				Thex->cmp = NULL ;
				Thex->cat = NULL ;
				Thex->type = ' ' ;
				Thex->cGe = Thex->Ge = Thex->Go = 0.0 ;
				Thex->et = Thex->eh = -999.0 ;
			}
		}

		//printf("<<Compodata>> end\n");
	}

	/***********************************************************************/

	int		Compntcount ( FILE *fi )
	{
		int		N ;
		long	ad ;
		char	s[SCHAR] ;

		N = 0 ;
		ad = ftell ( fi ) ;

		while ( fscanf ( fi, "%s", s ) != EOF )
		{
			//printf ( "%s\n", s ) ;
			if ( strcmp ( s, ";" ) == 0 )
			{
				N++ ;

				fscanf ( fi, "%s", s ) ;

				if ( *s == '*' )
					break ;
				else if ( *s == '(' )
					N++ ;
			}
			else if ( strcmp ( s, DIVGAIR_TYPE  ) == 0
				|| strcmp ( s, CVRGAIR_TYPE ) == 0 )
				N++ ;
			else if ( *s == '(' )
				N++ ;
		}

		fseek ( fi, ad, SEEK_SET ) ;

		return ( N ) ;
	}
