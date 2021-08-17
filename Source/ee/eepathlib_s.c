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

/*  ee_pathlib_s.c   */

/* 経路の定義用関数 */


#include <stdio.h>
#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"

/* ----------------------------------------------- */

/*  システム要素出力端割当  */

void pelmco(char pflow, PELM *Pelm, char *errkey)
{
	int		Nout, i, ii, err=0; 
	COMPNT	*cmp;
	ELOUT	*elmo;
	STANK	*stank;
	char	Err[SCHAR];
	//THEX	*Thex ;
	//EVAC	*Evac ;
	//int		N ;

	cmp = Pelm->cmp;
	//   Pelm->out = NULL ;
	elmo = cmp->elouts;
	Nout = cmp->Nout;

	if (strcmp(cmp->eqptype, DIVERG_TYPE) == 0 ||
		strcmp(cmp->eqptype, DIVGAIR_TYPE) == 0)
	{
		for (i = 0; i < Nout; i++, elmo++)
		{
			if (elmo->id != '*')
			{
				Pelm->co = elmo->id = '*';
				Pelm->out = elmo;
				err = 0;
				break;
			}
		}
	}
	else if (strcmp(cmp->eqptype, STANK_TYPE) == 0)
	{
		stank = (STANK *)cmp->eqp;
		for (i = 0; i < stank->Nin; i++)
		{
			if (stank->pthcon[i] == Pelm->co)
				break;
		}
		ii = i;
		for (i = 0; i < Nout; i++, elmo++)
		{
			if (elmo->id == stank->pthcon[ii])
			{
				Pelm->out = elmo;
				err = 0;
				break;
			}
		}
	}
	/*****************
	else if (strcmp(cmp->eqptype, THEX_TYPE) == 0)
	{
	Thex = (THEX *) cmp->eqp ;
	for (i = 0; i < 4; i++, elmo++)
	{
	if (e
	}
	*********************************/
	else if (Nout == 1)
	{
		Pelm->out = elmo;
		Pelm->co = elmo->id;
	}

	else if (strcmp(cmp->eqptype, RDPANEL_TYPE) == 0)
	{
		if (pflow == WATER_FLD || pflow == AIRa_FLD)
		{
			Pelm->out = elmo;
			Pelm->co = elmo->id;
		}
		else if (pflow == AIRx_FLD)
		{
			elmo++;
			Pelm->out = elmo;
			Pelm->co = elmo->id;
		}
	}
	else
	{  
		err=1;
		for (elmo = cmp->elouts; elmo < (cmp->elouts) + Nout; elmo++)
		{	
			if (Pelm->co == elmo->id)
			{
				Pelm->out = elmo;
				err=0;
				break;
			} 
		}
	}
	if (err)
	{
		for (elmo = cmp->elouts; elmo < cmp->elouts + Nout; elmo++)
		{

			/**************
			printf("<<pelmco>> cmp->name=%s  Pflow=%c elmo->id=%c\n",
			cmp->name, pflow, elmo->id);
			*****************/

			//if (( strcmp ( cmp->eqptype, THEX_TYPE )) != 0 )
			//{
			if ((pflow == AIRa_FLD && elmo->id == 't')
				|| (pflow == AIRx_FLD && elmo->id == 'x') 
				|| (pflow == WATER_FLD && elmo->id == 'W'))
			{
				Pelm->out = elmo ;
				Pelm->co = elmo->id ;
				err=0;
				break;
			}
			//}
		}
	} 

	// Satoh追加　気化冷却器　2013/10/31
	if (err)
	{
		if(strcmp(cmp->eqptype, EVAC_TYPE) == 0)
			Nout = 4 ;

		for (elmo = cmp->elouts; elmo < (cmp->elouts) + Nout; elmo++)
		{
			if ( Pelm->co == elmo->id || (Pelm->co == 'W' && elmo->id == 'V') || (Pelm->co == 'w' && elmo->id == 'v'))
			{
				Pelm->out = elmo ;
				err = 0 ;
				break ;
			}
		}
	}

	strcpy_s(Err, sizeof(Err), errkey);
	strcat_s(Err, sizeof(Err), " <pelmco>");
	Errprint(err, Err, cmp->name); 
}
/* ----------------------------------------------- */

/*  システム要素入力端割当  */

