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
								 影の計算
								 FILE=SHADOW.c
								 Create Date=1999.6.7

								 */


#include <stdio.h>
#include <math.h>
#include "wthrd.h"
#include "MODEL.h"
#include "exs.h"
#include "FUNC.h"

void SHADOW(int g, double DE, int opn, int lpn, double ls, double ms, double ns,
	bekt *s, bekt *t, P_MENN *op, P_MENN *OP, P_MENN *LP,
	double *wap, double *wip, int nday)
{
	double Am, Bm, Amx, Amy, Amz, Bmx, Bmy, Bmz, A, B;
	int  i, j, h, k;
	double  Px, Py, Pz, Qx, Qy, Qz, OX, OY, OZ, OX1, OY1, OZ1, SHITA;
	int rp, rp2;   /*--多角形ループ-*/
	int naibu;     /*--壁面の内部かフラグ*/
	int naibuw;    /*--窓面の内部かフラグ*/
	int omote;     /*--面から見えるかフラグ　0:見えない 1:見える--*/
	double  DEM;
	double S, T;
	int flg = 0;
	double tau = 0.0;  /*--透過率（合計）--*/

	int loopA, loopB ;   /* ループカウンター　横・縦  20170821 higuchi*/
	int AMAX, BMAX ;  /* 分割数　幅・高さ*/

	DEM = DE / 1000.;  /*--mmをmに変換--*/
	rp = op->polyd - 2;

	for (i = 0; i < rp; i++){
		Amx = op->P[i + 1].X - op->P[0].X;
		Amy = op->P[i + 1].Y - op->P[0].Y;
		Amz = op->P[i + 1].Z - op->P[0].Z;
		Am = sqrt(Amx * Amx + Amy * Amy + Amz * Amz);
		Bmx = op->P[i + 2].X - op->P[0].X;
		Bmy = op->P[i + 2].Y - op->P[0].Y;
		Bmz = op->P[i + 2].Z - op->P[0].Z;
		Bm = sqrt(Bmx * Bmx + Bmy * Bmy + Bmz * Bmz);

		/* 20170821 higuchi add*/
		AMAX = (int)ceil(Am / DEM);
		BMAX = (int)ceil(Bm / DEM);


		for (loopA = 0; loopA < AMAX-1; loopA++) {
			for (loopB=0 ; loopB < BMAX-1 ; loopB++){

				A = (DEM / 2.0) + DEM*(double)loopA;
				B = (DEM / 2.0) + DEM*(double)loopB;

		//for (A = DEM / 2.; A < Am; A = A + DEM){
			//for (B = DEM / 2.; B < Bm; B = B + DEM){
				Px = 0.0, Py = 0.0, Pz = 0.0;
				Qx = 0.0, Qy = 0.0, Qz = 0.0;
				S = 0.0, T = 0.0;
				OX = 0.0, OY = 0.0, OZ = 0.0;
				OX1 = 0.0, OY1 = 0.0, OZ1 = 0.0;
				SHITA = 0.0;


				/*----
				printf("DEM=%f,A=%f,B=%f\n",DEM,A,B) ;
				----*/
				/*--グリッドを移動するＱ点の座標を求める---*/
				OX = (A / Am) * Amx;
				OY = (A / Am) * Amy;
				OZ = (A / Am) * Amz;
				OX1 = (B / Bm) * Bmx;
				OY1 = (B / Bm) * Bmy;
				OZ1 = (B / Bm) * Bmz;

				Qx = op->P[0].X + OX + OX1;
				Qy = op->P[0].Y + OY + OY1;
				Qz = op->P[0].Z + OZ + OZ1;

				naibu = 0;
				/*------QがＯＰの内部にあるか------*/
				rp2 = op->polyd - 2;
				for (j = 0; j < rp2; j++){
					INOROUT(Qx, Qy, Qz, op->P[0], op->P[j + 1], op->P[j + 2], &S, &T);
					if ((S >= 0.0 && T >= 0.0) && ((S + T) <= 1.0)){
						(*wap) = (*wap) + 1.0;
						naibu = 1;  /*--壁内部--*/
						naibuw = -1; /*--窓番号初期化--*/
						/*---Qが窓面にあるか----*/
						for (k = 0; k < op->wd; k++){
							for (h = 0; h < 2; h++){
								INOROUT(Qx, Qy, Qz, op->opw[k].P[0], op->opw[k].P[h + 1], op->opw[k].P[h + 2], &S, &T);
								if ((S >= 0.0 && T >= 0.0) && (S + T) <= 1.0){
									wip[k] = wip[k] + 1.0;
									naibuw = k;
									break;
								}
							}
							if (naibuw != -1)
								break;
						}
						break;
					}
				}
				if (naibu == 0)
					continue;

				/*---建物自身による影を考慮----*/
				for (j = 0; j<opn; j++){
					omote = 0;
					for (k = 0; k<OP[j].polyd; k++){
						if (s->ps[j][k]>0.0){
							omote = 1;
							break;
						}
					}
					if ((g != j) && (omote > 0)){
						KOUTEN(Qx, Qy, Qz, ls, ms, ns, &Px, &Py, &Pz, OP[j].P[0], OP[j].e);
						YOGEN(Qx, Qy, Qz, Px, Py, Pz, &SHITA, op->e);
						rp2 = OP[j].polyd - 2;
						for (k = 0; k<rp2; k++){   /*--多角形ループ---*/

							//printf("OP[%d] %s Px=%f Py=%f Pz=%f\n",j,OP[j].opname,Px,Py,Pz) ;

							INOROUT(Px, Py, Pz, OP[j].P[0], OP[j].P[k + 1], OP[j].P[k + 2], &S, &T);
							if (((S>0.0 && T > 0.0) && ((S + T)<1.0)) && (SHITA>0)){
								// if(strcmp(op->opname,"sroofwal")==0){
								// printf("name=%s->%s Px=%f Py=%f Pz=%f\n",op->opname,OP[j].opname,Px,Py,Pz) ;
								//  }

								if (naibu == 1)
									op->sum = op->sum + 1.0;
								if (naibuw >= 0)
									op->opw[naibuw].sumw = op->opw[naibuw].sumw + 1.0;
								goto koko;
							}
						}
					}
				}

				/*-------------障害物による影を考慮----------------------*/
				flg = 0;   // 100703 higuchi add
				for (h = 0; h<lpn; h++){
					omote = 0;
					for (k = 0; k<LP[h].polyd; k++){
						if (t->ps[h][k]>0.0){
							omote = 1;
							break;
						}
					}
					if (omote > 0){

						//	    flg = 0 ;  100703 higuchi dell
						KOUTEN(Qx, Qy, Qz, ls, ms, ns, &Px, &Py, &Pz, LP[h].P[0], LP[h].e);
						YOGEN(Qx, Qy, Qz, Px, Py, Pz, &SHITA, op->e);

						//printf("SHITA=%f\n", SHITA) ;

						rp2 = LP[h].polyd - 2;
						for (k = 0; k<rp2; k++){

							//printf("LP[%d] %s Px=%f Py=%f Pz=%f\n",j,LP[h].opname,Px,Py,Pz) ;


							INOROUT(Px, Py, Pz, LP[h].P[0], LP[h].P[k + 1], LP[h].P[k + 2], &S, &T);
							if (((S>0.0 && T > 0.0) && ((S + T)<1.0)) && (SHITA>0)){

								//printf("name=%s->%s Px=%f Py=%f Pz=%f\n",op->opname,LP[h].opname,Px,Py,Pz) ;
								//printf("SHITA=%f name=%s->%s Q x=%f y=%f z=%f P x=%f y=%f z=%f\n",SHITA, op->opname, LP[h].opname, Qx, Qy, Qz, Px, Py, Pz);

								if (flg == 0){
									tau = 1. - LP[h].shad[nday];
									flg = 1;
								}
								else
									tau = tau * (1. - LP[h].shad[nday]);

								break;  //100703 higuchi add
								//		    op->sum = op->sum + LP[h].shad[nday] ;
								//            if(naibuw >= 0)               
								//		       op->opw[naibuw].sumw=op->opw[naibuw].sumw+LP[h].shad[nday] ;
								//		    goto koko ;
							}
						}  // for

					}
				}  // for

				op->sum = op->sum + (1. - tau);
				//printf("op->sum=%lf\n", op->sum);
				if (naibuw >= 0)
					op->opw[naibuw].sumw = op->opw[naibuw].sumw + (1 - tau);
				tau = 1;
			koko:;
			}
		}
	}

	for (i = 0; i < op->wd; i++){
		(*wap) = (*wap) - wip[i];
		op->sum = op->sum - op->opw[i].sumw;
		if (wip[i] == 0)
			op->opw[i].sumw = 0;
		else
			op->opw[i].sumw = (op->opw[i].sumw / wip[i]);
	}

	/*--
	printf("sum=%f,wap=%f\n",op->sum,*wap) ;
	--*/
	if (*wap == 0)
		op->sum = 0;
	else
		op->sum = op->sum / (*wap);
}

