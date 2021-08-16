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

/* ee_syselmalc_s.c    */

/* 機器使用データの割り付けおよびシステム要素から入力、出力要素の割り付け */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fesy.h"
#include "fnesy.h"
#include "fnmcs.h"
#include "fnlib.h"
#include "fnfio.h"


void Elmalloc(char *errkey, 
			  int Ncompnt, COMPNT *Compnt, EQCAT *Eqcat, EQSYS *Eqsys, 
			  ELOUT **Elo, int *Nelout, ELIN **Eli, int *Nelin)
{
	COMPNT	*cmp ;
	ELOUT	*elmo, *elop, *elo, *eloc ;
	ELIN	*elmi, *elic ;
	ROOM	*room;
	RDPNL	*rdpnl;
	//AIRFLOW	*airflow ;
	HCC		*Hcc;
	BOI		*Boi;
	REFA	*Refa;
	COLL	*Coll;
	PIPE	*Pipe; 
	STANK	*Stank;
	HEX		*Hex;
	PUMP	*Pump;
	COMPNT	**Cnvrg;
	FLIN	*Flin;
	HCLOAD	*Hcload;
	STHEAT	*Stheat ;
	ELOUT	*Elout ;
	ELIN	*Elin ;
	THEX	*Thex ;
	VALV	*Valv ;
	QMEAS	*Qmeas ;
	PV		*PV ;
	DESI	*Desi ;
	EVAC	*Evac ;

	/*---- Satoh Debug VAV  2000/12/5 ----*/
	VAV		*VAV ;

	OMVAV	*OMvav ;

	int		i, ii, m, mm, neqp, ncat, icv = 0;
	char	*c, *name;
	static char *idmrkc="txW";
	//static char *idmrkthex = "txtx" ;
	char	*id ;
	int		Nout, Nin ;
	int		Nvalv = 0, NQmeas = 0, NOMvav=0 ;
	char	*idTe = "EO", *idTo = "OE" ;
	char	*idxe, *idxo, *idd ;

	cmp = Compnt ;
	Elcount ( Ncompnt, Compnt, &Nout, &Nin ) ;

	if ( Nout > 0 )
	{
		if (( *Elo = ( ELOUT * ) malloc ( Nout * sizeof ( ELOUT ))) == NULL )
			Ercalloc ( Nout, "<Elmalloc>  Elout alloc" ) ;
		Eloutinit ( *Elo, Nout ) ;
	}

	if ( Nin > 0 )
	{
		if (( *Eli = ( ELIN * ) malloc ( Nin * sizeof ( ELIN ))) == NULL )
			Ercalloc ( Nin, "<Elmalloc>  Elin alloc" ) ;
		Elininit ( Nin, *Eli ) ;
	}

	//	printf ( "<Elmalloc> Elout=%d  Elin=%d\n", Nout, Nin ) ;
	Elout = *Elo ;
	Elin = *Eli ;
	elmo = Elout;
	elmi = Elin;

	Cnvrg = Eqsys->Cnvrg;
	Flin = Eqsys->Flin;
	Hcload = Eqsys->Hcload;

	for (m = 0; m < Ncompnt; m++, Compnt++) 
	{
		if (strcmp(Compnt->eqptype, PV_TYPE) != 0)
		{
			eloc = Elout ;
			elic = Elin ;
			Compnt->elouts = Elout;
			Compnt->elins = Elin;
		}

		name = Compnt->name;
		neqp = Compnt->neqp;
		ncat = Compnt->ncat;

		//printf ( "m=%3d mMax=%3d name=%s\n", m, Ncompnt, name ) ;
		c = Compnt->eqptype;  

		if (strcmp(c, ROOM_TYPE) == 0)
#if SIMUL_BUILDG  /**************************/
		{
			room = (ROOM *)Compnt->eqp;
			room->cmp = Compnt;

			id = idmrkc ;
			for (i = 0; i < 2; i++, id++, Elout++)
			{ 
				Elout->cmp = Compnt;
				Elout->elins = Elin;	
				Elout->id = Elout->fluid = *id ;
				if (i == 0)
					Elout->Ni = room->Nachr + room->Ntr + room->Nrp + room->Nasup;
				else if (i == 1)
					Elout->Ni = room->Nachr + room->Nasup;

				Elin += Elout->Ni ;

				//for (ii=0; ii < Elout->Ni; ii++)
				//	Elin++;

				//Elout++;
			}
			elo = Compnt->elouts;
			room->elinasup = elo->elins + room->Nachr + room->Ntr + room->Nrp;
			elo++;
			room->elinasupx = elo->elins + room->Nachr;
		}
		else if (strcmp(c, RDPANEL_TYPE) == 0)
		{
			rdpnl = (RDPNL *)Compnt->eqp;
			rdpnl->cmp = Compnt;
			rdpnl->Tpi = 15.0 ;
			Elout->cmp = Compnt;
			Elout->elins = Elin;
			Elout->id = 'f';
			Elout->Ni = 1 + 1 + rdpnl->Ntrm[0] + rdpnl->Nrp[0];
			if (rdpnl->MC == 2)
				Elout->Ni += 1 + rdpnl->Ntrm[1] + rdpnl->Nrp[1];

			Elout++;

			Elin->id = 'f';
			Elin++;     

			for (mm = 0; mm < rdpnl->MC; mm++)
			{

				Elin->id = 'r';
				Elin++;
				for (ii = 0; ii < rdpnl->Ntrm[mm]; ii++, Elin++)
				{
					Elin->id = 'r';
					//Elin++;
				}
				for (ii = 0; ii < rdpnl->Nrp[mm]; ii++, Elin++)
				{
					Elin->id = 'f';
					//Elin++;
				}
			}


			/* 空気経路湿度用 */
			Elout->Ni = 1;
			Elout->cmp = Compnt;
			Elout->elins = Elin;
			Elout->id = Elin->id = 'x';
			Elout++;
			Elin++;
		}
		//else if (strcmp(c, AIRFLOW_TYPE) == 0)
		//{
		//	airflow = (AIRFLOW *)Compnt->eqp ;
		//	airflow->cmp = Compnt ;
		//	//airflow->Tpi = 15.0 ;
		//	Elout->cmp = Compnt ;
		//	Elout->elins = Elin ;
		//	Elout->id = 'f' ;
		//	//Elout->Nin = 1 + 1 ;

		//	Elout++ ;
		//	Elin->id = 'f' ;
		//	Elin++ ;

		//	// 湿度用
		//	Elout->Ni = 1 ;
		//	Elout->cmp = Compnt ;
		//	Elout->elins = Elin ;
		//	Elout->id = Elin->id = 'x' ;
		//	Elout++ ;
		//	Elin++ ;
		//}
#else
			;
#endif /**********************************************/
		else if (strcmp(c, DIVERG_TYPE) == 0 || strcmp(c, DIVGAIR_TYPE) == 0)
		{
			if (strcmp(c, DIVGAIR_TYPE) == 0)
				Compnt->airpathcpy = 'y';
			else
				Compnt->airpathcpy = 'n';

			Compnt->Nin = 1;

			for (i=0; i<Compnt->Nout; i++, Elout++ )
			{
				Elout->cmp = Compnt;

				Elout->Ni = Compnt->Nin;
				Elout->elins = Elin;

				Elout->id = Compnt->ido[i];
				//Elout++;
			}

			Elin->id = 'i';
			Elin++;

		}
		else if (strcmp(c, CONVRG_TYPE) == 0 || strcmp(c, CVRGAIR_TYPE) == 0)
		{
			if (strcmp(c, CVRGAIR_TYPE) == 0)
				Compnt->airpathcpy = 'y';
			else
				Compnt->airpathcpy = 'n';

			Compnt->Nout = 1;

			Cnvrg[icv] = Compnt;
			icv++ ;

			Elout->id = 'o';
			Elout->cmp = Compnt;

			Elout->Ni = Compnt->Nin;
			Elout->elins = Elin;

			Elout++;
			for (i=0; i<Compnt->Nin; i++, Elin++)
			{
				Elin->id = Compnt->idi[i];
				//Elin++;
			}
		}
		else if (strcmp(c, HCCOIL_TYPE) == 0)
		{
			Hcc =Eqsys->Hcc + neqp;
			Compnt->eqp = Hcc;
			Hcc->name = name;
			Hcc->cmp = Compnt;
			Hcc->cat = Eqcat->Hccca + ncat;

			for (i = 0; i < Compnt->Nout; i++, Elout++, Elin++)
			{
				Elout->cmp = Compnt;

				Elout->Ni = Compnt->Nin;
				Elout->elins = Compnt->elins;

				Elout->id = Compnt->ido[i];
				Elin->id = Compnt->idi[i];
				//Elout++;
				//Elin++;
			}
		}  
		else if (strcmp(c, HEXCHANGR_TYPE) == 0)
		{
			Hex = Eqsys->Hex + neqp;
			Compnt->eqp = Hex;
			Hex->name = name;
			Hex->cmp = Compnt;
			Hex->cat = Eqcat->Hexca + ncat;  

			for (i = 0; i < Compnt->Nout; i++, Elout++, Elin++)
			{	
				Elout->cmp = Compnt;

				Elout->Ni = Compnt->Nin;
				Elout->elins = Compnt->elins;

				Elout->id = Compnt->ido[i];
				Elin->id = Compnt->idi[i]; 

				//Elout++;
				//Elin++;
			}
		}

		else if (strcmp(c, BOILER_TYPE) == 0)
		{
			Boi =Eqsys->Boi + neqp;
			Compnt->eqp = Boi;
			Boi->name = name;
			Boi->cmp = Compnt;
			Boi->cat = Eqcat->Boica + ncat;

			Elout->cmp = Compnt;

			Elout->Ni = Compnt->Nin;
			Elout->elins = Elin;

			/****
			Elout->id = Compnt->ido[0];
			Elin->id = Compnt->idi[0];
			*****/
			Elout++;
			Elin++;
		}
		else if (strcmp(c, COLLECTOR_TYPE ) == 0 || strcmp(c, ACOLLECTOR_TYPE ) == 0)
		{
			Coll =Eqsys->Coll + neqp;
			Compnt->eqp = Coll;
			Coll->name = name;
			Coll->cmp = Compnt;
			Coll->cat = Eqcat->Collca + ncat;
			Coll->Ac = Compnt->Ac ;

			if (Coll->cat->type == COLLECTOR_PDT)
			{
				Elout->cmp = Compnt;

				Elout->Ni = 1;
				Elout->elins = Elin;

				Elout++;
				Elin++;
			}
			else
			{
				id = idmrkc ;
				for (i = 0; i < Compnt->Nout; i++, id++, Elout++, Elin++)
				{
					Elout->cmp = Compnt;
					Elout->id = Elin->id = *id ;
					Elout->elins = Elin;
					Elout->Ni = 1;
				}
			}
		}
		else if (strcmp(c, PV_TYPE) == 0)
		{
			PV = Eqsys->PVcmp + neqp;
			Compnt->eqp = PV;
			PV->name = name;
			PV->cmp = Compnt;
			PV->cat = Eqcat->PVca + ncat;
			PV->PVcap = Compnt->PVcap ;
			PV->Area = Compnt->Area ;
		}
		else if (strcmp(c, REFACOMP_TYPE) == 0)
		{
			Refa =Eqsys->Refa + neqp;
			Compnt->eqp = Refa;
			Refa->name = name;
			Refa->cmp = Compnt;
			Refa->cat = Eqcat->Refaca + ncat;

			/******
			for (i=0; i<Compnt->Nout; i++)
			{
			Elout->cmp = Compnt;
			Elout->id = Compnt->ido[i];
			Elin->id = Compnt->idi[i];
			Elout++, Elin++;
			}*****/

			Elout->cmp = Compnt;

			Elout->Ni = Compnt->Nin;
			Elout->elins = Elin;

			Elout++, Elin++;
		}
		else if (strcmp(c, PUMP_TYPE ) == 0)
		{
			Pump =Eqsys->Pump + neqp;
			Compnt->eqp = Pump;
			Pump->name = name;
			Pump->cmp = Compnt;
			Pump->cat = Eqcat->Pumpca + ncat;

			if ( Pump->cat->pftype == PUMP_PF )
			{
				Elout->cmp = Compnt;
				Elout->elins = Elin;
				Elout->Ni = 1;
				Elout++;
				Elin++;
			}
			else
			{
				id = idmrkc ;
				for (i = 0; i < Compnt->Nout; i++, id++, Elout++, Elin++ )
				{
					Elout->cmp = Compnt;
					Elout->id = Elin->id = *id ;
					Elout->elins = Elin;
					Elout->Ni = 1;
					//Elout++;
					//Elin++;
				}
			}
		}

		/***********
		else if (strcmp(c, FANPUMP_TYPE) == 0)
		{
		Elout->cmp = Compnt;
		Elout->id = 'o', Elin->id = 'i';
		Elout++;
		Elin++;
		} 
		**************/

		else if (strcmp(c, PIPEDUCT_TYPE) == 0)
		{
			Pipe =Eqsys->Pipe + neqp;
			Compnt->eqp = Pipe;
			Pipe->name = name;
			Pipe->cmp = Compnt;
			Pipe->cat = Eqcat->Pipeca + ncat;

			if (Pipe->cat->type == PIPE_PDT)
			{
				Elout->cmp = Compnt;
				Elout->elins = Elin;
				Elout->Ni = 1;
				Elout++;
				Elin++;
			}
			else
			{
				id = idmrkc ;
				for (i = 0; i < Compnt->Nout; i++, id++, Elout++, Elin++)
				{
					Elout->cmp = Compnt;
					Elout->id = Elin->id = *id ;
					Elout->elins = Elin;
					Elout->Ni = 1;
					//Elout++;
					//Elin++;
				}
			}
		}    
		else if (strcmp(c, STANK_TYPE) == 0)
		{
			Stank = Eqsys->Stank + neqp;
			Compnt->eqp = Stank;
			Stank->name = name;
			Stank->cmp = Compnt;
			Stank->cat = Eqcat->Stankca + ncat;
			/****************
			if (Compnt->tparm == NULL)
			Compnt->tparm = stralloc(Stank->cat->tparm);
			***************/

			Stankmemloc("Stankmemloc", Stank);

			Compnt->Nin = Stank->Nin;
			Compnt->Nout = Stank->Nin;
			Compnt->idi = scalloc(Stank->Nin, errkey);
			Compnt->ido = scalloc(Stank->Nin, errkey);
			/*****
			printf("Elmalc  Ndiv=%d  Nin=%d\n", Stank->Ndiv, Stank->Nin);
			printf("Elmalc  pthcon Jin Jout\n");
			for (i = 0; i < Stank->Nin; i++)
			printf("  %d        %c    %d    %d\n",
			i, Stank->pthcon[i], Stank->Jin[i],  Stank->Jout[i]);
			***********/

			for (i=0; i< Compnt->Nout; i++, Elout++)
			{
				Elout->cmp = Compnt;

				Elout->Ni = Compnt->Nin;
				Elout->elins = Elin;

				Elout->id = Compnt->ido[i] = Stank->pthcon[i];

				//Elout++;
			}
			for (i=0; i< Compnt->Nin; i++, Elin++)
			{
				Elin->id = Compnt->idi[i] = Stank->pthcon[i];
				//Elin++;
			}
		}
		else if (strcmp(c, FLIN_TYPE) == 0)
		{
			Compnt->eqp = Flin;
			Flin->cmp = Compnt;
			Flin->name = name;

			flindat(Flin);

			for (i = 0; i < Compnt->Nout; i++, Elout++, Elin++)
			{
				Elout->cmp = Compnt;

				Elout->Ni = Compnt->Nin;
				Elout->elins = Compnt->elins;

				Elout->id = Compnt->ido[i];

				/*****
				printf("<<Elmalloc>>  FLIN_TYPE  i=%d Eo->id=%c\n", 
				i, Elout->id);
				****************/
				//Elout++, Elin++;
			}

			Flin++;
		}
		// 仮想空調機
		else if (strcmp(c, HCLOAD_TYPE) == 0 ||
			strcmp(c, HCLOADW_TYPE) == 0 || strcmp(c, RMAC_TYPE) == 0 || strcmp(c, RMACD_TYPE) == 0)
		{
			Compnt->eqp = Hcload;
			Hcload->cmp = Compnt;
			Hcload->name = name;

			// ルームエアコンの場合
			if (strcmp(c, RMAC_TYPE) == 0)
			{
				Hcload->RMACFlg = 'Y' ;

				// エアコンの機器スペックを読み込む
				rmacdat(Hcload) ;
			}
			else if (strcmp(c, RMACD_TYPE) == 0)
			{
				Hcload->RMACFlg = 'y' ;

				// エアコンの機器スペックを読み込む
				rmacddat(Hcload) ;
			}
			/*---- Roh Debug for a constant outlet humidity model of wet coil  2003/4/25 ----*/
			if ( Compnt->ivparm != NULL )
				Hcload->RHout = *(Compnt->ivparm);

			if (Compnt->wetparm != NULL && strcmp(Compnt->wetparm, "wet") == 0)
				Hcload->wet = 'y';		// 湿りコイル（吹出相対湿度一定
			else
				Hcload->wet = 'n';		// 吹出相対湿度は成り行き

			// 空気のみの流入、流出
			if (strcmp(c, HCLOAD_TYPE) == 0 || strcmp(c, RMAC_TYPE) == 0 || strcmp(c, RMACD_TYPE) == 0)
			{
				Hcload->type = 'D';
				Compnt->Nout = 2;
				Compnt->Nin = 2;
			}	 

			// 空気＋水の流入、流出
			else
			{
				Hcload->type = 'W';      
				Compnt->Nout = 3;
				Compnt->Nin = 3;
			}

			// 空気の絶対湿度用経路コピーを行う
			Compnt->airpathcpy = 'y';
			id = idmrkc ;
			for (i = 0; i < Compnt->Nout; i++, id++, Elout++)
			{
				Elout->cmp = Compnt;
				Elout->id = Elin->id = *id ;
				Elout->elins = Elin;

				Elout->Ni = 1;
				//Elout->Ni = 2;
				// 湿りコイル（吹出相対湿度一定）で出口絶対湿度の経路の場合
				// 要素方程式の未知数は2つ（入口絶対湿度と出口温度）
				if (i == 1 && Hcload->wet == 'y')
					Elout->Ni = 2;
				// 冷温水コイルで水側系統の場合
				// 要素方程式の未知数は5個
				// 水入口温度、空気入口温度、空気入口湿度
				// 空気出口温度、空気出口湿度
				else if (i == 2 && Hcload->type == 'W')     
					Elout->Ni = 5;

				for (ii = 0; ii < Elout->Ni; ii++, Elin++)
				{
					// 空気出口絶対湿度の計算の2つ目の変数は空気出口温度
					if (i == 1 && ii == 1)
						Elin->id = '*';
					//Elin++;
				}

				/***** printf("xxx Elmalloc xxx   %s  i=%d  Elout->Ni=%d\n",
				Hcload->name, i, Elout->Ni); *****/

				//Elout++;
			}
			Hcload++;
		}

		/*---- Satoh Debug VAV  2000/12/5 ----*/
		else if (strcmp(c, VAV_TYPE) == 0 ||strcmp(c, VWV_TYPE) == 0)
		{
			VAV = Eqsys->vav + neqp ;
			Compnt->eqp = VAV;
			VAV->name = name;
			VAV->cmp = Compnt;
			VAV->cat = Eqcat->Vavca + ncat;
			Compnt->Nin = Compnt->Nout = 2;

			if ( VAV->cat->type == VAV_PDT )
			{
				Compnt->airpathcpy = 'y' ;
				id = idmrkc ;
				for ( i = 0; i < Compnt->Nout; i++, id++, Elout++, Elin++ )
				{
					Elout->cmp = Compnt;
					Elout->id = Elin->id = *id ;
					Elout->elins = Elin;
					Elout->Ni = 1;
					//Elout++;
					//Elin++;
				}
			}
			else
			{
				Elout->cmp = Compnt ;
				Elout->elins = Elin ;
				Elout->Ni = 1 ;
				Elout++ ;
				Elin++ ;
			}
			VAV++ ;
		}
		// 電気蓄熱暖房器
		else if (strcmp(c, STHEAT_TYPE) == 0)
		{
			Stheat = Eqsys->stheat + neqp ;
			Compnt->eqp = Stheat;
			Stheat->name = name;
			Stheat->cmp = Compnt;
			Stheat->cat = Eqcat->stheatca + ncat;
			Compnt->airpathcpy = 'y' ;
			Compnt->Nin = Compnt->Nout = 2;

			id = idmrkc ;
			for ( i = 0; i < Compnt->Nout; i++, id++, Elout++, Elin++ )
			{
				Elout->cmp = Compnt;
				Elout->id = Elin->id = *id ;
				Elout->elins = Elin;
				Elout->Ni = 1;

				//Elout++;
				//Elin++;
			}
			Stheat++ ;
		}
		// Satoh追加　デシカント槽　2013/10/23
		else if (strcmp(c, DESI_TYPE) == 0)
		{
			Desi = Eqsys->Desi + neqp ;
			Compnt->eqp = Desi;
			Desi->name = name;
			Desi->cmp = Compnt;
			Desi->cat = Eqcat->Desica + ncat;

			// 絶対湿度経路のコピー
			Compnt->airpathcpy = 'y' ;

			id = idmrkc ;
			for ( i = 0; i < Compnt->Nout; i++, id++, Elout++ )
			{
				Elout->cmp = Compnt;
				Elout->id = Elin->id = *id ;
				Elout->elins = Elin;
				//Elin++ ;
				// すべての出口状態計算のための変数は2つ（温度と湿度）
				Elout->Ni = 2;

				//Elout++;
				//Elin++;

				//// 温度計算の場合
				//if(i == 0)
				//	idd = stralloc("tx") ;
				//else
				//	idd = stralloc("xt") ;

				for(ii = 0; ii < Elout->Ni; ii++, Elin++)
				{
					//Elin->id = idd[ii] ;
				}
				//free(idd) ;
			}
			Desi++ ;
		}
		// Satoh追加　気化冷却器 2013/10/26
		else if (strcmp(c, EVAC_TYPE) == 0)
		{
			Evac = Eqsys->Evac + neqp ;
			Compnt->eqp = Evac ;
			Evac->name = name ;
			Evac->cmp = Compnt ;
			Evac->cat = Eqcat->Evacca + ncat ;

			// 機器の出入口数（Tdry, xdry, Twet, xwet）
			Compnt->Nout = Compnt->Nin = 4 ;

			// 絶対湿度経路のコピー
			Compnt->airpathcpy = 'y' ;

			// D:Tdry d:xdry V:Twet v:xwet
			idd = stralloc("DdVv") ;
			for ( i = 0; i < Compnt->Nout; i++, Elout++ )
			{
				Elout->cmp = Compnt;
				Elout->id = Elin->id = idd[i] ;
				Elout->elins = Elin;
				// すべての出口状態計算のための変数は4つ（Wet、Dryの温度と湿度）
				Elout->Ni = 4 ;
				// 出口状態計算のための変数分だけメモリを確保する
				Elin += Elout->Ni ;
			}
		}
		else if ( strcmp ( c, VALV_TYPE ) == 0
			|| strcmp ( c, TVALV_TYPE ) == 0 )
		{
			Valv = Eqsys->Valv + Nvalv ;
			Compnt->eqp = Valv ;
			Valv->name = name ;
			Valv->cmp = Compnt ;

			if ( Valv->cmp->valvcmp != NULL )
				Valv->cmb = Valv->cmp->valvcmp ;

			Valv++ ;
			Nvalv++ ;
		}

		// Satoh OMVAV  2010/12/16
		else if ( strcmp ( c, OMVAV_TYPE ) == 0 )
		{
			OMvav = Eqsys->OMvav + NOMvav ;
			Compnt->eqp = OMvav ;
			OMvav->name = name ;
			OMvav->cmp = Compnt ;
			OMvav->cat = Eqcat->OMvavca + ncat ;
			//eloc = Elout ;
			//elic = Elin ;
			//Compnt->elouts = Elout ;
			//Compnt->elins = Elin ;

			if (OMvav->cmp->omparm != NULL)
				OMvavControl(OMvav, cmp, Ncompnt) ;

			OMvav++ ;
			NOMvav++ ;
		}
		else if ( strcmp ( c, QMEAS_TYPE ) == 0 )
		{
			Qmeas = Eqsys->Qmeas + NQmeas ;
			Compnt->eqp = Qmeas ;
			Qmeas->name = name ;
			Qmeas->cmp = Compnt ;

			Qmeas++ ;
			NQmeas++ ;
		}

		else if ( strcmp ( c, THEX_TYPE ) == 0 )
		{
			Thex = Eqsys->Thex + neqp ;
			Compnt->eqp = Thex ;
			Thex->name = name ;
			Thex->cmp = Compnt ;
			Thex->cat = Eqcat->Thexca + ncat ;
			Compnt->airpathcpy = 'y' ;
			Compnt->Nout = 4 ;

			for ( i = 0; i < Compnt->Nout; i++, Elout++ )
			{
				Elout->cmp = Compnt ;
				Elout->id = Compnt->ido[i] ;
				//Elin->id = Compnt->idi[i] ;
				Elout->elins = Elin ;

				if ( Thex->cat->eh > 0.0 )
				{
					Elout->Ni = 5 ;
					idxe = stralloc ( "eE*Oo" ) ;
					idxo = stralloc ( "oO*Ee" ) ;
				}
				else
				{
					Elout->Ni = 1 ;
					idxe = stralloc ( "e" ) ;
					idxo = stralloc ( "o" ) ;
				}

				//Elout->fluid = AIR_FLD ;

				if ( i == 0 || i == 2 )		// 温度の経路（要素方程式の変数は2つ）
					Elout->Ni = 2 ;

				for ( ii = 0; ii < Elout->Ni; ii++, Elin++ )
				{
					if ( i == 0 )
						Elin->id = idTe[ii] ;
					else if ( i == 2 )
						Elin->id = idTo[ii] ;
					else if ( i == 1 )
						Elin->id = idxe[ii] ;
					else if ( i == 3 )
						Elin->id = idxo[ii] ;
				}

				free ( idxe ) ;
				free ( idxo ) ;
			}
			Thex++ ;
		}
		else
			Errprint(1, errkey, c);

		/***********
		elop=Compnt->elouts;
		for (i=0; i<Compnt->Nout; i++, elop++)
		elop->coeffin = dcalloc(elop->Ni, errkey);
		*********/

		/****** 下記の　if 文の部分は、要素増加の改良では注意する必要あり  98/5/26 **/
		/******************
		if (strcmp(c, ROOM_TYPE) != 0 && strcmp(c, RDPANEL_TYPE) != 0
		&& strcmp(c, HCLOAD_TYPE) != 0 && strcmp(c, HCLOADW_TYPE) != 0 
		&& strcmp(c, PIPEDUCT_TYPE) != 0 && strcmp(c, PUMP_TYPE) != 0)
		{
		elop = Compnt->elouts;
		for (i = 0; i <Compnt->Nout; i++, elop++)
		{
		elop->Ni = Compnt->Nin;
		elop->elins = Compnt->elins;
		}
		}
		******************/

		elop=Compnt->elouts;
		for (i=0; i<Compnt->Nout; i++, elop++)
			elop->coeffin = dcalloc(elop->Ni, errkey);

		//		   printf ( "Elin=%2d Elout=%2d Compname=%s\n",
		//			   Elin - elic, Elout - eloc, Compnt->name ) ;
	}

	*Nelout = (int)(Elout - elmo);
	*Nelin = (int)(Elin - elmi);

	Elin = elmi;
	for (i = 0; i < *Nelin; i++, Elin++)
		Elin->upo = Elin->upv = NULL;

	//   C = cmp ;
	//   for ( i = 0; i < Ncompnt; i++, C++ )
	//	   printf ( "[%3d] Nin=%d  Nout=%d  Name=%s\n",
	//			i, C->Nin, C->Nout, C->name ) ;

	//   printf ( "<Elmalloc> Elout2=%d  Elin2=%d\n", *Nelout, *Nelin ) ;
}