void pelmci(char pflow, PELM *Pelm, char *errkey)
{
	int  Nin, i, err=0, N;
	COMPNT *cmp;
	ELIN   *elmi;
	ELOUT  *Elout, *Eo ;

	ROOM   *room;
	HCLOAD *Hcload;
	THEX	*Thex ;

	EVAC	*Evac ;

	char Err[SCHAR]; 

	cmp = Pelm->cmp;
	elmi = cmp->elins ;
	Nin = cmp->Nin;

	if (Nin <= 0)
		return;

	else if (strcmp(cmp->eqptype, CONVRG_TYPE) == 0 || 
		strcmp(cmp->eqptype, CVRGAIR_TYPE) == 0)
	{
		for (i = 0; i < Nin; i++, elmi++)
		{
			if (elmi->id != '*')
			{
				Pelm->ci = elmi->id = '*';
				Pelm->in = elmi;
				break;
			}
		}
	}

#if SIMUL_BUILDG   /*****************************/
	else if (strcmp(cmp->eqptype, ROOM_TYPE) == 0)
	{
		room = (ROOM *)cmp->eqp;
		/*************
		if (pflow == AIRa_FLD)
		ii = room->Nachr + room->Nrp;
		else if (pflow == AIRx_FLD)
		ii = Nin - room->Nasup;
		elmi += ii;
		**************/

		if (pflow == AIRa_FLD)
			elmi = room->elinasup;
		else if (pflow == AIRx_FLD)
			elmi = room->elinasupx;


		/***************
		printf("<<pelmci>>  room=%s  ii=%d  Nin=%d\n", room->name, ii, Nin);
		***********************/

		for (i = 0; i < room->Nasup; i++, elmi++)
		{
			if (elmi->id != '*')
			{
				Pelm->ci = elmi->id = '*';
				Pelm->in = elmi;
				break;
			}
		}
	}
	else if (strcmp(cmp->eqptype, RDPANEL_TYPE) == 0)
	{

		Elout = cmp->elouts;
		if (pflow == WATER_FLD || pflow == AIRa_FLD)
		{
			for (i = 0; i < Elout->Ni; i++, elmi++)
			{
				if (elmi->id == 'f')
				{
					Pelm->ci = elmi->id;
					Pelm->in = elmi;
					break;
				}
			}
		}
		else if (pflow == AIRx_FLD)
		{
			Elout++;
			Pelm->ci = Elout->elins->id;
			Pelm->in = Elout->elins;
		}
	} 
#endif /*************************************/

	else if (Nin == 1)
	{
		Pelm->in = elmi;
		Pelm->ci = elmi->id;
	}

	else
	{
		err=1;

		for (elmi = cmp->elins; elmi <( cmp->elins) + Nin; elmi++)
		{
			/*************
			printf("------pelmci  %s  ci=%c  i=%d\n",
			cmp->name, elmi->id, elmi - cmp->elins);
			/********************/
			// ACの絶対湿度はここに入った
			if (Pelm->ci == elmi->id)
			{ 

				Pelm->in = elmi, err=0;
				break;
			}
		}
		/**************
		printf("-----pelmci   %s  Nin=%d   err=%d\n", cmp->name, Nin, err);
		****************/

	}
	if (err)
	{

		if (strcmp(cmp->eqptype, HCLOADW_TYPE) == 0)
		{
			Hcload = cmp->eqp;
			if(Hcload->wet == 'y')
				Nin = 4;
		}
		else if (strcmp(cmp->eqptype, DESI_TYPE) == 0)
			Nin = 4 ;

		for (elmi = cmp->elins; elmi < (cmp->elins) + Nin; elmi++)
		{
			//printf("name=%s pflow=%c id=%c\n", cmp->name, pflow, elmi->id) ;
			if ((pflow == AIRa_FLD && elmi->id == 't')
				|| (pflow == AIRx_FLD && elmi->id == 'x')
				|| (pflow == WATER_FLD && elmi->id == 'W'))
			{
				//if(elmi->upo != NULL)
				//	printf("name=%s upo=%s\n", cmp->name, elmi->upo->cmp->name) ;
				//else
				//	printf("name=%s upo=NULL\n", cmp->name) ;
				Pelm->in = elmi,  Pelm->ci = elmi->id,  err=0;
				break;
			}
		}

		//printf("\n") ;
	}      

	if (err)
	{

		if (strcmp(cmp->eqptype, THEX_TYPE) == 0)
		{
			Thex = cmp->eqp;
			//if(Hcload->wet == 'y')
			N = 0 ;
			Eo = cmp->elouts ;
			for ( i = 0; i < cmp->Nout; i++, Eo++ )
				N += Eo->Ni ;

			Nin = N ;
		}
		//else if (strcmp(cmp->eqptype, EVAC_TYPE) == 0)
		//{
		//	Evac = cmp->eqp;
		//	N = 0 ;
		//	Eo = cmp->elouts ;
		//	for ( i = 0; i < cmp->Nout; i++, Eo++ )
		//		N += Eo->Ni ;

		//	Nin = N ;
		//}

		for (elmi = cmp->elins; elmi < (cmp->elins) + Nin; elmi++)
		{
			/*************/
			//printf("------pelmci  %s  ci=%c  i=%d\n",
			//cmp->name, elmi->id, elmi - cmp->elins);
			/********************/
			if ( Pelm->ci == elmi->id )
			{
				Pelm->in = elmi ;
				err=0;
				break;
			}
		}
	}
	if(err)
	{
		if(strcmp(cmp->eqptype, EVAC_TYPE) == 0)
		{
			Evac = cmp->eqp;
			N = 0 ;
			Eo = cmp->elouts ;
			for ( i = 0; i < cmp->Nout; i++, Eo++ )
				N += Eo->Ni ;

			Nin = N ;
		}

		for (elmi = cmp->elins; elmi < (cmp->elins) + Nin; elmi++)
		{
			/*************/
			//printf("------pelmci  %s  ci=%c  i=%d\n",
			//cmp->name, elmi->id, elmi - cmp->elins);
			/********************/
			if ( Pelm->ci == elmi->id || (Pelm->ci == 'W' && elmi->id == 'V') || (Pelm->ci == 'w' && elmi->id == 'v'))
			{
				Pelm->in = elmi ;
				err=0;
				break;
			}
		}
	}

	strcpy_s(Err, sizeof(Err), errkey);
	strcat_s(Err, sizeof(Err), " <pelmci>");
	Errprint(err, Err, cmp->name);
}
/* ----------------------------------------------- */

