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

/* rmvent.c  */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnesy.h"
#include "fnlib.h"
#include "fnfio.h"

/* ------------------------------------------------------------------ */

/*  外気導入量および室間相互換気量の設定スケジュ－ル入力   */

void Ventdata(FILE *fi, char *dsn, SCHDL *Schdl, ROOM *Room, SIMCONTL *Simc )
{
	ACHIR	*achr;
	ROOM	*room, *Rm ;
	char	name1[SCHAR], name2[SCHAR], s[SCHAR], ss[SCHAR], 
		err[SCHAR+128], E[SCHAR], c, *ce, *st;
	double	val, *vall;
	int		i, j, v, k ;
	
	vall = Schdl->val ;

	sprintf (E, ERRFMT, dsn);
	while ( fscanf ( fi, " %s ", name1 ), *name1 != '*' )
	{
		strcat ( strcpy ( err, E ), name1 ) ;
		i = idroom ( name1, Room, err ) ;
		Rm = Room + i ;
		
		fscanf ( fi, " %s ", s ) ;
		if (( st = strchr ( s, '=' )) != 0 )
		{
			while ( s )
			{
				*st = '\0';
				if ( strcmp( s, "Vent") == 0 )
				{
					sscanf ( st + 1, "(%lf,%[^)])", &val, ss ) ;
					Rm->Gve = val;

					if (( k = idsch(ss, Schdl->Sch, NULL )) >= 0 )
						Rm->Vesc = &vall[k] ;
					else
						Rm->Vesc = envptr(ss, Simc, 0, NULL, NULL, NULL) ;

					//Rm->vesc = idsch(ss, Sch, err);
					/******** 
					printf("Ventdata   room=%s  %s   %lf  %s\n",
					Rm->name, "Vent", val, ss);
					****/
				}
				else if (strcmp(s, "Inf") == 0)
				{
					sscanf(st+1, "(%lf,%[^)])", &val, ss);
					Rm->Gvi = val;

					if (( k = idsch(ss, Schdl->Sch, NULL )) >= 0 )
						Rm->Visc = &vall[k] ;
					else
						Rm->Visc = envptr(ss, Simc, 0, NULL, NULL, NULL) ;

					//Rm->visc = idsch(ss, Sch, err);
					/********
					printf("Ventdata   room=%s  %s   %lf  %s\n",
					Rm->name, "Inf ", val, ss);
					****/
				}
				else
				{
					sprintf ( err, "Room=%s  %s", Rm->name, s ) ;
					Eprint ( "<Ventedata>", err ) ;
				}
				
				if ( strchr ( st + 1, ';') != 0)
					break;
				fscanf ( fi, " %s ", s ) ;
				if ( *s == ';')
					break ;
				st = strchr ( s, '=' ) ;
			}
		}
		else
		{
			c = *s ; 
			fscanf ( fi, " %s v= %s ;", name2, s ) ;
			if ( c == ';' ) break ;
			/******************
			printf("Aichdata  %s (%c)  %s\n", name1, c, name2);
			************/
			strcat ( strcpy ( err, E ), name2 ) ;
			j = idroom ( name2, Room, err ) ;
			
			if (( ce = strchr ( s, ';' )) != 0 )
				*ce = '\0' ;
			strcat ( strcpy ( err, E ), s ) ;
			v = idsch ( s, Schdl->Sch, err ) ;

			switch ( c )
			{
			/************************
			case '>':
			room = Room + j;
			achr = room->achr++;
			achr->rm = i;
			achr->sch = v;
			room->Nachr++;
			break;
			
			 case '<':
			 room = Room + i;
			 achr = room->achr++;
			 achr->rm = j;
			 achr->sch = v;
			 room->Nachr++;
			 break;
				***********************/
				
			case '-':
				room = Room + j;
				achr = room->achr + room->Nachr;
				achr->rm = i;
				achr->room = Room + i;
				achr->sch = v;
				room->Nachr++;
				
				room = Room + i;
				achr = room->achr + room->Nachr;
				achr->rm = j;
				achr->room = Room + j;
				achr->sch = v;
				room->Nachr++;	      
				break;
			}
		}
    } 
}

/* ------------------------------------------------------------------ */


/*  室間相互換気量の設定   */

void Aichschdlr(double *val, int Nroom, ROOM *Room)
{
	int		i ;
	ACHIR	*achr ;
	double	*v ;

	achr = NULL ;
	
	while (Nroom--)
	{
		achr = Room->achr;
		
		for (i = 0; i < Room->Nachr; i++, achr++)
		{
			v = val + achr->sch ;
			if ( *v > 0.0)
				achr->Gvr = *v ;
			else 
				achr->Gvr =  0.0;
		}
		Room++;
	}
}
