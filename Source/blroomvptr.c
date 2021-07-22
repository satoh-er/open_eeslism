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

#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"
#include "psy.h"

/* 室及び関連システム変数、内部変数のポインター  */

int roomvptr(int Nk, char **key, ROOM *Room, VPTR *vptr)
{
	RMSRF *Sd;
	int i;
	
	vptr->ptr = NULL;
	
	if (Nk == 2)
	{
		if (strcmp(key[1], "Tr") == 0)
		{
			vptr->ptr = &Room->Tr;
			vptr->type = VAL_CTYPE;
		}
		else if (strcmp(key[1], "xr") == 0)
		{	 
			vptr->ptr = &Room->xr;
			vptr->type = VAL_CTYPE;
		}
		else if (strcmp(key[1], "RH") == 0)
		{	 
			vptr->ptr = &Room->RH;
			vptr->type = VAL_CTYPE;
		}
		else if (strcmp(key[1], "PMV") == 0)
		{	 
			vptr->ptr = &Room->PMV;
			vptr->type = VAL_CTYPE;
		}
		else if (strcmp(key[1], "Tsav") == 0)
		{	 
			vptr->ptr = &Room->Tsav;
			vptr->type = VAL_CTYPE;
		}
		else if (strcmp(key[1], "Tot") == 0)
		{	 
			vptr->ptr = &Room->Tot;
			vptr->type = VAL_CTYPE;
		}      
		else if (strcmp(key[1], "hr") == 0)
		{
			vptr->ptr = &Room->hr ;
			vptr->type = VAL_CTYPE;
		}
	}
	else if (Nk == 3)
	{
		Sd = Room->rsrf;
		for (i = 0; i < Room->N; i++, Sd++)
		{
			if (Sd->name != NULL && strcmp(key[1], Sd->name) == 0)
			{
				if (strcmp(key[2], "Ts") == 0)
				{
					vptr->ptr = &Sd->Ts;
					vptr->type = VAL_CTYPE;
				}
				else if (strcmp(key[2], "Tmrt") == 0)
				{
					vptr->ptr = &Sd->Tmrt;
					vptr->type = VAL_CTYPE;
				}
				else if (strcmp(key[2], "Te") == 0)
				{
					vptr->ptr = &Sd->Tcole ;
					vptr->type = VAL_CTYPE;
				}
			}
		}
	}
	return (vptr->ptr == NULL);
}

/* ------------------------------------------- */

/* 室負荷計算時の設定値ポインター */

int roomldptr(char *load, char **key, ROOM *Room, VPTR *vptr, char *idmrk)
{
	int		err = 0, i ;
	RMSRF	*Sd ;
	
	if (strcmp(key[1], "Tr") == 0)
	{
		vptr->ptr = &Room->rmld->Tset;
		vptr->type = VAL_CTYPE;
		Room->rmld->loadt = load;
		Room->rmld->tropt = 'a';
		*idmrk = 't';
	}
	else if (strcmp(key[1], "Tot") == 0)
	{
		vptr->ptr = &Room->rmld->Tset;
		vptr->type = VAL_CTYPE;
		Room->rmld->loadt = load;
		Room->rmld->tropt = 'o';
		*idmrk = 't';
	}
	else if (strcmp(key[1], "RH") == 0)
	{
		vptr->ptr = &Room->rmld->Xset;
		vptr->type = VAL_CTYPE;
		Room->rmld->loadx = load;
		Room->rmld->hmopt = 'r';
		*idmrk = 'x';
	} 
	else if (strcmp(key[1], "Tdp") == 0)
	{
		vptr->ptr = &Room->rmld->Xset;
		vptr->type = VAL_CTYPE;
		Room->rmld->loadx = load;
		Room->rmld->hmopt = 'd';
		*idmrk = 'x';
	}   
	else if (strcmp(key[1], "xr") == 0)
	{
		vptr->ptr = &Room->rmld->Xset;
		vptr->type = VAL_CTYPE;
		Room->rmld->loadx = load;
		Room->rmld->hmopt = 'x';
		*idmrk = 'x';
	}
	else if (key[2] != NULL && strcmp(key[2], "Ts") == 0)
	{
		Sd = Room->rsrf ;

		for ( i = 0; i < Room->N; i++, Sd++ )
		{
			if ( Sd->name != NULL && strcmp ( Sd->name, key[1] ) == 0)
			{
				vptr->ptr = &Sd->Ts ;
				vptr->type = VAL_CTYPE ;
				Room->rmld->loadt = load ;
				*idmrk = 't' ;
				err = 0 ;
				break ;
			}
			err = 1 ;
		}
	}
	else
		err = 1;
	
	return err;
}

/* ------------------------------------------- */

/* 室負荷計算時のスケジュール設定 */

void roomldschd(ROOM *Room)
{
	ELOUT  *Eo;
	RMLOAD *rmld;
	
	if ((rmld = Room->rmld) != NULL)
	{      
		Eo = Room->cmp->elouts;      
		if (rmld->loadt != NULL)	 
		{
		/*******
		printf("xxxxx roomldschd  xxx Eo=%d Eoldobj=%d lpath=%c\n",
		Eo, Eo->eldobj, Eo->lpath->control);
			*******/
			
			if (Eo == Eo->eldobj || Eo->eldobj->control != OFF_SW)
			{
				if (rmld->Tset > TEMPLIMIT)
				{
					Eo->sysv = Room->Tr = rmld->Tset;
					Eo->control = LOAD_SW;
				}
				else
				{
					if ( Room->VAVcontrl != NULL )
					{
						Room->VAVcontrl->cmp->control = OFF_SW ;
						Room->VAVcontrl->cmp->elouts->control = OFF_SW ;
					}
				}
			}
		}
		Eo++;
		if (rmld->loadx != NULL)
		{
			if (Eo == Eo->eldobj || Eo->eldobj->control != OFF_SW)
			{
				switch (rmld->hmopt)
				{
				case 'r':	 
					if (rmld->Xset > 0.0)
					{
						Eo->sysv = FNXtr(Room->Tr, rmld->Xset);	       
						Eo->control = LOAD_SW;
					}
					break;
				case 'd':	 
					if (rmld->Xset > TEMPLIMIT)
					{
						Eo->sysv = FNXp(FNPws(rmld->Xset));	       
						Eo->control = LOAD_SW;
					}
					break;
				case 'x':
					if (rmld->Xset > 0.0)
					{	       
						Eo->sysv = rmld->Xset;
						Eo->control = LOAD_SW;
					}
					break;
				}
			}
		}
	}
}
