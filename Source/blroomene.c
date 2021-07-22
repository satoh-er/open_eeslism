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

/* bl_roomene.c */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"
#include "fnmcs.h"
#include "psy.h"
#include "fnfio.h"
#include "fnbldg.h"

/* 室温・湿度計算結果代入、室供給熱量計算
およびパネル入口温度代入、パネル供給熱量計算 */

void Roomene(RMVLS *Rmvls, int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl, EXSFS *Exsfs, WDAT *Wd)
{
	int    i, j;
	ELOUT  *E;
	ELIN   *elin, *elix;
	AIRSUP *A;
	extern double ca, ro;
	RDPNL  *rdpnl;

	rdpnl = Rdpnl;

	for (i = 0; i < Nroom; i++, Room++)
	{
		E = Room->cmp->elouts;
		Room->Tr = E->sysv;
		E++;
		Room->xr = E->sysv;
		Room->RH = FNRhtx(Room->Tr, Room->xr);
		Room->hr = FNH(Room->Tr, Room->xr);

		if ((A = Room->arsp) != NULL)
		{
			elin = Room->elinasup;
			elix = Room->elinasupx;

			for (j = 0; j < Room->Nasup; j++, A++, elin++, elix++)
			{
				if (elin->lpath->control != OFF_SW)
				{
					A->G = elin->lpath->G;
					A->Tin = elin->sysvin;
					A->Xin = elix->sysvin;
				}
				else
					A->G = A->Tin = A->Xin = 0.0;

				if (elin->lpath->control != OFF_SW)
					A->Qs = ca * elin->lpath->G * (elin->sysvin - Room->Tr);
				else
					A->Qs = 0.0;

				A->Ql = 0.0;
				if (elix->lpath != NULL && elix->lpath->control != OFF_SW)
					A->Ql = ro * elix->lpath->G * (elix->sysvin - Room->xr);

				A->Qt = A->Qs + A->Ql;
				//elin++, elix++;
			}
		}
	}
	for (i = 0; i < Nrdpnl; i++, Rdpnl++)
	{
		RMSRF	*Sd;
		WALL	*Wall;
		Sd = Rdpnl->sd[0];
		Wall = Sd->mw->wall;
		if (Rdpnl->cmp->control != OFF_SW)
		{
			double	cG;

			E = Rdpnl->cmp->elouts;
			Rdpnl->Tpi = Rdpnl->cmp->elins->sysvin;
			Rdpnl->Tpo = E->sysv;
			cG = Rdpnl->cG;
			Rdpnl->Q = cG * (E->sysv - Rdpnl->Tpi);

			if (Wall->WallType == 'C')
			{
				double	ECG;
				double	Kc;

				// 空気集熱建材の相当外気温度を再計算
				//CalcSolarWallTe(Rmvls, Wd, Exsfs) ;

				// 通気層の表面温度の計算
				FNTsuTsd(Sd, Wd, Exsfs);

				if (Wall->chrRinput == 'Y')
					Kc = Sd->dblKc;
				else
					Kc = Wall->Kc;

				ECG = cG * Rdpnl->Ec / (Kc * Sd->A);
				Rdpnl->sd[0]->Tf = (1. - ECG) * Sd->Tcole + ECG * Rdpnl->Tpi;

				// 集熱器途中温度の出力
				if (Sd->Ndiv > 0)
				{
					int		k;
					double	*Tc;
					Tc = Sd->Tc;
					for (k = 0; k < Sd->Ndiv; k++, Tc++)
					{
						double	Ec;
						//printf("%lf\n", exp(-Wall->Kc * Sd->A * (double)(k+1) / (double)Sd->Ndiv / cG)) ;
						Ec = 1. - exp(-Kc * Sd->A * (double)(k + 1) / (double)Sd->Ndiv / cG);
						//printf("Ec=%lf\n", Ec) ;
						*Tc = (1. - Ec) * Rdpnl->Tpi + Ec * Sd->Tcole;
					}
				}
			}
		}
		else
		{
			double	*Tc;
			int		k;
			Rdpnl->Q = Rdpnl->Tpi = Rdpnl->sd[0]->Tf = 0.0;

			if (Sd->Ndiv > 0)
			{
				Tc = Sd->Tc;
				for (k = 0; k < Sd->Ndiv; k++, Tc++)
					*Tc = 0.0;
			}

			// 通気層の表面温度の計算
			if (Wall->WallType == 'C')
				FNTsuTsd(Sd, Wd, Exsfs);
		}
	}
}

