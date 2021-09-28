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

/* escntllb_s.c */

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include "MODEL.h"   /*-----higuchi 070918---*/
#include "fesy.h"
#include "fnesy.h"
#include "fnmcs.h"
#include "fnlib.h"
#include "fnbld.h"
#include "fnfio.h"

#define   NMKEYMX 50

/*  システム変数名、内部変数名、スケジュール名のポインター  */

int ctlvptr(char *s, SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, int Nmpath, MPATH *Mpath,
			WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl, VPTR *vptr, VPTR *vpath)
{
	int i, err = 0;
	
	if ((i = idsch(s, Schdl->Sch, NULL))  >= 0)
	{	 
		vptr->ptr = Schdl->val + i;
		vptr->type = VAL_CTYPE;
		
		/*****
		printf("xxxxx ctlvptr xxxxx Sch-val i=%d  err=%d\n", i, err);
		*****/
	}
	else if ((i = idscw(s, Schdl->Scw, NULL)) >= 0)
	{
		vptr->ptr = Schdl->isw + i;
		vptr->type = SW_CTYPE;
		
		/*****
		printf("xxxxx ctlvptr xxxxx Scw-isw i=%d  err=%d\n", i, err);
		*****/
	}
	else
		err = kynameptr(s, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, vptr, vpath);
	
	Errprint(err, "<ctlvptr>", s);
	
	return err;
}
/* ----------------------------------------------------------------- */

/* システム経路名、要素名、システム変数名、内部変数名の分離 */

int strkey(char *s, char **key)
{
	char *st;
	char ss[SCHAR];
	int  i, nk;
	
	if ((i = (int)strlen(s)) == 0)
		return 0;
	else
	{  
		strcpy(ss, s); 
		
		key[0] = ss;
		nk = 1;
		for (st = ss + 1; st <ss + i; st++)
		{	 
			if (*st == '_')
			{
				*st = '\0';
				st++ ;
				key[nk] = st;
				nk++ ;
			}
		}
		/************* 
		printf("   strkey  nk=%d", nk);
		for (i = 0; i < nk; i++)
		printf(" key[%d]=%s",i, key[i]);
		printf("\n");
		****************/
		
		return nk;
	}
}
/* ----------------------------------------------------------------- */

/*  経路名、システム変数名、内部変数名のポインター  */

