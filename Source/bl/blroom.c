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

/*  room.c       */

#include <stdlib.h>
#include "common.h"
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"  


/* ----------------------------------------------------------- */

/*  室内サブシステムに関する計算         */

void	RMcf(ROOM *Room)
{ 
	MWALL	*Mw;
	RMSRF	*Sdn, *sdj, *sdk;
	TRNX	*trn;
	RPANEL	*rmpnl;
	double  *alr, *arn, *rpnl;
	double  *XA, *wrn, *wsp;
	extern  double  dTM;
	int     n, N, j, kk, mp, M, MM;
	double	K, kc, ali;
	//double	K, kc, ali, AR;
	char	E[SCHAR] ;

	N = Room->N;
	*E = '\0' ;
	Sdn = Room->rsrf;
	for (n = 0; n < N; n++, Sdn++)
	{
	/**********************
	printf ( "Sdn->mrk=%c\n", Sdn->mrk ) ;
		/***********************/
		
		if (Sdn->mrk == '*' || Sdn->PCMflg == 'Y')
		{
		/****************************
		printf ( "Sdn->typ=%c\n", Sdn->typ ) ;
			/****************************/
			
			// 壁体（窓以外の場合）
			if ( Sdn->typ == 'H' || Sdn->typ == 'E' || Sdn->typ == 'e')
			{ 
				Mw = Sdn->mw;
				M = Mw->M;
				mp = Mw->mp;
				
				/********************
				printf ( "Sdn->mwside=%c\n", Sdn->mwside ) ;
				/*************************/
				
				if (Sdn->mwside == 'i')
				{
					Sdn->FI = Mw->uo * Mw->UX[0];

					if (Sdn->mw->wall->WallType != 'C')
						Sdn->FO = Mw->um * Mw->UX[M-1];
					else
						Sdn->FO = Sdn->ColCoeff * Mw->UX[M-1];
					
					/*******************
					printf ( "uo=%lf UX=%lf  um=%lf UX=%lf\n",
					Mw->uo, Mw->UX[0], Mw->um, Mw->UX[M-1] ) ;
					/**************************/
					
					if (Sdn->rpnl != NULL)
						Sdn->FP = Mw->Pc * Mw->UX[mp] * Sdn->rpnl->Wp;
					else
						Sdn->FP = 0.0;
				}
				else
				{
					MM = (M - 1) * M;

					if (Sdn->mw->wall->WallType != 'C')
						Sdn->FI = Mw->um * Mw->UX[MM + M - 1];
					else
						Sdn->FI = Sdn->ColCoeff * Mw->UX[MM + M - 1];

					Sdn->FO = Mw->uo * Mw->UX[MM];
					if (Sdn->rpnl != NULL)
					{
						//if (Sdn->mw->wall->WallType == 'P' )
							Sdn->FP = Mw->Pc * Mw->UX[MM + mp] * Sdn->rpnl->Wp;
						//else		// 建材一体型空気集熱パネル
						//{
						//	Sdn->FO = Mw->UX[MM];
						//	Sdn->FP = Mw->um * EWc * Mw->UX[MM + mp] ;
						//}
					}
					else
						Sdn->FP = 0.0;
				}     
			}
			else
			{
				// 窓の場合
				/***      K = Sdn->K = 1.0/(Sdn->Rwall + rai + rao); ***/
				K = Sdn->K;
				ali= Sdn->ali;
				Sdn->FI = 1.0 - K/ali;
				Sdn->FO = K/ali;
				Sdn->FP = 0.0;
			}
		}
		/************
		printf("--E45---RMcf  %s n=%d A=%lf k=%lf ali=%5.2lf alo=%5.2lf FI=%lf FO=%lf\n",
		Room->name, n, Sdn->A, Sdn->K, Sdn->ali, Sdn->alo,  Sdn->FI, Sdn->FO);
		/*********/
	}
	
	alr = Room->alr;
	XA = Room->XA;
	Sdn = Room->rsrf;
	for (n = 0; n < N; n++, Sdn++)
	{ 
		for (j=0; j<N; j++)
			XA[n*N+j] = -Sdn->FI * alr[n*N+j] / Sdn->ali;
		
		XA[n*N+n] = 1.0;
	}
	

        /*-- higuchi--
	sprintf ( E, "Room=%s  <Rmcf>", Room->name ) ;
        printf("%s, %d, %f",E,N,XA) ;
	----*/
	sprintf_s(E, sizeof(E), "<RMcf> name=%s", Room->name);
	matinv(XA, N, N, E );
       
        /*--
        matinv(XA, N ,N) ;
        --*/
	
	/*****
	printf(" RMcf matinv\n", j);*****/
	
	Sdn = Room->rsrf; 
	for (n = 0; n < N; n++, Sdn++)
	{ 
	/*******
		printf("RMcf   n=%d\n", n); ******/
		
		Sdn->WSR =0.0;
		/***Sdn->WSP = 0.0; ***/
		
		sdj = Room->rsrf;
		for(j = 0; j < N; j++, sdj++)
		{	 
			kc = sdj->alic / sdj->ali;  
			Sdn->WSR += XA[n * N + j] * sdj->FI * kc;
			
			/*****Sdn->WSP += XA[n*N+j] * sdj->FP; *****/
		}
		
		wrn = Sdn->WSRN;
		trn = Room->trnx;
		for (j = 0; j < Room->Ntr; j++, trn++, wrn++)
		{
			sdk = trn->sd;
			kk = (int)(sdk - Room->rsrf);
			*wrn = XA[n * N + kk] * sdk->FO * sdk->nxsd->alic / sdk->nxsd->ali;
			//wrn++;
		}
		/******printf("RMcf  room=%s Sd=%d wsp Room->Nrp=%d\n", 
		Room->name, n, Room->Nrp);   ******/
		
		wsp = Sdn->WSPL;
		rmpnl = Room->rmpnl;
		for (j = 0; j < Room->Nrp; j++, rmpnl++, wsp++)
		{
		/******
		printf("RMcf j=%d  pnl->name=%s pnl->sd=%d sd=%d\n", 
		j, rmpnl->pnl->name, rmpnl->sd - Room->rsrf, 
		rmpnl->pnl->sd[0] - Room->rsrf);
			**********/
			
			sdk = rmpnl->sd; 
			kk = (int)(sdk - Room->rsrf);
			*wsp = XA[n * N + kk] * sdk->FP;			// XA：室内表面温度計算のためのマトリックス
														// FP：パネルの係数
			//wsp++;
		}
		/*****printf("RMcf   wsp end\n");  *****/    
		
	}
	
	Room->AR=0.0;
	Sdn = Room->rsrf;
	for (n = 0; n < N; n++, Sdn++)     
		Room->AR += Sdn->A * Sdn->alic * (1.0 - Sdn->WSR);
	
	// 室内空気の総合熱収支式の係数
	arn = Room->ARN;
	for (j = 0; j < Room->Ntr; j++, arn++)
	{
		*arn = 0.0;
		sdk = Room->rsrf;
		for (n = 0; n < N; n++, sdk++)
			*arn += sdk->A * sdk->alic * sdk->WSRN[j];
		
			/*****
			printf("----- RMcf %s j=%d ARN=%lf\n", Room->name, j, *arn);
		************/
	}
	rpnl = Room->RMP;
	for (j = 0; j <Room->Nrp; j++, rpnl++)		// 室のパネル総数
	{
	/*****
		printf(" RMcf panel j=%d\n", j);*****/
		*rpnl = 0.0;
		sdk = Room->rsrf;
		for (n = 0; n < N; n++, sdk++)
			*rpnl += sdk->A * sdk->alic * sdk->WSPL[j];			// WSPL：パネルに関する係数
	}
	
	
	/**** *RMtp = -(*RMtp); ***/
	
	/*****printf("-- room,c RMcf MRM=%lf  dTM=%lf  AR=%lf\n",Room->MRM, dTM, AR);
	/******/
	
	// 室温の係数
	// 家具の熱容量の計算
	void	FunCoeff(ROOM *Room);
	FunCoeff(Room);
	//Room->FunHcap = 0.;
	//if (Room->CM != NULL && *(Room->CM) > 0.)
	//{
	//	if (Room->MCAP != NULL && *(Room->MCAP) > 0.)
	//		Room->FunHcap += *(Room->MCAP);
	//	if (Room->PCM != NULL)
	//	{
	//		double	FNPCMStatefun(double Ss, double Sl, double Ql, double Ts, double Tl, double oldT, double T, int DivTemp);
	//		Room->PCMQl = FNPCMStatefun(Room->PCM->Cros, Room->PCM->Crol, Room->PCM->Ql,
	//			Room->PCM->Ts, Room->PCM->Tl, Room->oldTM, Room->TM, Room->PCM->DivTemp);
	//		printf("比熱計算 TM=%lf oldTM=%lf Ql=%lf\n", Room->TM, Room->oldTM, Room->PCMQl);
	//		Room->FunHcap += Room->mPCM * Room->PCMQl;
	//	}
	//}
	//if (Room->FunHcap > 0.)
	//	Room->FMT = 1. / (Room->FunHcap / dTM / *(Room->CM) + 1.);
	//else
	//	Room->FMT = 1.0 ;

	//Room->RMt = Room->MRM/dTM + AR ;

	//if (Room->FunHcap>0.)
	//	Room->RMt -= *(Room->CM) * ( Room->FMT - 1.0 ) ;
	
	/**************************************/
	//printf("RMt=%lf AR=%lf FMT=%lf\n", Room->RMt, Room->AR, Room->FMT);
	/**************************************/
}

