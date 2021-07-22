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

/*   bl_panel.c  */

#include <math.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnfio.h"
#include "fnbldg.h"
#include "fnmcs.h"
#include "fnlib.h"

#define WPTOLE  1.0e-10

/*  輻射パネル有効熱容量流量  */

void panelwp(RDPNL *rdpnl)
{
	RMSRF *Sd;
	ELOUT *Eo;
	int   i;
	WALL  *Wall ;
	double	Kc, Kcd ;
	
	Sd = rdpnl->sd[0]; 
	Eo = rdpnl->cmp->elouts;
	Wall = Sd->mw->wall ;

	if(Wall->chrRinput == 'Y')
	{
		Kc = Sd->dblKc ;
		Kcd = Sd->dblKcd ;
	}
	else
	{
		Kc = Wall->Kc ;
		Kcd = Wall->Kcd ;
	}
	
	if (Eo->control != OFF_SW && rdpnl->cmp->elins->upv != NULL)
	{
		/***** if (Eo->control != OFF_SW) *****/
		rdpnl->cG = Eo->G * spcheat(Eo->fluid) ;

		if ( Wall->WallType == 'P' )
			rdpnl->Wp = rdpnl->cG * rdpnl->effpnl / Sd->A;
		else
		{
			rdpnl->Ec = 1. - exp ( - Kc * Sd->A / rdpnl->cG ) ;
			rdpnl->Wp = Kcd * rdpnl->cG * rdpnl->Ec / ( Kc * Sd->A ) ;
			//printf("Wp=%lf\n", rdpnl->Wp ) ;
		}
	}
	else
	{
		rdpnl->cG = 0. ;
		rdpnl->Ec = 0.0 ;
		rdpnl->Wp = 0.0;
	}
		/*******************
		printf(" panelwp  1 Eo->control=%c  G=%lf Wp=%lf\n",
		Eo->control, Eo->G, rdpnl->Wp);
	/***********/
	
	// 流量が前時刻から変化していれば係数行列を作りなおす
	if ((fabs((double)(rdpnl->Wp - rdpnl->Wpold)) >= WPTOLE) || rdpnl->sd[0]->PCMflg == 'Y')
	{      
		rdpnl->Wpold = rdpnl->Wp;
		
		/*
		printf(" panelwp 2  MC=%d\n", rdpnl->MC);
		*/
		
		for (i = 0; i < rdpnl->MC; i++)
		{
			rdpnl->sd[i]->mrk = '*';    // 表面の係数行列再作成
			rdpnl->rm[i]->mrk = '*';	// 室の係数行列再作成
		}
	}
	/*
	printf(" panelwp end\n");
	*/
	
}

/* -------------------------------------- */

/*  輻射パネル計算用係数    */

