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
#include "MODEL.h"   /*---higuchi 070918 ---*/
#include "fesy.h"
#include "fnfio.h"
#include "fnbld.h"
//#include "build.h"

/* -------------------------------------- */

void Rmhtrcf(EXSFS *Exs, char *Emrk, ROOM *Room, RMSRF *Sd, WDAT *Wd)
{
	int		i, N, brs, n, Nrm;
	RMSRF	*sd, *nxsd;
	static int   count = 0;
	ROOM	*rm;
	extern int DEBUG;
	//void	HeapCheck(char *s);

	//HeapCheck("Rmhtrcf");
	if (Room != NULL)
	{
		N = 0;
		rm = Room;
		Nrm = rm->end;

		for (i = 0; i < Nrm; i++, rm++, Emrk++)
		{
			N = rm->N;
			brs = rm->brs;
			sd = Sd + brs;

			/***********************
			printf("----- Rmhtrcf  i=%d count=%d Emrk=%c\n",
			i, count, *Emrk); /*********/

			if (DEBUG)
				printf("Room Name=%s\n", rm->name);
			/************************/

			// 放射熱交換係数の計算
			if (count == 0 || *Emrk == '*')
				radex(N, sd, rm->F, rm->Wradx);

			/**********************/
			if (DEBUG)
				printf("radex end\n");
			/************************/

			// 放射熱伝達率の入れ替え
			// Wradx：放射熱交換係数
			//printf("<Rmhtrcf1> i=%d name=%s alr=%lf\n", i, rm->name, rm->alr);
			Radcf0(rm->Tsav, &rm->alrbold, N, sd, rm->Wradx, rm->alr);

			//printf("i=%d ali=%lf\n", i, Sd->ali);
			/**********************/
			if (DEBUG)
				printf("Radcf0 end\n");
			/************************/

			//if(sd->alicsch != NULL)
			//	printf("name=%s\n", sd->name) ;
			// 室内側対流熱伝達率の計算
			//printf("Tr=%lf\n", rm->Tr);
			//printf("<Rmhtrcf2> i=%d name=%s alr=%lf\n", i, rm->name, rm->alr);
			Htrcf(rm->alc, Exs->alosch, Exs->alotype, Exs->Exs,
				rm->Tr, N, rm->alr, sd, &rm->mrk, Wd);

			/**********************/
			if (DEBUG)
				printf("Htrcf end\n");
			/************************/
		}
		count++;
	}


	//printf("ali=%lf\n", Sd->ali);
	if (Sd != NULL)
	{
		sd = Sd;

		N = Sd->end;
		for (n = 0; n < N; n++, sd++)
		{
			if (sd->mwtype == 'C' && sd->mwside == 'i')
			{
				// 内壁の場合は裏面室の熱伝達率を入れ替える
				nxsd = sd->nxsd;
				sd->alo = nxsd->ali;
				nxsd->alo = sd->ali;
			}
		}
	}
}
/* ----------------------------------------------------------------- */