// 家具内蔵PCMの係数計算
void	FunCoeff(ROOM *Room)
{
	extern double dTM;
	// 室温の係数
	// 家具の熱容量の計算
	Room->FunHcap = 0.;
	if (Room->CM != NULL && *(Room->CM) > 0.)
	{
		if (Room->MCAP != NULL && *(Room->MCAP) > 0.)
			Room->FunHcap += *(Room->MCAP);
		if (Room->PCM != NULL)
		{
			//if (Room->oldTM<20. && Room->TM>20.)
			//	printf("TEST\n");
			if (Room->PCM->spctype == 'm')
				Room->PCMQl = FNPCMStatefun(Room->PCM->Ctype, Room->PCM->Cros, Room->PCM->Crol, Room->PCM->Ql,
				Room->PCM->Ts, Room->PCM->Tl, Room->PCM->Tp, Room->oldTM, Room->TM, Room->PCM->DivTemp, Room->PCM->PCMp);
			else
				Room->PCMQl = FNPCMstate_table(&Room->PCM->chartable[0], Room->oldTM, Room->TM, Room->PCM->DivTemp);
			//printf("比熱計算 TM=%lf oldTM=%lf Ql=%lf\n", Room->TM, Room->oldTM, Room->PCMQl);
			Room->FunHcap += Room->mPCM * Room->PCMQl;
		}
	}
	if (Room->FunHcap > 0.)
		Room->FMT = 1. / (Room->FunHcap / dTM / *(Room->CM) + 1.);
	else
		Room->FMT = 1.0;

	Room->RMt = Room->MRM / dTM + Room->AR;

	if (Room->FunHcap>0.)
		Room->RMt -= *(Room->CM) * (Room->FMT - 1.0);
}

