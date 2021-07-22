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

/* envptr.c  */


#include <stdlib.h>
#include <string.h>
#include "fesy.h"
#include "fnesy.h"
#include "fnlib.h"

/*  システム要素周囲条件（温度など）のポインター  */

double *envptr(char *s, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd, EXSFS *Exsf)
{
	int   err;
	VPTR  vptr, pdmy;
	MPATH dmy;
	double *val;
	
	val = NULL;
	
	if (isstrdigit(s))
	{
		val = dcalloc(1, "<*envptr>");
		*val = atof(s);
	}
	else
	{
		err = kynameptr(s, Simc, Ncompnt, Compnt, 0, &dmy, Wd, Exsf, &vptr, &pdmy);
		if (err == 0 && vptr.type == VAL_CTYPE)
			val = vptr.ptr;
		else
			Eprint("<*envptr>", s);
	}
	
	if ( val == NULL )
		printf ("xxxx  %s\n", s ) ;

	return val;
}

/* Sato Create  1999/9/22  */

ROOM   *roomptr ( char *s, int Ncompnt, COMPNT *Compnt )
{
	int    i ;
	ROOM   *rm ;
	
	rm = NULL ;
	
	for ( i = 0; i < Ncompnt; i++, Compnt++ )
		if ( s != NULL && Compnt->name != NULL && strcmp (s, Compnt->name) == 0 )
			rm = (ROOM *)Compnt->eqp ;
		
		return rm ;
}

/*********** Satoh Create  2001/5/3 ********************/
void   *hccptr ( char c, char *s, int Ncompnt, COMPNT *Compnt, char *m )
{
	int    i ;
	void   *h ;
	
	h = NULL ;
	
	for ( i = 0; i < Ncompnt; i++, Compnt++ )
	{
		if ( s != NULL && Compnt->name != NULL
			&& strcmp (s, Compnt->name) == 0 )
		{
			if ( c == 'c' && strcmp (Compnt->eqptype, HCCOIL_TYPE) == 0)
			{
				h = (HCC *)Compnt->eqp ;
				*m = 'c' ;
				return h ;
			}
			else if ( c == 'h' && strcmp (Compnt->eqptype, HCLOADW_TYPE) == 0)
			{
				h = (HCLOAD *)Compnt->eqp ;
				*m = 'h' ;
				return h ;
			}
		}
	}
	
	return h ;
}

/*********** Satoh Create  2003/5/17 ********************/
/* 放射パネルの検索 */

void   *rdpnlptr ( char *s, int Ncompnt, COMPNT *Compnt )
{
	int    i ;
	void   *h ;
	
	h = NULL ;
	
	for ( i = 0; i < Ncompnt; i++, Compnt++ )
	{
		if ( s != NULL && Compnt->name != NULL
			&& strcmp (s, Compnt->name) == 0 )
		{
			if ( strcmp (Compnt->eqptype, RDPANEL_TYPE) == 0)
			{
				h = (RDPNL *)Compnt->eqp ;

				return h ;
			}
		}
	}
	
	return h ;
}
