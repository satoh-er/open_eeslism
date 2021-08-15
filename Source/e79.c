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

/*   ees70.c   */

/*   室内熱環境制御システムシミュレ－ションプログラム

　　EESLISM   ver 3x  94/2, ver 3.1 95/7
  ver 3.5 96/3  ver 4.5 M. Udagawa      */

/*******************/

#define TEST 0
/*******************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <math.h>
//#include <iostream>
#include "esize.h"
#include "fesy.h"
#include "MODEL.h"     /*--higuchi  -*/
#include "FUNC.h"      /*--higuchi  -*/
#include "fnbld.h"
#include "fnesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "INPUT.h"     /*--higuchi  -*/
#include "fnlib.h"
#include "sun.h"
#include "psy.h"
#include "mfree.h"
#define  ALO  23.0

/*-------------------higuchi add----------------start*/
//#define MAXBDP 100       
//#define MAXOBS 100   
//#define MAXTREE 10    /*--樹木の最大本数--*/
//#define MAXPOLY 50     
/*------------------higuchi add----------------end*/

char	*UNIT = "SI";
double	PI = 3.141592654;
int	VAV_Count_MAX ;
char	*Fbmlist;
double	Sgm = 5.67e-8;
double	ca = 1005.0, cv = 1846.0, roa = 1.29, cw = 4186.0;
double	row = 1000.0, ro = 2501000.;
double	g = 9.8;
double	dTM, cff_kWh;
int		DEBUG = 0;
int		dayprn;

char	DAYweek[][4] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "Hol" };
FILE	*ferr;
void	bdhpri(char *ofile, RMVLS rmvls, EXSFS *exs);
int		NSTOP = 0, DISPLAY_DELAY = 0;
int		SETprint = 0;

/* -------------------------------------------------- */