void Rmrdshfc(ROOM *Room, RMSRF *Sd)
{
	int		i, brs, N;
	RMSRF	*sd;

	if (Room != NULL)
	{
		N = Room->end;

		for (i = 0; i < N; i++, Room++)
		{
			brs = Room->brs;
			sd = Sd + brs;

			Radshfc(Room->N, Room->FArea, Room->Area, sd, Room->tfsol, Room->eqcv, Room->name, Room->fsolm);
		}
	}
}
/* ----------------------------------------------------------------- */
void Rmhtrsmcf(int Nsrf, RMSRF *Sd)
{
	int n;

	for (n = 0; n < Nsrf; n++, Sd++)
		Sd->K = 1.0 / (Sd->Rwall + 1.0 / Sd->ali + 1.0 / Sd->alo);
}
/* ----------------------------------------------------------------- */
// 透過日射、相当外気温度の計算
void Rmexct(ROOM *Room, int Nsrf, RMSRF *Sd, WDAT *Wd, EXSF *Exs, SNBK *Snbk,
	QRM *Qrm, int nday, int mt)
{
	extern double dTM, ro, ca;
	int		n, nn, i, ed, sb, Nrm, j;
	double  Fsdw, Idre, Idf, RN;
	double  //Qgt,
		Qgtn, Qga, //Qsab,
		//Qrnab,
		Sab, Rab;
	RMSRF	*Sdn, *Sdnx;
	QRM		*Q;
	ROOM	*rm;
	EXSF	*e;
	SNBK	*S;
	double	Tr;
	struct elout	*Eo;
	double	*dcalloc(unsigned int m, char *errkey);

	if (Room != NULL)
	{
		Nrm = Room->end;
		Q = Qrm;
		rm = Room;

		// 部位ごとの日射吸収比率のスケジュール対応（比率入力部位の日射入射比率初期化）
		for (i = 0; i < Nrm; i++, rm++)
		{
			RMSRF *rsd;
			// 室内部位の日射吸収比率の計算
			// 2017/12/25毎時計算へ変更
			// 家具の日射吸収割合
			rm->tfsol = 0.0;
			if (rm->fsolm != NULL)
				rm->tfsol = *(rm->fsolm);

			rsd = rm->rsrf;
			for (j = 0; j < rm->N; j++, rsd++)
			{
				// 床の場合
				if (rsd->ble == 'F' || rsd->ble == 'f')
				{
					// どの部位も日射吸収比率が定義されていない場合
					if (rm->Nfsolfix == 0)
					{
						// 床の日射吸収比率は固定
						rsd->ffix_flg = '*';
						free(rsd->fsol);
						rsd->fsol = NULL;
						rsd->fsol = dcalloc(1, "<Roomdata> dcalloc");
						*(rsd->fsol) = *(rm->flrsr) * rsd->A / rm->FArea;
					}
				}

				// fsolが規定されている部位についてfsolを合計する
				if (rsd->ffix_flg == '*')
					rm->tfsol += *(rsd->fsol);		// fsolの合計値計算
			}
		}
		// 室内部位の日射吸収比率の計算（毎計算ステップへ変更）2017/12/25
		Rmrdshfc(Room, Sd);
		rm = Room;
		for (i = 0; i < Nrm; i++, Q++, rm++)
		{
			rm->Qgt = 0.0;
			rm->Qsolm = 0.0;
			rm->Qsab = rm->Qrnab = 0.0;
			Q->solo = Q->solw = Q->asl = 0.0;
			Sdn = Sd + rm->brs;
			//printf("<Rmexct> name=%s\n", rm->name);
			for (nn = 0, n = rm->brs; nn < rm->N; nn++, n++, Sdn++)
			{
				/**************
			  printf(" xxx Rmexct xxx  i=%d  n=%d  nn=%d  nxrm=%d\n", i, n, nn, Sdn->nxrm);
			  *****************/

				ed = Sdn->exs;
				e = Exs + ed;
				Sdn->RSsol = Sdn->RSsold = 0.;
				Fsdw = 0.0;
				Qgtn = Qga = 0.0;
				//printf("sname=%s\n",Sdn->sname) ;
				if (Sdn->sname == NULL)
				{  /*---higuchi 070918---start-*/
					Sdn->Fsdworg = 0.;
					if ((sb = Sdn->sb) >= 0 && e->cinc > 0.0)
					{
						S = Snbk + sb;
						Fsdw = FNFsdw(S->type, S->ksi, e->tazm, e->tprof,
							S->D, S->W, S->H, S->W1, S->H1, S->W2, S->H2);
						Sdn->Fsdworg = Fsdw;
						/*********
							printf(" xxx Rmexct xxx  n=%d  sb=%d  type=%d  tazm=%lf  tprof=%lf Fsdw=%lf\n",
							n, sb, S->type, e->tazm, e->tprof, Fsdw);
							************/
					}
					else
						Fsdw = 0.0;

					Idre = e->Idre;
					Idf = e->Idf;
					RN = e->rn;

					//printf("1:Fsdw=%f,Idre=%f,Idf=%f,rn=%f\n",Fsdw,Idre,Idf,rn) ;

				}  /*---higuchi 070918 end--*/
				/*--higuchi 070918 start--*/
				else
				{

					Fsdw = Sdn->Fsdw;
					//                  Idre = Sdn->Idre ;  090131 higuchi Sdn->Idre が影をすでに考慮していたため、下に変更
					Idre = e->Idre;    /*--090131 higuchi  --*/
					Idf = Sdn->Idf;
					RN = Sdn->rn;

					//printf("%s:Fsdw=%f,Idre=%f,Idf=%f,rn=%f\n",Sdn->sname,Fsdw,Idre,Idf,RN) ;

				}  /*---higuchi 070918 end --*/

				switch (Sdn->ble)
				{
				case 'W':
					// 通常窓の場合
					//if ( Sdn->window->AirFlowFlg != 'Y' )
					//{
					/*--higuchi dell --
					Glasstga ( Sdn->A, Sdn->tgtn, Sdn->Bn,
					e->cinc, Fsdw, e->Idre, e->Idf, &Qgtn, &Qga);
					Rab =  Sdn->Eo * e->rn / Sdn->alo;
					---*/
					/*--higuchi add--*/
					Glasstga(Sdn->A, Sdn->tgtn, Sdn->Bn,
						e->cinc, Fsdw, Idre, Idf, &Qgtn, &Qga, Sdn->window->Cidtype, e->Prof, e->Gamma);
					Rab = Sdn->Eo * RN / Sdn->alo;

					Sab = Sdn->TeEsol = Qga / Sdn->A;
					Sdn->TeErn = -Rab;
					Sdn->TeEsol = Sab / Sdn->K;

					Sdn->Te = Sab / Sdn->K - Rab + Wd->T;

					// 開口部の透過日射熱取得
					Sdn->Qgt = Qgtn;
					// 開口部の吸収日射熱取得
					Sdn->Qga = Qga;
					// 開口部の夜間放射熱取得
					Sdn->Qrn = -Rab;

					rm->Qgt += Qgtn;
					rm->Qsab += Sab * Sdn->A;
					rm->Qrnab += Rab * Sdn->A * Sdn->K;

					/*---higuchi dell---
					Q->solw += Sdn->A * ( e->Idre + e->Idf);
					---*/
					Q->solw += Sdn->A * (Idre + Idf);  /*--higuchi add  --*/
					//}
					break;

				case 'E':  case 'F':  case 'R':			// このあたりを参考に修正（相当外気温度の計算）
					if (Sdn->typ != 'E' && Sd->typ != 'e')
					{
						WALL *wall;

						/*---higuchi dell
						Sab = Sdn->TeEsol =
						Sdn->as * ( e->Idre * ( 1.0 - Fsdw ) + e->Idf ) / Sdn->alo ;
						Rab = Sdn->Eo * e->rn / Sdn->alo;
						--*/
						/*---higuchi add---*/
						Sab = Sdn->TeEsol =
							Sdn->as * (Idre * (1.0 - Fsdw) + Idf) / Sdn->alo;
						Rab = Sdn->Eo * RN / Sdn->alo;			// 長波長
						//printf("%s:Sab=%f,Rab=%f\n",Sdn->sname,Sab,Rab) ;
						/*------------------*/

						Sdn->TeErn = -Rab;

						// 建材一体型空気集熱器のための相当外気温度修正
						if (Sdn->rpnl != NULL && Sdn->rpnl->type == 'C')
						{
							wall = Sdn->mw->wall;
							//wall->Scol = FNScol( wall->ta, Idre + Idf, wall->Ku, Sdn->alo, Sdn->Eo, e->Fs, RN ) ;
							Sdn->Te = Sdn->Tcole;
							//printf ( "Te=%lf\n", Sdn->Te ) ;
							Sdn->Iwall = Idre * (1.0 - Fsdw) + Idf;
						}
						else
							Sdn->Te = Sab - Rab + Wd->T;

						rm->Qsab += Sab * Sdn->A * Sdn->K;
						Sdn->Qga = Sab*Sdn->A*Sdn->K;
						rm->Qrnab += Rab * Sdn->A * Sdn->K;
						/*---higuchi dell--
						Q->solo += Sdn->A * ( e->Idre + e->Idf ) ;
						Q->asl += Sdn->as * Sdn->A * ( e->Idre + e->Idf ) ;
						---*/
						/*---higuchi add---*/
						Q->solo += Sdn->A * (Idre + Idf);
						Q->asl += Sdn->as * Sdn->A * (Idre + Idf);
						/*-------------------*/
					}
					else
					{
						Sdn->Te = e->Tearth;
						Sdn->TeEsol = Sdn->TeErn = 0.0;
					}
					break;

				case 'i':  case 'f':  case 'c':  case 'd':
					if (Sdn->nxrm < 0)
					{
						Tr = Sdn->room->Trold;
						Eo = Sdn->room->cmp->elouts;
						if (Eo->control == LOAD_SW)
							Tr = Sdn->room->rmld->Tset;
						//Sdn->Te = Sdn->c * Room->Trold + (1.0 - Sdn->c) * Wd->T;
						Sdn->Te = Sdn->c*Tr + (1.0 - Sdn->c)*Wd->T;
					}
					else
					{
						Tr = Sdn->nextroom->Trold;
						Eo = Sdn->nextroom->cmp->elouts;
						if (Eo->control == LOAD_SW)
							Tr = Sdn->nextroom->rmld->Tset;
						//Sdn->Te = Sdn->c * (Room+Sdn->nxrm)->Trold + (1.0 - Sdn->c) * Wd->T;
						Sdn->Te = Sdn->c * Tr + (1.0 - Sdn->c) * Wd->T;
					}
					//printf("Te=%.2lf\tTr=%.2lf\tTo=%.2lf\n", Sdn->Te, Tr, Wd->T);

					//Sdn->Te = Sdn->c * ( Room + Sdn->nxrm )->Trold + (1.0 - Sdn->c) * Wd->T ;
					Sdn->TeEsol = Sdn->TeErn = 0.0;

					/*********
					//if (Sdn->mwtype != 'C')
						printf(" xxx Rmexct xxx  n=%d ble=%c  c=%lf  name=%s Te=%lf\n",
						n, Sdn->ble, Sdn->c, Sdn->room->name, Sdn->Te);
					/**********/

					break;
				}
			}	// 表面ループ

			// 室内部位への入射日射の計算（吸収日射ではない）
			Sdn = Sd + rm->brs;
			for (nn = 0, n = rm->brs; nn < rm->N; nn++, n++, Sdn++)
			{
				// 室内部位への入射日射量の計算
				Sdn->RSsold = rm->Qgt * Sdn->srg;
			}
		}	// 室ループ終了

		Nrm = Room->end;
		rm = Room;
		Q = Qrm;
		//if (mt == 7)
		//	free(Exs[6].name);
		// 透過日射の室内部位の最終計算（隣接室への日射分配、透過日射のうちガラスから屋外に放熱される分も考慮）
		for (i = 0; i < Nrm; i++, Q++, rm++)
		{
			// 透過間仕切りなど、隣接空間への透過日射分配の計算
			Sdn = Sd + rm->brs;
			for (nn = 0, n = rm->brs; nn < rm->N; nn++, n++, Sdn++)
			{
				//if (i == 1 && mt == 7)
				//	printf("debug\n");
				if (Sdn->tnxt>0. && Sdn->RSsold>0.)
				{
					double	RSsol;
					ROOM	*Rmnxt;
					Rmnxt = Room + Sdn->nxrm;
					RSsol = Sdn->RSsold*Sdn->tnxt;
					//printf("Room=%s name=%s tnxt=%lf\n", rm->name, Sdn->name, Sdn->tnxt);
					// 入射日射×透過率が当該室の透過日射熱取得より減ずる
					rm->Qgt -= RSsol;
					if (Sdn->nextroom != NULL)
					{
						// 外皮でない場合は隣室の透過日射熱取得に透過分を加算
						Rmnxt->Qgt += RSsol;
					}
				}

				// 透過日射が入射したときに屋外に放熱されるときには、表面吸収日射はゼロとする
				if (Sdn->RStrans == 'y')
				{
					rm->Qgt -= Sdn->RSsold;
					Sdn->RSsol = 0.;
				}
				//if (i == 1 && mt == 7)
				//	free(Exs[6].name);
			}
		}

		//if (mt == 7)
		//	free(Exs[6].name);
		Nrm = Room->end;
		rm = Room;
		Q = Qrm;
		for (i = 0; i < Nrm; i++, Q++, rm++)
		{
			// 室内部位の短波長吸収量の計算
			Sdn = Sd + rm->brs;
			for (nn = 0, n = rm->brs; nn < rm->N; nn++, n++, Sdn++)
			{
				//printf("Sdn->ali=%lf\n", Sdn->ali);
				//if (Sdn->RStrans == 'n')
					Sdn->RSsol = rm->Qgt * Sdn->srg2 / Sdn->A;
				//else
				//{
				//	// 透過日射が入射したときに屋外に放熱されるときには、表面吸収日射はゼロとする
				//	rm->Qgt -= Sdn->RSsol;
				//	Sdn->RSsol = 0.;
				//}

				Sdn->RS = (Sdn->RSsol * Sdn->A + rm->Hr * Sdn->srh
					+ rm->Lr * Sdn->srl + rm->Ar * Sdn->sra + rm->Qeqp * Sdn->eqrd) / Sdn->A;

				Sdn->RSin = (rm->Hr * Sdn->srh +
					rm->Lr * Sdn->srl + rm->Ar * Sdn->sra + rm->Qeqp * Sdn->eqrd) / Sdn->A;
				Sdn->RSli = rm->Lr * Sdn->srl / Sdn->A;

				//Sdn->Qi = Sdn->Qr + Sdn->Qc - Sdn->RS * Sdn->A ;
			}
			/***printf("<Rmexct>  i=%d rsrnx=%c\n", i, Room->rsrnx);
			*****/

			// 室の透過日射熱取得を再度積算（透明間仕切りによる隣接空間からの透過日射を考慮するため）
			//Sdn = Sd + rm->brs;
			//rm->Qgt = 0.0;
			//for (nn = 0, n = rm->brs; nn < rm->N; nn++, n++, Sdn++)
			//	rm->Qgt += Sdn->RSsol*Sdn->A;

			if (rm->rsrnx == 'y')
			{
				Sdn = Sd + rm->brs;
				for (nn = 0, n = rm->brs; nn < rm->N; nn++, n++, Sdn++)
				{
					if (Sdn->ble == 'c' || Sdn->ble == 'f')
					{
						if (Sdn->nxn >= 0)
							Sdn->Te += (Sd + Sdn->nxn)->RS / Sdn->alo;
					}
				}
			}

			Q->tsol = rm->Qgt;
			Q->asol = rm->Qsab;
			Q->arn = rm->Qrnab;
			// 家具の日射吸収量の計算
			rm->Qsolm = 0.;
			if (rm->fsolm != NULL)
				rm->Qsolm = rm->Qgt * rm->Srgm2;

			/****************************
			Q->hums = rm->Hc + rm->Hr;
			Q->light = rm->Lc + rm->Lr;
			Q->apls = rm->Ac + rm->Ar;
			Q->huml = rm->HL ;
			Q->apll = rm->AL;
			Q->hgins = Q->hums + Q->light + Q->apls ;
			Q->Qinfs = ca * rm->Gvi * ( Wd->T - rm->Tr ) ;
			Q->Qinfl = ro * rm->Gvi * ( Wd->x - rm->xr ) ;
			Q->Qeqp = rm->Qeqp ;
			Q->Qsto = rm->RMC * (rm->Trold - rm->Tr) / dTM ;
			Q->Qstol = rm->GRM * ro * ( rm->xrold - rm->xr ) / dTM ;
			************************************/
		}	// 室ループ

		Sdn = Sd;
		for (n = 0; n < Nsrf; n++, Sdn++)
		{
			if (Sdn->mwtype == 'C')
			{
				Sdnx = Sdn->nxsd;
				Sdn->Te = (Sdnx->alir * Sdnx->Tmrt + Sdnx->RS) / Sdnx->ali;

				/*
				printf("----- Rmexct  n=%d Te=%lf nxalir=%lf nxTmrt=%lf nxSdnx->RS nxali=%lf\n",
				n, Sdn->Te, Sdnx->alir, Sdnx->Tmrt, Sdnx->RS, Sdnx->ali);
				*/
			}
		}
	}
}
/* ----------------------------------------------------------------- */

