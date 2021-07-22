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

/*  bl_roomvar.c  */

#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"
#include "fnesy.h"
#include "fnmcs.h"

/* ------------------------------------- */

/* 室間換気、放射パネルとシステム入力要素への割り付け */

void Roomelm(int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl) 
{
	COMPNT *compnt, *cmp;
	ROOM   *room;
	TRNX   *trnx;
	RPANEL *rmpnl;
	ACHIR  *achr;
	ELIN   *elin, *elinx, *elip;
	int    n, i, m;
//	double	*B ;

//	B = dcalloc ( 100, "<Roomelm>" ) ;
	
	room = Room;
	
	for (n = 0; n < Nroom; n++, room++)
	{
		compnt = room->cmp;
		elin = compnt->elins;
		elinx = elin + compnt->elouts->Ni;
		achr = room->achr;
		
		/*******************
		printf("xxxxx Roomelm 1  n=%d %s  Nachr=%d\n", n, room->name, room->Nachr);
		/***************/
		
		for (i = 0; i < room->Nachr; i++, achr++, elin++, elinx++)
		{
			cmp = achr->room->cmp;
			
			/************************
			printf("-----Roomelm i=%d   achr->room=%s  achr->room->cmp=%s\n",
			i, achr->room->name, achr->room->cmp->name);
			************************/
			
			elin->upo = elin->upv = cmp->elouts;
			elinx->upo = elinx->upv =cmp->elouts + 1;
			//elin++;
			//elinx++;
			/***achr++***/
		}
		
		/*************
		printf("xxxxx Roomelm 2  n=%d %s Ntr=%d\n", n, room->name, room->Ntr);
		*****************/
		
		trnx = room->trnx;
		for (i = 0; i < room->Ntr; i++, trnx++, elin++)
		{
			//printf("i=%d Room=%s\n", i, room->name ) ;
		/***************
		printf("----- Roomelm  i=%d nextroom=%s\n", i, trnx->nextroom->name);
			/***********************/
			
			cmp = trnx->nextroom->cmp;
			elin->upo = elin->upv = cmp->elouts;
			//elin++;
		}
		
		/***************
		printf("xxxxx Roomelm 3  n=%d  Nrp=%d\n", n, room->Nrp);
		*********************/
		
		rmpnl = room->rmpnl;
		for (i = 0; i < room->Nrp; i++, rmpnl++, elin++)
		{
		/******************
		printf("----- Roomelm  i=%d name=%s\n", i, rmpnl->pnl->name);
			/************************/
			
			elip = rmpnl->pnl->cmp->elins;
			
			/********************
			printf("Roomelm  ***** panel=%s elip=%d\n",
			rmpnl->pnl->name, elip - rmpnl->pnl->cmp->elins);
			********************/
			
			/*****
			cmp = rmpnl->pnl->cmp;****/
			
			elin->upo = elip->upo;
			
			/*****************
			printf("xxxxx Roomelm  panel i=%d room=%s pnl=%s upo=%s\n", 
			i, room->name, rmpnl->pnl->name, elip->upo->cmp->name);
			********************/
			
			/*****elip++, elin++; *****/
			
			//elin++;
			
			/****
			elip++;
			elip->upo = elip->upv = compnt->elouts;
			*****/
		}
		
		/***************
		printf("xxxxx Roomelm 4 n=%d\n", n);
		********************/
		
	}
	
	/****************
	printf("xxxxx Roomelm 4 Nrdpnl=%d\n", Nrdpnl);
	******************/
	
	for (n = 0; n < Nrdpnl; n++, Rdpnl++)
	{
		elin = Rdpnl->cmp->elins + 1;
		
		for (m = 0; m < Rdpnl->MC; m++)
		{
			room = Rdpnl->rm[m];
			elin->upo = elin->upv = room->cmp->elouts;
			elin++;
			
			trnx = room->trnx;
			for (i = 0; i < Rdpnl->Ntrm[m]; i++, trnx++, elin++ )
			{
				cmp = trnx->nextroom->cmp;
				elin->upo = elin->upv = cmp->elouts;
				//elin++;
			}
			
		}
	}
}

/* -------------------------------  */

/* 室、放射パネルのシステム方程式作成 */