// PCM内蔵壁体の収束判定
void	PCMwlchk(int counter, RMVLS *Rmvls, EXSFS *Exsfs, WDAT *Wd, int *LDreset)
{
	int	i;
	ROOM	*Rm;
	int		Rmwlcreset;
	//static double EPS = 0.05;		// 収束判定値（見かけの比熱の5%）
	//double	relxcoeff = 0.1;

	Rmwlcreset = 0;
	Rm = Rmvls->Room;
	// 室温の仮計算
	for (i = 0; i < Rmvls->Nroom; i++, Rm++)
	{
		ELOUT	*Eo;
		Eo = Rm->cmp->elouts;
		Rm->Tr = Eo->sysv;
	}
	// 部位の表面温度の計算
	Rmsurftd(Rmvls->Nroom, Rmvls->Room, Rmvls->Sd);

	// 壁体内部温度の仮計算
	RMwltd(Rmvls->Nmwall, Rmvls->Mw);

	// PCM温度の収束判定
	Rm = Rmvls->Room;
	for (i = 0; i < Rmvls->Nroom; i++, Rm++)
	{
		int	j;
		RMSRF	*Sd;
		Sd = Rm->rsrf;
		// 部位でのループ
		for (j = 0; j < Rm->N; j++, Sd++)
		{
			// PCM建材が使用されているときだけチェックする
			if (Sd->PCMflg == 'Y')
			{
				MWALL	*mw;
				WALL	*Wall;
				//RMSRF	*Sd;
				double	*Toldd, *Twd, *Told;
				//if (i == 10 && j == 5)
				//	printf("xxxxx\n");
				mw = Sd->mw;
				Wall = mw->wall;
				Told = mw->Told;
				Toldd = mw->Toldd;
				Twd = mw->Twd;
				//Sd = mw->sd;
				PCMSTATE	*pcmstate;
				pcmstate = Sd->pcmstate;
				double	ToldPCMave, ToldPCMNodeL, ToldPCMNodeR;
				int	wall_reset = 0;
				if (Sd->mwside == 'i')
				{
					PCM		*PCM, *PCM1;
					int	m;


					for (m = 0; m < mw->M; m++, pcmstate++)
					{
						// PCM構造体
						PCM = Wall->PCMLyr[m];
						PCM1 = Wall->PCMLyr[m + 1];
						double	T, Toldn;
						int		PCMresetR, PCMresetL;
						double	nWeightR, nWeightL;
						PCMresetR = PCMresetL = 0;
						nWeightL = nWeightR = -999.;
						if (PCM != NULL && PCM->iterate == 'y')
						{
							//if (i == 10 && j == 5)
							//	printf("m=%d\n", m);
							pcmstate->tempPCMave = (Twd[m - 1] + Twd[m]) * 0.5;
							pcmstate->tempPCMNodeL = Twd[m - 1];
							pcmstate->tempPCMNodeR = Twd[m];
							ToldPCMave = (Told[m - 1] + Told[m]) * 0.5;
							ToldPCMNodeL = Told[m - 1];
							ToldPCMNodeR = Told[m];
							// PCM温度
							if (PCM->AveTemp == 'y')
							{
								T = pcmstate->tempPCMave;
								Toldn = ToldPCMave;
							}
							else
							{
								T = pcmstate->tempPCMNodeR;
								Toldn = ToldPCMNodeR;
							}
							//pcmstate->tempPCM = T;
							// m層の見かけの比熱
							if (PCM->spctype == 'm')
								pcmstate->capmR = FNPCMStatefun(PCM->Ctype, PCM->Cros, PCM->Crol,
								PCM->Ql, PCM->Ts, PCM->Tl, PCM->Tp, Toldn, T, PCM->DivTemp, PCM->PCMp);
							else
								pcmstate->capmR = FNPCMstate_table(&PCM->chartable[0], Toldn, T, PCM->DivTemp);
							//printf("capmR=%.0lf oldcapmR=%.0lf T=%.2lf Toldd=%.2lf oldT=%.2lf\n", pcmstate->capmR, pcmstate->oldcapmR, T, Toldd[m], Toldn);
							// 収束判定
							if (fabs(pcmstate->capmR - pcmstate->oldcapmR) > pcmstate->oldcapmR*PCM->IterateJudge 
								|| (PCM->iterateTemp=='y' && fabs(Twd[m] - Toldd[m]) > 1.e-2))
							{
								// 収束前ステップと現ステップの平均値で再計算
								//Toldd[m] = ((1. - PCM->nWeight)*Toldd[m] + PCM->nWeight*Twd[m]);
								//pcmstate->oldcapmR = pcmstate->capmR;
								//// 負荷再計算フラグ
								//(*LDreset)++;
								//// 後退差分係数行列再作成フラグ
								//Rmwlcreset++;
								nWeightR = PCM->nWeight;
								PCMresetR = 1;
							}
							pcmstate->oldcapmR = pcmstate->capmR;
						}

						if (PCM1 != NULL && PCM1->iterate == 'y')
						{
							(pcmstate + 1)->tempPCMave = (Twd[m] + Twd[m + 1]) * 0.5;
							(pcmstate + 1)->tempPCMNodeL = Twd[m];
							(pcmstate + 1)->tempPCMNodeR = Twd[m + 1];
							ToldPCMave = (Told[m] + Told[m + 1]) * 0.5;
							ToldPCMNodeL = Told[m];
							ToldPCMNodeR = Told[m + 1];
							// PCM温度
							if (PCM1->AveTemp == 'y')
							{
								T = (pcmstate + 1)->tempPCMave;
								Toldn = ToldPCMave;
							}
							else
							{
								T = (pcmstate + 1)->tempPCMNodeL;
								Toldn = ToldPCMNodeL;
							}
							// m層の見かけの比熱
							if (PCM1->spctype == 'm')
								(pcmstate + 1)->capmL = FNPCMStatefun(PCM1->Ctype, PCM1->Cros, PCM1->Crol,
								PCM1->Ql, PCM1->Ts, PCM1->Tl, PCM1->Tp, Toldn, T, PCM1->DivTemp, PCM1->PCMp);
							else
								(pcmstate + 1)->capmL = FNPCMstate_table(&PCM1->chartable[0], Toldn, T, PCM1->DivTemp);
							//printf("capmL=%.0lf oldcapmL=%.0lf T=%.2lf Toldd=%.2lf oldT=%.2lf\n", (pcmstate + 1)->capmL, (pcmstate + 1)->oldcapmL, T, Toldd[m], Toldn);
							// 収束判定
							if (fabs((pcmstate + 1)->capmL - (pcmstate + 1)->oldcapmL) > (pcmstate + 1)->oldcapmL*PCM1->IterateJudge
								|| (PCM1->iterateTemp=='y' && fabs(Twd[m] - Toldd[m]) > 1.e-2))
							{
								// 収束前ステップと現ステップの平均値で再計算
								//Toldd[m] = ((1. - PCM1->nWeight)*Toldd[m] + PCM1->nWeight*Twd[m]);
								//(pcmstate + 1)->oldcapmL = (pcmstate + 1)->capmL;
								//// 負荷再計算フラグ
								//(*LDreset)++;
								//// 後退差分係数行列再作成フラグ
								//Rmwlcreset++;
								nWeightL = PCM1->nWeight;
								PCMresetL = 1;
							}
							(pcmstate + 1)->oldcapmL = (pcmstate + 1)->capmL;
						}

						if (PCMresetR + PCMresetL != 0)
						{
							double nWeight;
							if (nWeightR > 0. && nWeightL > 0.)
								nWeight = (nWeightR + nWeightL) / 2.;
							else
								nWeight = dmax(nWeightR, nWeightL);

							//nWeight = dmax(0.5*exp(-0.01*(double)counter), 0.00001);
							//printf("%d nWeight=%lf\n", counter, nWeight);
							////nWeight = 0.5;
							//Toldd[m] = (1. - nWeight)*Toldd[m] + nWeight * Twd[m];
							//// 負荷再計算フラグ
							//(*LDreset)++;
							//// 後退差分係数行列再作成フラグ
							//Rmwlcreset++;

							//printf("再計算します\n");
						}

						//if (((PCMl != NULL && PCMl->iterate == 'y') || (PCMr != NULL && PCMr->iterate == 'y')) && fabs(*Toldd - *Twd) > 1.e-2)
						//{
						//	//printf("再計算します\n");
						//	// リセットフラグのインクリメント
						//	(*LDreset)++;
						//	// 収束前ステップと現ステップの平均値で再計算
						//	*Toldd = (*Toldd + *Twd*0.5) / 1.5;
						//}
					} // 層構成のループ完了

					// 暫定現在時刻のPCM温度を更新
					if (wall_reset > 0)
					{
						double nWeight;
						nWeight = dmax(0.5*exp(-0.01*(double)counter), 0.01);
					}
				}
			}
		}
	}

	if (Rmwlcreset > 0)
	{
		// 室の係数（壁体熱伝導等））、定数項の計算
		Roomcf(Rmvls->Nmwall, Rmvls->Mw, Rmvls->Nroom, Rmvls->Room,
			Rmvls->Nrdpnl, Rmvls->Rdpnl, Wd, Exsfs);
	}
}

