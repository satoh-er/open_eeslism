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
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnbld.h"

/* 室供給熱量、放射パネルについての出力 */

void Rmpnlprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
		int Nroom, ROOM *Room)
{
   int j;
   static int id = 0;
   
   if (id == 0)
   {      
      ttlprint(fo, mrk, Simc);
      
      for (j = 0; j < 2; j++)
      {
	 if (j == 0)
	    fprintf(fo, "-cat\n");
         rmqaprint(fo, id, Nroom, Room);
	 if (j == 0)
	    fprintf(fo, "*\n#\n");
         id++;
      }
   }
   
   fprintf(fo, "%02d %02d %5.2lf\n", mon, day, time);
   rmqaprint(fo, id, Nroom, Room);
}