// 室の係数、定数項の計算
void Roomcf(int Nmwall, MWALL *Mw, int Nroom, ROOM *Room,
	int Nrdpnl, RDPNL *rdpnl, WDAT *Wd, EXSFS *Exsf)
{
	extern double dTM, ro, ca;
	int		i;
	RDPNL	*Rdpnl;

	for (Rdpnl = rdpnl; Rdpnl < rdpnl + Nrdpnl; Rdpnl++)
		panelwp(Rdpnl);

	// 壁体係数行列の作成（壁体数RMSRF分だけループ）
	RMwlc(Nmwall, Mw, Exsf, Wd);

	for (i = 0; i < Nroom; i++, Room++)
	{
		/*********
			printf("Roomcf RMcf begin\n"); ***********/

		RMcf(Room);

		/***********
		printf("Roomcf RMrc begin\n"); ************/

		// 室の定数項の計算
		RMrc(Room);
		//RMrc(Room, &Room->RMC);

		Room->RMx = Room->GRM / dTM;
		Room->RMXC = Room->RMx * Room->xrold + (Room->HL + Room->AL) / ro;

		Room->RMt += ca * Room->Gvent;
		Room->RMC += ca * Room->Gvent * Wd->T;
		Room->RMx += Room->Gvent;
		Room->RMXC += Room->Gvent * Wd->x;

		//if (i == 1)
		//	printf("name=%s RMt=%lf RMC=%lf\n", Room->name, Room->RMt, Room->RMC);
	}
	/************
	xprroom (Nroom, Room);
	********/

	for (Rdpnl = rdpnl; Rdpnl < rdpnl + Nrdpnl; Rdpnl++)
	{

		/*********
		printf("Roomcf Ranelcf begin\n");
		************/

		Panelcf(Rdpnl);

		/*******************
		printf("Roomcf Ranelcf end\n");
		*********************/

		Rdpnl->EPC = Panelce(Rdpnl);

		/**************
		printf("Roomcf  %s  Wp=%lf    Epw=%lf  EPt[0]=%lf  EPt[1]=%lf  EPC=%lf\n",
		Rdpnl->name, Rdpnl->Wp,  Rdpnl->Epw, Rdpnl->EPt[0], Rdpnl->EPt[1],Rdpnl->EPC);

		for (m = 0; m < Rdpnl->MC; m++)
		{
		for (j = 0; j < Rdpnl->Ntrm[m]; j++)
		printf("EPR[%d]=%lf  ", j, Rdpnl->EPR[m][j]);
		for (j = 0; j < Rdpnl->Nrp[m]; j++)
		printf("EPW[%d]=%lf  ", j, Rdpnl->EPW[m][j]);
		printf("\nm=%d  EPR/EPW end\n", m);
		}

		****************/
	}
}