// PCM内蔵家具のPCM温度収束判定
void	PCMfunchk(int Nroom, ROOM *Room, WDAT *Wd, int *LDreset)
{
	int	intI;
	double	tempTM;
	extern double dTM, ca;

	for (intI = 0; intI < Nroom; intI++, Room++)
	{
		if (Room->PCM != NULL)
		{
			tempTM = Room->TM;
			// 家具の温度の計算
			Room->TM = Room->FMT * Room->Tr + Room->FMC;
			//printf("Room=%s TMd=%.2lf TM=%.2lf Tr=%.2lf\n", Room->name, tempTM, Room->TM, Room->Tr);
			if (fabs(tempTM - Room->TM)>1.e-2 && Room->PCM->iterate == 'y')
			{
				(*LDreset)++;
				// 室温の係数
				// 家具の熱容量の計算
				if (Room->PCM->nWeight > 0.0)
					Room->TM = tempTM*(1.0 - Room->PCM->nWeight) + Room->TM*Room->PCM->nWeight;
				else
					Room->TM = (tempTM + Room->TM) / 2.0;
				void	FunCoeff(ROOM *Room);
				FunCoeff(Room);

				// 室空気の熱収支の係数計算
				// 家具の影響項の追加
				if (Room->FunHcap > 0.0)
				{
					double	dblTemp;
					dblTemp = dTM / Room->FunHcap;
					//Room->FMC = 1. / (Room->FunHcap / dTM + *(Room->CM)) * (Room->HM + Room->FunHcap / dTM * Room->oldTM);
					Room->FMC = 1. / (dblTemp* *(Room->CM) + 1.)*(Room->oldTM + dblTemp*Room->Qsolm);
				}
				else
					Room->FMC = 0.0;

				Room->RMC = Room->MRM / dTM * Room->Trold + Room->HGc + Room->CA;
				if (Room->FunHcap > 0.)
				{
					Room->RMC += *(Room->CM) * Room->FMC;
				}
				Room->RMt += ca * Room->Gvent;
				Room->RMC += ca * Room->Gvent * Wd->T;
			}
		}
	}
}