//void RMrc(ROOM *Room, double *RMo)
void RMrc(ROOM *Room)
{
	RMSRF *Sdn; 
	MWALL *Mw;
	double *XA, *UX;
	double  //HGc, CA,
	*CRX;
	int  N, n, j, M, MM;
	
	extern double dTM;
	
	N = Room->N;
	XA = Room->XA;
	
	CRX = dcalloc ( N, "RMRc" ) ;
	
	Sdn = Room->rsrf;				// 室内表面の先頭アドレス
	for (n = 0; n < N; n++, Sdn++)			// N：表面総数
	{      
		Sdn->CF = 0.;
		if (Sdn->typ == 'H' || Sdn->typ == 'E'  || Sdn->typ == 'e')		// 壁の場合
		{ 
			Mw = Sdn->mw;
			M = Mw->M;
			if (Sdn->mwside != 'M')		// 室内側
			{
				for (j = 0; j < M; j++)
					Sdn->CF += Mw->UX[j] * Mw->Told[j];
			}
			else
			{
				MM = M * (M - 1);
				UX = Mw->UX + MM;
				for (j = 0; j < M; j++, UX++)
					Sdn->CF += *UX * Mw->Told[j];
			}
		}
	}
	
	Room->HGc = Room->Hc + Room->Lc + Room->Ac + Room->Qeqp * Room->eqcv;
	
	/***************
	printf ( "name=%s  Qeqp=%lf\n", Room->name, Room->Qeqp ) ;
	*********************/
	
	// 表面熱収支に関係する係数の計算
	Sdn = Room->rsrf;
	for (n = 0; n < N; n++, Sdn++)
	{
		
		CRX[n] = Sdn->CF + Sdn->FO * Sdn->Te + Sdn->FI * Sdn->RS/Sdn->ali;
		/*** 
		printf("xxxxRMrcxxxx n=%d ali=%6.4lf Te=%5.2lf  Told=%5.2lf CRX=%lf\n", 
		n, Sdn->ali, Sdn->Te, Sdn->mw->Told[0], CRX[n]);
		****/
		
	}
	
	// 相互放射の計算
	Sdn = Room->rsrf;
	for (n = 0; n < N; n++, Sdn++)
	{ 
		Sdn->WSC = 0.0;
		for (j = 0; j < N; j++)
			Sdn->WSC += XA[n * N + j] * CRX[j]; 
	}
	
	Sdn = Room->rsrf;    
	Room->CA = 0.0;
	for (n = 0; n < N; n++, Sdn++)
		Room->CA += Sdn->A * Sdn->alic * Sdn->WSC;
	
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
		Room->FMC = 0.0 ;

	//*RMo = Room->MRM / dTM * Room->Trold + Room->HGc + Room->CA;
	Room->RMC = Room->MRM / dTM * Room->Trold + Room->HGc + Room->CA;
	if (Room->FunHcap > 0.)
	{
		Room->RMC += *(Room->CM) * Room->FMC;
		//*RMo += *(Room->CM) * Room->FMC;
	}
	free ( CRX ) ;
}
/* ----------------------------------------------------- */
// 室内表面温度の計算
void	RMsrt ( ROOM *Room )
{    
	double	*alr ;
	int		n, j, N ;
	RMSRF	*Sdn, *Sd ;
	TRNX	*trn ;
	RPANEL	*rmpnl ;
	
	N = Room->N ;
	
	Sdn = Room->rsrf;
	for (n = 0; n < N; n++, Sdn++)
	{
		Sdn->Ts = Sdn->WSR * Room->Tr + Sdn->WSC;
		
		trn = Room->trnx;
		for (j = 0; j < Room->Ntr; j++, trn++)
			Sdn->Ts += Sdn->WSRN[j] * trn->nextroom->Tr;
		
		rmpnl = Room->rmpnl;
		for (j = 0; j < Room->Nrp; j++, rmpnl++)
		{
		/*****
		printf("-----RMsrt n=%d %s  j=%d %s Tpi=%lf\n", 
		n, Sdn->name, j, rmpnl->pnl->name, rmpnl->pnl->Tpi);
			*****/
			
			Sdn->Ts += Sdn->WSPL[j] * rmpnl->pnl->Tpi;
		}
	}
	
	Sdn = Room->rsrf;
	alr = Room->alr;
	for ( n = 0; n < N; n++, Sdn++ )
	{ 
		Sdn->Tmrt = 0.0 ;
		Sd = Room->rsrf ;
		for ( j = 0; j < N; j++, Sd++ )
		{
			if ( j != n )
				Sdn->Tmrt += Sd->Ts * alr[n * N + j];
		}
		Sdn->Tmrt /= alr[n * N + n];
	}

	Sd = Room->rsrf ;
	for ( n = 0; n < N; n++, Sd++ )
	{
		Sd->Qc = Sd->alic * Sd->A * ( Sd->Ts - Room->Tr ) ;
		Sd->Qr = Sd->alir * Sd->A * ( Sd->Ts - Sd->Tmrt ) ;
		Sd->Qi = Sd->Qc + Sd->Qr - Sd->RS * Sd->A ;
	}
}
/* ----------------------------------------------------- */