int main(int Narg, char **File)
{
	char	s[SCHAR], hptest[SCHAR];

	DAYTM	Daytm;
	SIMCONTL	Simc;
	FLOUT	Flout[FLOUTMX];
	int		Nflout;
	LOCAT	Loc;
	WDAT	Wd, Wdd, Wdm;
	int	day, nday, tt, mm, mt, mta, mtb, dminute;
	SCHDL	Schdl;
	RMVLS	Rmvls;
	ROOM	*Rm;
	RMSRF	*Sd;
	EQCAT	Eqcat;
	EQSYS	Eqsys;
	int		LDreset, DWreset, TKreset, BOIreset, Evacreset;
	int		PCMfunreset;
	int		VAVreset;
	int		Valvreset;
	int		i, n, rm, j;
	char	Err[SCHAR];
	char	*mrk;
	PELM	*Pe;
	ELOUT	*Eo;

	/* ============================ */

	int		Ncompnt, Nelout, Nelin;
	int		Ncmpalloc;
	COMPNT	*Compnt;
	ELOUT	*Elout;
	ELIN	*Elin;
	SYSEQ	Syseq;
	int	Nmpath, Npelm, Nplist;
	MPATH	*Mpath;
	PLIST	*Plist;
	PELM	*Pelm;
	int	Ncontl = 0, Nctlif = 0, Nctlst = 0;
	CONTL	*Contl;
	CTLIF	*Ctlif;
	CTLST	*Ctlst;
	int      key = 0;
	char	*Ifile;
	char	*Path, *st;
	EXSFS	Exsf;
	double	*Soldy, *Solmon;

	/*---------------higuchi add-------------------start*/

	char RET[SCHAR], RET1[SCHAR], RET3[SCHAR], RET14[SCHAR],
		RET15[SCHAR];
#if TEST
	char RET4[150], RET6[150], RET7[150], RET8[150], RET9[150],
		RET10[150], RET11[150],RET12[150],RET13[150], RET5[150] ;
#endif
	int    bdpn = 0,      /*--BDPの総数--*/
		obsn = 0,      /*--OBSの総数--*/
		lpn = 0,       /*--LP(被受照面)の総数--*/
		opn = 0,       /*--OP(受照面)の総数--*/
		mpn = 0,       /*--MP(受照面)の総数--*/
		monten = 1000,   /*--モンテカルロ法の際の射出数--*/
		polyn = 0,       /*--直接座標入力するポリゴンの総数--*/
		treen = 0,     /*--TREE(樹木)の総数--*/
		shadn = 0;    /*--日射遮蔽率テーブルの総数--*/

	double DE = 100.0,      /*--壁面の分割による微小四角形の辺の長さ--*/
		ls = 0.0, ms = 0.0, ns = 0.0,  /*--太陽の方向余弦--*/
		co = 0.0;     /*--壁面への太陽光線の入射角--*/

	double *wap, **wip;  /*--壁と窓の微小面の総数--*/
	wap = NULL;
	wip = NULL;

	bekt   *uop, *ulp, *ullp, *ulmp;
	uop = ulp = ullp = ulmp = NULL;

	XYZ    **gp;             /*-地面の座標-*/
	gp = NULL;
	int    gpn = 50;          /*--地面の代表点の数--*/

	int    k, l;

	FILE      *fp1, *fp2, *fp3, *fp4;
	fp1 = fp2 = fp3 = fp4 = NULL;

	//BBDP   BDP[MAXBDP] ;                
	//OBS    obs[MAXOBS] ; 
	//TREE   tree[MAXTREE] ;    /*-樹木データ-*/  
	//POLYGN poly[MAXPOLY] ;      /*--POLYGON--*/  
	//SHADTB shadtb[pmax] ;     /*-LP面の日射遮蔽率スケジュール-*/ 
	//P_MENN op[pmax], lp[pmax], *mp ;    

	// stack overflowが発生するので修正
	//    satoh makoto 2008/11/8
	BBDP   *BDP;
	OBS    *obs;
	TREE   *tree;    /*-樹木データ-*/
	POLYGN *poly;      /*--POLYGON--*/
	SHADTB *shadtb;     /*-LP面の日射遮蔽率スケジュール-*/
	P_MENN *op, *lp, *mp;
	P_MENN	*dummy;
	NOPLPMP Noplpmp;	// OP、LP、MPの定義数

	int Datintvl = 10;      /*--110413 higuchi add 影計算の間隔（日）--*/
	int dcnt;          /*--110413 higuchi add カウンター--*/
	SHADSTR *Sdstr;   /*--110413 higuchi add 影面積ストア--*/

	//char *st ;
	char Ipath[SCHAR];

	//long	XX ;
	//sprintf(Ipath,"123re") ;
	//XX = strtol(Ipath,&st,10) ;

	//FILE	*fset = fopen("SET.csv", "w");
	//for (double TA = 0.; TA < 45.; TA += 5.)
	//for (double TR = 10.; TR < 45; TR += 5)
	//for (double Vel = 0.1; Vel <= 1.1; Vel += 0.1)
	//for (double RH = 10; RH < 100; RH += 40)
	//for (double Met = 1.; Met <= 4; Met += 1.)
	//for (double Clo = 0.0; Clo <= 4; Clo += 1.)
	//double TA = 20;
	//double TR = 17.;
	//double Vel = 0.1;
	//double RH = 40;
	//double Met = 1.;
	//double Clo = 0.6;
	//	fprintf(fset,"%.0lf,%.0lf,%.1lf,%.0lf,%.1lf,%.1lf,%.2lf\n", TA, TR, Vel, RH, Met, Clo, SET_star(TA, TR, Vel, RH, Met, Clo, 0., 101.3));
	//fclose(fset);


	BDP = NULL;
	obs = NULL;
	tree = NULL;
	poly = NULL;
	shadtb = NULL;
	op = NULL;
	lp = NULL;
	dummy = NULL;

	//BDP = (BBDP *)malloc(sizeof (BBDP)* MAXBDP);
	//obs = (OBS *)malloc(sizeof (OBS)* MAXOBS);
	//tree = (TREE *)malloc(sizeof (TREE)* MAXTREE);
	//poly = (POLYGN *)malloc(sizeof (POLYGN)* MAXPOLY);
	//shadtb = (SHADTB *)malloc(sizeof (SHADTB)* pmax);
	//op = (P_MENN *)malloc(sizeof (P_MENN)* pmax);
	//lp = (P_MENN *)malloc(sizeof (P_MENN)* pmax);

	//// 動的確保したメモリの初期化
	//void	KAGE_SUNValinit(BBDP *bdp, OBS *obs, TREE *tree, POLYGN *poly, SHADTB *shadtb, P_MENN *op, P_MENN *lp);
	//KAGE_SUNValinit(BDP, obs, tree, poly, shadtb, op, lp);

	//dummy = op;
	//for (k = 0; k < pmax; k++, dummy++)
	//	dummy->P = NULL;
	//dummy = lp;
	//for (k = 0; k < pmax; k++, dummy++)
	//	dummy->P = NULL;

	mp = NULL;

	// ここまで修正　satoh 2008/11/8

	/*---------------higuchi add--------------------end*/

	Contl = NULL;
	Ctlif = NULL;
	Ctlst = NULL;

	Mpath = NULL;
	Plist = NULL;
	Pelm = NULL;
	Elout = NULL;
	Elin = NULL;
	ferr = NULL;

	Wd.EarthSurface = NULL;
	Exsf.EarthSrfFlg = 'N';
	Exsf.Nexs = 0;
	Exsf.Exs = NULL;
	Soldy = NULL;
	Loc.name = NULL;
	Fbmlist = NULL;

	Floutinit(Flout);
	Rmvlsinit(&Rmvls);
	Simcinit(&Simc);

	Eqsysinit(&Eqsys);
	Locinit(&Loc);
	Eqcatinit(&Eqcat);
	Schdlinit(&Schdl);

	Path = NULL;
	/*--*/
	if (Narg >= 3)
	{
		for (i = 2; i < Narg; i++)
		{
			if (strcmp(File[i], "-nstop") == 0)
				NSTOP = 1;
			else if (strcmp(File[i], "-delay") == 0)
				DISPLAY_DELAY = 1;
			else if (*File[i] != '-')
			{
				Path = stralloc(File[i]);

				if ((st = strchr(Path, '\"')) != 0)
					*st = '\0';

#if WIN32
				_chdir(Path);
#else
				chdir(Path);
#endif

				free(Path);
			}
			else
				NSTOP = 1;
		}
	}
	/*--*/

	// コンソール出力の遅延を回避
	if (DISPLAY_DELAY == 1)
		setvbuf(stdout, (char *)NULL, _IONBF, 0);

	/* ------------------------------------------------------ */

	sprintf(Err, ERRFMT, "(main)");
	Psyint();

	//for(i=0 ;i<Narg ; i++){
	//	printf("[%d]=%s\n",i,File[i]);
	//}
	// printf("File=%s %s\n",File[1],File[2]);
	if (Narg == 1)
	{
		printf("=== (main)  Input data file name ? \n");
		scanf("%s", s);
		File[1] = s;
		printf(" data file = %s\n", s);
	}

	//printf("Narg=%d file=%s\n",Narg,File[1]);
	Ifile = NULL;
	Ifile = stralloc(File[1]);
	strcpy(s, Ifile);

	// 入力されたパスが"で始まる場合に除去する
	if (Ifile[0] == '"')
		sscanf(Ifile, "\"%~[\"]\"", s);

	if ((st = strrchr(Ifile, '\\')) != NULL)
		*st = '\0';

	// 一時吐き出しファイル"*.ewk"のパス変更
	strcat(strcpy(Ipath, Ifile), "\\");

	eesprera(s, Ipath);
	char EWKFile[SCHAR];
	strcpy(EWKFile, STRCUT(s, '.'));

	eespre("bdata0.ewk", EWKFile, &key);

	Simc.File = File[1];
	Simc.Loc = &Loc;

	Eeinput(EWKFile, &Simc, &Schdl, &Exsf, &Rmvls, &Eqcat, &Eqsys, &Compnt, &Ncompnt, &Ncmpalloc,
		&Elout, &Nelout, &Elin, &Nelin, &Mpath, &Nmpath, &Plist, &Pelm, &Npelm,
		&Contl, &Ncontl, &Ctlif, &Nctlif, &Ctlst, &Nctlst, Flout, &Nflout, &Wd,
		&Daytm, key, &Nplist,
		&bdpn, &obsn, &treen, &shadn, &polyn, &BDP, &obs, &tree, &shadtb, &poly, &monten, &gpn, &DE, &Noplpmp);
	//printf ( "bdpn=%d \n", bdpn ) ;

	// 外部障害物のメモリを確保
	op = (P_MENN *)malloc(sizeof (P_MENN)* Noplpmp.Nop);
	lp = (P_MENN *)malloc(sizeof (P_MENN)* Noplpmp.Nlp);
	void P_MENNinit(P_MENN *pm, int N);
	P_MENNinit(op, Noplpmp.Nop);
	P_MENNinit(lp, Noplpmp.Nlp);

	// 最大収束回数のセット
	int		LOOP_MAX;
	LOOP_MAX = VAV_Count_MAX = Simc.MaxIterate;

	free(Ifile);
	//if(Rmvls.Sd->alicsch != NULL)
	//	printf("aaaaaa\n") ;
	// 動的カーテンの展開
	Sd = Rmvls.Sd;
	for (i = 0; i < Rmvls.Nsrf; i++, Sd++)
	{
		if (Sd->dynamiccode != NULL)
			ctifdecode(Sd->dynamiccode, Sd->ctlif, &Simc, Ncompnt, Compnt, Nmpath, Mpath, &Wd, &Exsf, &Schdl);
	}

	/////////////////////////////////
	sprintf(hptest, "Main1");
	HeapCheck(hptest);
	/////////////////////////////////

	if (bdpn != 0){

		strcpy(RET, STRCUT(s, '.'));
		//printf ( "SSSSS1\n" ) ;
		strcpy(RET1, RET);
		//printf ( "SSSSS2\n" ) ;
		strcpy(RET3, RET);
		//printf ( "SSSSS3\n" ) ;
		strcpy(RET14, RET);
		//printf ( "SSSSS4\n" ) ;
		strcpy(RET15, RET);

		//printf ( "SSSSS5\n" ) ;

#if TEST
		strcpy(RET5,RET) ;
		strcpy(RET6,RET) ;
		strcpy(RET7,RET) ;
		strcpy(RET8,RET) ;
		strcpy(RET9,RET) ;
		strcpy(RET10,RET) ; 
		strcpy(RET11,RET) ;
		strcpy(RET12,RET) ;
		strcpy(RET13,RET) ;
		strcpy(RET4,RET) ; 
		strcpy(RET16,RET) ;

		strcat(RET5,"_GP") ;
		strcat(RET4,"_ffaclp") ; 
		strcat(RET6,"_OPMP") ;
		strcat(RET7,"_MP") ;
		strcat(RET8,"_LP") ;
		strcat(RET9,"_LPE") ;
		strcat(RET10,"_MPE") ;
		strcat(RET11,"_ULLP") ;
		strcat(RET12,"_ULMP") ; 
		strcat(RET13,"_OPLP") ;
		strcat(RET16,"_SHAD") ;
#endif    
		strcat(RET, "_shadow.gchi");
		//printf ( "SSSSS6\n" ) ;
		strcat(RET1, "_I.gchi");
		//printf ( "SSSSS7\n" ) ;
		strcat(RET3, "_ffactor.gchi");
		//printf ( "SSSSS8\n" ) ;
		strcat(RET14, "_lwr.gchi");
		//printf ( "SSSSS9\n" ) ;

		if ((fp1 = fopen(RET, "w")) == NULL){
			printf("File not open _shadow.gchi\n");
			exit(1);
		}
		if ((fp2 = fopen(RET1, "w")) == NULL){
			printf("File not open _I.gchi\n");
			exit(1);
		}

		if ((fp3 = fopen(RET14, "w")) == NULL){
			printf("File not open _lwr.gchi\n");
			exit(1);
		}
		if ((fp4 = fopen(RET3, "w")) == NULL){
			printf("File not open _ffactor.gchi\n");
			exit(1);
		}

		/*---座標の変換-------*/
		LP_COORDNT(&lpn, bdpn, obsn, treen, polyn, poly, tree, obs, BDP, lp);

		OP_COORDNT(&opn, bdpn, BDP, op, polyn, poly);

		/*---LPの構造体に日毎の日射遮蔽率を代入----*/
		for (i = 0; i < lpn; i++){
			for (j = 0; j < shadn; j++){
				if (strcmp(lp[i].opname, shadtb[j].lpname) == 0){
					for (k = 1; k < 366; k++){
						for (l = 0; l < shadtb[j].indatn; l++){
							if ((k >= shadtb[j].ndays[l]) && (k <= shadtb[j].ndaye[l])){
								lp[i].shad[k] = shadtb[j].shad[l];
								break;
							}
						}
					}
				}
			}
		}


#if TEST
		lp_shad_printf(lpn,lp, RET16) ;
#endif  

		/*----mpの総数をカウント mpは、OP面+OPW面---------------*/
		for (i = 0; i < opn; i++){
			mpn = mpn + 1;
			for (j = 0; j < op[i].wd; j++)
				mpn = mpn + 1;
		}

		/*---窓 壁のカウンター変数の初期化--*/
		wap = (double *)malloc(sizeof(double)*opn);
		wip = (double **)malloc(sizeof(double *)*opn);
		for (i = 0; i < opn; i++){
			if (op[i].wd != 0)
				wip[i] = (double *)malloc(sizeof(double)*op[i].wd);
		}

		//printf("gpn=%d\n",gpn) ;
		/*---領域の確保   gp 地面の座標(X,Y,Z)   ---*/
		gp = (XYZ **)malloc(sizeof(XYZ *)*mpn);
		for (i = 0; i < mpn; i++)
			gp[i] = (XYZ *)malloc(sizeof(XYZ)*(gpn + 1));

		/*---領域の確保 mp    --*/
		mp = (P_MENN *)malloc(sizeof(P_MENN)*Noplpmp.Nmp);
		P_MENNinit(mp, mpn);

		/*----OP,OPWの構造体をMPへ代入する----*/
		DAINYUU_MP(mp, op, opn, mpn);

		for (i = 0; i < mpn; i++)
			fprintf(fp1, "%s\n", mp[i].opname);

		/*---ベクトルの向きを判別する変数の初期化--*/
		/*---opから見たopの位置---*/
		uop = (bekt *)malloc(sizeof(bekt)*opn);
		for (i = 0; i < opn; i++){
			uop[i].ps = (double **)malloc(sizeof(double *)*opn);
			for (j = 0; j < opn; j++)
				uop[i].ps[j] = (double *)malloc(sizeof(double)*op[j].polyd);
		}

		/*---opから見たlpの位置---*/
		ulp = (bekt *)malloc(sizeof(bekt)*opn);
		for (i = 0; i < opn; i++){
			ulp[i].ps = (double **)malloc(sizeof(double *)*lpn);
			for (j = 0; j < lpn; j++)
				ulp[i].ps[j] = (double *)malloc(sizeof(double)*lp[j].polyd);
		}

		/*---lpから見たlpの位置---*/
		ullp = (bekt *)malloc(sizeof(bekt)*lpn);
		for (i = 0; i < lpn; i++){
			ullp[i].ps = (double **)malloc(sizeof(double *)*lpn);
			for (j = 0; j < lpn; j++)
				ullp[i].ps[j] = (double *)malloc(sizeof(double)*lp[j].polyd);
		}
		//printf("Debug Print ullp end\n");

		/*---lpから見たmpの位置---*/
		ulmp = (bekt *)malloc(sizeof(bekt)*lpn);
		for (i = 0; i < lpn; i++){
			ulmp[i].ps = (double **)malloc(sizeof(double *)*mpn);
			for (j = 0; j < mpn; j++)
				ulmp[i].ps[j] = (double *)malloc(sizeof(double)*mp[j].polyd);
		}
		//printf("Debug Print ulmp end\n");

		/*------CG確認用データ作成-------*/
		HOUSING_PLACE(lpn, mpn, lp, mp, RET15);
		//printf("Debug Print HOUSING_PLACE end\n");

		/*----前面地面代表点および壁面の中心点を求める--------*/
		GRGPOINT(mp, mpn);
		for (i = 0; i<lpn; i++)
			GDATA(lp[i], &lp[i].G);
		//printf("Debug Print GDATA end\n");

		// 20170426 higuchi add 条件追加　形態係数を計算しないパターンを組み込んだ
		if (monten > 0) {
			/*---LPから見た天空に対する形態係数算出------*/
			FFACTOR_LP(lpn, mpn, monten, lp, mp);
		}

		for (i = 0; i < mpn; i++){
			for (j = 0; j < Rmvls.Sd[0].end; j++){
				if ((Rmvls.Sd[j].sname != NULL) &&
					((strcmp(Rmvls.Sd[j].sname, mp[i].opname)) == 0)){
					mp[i].exs = Rmvls.Sd[j].exs;
					mp[i].as = Rmvls.Sd[j].as;
					mp[i].alo = Rmvls.Sd[j].alo;
					mp[i].Eo = Rmvls.Sd[j].Eo;
					break;
				}
			}
		}

		//printf("Debug Print mp end\n");

		for (i = 0; i < mpn; i++){
			mp[i].refg = Exsf.Exs[mp[i].exs].Rg;
			//printf("mp[%d].refg=%f\n",i,mp[i].refg) ;
		}
		/*---面の裏か表かの判断をするためのベクトル値の算出--*/
		URA(opn, opn, op, uop, op);          /*--opから見たopの位置----*/
		URA(lpn, lpn, lp, ullp, lp);         /*--lpから見たlpの位置----*/
		URA(lpn, mpn, mp, ulmp, lp);         /*--lpから見たmpの位置----*/
		URA(opn, lpn, lp, ulp, op);          /*--opから見たlpの位置----*/



#if TEST
		/*---op,lp座標の確認-------*/
		ZPRINT(lp, op, lpn, opn, RET13) ;
		ZPRINT(mp, op, mpn, opn, RET6) ;
		mp_printf(mpn, mp, RET7) ;
		lp_printf(lpn, lp, RET8) ;
		e_printf(lpn, lp, RET9) ; 
		e_printf(mpn, mp, RET10) ;
		errbekt_printf(lpn,lpn,ullp,RET11) ;
		errbekt_printf(lpn,mpn,ulmp,RET12) ;
#endif


		fprintf(fp2, "M\nD\nmt\nname\ngl_shadow\nIsky\nIg\nIb\nIdf\nIdre\n");
		fprintf(fp3, "M\nD\nmt\nname\nRsky\nreff\nreffg\nReff\n");
	}

	/*-----------------higuchi add-----------------------------end*/

	if (DEBUG)
	{
		printf("eeinput end\n");

		Pe = Pelm;
		for (i = 0; i < Npelm; i++, Pe++)
			printf("[%3d] Pelm=%s\n", i, Pe->cmp->name);

		Eo = Elout;
		for (i = 0; i < Nelout; i++, Eo++)
			printf("[%3d] Eo_cmp=%s\n", i, Eo->cmp->name);

		printf("Npelm=%d Ncmpalloc=%d Ncompnt=%d Nelout=%d Nelin=%d\n",
			Npelm, Ncmpalloc, Ncompnt, Nelout, Nelin);
	}
	/////////////////////////////////
	sprintf(hptest, "Main2");
	HeapCheck(hptest);
	/////////////////////////////////

	Soldy = dcalloc(Exsf.Nexs, "<main>  Soldy");
	Solmon = dcalloc(Exsf.Nexs, "<main>  Soldy");
	/****
	printf("<<main>>  Simc->wfname=%s\n", Simc.wfname);
	printf("<<main>>   dTM=%d\n", Simc.dTm);*****/

	dTM = Simc.dTm;
	dminute = Simc.dTm / 60;
	cff_kWh = dTM / 3600. / 1000.;

	Rm = Rmvls.Room;
	for (rm = 0; rm < Rmvls.Nroom; rm++, Rm++)
		Rm->Qeqp = 0.0;

	dprschtable(Schdl.Seasn, Schdl.Wkdy, Schdl.Dsch, Schdl.Dscw);
	//dprschdata ( Schdl.Sch, Schdl.Scw ) ;
	//dprachv ( Rmvls.Nroom, Rmvls.Room ) ;
	dprexsf(Exsf.Exs);
	dprwwdata(Rmvls.Wall, Rmvls.Window);
	dprroomdata(Rmvls.Room, Rmvls.Sd);
	dprballoc(Rmvls.Mw, Rmvls.Sd);

	/////////////////////////////////
	sprintf(hptest, "Main3");
	HeapCheck(hptest);
	/////////////////////////////////

	eeflopen(&Simc, Nflout, Flout);

	if (DEBUG)
	{
		printf("<<main>> eeflopen \n");
	}

	if (ferr)
		fprintf(ferr, "\n<<main>> eeflopen end\n");

	Tinit(Rmvls.Twallinit, Rmvls.Nroom, Rmvls.Room,
		Rmvls.Nsrf, Rmvls.Sd, Rmvls.Nmwall, Rmvls.Mw);

	if (DEBUG)
	{
		printf("<<main>> Tinit\n");
	}

	if (ferr)
		fprintf(ferr, "\n<<main>> Tinit\n");


	// ボイラ機器仕様の初期化
	Boicaint(Eqcat.Nboica, Eqcat.Boica, &Simc, Ncompnt, Compnt, &Wd, &Exsf, &Schdl);
	Mecsinit(dTM, &Eqsys, &Simc, Ncompnt, Compnt, Exsf.Nexs, Exsf.Exs, &Wd, &Rmvls);

	if (DEBUG)
	{
		printf("<<main>> Mecsinit\n");
	}

	if (ferr)
		fprintf(ferr, "\n<<main>> Mecsinit\n");

	/*******************

	1997.11.18	熱損失係数計算用改訂

	*******************/

	bdhpri(Simc.ofname, Rmvls, &Exsf);

	//	xprtwallinit (Rmvls.Nmwall, Rmvls.Mw);

	/* --------------------------------------------------------- */

	Daytm.ddpri = 0;

	if (Simc.sttmm < 0)
		Simc.sttmm = dminute;

	tt = Simc.sttmm / 100;
	mm = Simc.sttmm % 100;
	mta = (tt * 60 + mm) / dminute;
	mtb = (24 * 60) / dminute;
	//mtb = 12;
	// 110413 higuchi add  影面積をストアして、影計算を10日おきにする
	Sdstr = (SHADSTR*)malloc(sizeof (SHADSTR)* mpn);
	for (i = 0; i < mpn; i++)
	{
		Sdstr[i].sdsum = (double *)malloc(sizeof(double)*mtb);
		int jj;
		for (jj = 0; jj < mtb; jj++)
			Sdstr[i].sdsum[jj] = 0.0;
	}

	dcnt = 0;

	for (nday = Simc.daystartx; nday <= Simc.dayend; nday++)
	{
		if (dcnt == Datintvl) {
			dcnt = 0;
			MATINIT_sdstr(mpn, mtb, Sdstr);  // 110413 higuchi add
		}
		dcnt = dcnt + 1;  // 110413 higuchi add

		if (dayprn && ferr)
			fprintf(ferr, "\n\n\t===== Dayly Loop =====\n\n");

		// 計算する日付の決定
		day = (nday > 365 ? nday - 365 : nday);
		// 周期定常計算オプション
		if (Simc.perio == 'y')
			day = Simc.daystart;
		Daytm.day = day;

		//if (day == 151)
		//	printf("aaaaaa\n");

		dayprn = Simc.dayprn[day];

		if (Simc.perio != 'y' && nday > Simc.daystartx)
			monthday(&Daytm.Mon, &Daytm.Day, Daytm.Mon, Daytm.Day);

		if (nday >= Simc.daystart)
			Daytm.ddpri = 1;
		if (Simc.perio == 'y' && nday != Simc.dayend)
			Daytm.ddpri = 0;

		if (nday > Simc.daystartx)
			mta = 1;

		for (mt = mta; mt <= mtb; mt++)
		{
			if (dayprn && ferr)
				fprintf(ferr, "\n\n\t===== Timely Loop =====\n\n");

			//printf("dcnt=%d,nday=%d,mt=%d, mta=%d, mtb=%d, dminute=%d, mm=%d, tt=%d\n",dcnt,nday,mt,mta,mtb,dminute,mm,tt) ;

			if (mm >= 60)
			{
				mm -= 60;
				tt++;
			}

			if (tt > 24 || (tt == 24 && mm > 0))
				tt -= 24;

			Daytm.tt = tt;
			Daytm.ttmm = tt * 100 + mm;
			Daytm.time = (double)Daytm.ttmm / 100.0;
			//if (Daytm.Mon == 11 && Daytm.Day == 12 && Daytm.ttmm == 800)
			//	printf("aaaaaa\n");

			if (DEBUG)
			{
				printf("<< main >> nday=%d mm=%d mt=%d  tt=%d mm=%d\n",
					nday, mm, mt, tt, mm);
			}

			//if (day == 16 && Daytm.ttmm == 800)
			//	printf("xxxxxx\n");

			Vcfinput(&Daytm, Simc.Nvcfile, Simc.Vcfile, Simc.perio);
			Weatherdt(&Simc, &Daytm, &Loc, &Wd, Exsf.Exs, Exsf.EarthSrfFlg);

			if (dayprn && ferr)
			{
				fprintf(ferr, "\n\n\n---- date=%2d/%2d nday=%d day=%d time=%5.2lf ----\n",
					Daytm.Mon, Daytm.Day, nday, day, Daytm.time);
			}

			if (DEBUG)
			{
				printf("---- date=%2d %2d nday=%d day=%d time=%5.2lf ----\n",
					Daytm.Mon, Daytm.Day, nday, day, Daytm.time);
			}

			if (dayprn && ferr)
				Flinprt(Eqsys.Nflin, Eqsys.Flin);

			/***   if (Daytm.ttmm == 100 )****/
			if (mt == mta)
			{
				printf("%d/%d", Daytm.Mon, Daytm.Day);
				if (nday < Simc.daystart)
					printf(")");
				if (Daytm.ddpri && Simc.dayprn[day])
					printf(" *");
				printf("\n");

				/*------------------------higuchi add---形態係数の算出---------start*/
				//printf("nday=%d,day=%d\n",nday,day) ;
				//printf("bdpn=%d\n",bdpn) ;

				// 20170426 higuchi add 形態係数を計算しない処理の追加
				if (bdpn != 0 && monten > 0){
					if (nday == Simc.daystartx){
						printf("form_factor calcuration start\n");
						GR_MONTE_CARLO(mp, mpn, lp, lpn, monten, day);
						MONTE_CARLO(mpn, lpn, monten, mp, lp, gp, gpn, day, Simc.daystartx);
						ffactor_printf(fp4, mpn, lpn, mp, lp, Daytm.Mon, Daytm.Day);
#if TEST
						gp_printf(gp, mp, mpn, lpn, RET5) ;
						ffactor_printf(fp4, lpn, mpn, lp, mp, Daytm.Mon, Daytm.Day) ;
#endif
						printf("form_factor calcuration end\n");
					}
					else{
						for (i = 0; i < lpn; i++){
							k = day - 1;
							if (k == 0)
								k = 365;
							if (lp[i].shad[day] != lp[i].shad[k]){
								printf("form_factor calcuration start:shad[%d]=%f,shad[%d]=%f\n", nday, lp[i].shad[day], k, lp[i].shad[k]);
								GR_MONTE_CARLO(mp, mpn, lp, lpn, monten, day);
								MONTE_CARLO(mpn, lpn, monten, mp, lp, gp, gpn, day, Simc.daystartx);
								ffactor_printf(fp4, mpn, lpn, mp, lp, Daytm.Mon, Daytm.Day);
								printf("form_factor calcuration end\n");
								break;
							}
						}
					}

				}
				/*------------------------higuchi add-----------------------end*/



				if (DEBUG)
				{
					printf(" ** daymx=%d  Tgrav=%lf  DT=%lf  Tsupw=%lf\n",
						Loc.daymxert, Loc.Tgrav, Loc.DTgr, Wd.Twsup);
				}
			}

			// 傾斜面日射量の計算
			Exsfsol(Exsf.Nexs, &Wd, Exsf.Exs);

			/*==transplantation to eeslism from KAGExSUN by higuchi 070918==start*/
			if (bdpn != 0){

				MATINIT_sum(opn, op);
				MATINIT_sum(mpn, mp);
				MATINIT_sum(lpn, lp);


				ls = -Wd.Sw; ms = -Wd.Ss; ns = Wd.Sh;

				if (Wd.Sh > 0.0){

					// 110413 higuchi add 下の条件
					if (dcnt == 1){

						for (j = 0; j < opn; j++){

							wap[j] = 0.0;
							for (i = 0; i<op[j].wd; i++)
								wip[j][i] = 0.0;
							CINC(op[j], ls, ms, ns, &co);
							if (co>0.0){
								SHADOW(j, DE, opn, lpn, ls, ms, ns,
									&uop[j], &ulp[j], &op[j], op, lp, &wap[j], wip[j], day);
							}
							else{
								op[j].sum = 1.0;
								for (i = 0; i < op[j].wd; i++)
									op[j].opw[i].sumw = 1.0;
							}
						}
						//printf("dcnt1=%d\n",dcnt) ;
						DAINYUU_SMO2(opn, mpn, op, mp, Sdstr, dcnt, mt);

						// 20170426 higuchi add 条件追加
						if (dayprn) {
							shadow_printf(fp1, Daytm.Mon, Daytm.Day, Daytm.time, mpn, mp);
						}

					}
					else {
						//printf("dcnt2=%d\n",dcnt) ;
						DAINYUU_SMO2(opn, mpn, op, mp, Sdstr, dcnt, mt);
						// 20170426 higuchi add 条件追加
						if (dayprn) {
							shadow_printf(fp1, Daytm.Mon, Daytm.Day, Daytm.time, mpn, mp);
						}
					}

					//SHADSTR *Sdstrd;
					//Sdstrd = Sdstr;
					//for (i = 0; i < mpn; i++, Sdstrd++)
					//{
					//	int m;
					//	for (m = 0; m < mtb; m++)
					//		printf("Sdstr[%d].sdsum[%d]=%lf\n", i, m, Sdstrd->sdsum[m]);
					//}
				}

				// 20170426 higuchi add 条件追加
				if (dayprn) {
					fprintf(fp2, "%d %d %5.2f\n", Daytm.Mon, Daytm.Day, Daytm.time);
					fprintf(fp3, "%d %d %5.2f\n", Daytm.Mon, Daytm.Day, Daytm.time);
				}

				// 20170426 higuchi add 引数追加 dayprn,monten
				OPIhor(fp2, fp3, lpn, mpn, mp, lp, &Wd, ullp, ulmp, gp, day, dayprn, monten);

				for (i = 0; i < Rmvls.Sd[0].end; i++){
					if (Rmvls.Sd[i].sname != NULL){
						for (j = 0; j < mpn; j++){
							if ((strcmp(Rmvls.Sd[i].sname, mp[j].opname)) == 0){
								Rmvls.Sd[i].Fsdw = mp[j].sum;
								//printf("Sd->Fswd=%lf\n", Rmvls.Sd[i].Fsdw);
								Rmvls.Sd[i].Idre = mp[j].Idre;
								Rmvls.Sd[i].Idf = mp[j].Idf;
								Rmvls.Sd[i].Iw = mp[j].Iw;
								Rmvls.Sd[i].rn = mp[j].Reff;
								//printf("Sd->ali=%lf\n", Rmvls.Sd[i].ali);
								break;
							}
						}
					}
				}
				/////////////////////////////////
				sprintf(hptest, "Main KAGE-SUN");
				HeapCheck(hptest);
				/////////////////////////////////
				//printf("H=%5.2f\n",Daytm.time) ;
			}


			/*===============higuchi 070918============================end*/

			if (dayprn && ferr)
				xprsolrd(Exsf.Nexs, Exsf.Exs);

			if (DEBUG)
			{
				xprsolrd(Exsf.Nexs, Exsf.Exs);
				printf("<<main>> Exsfsol\n");
			}

			// 現時刻ステップのスケジュール作成
			Eeschdlr(day, Daytm.ttmm, &Schdl, &Rmvls);

			if (DEBUG)
			{
				printf("<<main>>  Eeschdlr\n");
			}

			// 制御で使用する状態値を計算する（集熱器の相当外気温度）
			CalcControlStatus(&Eqsys, &Rmvls, &Wd, &Exsf);
			// 制御情報の更新
			Contlschdlr(dTM, Ncontl, Contl, Nmpath, Mpath, Ncompnt, Compnt);
			// 空調発停スケジュール設定が完了したら人体発熱を再計算
			Qischdlr(Rmvls.Nroom, Rmvls.Room);

			if (DEBUG)
			{
				printf("<<main>> Contlschdlr\n");
			}

			/***
			eloutprint(0, Nelout, Elout, Compnt);
			*****/

			VAVcountreset(Eqsys.Nvav, Eqsys.vav);
			Valvcountreset(Eqsys.Nvalv, Eqsys.Valv);
			Evaccountreset(Eqsys.Nevac, Eqsys.Evac);

			/*---- Satoh Debug VAV  2000/12/6 ----*/
			/* VAV 計算繰り返しループの開始地点 */
			for (j = 0; j < VAV_Count_MAX; j++)
			{
				if (DEBUG)
					printf("\n\n====== VAV LOOP Count=%d ======\n\n\n", j);
				if (dayprn && ferr)
					fprintf(ferr, "\n\n====== VAV LOOP Count=%d ======\n\n\n", j);

				VAVreset = 0;
				Valvreset = 0;

				Pumpflow(Eqsys.Npump, Eqsys.Pump);

				if (DEBUG)
				{
					printf("<<main>> Pumpflow\n");
				}

				if (Simc.dayprn[day] && ferr)
					fprintf(ferr, "<<main>> Pumpflow\n");

				Pflow(Nmpath, Mpath, &Wd);

				if (DEBUG)
				{
					printf("<<main>> Pflow\n");
				}

				if (dayprn && ferr)
					fprintf(ferr, "<<main>> Pflow\n");

				/************
				eloutprint(0, Nelout, Elout, Compnt);
				***********/

				Sysupv(Nmpath, Mpath, &Rmvls);

				if (DEBUG)
				{
					printf("<<main>> Sysupv\n");
				}

				if (dayprn && ferr)
					fprintf(ferr, "<<main>> Sysupv\n");

				/*****
				elinprint(0, Ncompnt, Compnt, Elout, Elin);
				***********/

				mrk = Rmvls.Emrk;
				for (i = 0; i < Rmvls.Nroom; i++, mrk++)
					*mrk = '!';

				Sd = Rmvls.Sd;
				for (n = 0; n < Rmvls.Nsrf; n++, Sd++)
					Sd->mrk = '!';

				Mecscf(&Eqsys);

				if (DEBUG)
				{
					printf("<<main>> Mecscf\n");
				}

				/////////////////////////////////
				sprintf(hptest, "Main eeroomcf");
				HeapCheck(hptest);
				/////////////////////////////////

				/*======higuchi update 070918==========*/
				eeroomcf(&Wd, &Exsf, &Rmvls, nday, mt);
				/*=====================================*/
				//if (tt == 7)
				//	free(Exsf.Exs[6].name);

				if (DEBUG)
				{
					printf("<<main>> eeroomcf\n");
				}

				/*   作用温度制御時の設定室内空気温度  */
				Rmotset(Rmvls.Nroom, Rmvls.Room);
				if (DEBUG)
				{
					printf("<<main>> Rmotset End\n");
				}

				/* 室、放射パネルのシステム方程式作成 */
				Roomvar(Rmvls.Nroom, Rmvls.Room, Rmvls.Nrdpnl, Rmvls.Rdpnl);

				if (DEBUG)
				{
					printf("<<main>>  Roomvar\n");
					eloutprint(1, Nelout, Elout, Compnt);
					elinprint(1, Ncompnt, Compnt, Elout, Elin);
				}

				if (dayprn && ferr)
				{
					fprintf(ferr, "<<main>>  Roomvar\n");
					eloutfprint(1, Nelout, Elout, Compnt);
					elinfprint(1, Ncompnt, Compnt, Elout, Elin);
				}
				//eloutprint(1, Nelout, Elout, Compnt);  

				//hcldmodeinit ( &Eqsys ) ;

				for (i = 0; i < LOOP_MAX; i++)
				{
					char	s[256];
					sprintf(s, "Loop Start %d", i);
					HeapCheck(s);

					if (i == 0)
						hcldwetmdreset(&Eqsys);

					if (DEBUG)
						printf("再計算が必要な機器のループ %d\n", i);

					if (dayprn && ferr)
						fprintf(ferr, "再計算が必要な機器のループ %d\n\n\n", i);

					LDreset = 0;
					DWreset = 0;
					TKreset = 0;
					BOIreset = 0;
					Evacreset = 0;
					PCMfunreset = 0;

					/********************************
					if ( TKreset > 0 )
					printf("<< main >> nday=%d mt=%d  tt=%d mm=%d TKreset=%d\n",
					nday, mt, tt, mm, TKreset );
					****************************/

					Stankcfv(Eqsys.Nstank, Eqsys.Stank);

					HeapCheck("Hcldcfv Start");
					Hcldcfv(Eqsys.Nhcload, Eqsys.Hcload);
					HeapCheck("Hcldcfv End");

					Syseqv(Nelout, Elout, &Syseq);

					Sysvar(Ncompnt, Compnt);

					HeapCheck("Roomene Start");
					Roomene(&Rmvls, Rmvls.Nroom, Rmvls.Room, Rmvls.Nrdpnl, Rmvls.Rdpnl, &Exsf, &Wd);

					Roomload(Rmvls.Nroom, Rmvls.Room, &LDreset);
					HeapCheck("Roomload End");

					// PCM家具の収束判定
					PCMfunchk(Rmvls.Nroom, Rmvls.Room, &Wd, &PCMfunreset);

					// 壁体内部温度の計算と収束計算のチェック
					if (Rmvls.pcmiterate == 'y')
						PCMwlchk(i, &Rmvls, &Exsf, &Wd, &LDreset);
					//if (PCMfunreset > 0)
					//{
					//	// 室の係数、定数項の計算
					//	eeroomcf(&Wd, &Exsf, &Rmvls, nday, mt);
					//	/*   作用温度制御時の設定室内空気温度  */
					//	Rmotset(Rmvls.Nroom, Rmvls.Room);
					//	/* 室、放射パネルのシステム方程式作成 */
					//	Roomvar(Rmvls.Nroom, Rmvls.Room, Rmvls.Nrdpnl, Rmvls.Rdpnl);
					//}

					Boiene(Eqsys.Nboi, Eqsys.Boi, &BOIreset);

					Refaene(Eqsys.Nrefa, Eqsys.Refa, &LDreset);

					HeapCheck("Hcldene Start");
					Hcldene(Eqsys.Nhcload, Eqsys.Hcload, &LDreset, &Wd);
					HeapCheck("Hcldene End");

					Hccdwreset(Eqsys.Nhcc, Eqsys.Hcc, &DWreset);

					Stanktss(Eqsys.Nstank, Eqsys.Stank, &TKreset);

					Evacene(Eqsys.Nevac, Eqsys.Evac, &Evacreset);

					if ((BOIreset + LDreset + DWreset + TKreset + Evacreset + PCMfunreset) == 0)
						break;
				}

				if (i == LOOP_MAX)
					printf("収束しませんでした。 MAX=%d\n", LOOP_MAX);

				// 状態値ループの終了
				HeapCheck("Loop End");

				//printf ( "Loop=%d\n", i ) ;
				Hccene(Eqsys.Nhcc, Eqsys.Hcc);
				// 風量の計算は最初だけ
				//if (i == 0)
					VAVene(Eqsys.Nvav, Eqsys.vav, &VAVreset);
				Valvene(Eqsys.Nvalv, Eqsys.Valv, &Valvreset);

				//printf ("\n\nVAVreset=%d\n", VAVreset ) ;
				/***************/
				if (VAVreset == 0 && Valvreset == 0)
					break;
				VAVcountinc(Eqsys.Nvav, Eqsys.vav);
				Valvcountinc(Eqsys.Nvalv, Eqsys.Valv);

				// 風量が変わったら電気蓄熱暖房器の係数を再計算
				Stheatcfv(Eqsys.Nstheat, Eqsys.stheat);
				/*****************/

				/*---- Satoh Debug VAV  2000/12/6 ----*/
				/* VAV 計算繰り返しループの終了地点 */
			}

			// 太陽電池内蔵壁体の発電量計算
			CalcPowerOutput(Rmvls.Nsrf, Rmvls.Sd, &Wd, &Exsf);

			//VAVcountreset ( Eqsys.Nvav, Eqsys.vav ) ;

			//printf ( "VAV Loop=%d\n", j ) ;

			if (Simc.helmkey == 'y')
				Helmroom(Rmvls.Nroom, Rmvls.Room, Rmvls.Qrm, &Rmvls.Qetotal, dTM, Wd.T, Wd.x);

			/*************
			printf ("xxxmain Pathheat\n");
			Pathheat(Nmpath, Mpath);
			************************/

			// 室の熱取得要素の計算
			Qrmsim(Rmvls.Room, &Wd, Rmvls.Qrm);

			Rm = Rmvls.Room;
			for (rm = 0; rm < Rmvls.Nroom; rm++, Rm++)
				Rm->Qeqp = 0.0;

			if (DEBUG)
				printf("Mecsene st\n");

			/*  システム使用機器の供給熱量、エネルギーの計算  */
			Mecsene(&Eqsys);

			/***********************
			printf ( "Mecsene en\n" ) ;
			/***********************/

			if (DEBUG)
			{
				mecsxprint(&Eqsys);
			}

			/* ------------------------------------------------ */
			if (DEBUG)
				printf("xxxmain 2\n");

			// 前時刻の室温の入れ替え、OT、MRTの計算
			Rmsurft(Rmvls.Nroom, Rmvls.Room, Rmvls.Sd);

			if (DEBUG)
				printf("xxxmain 3\n");

			//if (Daytm.Mon == 1 && Daytm.Day == 5 && fabs(Daytm.time - 23.15) < 1.e-5)
			//	printf("debug\n");

			// 壁体内部温度の計算（ヒステリシス考慮PCMの状態値もここで設定）
			RMwlt(Rmvls.Nmwall, Rmvls.Mw);

			if (DEBUG)
				printf("xxxmain 4\n");

			// PMV、SET*の計算
			Rmcomfrt(Rmvls.Nroom, Rmvls.Room);

			if (DEBUG)
				printf("xxxmain 5\n");

			//xprsolrd (Exsf.Nexs, Exsf.Exs);

			// 代表日の毎時計算結果のファイル出力
			Eeprinth(&Daytm, &Simc, Nflout, Flout, &Rmvls, &Exsf,
				Nmpath, Mpath, &Eqsys, &Wd);

			if (DEBUG)
				printf("xxxmain 6\n");

			if (Daytm.ddpri)
			{
				// 室の日集計、月集計
				Roomday(Daytm.Mon, Daytm.Day, day, Daytm.ttmm, Rmvls.Nroom, Rmvls.Room, Rmvls.Nrdpnl, Rmvls.Rdpnl, Simc.dayend);
				if (Simc.helmkey == 'y')
					Helmdy(day, Rmvls.Nroom, Rmvls.Room, &Rmvls.Qetotal);

				Compoday(Daytm.Mon, Daytm.Day, day, Daytm.ttmm, &Eqsys, Simc.dayend);
				/**   if (Nqrmpri > 0)  **/
				Qrmsum(Daytm.Day, Rmvls.Nroom, Rmvls.Room, Rmvls.Qrm,
					Rmvls.Trdav, Rmvls.Qrmd);

				if (DEBUG)
					printf("xxxmain 7\n");

				// 気象データの日集計、月集計
				Wdtsum(Daytm.Mon, Daytm.Day, day, Daytm.ttmm, &Wd, Exsf.Nexs, Exsf.Exs, &Wdd, &Wdm, Soldy, Solmon, &Simc);
			}

			if (DEBUG)
				printf("xxxmain 8\n");

			if (DEBUG)
			{
				//xprtwpanel (Rmvls.Nroom, Rmvls.Room, Twp, Sd, Mw); 
				xprtwsrf(Rmvls.Nsrf, Rmvls.Sd);
				xprrmsrf(Rmvls.Nsrf, Rmvls.Sd);
				xprtwall(Rmvls.Nmwall, Rmvls.Mw);
			}

			mm += dminute;

			// 時刻ループの最後
			HeapCheck("Time Loop End");
		}
		// 日集計の出力
		Eeprintd(&Daytm, &Simc, Nflout, Flout, &Rmvls, Exsf.Nexs, Exsf.Exs, Soldy, &Eqsys, &Wdd);
		/*****printf ("xxxmain 9\n");/****/

		//if (Daytm.Mon == 4 && Daytm.Day == 25)
		//	printf("debug\n");

		// 月集計の出力
		if (isEndDay(Daytm.Mon, Daytm.Day, Daytm.day, Simc.dayend) == 1 && Daytm.ddpri)
		{
			//printf("月集計出力\n") ;
			Eeprintm(&Daytm, &Simc, Nflout, Flout, &Rmvls, Exsf.Nexs, Exsf.Exs, Solmon, &Eqsys, &Wdm);
		}
	}
	//free(Exsf.Exs[6].name);

	// 月－時刻別集計値の出力
	Eeprintmt(&Simc, Nflout, Flout, &Eqsys, Rmvls.Nrdpnl, Rmvls.Rdpnl);

	if (DEBUG)
	{
		printf("メモリ領域の解放\n");
	}

	eeflclose(Nflout, Flout);
	//printf("1\n") ;
	Eloutfree(Nelout, Elout);
	//printf("2\n") ;
	HeapCheck("Memoryfree");
	SchdlFree(&Schdl);
	//printf("3\n") ;
	SimcFree(&Simc);
	//printf("4\n") ;
	Floutfree(Flout);
	//printf("5\n") ;
	free(Loc.name);
	//printf("6\n") ;
	Compntfree(Ncmpalloc, Compnt);
	//printf("7\n") ;
	if (Wd.EarthSurface != NULL)
		free(Wd.EarthSurface);

	Plistfree(Nplist, Plist);
	//printf("8\n") ;
	free(Plist);
	//printf("9\n") ;

	Mpathfree(Nmpath, Mpath);
	//printf("10\n") ;
	free(Mpath);
	//printf("11\n") ;

	Eqcatfree(&Eqcat);
	Eqsysfree(&Eqsys);
	Rmvlsfree(&Rmvls);
	//printf("12\n") ;

	//Eloutfree ( Nelout, Elout ) ;

	free(Contl);
	free(Ctlst);
	free(Ctlif);
	HeapCheck("最後");
	Exsffree(&Exsf);
	free(Soldy);
	free(Solmon);
	free(Pelm);
	free(Elout);
	free(Fbmlist);
	free(Elin);

	/*------------------higuchi add---------------------start*/
	if (bdpn != 0){
		for (i = 0; i < opn; i++){
			for (j = 0; j < opn; j++)
				free(uop[i].ps[j]);
		}
		for (i = 0; i < opn; i++)
			free(uop[i].ps);
		free(uop);

		for (i = 0; i < opn; i++){
			for (j = 0; j < lpn; j++)
				free(ulp[i].ps[j]);
		}
		for (i = 0; i < opn; i++)
			free(ulp[i].ps);
		free(ulp);

		for (i = 0; i < lpn; i++){
			for (j = 0; j < lpn; j++)
				free(ullp[i].ps[j]);
		}
		for (i = 0; i < lpn; i++)
			free(ullp[i].ps);
		free(ullp);

		for (i = 0; i < lpn; i++){
			for (j = 0; j < mpn; j++)
				free(ulmp[i].ps[j]);
		}
		for (i = 0; i < lpn; i++)
			free(ulmp[i].ps);
		free(ulmp);

		for (i = 0; i < lpn; i++)
		{
			if (lp[i].P != NULL)
				free(lp[i].P);
		}


		//for (i = 0; i < opn; i++){
		//	for (j = 0; j < op[i].wd; j++)
		//		free(op[i].opw[j].P);
		//}

		for (i = 0; i < opn; i++)
			free(op[i].P);

		free(wap);
		for (i = 0; i < opn; i++){
			if (op[i].wd != 0){
				free(wip[i]);
			}
		}
		free(wip);

		for (i = 0; i < mpn; i++)
			free(gp[i]);
		free(gp);

		// 110413 higuchi add
		for (i = 0; i < mpn; i++){
			free(Sdstr[i].sdsum);
		}
		free(Sdstr);

		// stack overflow対策で修正 satoh 2008/11/8
		BBDP *bdp;
		bdp = BDP;
		for (i = 0; i < bdpn; i++, bdp++)
		{
			free(bdp->bdpname);
			sunblk *sb;
			sb = bdp->SBLK;
			for (j = 0; j < bdp->sumsblk; j++, sb++)
			{
				free(sb->sbfname);
				free(sb->snbname);
			}
			RRMP *rp;
			rp = bdp->RMP;
			for (j = 0; j < bdp->sumRMP; j++, rp++)
			{
				free(rp->rmpname);
				free(rp->wallname);
				MADO *wp;
				wp = rp->WD;
				for (k = 0; k < rp->sumWD; k++, wp++)
					free(wp->winname);
				free(rp->WD);
			}

			free(bdp->SBLK);
		}
		free(BDP);
		OBS *obsp;
		obsp = obs;
		for (i = 0; i < obsn; i++, obsp++)
		{
			free(obsp->fname);
			free(obsp->obsname);
		}
		free(obs);
		TREE *tred;
		tred = tree;
		for (i = 0; i < treen; i++, tred++)
		{
			free(tred->treename);
			free(tred->treetype);
		}
		free(tree);
		POLYGN *pold;
		pold = poly;
		for (i = 0; i < polyn; i++, pold++)
		{
			free(pold->polyname);
			free(pold->wallname);
		}
		free(poly);
		SHADTB *shdp;
		shdp = shadtb;
		for (i = 0; i < shadn; i++, shdp++)
			free(shdp->lpname);
		free(shadtb);
		P_MENN *opp;
		opp = op;
		for (i = 0; i < Noplpmp.Nop; i++, opp++)
		{
			WD_MENN *opw;
			opw = opp->opw;
			for (j = 0; j < opp->Nopw; j++, opw++)
			{
				free(opw->opwname);
				free(opw->P);
			}
			free(opp->opw);
		}
		free(op);
		P_MENN *lpp;
		lpp = lp;
		for (i = 0; i < Noplpmp.Nlp; i++, lpp++)
		{
			WD_MENN *opw;
			opw = lpp->opw;
			for (j = 0; j < lpp->Nopw; j++, opw++)
			{
				free(opw->opwname);
				free(opw->P);
			}
			free(lpp->opw);
		}
		free(lp);
		//P_MENN *mpp;
		//mpp = mp;
		//for (i = 0; i < mpn; i++, mpp++)
		//{
		//	WD_MENN *opw;
		//	opw = mpp->opw;
		//	for (j = 0; j < mpp->Nopw; j++, opw++)
		//	{
		//		free(opw->opwname);
		//		free(opw->P);
		//	}
		//	free(mpp->opw);
		//}
		free(mp);

		fclose(fp1);
		fclose(fp2);
		fclose(fp3);
		fclose(fp4);
	}

	//HeapCheck("最後") ;

	/*---------------------higuchi 1999.7.21-----------end*/


	return (0);

}

