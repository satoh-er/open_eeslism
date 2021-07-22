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

					  構造体から別の構造体へ代入
					  FILE=DAINYUU.c
					  Create Date=1999.6.7
					  */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include "MODEL.h"

void	preexit(void);
char	*stralloc(char *s);
/*---------------------------------------------------------------------*/
void DAINYUU_MP(P_MENN *mp, P_MENN *op, int opn, int mpn){
	int k, i, j, l;

	k = 0;
	for (i = 0; i < opn; i++){

		mp[k].P = (XYZ *)malloc(sizeof(XYZ)*op[i].polyd);
		mp[k] = op[i];
		mp[k].wd = 0;

		mp[k].sbflg = 0;
		mp[k].wlflg = 0;

		strcpy(mp[k].opname, op[i].opname);
		for (j = 0; j < op[i].wd; j++){
			k++;

			mp[k].wd = 0;

			mp[k].sbflg = 0;
			mp[k].wlflg = 1;

			mp[k].refg = op[i].refg;
			mp[k].ref = op[i].opw[j].ref;

			mp[k].rgb[0] = op[i].opw[j].rgb[0];
			mp[k].rgb[1] = op[i].opw[j].rgb[1];
			mp[k].rgb[2] = op[i].opw[j].rgb[2];

			mp[k].polyd = op[i].opw[j].polyd;
			mp[k].P = (XYZ *)malloc(sizeof(XYZ)*op[i].opw[j].polyd);

			mp[k].grpx = op[i].opw[j].grpx;
			mp[k].wb = op[i].wb;
			mp[k].wa = op[i].wa;
			mp[k].e = op[i].e;

			mp[k].opname = stralloc(op[i].opw[j].opwname);
			for (l = 0; l < mp[k].polyd; l++){
				mp[k].P[l] = op[i].opw[j].P[l];
			}
		}
		k++;
	}
	//printf("k=%d mpn=%d\n",k,mpn) ;
}
/*-------------------------------------------------------------------------*/
void DAINYUU_GP(XYZ *p, XYZ O, XYZ E, double ls, double ms, double ns){

	double t, u;

	u = E.X*ls + E.Y*ms + E.Z*ns;

	if (u != 0.0){
		t = (E.X*O.X + E.Y*O.Y + E.Z*O.Z) / u;
		p->X = ls*t;
		p->Y = ms*t;
		p->Z = ns*t;
	}
	else{
		printf("error DAINYUU.c GP\n");
		//getch();
		preexit();
		exit(1);
	}
}
/*-----------------------------------------------------------------------*/
void DAINYUU_SMO(int opn, int mpn, P_MENN *op, P_MENN *mp)
{
	int i, j, k;

	k = 0;
	for (i = 0; i < opn; i++){
		mp[k].sum = op[i].sum;
		//printf("mp[%d].sum=%f\n",k,mp[k].sum) ;
		for (j = 0; j < op[i].wd; j++){
			k++;
			mp[k].sum = op[i].opw[j].sumw;
		}
		k++;
	}
}
/*-----------------------------------------------------------------------*/
void DAINYUU_SMO2(int opn, int mpn, P_MENN *op, P_MENN *mp, SHADSTR *Sdstr, int dcnt, int tm)
{
	int i, j, k;

	k = 0;

	// 過去の影で近似しない時間
	if (dcnt == 1) {
		for (i = 0; i < opn; i++){
			mp[k].sum = op[i].sum;
			Sdstr[k].sdsum[tm] = mp[k].sum;
			//printf("<DAINYUU_SMO2> mp[%d].sum=%f\n",k,mp[k].sum) ;
			for (j = 0; j < op[i].wd; j++){
				k++;
				mp[k].sum = op[i].opw[j].sumw;
				Sdstr[k].sdsum[tm] = mp[k].sum;
			}
			k++;
		}
	}
	//過去の影で近似する時間
	else {
		for (k = 0; k < mpn; k++)
			mp[k].sum = Sdstr[k].sdsum[tm];
	}

}