// 重量壁（後退差分）の係数行列の作成
void RMwlc (int Nmwall, MWALL *Mw, EXSFS *Exsfs, WDAT *Wd)
{
	RMSRF  *Sd;
	int  i;
	double  rai, rao, Wp;
	
	for (i = 0; i < Nmwall; i++, Mw++)
	{
		//double ECG, ECt, CFc ;
		WALL *Wall ;

		Sd = Mw->sd;
		Wall = Mw->wall ;

		rai = 1.0 / Sd->ali;					// このあたりを修正
		//printf("ali=%lf  rai=%lf\n", Sd->ali, rai ) ;
		//if ( Wall->WallType != 'C')
			rao = 1.0 / Sd->alo;
		//else
		//{
		//	// パネルが動作しているとき
		//	if ( Sd->rpnl->Wp > 0. )
		//	{
		//		// 境界条件の計算
		//		ECG = ECt = *EWc = CFc = 0.0 ;
		//		FNBoundarySolarWall( Sd, Wd, Exsfs, &Sd->rpnl->Ec, &ECG, &ECt, EWc, &CFc) ;

		//		rao = 1. / ( Wall->Kdd * ( 1. - ECG ) / Wall->Ko ) ;
		//	}
		//	else
		//		rao = 1. / Wall->KdKo ;
		//}
		
		Mw->res[0] = rai;						// 室内側表面熱抵抗
		if (Sd->typ == 'H')
			Mw->res[Mw->M] = rao;				// 室外側表面熱抵抗
		
		// 壁体にパネルがある場合
		if (Sd->rpnl != NULL)
		{
			Wp = Sd->rpnl->Wp;
		}
		else
			Wp = 0.0;
		
		/****
		printf("----- RMwlc  ble=%c  MAX=%d  i=%d  name=%s  M=%d ali=%lf alo=%lf\n",
			Mw->wall->ble, Nmwall, i, Sd->name, Mw->M, Sd->ali, Sd->alo);  /****/
		
		// 行列作成
		Wallfdc (Mw->M, Mw->mp, Mw->res, Mw->cap, Wp, Mw->UX,
			&Mw->uo, &Mw->um, &Mw->Pc, Mw->wall->WallType, Sd, Wd, Exsfs, Mw->wall, 
			Mw->Told, Mw->Toldd, Mw->sd->pcmstate);
	}
}

