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

/*  es_cntlsch_s.c */
#include "common.h"
#include "MODEL.h"   /*-----higuchi 070918---*/
#include "fesy.h"
#include "fnesy.h"
#include "fnmcs.h"
#include "fnbld.h"
#include "fnlib.h"


void Contlschdlr(double dTM, int Ncontl, CONTL *Contl, int Nmpath, MPATH *Mpath, 
				 int Ncompnt, COMPNT *Compnt)
{
	int    i, m;
	MPATH  *Mp;
	PLIST  *Pli;
	ELOUT  *Eo;
	HCC    *Hcc;
	FLIN   *Flin;
	COMPNT *cmp;
	VALV	*v ;
	
	// 全ての経路、機器を停止で初期化
	Mp = Mpath;
	for (i = 0; i < Nmpath; i++, Mp++)
	{
		Mp->control = OFF_SW ;
		mpathschd(OFF_SW, Mp->Nlpath, Mp->plist);
	}
	
	// 機器の制御情報を「停止」で初期化
	cmp = Compnt;
	for (i = 0; i < Ncompnt; i++, cmp++)
	{
		cmp->control = OFF_SW ;

		if ( strcmp ( cmp->eqptype, VALV_TYPE ) == 0
			|| strcmp ( cmp->eqptype, TVALV_TYPE ) == 0 )
		{
			v = ( VALV * ) cmp->eqp ;

			if ( v->xinit != NULL )
				v->x = *v->xinit ;
			else
				v->x = 1.0 ;
		}

		Eo = cmp->elouts;
		for (m = 0; m < cmp->Nout; m++, Eo++)
		{
			Eo->sysld = 'n';
			Eo->control = OFF_SW ;
		}
	}
	
	/****
	printf("<<Contlschdlr>>\n");
	contlxprint(Ncontl, Contl); /*****/
	
	// CONTLの制御情報を反映
	for (i = 0; i < Ncontl; i++, Contl++)
	{
		Contl->lgv = 1;
		// True:1、False:0
		// if分で制御される場合
		if (Contl->type == 'c')
		{
			Contl->lgv = contrlif(Contl->cif);

			// ANDで2条件の場合
			if (Contl->andcif != NULL)
				Contl->lgv = Contl->lgv * contrlif(Contl->andcif);
			// ANDで3条件の場合
			if (Contl->andandcif != NULL)
				Contl->lgv = Contl->lgv * contrlif(Contl->andandcif);
			// or条件の場合
			if (Contl->orcif != NULL)
				Contl->lgv = Contl->lgv + contrlif(Contl->orcif);
		}
		
		if (Contl->lgv)
		{
			if ( Contl->cst != NULL )
			{
				if (Contl->cst->type == VAL_CTYPE)
				{
				/****
				printf ("  contlschdlr  lft= %lf  rgt = %lf\n",
				*Contl->cst->lft.v ,*Contl->cst->rgt.v);
					**********/
					
					*Contl->cst->lft.v = *Contl->cst->rgt.v;
					/****
					printf ("  contlschdlr  lft= %lf  rgt = %lf\n",
					*Contl->cst->lft.v ,*Contl->cst->rgt.v);
					**********/
				}
				else
				{
				/*************
				printf ("  Contlschdlr  lft= %c  rgt = %c\n",
				*Contl->cst->lft.s ,*Contl->cst->rgt.s);
					*******************/
					
					*Contl->cst->lft.s = *Contl->cst->rgt.s;
					
					/************
					printf ("  Contlschdlr  lft= %c  rgt = %c\n",
					*Contl->cst->lft.s ,*Contl->cst->rgt.s);
					*****************/
					/**** printf("<<Contlschdlr    2>>\n");
					*******/
					
					if (Contl->cst->pathtype == MAIN_CPTYPE)
					{
						Mp = (MPATH *)Contl->cst->path;
						Mp->control = *Contl->cst->lft.s ;

						//if ( Mp->mpair != NULL )
						//	Mp->control = Mp->mpair->control ;
						
						mpathschd(*Contl->cst->lft.s, Mp->Nlpath, Mp->plist);
					}
					else if (Contl->cst->pathtype == LOCAL_CPTYPE)
					{
						Pli = (PLIST *)Contl->cst->path;
						lpathscdd(*Contl->cst->lft.s, Pli);
					}
				}
			}
		}
	}
	/*********
	printf("<<Contlschdlr  3>>\n");**********/
	
	for (i = 0; i < Ncompnt; i++, Compnt++)
	{
		if (strcmp(Compnt->eqptype, ROOM_TYPE) == 0)
			
#if SIMUL_BUILDG
		{
			Compnt->control = ON_SW;
			Eo = Compnt->elouts;
			Eo->control = ON_SW;
			Eo++;
			Eo->control = ON_SW;
			
			roomldschd((ROOM *)Compnt->eqp);
		}
#else
		;
#endif
		
		else if (strcmp(Compnt->eqptype, BOILER_TYPE) == 0)
			boildschd((BOI *)Compnt->eqp);
		
		else if (strcmp(Compnt->eqptype, REFACOMP_TYPE) == 0)
			refaldschd((REFA *)Compnt->eqp);
		
		else if (strcmp(Compnt->eqptype, HCLOAD_TYPE) == 0
			|| strcmp(Compnt->eqptype, HCLOADW_TYPE) == 0
			|| strcmp(Compnt->eqptype, RMAC_TYPE) == 0 || strcmp(Compnt->eqptype, RMACD_TYPE) == 0)
		{
		/*****************************
		printf ("<Contlschdlr> Compnt->eqptype=%s\n",
		Compnt->eqptype ) ;
			*************************/
			hcldschd((HCLOAD *)Compnt->eqp);
			/************************
			printf ("bbbbbbbbb\n" ) ;
			************************/
		}
		
		else if (strcmp(Compnt->eqptype, PIPEDUCT_TYPE) == 0)
			pipeldsschd((PIPE *)Compnt->eqp);

		else if (strcmp(Compnt->eqptype, RDPANEL_TYPE) == 0)
			rdpnlldsschd((RDPNL *)Compnt->eqp) ;
		
		else if (strcmp(Compnt->eqptype, HCCOIL_TYPE) == 0)
		{
			Eo = Compnt->elouts + 1 ;
			if (Eo->lpath == NULL)
			{
				Eo->control = OFF_SW;
				Hcc = (HCC *)Compnt->eqp;
				Hcc->wet = 'd';
			}
		}
		else if (strcmp(Compnt->eqptype, FLIN_TYPE) == 0)
		{
			Compnt->control = FLWIN_SW;
			Flin = (FLIN *)Compnt->eqp;
			Eo = Compnt->elouts;
			Eo->control = FLWIN_SW;
			Eo->sysv = *Flin->vart;
			if(Flin->awtype == 'A')
			{
				Eo++;
				Eo->control = FLWIN_SW;
				Eo->sysv = *Flin->varx;
			}
		}
		
		Eo = Compnt->elouts;
		for (m = 0; m < Compnt->Nout; m++, Eo++)
		{
		/**********
		printf("<Contrlschdlr> m=%d %s control=%c sysld=%c\n", 
		m, Compnt->name, Eo->control, Eo->sysld);
			************/
			
			if (Eo->control == LOAD_SW)
				Eo->eldobj->sysld = 'y';
		}
	}
	
	Mp = Mpath;
	for (m = 0; m < Nmpath; m++, Mp++)
	{
		Pli = Mp->plist;
		for (i = 0; i < Mp->Nlpath; i++, Pli++)
		{
		/*****
		printf("<<Contlschdlr>> i=%d  Pli->batch=%c\n", i,  Pli->batch);
			*****/
			if (Pli->batch == 'y')
				lpathschbat(dTM, Pli);
		}
	}
}

