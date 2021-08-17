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

/*

						入力データの計算用構造体への変換
						FILE=COORDNT.c
						Create Date=1999.6.7
						*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>		// satoh追加 2008/11/8
#include "esize.h"
#include "fesy.h"
#include "MODEL.h"
//#include "exs.h"
#include "FUNC.h" 
#include "fnlib.h"
#define test 0

void	preexit(void);

void LP_COORDNT(int *lpn, int bdpn, int obsn, int treen, int polyn,
	POLYGN *poly, TREE *tree, OBS *obs, BBDP *BDP, P_MENN *lp)
{
	char name[SCHAR];
	int i, j, k, p;
	double    sWa, cWa, sWb, cWb, a, b, c, chWA, shWA;
	double    csWA, ssWA, csWb, ssWb;
	double    caWb, saWa, caWa, saWb;
	double    cbWa, sbWa;
	char	*stralloc(char *s);

	k = 0;
	for (i = 0; i < bdpn; i++){
		if (test == 1)
			printf("BDP=%s\n", BDP[i].bdpname);
		sWb = sin(BDP[i].Wb*M_rad);
		cWb = cos(BDP[i].Wb*M_rad);
		sWa = sin(-BDP[i].Wa*M_rad);
		cWa = cos(-BDP[i].Wa*M_rad);

		if (BDP[i].sumsblk != 0){
			for (j = 0; j < BDP[i].sumsblk; j++){

				if (strcmp(BDP[i].SBLK[j].sbfname, "HISASI") == 0){
					a = 0.0; b = 0.0; c = 0.0; chWA = 0.0; shWA = 0.0;

					a = BDP[i].x0 + BDP[i].SBLK[j].x*cWa - BDP[i].SBLK[j].y*cWb*sWa;
					b = BDP[i].y0 + BDP[i].SBLK[j].x*sWa + BDP[i].SBLK[j].y*cWb*cWa;
					c = BDP[i].z0 + BDP[i].SBLK[j].y*sWb;

					chWA = cos((BDP[i].SBLK[j].WA - BDP[i].Wb)*M_rad);
					shWA = sin((BDP[i].SBLK[j].WA - BDP[i].Wb)*M_rad);

					strcpy_s(lp[k].opname, OPNAME_SZ, BDP[i].SBLK[j].snbname);
					lp[k].wa = BDP[i].Wa;
					lp[k].wb = BDP[i].Wb + (180.0 - BDP[i].SBLK[j].WA);
					if (lp[k].wb > 180.0)
						lp[k].wb = 360.0 - lp[k].wb;

					lp[k].wd = 0;
					lp[k].ref = 0.0;   /*--付設障害物からの反射率を０としている-*/
					for (p = 0; p < 366; p++){
						lp[k].shad[p] = 1;
					}

					lp[k].sbflg = 1;

					lp[k].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k].polyd = 4;
					lp[k].P = (XYZ *)malloc(4 * sizeof(XYZ));

					lp[k].P[0].X = a;
					lp[k].P[0].Y = b;
					lp[k].P[0].Z = c;

					lp[k].P[1].X = a + BDP[i].SBLK[j].D*chWA*sWa;
					lp[k].P[1].Y = b - BDP[i].SBLK[j].D*chWA*cWa;
					lp[k].P[1].Z = c + BDP[i].SBLK[j].D*shWA;

					lp[k].P[2].X = lp[k].P[1].X + BDP[i].SBLK[j].W*cWa;
					lp[k].P[2].Y = lp[k].P[1].Y + BDP[i].SBLK[j].W*sWa;
					lp[k].P[2].Z = lp[k].P[1].Z;

					lp[k].P[3].X = a + BDP[i].SBLK[j].W*cWa;
					lp[k].P[3].Y = b + BDP[i].SBLK[j].W*sWa;
					lp[k].P[3].Z = c;

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k].e.Z = cos(lp[k].wb*M_rad);
					lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
					lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
					CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

					//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

					k = k + 1;
				}

				/*---------------------------------------*/
				else if (strcmp(BDP[i].SBLK[j].sbfname, "BARUKONI") == 0){

					a = 0.0; b = 0.0; c = 0.0; chWA = 0.0; shWA = 0.0;

					a = BDP[i].x0 + BDP[i].SBLK[j].x*cWa - BDP[i].SBLK[j].y*cWb*sWa;
					b = BDP[i].y0 + BDP[i].SBLK[j].x*sWa + BDP[i].SBLK[j].y*cWb*cWa;
					c = BDP[i].z0 + BDP[i].SBLK[j].y*sWb;

					chWA = cos((90.0 - BDP[i].Wb)*M_rad);
					shWA = sin((90.0 - BDP[i].Wb)*M_rad);

					strcpy_s(lp[k].opname, OPNAME_SZ, BDP[i].SBLK[j].snbname);

					lp[k].wa = BDP[i].Wa;
					lp[k].wb = BDP[i].Wb + 90.0;
					if (lp[k].wb > 180.0)
						lp[k].wb = -(360.0 - lp[k].wb);

					lp[k].ref = 0.0;
					lp[k].sbflg = 1;

					for (p = 0; p < 366; p++)
						lp[k].shad[p] = 1;

					lp[k].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k].polyd = 4;
					lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k].wd = 0;

					lp[k].P[0].X = a;
					lp[k].P[0].Y = b;
					lp[k].P[0].Z = c;

					lp[k].P[1].X = a + BDP[i].SBLK[j].D*chWA*sWa;
					lp[k].P[1].Y = b - BDP[i].SBLK[j].D*chWA*cWa;
					lp[k].P[1].Z = c + BDP[i].SBLK[j].D*shWA;

					lp[k].P[2].X = lp[k].P[1].X + BDP[i].SBLK[j].W*cWa;
					lp[k].P[2].Y = lp[k].P[1].Y + BDP[i].SBLK[j].W*sWa;
					lp[k].P[2].Z = lp[k].P[1].Z;

					lp[k].P[3].X = a + BDP[i].SBLK[j].W*cWa;
					lp[k].P[3].Y = b + BDP[i].SBLK[j].W*sWa;
					lp[k].P[3].Z = c;

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k].e.Z = cos(lp[k].wb*M_rad);
					lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
					lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
					CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

					//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

					strcpy_s(name, sizeof(name), "2");
					strcat_s(name, sizeof(name), lp[k].opname);
					strcpy_s(lp[k + 1].opname, OPNAME_SZ, name);

					lp[k + 1].wa = BDP[i].Wa - 90.0;
					if (lp[k + 1].wa <= -180.0)
						lp[k + 1].wa = 360.0 + lp[k + 1].wa;
					lp[k + 1].wb = 90.0;
					lp[k + 1].ref = 0.0;
					lp[k + 1].sbflg = 1;

					for (p = 0; p < 366; p++)
						lp[k + 1].shad[p] = 1;

					lp[k + 1].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k + 1].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k + 1].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k + 1].polyd = 4;
					lp[k + 1].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k + 1].wd = 0;

					lp[k + 1].P[0] = lp[k].P[0];
					lp[k + 1].P[1] = lp[k].P[1];
					lp[k + 1].P[2].X = lp[k + 1].P[1].X + BDP[i].SBLK[j].H*cWb*sWa;
					lp[k + 1].P[2].Y = lp[k + 1].P[1].Y - BDP[i].SBLK[j].H*cWb*cWa;
					lp[k + 1].P[2].Z = lp[k + 1].P[1].Z - BDP[i].SBLK[j].H*sWb;
					lp[k + 1].P[3].X = a + BDP[i].SBLK[j].H*cWb*sWa;
					lp[k + 1].P[3].Y = b - BDP[i].SBLK[j].H*cWb*cWa;
					lp[k + 1].P[3].Z = c - BDP[i].SBLK[j].H*sWb;

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k + 1].e.Z = cos(lp[k + 1].wb*M_rad);
					lp[k + 1].e.Y = -sin(lp[k + 1].wb*M_rad)*cos(lp[k + 1].wa*M_rad);
					lp[k + 1].e.X = -sin(lp[k + 1].wb*M_rad)*sin(lp[k + 1].wa*M_rad);
					CAT(&lp[k + 1].e.X, &lp[k + 1].e.Y, &lp[k + 1].e.Z);

					//HOUSEN2(&lp[k+1].P[0],&lp[k+1].P[1],&lp[k+1].P[2],&lp[k+1].e) ;

					strcpy_s(name, sizeof(name), "3");
					strcat_s(name, sizeof(name),  lp[k].opname);
					strcpy_s(lp[k + 2].opname, OPNAME_SZ, name);

					lp[k + 2].wa = BDP[i].Wa;
					lp[k + 2].wb = BDP[i].Wb - 90.0;
					lp[k + 2].ref = BDP[i].SBLK[j].ref;
					lp[k + 2].sbflg = 1;


					for (p = 0; p < 366; p++)
						lp[k + 2].shad[p] = 1;

					lp[k + 2].wd = 0;

					lp[k + 2].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k + 2].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k + 2].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k + 2].polyd = 4;
					lp[k + 2].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k + 2].P[0] = lp[k + 1].P[3];
					lp[k + 2].P[1] = lp[k + 1].P[2];
					lp[k + 2].P[2].X = lp[k + 2].P[1].X + BDP[i].SBLK[j].W*cWa;
					lp[k + 2].P[2].Y = lp[k + 2].P[1].Y + BDP[i].SBLK[j].W*sWa;
					lp[k + 2].P[2].Z = lp[k + 2].P[1].Z;
					lp[k + 2].P[3].X = lp[k + 2].P[0].X + BDP[i].SBLK[j].W*cWa;
					lp[k + 2].P[3].Y = lp[k + 2].P[0].Y + BDP[i].SBLK[j].W*sWa;
					lp[k + 2].P[3].Z = lp[k + 2].P[0].Z;

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k + 2].e.Z = cos(lp[k + 2].wb*M_rad);
					lp[k + 2].e.Y = -sin(lp[k + 2].wb*M_rad)*cos(lp[k + 2].wa*M_rad);
					lp[k + 2].e.X = -sin(lp[k + 2].wb*M_rad)*sin(lp[k + 2].wa*M_rad);
					CAT(&lp[k + 2].e.X, &lp[k + 2].e.Y, &lp[k + 2].e.Z);

					//HOUSEN2(&lp[k+2].P[0],&lp[k+2].P[1],&lp[k+2].P[2],&lp[k+2].e) ;

					strcpy_s(name, sizeof(name), "4");
					strcat_s(name, sizeof(name),  lp[k].opname);
					strcpy_s(lp[k + 3].opname, OPNAME_SZ, name);

					lp[k + 3].wa = BDP[i].Wa + 90.0;
					if (lp[k + 3].wa >= 180.0)
						lp[k + 3].wa = lp[k + 3].wa - 360.0;
					lp[k + 3].wb = 90.0;
					lp[k + 3].ref = 0.0;
					lp[k + 3].sbflg = 1;


					for (p = 0; p < 366; p++)
						lp[k + 3].shad[p] = 1;

					lp[k + 3].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k + 3].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k + 3].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k + 3].polyd = 4;
					lp[k + 3].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k + 3].wd = 0;

					lp[k + 3].P[0] = lp[k].P[3];
					lp[k + 3].P[1] = lp[k].P[2];
					lp[k + 3].P[2] = lp[k + 2].P[2];
					lp[k + 3].P[3] = lp[k + 2].P[3];

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k + 3].e.Z = cos(lp[k + 3].wb*M_rad);
					lp[k + 3].e.Y = -sin(lp[k + 3].wb*M_rad)*cos(lp[k + 3].wa*M_rad);
					lp[k + 3].e.X = -sin(lp[k + 3].wb*M_rad)*sin(lp[k + 3].wa*M_rad);
					CAT(&lp[k + 3].e.X, &lp[k + 3].e.Y, &lp[k + 3].e.Z);

					//HOUSEN2(&lp[k+3].P[0],&lp[k+3].P[1],&lp[k+3].P[2],&lp[k+3].e) ;

					strcpy_s(name, sizeof(name), "5");
					strcat_s(name, sizeof(name),  lp[k].opname);
					strcpy_s(lp[k + 4].opname, OPNAME_SZ, name);

					lp[k + 4].wa = BDP[i].Wa - 180;
					if (lp[k + 4].wa > 180)
						lp[k + 4].wa = lp[k + 4].wa - 360;
					else if (lp[k + 4].wa < 180)
						lp[k + 4].wa = lp[k + 4].wa + 360;

					lp[k + 4].wb = BDP[i].Wb;
					lp[k + 4].ref = 0.0;
					lp[k + 4].sbflg = 1;


					for (p = 0; p < 366; p++)
						lp[k + 4].shad[p] = 1;

					lp[k + 4].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k + 4].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k + 4].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k + 4].polyd = 4;
					lp[k + 4].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k + 4].wd = 0;

					lp[k + 4].P[0] = lp[k + 1].P[2];
					lp[k + 4].P[1].X = lp[k + 4].P[0].X - BDP[i].SBLK[j].h*sWa*cWb;
					lp[k + 4].P[1].Y = lp[k + 4].P[0].Y + BDP[i].SBLK[j].h*cWa*cWb;
					lp[k + 4].P[1].Z = lp[k + 4].P[0].Z + BDP[i].SBLK[j].h*sWb;
					lp[k + 4].P[2].X = lp[k + 4].P[1].X + BDP[i].SBLK[j].W*cWa;
					lp[k + 4].P[2].Y = lp[k + 4].P[1].Y + BDP[i].SBLK[j].W*sWa;
					lp[k + 4].P[2].Z = lp[k + 4].P[1].Z;
					lp[k + 4].P[3] = lp[k + 3].P[2];

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k + 4].e.Z = cos(lp[k + 4].wb*M_rad);
					lp[k + 4].e.Y = -sin(lp[k + 4].wb*M_rad)*cos(lp[k + 4].wa*M_rad);
					lp[k + 4].e.X = -sin(lp[k + 4].wb*M_rad)*sin(lp[k + 4].wa*M_rad);
					CAT(&lp[k + 4].e.X, &lp[k + 4].e.Y, &lp[k + 4].e.Z);

					//HOUSEN2(&lp[k+4].P[0],&lp[k+4].P[1],&lp[k+4].P[2],&lp[k+4].e) ;
					k = k + 5;
				}

				/*---------------------------------------------------*/
				else if (strcmp(BDP[i].SBLK[j].sbfname, "SODEKABE") == 0){

					a = 0.0; b = 0.0; c = 0.0; csWA = 0.0; ssWA = 0.0; csWb = 0.0; ssWb = 0.0;

					a = BDP[i].x0 + BDP[i].SBLK[j].x*cWa - BDP[i].SBLK[j].y*cWb*sWa;
					b = BDP[i].y0 + BDP[i].SBLK[j].x*sWa + BDP[i].SBLK[j].y*cWb*cWa;
					c = BDP[i].z0 + BDP[i].SBLK[j].y*sWb;
					csWA = cos((-BDP[i].Wa - BDP[i].SBLK[j].WA)*M_rad);
					ssWA = sin((-BDP[i].Wa - BDP[i].SBLK[j].WA)*M_rad);
					csWb = cos((90.0 - BDP[i].Wb)*M_rad);
					ssWb = sin((90.0 - BDP[i].Wb)*M_rad);

					strcpy_s(lp[k].opname, OPNAME_SZ, BDP[i].SBLK[j].snbname);

					lp[k].wa = BDP[i].Wa - 90.0;
					if (lp[k].wa <= -180.0)
						lp[k].wa = 360.0 + lp[k].wa;
					lp[k].wb = 90.0;
					lp[k].ref = 0.0;
					lp[k].sbflg = 1;

					for (p = 0; p < 366; p++)
						lp[k].shad[p] = 1;

					lp[k].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k].polyd = 4;
					lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k].wd = 0;

					lp[k].P[0].X = a;
					lp[k].P[0].Y = b;
					lp[k].P[0].Z = c;
					lp[k].P[1].X = a + BDP[i].SBLK[j].D*csWb*csWA;
					lp[k].P[1].Y = b + BDP[i].SBLK[j].D*csWb*ssWA;
					lp[k].P[1].Z = c + BDP[i].SBLK[j].D*ssWb;
					lp[k].P[2].X = lp[k].P[1].X + BDP[i].SBLK[j].H*cWb*sWa;
					lp[k].P[2].Y = lp[k].P[1].Y - BDP[i].SBLK[j].H*cWb*cWa;
					lp[k].P[2].Z = lp[k].P[1].Z - BDP[i].SBLK[j].H*sWb;
					lp[k].P[3].X = a + BDP[i].SBLK[j].H*cWb*sWa;
					lp[k].P[3].Y = b - BDP[i].SBLK[j].H*cWb*cWa;
					lp[k].P[3].Z = c - BDP[i].SBLK[j].H*sWb;

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k].e.Z = cos(lp[k].wb*M_rad);
					lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
					lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
					CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

					//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

					k = k + 1;

				}
				/*------------------------------------------------------*/
				else if (strcmp(BDP[i].SBLK[j].sbfname, "MADOHIYOKE") == 0){

					a = 0.0; b = 0.0; c = 0.0; chWA = 0.0; shWA = 0.0;

					a = BDP[i].x0 + BDP[i].SBLK[j].x*cWa - BDP[i].SBLK[j].y*cWb*sWa;
					b = BDP[i].y0 + BDP[i].SBLK[j].x*sWa + BDP[i].SBLK[j].y*cWb*cWa;
					c = BDP[i].z0 + BDP[i].SBLK[j].y*sWb;
					chWA = cos((90.0 - BDP[i].Wb)*M_rad);
					shWA = sin((90.0 - BDP[i].Wb)*M_rad);

					strcpy_s(lp[k].opname, OPNAME_SZ, BDP[i].SBLK[j].snbname);

					lp[k].wa = BDP[i].Wa - 180;
					if (lp[k].wa > 180)
						lp[k].wa = lp[k].wa - 360;
					else if (lp[k].wa < 180)
						lp[k].wa = lp[k].wa + 360;

					lp[k].wb = BDP[i].Wb;
					lp[k].ref = 0.0;
					lp[k].sbflg = 1;

					for (p = 0; p < 366; p++)
						lp[k].shad[p] = 1;

					lp[k].rgb[0] = BDP[i].SBLK[j].rgb[0];
					lp[k].rgb[1] = BDP[i].SBLK[j].rgb[1];
					lp[k].rgb[2] = BDP[i].SBLK[j].rgb[2];
					lp[k].polyd = 4;
					lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					lp[k].wd = 0;

					/*--
					lp[k].P[1].X=a+BDP[i].SBLK[j].D*chWA*sWa ;
					lp[k].P[1].Y=b-BDP[i].SBLK[j].D*chWA*cWa ;
					lp[k].P[1].Z=c+BDP[i].SBLK[j].D*shWA ;
					lp[k].P[0].X=lp[k].P[1].X+BDP[i].SBLK[j].H*cWb*sWa ;
					lp[k].P[0].Y=lp[k].P[1].Y-BDP[i].SBLK[j].H*cWb*cWa ;
					lp[k].P[0].Z=lp[k].P[1].Z-BDP[i].SBLK[j].H*sWb ;
					lp[k].P[2].X=lp[k].P[1].X+BDP[i].SBLK[j].W*cWa ;
					lp[k].P[2].Y=lp[k].P[1].Y+BDP[i].SBLK[j].W*sWa ;
					lp[k].P[2].Z=lp[k].P[1].Z ;
					lp[k].P[3].X=lp[k].P[0].X+BDP[i].SBLK[j].W*cWa ;
					lp[k].P[3].Y=lp[k].P[0].Y+BDP[i].SBLK[j].W*sWa ;
					lp[k].P[3].Z=lp[k].P[0].Z ;
					--*/

					lp[k].P[0].X = a + BDP[i].SBLK[j].D*chWA*sWa;
					lp[k].P[0].Y = b - BDP[i].SBLK[j].D*chWA*cWa;
					lp[k].P[0].Z = c + BDP[i].SBLK[j].H*shWA;
					lp[k].P[1].X = lp[k].P[0].X + BDP[i].SBLK[j].H*cWb*sWa;
					lp[k].P[1].Y = lp[k].P[0].Y - BDP[i].SBLK[j].H*cWb*cWa;
					lp[k].P[1].Z = lp[k].P[0].Z - BDP[i].SBLK[j].H*sWb;
					lp[k].P[3].X = lp[k].P[0].X + BDP[i].SBLK[j].W*cWa;
					lp[k].P[3].Y = lp[k].P[0].Y + BDP[i].SBLK[j].W*sWa;
					lp[k].P[3].Z = lp[k].P[0].Z;
					lp[k].P[2].X = lp[k].P[1].X + BDP[i].SBLK[j].W*cWa;
					lp[k].P[2].Y = lp[k].P[1].Y + BDP[i].SBLK[j].W*sWa;
					lp[k].P[2].Z = lp[k].P[1].Z;

					/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
					lp[k].e.Z = cos(lp[k].wb*M_rad);
					lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
					lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
					CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

					//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;
					k = k + 1;

				}
			}
		}

	}