/* ----------------------------------------------------------------- */
// 前時刻の室温の入れ替え、OT、MRTの計算
void Rmsurft(int Nroom, ROOM *Room, RMSRF *Sd)
{
	int		i, N, brs;
	RMSRF	*sd;
	extern int	DEBUG;
	//static double	r = 0.5 ;
	double	r;


	if (Room == NULL)
		return;

	// 重み係数が未定義もしくは不適切な数値の場合の対処
	if (Room->OTsetCwgt == NULL
		|| *(Room->OTsetCwgt) < 0.0 || *(Room->OTsetCwgt) > 1.0)
		r = 0.5;
	else
		r = *(Room->OTsetCwgt);

	if (DEBUG)
		printf("<Rmsurft> Start\n");

	for (i = 0; i < Nroom; i++, Room++)
	{
		if (DEBUG)
			printf("Room[%d]=%s\tN=%d\tbrs=%d\n", i, Room->name, Room->N, Room->brs);

		N = Room->N;
		brs = Room->brs;
		sd = Sd + brs;

		// 前時刻の温度の入れ替え
		Room->mrk = 'C';
		Room->Trold = Room->Tr;
		Room->xrold = Room->xr;
		//printf("name=%s Tr=%lf\n", Room->name, Room->Trold);

		if (Room->FunHcap > 0)
		{
			// 家具の温度の計算
			Room->TM = Room->FMT * Room->Tr + Room->FMC;
			// 家具の吸放熱量の計算
			if (Room->CM != NULL)
				Room->QM = *Room->CM * (Room->TM - Room->Tr);

			Room->oldTM = Room->TM;
		}

		if (DEBUG)
			printf("<Rmsurft>  RMsrt start\n");

		// 室内表面温度の計算
		RMsrt(Room);

		if (DEBUG)
			printf("<Rmsurft>  RMsrt end\n");

		Room->Tsav = RTsav(N, sd);
		Room->Tot = r * Room->Tr + (1.0 - r) * Room->Tsav;
		//printf("name=%s Tsav=%.2lf Tot=%.2lf\n", Room->name, Room->Tsav, Room->Tot);
	}
}