/* システム要素接続データのコピー（空気系統湿度経路用） */

void plistcpy(MPATH *Mpath, int *Npelm, PELM *Pelm, PLIST *Plist,
			  int Ncompnt, COMPNT *Compnt)     
{
	MPATH  *mpi;
	PLIST  *pli;
	PELM   *peli;
	COMPNT *cmp;
	int    i, j, nelm;
	char   s[SCHAR], *st;

	mpi = Mpath - 1;

	mpi->mpair = Mpath;

	strcpy_s ( s, sizeof(s), mpi->name ) ;
	strcat_s ( s, sizeof(s), ".x" ) ;
	Mpath->name = stralloc ( s ) ;

	/******************
	Mpath->name = stralloc(mpi->name);
	srealloc ( Mpath->name, strlen ( Mpath->name ) + 2 + 1 ) ;
	strcat(Mpath->name, ".x");
	/**********************/

	Mpath->Nlpath = mpi->Nlpath;
	Mpath->plist = Plist;
	mpi->fluid = AIRa_FLD;
	Mpath->fluid = AIRx_FLD;
	Mpath->G0 = mpi->G0 ;
	Mpath->rate = mpi->rate ;

	pli = mpi->plist;

	/*****
	Pelm += *Npelm;*************/

	for (i = 0; i < mpi->Nlpath; i++, pli++, Plist++)
	{   
		pli->lpair = Plist;
		pli->plistx = Plist ;
		Plist->plistt = pli ;

		peli = pli->pelm;
		nelm = 0;
		Plist->pelm = NULL;
		Plist->org = 'n';	 
		Plist->type = pli->type ;
		Plist->Go = pli->Go ;
		Plist->Nvav = pli->Nvav ;
		Plist->Nvalv = pli->Nvalv ;
		Plist->NOMVAV = pli->NOMVAV ;
		Plist->OMvav = pli->OMvav ;
		Plist->valv = pli->valv ;
		Plist->rate = pli->rate ;
		//Plist->Npump = pli->Npump ;
		//Plist->Pump = pli->Pump ;
		Plist->UnknownFlow = pli->UnknownFlow ;

		if (pli->name != NULL)
		{
			strcpy_s ( s, sizeof(s), pli->name ) ;
			strcat_s ( s, sizeof(s), ".x" ) ;
			Plist->name = stralloc ( s ) ;

			/***************************
			Plist->name = stralloc(pli->name);
			strcat(Plist->name, ".x");
			/*****************************/
		}
		else
			Plist->name = stralloc(".x");

		for (j = 0; j < pli->Nelm; j++, peli++)
		{
			if (peli->cmp->airpathcpy == 'y')
			{
				if (Plist->pelm == NULL)
					Plist->pelm = Pelm;
				//if (Pelm->Pelmx == NULL)
				//Pelm->Pelmx = Pelm ;

				(*Npelm)++;

				if (strcmp(peli->cmp->eqptype, CVRGAIR_TYPE) == 0
					|| strcmp(peli->cmp->eqptype, DIVGAIR_TYPE) == 0)
				{
					for (cmp = peli->cmp + 1; cmp < Compnt + Ncompnt; cmp++)
					{
						strcpy_s(s, sizeof(s), cmp->name);
						if ((st = strchr(s, '.') )!= NULL)
						{
							*st = '\0';
							if (strcmp(peli->cmp->name, s) == 0)
								break;
						}
					}
					Pelm->cmp = cmp;
				}
				else if (strcmp(peli->cmp->eqptype, THEX_TYPE) == 0)
				{
					Pelm->cmp = peli->cmp ;
					if ( peli->ci == 'E' )
						Pelm->ci = Pelm->co = 'e' ;
					else
						Pelm->ci = Pelm->co = 'o' ;
				}
				// Satoh追加　気化冷却器　2013/10/31
				else if(strcmp(peli->cmp->eqptype, EVAC_TYPE) == 0)
				{
					Pelm->cmp = peli->cmp ;
					if(peli->ci == 'D')
						Pelm->ci = Pelm->co = 'd' ;
					else if(peli->ci == 'W')
						Pelm->ci = Pelm->co = 'w' ;
				}
				//else if (strcmp(peli->cmp->eqptype, DESI_TYPE) == 0)
				//{
					//Pelm->cmp = peli->cmp;
					//if ( peli->ci == 't')
					//	Pelm->ci = Pelm->co = 'x' ;
					//else
					//	Pelm->ci = Pelm->co = 't' ;
				//}
				else
				{
					Pelm->cmp = peli->cmp;	
					//					Pelm->out = peli->out ;
					Pelm->ci = peli->ci;
					Pelm->co = peli->co;
				}

				Pelm->out = peli->out ;
				Pelm++ ;
				nelm++;
			}
		}
		Plist->Nelm = nelm;	 
	}
}