// 月の末日かどうかをチェックする
int	isEndDay(int Mon, int Day, int Dayend, int SimDayend)
{
	static int	Nde[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int	Flg, nday;
	Flg = 0;
	nday = SimDayend;
	if (nday > 365)
		nday -= 365;
	if (Day == Nde[Mon - 1] || Dayend == nday)
		Flg = 1;

	return (Flg);
}


// KAGE-SUN用変数の初期化
//void	KAGE_SUNValinit(BBDP *bdp, OBS *obs, TREE *tree, POLYGN *poly, SHADTB *shadtb, P_MENN *op, P_MENN *lp)
//{
//	int	i, j, k, ii;
//
	//for (i = 0; i < MAXBDP; i++, bdp++)
	//{
	//	bdp->bdpname[0] = '\0';
	//	bdp->exh = bdp->exw = 0.;
	//	bdp->sumRMP = bdp->sumsblk = 0;
	//	bdp->x0 = bdp->y0 = bdp->z0 = 0.;
	//	bdp->Wa = bdp->Wb = 0.;
	//	RRMP	*rmp;
	//	rmp = bdp->RMP;
	//	for (j = 0; j < MAXRMP; j++, rmp++)
	//	{
	//		rmp->rmpname[0] = '\0';
	//		rmp->wallname[0] = '\0';
	//		rmp->sumWD = 0;
	//		rmp->ref = rmp->xb0 = rmp->yb0 = rmp->Rw = rmp->Rh = rmp->grpx = 0.;
	//		matinit(rmp->rgb, 3);
	//		MADO	*mado;
	//		mado = rmp->WD;
	//		for (k = 0; k < MAXWD; k++, mado++)
	//		{
	//			mado->winname = NULL;
	//			mado->xr = mado->yr = mado->Ww = mado->Wh = mado->ref = mado->grpx = 0.;
	//			matinit(mado->rgb, 3);
	//		}
	//	}
	//	//sunblk	*sblk;
	//	//sblk = bdp->SBLK;
	//	//for (j = 0; j < MAXSBLK; j++, sblk++)
	//	//{
	//	//	sblk->sbfname[0] = sblk->snbname[0] = '\0';
	//	//	matinit(sblk->rgb, 3);
	//	//	sblk->x = sblk->y = sblk->D = sblk->W = sblk->H = sblk->h = sblk->WA = sblk->ref = 0.;
	//	//}
	//}

	//for (i = 0; i < MAXOBS; i++, obs++)
	//{
	//	obs->fname[0] = obs->obsname[0] = '\0';
	//	obs->x = obs->y = obs->z = obs->H = obs->D = obs->W = obs->Wa = obs->Wb = 0.0;
	//	matinit(obs->ref, 4);
	//	matinit(obs->rgb, 3);
	//}

	//for (i = 0; i < MAXTREE; i++, tree++)
	//{
	//	tree->treename[0] = tree->treetype[0] = '\0';
	//	tree->x = tree->y = tree->z = tree->W1 = tree->W2 = tree->W3 = tree->W4 = 0.;
	//	tree->H1 = tree->H2 = tree->H3 = 0.0;
	//}

	//for (i = 0; i < MAXPOLY; i++, poly++)
	//{
	//	poly->polyknd[0] = poly->polyname[0] = poly->polyname[0] = '\0';
	//	poly->polyd = 0;
	//	poly->ref = poly->refg = poly->grpx = 0.;
	//	poly->P = NULL;
	//	matinit(poly->rgb, 3);
	//}

	//void matiniti(int *A, int N);
	//for (i = 0; i < pmax; i++, shadtb++)
	//{
	//	shadtb->lpname[0] = '\0';
	//	shadtb->indatn = 0;
	//	matinit(shadtb->shad, 12);
	//	matiniti(shadtb->ndays, 12);
	//	matiniti(shadtb->ndaye, 12);
	//}

//	void P_MENNinit(P_MENN *pm, int N);
//	P_MENNinit(op, pmax);
//	P_MENNinit(lp, pmax);
//}

void matiniti(int *A, int N)
{
	int i;
	for (i = 0; i < N; i++, A++)
		*A = 0;
}

void P_MENNinit(P_MENN *pm, int N)
{
	int i;
	for (i = 0; i < N; i++, pm++)
	{
		pm->Nopw = 0;
		pm->opname = NULL;
		matinit(pm->rgb, 3);
		matinit(pm->faiwall, pmax);
		pm->wd = pm->exs = 0;
		pm->grpx = pm->faia = pm->faig = pm->grpfaia = pm->sum = pm->ref = pm->refg = pm->wa = pm->wb = 0.0;
		pm->Ihor = pm->Idre = pm->Idf = pm->Iw = pm->Reff = pm->rn = pm->Te = pm->Teg = 0.;
		matinit(pm->shad, 365);
		pm->alo = pm->as = pm->Eo = 0.;
		pm->polyd = pm->sbflg = 0;
		pm->P = NULL;
		pm->opw = NULL;
		//WD_MENN *opw;
		//opw = pm->opw;
		//for (j = 0; j < MAXWD; j++, opw++)
		//{
		//	opw->opwname[0] = '\0';
		//	opw->P = NULL;
		//	opw->polyd = 0;
		//	opw->ref = opw->grpx = opw->sumw = 0.;
		//	matinit(opw->rgb, 3);
		//}
	}
}