/* ----------------------------------------------------------------- */
// PCM収束計算過程における部位表面温度の計算
void Rmsurftd(int Nroom, ROOM *Room, RMSRF *Sd)
{
	int		i, N, brs;
	RMSRF	*sd;
	extern int	DEBUG;
	//static double	r = 0.5 ;
	double	r;


	if (Room == NULL)
		return;

	// 重み係数が未定義もしくは不適切な数値の場合の対処
	if (Room->OTsetCwgt == NULL
		|| *(Room->OTsetCwgt) < 0.0 || *(Room->OTsetCwgt) > 1.0)
		r = 0.5;
	else
		r = *(Room->OTsetCwgt);

	if (DEBUG)
		printf("<Rmsurft> Start\n");

	for (i = 0; i < Nroom; i++, Room++)
	{
		if (DEBUG)
			printf("Room[%d]=%s\tN=%d\tbrs=%d\n", i, Room->name, Room->N, Room->brs);

		N = Room->N;
		brs = Room->brs;
		sd = Sd + brs;

		// 前時刻の温度の入れ替え
		//Room->mrk = 'C';
		//Room->Trold = Room->Tr;
		//Room->xrold = Room->xr;
		//printf("name=%s Tr=%lf\n", Room->name, Room->Trold);

		//if (Room->FunHcap > 0)
		//{
		//	// 家具の温度の計算
		//	Room->TM = Room->FMT * Room->Tr + Room->FMC;
		//	// 家具の吸放熱量の計算
		//	if (Room->CM != NULL)
		//		Room->QM = *Room->CM * (Room->TM - Room->Tr);

		//	Room->oldTM = Room->TM;
		//}

		if (DEBUG)
			printf("<Rmsurft>  RMsrt start\n");

		// 室内表面温度の計算
		RMsrt(Room);

		if (DEBUG)
			printf("<Rmsurft>  RMsrt end\n");

		Room->Tsav = RTsav(N, sd);
		Room->Tot = r * Room->Tr + (1.0 - r) * Room->Tsav;
	}
}