#if test
	printf("k=%d\n",k) ;
#endif
	/*-----------------------------------------------------*/

	for (i = 0; i < obsn; i++){
		if (strcmp(obs[i].fname, "rect") == 0){
			caWb = 0.0; saWa = 0.0; caWa = 0.0; saWb = 0.0;

			caWb = cos(obs[i].Wb*M_rad);
			saWa = sin(-obs[i].Wa*M_rad);
			saWb = sin(obs[i].Wb*M_rad);
			caWa = cos(-obs[i].Wa*M_rad);

			strcpy_s(lp[k].opname, OPNAME_SZ, obs[i].obsname);

			lp[k].wa = obs[i].Wa;
			lp[k].wb = obs[i].Wb;
			lp[k].ref = obs[i].ref[0];
			for (p = 0; p < 366; p++)
				lp[k].shad[p] = 1;


			lp[k].rgb[0] = obs[i].rgb[0];
			lp[k].rgb[1] = obs[i].rgb[1];
			lp[k].rgb[2] = obs[i].rgb[2];
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].wd = 0;
			lp[k].sbflg = 0;

			lp[k].P[0].X = obs[i].x;
			lp[k].P[0].Y = obs[i].y;
			lp[k].P[0].Z = obs[i].z;
			lp[k].P[1].X = obs[i].x - obs[i].H*caWb*saWa;
			lp[k].P[1].Y = obs[i].y + obs[i].H*caWb*caWa;
			lp[k].P[1].Z = obs[i].z + obs[i].H*saWb;
			lp[k].P[2].X = obs[i].x + obs[i].W*caWa - obs[i].H*caWb*saWa;
			lp[k].P[2].Y = obs[i].y + obs[i].H*caWb*caWa + obs[i].W*saWa;
			lp[k].P[2].Z = obs[i].z + obs[i].H*saWb;
			lp[k].P[3].X = obs[i].x + obs[i].W*caWa;
			lp[k].P[3].Y = obs[i].y + obs[i].W*saWa;
			lp[k].P[3].Z = obs[i].z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;
		}
		else if (strcmp(obs[i].fname, "cube") == 0){
			cbWa = 0.0; sbWa = 0.0;

			cbWa = cos(-obs[i].Wa*M_rad);
			sbWa = sin(-obs[i].Wa*M_rad);

			strcpy_s(lp[k].opname, OPNAME_SZ, obs[i].obsname);

			lp[k].wa = obs[i].Wa;
			lp[k].wb = 90.0;
			lp[k].ref = obs[i].ref[0];
			for (p = 0; p < 366; p++)
				lp[k].shad[p] = 1;

			lp[k].rgb[0] = obs[i].rgb[0];
			lp[k].rgb[1] = obs[i].rgb[1];
			lp[k].rgb[2] = obs[i].rgb[2];
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].wd = 0;
			lp[k].sbflg = 0;

			lp[k].P[0].X = obs[i].x;
			lp[k].P[0].Y = obs[i].y;
			lp[k].P[0].Z = obs[i].z;
			lp[k].P[1].X = obs[i].x;
			lp[k].P[1].Y = obs[i].y;
			lp[k].P[1].Z = obs[i].z + obs[i].H;
			lp[k].P[2].X = obs[i].x + obs[i].W*cbWa;
			lp[k].P[2].Y = obs[i].y + obs[i].W*sbWa;
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = lp[k].P[2].X;
			lp[k].P[3].Y = lp[k].P[2].Y;
			lp[k].P[3].Z = lp[k].P[0].Z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			strcpy_s(name, sizeof(name), "2");
			strcat_s(name, sizeof(name),  lp[k].opname);
			strcpy_s(lp[k + 1].opname, OPNAME_SZ, name);

			lp[k + 1].wa = obs[i].Wa - 90.0;
			if (lp[k + 1].wa <= -180.0)
				lp[k + 1].wa = 360.0 + lp[k + 1].wa;
			lp[k + 1].wb = 90.0;
			lp[k + 1].ref = obs[i].ref[1];
			for (p = 0; p < 366; p++)
				lp[k + 1].shad[p] = 1;

			lp[k + 1].rgb[0] = obs[i].rgb[0];
			lp[k + 1].rgb[1] = obs[i].rgb[1];
			lp[k + 1].rgb[2] = obs[i].rgb[2];
			lp[k + 1].polyd = 4;
			lp[k + 1].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k + 1].wd = 0;
			lp[k + 1].sbflg = 0;

			lp[k + 1].P[0] = lp[k].P[3];
			lp[k + 1].P[1] = lp[k].P[2];
			lp[k + 1].P[2].X = lp[k + 1].P[1].X - obs[i].D*sbWa;
			lp[k + 1].P[2].Y = lp[k + 1].P[1].Y + obs[i].D*cbWa;
			lp[k + 1].P[2].Z = lp[k].P[2].Z;
			lp[k + 1].P[3].X = lp[k + 1].P[2].X;
			lp[k + 1].P[3].Y = lp[k + 1].P[2].Y;
			lp[k + 1].P[3].Z = lp[k + 1].P[0].Z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k + 1].e.Z = cos(lp[k + 1].wb*M_rad);
			lp[k + 1].e.Y = -sin(lp[k + 1].wb*M_rad)*cos(lp[k + 1].wa*M_rad);
			lp[k + 1].e.X = -sin(lp[k + 1].wb*M_rad)*sin(lp[k + 1].wa*M_rad);
			CAT(&lp[k + 1].e.X, &lp[k + 1].e.Y, &lp[k + 1].e.Z);
			//HOUSEN2(&lp[k+1].P[0],&lp[k+1].P[1],&lp[k+1].P[2],&lp[k+1].e) ;

			strcpy_s(name, sizeof(name), "3");
			strcat_s(name, sizeof(name),  lp[k].opname);
			strcpy_s(lp[k + 2].opname, OPNAME_SZ, name);

			lp[k + 2].wa = obs[i].Wa - 180.0;
			if (lp[k + 2].wa <= -180.0)
				lp[k + 2].wa = 360.0 + lp[k + 2].wa;
			lp[k + 2].wb = 90.0;
			lp[k + 2].ref = obs[i].ref[2];
			for (p = 0; p < 366; p++)
				lp[k + 2].shad[p] = 1;

			lp[k + 2].rgb[0] = obs[i].rgb[0];
			lp[k + 2].rgb[1] = obs[i].rgb[1];
			lp[k + 2].rgb[2] = obs[i].rgb[2];
			lp[k + 2].polyd = 4;
			lp[k + 2].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k + 2].wd = 0;
			lp[k + 2].sbflg = 0;
			lp[k + 2].P[0] = lp[k + 1].P[3];
			lp[k + 2].P[1] = lp[k + 1].P[2];
			lp[k + 2].P[2].X = obs[i].x - obs[i].D*sbWa;
			lp[k + 2].P[2].Y = obs[i].y + obs[i].D*cbWa;
			lp[k + 2].P[2].Z = lp[k + 2].P[1].Z;
			lp[k + 2].P[3].X = lp[k + 2].P[2].X;
			lp[k + 2].P[3].Y = lp[k + 2].P[2].Y;
			lp[k + 2].P[3].Z = lp[k + 2].P[0].Z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k + 2].e.Z = cos(lp[k + 2].wb*M_rad);
			lp[k + 2].e.Y = -sin(lp[k + 2].wb*M_rad)*cos(lp[k + 2].wa*M_rad);
			lp[k + 2].e.X = -sin(lp[k + 2].wb*M_rad)*sin(lp[k + 2].wa*M_rad);
			CAT(&lp[k + 2].e.X, &lp[k + 2].e.Y, &lp[k + 2].e.Z);
			//HOUSEN2(&lp[k+2].P[0],&lp[k+2].P[1],&lp[k+2].P[2],&lp[k+2].e) ;

			strcpy_s(name, sizeof(name), "4");
			strcat_s(name, sizeof(name),  lp[k].opname);
			strcpy_s(lp[k + 3].opname, OPNAME_SZ, name);

			lp[k + 3].wa = obs[i].Wa + 90.0;
			if (lp[k + 3].wa > 180.0)
				lp[k + 3].wa = 360.0 - lp[k + 3].wa;
			lp[k + 3].wb = 90.0;
			lp[k + 3].ref = obs[i].ref[3];
			for (p = 0; p < 366; p++)
				lp[k + 3].shad[p] = 1;


			lp[k + 3].rgb[0] = obs[i].rgb[0];
			lp[k + 3].rgb[1] = obs[i].rgb[1];
			lp[k + 3].rgb[2] = obs[i].rgb[2];
			lp[k + 3].polyd = 4;
			lp[k + 3].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k + 3].wd = 0;
			lp[k + 3].sbflg = 0;
			lp[k + 3].P[0] = lp[k + 2].P[3];
			lp[k + 3].P[1] = lp[k + 2].P[2];
			lp[k + 3].P[2] = lp[k].P[1];
			lp[k + 3].P[3] = lp[k].P[0];

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k + 3].e.Z = cos(lp[k + 3].wb*M_rad);
			lp[k + 3].e.Y = -sin(lp[k + 3].wb*M_rad)*cos(lp[k + 3].wa*M_rad);
			lp[k + 3].e.X = -sin(lp[k + 3].wb*M_rad)*sin(lp[k + 3].wa*M_rad);
			CAT(&lp[k + 3].e.X, &lp[k + 3].e.Y, &lp[k + 3].e.Z);
			//HOUSEN2(&lp[k+3].P[0],&lp[k+3].P[1],&lp[k+3].P[2],&lp[k+3].e) ;

			k = k + 4;
		}

		else if (strcmp(obs[i].fname, "r_tri") == 0){

			caWb = 0.0; saWa = 0.0; caWa = 0.0; saWb = 0.0;

			caWb = cos(obs[i].Wb*M_rad);
			saWa = sin(obs[i].Wa*M_rad);
			saWb = sin(obs[i].Wb*M_rad);
			caWa = cos(obs[i].Wa*M_rad);

			//TODO: 安全にコピーできていない。一旦freeしてメモリの確保が必要。
			strcpy_s(lp[k].opname, OPNAME_SZ, obs[i].obsname);

			lp[k].wa = obs[i].Wa;
			lp[k].wb = obs[i].Wb;
			lp[k].ref = obs[i].ref[0];
			for (p = 0; p < 366; p++)
				lp[k].shad[p] = 1;

			lp[k].rgb[0] = obs[i].rgb[0];
			lp[k].rgb[1] = obs[i].rgb[1];
			lp[k].rgb[2] = obs[i].rgb[2];
			lp[k].polyd = 3;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 3);

			lp[k].wd = 0;
			lp[k].sbflg = 0;

			lp[k].P[0].X = obs[i].x;
			lp[k].P[0].Y = obs[i].y;
			lp[k].P[0].Z = obs[i].z;
			lp[k].P[1].X = obs[i].x - obs[i].H*caWb*saWa;
			lp[k].P[1].Y = obs[i].y + obs[i].H*caWb*caWa;
			lp[k].P[1].Z = obs[i].z + obs[i].H*saWb;
			lp[k].P[2].X = obs[i].x + obs[i].W*caWa;
			lp[k].P[2].Y = obs[i].y + obs[i].W*saWa;
			lp[k].P[2].Z = obs[i].z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;
		}
		else if (strcmp(obs[i].fname, "i_tri") == 0){

			caWb = 0.0; saWa = 0.0; caWa = 0.0; saWb = 0.0;

			caWb = cos(obs[i].Wb*M_rad);
			saWa = sin(-obs[i].Wa*M_rad);
			saWb = sin(obs[i].Wb*M_rad);
			caWa = cos(-obs[i].Wa*M_rad);

			strcpy_s(lp[k].opname, OPNAME_SZ, obs[i].obsname);

			lp[k].wa = obs[i].Wa;
			lp[k].wb = obs[i].Wb;
			lp[k].ref = obs[i].ref[0];
			for (p = 0; p < 366; p++)
				lp[k].shad[p] = 1;

			lp[k].rgb[0] = obs[i].rgb[0];
			lp[k].rgb[1] = obs[i].rgb[1];
			lp[k].rgb[2] = obs[i].rgb[2];
			lp[k].polyd = 3;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 3);

			lp[k].wd = 0;
			lp[k].sbflg = 0;

			lp[k].P[0].X = obs[i].x;
			lp[k].P[0].Y = obs[i].y;
			lp[k].P[0].Z = obs[i].z;
			lp[k].P[1].X = obs[i].x + ((obs[i].W) / 2)*caWa - obs[i].H*caWb*saWa;
			lp[k].P[1].Y = obs[i].y + obs[i].H*caWb*caWa + ((obs[i].W) / 2)*saWa;
			lp[k].P[1].Z = obs[i].z + obs[i].H*saWb;
			lp[k].P[2].X = obs[i].x + obs[i].W*caWa;
			lp[k].P[2].Y = obs[i].y + obs[i].W*saWa;
			lp[k].P[2].Z = obs[i].z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;
		}
		else {
			printf("error--**COORDNT-lp\n");
			//getch()
			preexit();
			exit(1);
		}

	}

	/*--------------------------------------------------------*/
	for (i = 0; i < treen; i++){
		if (strcmp(tree[i].treetype, "treeA") == 0){
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcat_s(name, sizeof(name),  "-m1");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			for (p = 0; p < 366; p++)
				lp[k].shad[p] = 1;

			lp[k].rgb[0] = 0.4;
			lp[k].rgb[1] = 0.3;
			lp[k].rgb[2] = 0.01;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].wa = 0;
			lp[k].wb = 90.0;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			lp[k].P[0].X = tree[i].x - (tree[i].W1 * 0.5);
			lp[k].P[0].Y = tree[i].y - (tree[i].W1 * 0.5);
			lp[k].P[0].Z = tree[i].z;
			lp[k].P[1].X = lp[k].P[0].X;
			lp[k].P[1].Y = lp[k].P[0].Y;
			lp[k].P[1].Z = tree[i].z + tree[i].H1;
			lp[k].P[2].X = tree[i].x + (tree[i].W1 * 0.5);
			lp[k].P[2].Y = lp[k].P[0].Y;
			lp[k].P[2].Z = tree[i].z + tree[i].H1;
			lp[k].P[3].X = lp[k].P[2].X;
			lp[k].P[3].Y = lp[k].P[0].Y;
			lp[k].P[3].Z = tree[i].z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;

			/*----2----*/
			//strcpy(lp[k].opname, tree[i].treename);
			//strcat(lp[k].opname, "-m2");
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);
			lp[k].wa = -90.0;
			lp[k].wb = 90.0;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.4;
			lp[k].rgb[1] = 0.3;
			lp[k].rgb[2] = 0.01;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x + (tree[i].W1 * 0.5);
			lp[k].P[2].Y = tree[i].y + (tree[i].W1 * 0.5);
			lp[k].P[2].Z = tree[i].z + tree[i].H1;
			lp[k].P[3].X = lp[k].P[2].X;
			lp[k].P[3].Y = lp[k].P[2].Y;
			lp[k].P[3].Z = tree[i].z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;

			/*----3----*/
			//strcpy(lp[k].opname, tree[i].treename);
			//strcat(lp[k].opname, "-m3");
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcat_s(name, sizeof(name),  "-m3");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = 180.0;
			lp[k].wb = 90.0;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.4;
			lp[k].rgb[1] = 0.3;
			lp[k].rgb[2] = 0.01;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W1 * 0.5);
			lp[k].P[2].Y = tree[i].y + (tree[i].W1 * 0.5);
			lp[k].P[2].Z = tree[i].z + tree[i].H1;
			lp[k].P[3].X = lp[k].P[2].X;
			lp[k].P[3].Y = lp[k].P[2].Y;
			lp[k].P[3].Z = tree[i].z;

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;

			/*----4----*/
			//strcpy(lp[k].opname, tree[i].treename);
			//strcat(lp[k].opname, "-m4");
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcat_s(name, sizeof(name),  "-m4");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = 90.0;
			lp[k].wb = 90.0;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.4;
			lp[k].rgb[1] = 0.3;
			lp[k].rgb[2] = 0.01;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2] = lp[k - 3].P[1];
			lp[k].P[3] = lp[k - 3].P[0];

			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);
			//HOUSEN2(&lp[k].P[0],&lp[k].P[1],&lp[k].P[2],&lp[k].e) ;

			k = k + 1;

			/*----5----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = -22.5;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0].X = tree[i].x;
			lp[k].P[0].Y = tree[i].y - (tree[i].W2 * 0.5);
			lp[k].P[0].Z = tree[i].z + tree[i].H1;
			lp[k].P[1].X = tree[i].x;
			lp[k].P[1].Y = tree[i].y - (tree[i].W3 * 0.5);
			lp[k].P[1].Z = tree[i].z + tree[i].H1 + tree[i].H2;
			lp[k].P[2].X = tree[i].x + (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y - (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x + (tree[i].W2 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y - (tree[i].W2 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = tree[i].z + tree[i].H1;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);


			k = k + 1;

			/*----6----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}


			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x + (tree[i].W3 * 0.5);
			lp[k].P[2].Y = tree[i].y;
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x + (tree[i].W2 * 0.5);
			lp[k].P[3].Y = tree[i].y;
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----7----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x + (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y + (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x + (tree[i].W2 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y + (tree[i].W2 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----8----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x;
			lp[k].P[2].Y = tree[i].y + (tree[i].W3 * 0.5);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x;
			lp[k].P[3].Y = tree[i].y + (tree[i].W2 * 0.5);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----9----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = 360 + (lp[k - 1].wa - 45);
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y + (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x - (tree[i].W2 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y + (tree[i].W2 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----10----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W3 * 0.5);
			lp[k].P[2].Y = tree[i].y;
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x - (tree[i].W2 * 0.5);
			lp[k].P[3].Y = tree[i].y;
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----11----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			//strcat_s(name, sizeof(name),  "-m2");
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y - (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x - (tree[i].W2 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y - (tree[i].W2 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----12----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x;
			lp[k].P[2].Y = tree[i].y - (tree[i].W3 * 0.5);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x;
			lp[k].P[3].Y = tree[i].y - (tree[i].W2 * 0.5);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----13----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = -22.5;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0].X = tree[i].x;
			lp[k].P[0].Y = tree[i].y - (tree[i].W3 * 0.5);
			lp[k].P[0].Z = tree[i].z + tree[i].H1 + tree[i].H2;
			lp[k].P[1].X = tree[i].x;
			lp[k].P[1].Y = tree[i].y - (tree[i].W4 * 0.5);
			lp[k].P[1].Z = tree[i].z + tree[i].H1 + tree[i].H2 + tree[i].H3;
			lp[k].P[2].X = tree[i].x + (tree[i].W4 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y - (tree[i].W4 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x + (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y - (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----14----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x + (tree[i].W4 * 0.5);
			lp[k].P[2].Y = tree[i].y;
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x + (tree[i].W3 * 0.5);
			lp[k].P[3].Y = tree[i].y;
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----15----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x + (tree[i].W4 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y + (tree[i].W4 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x + (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y + (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----16----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x;
			lp[k].P[2].Y = tree[i].y + (tree[i].W4 * 0.5);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x;
			lp[k].P[3].Y = tree[i].y + (tree[i].W3 * 0.5);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*---17---*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = 360 + (lp[k - 1].wa - 45);
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}


			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W4 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y + (tree[i].W4 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x - (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y + (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*----18----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W4 * 0.5);
			lp[k].P[2].Y = tree[i].y;
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x - (tree[i].W3 * 0.5);
			lp[k].P[3].Y = tree[i].y;
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*---19----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x - (tree[i].W4 * 0.5) * cos(45 * M_rad);
			lp[k].P[2].Y = tree[i].y - (tree[i].W4 * 0.5) * sin(45 * M_rad);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x - (tree[i].W3 * 0.5) * cos(45 * M_rad);
			lp[k].P[3].Y = tree[i].y - (tree[i].W3 * 0.5) * sin(45 * M_rad);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			k = k + 1;

			/*---20----*/
			//strcpy(lp[k].opname, tree[i].treename);
			strcpy_s(name, sizeof(name), tree[i].treename);
			strcpy_s(lp[k].opname, OPNAME_SZ, name);

			lp[k].wa = lp[k - 1].wa - 45;
			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			for (p = 0; p < 366; p++){
				lp[k].shad[p] = 1;
			}

			lp[k].rgb[0] = 0.0;
			lp[k].rgb[1] = 1;
			lp[k].rgb[2] = 0.0;
			lp[k].polyd = 4;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			lp[k].P[0] = lp[k - 1].P[3];
			lp[k].P[1] = lp[k - 1].P[2];
			lp[k].P[2].X = tree[i].x;
			lp[k].P[2].Y = tree[i].y - (tree[i].W4 * 0.5);
			lp[k].P[2].Z = lp[k].P[1].Z;
			lp[k].P[3].X = tree[i].x;
			lp[k].P[3].Y = tree[i].y - (tree[i].W3 * 0.5);
			lp[k].P[3].Z = lp[k].P[0].Z;

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);


			k = k + 1;
		}

		else{
			printf("error--**COORDNT-lp TREE \n");
			//getch() ;
			preexit();
			exit(1);
		}
	}
	//printf("polyn=%d\n",polyn) ;
	/*-------多角形の障害物の直接入力----------------------*/
	for (i = 0; i < polyn; i++){

		if (strcmp(poly[i].polyknd, "OBS") == 0){
			strcpy_s(lp[k].opname, OPNAME_SZ, poly[i].polyname);

			lp[k].polyd = poly[i].polyd;
			lp[k].P = (XYZ *)malloc(sizeof(XYZ)*lp[k].polyd);

			for (j = 0; j < lp[k].polyd; j++)
				lp[k].P[j] = poly[i].P[j];

			HOUSEN2(&lp[k].P[0], &lp[k].P[1], &lp[k].P[2], &lp[k].e);
			lp[k].wb = acos(lp[k].e.Z) * (180 / M_pai);
			lp[k].wa = asin(lp[k].e.X / sin(lp[k].wb*M_rad)) * (180 / M_pai);
			/*--法線ベクトルの算出　HOUSEN2関数を使うと、向きが逆になるので、変更 091128 higuchi --*/
			lp[k].e.Z = cos(lp[k].wb*M_rad);
			lp[k].e.Y = -sin(lp[k].wb*M_rad)*cos(lp[k].wa*M_rad);
			lp[k].e.X = -sin(lp[k].wb*M_rad)*sin(lp[k].wa*M_rad);
			CAT(&lp[k].e.X, &lp[k].e.Y, &lp[k].e.Z);

			lp[k].ref = 0.0;
			lp[k].wd = 0;
			lp[k].sbflg = 0;

			lp[k].rgb[0] = poly[i].rgb[0];
			lp[k].rgb[1] = poly[i].rgb[1];
			lp[k].rgb[2] = poly[i].rgb[2];

			for (p = 0; p < 366; p++)
				lp[k].shad[p] = 1;

			k = k + 1;
		}
	}
	//printf("k=%d\n",k) ;
	*lpn = k;
}

