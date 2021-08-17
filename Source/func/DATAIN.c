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
#include <string.h>
#include "MODEL.h"
#include "exs.h"
#include "fnlib.h"
#define SCHAR 1024

/*

							  入力データの読み込み
							  FILE=DATAIN.c
							  Create Date=1999.6.7
							  */

void	preexit(void);
int		InputCount(FILE *fi, char *key);
char	*stralloc(char *s);

char NAME[SCHAR];

void HISASHI(FILE *fi, sunblk *sb){

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	sb->snbname = stralloc(NAME);

	/*-色の初期値--*/
	sb->rgb[0] = 0.0;
	sb->rgb[1] = 0.2;
	sb->rgb[2] = 0.0;

	// ex) 1Fhisa -xy 0 2.7 -DW 0.5 8.645 -a 90 ;
	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xy") == 0){
			fscanf_s(fi, "%lf", &sb->x);
			fscanf_s(fi, "%lf", &sb->y);
		}

		else if (strcmp(NAME, "-DW") == 0){
			fscanf_s(fi, "%lf", &sb->D);
			fscanf_s(fi, "%lf", &sb->W);
		}

		else if (strcmp(NAME, "-a") == 0)
			fscanf_s(fi, "%lf", &sb->WA);


		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &sb->rgb[0]);
			fscanf_s(fi, "%lf", &sb->rgb[1]);
			fscanf_s(fi, "%lf", &sb->rgb[2]);
		}

		else {
			printf("ERROR parameter----HISASI: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*--------------------------------------------------------------*/
void BARUKO(FILE *fi, sunblk *sb){

	sb->ref = 0.0;

	sb->rgb[0] = 0.0;
	sb->rgb[1] = 0.2;
	sb->rgb[2] = 0.0;


	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	sb->snbname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xy") == 0){
			fscanf_s(fi, "%lf", &sb->x);
			fscanf_s(fi, "%lf", &sb->y);
		}

		else if (strcmp(NAME, "-DHWh") == 0){
			fscanf_s(fi, "%lf", &sb->D);
			fscanf_s(fi, "%lf", &sb->H);
			fscanf_s(fi, "%lf", &sb->W);
			fscanf_s(fi, "%lf", &sb->h);
		}

		else if (strcmp(NAME, "-ref") == 0)
			fscanf_s(fi, "%lf", &sb->ref);

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &sb->rgb[0]);
			fscanf_s(fi, "%lf", &sb->rgb[1]);
			fscanf_s(fi, "%lf", &sb->rgb[2]);
		}

		else {
			printf("ERROR parameter----WBARUKONI: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*------------------------------------------------------------------*/
void SODEK(FILE *fi, sunblk *sb){

	sb->rgb[0] = 0.0;
	sb->rgb[1] = 0.2;
	sb->rgb[2] = 0.0;

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	sb->snbname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xy") == 0){
			fscanf_s(fi, "%lf", &sb->x);
			fscanf_s(fi, "%lf", &sb->y);
		}

		else if (strcmp(NAME, "-DH") == 0){
			fscanf_s(fi, "%lf", &sb->D);
			fscanf_s(fi, "%lf", &sb->H);
		}

		else if (strcmp(NAME, "-a") == 0)
			fscanf_s(fi, "%lf", &sb->WA);

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &sb->rgb[0]);
			fscanf_s(fi, "%lf", &sb->rgb[1]);
			fscanf_s(fi, "%lf", &sb->rgb[2]);
		}
		else {
			printf("ERROR parameter----SODEKABE: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*-----------------------------------------------------------------------*/
void SCREEN(FILE *fi, sunblk *sb){


	sb->rgb[0] = 0.0;
	sb->rgb[1] = 0.2;
	sb->rgb[2] = 0.0;

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	sb->snbname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xy") == 0){
			fscanf_s(fi, "%lf", &sb->x);
			fscanf_s(fi, "%lf", &sb->y);
		}

		else if (strcmp(NAME, "-DHW") == 0){
			fscanf_s(fi, "%lf", &sb->D);
			fscanf_s(fi, "%lf", &sb->H);
			fscanf_s(fi, "%lf", &sb->W);
		}

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &sb->rgb[0]);
			fscanf_s(fi, "%lf", &sb->rgb[1]);
			fscanf_s(fi, "%lf", &sb->rgb[2]);
		}

		else {
			printf("ERROR paramater---MADOHIYOKE: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*----------------------------------------------------------------*/
void rmpdata(FILE *fi, RRMP *rp, MADO *wp){

	rp->ref = 0.0;
	rp->grpx = 1.0;

	rp->rgb[0] = 0.9;
	rp->rgb[1] = 0.9;
	rp->rgb[2] = 0.9;

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	//printf("<rmpdata> 1 NAME=%s\n", NAME);
	rp->rmpname = stralloc(NAME);
	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	//printf("<rmpdata> 2 NAME=%s\n", NAME);
	rp->wallname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		//printf("<rmpdata> 3 NAME=%s\n", NAME);
		if (strcmp(NAME, "-xyb") == 0){
			fscanf_s(fi, "%lf", &rp->xb0);
			fscanf_s(fi, "%lf", &rp->yb0);
		}

		else if (strcmp(NAME, "-WH") == 0){
			fscanf_s(fi, "%lf", &rp->Rw);
			fscanf_s(fi, "%lf", &rp->Rh);
		}

		else if (strcmp(NAME, "-ref") == 0)
			fscanf_s(fi, "%lf", &rp->ref);

		else if (strcmp(NAME, "-grpx") == 0)
			fscanf_s(fi, "%lf", &rp->grpx);

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &rp->rgb[0]);
			fscanf_s(fi, "%lf", &rp->rgb[1]);
			fscanf_s(fi, "%lf", &rp->rgb[2]);
		}

		else {
			printf("ERROR parameter----RMP: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}

	rp->sumWD = 0;
	//wp = rp->WD;
	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		//printf("<rmpdata> 4 NAME=%s\n", NAME);
		wp->ref = 0.0;
		wp->grpx = 1.0;

		wp->rgb[0] = 0.0;
		wp->rgb[1] = 0.3;
		wp->rgb[2] = 0.8;

		if (strcmp(NAME, "WD") != 0){
			printf("ERROR parameter----WD: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}

		rp->sumWD = rp->sumWD + 1;

		fscanf_s(fi, "%s", NAME, sizeof(NAME));
		//printf("<rmpdata> 5 NAME=%s\n", NAME);
		wp->winname = stralloc(NAME);

		while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

			//printf("<rmpdata> 6 NAME=%s\n", NAME);
			if (strcmp(NAME, "-xyr") == 0){
				fscanf_s(fi, "%lf", &wp->xr);
				fscanf_s(fi, "%lf", &wp->yr);
			}

			else if (strcmp(NAME, "-WH") == 0){
				fscanf_s(fi, "%lf", &wp->Ww);
				fscanf_s(fi, "%lf", &wp->Wh);
			}

			else if (strcmp(NAME, "-ref") == 0)
				fscanf_s(fi, "%lf", &wp->ref);

			else if (strcmp(NAME, "-grpx") == 0)
				fscanf_s(fi, "%lf", &wp->grpx);

			else if (strcmp(NAME, "-rgb") == 0){
				fscanf_s(fi, "%lf", &wp->rgb[0]);
				fscanf_s(fi, "%lf", &wp->rgb[1]);
				fscanf_s(fi, "%lf", &wp->rgb[2]);
			}

			else {
				printf("ERROR parameter----WD: %s\n", NAME);
				//getch();
				preexit();
				exit(1);
			}
		}
		wp++;
	}
}
/*------------------------------------------------------------------*/
void rectdata(FILE *fi, OBS *obs){

	obs->ref[0] = 0.0;

	obs->rgb[0] = 0.7;
	obs->rgb[1] = 0.7;
	obs->rgb[2] = 0.7;

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	obs->obsname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xyz") == 0){
			fscanf_s(fi, "%lf", &obs->x);
			fscanf_s(fi, "%lf", &obs->y);
			fscanf_s(fi, "%lf", &obs->z);
		}

		else if (strcmp(NAME, "-WH") == 0){
			fscanf_s(fi, "%lf", &obs->W);
			fscanf_s(fi, "%lf", &obs->H);
		}

		else if (strcmp(NAME, "-WaWb") == 0){
			fscanf_s(fi, "%lf", &obs->Wa);
			fscanf_s(fi, "%lf", &obs->Wb);
		}

		else if (strcmp(NAME, "-ref") == 0)
			fscanf_s(fi, "%lf", &obs->ref[0]);

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &obs->rgb[0]);
			fscanf_s(fi, "%lf", &obs->rgb[1]);
			fscanf_s(fi, "%lf", &obs->rgb[2]);
		}

		else {
			printf("ERROR parameter----OBS->rect: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*------------------------------------------------------------------*/
void cubdata(FILE *fi, OBS *obs){

	int i;

	for (i = 0; i < 3; i++)
		obs->ref[i] = 0.0;

	obs->rgb[0] = 0.7;
	obs->rgb[1] = 0.7;
	obs->rgb[2] = 0.7;

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	obs->obsname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xyz") == 0){
			fscanf_s(fi, "%lf", &obs->x);
			fscanf_s(fi, "%lf", &obs->y);
			fscanf_s(fi, "%lf", &obs->z);
		}

		else if (strcmp(NAME, "-WDH") == 0){
			fscanf_s(fi, "%lf", &obs->W);
			fscanf_s(fi, "%lf", &obs->D);
			fscanf_s(fi, "%lf", &obs->H);
		}

		else if (strcmp(NAME, "-Wa") == 0)
			fscanf_s(fi, "%lf", &obs->Wa);

		else if (strcmp(NAME, "-ref0") == 0)
			fscanf_s(fi, "%lf", &obs->ref[0]);

		else if (strcmp(NAME, "-ref1") == 0)
			fscanf_s(fi, "%lf", &obs->ref[1]);

		else if (strcmp(NAME, "-ref2") == 0)
			fscanf_s(fi, "%lf", &obs->ref[2]);

		else if (strcmp(NAME, "-ref3") == 0)
			fscanf_s(fi, "%lf", &obs->ref[3]);

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &obs->rgb[0]);
			fscanf_s(fi, "%lf", &obs->rgb[1]);
			fscanf_s(fi, "%lf", &obs->rgb[2]);
		}

		else {
			printf("ERROR parameter----OBS->cube: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*-------------------------------------------------------------------*/
void tridata(FILE *fi, OBS *obs){

	obs->ref[0] = 0.0;

	obs->rgb[0] = 0.7;
	obs->rgb[1] = 0.7;
	obs->rgb[2] = 0.7;

	fscanf_s(fi, "%s", NAME, sizeof(NAME));
	obs->obsname = stralloc(NAME);

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

		if (strcmp(NAME, "-xyz") == 0){
			fscanf_s(fi, "%lf", &obs->x);
			fscanf_s(fi, "%lf", &obs->y);
			fscanf_s(fi, "%lf", &obs->z);
		}

		else if (strcmp(NAME, "-WH") == 0){
			fscanf_s(fi, "%lf", &obs->W);
			fscanf_s(fi, "%lf", &obs->H);
		}

		else if (strcmp(NAME, "-WaWb") == 0){
			fscanf_s(fi, "%lf", &obs->Wa);
			fscanf_s(fi, "%lf", &obs->Wb);
		}

		else if (strcmp(NAME, "-ref") == 0)
			fscanf_s(fi, "%lf", &obs->ref[0]);

		else if (strcmp(NAME, "-rgb") == 0){
			fscanf_s(fi, "%lf", &obs->rgb[0]);
			fscanf_s(fi, "%lf", &obs->rgb[1]);
			fscanf_s(fi, "%lf", &obs->rgb[2]);
		}

		else {
			printf("ERROR parameter----OBS->triangle: %s\n", NAME);
			//getch();
			preexit();
			exit(1);
		}
	}
}
/*-------------------------------------------------------------------*/
// 20170503 higuchi add 
void dividdata(FILE *fi, int *monten, double *DE) {

	//if (strcmp(NAME, "DIVID") != 0) {
	//	printf("error DIVID\n", NAME);
	//	exit(1);
	//}

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';') {

		if (strcmp(NAME, "DE") == 0) {
			//printf("Name=%s\n", NAME);
			fscanf_s(fi, "%lf", DE);
		}
		else if (strcmp(NAME, "MONT") == 0) {
			fscanf_s(fi, "%d", monten);
		}
		else {
			printf("ERROR parameter----DIVID: %s\n", NAME);
			preexit();
			exit(1);
		}

	}

	fscanf_s(fi, "%s", NAME, sizeof(NAME));

}
void treedata(FILE *fi, int *treen, TREE **tree){
	int i;
	int Ntree;
	TREE *tred;
	// BDPの数を数える
	Ntree = InputCount(fi, ";");
	printf("<treedata> Ntree=%d\n", Ntree);
	if (Ntree > 0)
	{
		*tree = (TREE *)malloc(sizeof (TREE)* Ntree);
		if (*tree == NULL)
			printf("<treedata> treeのメモリが確保できません\n");

		// 構造体の初期化
		tred = *tree;
		for (i = 0; i < Ntree; i++, tred++)
		{
			tred->treename = tred->treetype = NULL;
			tred->x = tred->y = tred->z = tred->W1 = tred->W2 = tred->W3 = tred->W4 = 0.;
			tred->H1 = tred->H2 = tred->H3 = 0.0;
		}
	}

	*treen = 0;
	tred = *tree;

	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != '*'){

		tred->treetype = stralloc(NAME);

		fscanf_s(fi, "%s", NAME, sizeof(NAME));
		tred->treename = stralloc(NAME);


		if (strcmp(tred->treetype, "treeA") == 0){
			while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){

				if (strcmp(NAME, "-xyz") == 0){
					fscanf_s(fi, "%lf", &tred->x);
					fscanf_s(fi, "%lf", &tred->y);
					fscanf_s(fi, "%lf", &tred->z);
				}

				else if (strcmp(NAME, "-WH1") == 0){
					fscanf_s(fi, "%lf", &tred->W1);
					fscanf_s(fi, "%lf", &tred->H1);
				}

				else if (strcmp(NAME, "-WH2") == 0){
					fscanf_s(fi, "%lf", &tred->W2);
					fscanf_s(fi, "%lf", &tred->H2);
				}

				else if (strcmp(NAME, "-WH3") == 0){
					fscanf_s(fi, "%lf", &tred->W3);
					fscanf_s(fi, "%lf", &tred->H3);
				}

				else if (strcmp(NAME, "-W4") == 0)
					fscanf_s(fi, "%lf", &tred->W4);

				else {
					printf("ERROR parameter----TREE: %s %s\n", tred->treename, NAME);
					//getch();
					preexit();
					exit(1);
				}
			}
		}

		else{
			printf("ERROR parameter----TREE: %s\n", tred->treetype);
			//getch();
			preexit();
			exit(1);
		}

		(*treen)++;
		tred++;
		/*--
		printf("treen=%d\n",*treen) ;
		--*/
	}
}