/* ----------------------------------------------------- */

// 壁体内部温度の計算
void RMwlt (int Nmwall, MWALL *Mw)
{
	RMSRF *Sd, *nxsd;
	ROOM  *Room;
	int i;
	double Tie, Tee, WTp;
	
	for (i = 0; i < Nmwall; i++, Mw++)
	{
		Sd = Mw->sd;
		nxsd = Sd->nxsd;
		Room = Sd->room;
		
		if (Sd->mwtype == 'C')
			Tee = (nxsd->alic * nxsd->room->Tr + nxsd->alir * nxsd->Tmrt + nxsd->RS) /
			nxsd->ali;
		else
			Tee = Sd->Te;
		
		Tie = (Sd->alic * Room->Tr + Sd->alir * Sd->Tmrt + Sd->RS) / Sd->ali;
		
		if ( DEBUG )
		{
			printf("----- RMwlt i=%d room=%s ble=%c %s  Tie=%lf Tee=%lf\n",
				i, Sd->room->name, Sd->ble, Sd->name, Tie, Tee);
		}
		
		if (Sd->rpnl != NULL)
			WTp = Sd->rpnl->Wp * Sd->rpnl->Tpi;
		else
			WTp = 0.0;
		
		// 壁体表面、壁体内部温度の計算
		Twall (Mw->M, Mw->mp, Mw->UX, Mw->uo, Mw->um, Mw->Pc,
			Tie, Tee, WTp, Mw->Told, Mw->Tw, Sd, Mw->wall->PCMLyr);

		// 壁体表面温度、壁体内部温度の更新
		int m;
		double	*Told, *Tw, *Twd, *Toldd;
		Told = Mw->Told;
		Tw = Mw->Tw;
		Twd = Mw->Twd;
		Toldd = Mw->Toldd;
		for (m = 0; m < Mw->M; m++, Tw++, Told++, Twd++, Toldd++)
		{
			// 前時刻の壁体内部温度を更新
			*Told = *Tw;
			// 収束過程初期値の壁体内部温度を更新
			*Twd = *Tw;
			*Toldd = *Tw;
		}

		//// 建材一体型空気集熱器がある場合には、相当外気温度を計算する
		//if (Mw->wall->ta > 0.)
		//{
		//	Sd->ColTe = Mw->Tw[Mw->wall->mp] ;

		//	printf ( "ColTe=%lf\n", Sd->ColTe ) ;
		//}
	}
}