/* ----------------------------------------------- */

/*  合流レベルの設定  */

void plevel(int Nmpath, MPATH *Mpath, int Ncnvrg, COMPNT **Cnvrg)
{
	int    i, j, lvc=0,lvcmx, lvcf;
	PLIST  *Plist;
	COMPNT *cmp;
	ELIN   *elin;

	for (i = 0; i < Ncnvrg; i++)
	{
		cmp = Cnvrg[i];
		cmp->elouts->lpath->lvc = -1;
	}

	lvcf = Ncnvrg;

	/*********************
	printf ( "lvcf=%d\n", lvcf ) ;
	**********************/

	while (lvcf)
	{
		for (i = 0; i < Ncnvrg; i++)
		{
			cmp = Cnvrg[i];
			if (cmp->elouts->lpath->lvc <= 0)
			{
				elin = cmp->elins;
				for (j = 0; j < cmp->Nin; j++, elin++)
				{
					Plist = elin->lpath;
					if (Plist->type != CONVRG_LPTP)
						lvc = 0;
					else
					{
						if (Plist->lvc > 0)
							lvc = (lvc > Plist->lvc ? lvc : Plist->lvc);
						else
							break;
					}
				}
				if (j == cmp->Nin)
				{
					lvc++;
					cmp->elouts->lpath->lvc = lvc;

					lvcf--;
				}
			}
		}
		/*******************
		printf ( "lvcf=%d\n", lvcf ) ;
		********************/
	}
	for (i = 0; i < Nmpath; i++, Mpath++)
	{
		lvcmx = 0;
		Plist = Mpath->plist;
		for (j = 0; j < Mpath->Nlpath; j++, Plist++)
		{
			if (Plist->lvc > lvcmx)
				lvcmx = Plist->lvc; 
		}
		Mpath->lvcmx = lvcmx;
	}
}

/* ----------------------------------------------- */