int kynameptr(char *s, SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, int Nmpath, MPATH *Mpath,
			  WDAT *Wd, EXSFS *Exsf, VPTR *vptr, VPTR *vpath)
{
	char *e, *key[NMKEYMX];
	int  i, nk, err = 0;
	EXSF	*Exs ;
	
	nk = strkey(s, key) ;
	if ( nk )
	{
		if (strcmp(key[0], "Ta") == 0)
		{
			vptr->type = VAL_CTYPE;
			vptr->ptr = &Wd->T;
		}
		else if (strcmp(key[0], "xa") == 0)
		{
			vptr->type = VAL_CTYPE;
			vptr->ptr = &Wd->x;
		}
		else if (strcmp(key[0], "RHa") == 0)
		{
			vptr->type = VAL_CTYPE;
			vptr->ptr = &Wd->RH;
		}
		else if (strcmp(key[0], "ha") == 0)
		{
			vptr->type = VAL_CTYPE ;
			vptr->ptr = &Wd->h ;
		}
		else if (strcmp(key[0], "Twsup") == 0)
		{
			vptr->type = VAL_CTYPE;
			vptr->ptr = &Wd->Twsup;
		}
		else if (strcmp(key[0], "Ihol") == 0)
		{
			vptr->type = VAL_CTYPE;
			vptr->ptr = &Wd->Ihor;
		}
		else 
		{
			// 傾斜面名称の検索
			if ( Exsf != NULL )
			{
				Exs = Exsf->Exs ;
				for ( i = 0; i < Exsf->Nexs; i++, Exs++)
				{
					if (strcmp(key[0], Exs->name ) == 0)
					{
						// 傾斜面への入射直達日射量
						if(strcmp(key[1], "Idre") == 0)
						{
							vptr->type = VAL_CTYPE ;
							vptr->ptr = &Exs->Idre ;
							return 0 ;
						}
						// 傾斜面への入射拡散日射量
						else if(strcmp(key[1], "Idf") == 0)
						{
							vptr->type = VAL_CTYPE ;
							vptr->ptr = &Exs->Idf ;
							return 0 ;
						}
						// 傾斜面への入射全日射量
						else if(strcmp(key[1], "Iw") == 0)
						{
							vptr->type = VAL_CTYPE ;
							vptr->ptr = &Exs->Iw ;
							return 0 ;
						}
					}
				}
			}

			if (Nmpath > 0)
				err = pathvptr(nk, key, Nmpath, Mpath, vptr, vpath);
			else
				err = 1;

			if (err)
			{
				if (Simc->Nvcfile > 0)
					err = vcfptr(key, Simc, vptr);
				else
					err =1;
			}

			if (err)
			{	 
				for (i = 0; i < Ncompnt; i++, Compnt++)
				{	    
					if (strcmp(key[0], Compnt->name) == 0)
					{
						//printf("%s\n", Compnt->name) ;
						err = compntvptr(nk, key, Compnt, vptr) ;
						if ( err )
						{
							e = Compnt->eqptype;
							if (strcmp(e, ROOM_TYPE) == 0)
#if SIMUL_BUILDG
								/*** if (nk>1) ****/
								err = roomvptr(nk,  key, (ROOM *)Compnt->eqp, vptr);
#else
							;
#endif
							else if (strcmp(e, REFACOMP_TYPE) == 0)
							{
							/*****
							printf("xxxx kynameptr xxxx key[0]=%s key[1]=%s err=%d\n",
								key[0], key[1], err); *****/
								err = refaswptr(key, (REFA *)Compnt->eqp, vptr);
								/*****
								printf("xxxx kynameptr xxxx key[0]=%s key[1]=%s err=%d\n",
								key[0], key[1], err); *****/
							}
							else if (strcmp(e, HCLOAD_TYPE) == 0 ||
								strcmp(e, HCLOADW_TYPE) == 0 || strcmp(e, RMAC_TYPE) == 0 || strcmp(e, RMACD_TYPE) == 0 )
								err = hcldswptr(key, (HCLOAD *)Compnt->eqp, vptr) ;
							/* VAV Satoh Debug 2001/1/19 */
							else if (strcmp(e, VAV_TYPE) == 0 || strcmp(e, VWV_TYPE) == 0)
								err = vavswptr(key, (VAV *)Compnt->eqp, vptr) ;
							else if (strcmp(e, COLLECTOR_TYPE) == 0)
								err = collvptr(key, (COLL *)Compnt->eqp, vptr);
							else if (strcmp(e, STANK_TYPE) == 0)
								err = stankvptr(key, (STANK *)Compnt->eqp, vptr);
							else if (strcmp(e, STHEAT_TYPE) == 0)
								err = stheatvptr(key, (STHEAT *)Compnt->eqp, vptr, vpath);
							// Satoh追加　デシカント槽　2013/10/23
							else if (strcmp(e, DESI_TYPE) == 0)
								err = Desivptr(key, (DESI *)Compnt->eqp, vptr);
							else if (strcmp(e, PIPEDUCT_TYPE) == 0)
								err = pipevptr(key, (PIPE *)Compnt->eqp, vptr) ;
							else if (strcmp(e, RDPANEL_TYPE) == 0)
								err = rdpnlvptr(key, (RDPNL *)Compnt->eqp, vptr) ;
							else if (strcmp(e, VALV_TYPE) == 0 || strcmp(key, TVALV_TYPE) == 0)
								err = valv_vptr(key, (VALV*)Compnt->eqp, vptr);
							else
								Eprint ( "CONTL", Compnt->name ) ;
						}
						break;
					}
				}
				
			}
		}
	}
	else
		err = 1;
	
	Errprint(err, "<kynameptr>", s);
	
	return err;
}