// 壁体内部温度の仮計算
void RMwltd(int Nmwall, MWALL *Mw)
{
	RMSRF *Sd, *nxsd;
	ROOM  *Room;
	int i;
	double Tie, Tee, WTp;

	for (i = 0; i < Nmwall; i++, Mw++)
	{
		Sd = Mw->sd;
		nxsd = Sd->nxsd;
		Room = Sd->room;

		if (Sd->PCMflg == 'Y')
		{
			if (Sd->mwtype == 'C')
				Tee = (nxsd->alic * nxsd->room->Tr + nxsd->alir * nxsd->Tmrt + nxsd->RS) /
				nxsd->ali;
			else
				Tee = Sd->Te;

			Tie = (Sd->alic * Room->Tr + Sd->alir * Sd->Tmrt + Sd->RS) / Sd->ali;

			if (DEBUG)
			{
				printf("----- RMwlt i=%d room=%s ble=%c %s  Tie=%lf Tee=%lf\n",
					i, Sd->room->name, Sd->ble, Sd->name, Tie, Tee);
			}

			if (Sd->rpnl != NULL)
				WTp = Sd->rpnl->Wp * Sd->rpnl->Tpi;
			else
				WTp = 0.0;

			// 壁体内部温度の仮計算
			Twalld(Mw->M, Mw->mp, Mw->UX, Mw->uo, Mw->um, Mw->Pc,
				Tie, Tee, WTp, Mw->Told, Mw->Twd, Sd);

			//// 建材一体型空気集熱器がある場合には、相当外気温度を計算する
			//if (Mw->wall->ta > 0.)
			//{
			//	Sd->ColTe = Mw->Tw[Mw->wall->mp] ;

			//	printf ( "ColTe=%lf\n", Sd->ColTe ) ;
			//}
		}
	}
}

/* ------------------------------------------------------ */

double  RTsav (int N, RMSRF *Sd)
{ 
	double  Tav, Aroom;
	int     n;
	
	Tav = Aroom = 0.0;  
	for (n = 0; n < N; n++, Sd++)
	{
		Tav += Sd->Ts * Sd->A;
		Aroom += Sd->A;
	}
	return (Tav / Aroom);
}
