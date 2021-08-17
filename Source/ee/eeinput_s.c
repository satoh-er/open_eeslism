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


#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "MODEL.h"     /*--higuchi 070918 -*/

#include "fesy.h"
#include "fnbld.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "winerror.h"
#include "FUNC.h"      /*--higuchi  -*/

/* -------------------------------------------------- */

/*  建築・設備システムデータ入力  */

void Eeinput(char *Ipath, SIMCONTL *Simc, SCHDL *Schdl,
	EXSFS *Exsf, RMVLS *Rmvls, EQCAT *Eqcat, EQSYS *Eqsys,
	COMPNT **Compnt, int *Ncompnt, int *Ncmpalloc, ELOUT **Elout, int *Nelout, ELIN **Elin, int *Nelin,
	MPATH **Mpath, int *Nmpath, PLIST **Plist, PELM **Pelm, int *Npelm,
	CONTL **Contl, int *Ncontl, CTLIF **Ctlif, int *Nctlif, CTLST **Ctlst, int *Nctlst,
	FLOUT *Flout, int *Nflout, WDAT *Wd, DAYTM *Daytm, int key, int *Nplist,
	int *bdpn, int *obsn, int *treen, int *shadn, int *polyn, BBDP **bp, OBS **obs,
	TREE **tree, SHADTB **shadtb, POLYGN **poly, int *monten, int *gpn, double *DE, NOPLPMP *Noplpmp)
{
	FILE	*fi, *fbmlist;
	FLOUT	*flo;
	double	Twallinit;
	int		i, j, dtm = 3600, nday,
		Nday, daystartx = 0, daystart = 0, dayend = 0;
	char	s[SCHAR], Err[SCHAR], *File, hptest[SCHAR];
	int		wdpri = 0, revpri = 0, pmvpri = 0, Nrmspri = 0, Nqrmpri = 0, Nwalpri = 0, Npcmpri = 0;
	SCH		*Sch, *Scw;
	extern char	*Fbmlist;
	int		SYSCMP_ID = 0, SYSPTH_ID = 0;

#if SIMUL_BUILDG
	DFWL	dfwl;
	RMSRF	*Sdd, *Sd;
	ROOM	*Rm;
#endif
	/*-------higuchi 070918---------start*/
	//RRMP *rp;
	//MADO *wp;
	//sunblk *sb;
	int  smonth, sday, emonth, eday;

	//sb = bp->SBLK;
	//rp = bp->RMP;
	//wp = rp->WD;
	/*-------higuchi------------end*/


	File = NULL;

	s[0] = '\0';
	sprintf_s(Err, sizeof(Err), ERRFMT, "(Eeinput)");

	//*Nexs=0 ;

#if SIMUL_BUILDG  /******************************/

	Rmvls->Nwall = 0;
	Rmvls->Nwindow = 0;
	Rmvls->Nroom = 0;

	Sdd = Rmvls->Sd;
	Rmvls->Nsrf = 0;
	Rmvls->Nrdpnl = Rmvls->Nmwall = 0;
#endif

	if (fopen_s(&fi, "dayweek.efl", "r") != 0)
	{
		Eprint("<Eeinput>", "dayweek.efl");

		preexit();
		exit(EXIT_DAYWEK);
	}

	Dayweek(fi, Ipath, Simc->daywk, key);
	fclose(fi);

#if SIMUL_BUILDG  /******************************/
	if (DEBUG)
		dprdayweek(Simc->daywk);
#endif

	strcpy_s(s, sizeof(s), Ipath);
	strcat_s(s, sizeof(s), "schtba.ewk");
	if (fopen_s(&fi, s, "r") != 0)
	{
		Eprint("<Eeinput>", "schtba.ewk");

		preexit();
		exit(EXIT_SCHTB);
	}

	else
	{
		Schtable(fi, s, Schdl);
		fclose(fi);

		Schname(Ipath, "Schname", Schdl);
		Sch = Schdl->Sch;
		Scw = Schdl->Scw;

		Schdl->Nsch = Sch->end;
		Schdl->Nscw = Scw->end;
	}

	strcpy_s(s, sizeof(s), Ipath);
	strcat_s(s, sizeof(s), "schnma.ewk");
	if (fopen_s(&fi, s, "r") != 0)
	{
		Eprint("<Eeinput>", "schnma.ewk");

		preexit();
		exit(EXIT_SCHNM);
	}

	else
	{
		Schdata(fi, "schnm", Simc->daywk, Schdl);
		fclose(fi);

		Sch = Schdl->Sch;
		Scw = Schdl->Scw;
		Schdl->Nsch = Sch->end;
		Schdl->Nscw = Scw->end;

		if (Schdl->Nsch > 0)
			Schdl->val = dcalloc(Schdl->Nsch, "-- Schdl->val --");
		else
			Schdl->val = NULL;

		if (Schdl->Nscw > 0)
			Schdl->isw = scalloc(Schdl->Nscw, "-- Schdl->isw --");
		else
			Schdl->isw = NULL;
	}

	strcpy_s(s, sizeof(s), Ipath);
	strcat_s(s, sizeof(s), "bdata.ewk");
	if (fopen_s(&fi, s, "r") != 0)
	{
		Eprint("<Eeinput>", "bdata.ewk");

		preexit();
		exit(EXIT_BDATA);
	}


	while (fscanf_s(fi, "%s", s, sizeof(s)), printf("=== %s\n", s), s[0] != '*')
	{
		if (strcmp("TITLE", s) == 0)
		{
			fscanf_s(fi, "%[^;];", Simc->title, sizeof(Simc->title));
			printf("%s\n", Simc->title);
		}
		else if (strcmp("GDAT", s) == 0)
		{
			sprintf_s(hptest, sizeof(hptest), "GDAT");
			HeapCheck(hptest);
			Wd->RNtype = 'C';
			Wd->Intgtsupw = 'N';
			Simc->perio = 'n';			// 周期定常計算フラグを'n'に初期化
			Gdata(fi, s, Simc->File, &Simc->wfname, &Simc->ofname, &dtm, &Simc->sttmm,
				&daystartx, &daystart, &dayend, &Twallinit, Simc->dayprn,
				&wdpri, &revpri, &pmvpri, &Simc->helmkey, &Simc->MaxIterate, Daytm, Wd, &Simc->perio);
			//printf("daystart=%d, dayend=%d daystartx=%d\n",daystart,dayend,daystartx);
			if (Simc->wfname == NULL)
				Simc->wdtype = 'E';
			else
				Simc->wdtype = 'H';
			//printf("DayS=%d,DayE=%d\n",daystart,dayend) ;
#if SIMUL_BUILDG  /******************************/
			Rmvls->Twallinit = Twallinit;
#endif 

			Simc->dTm = dtm;

			Simc->unit = stralloc("t_C x_kg/kg r_% q_W e_W");
			Simc->unitdy = stralloc("Q_kWh E_kWh");

			printf("== File  Output=%s\n", Simc->ofname);
			HeapCheck(hptest);
		}

		else if (strcmp("SCHTB", s) == 0)
		{
			Schtable(fi, s, Schdl);
			Schname(Ipath, "Schname", Schdl);

			Sch = Schdl->Sch;
			Scw = Schdl->Scw;
			Schdl->Nsch = Sch->end;
			Schdl->Nscw = Scw->end;
		}
		else if (strcmp("SCHNM", s) == 0)
		{
			Schdata(fi, s, Simc->daywk, Schdl);

			Sch = Schdl->Sch;
			Scw = Schdl->Scw;
			Schdl->Nsch = Sch->end;
			Schdl->Nscw = Scw->end;
		}

		else if (strcmp("EXSRF", s) == 0)
		{
			sprintf_s(hptest, sizeof(hptest), "EXSRF");
			HeapCheck(hptest);
			Exsfdata(fi, s, Exsf, Schdl, Simc);
		}

#if SIMUL_BUILDG  
		else if (strcmp("SUNBRK", s) == 0)
			Snbkdata(fi, s, &Rmvls->Snbk);
		else if (strcmp("PCM", s) == 0)
			PCMdata(fi, s, &Rmvls->PCM, &Rmvls->Npcm, &Rmvls->pcmiterate);
		else if (strcmp("WALL", s) == 0)
		{
			sprintf_s(hptest, sizeof(hptest), "Wallata");
			HeapCheck(hptest);

			if (Fbmlist == NULL)
				File = stralloc("wbmlist.efl");
			else
				File = Fbmlist;

			if (fopen_s(&fbmlist, File, "r") != 0)
			{
				Eprint("<Eeinput>", "wbmlist.efl");

				preexit();
				exit(EXIT_WBMLST);
			}
			/*******************/

			Walldata(fi, fbmlist, s, &Rmvls->Wall, &Rmvls->Nwall, &dfwl, Rmvls->PCM, 
				Rmvls->Npcm);

			//////////////////////////////////////

			HeapCheck(hptest);
		}
		else if (strcmp("WINDOW", s) == 0)
			Windowdata(fi, s, &Rmvls->Window, &Rmvls->Nwindow);
		else if (strcmp("ROOM", s) == 0)
		{
			Roomdata(fi, "Roomdata", Exsf->Exs, &dfwl, Rmvls, Schdl, Simc);
			Balloc(Rmvls->Nsrf, Rmvls->Sd, Rmvls->Wall, &Rmvls->Mw, &Rmvls->Nmwall);
			//////////////////////////////////////
			sprintf_s(hptest, sizeof(hptest), "Roomdata");
			HeapCheck(hptest);
			//if(Rmvls->Sd->alicsch != NULL)
			//	printf("Balloc end NULL check\n") ;
		}
		else if (strcmp("RAICH", s) == 0 || strcmp("VENT", s) == 0)
			Ventdata(fi, s, Schdl, Rmvls->Room, Simc);
		else if (strcmp("RESI", s) == 0)
			Residata(fi, s, Schdl, Rmvls->Room, &pmvpri, Simc);
		else if (strcmp("APPL", s) == 0)
			Appldata(fi, s, Schdl, Rmvls->Room, Simc);
#endif
		else if (strcmp("VCFILE", s) == 0)
			Vcfdata(fi, Simc);
		else if (strcmp("EQPCAT", s) == 0)
		{
			//printf("%s\n", Rmvls->Room->trnx->nextroom->name) ;
			Eqcadata(fi, "Eqcadata", Eqcat);
			//printf("%s\n", Rmvls->Room->trnx->nextroom->name) ;
		}
		else if (strcmp("SYSCMP", s) == 0)
		{
			/*****Flwindata(Flwin, Nflwin,  Wd);********/
			Compodata(fi, "Compodata", Rmvls, Eqcat, Compnt, Ncompnt, Eqsys, Ncmpalloc, 0);
			/****
			cmpprint(1, *Ncompnt, Compnt, Elout, Elin);/****/
			//printf("%s\n", Rmvls->Room->trnx->nextroom->name) ;
			Elmalloc("Elmalloc ", *Ncompnt, *Compnt, Eqcat, Eqsys,
				Elout, Nelout, Elin, Nelin);
			/****
			cmpprint(1, *Ncompnt, Compnt, Elout, Elin);
			eloutprint(1, *Nelout, Elout, Compnt);
			elinprint(1, *Ncompnt, Compnt, Elout, Elin);
			/*****/
			SYSCMP_ID++;
		}
		else if (strcmp("SYSPTH", s) == 0)
		{
			if (SYSCMP_ID == 0)
			{
				Compodata(fi, "Compodata", Rmvls, Eqcat, Compnt, Ncompnt, Eqsys, Ncmpalloc, 1);

				Elmalloc("Elmalloc ", *Ncompnt, *Compnt, Eqcat, Eqsys,
					Elout, Nelout, Elin, Nelin);
				SYSCMP_ID++;
			}

			//printf("%s\n", Rmvls->Room->trnx->nextroom->name) ;

			Pathdata(fi, "Pathdata", Simc, Wd, *Ncompnt, *Compnt, Schdl,
				Mpath, Nmpath, Plist, Pelm, Npelm, Nplist, 0, Eqsys);

			/*******
			printf("xxxxx Eeinput   Pathdata end\n");
			elinprint(1, *Ncompnt, Compnt, Elout, Elin);
			*********/

#if SIMUL_BUILDG     /*********************/ 
			/*
			printf("xxxxx Eeinput   Roomelm begin\n");
			*/
			Roomelm(Rmvls->Nroom, Rmvls->Room, Rmvls->Nrdpnl, Rmvls->Rdpnl);

			/*
			printf("xxxxx Eeinput   Roomelm end\n");
			*/
			/*******
			printf("xxxxx Eeinput   Pathdata end\n");
			elinprint(1, *Ncompnt, Compnt, Elout, Elin);
			*********/
#endif

			// 変数の割り当て
			Hclelm(Eqsys->Nhcload, Eqsys->Hcload);
			Thexelm(Eqsys->Nthex, Eqsys->Thex);
			Desielm(Eqsys->Ndesi, Eqsys->Desi);
			Evacelm(Eqsys->Nevac, Eqsys->Evac);

			Qmeaselm(Eqsys->Nqmeas, Eqsys->Qmeas);
			/********
			cmpprint(0, *Ncompnt, Compnt, Elout, Elin);

			eloutprint(0, *Nelout, Elout, Compnt);
			elinprint(0, *Ncompnt, Compnt, Elout, Elin);
			********/

			/****plistprint(Nmpath, Mpath, Pelm, Elout, Elin);*****/

			SYSPTH_ID++;
		}
		else if (strcmp("CONTL", s) == 0)
		{
			if (SYSCMP_ID == 0)
			{

				Compodata(fi, "Compodata", Rmvls, Eqcat, Compnt, Ncompnt, Eqsys, Ncmpalloc, 1);

				Elmalloc("Elmalloc ", *Ncompnt, *Compnt, Eqcat, Eqsys,
					Elout, Nelout, Elin, Nelin);
				SYSCMP_ID++;
			}

			if (SYSPTH_ID == 0)
			{
				Pathdata(fi, "Pathdata", Simc, Wd, *Ncompnt, *Compnt, Schdl,
					Mpath, Nmpath, Plist, Pelm, Npelm, Nplist, 1, Eqsys);

#if SIMUL_BUILDG
				Roomelm(Rmvls->Nroom, Rmvls->Room, Rmvls->Nrdpnl, Rmvls->Rdpnl);
#endif

				Hclelm(Eqsys->Nhcload, Eqsys->Hcload);
				Thexelm(Eqsys->Nthex, Eqsys->Thex);
				Desielm(Eqsys->Ndesi, Eqsys->Desi);
				Evacelm(Eqsys->Nevac, Eqsys->Evac);

				Qmeaselm(Eqsys->Nqmeas, Eqsys->Qmeas);

				SYSPTH_ID++;
			}

			Contrldata(fi, Contl, Ncontl, Ctlif, Nctlif, Ctlst, Nctlst,
				Simc, *Ncompnt, *Compnt, *Nmpath, *Mpath, Wd, Exsf, Schdl);

		}

		/*--------------higuchi add-------------------start*/

		// 20170503 higuchi add
		else if (strcmp("DIVID", s) == 0) {
			dividdata(fi, monten, DE);
		}

		/*--対象建物データ読み込み--*/
		else if (strcmp("COORDNT", s) == 0)

			bdpdata(fi, bdpn, bp, Exsf);

		/*--障害物データ読み込み--*/
		else if (strcmp("OBS", s) == 0)
			obsdata(fi, obsn, obs);

		/*--樹木データ読み込み--*/
		else if (strcmp("TREE", s) == 0)
			treedata(fi, treen, tree);

		/*--多角形障害物直接入力分の読み込み--*/
		else if (strcmp("POLYGON", s) == 0)
			polydata(fi, polyn, poly);

		/*--落葉スケジュール読み込み--*/
		else if (strcmp("SHDSCHTB", s) == 0){
			*shadn = 0;
			int	InputCount(FILE *fi, char *key);
			int Nshadn;
			SHADTB *shdp;;
			// 落葉スケジュールの数を数える
			Nshadn = InputCount(fi, ";");
			//printf("<Eeinput> Nshadn=%d\n", Nshadn);
			if (Nshadn > 0)
			{
				*shadtb = (SHADTB *)malloc(sizeof(SHADTB)*Nshadn);
				if (*shadtb == NULL)
					printf("<Eeinput> shadtbのメモリが確保できません\n");
				int i;
				shdp = *shadtb;
				for (i = 0; i < Nshadn; i++, shdp++)
				{
					shdp->lpname = NULL;
					shdp->indatn = 0;
					matinit(shdp->shad, 12);
					imatinit(shdp->ndays, 12);
					imatinit(shdp->ndaye, 12);
				}
			}
			shdp = *shadtb;
			while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != '*'){
				shdp->lpname = stralloc(s);
				(*shadn)++;
				shdp->indatn = 0;
				while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != ';'){
					sscanf_s(s, "%d/%d-%lf-%d/%d", &smonth, &sday, &shdp->shad[shdp->indatn], &emonth, &eday);
					shdp->ndays[shdp->indatn] = nennkann(smonth, sday);
					shdp->ndaye[shdp->indatn] = nennkann(emonth, eday);
					shdp->indatn = shdp->indatn + 1;
				}
				shdp++;
			}
		}
		/*----------higuchi add-----------------end-*/

		else
		{
			strcat_s(Err, sizeof(Err), "  ");
			strcat_s(Err, sizeof(Err), s);
			Eprint("<Eeinput>", Err);
		}


		//if(Rmvls->Sd != NULL && Rmvls->Sd->alicsch != NULL)
		//	printf("Balloc end NULL check\n") ;
	}


	/*--------------higuchi 070918-------------------start-*/
	if (*bdpn != 0){
		//printf("number of point in montekalro :") ;
		//scanf("%d",monten) ; 
		//printf("number of point in ground :") ;
		//scanf("%d",gpn) ; 
		//printf("deviding of wall mm:") ;
		//scanf("%lf",DE) ;
		//*DE = 100.;
		printf("deviding of wall mm: %f\n", *DE) ;
		printf("number of point in montekalro: %d\n", *monten) ;
	}
	/*----------------higuchi 7.11,061123------------------end*/

	fclose(fi);

	// 外部障害物の数を数える
	Noplpmp->Nop = OPcount(*bdpn, *bp, *polyn, *poly);
	Noplpmp->Nlp = LPcount(*bdpn, *bp, *obsn, *obs, *treen, *polyn, *poly);
	Noplpmp->Nmp = Noplpmp->Nop + Noplpmp->Nlp;

	//printf("Nop=%d Nlp=%d\n", Noplpmp->Nop, Noplpmp->Nlp);
	//////////////////////////////////////
	sprintf_s(hptest, sizeof(hptest), "Innput End");
	HeapCheck(hptest);

	if (SYSCMP_ID == 0)
	{
		Compodata(fi, "Compodata", Rmvls, Eqcat, Compnt, Ncompnt, Eqsys, Ncmpalloc, 1);

		Elmalloc("Elmalloc ", *Ncompnt, *Compnt, Eqcat, Eqsys,
			Elout, Nelout, Elin, Nelin);
	}

	if (SYSPTH_ID == 0)
	{
		Pathdata(fi, "Pathdata", Simc, Wd, *Ncompnt, *Compnt, Schdl,
			Mpath, Nmpath, Plist, Pelm, Npelm, Nplist, 1, Eqsys);

#if SIMUL_BUILDG
		Roomelm(Rmvls->Nroom, Rmvls->Room, Rmvls->Nrdpnl, Rmvls->Rdpnl);
#endif

		Hclelm(Eqsys->Nhcload, Eqsys->Hcload);
		Thexelm(Eqsys->Nthex, Eqsys->Thex);

		Qmeaselm(Eqsys->Nqmeas, Eqsys->Qmeas);
	}

	/*****vcfileint(Eqsys->Nvcdata, Eqsys->Vcdata, Simc);********/