/*-------------------------*/
void polydata(FILE *fi, int *polyn, POLYGN **poly){

	int i;
	int	Npoly;
	void	matinit(double *A, int N);
	POLYGN *polyp;
	// BDPの数を数える
	Npoly = InputCount(fi, ";");
	printf("<polydata> Npoly=%d\n", Npoly);
	if (Npoly > 0)
	{
		*poly = (POLYGN *)malloc(sizeof (POLYGN)* Npoly);
		if (*poly == NULL)
			printf("<polydata> polyのメモリが確保できません\n");

		// 構造体の初期化
		polyp = *poly;
		for (i = 0; i < Npoly; i++, polyp++)
		{
			polyp->polyknd[0] = '\0';
			polyp->polyname = polyp->wallname = NULL;
			polyp->polyd = 0;
			polyp->ref = polyp->refg = polyp->grpx = 0.;
			polyp->P = NULL;
			matinit(polyp->rgb, 3);
		}
	}

	*polyn = 0;
	polyp = *poly;
	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != '*'){

		polyp->grpx = 1.0;

		polyp->rgb[0] = 0.9;
		polyp->rgb[1] = 0.9;
		polyp->rgb[2] = 0.9;

		strcpy_s(polyp->polyknd, sizeof(polyp->polyknd), NAME);
		if ((strcmp(polyp->polyknd, "RMP") != 0) && (strcmp(polyp->polyknd, "OBS") != 0)){
			printf("ERROR parameter----POLYGON: %s  <RMP> or <OBS> \n", polyp->polyknd);
			//getch();
			preexit();
			exit(1);
		}

		fscanf_s(fi, "%d", &polyp->polyd);
		polyp->P = (XYZ *)malloc(sizeof(XYZ)*polyp->polyd);

		fscanf_s(fi, "%s", NAME, sizeof(NAME));
		polyp->polyname = stralloc(NAME);
		fscanf_s(fi, "%s", NAME, sizeof(NAME));
		polyp->wallname = stralloc(NAME);

		while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){
			if (strcmp(NAME, "-xyz") == 0){
				for (i = 0; i < polyp->polyd; i++){
					fscanf_s(fi, "%lf", &polyp->P[i].X);
					fscanf_s(fi, "%lf", &polyp->P[i].Y);
					fscanf_s(fi, "%lf", &polyp->P[i].Z);
				}
			}

			else if (strcmp(NAME, "-rgb") == 0){
				fscanf_s(fi, "%lf", &polyp->rgb[0]);
				fscanf_s(fi, "%lf", &polyp->rgb[1]);
				fscanf_s(fi, "%lf", &polyp->rgb[2]);
			}

			else if (strcmp(NAME, "-ref") == 0)
				fscanf_s(fi, "%lf", &polyp->ref);

			else if (strcmp(NAME, "-refg") == 0)
				fscanf_s(fi, "%lf", &polyp->refg);

			else if (strcmp(NAME, "-grpx") == 0)
				fscanf_s(fi, "%lf", &polyp->grpx);

			else{
				printf("ERROR parameter----POLYGON: %s\n", NAME);
				//getch();
				preexit();
				exit(1);
			}
		}
		(*polyn)++;
		polyp++;
		/*--
		printf("polyn=%d\n",*polyn) ;
		--*/
	}
}
/*---------------------------------------------------------------------------*/
void bdpdata(FILE *fi, int *bdpn, BBDP **bp,EXSFS *Exsf){

	RRMP *rp;
	MADO *wp;
	sunblk *sb;
	int	Nbdp;
	BBDP *bbdp;

	// BDPの数を数える
	Nbdp = InputCount(fi, "*");
	//printf("<bdpdata> Nbdp=%d\n", Nbdp);
	if (Nbdp > 0)
	{
		*bp = (BBDP *)malloc(sizeof (BBDP)* Nbdp);
		if (*bp == NULL)
			printf("<bdpdata> bpのメモリが確保できません\n");
		
		bbdp = *bp;
		int i;
		for (i = 0; i < Nbdp; i++, bbdp++)
		{
			bbdp->bdpname = NULL;
			bbdp->exh = bbdp->exw = 0.;
			bbdp->sumRMP = bbdp->sumsblk = 0;
			bbdp->x0 = bbdp->y0 = bbdp->z0 = 0.;
			bbdp->Wa = bbdp->Wb = 0.;
			bbdp->SBLK = NULL;
			bbdp->RMP = NULL;
			bbdp->exsfname = NULL;
		}
	}

	bbdp = *bp;
	//sb = bbdp->SBLK;
	//rp = bbdp->RMP;
	//wp = rp->WD;

	*bdpn = 0;

	// ex) BDP south -xyz 0.0 0.0 0.0 -WA 0.0 -WB 90.0 -WH 8.645 5.400 ;
	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != '*'){
		//printf("<bdpdata> 1 NAME=%s", NAME);
		if (strcmp(NAME, "BDP") != 0){
			printf("error BDP\n");
			exit(1);
		}
		//printf("<bdpdata> 2 NAME=%s", NAME);

		fscanf_s(fi, "%s", NAME, sizeof(NAME));
		bbdp->bdpname = stralloc(NAME);
		//printf("<bdpdata> 3 NAME=%s", NAME);

		while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != ';'){
			//printf("<bdpdata> 4 NAME=%s", NAME);

			if (strcmp(NAME, "-xyz") == 0){
				fscanf_s(fi, "%lf", &bbdp->x0);
				fscanf_s(fi, "%lf", &bbdp->y0);
				fscanf_s(fi, "%lf", &bbdp->z0);
			}

			else if (strcmp(NAME, "-WA") == 0)
				fscanf_s(fi, "%lf", &bbdp->Wa);

			else if (strcmp(NAME, "-WB") == 0)
				fscanf_s(fi, "%lf", &bbdp->Wb);

			else if (strcmp(NAME, "-WH") == 0){
				fscanf_s(fi, "%lf", &bbdp->exw);
				fscanf_s(fi, "%lf", &bbdp->exh);
			}
			// Satoh修正（2018/1/23）
			else if (strcmp(NAME, "-exs") == 0)
			{
				fscanf_s(fi, "%s", NAME, sizeof(NAME));
				bbdp->exsfname = stralloc(NAME);
				//外表面の検索
				EXSF *Exs;
				int id, i;
				id = 0;
				Exs = Exsf->Exs;
				for (i = 0; i < Exsf->Nexs; i++, Exs++)
				{
					if (strcmp(bbdp->exsfname, Exs->name) == 0)
					{
						bbdp->Wa = Exs->Wa;
						bbdp->Wb = Exs->Wb;
						id = 1;
						break;
					}
				}
				if (id == 0)
					printf("BDP<%s> %s is not found in EXSRF\n", bbdp->bdpname, bbdp->exsfname);
			}
			else {
				printf("ERROR parameter----BDP %s\n", NAME);
				//getch();
				preexit();
				exit(1);
			}
		}

		// SBLKの個数を数えてメモリを確保
		int Nsblk;
		int		SBLKCount(FILE *fi);
		Nsblk = SBLKCount(fi);
		//printf("<bdpdata> Nsblk=%d\n", Nsblk);
		// メモリの確保
		if (Nsblk > 0)
		{
			bbdp->SBLK = (sunblk *)malloc(sizeof(sunblk)*Nsblk);
			if (bbdp->SBLK == NULL)
				printf("<bdpdata> SBLKのメモリが確保できません\n");

			sb = bbdp->SBLK;
			int i;
			for (i = 0; i < Nsblk; i++, sb++)
			{
				sb->D = sb->H = sb->h = sb->ref = sb->W = sb->WA = sb->x = sb->y = 0.0;
				sb->sbfname = sb->snbname = NULL;
				matinit(sb->rgb, 3);
			}
		}

		// RMPの個数を数えてメモリを確保
		int Nrmp;
		int		RMPCount(FILE *fi);
		Nrmp = RMPCount(fi);
		//printf("<bdpdata> Nrmp=%d\n", Nrmp);
		// メモリの確保
		if (Nrmp > 0)
		{
			bbdp->RMP = (RRMP *)malloc(sizeof(RRMP)*Nrmp);
			if (bbdp->RMP == NULL)
				printf("<bdpdata> RMPのメモリが確保できません\n");

			rp = bbdp->RMP;
			int i;
			for (i = 0; i < Nrmp; i++, rp++)
			{
				rp->rmpname = rp->wallname = NULL;
				rp->sumWD = 0;
				rp->ref = rp->xb0 = rp->yb0 = rp->Rw = rp->Rh = rp->grpx = 0.0;
				matinit(rp->rgb, 3);
				rp->WD = NULL;
			}
		}

		sb = bbdp->SBLK;
		rp = bbdp->RMP;
		if (rp != NULL)
			wp = rp->WD;
		while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != '*'){

			if (strcmp(NAME, "SBLK") == 0){
				sb->ref = 0.0;
				fscanf_s(fi, "%s", NAME, sizeof(NAME));
				sb->sbfname = stralloc(NAME);

				// ex) SBLK HISASI 1Fhisa -xy 0 2.7 -DW 0.5 8.645 -a 90 ;
				if (strcmp(sb->sbfname, "HISASI") == 0)
					HISASHI(fi, sb);

				else if (strcmp(sb->sbfname, "BARUKONI") == 0)
					BARUKO(fi, sb);

				else if (strcmp(sb->sbfname, "SODEKABE") == 0)
					SODEK(fi, sb);

				// ex) SBLK MADOHIYOKE LDsudare-s1 -xy 0.7525 2.05 -DHW 0.1 2.0 1.8 -rgb 0.76 0.34 0.11 ;
				else  if (strcmp(sb->sbfname, "MADOHIYOKE") == 0)
					SCREEN(fi, sb);

				else{
					printf("ERROR----\nhiyoke no syurui <HISASI> or <BARUKONI> or <SODEKABE> or <MADOHIYOKE> : %s \n", sb->sbfname);
					//getch();
					preexit();
					exit(1);
				}

				sb++;
				bbdp->sumsblk++;
			}

			else if (strcmp(NAME, "RMP") == 0){
				// WDの数を数えてメモリを確保
				int Nwd;
				int		WDCount(FILE *fi);
				Nwd = WDCount(fi);
				//printf("<bdpdata> Nwd=%d\n", Nwd);
				if (Nwd > 0)
				{
					rp->WD = (MADO *)malloc(sizeof(MADO)*Nwd);
					if (rp->WD == NULL)
						printf("<bdpdata> rp->WDのメモリが確保できません\n");
					wp = rp->WD;
					int i;
					for (i = 0; i < Nwd; i++, wp++)
					{
						wp->winname = NULL;
						matinit(wp->rgb, 3);
						wp->grpx = wp->ref = wp->Wh = wp->xr = wp->yr = 0.0;
					}
				}
				rp->ref = 0.0;
				wp = rp->WD;
				bbdp->sumRMP++;
				rmpdata(fi, rp, wp);

				rp++;
				wp = rp->WD;
			}
			else{
				printf("ERROR----<SBLK> or <RMP> : %s \n", NAME);
				//getch();
				preexit();
				exit(1);
			}
		}

		(*bdpn)++;
		bbdp++;

		//rp = bbdp->RMP;
		//if (rp != NULL)
		//	wp = rp->WD;
		//sb = bbdp->SBLK;

	}
}
/*--------------------------------------------------------------------------*/
void obsdata(FILE *fi, int *obsn, OBS **obs)
{

	int i;
	int	Nobs;
	OBS	*obsp;

	// BDPの数を数える
	Nobs = InputCount(fi, ";");
	//printf("<obsdata> Nobs=%d\n", Nobs);
	if (Nobs > 0)
	{
		*obs = (OBS *)malloc(sizeof (OBS)* Nobs);
		if (*obs == NULL)
			printf("<obsdata> obsのメモリが確保できません\n");

		obsp = *obs;
		for (i = 0; i < Nobs; i++, obsp++)
		{
			obsp->fname = obsp->obsname = NULL;
			obsp->x = obsp->y = obsp->z = obsp->H = obsp->D = obsp->W = obsp->Wa = obsp->Wb = 0.0;
			matinit(obsp->ref, 4);
			matinit(obsp->rgb, 3);
		}
	}

	*obsn = 0;
	obsp = *obs;
	while (fscanf_s(fi, "%s", NAME, sizeof(NAME)), NAME[0] != '*'){
		// OBS名称
		obsp->fname = stralloc(NAME);

		for (i = 0; i < 4; i++)
			obsp->ref[i] = 0.0;

		if (strcmp(obsp->fname, "rect") == 0)
			rectdata(fi, obsp);

		else if (strcmp(obsp->fname, "cube") == 0)
			cubdata(fi, obsp);

		else if (strcmp(obsp->fname, "r_tri") == 0)
			tridata(fi, obsp);

		else if (strcmp(obsp->fname, "i_tri") == 0)
			tridata(fi, obsp);

		else {
			printf("ERROR parameter----OBS : %s\n", obsp->fname);
			//getch();
			preexit();
			exit(1);
		}

		obsp++;
		(*obsn)++;
	}
}