/*----------------------

受照面OPに変換する関数               1999.5.22
------------------------*/
void OP_COORDNT(int *opn, int bdpn, BBDP *BDP, P_MENN *op, int polyn, POLYGN *poly)
{
	int l, j, i, v, sumop, k;
	double cWa, sWa, cWb, sWb, a, b, c;
	double ax, by, cz;
	char	*stralloc(char *s);

	sumop = 0;

	for (l = 0; l < bdpn; l++){
		k = 0;
		cWa = 0.0; sWa = 0.0; cWb = 0.0; sWb = 0.0;

		cWa = cos(-BDP[l].Wa*M_rad);
		sWa = sin(-BDP[l].Wa*M_rad);
		cWb = cos(BDP[l].Wb*M_rad);
		sWb = sin(BDP[l].Wb*M_rad);

		sumop = sumop + BDP[l].sumRMP;
		v = sumop - BDP[l].sumRMP;
		for (j = v; j < sumop; j++){
			a = 0.0; b = 0.0; c = 0.0;

			/*--
			op[j].refg=BDP[l].refg ;
			--*/
			op[j].wd = BDP[l].RMP[k].sumWD;
			op[j].wlflg = 0;


			strcpy_s(op[j].opname, OPNAME_SZ, BDP[l].RMP[k].rmpname);

			op[j].rgb[0] = BDP[l].RMP[k].rgb[0];
			op[j].rgb[1] = BDP[l].RMP[k].rgb[1];
			op[j].rgb[2] = BDP[l].RMP[k].rgb[2];
			op[j].polyd = 4;
			op[j].P = (XYZ *)malloc(sizeof(XYZ)* 4);

			a = BDP[l].x0 + BDP[l].RMP[k].xb0*cWa - BDP[l].RMP[k].yb0*cWb*sWa;
			b = BDP[l].y0 + BDP[l].RMP[k].xb0*sWa + BDP[l].RMP[k].yb0*cWb*cWa;
			c = BDP[l].z0 + BDP[l].RMP[k].yb0*sWb;

			op[j].P[0].X = a;
			op[j].P[0].Y = b;
			op[j].P[0].Z = c;

			op[j].P[1].X = a - BDP[l].RMP[k].Rh*cWb*sWa;
			op[j].P[1].Y = b + BDP[l].RMP[k].Rh*cWb*cWa;
			op[j].P[1].Z = c + BDP[l].RMP[k].Rh*sWb;

			op[j].P[2].X = BDP[l].RMP[k].Rw*cWa + op[j].P[1].X;
			op[j].P[2].Y = BDP[l].RMP[k].Rw*sWa + op[j].P[1].Y;
			op[j].P[2].Z = op[j].P[1].Z;

			op[j].P[3].X = a + BDP[l].RMP[k].Rw*cWa;
			op[j].P[3].Y = b + BDP[l].RMP[k].Rw*sWa;
			op[j].P[3].Z = c;

			op[j].grpx = BDP[l].RMP[k].grpx;
			op[j].wa = BDP[l].Wa;
			op[j].wb = BDP[l].Wb;
			op[j].ref = BDP[l].RMP[k].ref;
			op[j].e.Z = cos(op[j].wb*M_rad);
			op[j].e.Y = -sin(op[j].wb*M_rad)*cos(op[j].wa*M_rad);
			op[j].e.X = -sin(op[j].wb*M_rad)*sin(op[j].wa*M_rad);
			CAT(&op[j].e.X, &op[j].e.Y, &op[j].e.Z);

			op[j].Nopw = BDP[l].RMP[k].sumWD;
			// opw構造体のメモリ確保
			if (op[j].Nopw > 0)
			{
				op[j].opw = (WD_MENN *)malloc(sizeof(WD_MENN)*op[j].Nopw);
				int m;
				WD_MENN *opw;
				opw = op[j].opw;
				for (m = 0; m < op[j].Nopw; m++, opw++)
				{
					opw->opwname = NULL;
					opw->P = NULL;
					opw->polyd = 0;
					opw->ref = opw->grpx = opw->sumw = 0.0;
					matinit(opw->rgb, 3);
				}
			}
			if (BDP[l].RMP[k].sumWD != 0){
				for (i = 0; i < BDP[l].RMP[k].sumWD; i++){
					ax = 0.0; by = 0.0; cz = 0.0;

					op[j].opw[i].opwname = stralloc(BDP[l].RMP[k].WD[i].winname);

					ax = a + BDP[l].RMP[k].WD[i].xr*cWa - BDP[l].RMP[k].WD[i].yr*cWb*sWa;
					by = b + BDP[l].RMP[k].WD[i].xr*sWa + BDP[l].RMP[k].WD[i].yr*cWb*cWa;
					cz = c + BDP[l].RMP[k].WD[i].yr*sWb;

					op[j].opw[i].grpx = BDP[l].RMP[k].WD[i].grpx;
					op[j].opw[i].ref = BDP[l].RMP[k].WD[i].ref;

					op[j].opw[i].rgb[0] = BDP[l].RMP[k].WD[i].rgb[0];
					op[j].opw[i].rgb[1] = BDP[l].RMP[k].WD[i].rgb[1];
					op[j].opw[i].rgb[2] = BDP[l].RMP[k].WD[i].rgb[2];
					op[j].opw[i].polyd = 4;
					op[j].opw[i].P = (XYZ *)malloc(sizeof(XYZ)* 4);

					op[j].opw[i].P[0].X = ax;
					op[j].opw[i].P[0].Y = by;
					op[j].opw[i].P[0].Z = cz;

					op[j].opw[i].P[1].X = ax - BDP[l].RMP[k].WD[i].Wh*cWb*sWa;
					op[j].opw[i].P[1].Y = by + BDP[l].RMP[k].WD[i].Wh*cWb*cWa;
					op[j].opw[i].P[1].Z = cz + BDP[l].RMP[k].WD[i].Wh*sWb;
					op[j].opw[i].P[2].X = ax + BDP[l].RMP[k].WD[i].Ww*cWa - BDP[l].RMP[k].WD[i].Wh*cWb*sWa;
					op[j].opw[i].P[2].Y = by + BDP[l].RMP[k].WD[i].Ww*sWa + BDP[l].RMP[k].WD[i].Wh*cWb*cWa;
					op[j].opw[i].P[2].Z = cz + BDP[l].RMP[k].WD[i].Wh*sWb;
					op[j].opw[i].P[3].X = ax + BDP[l].RMP[k].WD[i].Ww*cWa;
					op[j].opw[i].P[3].Y = by + BDP[l].RMP[k].WD[i].Ww*sWa;
					op[j].opw[i].P[3].Z = cz;
				}
			}
			k = k + 1;
		}
	}


	for (i = 0; i < polyn; i++){
		if (strcmp(poly[i].polyknd, "RMP") == 0){

			strcpy_s(op[sumop].opname, OPNAME_SZ, poly[i].polyname);
			op[sumop].ref = poly[i].ref;
			op[sumop].wd = 0;
			op[sumop].grpx = poly[i].grpx;

			op[sumop].rgb[0] = poly[i].rgb[0];
			op[sumop].rgb[1] = poly[i].rgb[1];
			op[sumop].rgb[2] = poly[i].rgb[2];
			op[sumop].polyd = poly[i].polyd;
			op[sumop].P = (XYZ *)malloc(sizeof(XYZ)*op[sumop].polyd);
			for (j = 0; j < op[sumop].polyd; j++)
				op[sumop].P[j] = poly[i].P[j];


			HOUSEN2(&op[sumop].P[0], &op[sumop].P[1], &op[sumop].P[2], &op[sumop].e);
			op[sumop].wb = acos(op[sumop].e.Z) * (180 / M_pai);
			op[sumop].wa = asin(op[sumop].e.X / sin(op[sumop].wb*M_rad)) * (180 / M_pai);

			/*--ポリゴンの法線ベクトルが逆で出てしまうのを修正 091128 higuchi--*/
			op[sumop].e.Z = cos(op[sumop].wb*M_rad);
			op[sumop].e.Y = -sin(op[sumop].wb*M_rad)*cos(op[sumop].wa*M_rad);
			op[sumop].e.X = -sin(op[sumop].wb*M_rad)*sin(op[sumop].wa*M_rad);


			/*--
			printf("wa=%f\n",sin(op[sumop].wb*M_rad)) ;
			printf("name=%s e.X=%f Y=%f Z=%f wb=%f wa=%f\n",op[sumop].opname,
			op[sumop].e.X,op[sumop].e.Y,op[sumop].e.Z,op[sumop].wb,op[sumop].wa) ;
			--*/
			sumop = sumop + 1;
		}

	}

	*opn = sumop;

}