#if SIMUL_BUILDG  /******************************/
	// 毎時計算ステップでの計算に切り替え
	//Rmrdshfc(Rmvls->Room, Rmvls->Sd);
#endif

	if (daystart > dayend)
		dayend = dayend + 365;
	Nday = dayend - daystart + 1;

	if (daystartx > daystart)
		daystart = daystart + 365;

	Nday += daystart - daystartx;
	Simc->dayend = daystartx + Nday - 1;
	Simc->daystartx = daystartx;
	Simc->daystart = daystart;

	strcpy_s(Simc->timeid, sizeof(Simc->timeid), "MDT");

	Simc->Ntimedyprt = Simc->dayend - Simc->daystart + 1;
	Simc->Dayntime = 24 * 3600 / dtm;
	Simc->Ntimehrprt = 0;
	//printf("daystart=%d, dayend=%d daystartx=%d\n",daystart,dayend,daystartx);
	for (nday = Simc->daystart; nday <= Simc->dayend; nday++)
	{
		//printf("nday=%d\n",nday) ;
		if (Simc->dayprn[nday])
			Simc->Ntimehrprt += Simc->Dayntime;
	}
#if SIMUL_BUILDG  /******************************/

	/*************
	printf("<<Eeinput>>  Nroom=%d\n", Rmvls->Nroom);
	/***************/

	Rm = Rmvls->Room;
	for (i = 0; i < Rmvls->Nroom; i++, Rm++)
	{
		if (Rm->sfpri == 'p')
			Nrmspri++;
		if (Rm->eqpri == 'p')
			Nqrmpri++;
	}

	/***************
	printf("<<Eeinput>> Sd  Rmvls->Nsrf=%d\n", Rmvls->Nsrf);
	/*************/

	int	Nshdpri;
	Nshdpri = 0;
	Nwalpri = 0;
	Sd = Rmvls->Sd;
	for (i = 0; i < Rmvls->Nsrf; i++, Sd++)
	{
		//printf("i=%d %s\n", i, Sd->dynamiccode) ;

		if (Sd->wlpri == 'p')
			Nwalpri++;

		if (Sd->pcmpri == 'y')
			Npcmpri++;

		// 日よけの影面積出力
		if (Sd->shdpri == 'p')
			Nshdpri++;
	}