int		InputCount(FILE *fi, char *key)
{
	int		N;
	long	ad;
	char	s[SCHAR]; // ss[SCHAR] ;

	N = 0;
	ad = ftell(fi);

	while (fscanf_s(fi, "%s", s, sizeof(s)) != EOF && *s != '*')
	{
		N++;

		while (fscanf_s(fi, "%s", s, sizeof(s)) != EOF)
		{
			if (strcmp(s, key) == 0)
				break;
		}
	}

	fseek(fi, ad, SEEK_SET);
	return (N);
}

int		SBLKCount(FILE *fi)
{
	int		N;
	long	ad;
	char	s[SCHAR]; // ss[SCHAR] ;

	N = 0;
	ad = ftell(fi);

	while (fscanf_s(fi, "%s", s, sizeof(s)) != EOF && *s != '*')
	{
		if (strcmp(s, "SBLK") == 0)
			N++;
	}

	fseek(fi, ad, SEEK_SET);
	return (N);
}

int		RMPCount(FILE *fi)
{
	int		N;
	long	ad;
	char	s[SCHAR]; // ss[SCHAR] ;

	N = 0;
	ad = ftell(fi);

	while (fscanf_s(fi, "%s", s, sizeof(s)) != EOF && *s != '*')
	{
		if (strcmp(s, "RMP") == 0)
			N++;
	}

	fseek(fi, ad, SEEK_SET);
	return (N);
}