/* ----------------------------------------------------------------- */

/*  経路名のポインター  */

int pathvptr(int nk, char **key, int Nmpath, MPATH *Mpath,  VPTR *vptr, VPTR *vpath)
{
	int i, err = 0;
	MPATH *Mp, *Mpe;
	PLIST *Plist, *Plie;
	
	Mp = Mpath;
	
	for (i = 0; i < Nmpath; i++, Mpath++)
	{      
		if (strcmp(key[0], Mpath->name) == 0)
		{
			vpath->type = MAIN_CPTYPE;
			vpath->ptr = Mpath;
			
			if (nk == 1 || strcmp(key[1], "control") == 0)
			{
				vptr->type = SW_CTYPE;
				vptr->ptr = &Mpath->control;
			}
			break;
		}
	}

	err = (i == Nmpath) ;

	if ( err )
	{
		Mpe = Mp + Nmpath - 1;
		Plie = Mpe->plist + Mpe->Nlpath;
		
		for (Plist = Mp->plist; Plist < Plie; Plist++)
		{
			if (Plist->name != NULL)
			{
			/***********
			printf("<< pathvptr >> Key=%s, Plist->name=%s\n",
			key[0], Plist->name);
				*****************/
				
				if (strcmp(key[0], Plist->name) == 0)
				{
					vpath->type = LOCAL_CPTYPE;
					vpath->ptr = Plist;
					
					if (nk == 1 || strcmp(key[1], "control") == 0)
					{
						vptr->type = SW_CTYPE;
						vptr->ptr = &Plist->control;
					}
					else if(strcmp(key[1], "G") == 0)
					{
						vptr->type = VAL_CTYPE;
						vptr->ptr = &Plist->G;
					}
					break;
				}
			}
		}
		if (Plist == Plie)
			err = 1;     
	}
	return err;
}

COMPNT	*Compntptr ( char *name, int N, COMPNT *Compnt )
{
	COMPNT	*C ;
	int		i ;

	C = NULL ;
	for ( i = 0; i < N; i++, Compnt++ )
	{
		if ( strcmp ( name, Compnt->name ) == 0 )
			C = Compnt ;
	}

	return C ;
}

/* ----------------------------------------------------------------- */

/*  システム要素出口温度、湿度のポインター  */

int compntvptr(int nk, char **key, COMPNT *Compnt, VPTR *vptr)
{
	ELOUT *Eo;
	int i, err = 0;
	VALV	*v ;
	char	*etype ;
	
	if (nk == 1 || strcmp(key[1], "control") == 0)
	{
		etype = Compnt->eqptype ;
		if ( strcmp ( etype, VALV_TYPE ) != 0 && strcmp ( etype, TVALV_TYPE ) != 0 )
		{
			// ボイラなど機器自体の停止ではなく、燃焼の停止とする
			//vptr->ptr = &Compnt->control;
			Eo = Compnt->elouts ;
			if (strcmp(etype, STHEAT_TYPE) == 0)
				vptr->ptr = &Compnt->control;
			else
				vptr->ptr = &Eo->control;
			vptr->type = SW_CTYPE;
		}
		else
		{
			v = ( VALV * ) Compnt->eqp ;
			vptr->ptr = &v->x ;
			vptr->type = VAL_CTYPE ;
			v->org = 'y' ;
		}
	}
	else
	{
		Eo = Compnt->elouts;
		for (i = 0; i < Compnt->Nout; i++, Eo++)
		{
			if ((Eo->fluid == AIRa_FLD && strcmp(key[1], "Taout") == 0)
				|| (Eo->fluid == AIRx_FLD && strcmp(key[1], "xout") == 0)
				|| (Eo->fluid == WATER_FLD && strcmp(key[1], "Twout") == 0))
			{
				vptr->ptr = &Eo->sysv;
				vptr->type = VAL_CTYPE;
				break;
			}
		}
		if (i == Compnt->Nout)
			err = 1;
	}   
	return err;
}