/* ------------------------------------------------------ */

/* 室負荷の計算 */

void Roomload(int Nroom, ROOM *Room, int *LDreset)
{
	int i, j, reset, resetl;
	ROOM   *rm;
	ELOUT  *Eo;
	AIRSUP *A;
	RMLOAD *rmld;
	ACHIR  *achr;

	TRNX  *trnx;
	RPANEL *rmpnl;
	double  *arn, *rmp;
	extern double  ca, ro;

	rm = Room;
	reset = 0;

	for (i = 0; i < Nroom; i++, Room++)
	{
		if ((rmld = Room->rmld) != NULL)
		{
			rmld->Qs = rmld->Ql = rmld->Qt = 0.0;
			Eo = Room->cmp->elouts;

			if (Eo->control == LOAD_SW)
			{
				rmld->Qs = Room->RMt * Room->Tr - Room->RMC;

				trnx = Room->trnx;
				arn = Room->ARN;
				for (j = 0; j < Room->Ntr; j++, trnx++, arn++)
				{
					rmld->Qs -= *arn * trnx->nextroom->Tr;
					//trnx++, arn++;
				}
				rmpnl = Room->rmpnl;
				rmp = Room->RMP;
				for (j = 0; j < Room->Nrp; j++, rmp++, rmpnl++)
				{
					rmld->Qs -= *rmp * rmpnl->pnl->Tpi;
					//rmp++, rmpnl++;
				}


				if ((A = Room->arsp) != NULL)
				{
					for (j = 0; j < Room->Nasup; j++, A++)
						rmld->Qs -= A->Qs;
				}
				achr = Room->achr;
				for (j = 0; j < Room->Nachr; j++, achr++)
					rmld->Qs -= ca * achr->Gvr * ((rm + achr->rm)->Tr - Room->Tr);

				/******
				printf("xxx Roomload i=%d Qs=%5.0lf loadt=%c, cotl=%c  sw=%c\n",
				i, rmld->Qs, *rmld->loadt, Eo->control, ON_SW);
				**************/
				reset = rmloadreset(rmld->Qs, *rmld->loadt, Eo, ON_SW);
				if (reset)
					(*LDreset)++;
			}
			Eo++;
			if (Eo->control == LOAD_SW)
			{
				rmld->Ql = ro * (Room->RMx * Room->xr - Room->RMXC);
				if ((A = Room->arsp) != NULL)
				{
					for (j = 0; j < Room->Nasup; j++, A++)
						rmld->Ql -= A->Ql;
				}

				achr = Room->achr;
				for (j = 0; j < Room->Nachr; j++, achr++)
					rmld->Ql -= ro * achr->Gvr * ((rm + achr->rm)->xr - Room->xr);

				resetl = rmloadreset(rmld->Ql, *rmld->loadt, Eo, ON_SW);
				if (reset || resetl)
				{
					Eo->control = ON_SW;
					Eo->eldobj->sysld = 'n';
					(*LDreset)++;
				}
			}
			rmld->Qt = rmld->Qs + rmld->Ql;
		}
	}
}