/* --------------------------------------------------- */

int contrlif(CTLIF *ctlif)
{
	int id;
	double a, b;
	
	if (ctlif->type == VAL_CTYPE)
	{
		a = *ctlif->lft1.v; 
		if (ctlif->Nlft == 2)
			a -= *ctlif->lft2.v;
		b = *ctlif->rgt.v;
		
		/*************************
		printf("<contrlif> Lft=%lf Rgt=%lf op=%c  \n",
			a, b, ctlif->op ) ;
			/**********************************/

		switch (ctlif->op)
		{
		case 'l':
			id = (a < b);
			break;
		case 'g':
			id = (a > b);
			break;
		case 'E':
			id = (a == b);
			break;
		case 'L':
			id = (a <= b);
			break;
		case 'G':
			id = (a >= b);
			break;
		case 'N':
			id = (a != b);
			break;
		default:
			id = 0;
			break;
		}
	}
	else
	{
		switch (ctlif->op)
		{
		case 'E':
			id = (*ctlif->lft1.s == *ctlif->rgt.s);
			break;
		case 'N':
			id = (*ctlif->lft1.s != *ctlif->rgt.s);
			break;
		default:
			id = 0;
			break;
		}
	}
	/*************************
	printf("%d\n", id) ;
	**************************/

	return id;
}