int		WDCount(FILE *fi)
{
	int		N;
	long	ad;
	char	s[SCHAR]; // ss[SCHAR] ;

	N = 0;
	ad = ftell(fi);

	int Flg;
	Flg = 0;
	while (fscanf_s(fi, "%s", s, sizeof(s)) != EOF)
	{
		if (strcmp(s, "WD") == 0)
			N++;

		if (strcmp(s, ";") == 0)
		{
			if (Flg == 1) // ;が2つ連続したら
				break;
			else
				Flg = 1;
		}
		else
			Flg = 0;
	}

	fseek(fi, ad, SEEK_SET);
	return (N);
}

int	OPcount(int Nbdp, BBDP *Bdp, int Npoly, POLYGN *poly)
{
	// 初期化
	int Nop = 0;

	int i;
	for (i = 0; i < Nbdp; i++, Bdp++)
	{
		Nop += Bdp->sumRMP;
		RRMP *RMP;
		RMP = Bdp->RMP;
		int j;
		for (j = 0; j < Bdp->sumRMP; j++, RMP++)
			Nop += RMP->sumWD;
	}
	
	for (i = 0; i < Npoly; i++, poly++)
	{
		if (strcmp(poly->polyknd, "RMP") == 0)
			Nop++;
	}

	return(Nop);
}

int LPcount(int Nbdp, BBDP *Bdp, int Nobs, OBS *Obs, int Ntree, int Npoly, POLYGN *poly)
{
	// 初期化
	int Nlp = 0;
	int i;
	for (i = 0; i < Nbdp; i++, Bdp++)
	{
		int j;
		sunblk *snbk;
		snbk = Bdp->SBLK;
		for (j = 0; j < Bdp->sumsblk; j++, snbk++)
		{
			if (strcmp(snbk->sbfname, "BARUKONI") == 0)
				Nlp += 5;
			else
				Nlp++;
		}
	}

	for (i = 0; i < Nobs; i++, Obs++)
	{
		if (strcmp(Obs->fname, "cube") == 0)
			Nlp += 4;
		else
			Nlp++;
	}

	// 樹木用
	Nlp += Ntree * 20;

	// ポリゴン
	for (i = 0; i < Npoly; i++, poly++)
	{
		if (strcmp(poly->polyknd, "RMP") == 0 || strcmp(poly->polyknd, "OBS") == 0)
			Nlp++;
	}

	return(Nlp);
}