/* ------------------------------------------------------ */

/* 室供給熱量の出力 */

void rmqaprint(FILE *fo, int id, int Nroom, ROOM *Room)
{
	ELIN   *Ei;
	RPANEL *rpnl;
	AIRSUP *A;
	int   i, j, Nload;
	int		Nfnt;

	switch (id)
	{
	case 0:
		if (Nroom > 0)
			fprintf(fo, "%s %d\n", ROOM_TYPE, Nroom);
		for (i = 0; i < Nroom; i++, Room++)
		{
			if (Room->rmld != NULL)
				Nload = 2;
			else
				Nload = 0;

			// 家具の温度出力
			Nfnt = 0;
			if (Room->FunHcap > 0.)
				Nfnt = 4;

			//fprintf(fo, " %s 5 %d 4 %d %d %d\n", Room->name,
			//	4 + Nload + Room->Nasup * 2 + Room->Nrp + Nfnt,
			//	Nload, Room->Nasup * 2, Room->Nrp);
			int Nset = Room->setpri;
			fprintf(fo, " %s 5 %d 4 %d %d %d\n", Room->name,
				4 + Nload + Room->Nasup * 5 + Room->Nrp + Nfnt + Nset,
				Nload, Room->Nasup * 5, Room->Nrp);
		}
		break;

	case 1:
		for (i = 0; i < Nroom; i++, Room++)
		{
			fprintf(fo, "%s_Tr t f %s_xr x f %s_RH r f %s_Ts t f ",
				Room->name, Room->name, Room->name, Room->name);
			// SET*の出力
			if (Room->setpri == 1)
				fprintf(fo, "%s_SET* t f ", Room->name);

			// 家具の温度
			if (Room->FunHcap > 0.)
				fprintf(fo, "%s_TM t f %s_QM q f %s_QMsol q f %s_PCMQl q f ", Room->name, Room->name, Room->name, Room->name);

			if (Room->rmld != NULL)
				fprintf(fo, "%s_Ls q f %s_Lt q f ", Room->name, Room->name);

			if (Room->Nasup)
			{
				Ei = Room->cmp->elins;
				Ei += Room->Nachr + Room->Nrp;
				for (j = 0; j < Room->Nasup; j++, Ei++)
				{
					if (Ei->lpath == NULL)
						fprintf(fo, "%s:%1d_G m f %s:%1d_Tin t f %s:%1d_Xin x f %s:%1d_Qas q f %s:%1d_Qat q f ",
						Room->name, j, Room->name, j, Room->name, j, Room->name, j, Room->name, j);
					else
						fprintf(fo, "%s:%s_G m f %s:%s_Tin t f %s:%s_Xin x f %s:%s_Qas q f %s:%s_Qat q f ",
						Room->name, Ei->lpath->name, Room->name, Ei->lpath->name, Room->name, Ei->lpath->name, Room->name, Ei->lpath->name,
						Room->name, Ei->lpath->name);
				}
			}

			rpnl = Room->rmpnl;
			for (j = 0; j < Room->Nrp; j++, rpnl++)
				fprintf(fo, "%s:%s_Qp q f ", Room->name, rpnl->pnl->name);

			fprintf(fo, "\n");
		}
		break;

	default:
		for (i = 0; i < Nroom; i++, Room++)
		{
			fprintf(fo, "%.2lf %5.4lf %2.0lf %.2lf ",
				Room->Tr, Room->xr, Room->RH, Room->Tsav);

			// SET*の出力
			if (Room->setpri == 1)
				fprintf(fo, "%.2lf ", Room->SET);

			// 家具の温度、吸放熱量の出力
			if (Room->FunHcap > 0.)
				fprintf(fo, "%.2lf %.2lf %.2lf %.2lf ", Room->TM, Room->QM, Room->Qsolm, Room->PCMQl);

			if (Room->rmld != NULL)
				fprintf(fo, "%.2lf %.2lf ", Room->rmld->Qs, Room->rmld->Qt);

			if (Room->Nasup)
			{
				A = Room->arsp;
				for (j = 0; j < Room->Nasup; j++, A++)
					fprintf(fo, "%.4g %4.1lf %.4lf %.2lf %.2lf ", A->G, A->Tin, A->Xin, A->Qs, A->Qt);
			}
			rpnl = Room->rmpnl;
			for (j = 0; j < Room->Nrp; j++, rpnl++)
				fprintf(fo, " %.2lf", -rpnl->pnl->Q);

			fprintf(fo, "\n");
		}
		break;
	}
}