void	pflowstrct ( int Nmpath, MPATH *Mpath )
{
	int	  m, i, j, n, M, MM, k ;
	PLIST  *Plist ;
	char   *etype;
	ELOUT  *Elout ;
	ELIN	  *Elin ;
	int		Ncb ;

	for ( m = 0; m < Nmpath; m++, Mpath++ )
	{
		/***************************************
		printf ( "<pflowstrct> Name=%s\n", Mpath->name ) ;
		/**********************************************/

		n = 0 ;
		Plist = Mpath->plist ;

		for ( i = 0; i < Mpath->Nlpath; i++, Plist++ )
		{
			// 流量未設定の末端経路を検索
			if ( Plist->Go == NULL 
				&& Plist->Nvav == 0
				//&& ( Plist->Npump == 0
				//|| ( Plist->Npump > 0 && ( strcmp(Plist->Pump->cat->type, PUMP_C) != 0
				//&& strcmp(Plist->Pump->cat->type, FAN_C) != 0)))
				&& Plist->rate == NULL && Plist->NOMVAV == 0 
				&& (Plist->Nvalv == 0 || (Plist->Nvalv > 0 && Plist->valv->monPlist == NULL && Plist->valv->MGo == NULL )))
			{
				Mpath->pl[n] = Plist ;
				Plist->n = n ;
				// 末端経路未知フラグの変更
				Plist->UnknownFlow = 0 ;
				n++ ;
			}

			if (Mpath->rate == 'Y' 
				&& (Plist->Go != NULL || Plist->Nvav > 0 || Plist->Nvalv > 0 || Plist->NOMVAV > 0))
				Mpath->G0 = &Plist->G ;		// 流量比率設定時の既知流量へのポインタをセット

			/**************************************
			printf ("i=%d  n=%d\n", i, Plist->n) ;
			/********************************************/
		}

		Mpath->NGv = n ;
		Mpath->NGv2 = n * n ;

		n = 0 ;
		Ncb = 0 ;

		Plist = Mpath->plist ;

		for ( i = 0; i < Mpath->Nlpath; i++, Plist++ )
		{
			/*if ( n < Mpath->NGv )
			{*/
			// 末端経路の先頭機器
			etype = Plist->pelm->cmp->eqptype ;

			// 末端経路の先頭が分岐・合流の場合
			if ( strcmp ( etype, CONVRG_TYPE ) == 0
				|| strcmp ( etype, CVRGAIR_TYPE ) == 0
				|| strcmp ( etype, DIVERG_TYPE ) == 0
				|| strcmp ( etype, DIVGAIR_TYPE ) == 0 )
			{
				MM = 0 ;

				for ( k = 0; k < n; k++ )
				{
					/************************
					printf ( "///// k=%d n=%d  %s\t%s\n", k, n,
					Plist->pelm->cmp->name, Mpath->cbcmp[k]->name ) ;
					/**********************************/

					if ( Plist->pelm->cmp == Mpath->cbcmp[k] )
					{
						MM++ ;
						break ;
					}
				}

				if ( MM == 0 )
				{
					// 末端経路の先頭機器（分岐・合流）の入口、出口経路の未知流量の数(M)を数える
					M = 0 ;

					// 末端経路の先頭機器の入口
					Elin = Plist->pelm->cmp->elins ;

					for ( j = 0; j < Plist->pelm->cmp->Nin; j++, Elin++ )
					{
						Plist->upplist = Elin->lpath ;
						// 末端経路の先頭機器の上流の流量が未定義
						//if ( Elin->lpath->Go == NULL )
						if ( Elin->lpath->UnknownFlow == 0 )
						{
							M++ ;
							break ;
						}

						//Elin++ ;
					}

					if ( M == 0 )
					{
						// 末端経路の先頭機器の出口
						Elout = Plist->pelm->cmp->elouts ;

						for ( j = 0; j < Plist->pelm->cmp->Nout; j++, Elout++ )
						{
							Plist->dnplist = Elout->lpath ;
							//if ( Elout->lpath->Go == NULL )
							// 末端経路の先頭機器の出口経路の流量が未知なら
							if ( Elout->lpath->UnknownFlow == 0 )
							{
								M++ ;
								break ;
							}

							//Elout++ ;
						}
					}

					// 末端経路先頭にある分岐・合流の入口もしくは出口経路の未知流量の数(M)
					if ( M > 0 )
					{
						// 流量連立方程式を解くときに使用する分岐・合流機器と数(n)
						Mpath->cbcmp[n] = Plist->pelm->cmp ;
						/******
						printf ("<pflowstrct> M=%d  n=%d  Name=%s\n",
						M, n, Mpath->cbcmp[n]->name ) ;
						/****************/

						n++ ;

						//							if ( n > CBCMPMX )
						//								printf ( "<pflowstrct> n=%d [max=%d]\n", n, CBCMPMX ) ;
					}
				}
			}
			//}
		}

		/*if ( n != Mpath->NGv )
		{
		sprintf_s ( Err, sizeof(Err), "xxxx  Nundflow=%d  Ncbeq=%d  Name=%s\n",
		n, m, Mpath->name ) ;
		Eprint ( "<pflowstrct>", Err ) ;
		}*/

		// 既知末端流量数のチェック
		if (n>0 && (n - 1 != Mpath->NGv))
			printf("<%s> 末端流量の与えすぎ、もしくは少なすぎです n=%d NGv=%d\n", Mpath->name, n, Mpath->NGv ) ;
	}
}
