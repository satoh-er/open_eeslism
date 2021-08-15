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
#include "common.h"
#include "fesy.h"
#include "fnfio.h"

//#define  DEBUG  0

void Sysupv(int Nmpath, MPATH *Mpath, RMVLS *Rmvls)
{
	extern int	dayprn ;
	ROOM   *room, *rm;
	RPANEL *rmpnl;
	RDPNL  *Rdpnl;
	int    Nrdpnl;
	PLIST  *Plist;
	PELM   *Pelm;
	ELOUT  *up;
	ELIN   *elin, *elp;
	int    m, i, j, jj;
	
	up = NULL ;

	for (m = 0; m < Nmpath; m++, Mpath++)
	{
		/* 停止要素のシステム方程式からの除外 */
		
		if ( DEBUG )
		{
			printf("\n\n<< Sysupv >> m=%d  MAX=%d\n", m, Nmpath );
		}
		
		Plist = Mpath->plist;
		for (i = 0 ; i < Mpath->Nlpath; i++, Plist++)
		{
			
			if ( DEBUG )
			{
				printf ( "\n<<Sysupv>  i=%d  iMAX=%d\n", i, Mpath->Nlpath ) ;
				printf ( "OFF_SW=%c  Plist->control=%c\n", OFF_SW, Plist->control ) ;
			}
			if ( dayprn && ferr )
			{
				fprintf ( ferr, "\n<<Sysupv>  i=%d  iMAX=%d\n", i, Mpath->Nlpath ) ;
				fprintf ( ferr, "OFF_SW=%c  Plist->control=%c\n", OFF_SW, Plist->control ) ;
			}
			
			if (Plist->control != OFF_SW)
			{
				Pelm = Plist->pelm;
				if (Plist->type == DIVERG_LPTP || Plist->type == CONVRG_LPTP)
					Pelm++;
				
				/***************  Testcode */
				if (Pelm->out != NULL && Pelm->out->control != FLWIN_SW && Pelm->in != NULL)
					up = Pelm->in->upo;
				/******************/
				
				Plist->plmvb = NULL;
				
				for ( j = (int)(Pelm - Plist->pelm); j < Plist->Nelm; j++, Pelm++)
				{
					
					if ( DEBUG )
					{
						printf("\n<< sysupv >> pelm=%d %s  MAX=%d\n",
							j, Pelm->cmp->name,Plist->Nelm);
						
						if ( Pelm->out != NULL )
							printf("<< Sysupv >> Pelm->out->control=%c\n",
							Pelm->out->control);
					}
					if ( dayprn && ferr )
					{
						fprintf(ferr, "\n<< sysupv >> pelm=%d %s  MAX=%d\n",
							j, Pelm->cmp->name,Plist->Nelm);
						
						if ( Pelm->out != NULL )
							fprintf(ferr, "<< Sysupv >> Pelm->out->control=%c\n",
							Pelm->out->control);
					}
					
					if (Pelm->out == NULL)
					{
						Pelm->in->upv = up ;
						
						if (Plist->plmvb == NULL)
							Plist->plmvb = Pelm ;
					}
					else if (Pelm->out->control != OFF_SW)
					{
						
						if ( DEBUG )
						{
							printf ( "<<<<<< Pelm->out->control=%c FLWIN_SW=%c\n",
								Pelm->out->control, FLWIN_SW ) ;
						}

						if ( dayprn && ferr )
						{
							fprintf ( ferr, "<<<<<< Pelm->out->control=%c FLWIN_SW=%c\n",
								Pelm->out->control, FLWIN_SW ) ;
						}
						
						if (Pelm->out->control == FLWIN_SW)
							up = Pelm->out;
						else
						{
							
							if ( DEBUG )
							{
								printf ( "up->cmp->name=%s\n", up->cmp->name ) ;
							}

							if ( dayprn && ferr )
							{
								fprintf ( ferr, "up->cmp->name=%s\n", up->cmp->name ) ;
							}
							
							/********** Testcode ********/
							if ( Pelm->in != NULL )
								Pelm->in->upv = up;
							
							if ( DEBUG )
							{
								printf("<< Sysupv >> pelm=%s up=%s\n",
									Pelm->cmp->name, Pelm->in->upv->cmp->name);
							}
							
							up = Pelm->out;
							if (Plist->plmvb == NULL)
								Plist->plmvb = Pelm;
						}
					}
					else if (Plist->batch == 'y' && j == 0)
						up = Pelm->out;
					else
					{
						if ( DEBUG )
						{
							printf ( "<Sysupv> 1\n" ) ;
						}
						
						if ( Pelm->in != NULL )
							Pelm->in->upv = NULL;
						
						if ( DEBUG )
						{
							printf ( "<Sysupv> 2\n" ) ;
						}
						
					}
				}
				if (Mpath->type == CIR_PTYP)
				{
					Pelm--;	       
					if (Pelm->out->control == OFF_SW)
					{
						Pelm = Plist->plmvb;
						//if ( Pelm != NULL && Pelm->in != NULL )
							Pelm->in->upv = up;
					}
				}
			}
			else
			{
				Pelm = Plist->pelm;
				for (j = 0; j < Plist->Nelm; j++, Pelm++)
					if (Pelm->in != NULL)
						Pelm->in->upv = NULL;
			}
		}
		/* 分岐要素のシステム方程式からの除外 */
		
		Plist = Mpath->plist;
		for (i = 0 ; i < Mpath->Nlpath; i++, Plist++)
		{
			
			if ( DEBUG )
			{
				printf("  Sysupv  BRC  i=%d\n", i);
			}
			
			if (Plist->type == DIVERG_LPTP)
			{
				Pelm = Plist->pelm;
				Pelm->out->control = OFF_SW;	    
				up = Plist->pelm->cmp->elins->upv;
				if ((Pelm = Plist->plmvb) != NULL)
					Pelm->in->upv = up;
			}
		}
   }
#if SIMUL_BUILDG  /*********************************/
   /* 放射パネル設置室についてのパネル上流要素 */
   
   room = Rmvls->Room;
   for (i = 0; i < Rmvls->Nroom; i++, room++)
   {
	   //	   if ( room->Nrp > 0 )
	   rmpnl = room->rmpnl;
	   //	   else
	   //		   rmpnl = NULL ;
	   
	   elin = room->cmp->elins + room->Nachr + room->Ntr;
	   for (j = 0; j < room->Nrp; j++, rmpnl++, elin++)
	   {
		   elp = rmpnl->pnl->cmp->elins;
		   
		   elin->upv = elp->upv;
		   //elin++;
	   }
   }
   
   /************************************/
   
   Nrdpnl = Rmvls->Nrdpnl;
   
   if ( Nrdpnl > 0 )
	   Rdpnl = Rmvls->Rdpnl;
   else
	   Rdpnl = NULL ;
   
	   /***************
   printf("  Sysupv  PANEL  Nrdpnl=%d\n", Nrdpnl); *************/
   
   for (i = 0; i < Nrdpnl; i++, Rdpnl++)
   {
	   for (j = 0; j < Rdpnl->MC; j++)
	   {
		   rm = Rdpnl->rm[j];
		   
		   rmpnl = rm->rmpnl;
		   elin = Rdpnl->cmp->elins + Rdpnl->elinpnl[j];
		   
		   /**************	 
		   printf("Sysupv j=%d  elinpnl=%d Nrp=%d\n",
		   j, Rdpnl->elinpnl[j], Rdpnl->Nrp[j]);
		   ****************/
		   
		   
		   for (jj = 0; jj < Rdpnl->Nrp[j]; jj++, rmpnl++, elin++)
		   {
			   elin->upv = rmpnl->pnl->cmp->elins->upv;
			   
			   /**************
			   printf("Sysupv   upv->name=%s\n", elin->upv->cmp->name);
			   ***************/
			   
			   //elin++;
		   }
		   /****************
		   printf("  Sysupv  PANEL  j=%d\n", j);
		   
			if (Rdpnl->cmp->elins->upv != NULL)
			{
			elin->upv = Rdpnl->cmp->elins->upv;
			
			 
			  printf("Sysupv   upv->name=%s\n",
			  Rdpnl->cmp->elins->upv->cmp->name);
			  }
			  else
			  printf("xxxxx <Sysupv>  Rdpnl->name=%s\n", Rdpnl->name);
		   ***********************/
	   }
   }
   
   
   
#endif
   
   if ( DEBUG )
   {
	   printf("  Sysupv end  ========\n");
   }
   
}