/*--------------------------------------------------------------------------------------------*/

// 室の熱取得要素の計算
void	Qrmsim(ROOM *Room, WDAT *Wd, QRM *Qrm)
{
	extern double dTM, ro, ca;
	QRM		*Q;
	ROOM	*rm;
	int		i, Nrm;

	if (Room != NULL)
	{
		Nrm = Room->end;

		Q = Qrm;
		rm = Room;
		for (i = 0; i < Nrm; i++, Q++, rm++)
		{
			Q->hums = rm->Hc + rm->Hr;
			Q->light = rm->Lc + rm->Lr;
			Q->apls = rm->Ac + rm->Ar;

			Q->huml = rm->HL;
			Q->apll = rm->AL;

			Q->hgins = Q->hums + Q->light + Q->apls;

			Q->Qinfs = ca * rm->Gvent * (Wd->T - rm->Tr);
			Q->Qinfl = ro * rm->Gvent * (Wd->x - rm->xr);
			Q->Qeqp = rm->Qeqp;
			Q->Qsto = rm->MRM * (rm->Trold - rm->Tr) / dTM;
			Q->Qstol = rm->GRM * ro * (rm->xrold - rm->xr) / dTM;

			if (rm->AEsch != NULL)
				Q->AE = rm->AE * *rm->AEsch;
			else
				Q->AE = 0.0;

			if (rm->AGsch != NULL)
				Q->AG = rm->AG * *rm->AGsch;
			else
				Q->AG = 0.0;
		}
	}
}
