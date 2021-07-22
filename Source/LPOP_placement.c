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

						 LPとOPの位置をCGで確認するための入力ファイルを作成する
						 FILE=LPOP_placement.c
						 Create Date=2006.11.4
						 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wthrd.h"
#include "MODEL.h"   
#include "exs.h"
#include "FUNC.h"  


void HOUSING_PLACE(int lpn, int mpn, P_MENN *lp, P_MENN *mp, char *RET)
{
	char NAMAE1[1024], NAMAE2[1024], NAMAE3[1024];
	FILE *fp1, *fp2, *fp3;
	int i, j;
	int mlpn;

	mlpn = lpn + mpn;

	strcpy(NAMAE1, RET);
	strcat(NAMAE1, "_placeLP.gchi");
	//printf("%s\n",NAMAE1) ;

	if ((fp1 = fopen(NAMAE1, "w")) == NULL){
		printf("File not open _placeLP.gchi\n");
		exit(1);
	}
	strcpy(NAMAE2, RET);
	strcat(NAMAE2, "_placeOP.gchi");

	if ((fp2 = fopen(NAMAE2, "w")) == NULL){
		printf("File not open _placeOP.gchi\n");
		exit(1);
	}

	strcpy(NAMAE3, RET);
	strcat(NAMAE3, "_placeALL.gchi");

	if ((fp3 = fopen(NAMAE3, "w")) == NULL){
		printf("File not open _placeALL.gchi\n");
		exit(1);
	}

	fprintf(fp1, "%d ", lpn);
	fprintf(fp3, "%d ", mlpn);
	for (i = 0; i < lpn; i++){
		fprintf(fp1, "%s %d\n", lp[i].opname, lp[i].polyd);
		fprintf(fp3, "%s %d\n", lp[i].opname, lp[i].polyd);
		fprintf(fp1, "%f %f %f\n", lp[i].rgb[0], lp[i].rgb[1], lp[i].rgb[2]);
		fprintf(fp3, "%f %f %f\n", lp[i].rgb[0], lp[i].rgb[1], lp[i].rgb[2]);
		for (j = 0; j < lp[i].polyd; j++){
			fprintf(fp1, "%f %f %f\n", lp[i].P[j].X, lp[i].P[j].Y, lp[i].P[j].Z);
			fprintf(fp3, "%f %f %f\n", lp[i].P[j].X, lp[i].P[j].Y, lp[i].P[j].Z);
		}
	}


	fprintf(fp2, "%d ", mpn);
	for (i = 0; i < mpn; i++){
		fprintf(fp2, "%s %d\n", mp[i].opname, mp[i].polyd);
		fprintf(fp3, "%s %d\n", mp[i].opname, mp[i].polyd);
		fprintf(fp2, "%f %f %f\n", mp[i].rgb[0], mp[i].rgb[1], mp[i].rgb[2]);
		fprintf(fp3, "%f %f %f\n", mp[i].rgb[0], mp[i].rgb[1], mp[i].rgb[2]);
		for (j = 0; j < mp[i].polyd; j++){
			fprintf(fp2, "%f %f %f\n", mp[i].P[j].X, mp[i].P[j].Y, mp[i].P[j].Z);
			fprintf(fp3, "%f %f %f\n", mp[i].P[j].X, mp[i].P[j].Y, mp[i].P[j].Z);
		}
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

}