/* ----------------------------------------------------------------- */

/* 負荷計算を行うシステム要素の設定システム変数のポインター */

int loadptr(COMPNT *loadcmp, char *load, char *s, int Ncompnt, COMPNT *Compnt,
			VPTR *vptr)
{
	ROOM *Room;
	char *key[NMKEYMX], idmrk=' ';
	int  i, nk, err=0;
	ELOUT  *Eo, *eold;
	RMLOAD	*R ;
	RDPNL	*Rdpnl ;
	
	nk = (strkey(s, key)) ;

	if ( nk )
	{
		for (i = 0; i < Ncompnt; i++, Compnt++)
		{
			if (strcmp(key[0], Compnt->name) == 0)
			{
				if (strcmp(Compnt->eqptype, BOILER_TYPE) == 0)
				{
					err = boildptr(load, key, (BOI *)Compnt->eqp, vptr);
					idmrk = 't';
					/*** break;***/
				}
				
				if (strcmp(Compnt->eqptype, REFACOMP_TYPE) == 0)
				{
					err = refaldptr(load, key, (REFA *)Compnt->eqp, vptr);
					idmrk = 't';
				}
#if SIMUL_BUILDG
				else if (strcmp(Compnt->eqptype, HCLOAD_TYPE) == 0
					|| strcmp(Compnt->eqptype, RMAC_TYPE) == 0|| strcmp(Compnt->eqptype, RMACD_TYPE) == 0)
				{
					err = hcldptr(load, key, (HCLOAD *)Compnt->eqp, vptr, &idmrk);
					/***break;***/
				}
				else if (strcmp(Compnt->eqptype, PIPEDUCT_TYPE) == 0)
					err = pipeldsptr(load, key, (PIPE *)Compnt->eqp, vptr, &idmrk);
				
				else if (strcmp(Compnt->eqptype, RDPANEL_TYPE) == 0)
				{
					Rdpnl = ( RDPNL * ) Compnt->eqp ;
					err = rdpnlldsptr(load, key, (RDPNL *)Compnt->eqp, vptr, &idmrk);
					if (loadcmp != NULL && strcmp(loadcmp->eqptype, OMVAV_TYPE) == 0)
					{
						Rdpnl->OMvav = ( OMVAV * ) loadcmp->eqp ;
						Rdpnl->OMvav->omwall = Rdpnl->sd[0] ;
					}
				}

				else if (strcmp(Compnt->eqptype, ROOM_TYPE) == 0)
				{
					Room = (ROOM *)Compnt->eqp;
					if (Room->rmld == NULL)
					{
						if ((Room->rmld = (RMLOAD *)malloc(1 * sizeof(RMLOAD))) == NULL)
							Ercalloc(1, "roomldptr");

						//原因不明だがkeyの内容が失われるので無理矢理復活させる処理//E.Togashi 2008.09.06*****
						strkey(s, key);
						//************************************************************************************

						R = Room->rmld ;
						R->loadt = R->loadx = NULL ;
						R->FOTN = R->FOPL = NULL ;

						if (loadcmp != NULL && strcmp(loadcmp->eqptype, VAV_TYPE) == 0)
							Room->VAVcontrl = (VAV *) loadcmp->eqp ;
					}
					err = roomldptr(load, key, Room, vptr, &idmrk);	       
					/***break;***/
				}
#endif
				if (err == 0)
				{
				/***************
				printf("<loadptr>   i=%d  %s\n", i, Compnt->name);
					*************************/
					
					if (loadcmp == NULL)
						loadcmp = Compnt;
					Eo = Compnt->elouts;
					eold = loadcmp->elouts;
					
					if (idmrk == 'x')
					{
						Eo++ ;
						eold++;
					}
					
					Eo->eldobj = eold;
					eold->emonitr = Eo;
					
					break;
				}
			}
			else
				err = 1;
		}
		return err;
	}
	else
		return 1;
}