#endif
	/****************
	printf("<<Eeinput>> Nwalpri=%d\n", Nwalpri);
	/**************/


	flo = Flout;

	Flout->idn = stralloc(PRTPATH);
	Flout++;

	Flout->idn = stralloc(PRTCOMP);
	Flout++;

	Flout->idn = stralloc(PRTDYCOMP);
	Flout++;

	Flout->idn = stralloc(PRTMNCOMP);
	Flout++;

	Flout->idn = stralloc(PRTMTCOMP);
	Flout++;

	Flout->idn = stralloc(PRTHRSTANK);
	Flout++;

	Flout->idn = stralloc(PRTWK);
	Flout++;

#if SIMUL_BUILDG  /******************************/

	Flout->idn = stralloc(PRTREV);
	Flout++;

	Flout->idn = stralloc(PRTHROOM);
	Flout++;

	Flout->idn = stralloc(PRTDYRM);
	Flout++;

	Flout->idn = stralloc(PRTMNRM);
	Flout++;

	/***********
	printf("<<Eeinput>>  Helminit\n");
	/***************/

	Helminit("Helminit", Simc->helmkey,
		Rmvls->Nroom, Rmvls->Room, &Rmvls->Qetotal);

	/*********
	printf("Eeinput Helminit end\n");
	/****************/

	if (Simc->helmkey == 'y')
	{
		Flout->idn = stralloc(PRTHELM);
		Flout++;

		Flout->idn = stralloc(PRTDYHELM);
		Flout++;

		Simc->Nhelmsfpri = 0;
		Rm = Rmvls->Room;
		for (i = 0; i < Rmvls->Nroom; i++, Rm++)
		{
			Sdd = Rm->rsrf;
			for (j = 0; j < Rm->N; j++, Sdd++)
			{
				if (Sdd->sfepri == 'y')
					Simc->Nhelmsfpri++;
			}
		}
		if (Simc->Nhelmsfpri > 0)
		{
			Flout->idn = stralloc(PRTHELMSF);
			Flout++;
		}
	}
	/********
	printf("<<Eeinput>> helmkey=%c Nhelmsfpri=%d\n", Simc->helmkey, Simc->Nhelmsfpri);
	/************/

	if (pmvpri > 0)
	{
		Flout->idn = stralloc(PRTPMV);
		Flout++;
	}

	if (Nqrmpri > 0)
	{
		Flout->idn = stralloc(PRTQRM);
		Flout++;

		Flout->idn = stralloc(PRTDQR);
		Flout++;
	}

	if (Nrmspri > 0)
	{
		Flout->idn = stralloc(PRTRSF);
		Flout++;
		Flout->idn = stralloc(PRTSFQ);
		Flout++;
		Flout->idn = stralloc(PRTSFA);
		Flout++;
		Flout->idn = stralloc(PRTDYSF);
		Flout++;
	}

	if (Nwalpri > 0)
	{
		Flout->idn = stralloc(PRTWAL);
		Flout++;
	}

	// 日よけの影面積出力
	if (Nshdpri > 0)
	{
		Flout->idn = stralloc(PRTSHD);
		Flout++;
	}

	if (Npcmpri > 0)
	{
		Flout->idn = stralloc(PRTPCM);
		Flout++;
	}

#endif /**************************************************/

	if (wdpri > 0)
	{
		Flout->idn = stralloc(PRTHWD);
		Flout++;

		Flout->idn = stralloc(PRTDWD);
		Flout++;

		Flout->idn = stralloc(PRTMWD);
		Flout++;
	}

	*Nflout = (int)(Flout - flo);

	if (Fbmlist == NULL)
		free(File);

	//////////////////////////////////////
	//printf("InputEnd\n");
	sprintf_s(hptest, sizeof(hptest), "Innput End");
	HeapCheck(hptest);

	//Sd = Rmvls->Sd ;
	//for ( i = 0; i < Rmvls->Nsrf; i++, Sd++ )
	//{
	//	printf("i=%d %s\n", i, Sd->dynamiccode) ;
	//}
}