/* ------------------------------------------------------ */

/* 放射パネルに関する出力 */

void panelprint(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl)
{
	int i, ld = 0;
	WALL	*Wall;

	switch (id)
	{
	case 0:
		if (Nrdpnl > 0)
			fprintf(fo, "%s %d\n", RDPANEL_TYPE, Nrdpnl);
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			RMSRF	*Sd;
			Sd = Rdpnl->sd[0];
			Wall = Sd->mw->wall;
			if (Sd->mw->wall->WallType == 'P')
				fprintf(fo, " %s 1 5\n", Rdpnl->name);
			else	// 屋根一体型集熱器
			{
				ld = 0;
				if (Wall->chrRinput == 'Y')
					ld = 5;
				if (Rdpnl->sd[0]->PVwallFlg != 'Y')
					fprintf(fo, " %s 1 %d\n", Rdpnl->name, Sd->Ndiv + 8 + ld);
				else			// 太陽電池内蔵
					fprintf(fo, " %s 1 %d\n", Rdpnl->name, Sd->Ndiv + 11 + ld);
			}
		}
		break;

	case 1:
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			RMSRF	*Sd;
			Sd = Rdpnl->sd[0];
			Wall = Sd->mw->wall;
			if (Sd->mw->wall->WallType == 'P')
			{
				fprintf(fo, "%s_c c c %s_G m f %s_Ti t f %s_To t f %s_Q q f\n",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			}
			else		// 屋根一体型空気集熱器
			{
				if (Sd->PVwallFlg != 'Y')
					fprintf(fo, "%s_c c c %s_G m f %s_Ti t f %s_To t f %s_Te t f %s_Tf t f %s_Q q f %s_S q f\n",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
				else
					fprintf(fo, "%s_c c c %s_G m f %s_Ti t f %s_To t f %s_Te t f %s_Tf t f %s_Q q f %s_S q f  %s_TPV t f  %s_Iw  q f  %s_P q f\n",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name,
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);

				if (Wall->chrRinput == 'Y')
					fprintf(fo, "%s_Ksu q f %s_Ksd q f %s_Kc q f %s_Tsu t f %s_Tsd t f\n", Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);

				if (Sd->Ndiv > 0)
				{
					int	k;
					for (k = 0; k < Sd->Ndiv; k++)
						fprintf(fo, "%s_Tc[%d] t f  ", Rdpnl->name, k + 1);
					fprintf(fo, "\n");
				}
			}
		}
		break;

	default:
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			RMSRF	*Sd;
			Sd = Rdpnl->sd[0];
			Wall = Sd->mw->wall;
			if (Sd->mw->wall->WallType == 'P')
			{
				fprintf(fo, "%c %g  %4.1lf %4.1lf %3.0lf\n", Rdpnl->cmp->elouts->control,
					Rdpnl->cmp->elouts->G, Rdpnl->Tpi, Rdpnl->cmp->elouts->sysv, Rdpnl->Q);
			}
			else
			{
				ELOUT	*Eo;
				WALL	*Wall;
				double	G = 0.0;
				Wall = Sd->mw->wall;
				Eo = Rdpnl->cmp->elouts;
				if (Eo->control != OFF_SW)
					G = Eo->G;

				fprintf(fo, "%c %g  %4.1lf %4.1lf %4.1lf %4.1lf %3.0lf %3.0lf  ", Eo->control,
					G, Rdpnl->Tpi, Eo->sysv, Sd->Tcole, Sd->Tf, Rdpnl->Q, Sd->Iwall * Sd->A);

				if (Sd->PVwallFlg == 'Y')
					fprintf(fo, "%4.1lf %4.0lf %3.0lf\n", Sd->PVwall.TPV, Sd->Iwall, Sd->PVwall.Power);
				else
					fprintf(fo, "\n");

				if (Wall->chrRinput == 'Y')
					fprintf(fo, "%.3lf %.3lf %.3lf %.1lf %.1lf\n", Sd->dblKsu, Sd->dblKsd, Sd->dblKc, Sd->dblTsu, Sd->dblTsd);

				if (Sd->Ndiv > 0)
				{
					int	k;
					double	*Tc;
					Tc = Sd->Tc;
					for (k = 0; k < Sd->Ndiv; k++, Tc++)
						fprintf(fo, "%4.1lf ", *Tc);
					fprintf(fo, "\n");
				}
			}
		}
		break;
	}
}