/* --------------------------------------------------- */

void mpathschd(char control, int Nlpath, PLIST *Plist)
{
	int j;
	
	for (j = 0; j < Nlpath; j++, Plist++)
	{
		Plist->control = control;
		lpathscdd(control, Plist);
	}
}

/* --------------------------------------------------- */

void lpathscdd(char control, PLIST *Plist)
{
	if (Plist->org == 'y')
	{
		lpathschd(control, Plist->Nelm, Plist->pelm);
		
		if (Plist->lpair != NULL)
		{
			Plist->lpair->control = control;
			lpathschd(control, Plist->lpair->Nelm, Plist->lpair->pelm);
		}
	}
}


/* --------------------------------------------------- */

void lpathschd(char control, int Nelm, PELM *Pelm)
{
	int    j;
	
	for (j = 0; j < Nelm; j++, Pelm++)
	{
		// 電気蓄熱暖房器の機器制御は変更しない
		if (strcmp(Pelm->cmp->eqptype, STHEAT_TYPE) != 0)
			Pelm->cmp->control = control;
		
		//	  if (Pelm->out == NULL )
		//		  printf("aaaaaa\n");
		
		if (Pelm->out != NULL)
		{
//			printf("bbbbbb\n");
			// 一時的に室の場合は止められないようにした。電気蓄熱暖房器も同様
			// 順次対応予定
			if (strcmp(Pelm->cmp->eqptype, ROOM_TYPE) != 0)// && strcmp(Pelm->cmp->eqptype, STHEAT_TYPE) != 0)
				Pelm->out->control = control;
			//else if (strcmp(Pelm->cmp->eqptype, STHEAT_TYPE) != 0)
			//	Pelm->out->control = control;
		}
	}      
}
/* --------------------------------------------------- */

/* 蓄熱槽のバッチ給水、排水時の設定  */

void lpathschbat(double dTM,  PLIST *Plist)
{
	int j, k=0, i, jt=0, ifl, Nelm, Ne;
	char  batop;
	double Tsout = 0.0, Gbat=0.0;
	STANK *Stank;
	PELM  *Pe, *Pelm, *Pei;
	extern double row;
	
	Stank = NULL ;
	Nelm = Plist->Nelm;
	Pe = Plist->pelm;
	Pelm = Plist->pelm;
	for (j = 0; j < Nelm; j++, Pe++)
	{
		if (strcmp(Pe->cmp->eqptype, STANK_TYPE) == 0)
		{
			Plist->G = Gbat = 0.0;
			k = (int)(Pe->out - Pe->cmp->elouts);
			Stank = (STANK *)Pe->cmp->eqp;
			Stank->batchop = OFF_SW;
			batop = *Stank->batchcon[k];
			
			/***printf("<<lpathschbat>> batchkon[k]=%c\n", batop);***/
			
			if (batop == BTFILL)
			{
				for (i = 0; i< Stank->Ndiv; i++)
				{	
					/***printf("<<lpathschbat>> i=%d dtankF=%c\n",i, Stank->dtankF[i]);***/
					
					if (Stank->dtankF[i] == TANK_EMPTY)
					{ 
						jt = j;
						Gbat += Stank->dvol[i];
						Stank->batchop = BTFILL;
					}
				}
			}
			else
			{
				ifl = 0;
				for (i = 0; i < Stank->Ndiv; i++)
				{
					if (Stank->dtankF[i] == TANK_FULL)
						ifl++;
				}
				if (Stank->Ndiv == ifl)
				{
					Stank->batchop = BTDRAW;
					jt = j;
				}
			}
			/***printf("<<lpathschbat>> Stank->batchop=%c\n", Stank->batchop);***/
			break;
		}
	}
	if (Stank->batchop == BTFILL || Stank->batchop == BTDRAW)
	{
		Pei = Pelm;
		Ne = Nelm;
		/***
		printf("<<lpathschbat>> Pelm->out->control=%c  Nelm=%d G=%lf\n", 
		Pelm->out->control, Nelm, Plist->G);***/
		if (Pelm->out->control == FLWIN_SW)
		{ 
			Pei++;
			Ne--;
		}
		/***printf("<<lpathschbat>> Ne=%d  Pei=%d\n", Ne, Pei - Pelm);***/
		
		lpathschd(ON_SW, Ne, Pei);
		
		/***printf("<<lpathschbat>> Nelm-jt=%d  jt=%d\n",
		Nelm -jt , jt);***/
		
		if (Stank->batchop == BTFILL)
		{
			Plist->G = Gbat * row / dTM;
			lpathschd(OFF_SW, Nelm - jt, Pelm + jt);	 
			(Pelm+jt)->out->control = BATCH_SW;
		}
		else if (Stank->batchop == BTDRAW)
		{    
			lpathschd(OFF_SW, jt, Pelm);
			(Pelm+jt)->out->control = BATCH_SW;
			
			for (j = 0; j <= Stank->Jout[k]; j++)
			{
				Stank->dtankF[j] = TANK_EMPTY;
				Tsout += Stank->Tssold[j];
				Gbat += Stank->dvol[j];
			}
			Tsout /= (double)(Stank->Jout[k] + 1);
			/***printf("<<lpathschbat>> Tsout=%lf\n", Tsout);***/
			Plist->G = Gbat * row / dTM;
			
			(Pelm+jt)->out->sysv = Tsout;
		}
	}
}