void Panelcf(RDPNL *rdpnl) 
{
	int   j, nn, m, mp, M, iup,
		nrp, n, N;
	double   *alr, *epr, *epw, ew, kd;
	ROOM  *rm;
	RMSRF *Sd, *Sdd;
	WALL *wall ;
	MWALL *Mw;
	double  C1;
	
	/*
	printf("Panelcf xxxxxxxxxxxxxx  name=%s Wp=%lf\n",
	rdpnl->name, rdpnl->Wp);
	*/
	
	if (rdpnl->Wp > 0.0)
	{     
	/*
	printf("Panelcf name  %s\n", rdpnl->cmp->name);
		*/
		
		for (m=0; m < rdpnl->MC; m++)
		{
			Sd = rdpnl->sd[m];
			rm = rdpnl->rm[m];
			N = rm->N;
			nrp = (int)(Sd - rm->rsrf);
			nn = N * nrp;
			
			if ( Sd->mrk == '*' || Sd->PCMflg == 'Y')
			{
				if (m == 0)
				{
					Mw = Sd->mw;
					mp = Mw->mp;
					M = Mw->M;
					
					iup = mp * M;
					
					rdpnl->FIp[m] = Mw->UX[iup] * Mw->uo;
					if (Mw->wall->WallType == 'P' )			// 通常の床暖房パネル
						rdpnl->FOp[m] = Mw->UX[iup + M-1] * Mw->um;
					else if(Mw->wall->WallType == 'C' )		// 屋根一体型空気集熱器
						rdpnl->FOp[m] = Mw->UX[iup + M-1] * Sd->ColCoeff ;
					rdpnl->FPp = Mw->UX[iup + mp] * Mw->Pc * rdpnl->Wp;
					//printf("FPp=%lf\n", rdpnl->FPp);
				}
				else
				{
					Mw = Sd->mw;
					rdpnl->FIp[1] = rdpnl->FOp[0];
					rdpnl->FOp[1] = rdpnl->FIp[0];
				}
				
				wall = Mw->wall ;
				//if ( Sd->mw->wall->WallType == 'P' )
				//{
					alr = rm->alr + nn;
					Sdd = rm->rsrf;
					C1 = Sd->alic;
					for (j = 0; j < N; j++, alr++, Sdd++)
					{ 
						if (j != nrp)
							C1 += *alr * Sdd->WSR;
					}
					C1 *= rdpnl->FIp[m] / Sd->ali;

					//printf("C1=%lf\n",C1) ;
					if (wall->WallType == 'P')		// 床暖房パネル
						rdpnl->EPt[m] = C1 * rdpnl->Wp * Sd->A;
					else							// 屋根一体型空気集熱器
					{
						if(wall->chrRinput == 'Y')	// 集熱器の特性が熱抵抗で入力されている場合
							kd = Sd->kd ;
						else
							kd = wall->kd ;
						rdpnl->EPt[m] = C1 * rdpnl->cG * rdpnl->Ec * kd ;
						//rdpnl->EPt[m] = C1 * rdpnl->cG * rdpnl->Ec * wall->KdKo ;
					}

					//printf("Ec=%lf  kdo=%lf\n", rdpnl->Ec, wall->KdKo) ;
				//}
				//else
				//	rdpnl->EPt[m] = rdpnl->Ec * wall->KdKo * Sd->FI ;

				//if ( Sd->mw->wall->WallType == 'P' )
				//{
					epr = rdpnl->EPR[m];
					for (j = 0; j < rm->Ntr; j++, epr++)
					{
						*epr = 0.0;
						alr = rm->alr + nn;
						Sdd = rm->rsrf;
						for (n = 0; n < N; n++, alr++, Sdd++)
						{
							if (n != nrp)	          
								*epr += *alr * Sdd->WSRN[j];
							
						}
						if (wall->WallType == 'P')
							*epr *= rdpnl->FIp[m] / Sd->ali * rdpnl->Wp * Sd->A;
						else
						{
							if(wall->chrRinput == 'Y')
								kd = Sd->kd ;
							else
								kd = wall->kd ;

							*epr *= rdpnl->FIp[m] / Sd->ali * rdpnl->cG * rdpnl->Ec * kd ;
							//*epr *= rdpnl->FIp[m] / Sd->ali * rdpnl->cG * rdpnl->Ec * wall->KdKo ;
						}
						
						/*********
						*epr += rdpnl->FOp[m] * Sd->nxsd->alic / Sd->nxsd->ali;
						***********/
					}
					if (wall->WallType == 'P')	// 通常の床暖房パネル
						rdpnl->Epw = rdpnl->Wp * Sd->A * (1.0 - rdpnl->FPp);
					else						// 屋根一体型空気集熱器
					{
						if(wall->chrRinput == 'Y')
							kd = Sd->kd ;
						else
							kd = wall->kd ;

						rdpnl->Epw = rdpnl->cG * (1.0 - rdpnl->Ec * ( 1. - kd *  rdpnl->FPp));
					}
				//}
				//else
				//	rdpnl->Epw = 1. - rdpnl->Ec * ( 1. + wall->KdKo * Sd->FP ) ;
				
				epw = rdpnl->EPW[m];
				for (j = 0; j < rm->Nrp; j++, epw++)
				{
					ew = 0.0;
					alr = rm->alr + nn;
					Sdd = rm->rsrf;
					for (n = 0; n < N; n++, alr++, Sdd++)
					{
						if (n != nrp)
							ew += *alr * Sdd->WSPL[j];
					}	    
					
					if (wall->WallType == 'P')
						*epw = rdpnl->Wp * Sd->A * rdpnl->FIp[m] *ew / Sd->ali;
					else
					{
						if(wall->chrRinput == 'Y')
							kd = Sd->kd ;
						else
							kd = wall->kd ;

						*epw = rdpnl->cG * rdpnl->FIp[m] *ew / Sd->ali * rdpnl->Ec * kd ;
					}
				}
			}
		}
	}
	else
	{
		rdpnl->Epw = 0.0;
		for (m = 0; m < rdpnl->MC; m++)
		{
			rm = rdpnl->rm[m];	
			rdpnl->EPt[m] = 0.0;
			
			epr = rdpnl->EPR[m];
			for (j = 0; j < rm->Ntr; j++, epr++)
				*epr = 0.0;
			
			epw = rdpnl->EPW[m];
			for (j = 0; j < rm->Nrp; j++, epw++)
				*epw = 0.0;
		}
	}
}

/* -------------------------------------------- */

/*  輻射パネルの外乱に関する項の計算     */