void Roomvar(int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl)
{
	ACHIR  *achr;
	COMPNT *compnt;
	ELOUT  *elout;
	ELIN   *elin;
	double  *cfin, *a, *epr, *epw;
	int    i, j, m;
	double  cG, G;
	extern double  ca;
	
	for (i = 0; i < Nroom; i++, Room++)
	{
	/********************
	printf ( "<<Roomvar>> Room=%d  Nroom=%d\n", i, Nroom ) ;
		/**********************/
		
		compnt = Room->cmp;
		elout = compnt->elouts;
		elin = compnt->elins;
		
		/***************************
		printf ("RMt=%lf\n", Room->RMt ) ;
		/****************************/
		
		elout->coeffo = Room->RMt;
		elout->Co = Room->RMC;
		cfin = elout->coeffin;
		
		achr = Room->achr;
		for (j = 0; j < Room->Nachr; j++, cfin++, achr++, elin++)
		{
			//	 printf("ccc\n");
			*cfin = -(ca * achr->Gvr);
			//cfin++ ;
			elout->coeffo += (ca * achr->Gvr);
			//achr++, elin++;
		}
		
		/*************
		printf("----- Roomvar  i=%d %s Ntr=%d\n", i,
		Room->name, Room->Ntr); /**************/
		a = Room->ARN;
		for (j = 0; j < Room->Ntr; j++, a++, cfin++, elin++)
		{
		/************
			printf("----- Roomvar  ARN[%d]=%lf\n", j, *a); /**********/
			
			*cfin = -(*a);
			//cfin++ ;
			//elin++;
		}
		a = Room->RMP;
		for (j = 0; j < Room->Nrp; j++, a++, cfin++, elin++)
		{
			*cfin = -(*a);
			//cfin++ ;
			//elin++;
		}
		
		/********
		if (Room->Nrp)
		{
		*cfin = Room->RMw;
			cfin++ ;
		elin++;
		}
		**********/
		
		for (j = 0; j < Room->Nasup; j++, cfin++, elin++)
		{
			*cfin = -(ca * elin->lpath->G);
			//cfin++ ;
			elout->coeffo += (ca * elin->lpath->G);	
			//elin++;
		}
		
		elout++;
		elout->coeffo = Room->RMx;
		elout->Co = Room->RMXC;
		
		cfin = elout->coeffin;
		
		achr = Room->achr;
		for (j = 0; j < Room->Nachr; j++, cfin++, elin++, achr++)
		{
			*cfin = -(achr->Gvr);
			//cfin++ ;
			elout->coeffo += achr->Gvr;
			//achr++, elin++;
		}
		for (j = 0; j < Room->Nasup; j++, cfin++, elin++)
		{
			*cfin = -(elin->lpath->G);
			//cfin++ ;
			elout->coeffo += elin->lpath->G;
			//elin++;
		}
	}
	/*********
	printf("-----Roomvar Nrdpnl=%d\n", Nrdpnl); ***************/
	
	for (i = 0; i < Nrdpnl; i++, Rdpnl++)
	{
		compnt = Rdpnl->cmp;
		G = compnt->elouts->lpath->G;
		cG = spcheat(compnt->elouts->fluid) * G;
		compnt->elouts->coeffo = cG;
		compnt->elouts->Co = Rdpnl->EPC;
		
		/*********
		printf("-----Roomvar  i=%d %s G=%lf coeffo=%lf Co=%lf\n",
		i, compnt->name, G, compnt->elouts->coeffo, compnt->elouts->Co );
		/************/
		
		cfin = compnt->elouts->coeffin;
		if (Rdpnl->sd[0]->mw->wall->WallType == 'P' )		// 通常の床暖房パネル
			*cfin = Rdpnl->Epw - cG;
		else												// 屋根一体型空気集熱器
			*cfin = - Rdpnl->Epw ;
		//printf ("cfin=%lf\n", *cfin) ;
		cfin++ ;
		
		/**********
		*cfin = -Rdpnl->EPt;
		**********/
		for (m = 0; m < Rdpnl->MC; m++)
		{ 
			*cfin = -Rdpnl->EPt[m];
			//printf ("cfin=%lf\n", *cfin) ;
			cfin++ ;
			epr = Rdpnl->EPR[m];
			epw = Rdpnl->EPW[m];
			for (j = 0; j <Rdpnl->Ntrm[m]; j++, epr++, cfin++)
			{
				*cfin = -(*epr);
				//printf ("cfin=%lf\n", *cfin) ;
				//cfin++ ;
			}
			
			/****
			cfin = *epw - cG;
			cfin++ ;
			***/
			
			for (j = 0; j < Rdpnl->Nrp[m]; j++, epw++, cfin++)
			{
				*cfin = -(*epw);
				//printf ("cfin=%lf\n", *cfin) ;
				//cfin++ ;
			}
		}
		
		
		/* 空気系統湿度計算用ダミー */
		elout = compnt->elouts;
		elout++;
		elout->coeffo = G;
		elout->Co = 0.0;
		cfin = elout->coeffin;
		*cfin = -G;
	}
}