/* --------------------------------------------------- */

void	contlxprint(int Ncontl, CONTL *C)
{
	int		i;
	CTLIF	*cif;
	CTLST	*cst;
	double	V ;
	CONTL	*Contl ;
	
	Contl = C ;
	if ( DEBUG )
	{
		printf("contlxprint --- Contlschdlr\n");
		
		for (i = 0; i < Ncontl; i++, Contl++)
		{
			printf("[%d]  type=%c  lgv=%d\n",
				i, Contl->type, Contl->lgv);
			cif = Contl->cif;
			cst = Contl->cst;
			
			if (cif != NULL)
			{
				printf("  iftype=%c [ %c ]", cif->type, cif->op);
				if (cif->type == VAL_CTYPE)
				{
					if (cif->Nlft > 1)
						V = *cif->lft2.v ;
					else
						V = 0.0 ;
					
					printf("  lft1=%lf  lft2=%lf  rgt=%lf\n", 
						*cif->lft1.v, V, *cif->rgt.v);
				}
				else
					printf("  lft1=%c  lft2=%c  rgt=%c\n", 
					*cif->lft1.s, *cif->lft2.s, *cif->rgt.s);
			}
			
			printf("  sttype=%c  pathtype=%c", cst->type, cst->pathtype);
			if (cst->type == VAL_CTYPE)
				printf("  lft=%lf    rgt=%lf\n", 
				*cst->lft.v,  *cst->rgt.v);
			else
				printf("  lft=%c    rgt=%c\n", 
				*cst->lft.s, *cst->rgt.s);
		}
	}

	Contl = C ;
	if ( ferr )
	{
		fprintf(ferr,"contlxprint --- Contlschdlr\n");
		
		for (i = 0; i < Ncontl; i++, Contl++)
		{
			fprintf(ferr,"[%d]\ttype=%c\tlgv=%d\n",
				i, Contl->type, Contl->lgv);
			cif = Contl->cif;
			cst = Contl->cst;
			
			if (cif != NULL)
			{
				fprintf(ferr,"\tiftype=%c\t[%c]", cif->type, cif->op);
				if (cif->type == VAL_CTYPE)
				{
					if (cif->Nlft > 1)
						V = *cif->lft2.v ;
					else
						V = 0.0 ;
					
					fprintf(ferr,"\tlft1=%.2g\tlft2=%.2grgt=%.2g\n", 
						*cif->lft1.v, V, *cif->rgt.v);
				}
				else
				{
					fprintf(ferr,"\tlft1=%c\tlft2=%c\trgt=%c\n", 
						*cif->lft1.s, *cif->lft2.s, *cif->rgt.s);
				}
			}
			
			fprintf(ferr,"\tsttype=%c\tpathtype=%c", cst->type, cst->pathtype);
			if (cst->type == VAL_CTYPE)
			{
				fprintf(ferr,"\tlft=%.2g\trgt=%.2g\n", 
					*cst->lft.v,  *cst->rgt.v);
			}
			else
			{
				fprintf(ferr,"\tlft=%c\trgt=%c\n", 
					*cst->lft.s, *cst->rgt.s);
			}
		}
	}
}
/* --------------------------------------------------- */

/* 負荷計算モードの再設定、
暖房時冷房負荷または冷房時暖房負荷のときは自然室温計算　*/

int rmloadreset(double Qload, char loadsw, ELOUT *Eo, char SWITCH)
{
	if (Eo->sysld == 'y')
	{
		if ((loadsw == HEATING_LOAD && Qload < 0.0) ||
			(loadsw == COOLING_LOAD && Qload > 0.0))
		{
			Eo->control = SWITCH;
			/******** Eo->eldobj->sysld = 'n';******/
			Eo->sysld = 'n';
			return 1;
		}
		else
			return 0;	 
	}
	else
		return 0;
}

