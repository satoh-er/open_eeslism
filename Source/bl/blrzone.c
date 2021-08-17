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

/* rzone.c */

#include "fesy.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"

/* ゾーン集計実施室の指定  */

void	Rzonedata(FILE *fi, char *dsn, int Nroom, ROOM *Room, int *Nrzone, RZONE *Rzone)
{
	int		i;
	char	s[SCHAR];
	ROOM	*Rm ;
	
	while ( fscanf_s(fi, "%s", s, sizeof(s)), *s != '*')
	{    
		fscanf_s(fi, "%s", s, sizeof(s)) ;
		Rzone->name = stralloc(s);
		Rzone->Nroom = 0;
		Rzone->Afloor = 0.0;
		
		while ( fscanf_s ( fi, "%s", s, sizeof(s)), *s != ';')
		{
			if (( i = idroom ( s, Room, NULL )) < Nroom )
			{
				Rm = Room + i ;
				*Rzone->rm = Rm ;
				Rzone->rm++ ;
				Rzone->Nroom++ ;
				Rzone->Afloor += Rm->FArea ;
			}
			else
				Eprint ( dsn, "<Rzinedata>  room name" ) ; 
		}
		(*Nrzone)++;
		Rzone++;
	}
}

/* -------------------------------------------------------- */

/* 室内熱環境、室負荷のゾーン集計  */

void Rzonetotal(int Nrzone, RZONE *Rzone)
{
	int i, j;
	ROOM *R;
	
	for (i = 0; i < Nrzone; i++, Rzone++)
	{
		Rzone->Tr = Rzone->xr = Rzone->RH = Rzone->Tsav = 0.0;
		Rzone->Qhs = Rzone->Qhl = Rzone->Qht = 0.0;
		Rzone->Qcs = Rzone->Qcl = Rzone->Qct = 0.0;
		
		for (j = 0; j < Rzone->Nroom; j++)
		{
			R = Rzone->rm[j];
			
			Rzone->Tr += R->Tr * R->FArea;
			Rzone->xr += R->xr * R->FArea;
			Rzone->RH += R->RH * R->FArea;
			Rzone->Tsav += R->Tsav * R->FArea;
			if (R->rmld != NULL)
			{
				if (R->rmld->Qs > 0.0)
					Rzone->Qhs += R->rmld->Qs;
				else
					Rzone->Qcs += R->rmld->Qs;
				
				if (R->rmld->Ql > 0.0)	       
					Rzone->Qhl += R->rmld->Ql;
				else
					Rzone->Qcl += R->rmld->Ql;
				
				if (R->rmld->Qt > 0.0)	      	    	       
					Rzone->Qht += R->rmld->Qt;
				else
					Rzone->Qct += R->rmld->Qt;
			}
		}
		Rzone->Tr /= Rzone->Afloor;
		Rzone->xr /= Rzone->Afloor;
		Rzone->RH /= Rzone->Afloor;
		Rzone->Tsav /= Rzone->Afloor;
	}
}
