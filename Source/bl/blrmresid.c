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

/*  rmresid.c   */

#include <stdio.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnesy.h"
#include "fnlib.h"
#include "fnfio.h"

/* --------------------------------------------------------- */
/*
居住者スケジュ－ルの入力              */

void	Residata (FILE *fi, char *dsn, SCHDL *Schdl, ROOM *Room, int *pmvpri, SIMCONTL *Simc)
{
	int		i, k ;
	char	s[SCHAR], ss[SCHAR], sss[SCHAR], s4[SCHAR], *st, *ce,
		err[SCHAR+128], Er[SCHAR];
	ROOM	*rm ;
	double	*vall ;
	extern int SETprint;
	
	*s = *ss = *sss = *s4 = *err = *Er = '\0' ;

	sprintf_s (Er, sizeof(Er), ERRFMT, dsn);
	vall = Schdl->val ;
	
	while (fscanf_s(fi, "%s", s, sizeof(s)), *s != '*')
	{
		strcpy_s(err, sizeof(err), Er);
		strcat_s(err, sizeof(err), s); 
		i = idroom ( s, Room, err ) ;
		rm = Room + i ;
		
		while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != ';')
		{
			strcpy_s(err, sizeof(err), Er);
			strcat_s(err, sizeof(err), s); 
			ce = strchr(s, ';');
			st = strchr(s, '=');
			*st = '\0';
			
			if (strcmp(s, "H") == 0)
			{
				sscanf_s(st+1, "(%lf,%[^,],%[^)])", &rm->Nhm, ss, sizeof(ss), sss, sizeof(sss));
				//rm->hmnsc = idsch(ss, Sch, err);

				if (( k = idsch(ss, Schdl->Sch, NULL )) >= 0 )
					rm->Hmsch = &vall[k] ;
				else
					rm->Hmsch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;

				//rm->hmwksc = idsch(sss, Schdl->Sch, err);

				if (( k = idsch(sss, Schdl->Sch, NULL )) >= 0 )
					rm->Hmwksch = &vall[k] ;
				else
					rm->Hmwksch = envptr ( sss, Simc, 0, NULL, NULL, NULL ) ;
			}
			else if (strcmp(s, "comfrt") == 0)
			{
				sscanf_s(st+1, "(%[^,],%[^,],%[^)])", ss, sizeof(ss), sss, sizeof(sss), s4, sizeof(s4));
				//rm->metsc = idsch(ss, Sch, err);

				if (( k = idsch(ss, Schdl->Sch, NULL )) >= 0 )
					rm->Metsch = &vall[k] ;
				else
					rm->Metsch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;

				//rm->closc = idsch(sss, Sch, err);

				if (( k = idsch(sss, Schdl->Sch, NULL )) >= 0 )
					rm->Closch = &vall[k] ;
				else
					rm->Closch = envptr ( sss, Simc, 0, NULL, NULL, NULL ) ;

				//rm->wvsc = idsch(s4, Sch, err);

				if (( k = idsch(s4, Schdl->Sch, NULL )) >= 0 )
					rm->Wvsch = &vall[k] ;
				else
					rm->Wvsch = envptr ( s4, Simc, 0, NULL, NULL, NULL ) ;

				*pmvpri = 1;
				if (SETprint == 1)
					rm->setpri = 1;
			}
			else
			{
				sprintf_s(err, sizeof(err), "Room=%s  %s", rm->name, s );
				Eprint ( "<Residata>", s ) ;
			}
			
			if (ce != 0)
				break;
		}
	}
}
/* --------------------------------------------------------- */
/*
照明・機器利用スケジュ－ルの入力              */

void	Appldata (FILE *fi, char *dsn, SCHDL *Schdl, ROOM *Room, SIMCONTL *Simc)
{
	int		i, k ;
	char	s[SCHAR], ss[SCHAR], *st, *ce,
		err[SCHAR+128], Er[SCHAR];
	ROOM	*rm ;
	double	*vall ;
	
	*s = *ss = *err = *Er = '\0' ;
	vall = Schdl->val ;

	sprintf_s (Er, sizeof(Er), ERRFMT, dsn);
	
	while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != '*')
	{
		strcpy_s(err, sizeof(err), Er);
		strcat_s(err, sizeof(err), s); 
		i = idroom(s, Room, err);
		rm = Room + i ;
		
		while (fscanf_s(fi, "%s", s, sizeof(s)), s[0] != ';')
		{
			ce = strchr(s, ';');
			st = strchr(s, '=');
			*st = '\0';
			
			if (strcmp(s, "L") == 0)
            {
				sscanf_s(st+1, "(%lf,%c,%[^)])", &rm->Light, &rm->Ltyp, 1, ss, sizeof(ss)); 

				if (( k = idsch ( ss, Schdl->Sch, NULL )) >= 0 )
					rm->Lightsch = &vall[k] ;
				else
					rm->Lightsch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;

				// rm->lgtsc = idsch(ss, Sch, err);
			}
			else if (strcmp(s, "As") == 0)
            {
				sscanf_s(st+1, "(%lf,%lf,%[^)])", &rm->Apsc, &rm->Apsr, ss, sizeof(ss));

				if (( k = idsch ( ss, Schdl->Sch, NULL )) >= 0 )
					rm->Assch = &vall[k] ;
				else
					rm->Assch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;

				// rm->apssc = idsch(ss, Sch, err); 
			}
			else if (strcmp(s, "Al") == 0)
            {
				sscanf_s(st+1, "(%lf,%[^)])", &rm->Apl, ss, sizeof(ss));

				if (( k = idsch ( ss, Schdl->Sch, NULL )) >= 0 )
					rm->Alsch = &vall[k] ;
				else
					rm->Alsch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;

				//rm->aplsc = idsch(ss, Sch, err);
			}
			else if (strcmp(s, "AE") == 0)
            {
				sscanf_s(st+1, "(%lf,%[^)])", &rm->AE, ss, sizeof(ss));

				if (( k = idsch ( ss, Schdl->Sch, NULL )) >= 0 )
					rm->AEsch = &vall[k] ;
				else
					rm->AEsch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;
			}
			else if (strcmp(s, "AG") == 0)
            {
				sscanf_s(st+1, "(%lf,%[^)])", &rm->AG, ss, sizeof(ss));

				if (( k = idsch ( ss, Schdl->Sch, NULL )) >= 0 )
					rm->AGsch = &vall[k] ;
				else
					rm->AGsch = envptr ( ss, Simc, 0, NULL, NULL, NULL ) ;
			}
			else
			{
				sprintf_s ( err, sizeof(err), "Room=%s  %s", rm->name, s ) ;
				Eprint ( "<Appldata>", err ) ;
			}
			
			if (ce != 0)
				break;
		}
	}
}