/*----------------------------------------------------------------------*/
void SHADOWlp(int g, double DE, int lpn, int mpn, double ls, double ms, double ns,
	bekt *s, bekt *t, P_MENN *lp, P_MENN *LP, P_MENN *MP)
{
	double Am, Bm, Amx, Amy, Amz, Bmx, Bmy, Bmz, A, B, wap = 0.0;
	int  i, j, k, h;
	double  Px, Py, Pz, Qx, Qy, Qz;
	double  S, T;
	double  OX, OY, OZ, OX1, OY1, OZ1, SHITA;
	int omote;     /*--面から見えるかフラグ-*/
	int  rp, rp2;   /*--多角形ループ回数--*/

	int loopA, loopB;   /* ループカウンター　横・縦  20170821 higuchi*/
	int AMAX, BMAX;  /* 分割数　幅・高さ*/

	double  DEM;  /*higuchi add 20180125*/
	DEM = (DE * 5) / 1000.;  /*--mmをmに変換-- 粗目のグリッドにする　higuchi add 20180125*/
	rp = lp->polyd - 2;
	for (i = 0; i < rp; i++){

		Amx = lp->P[i + 1].X - lp->P[0].X;
		Amy = lp->P[i + 1].Y - lp->P[0].Y;
		Amz = lp->P[i + 1].Z - lp->P[0].Z;
		Am = sqrt(Amx*Amx + Amy*Amy + Amz*Amz);
		Bmx = lp->P[i + 2].X - lp->P[0].X;
		Bmy = lp->P[i + 2].Y - lp->P[0].Y;
		Bmz = lp->P[i + 2].Z - lp->P[0].Z;
		Bm = sqrt(Bmx*Bmx + Bmy*Bmy + Bmz*Bmz);
		//AM = Am * 100.0;
		//BM = Bm * 100.0;

		//dea = AM / DE;
		//deb = BM / DE;

		/* 20180125 higuchi add*/
		AMAX = (int)ceil(Am / DEM);
		BMAX = (int)ceil(Bm / DEM);

		/* 20170821 higuchi add*/
		//AMAX = (int)ceil(Am / dea);
		//BMAX = (int)ceil(Bm / deb);

		for (loopA = 0; loopA < AMAX - 1; loopA++) {
			for (loopB = 0; loopB < BMAX - 1; loopB++) {

				/*--higuchi upd 20180125--*/
				A = (DEM/ 2.0) + DEM*(double)loopA;
				B = (DEM / 2.0) + DEM*(double)loopB;

		//for (A = (dea / 2); A < AM; A = A + dea){
			//for (B = (deb / 2); B < BM; B = B + deb){
				Px = 0.0, Py = 0.0, Pz = 0.0;
				Qx = 0.0, Qy = 0.0, Qz = 0.0;
				S = 0.0, T = 0.0, OX = 0.0, OY = 0.0, OZ = 0.0;
				OX1 = 0.0, OY1 = 0.0, OZ1 = 0.0, SHITA = 0.0;

				wap = wap + 1.0;

				OX = (A / Am)*Amx;
				OY = (A / Am)*Amy;
				OZ = (A / Am)*Amz;
				OX1 = (B / Bm)*Bmx;
				OY1 = (B / Bm)*Bmy;
				OZ1 = (B / Bm)*Bmz;

				//OX = (A / AM)*Amx;
				//OY = (A / AM)*Amy;
				//OZ = (A / AM)*Amz;
				//OX1 = (B / BM)*Bmx;
				//OY1 = (B / BM)*Bmy;
				//OZ1 = (B / BM)*Bmz;

				Qx = lp->P[0].X + OX + OX1;
				Qy = lp->P[0].Y + OY + OY1;
				Qz = lp->P[0].Z + OZ + OZ1;

				/*---------LPによる影を考慮------------------*/
				for (j = 0; j<lpn; j++){
					omote = 0;
					for (k = 0; k<LP[j].polyd; k++){
						if (s->ps[j][k]>0.0){
							omote = 1;
							break;
						}
					}
					if ((g != j) && (omote>0)){
						KOUTEN(Qx, Qy, Qz, ls, ms, ns, &Px, &Py, &Pz, LP[j].P[0], LP[j].e);
						YOGEN(Qx, Qy, Qz, Px, Py, Pz, &SHITA, lp->e);
						rp2 = LP[j].polyd - 2;
						for (k = 0; k<rp2; k++){
							//printf("LP[%d] name=%s\n", j, LP[j].opname);
							INOROUT(Px, Py, Pz, LP[j].P[0], LP[j].P[k + 1], LP[j].P[k + 2], &S, &T);
							if (((S >= 0.0 && T >= 0.0) && ((S + T) <= 1.0)) && (SHITA>0)){
								lp->sum = lp->sum + 1.0;
								goto koko;
							}
						}
					}
				}

				/*-----------------MPによる影を考慮----------------------*/
				for (h = 0; h<mpn; h++){
					omote = 0;
					for (k = 0; k<MP[h].polyd; k++){
						if (t->ps[h][k]>0.0){
							omote = 1;
							break;
						}
					}
					if (omote>0){
						KOUTEN(Qx, Qy, Qz, ls, ms, ns, &Px, &Py, &Pz, MP[h].P[0], MP[h].e);
						YOGEN(Qx, Qy, Qz, Px, Py, Pz, &SHITA, lp->e);
						rp2 = LP[h].polyd - 2;
						for (k = 0; k<rp2; k++){
							//printf("LP[%d] name=%s\n", h, LP[h].opname);
							INOROUT(Px, Py, Pz, LP[h].P[0], LP[h].P[k + 1], LP[h].P[k + 2], &S, &T);
							if (((S >= 0.0 && T >= 0.0) && ((S + T) <= 1.0)) && (SHITA>0)){
								lp->sum = lp->sum + 1.0;
								goto koko;
							}
						}
					}
				}
			koko:;
			}
		}
	}
	lp->sum = lp->sum / wap;
	//printf("sum=%f,wap=%f\n", lp->sum, wap);
}