/* --------------------------------------------------- */

/* 加熱、冷却モードの再設定、
加熱房時冷房負荷または冷却時暖房負荷のときは機器停止　*/

int chswreset(double Qload, char chmode, ELOUT *Eo)
{
	ELOUT	*Elo ;
/**********************************
printf ( "<chswreset> Qload=%lf chmode=%c \n", Qload, chmode ) ;
	**********************************/
	
	if ((chmode == HEATING_SW && Qload < 0.0) ||
		(chmode == COOLING_SW && Qload > 0.0))
	{
		Elo = Eo ;
		Elo->control = ON_SW;
		Elo->sysld = 'n';
		Elo->emonitr->control = ON_SW;
		//Elo->cmp->control = OFF_SW ;
		return 1 ;
	}

	//if((chmode == COOLING_SW && Qloadl > 0.)
	//{
	//	Elo = Eo ;
	//	Elo++ ;
	//	Elo->control = ON_SW;
	//	Elo->sysld = 'n';
	//	if(Elo->emonitr != NULL)
	//		Elo->emonitr->control = ON_SW;
	//	
	//	return 1;
	//}
	else
	{
		return 0;
	}
}
/* --------------------------------------------------- */

/* 仮想空調機の再設定
湿りコイルの非除湿時乾きコイルへの変更 */

int	chqlreset ( HCLOAD *Hcload )
{
	double	Ql, Qs ;
	char	wet ;
	char	chmode ;
	ELOUT	*Elo, *Elos ;

	Ql = Hcload->Ql ;
	Qs = Hcload->Qs ;
	wet = Hcload->wetmode ;
	chmode = Hcload->chmode ;
	Elos = Elo = Hcload->cmp->elouts ;

	Elo++ ;
	//if ( chmode == COOLING_SW && wet == 'y' && Ql > 0.0 )
	if ( wet == 'y' && Ql > 1.e-6 || wet=='y' && Qs>=0.0)
	{
		Hcload->wetmode = 'n' ;
		Elo->control = ON_SW ;
		Elo->sysld = 'n' ;

		if ( Elo->emonitr != NULL )
			Elo->emonitr->control = ON_SW ;
		//Hcload->cmp->control = OFF_SW;

		return 1 ;
	}
	//if ( wet == 'y' && Qs >= 0.0 )
	//{
	//	Hcload->wetmode = 'n' ;
	//	return 1 ;
	//}

	if(chmode == COOLING_SW && (Ql > 0. || Qs >= 0.))
	{
		Elo->control = ON_SW;
		Hcload->wetmode = 'n';
		Elo->sysld = 'n';
		if(Elo->emonitr != NULL)
			Elo->emonitr->control = ON_SW;
		
		//Hcload->cmp->control = OFF_SW ;

		return 1;
	}

	return 0 ;
}

/* 過負荷運転ための再設定 */

int maxcapreset(double Qload, double Qmax, char chmode, ELOUT *Eo)
{
	BOI	*Boi ;

	char Eocontrol, Eosysld, Eoemonitrcontrol, Boicmpcntrol, Boimode;

	Boi = (BOI *)Eo->cmp->eqp;

	Eocontrol = Eo->control;
	Eosysld = Eo->sysld;
	if (Eo->emonitr != NULL)
		Eoemonitrcontrol = Eo->emonitr->control;
	else
		return 0;

	Boicmpcntrol = Boi->cmp->control;
	Boimode = Boi->mode;

	if ((chmode == HEATING_SW && Qload > Qmax) ||
		(chmode == COOLING_SW && Qload < Qmax))
	{
		// 過負荷なので最大能力で再計算する
		Eo->control = ON_SW;
		Eo->sysld = 'n';
		Eo->emonitr->control = ON_SW;
		Boi->cmp->control = ON_SW;

		// 最大能力運転フラグ
		Boi->mode = 'M' ;
		
		//return 1;
	}
	//else
	//{
	//	Eo->control = LOAD_SW;
	//	Eo->sysld = 'y';
	//	Eo->emonitr->control = LOAD_SW;
	//	Boi->cmp->control = ON_SW;
	//	Eo->sysv = Boi->Toset;

	//	//return 1;
	//}
	if (Eo->control == Eocontrol && Eo->sysld == Eosysld && Eo->emonitr->control == Eoemonitrcontrol &&
		Boi->cmp->control == Boicmpcntrol && Boi->mode == Boimode)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