double Panelce (RDPNL *rdpnl)
{
	int  N;
	ROOM  *rm;
	RMSRF *Sd, *Sdd;
	MWALL *Mw;
	int    j, nn, m, mp, M, iup, nrp;
	double  CFp, C, *alr, CC, kd, ku;
	WALL *wall ;
	
	Sd = NULL ;
	Mw = NULL ;
	wall = NULL ;
	CC = 0.0 ;

	if (rdpnl->Wp > 0.0)
	{
		for (m = 0; m < rdpnl->MC; m++)
		{
			Sd = rdpnl->sd[m];
			
			if (m == 0)
			{
				Mw = Sd->mw;
				mp = Mw->mp;
				M = Mw->M;
				wall = Mw->wall ;
				
				iup =  mp * M;
				CFp = 0.0; 
				for (j = 0; j < M; j++)
					CFp += Mw->UX[iup + j] * Mw->Told[j];
				
				//if ( Mw->wall->WallType == 'C' )
				//	CFp *= rdpnl->Ec * wall->KdKo ;

				CC = CFp ;
				if ( Mw->wall->WallType == 'C')
				{
					if(Mw->wall->chrRinput == 'Y')
						kd = Sd->kd ;
					else
						kd = Mw->wall->kd ;
					CC = CFp * kd ;
				}
				if (rdpnl->MC == 1)
				{
					if ( Mw->wall->WallType == 'P' )
						CC += rdpnl->FOp[m] * Sd->Te;
					else
					{
						if(wall->chrRinput == 'Y')
						{
							kd = Sd->kd ;
							ku = Sd->ku ;
						}
						else
						{
							kd = Mw->wall->kd ;
							ku = Mw->wall->ku ;
						}
						CC += (ku + kd * rdpnl->FOp[m]) * Sd->Tcoleu;
						//CC += (ku + kd * rdpnl->FOp[m]) * Sd->Te;
						//CC += (1. + Mw->wall->KdKo * rdpnl->FOp[m]) * Sd->Te;
					}
				}
			}
			
			rm = rdpnl->rm[m];
			N = rm->N;
			nrp = (int)(Sd - rm->rsrf);
			nn = N * nrp;
			
			Sdd = rm->rsrf;
			nrp = (int)(Sd - Sdd);
			alr = rm->alr + nn;
			
			C = 0.0;
			for (j = 0; j < N; j++, Sdd++, alr++)
			{
				if (j != nrp)
					C += *alr * Sdd->WSC;
			}
			
			if ( Mw->wall->WallType == 'P' )
				CC += rdpnl->FIp[m] * (Sd->RS + C) / Sd->ali;
			else
			{
				if(wall->chrRinput == 'Y')
				{
					kd = Sd->kd ;
					ku = Sd->ku ;
				}
				else
				{
					kd = Mw->wall->kd ;
					ku = Mw->wall->ku ;
				}
				CC += kd * rdpnl->FIp[m] * (Sd->RS + C) / Sd->ali;
				//CC += Mw->wall->KdKo * rdpnl->FIp[m] * (Sd->RS + C) / Sd->ali;
			}
		}
		
		if ( Mw->wall->WallType == 'P')
			return ( CC * rdpnl->Wp * Sd->A );
		else
			return ( CC * rdpnl->cG * rdpnl->Ec );
    }
	else
		return ( 0.0 );
}

/* --------------------------- */

/* 負荷計算用設定値のポインター */

int rdpnlldsptr(char *load, char **key, RDPNL *Rdpnl, VPTR *vptr, char *idmrk)
{
	int err = 0;
	
	if (strcmp(key[1], "Tout") == 0)
	{
		vptr->ptr = &Rdpnl->Toset;
		vptr->type = VAL_CTYPE;
		Rdpnl->loadt = load;
		*idmrk = 't';
	}
	else
		err = 1;
	return err;
}

/* ------------------------------------------ */  

/* 負荷計算用設定値のスケジュール設定 */

void rdpnlldsschd(RDPNL *Rdpnl)
{
	ELOUT *Eo;
	
	Eo = Rdpnl->cmp->elouts; 
	
	if (Rdpnl->loadt != NULL)
	{
		if (Eo->control != OFF_SW)
		{
			if (Rdpnl->Toset > TEMPLIMIT)
			{
				Eo->control = ON_SW ;
				//Eo->control = LOAD_SW;
				//Eo->sysv = Rdpnl->Toset;
			}
			else
				Eo->control = OFF_SW;
		}	 
	}
}
/* ------------------------------------- */

/*  屋根一体型集熱器内部変数のポインター  */

int rdpnlvptr(char **key, RDPNL *Rdpnl, VPTR *vptr)
{
	int err = 0;
	
	if (Rdpnl->sd[0]->mw->wall->WallType == 'C' 
		&& strcmp(key[1], "Te") == 0)
	{
		vptr->ptr = &Rdpnl->sd[0]->Tcole ;
		vptr->type = VAL_CTYPE;
	}
	else
		err = 1;
	
	return err;
}