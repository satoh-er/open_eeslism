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

/*   sysvar.c  */

#include <string.h>
#include "MODEL.h"   /*-----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnesy.h"

void Sysvar(int Ncompnt, COMPNT *Compnt)
{
   int     m, i;
   //COMPNT  *C;
   ELIN    *I;
   
   for (m = 0; m < Ncompnt; m++, Compnt++)
   {
	   //C = Compnt;
	   I = Compnt->elins;
	   
	   for (i = 0; i < Compnt->Nin; i++, I++)
	   {
		   /*****printf("  Sysvar  m=%d (%s) i=%d\n", m, Compnt->name, i);***/	 
		   /* =================================================== */
		   I->sysvin = 0.0;
		   if(I->upv != NULL)
			   I->sysvin = I->upv->sysv;
		   
			   /* ====================
			   else if (I->lpath != NULL)	 
			   if (I->lpath->flwin != NULL)
			   I->sysvin = *(I->lpath->flwinval);
		   =========================== */
	   }

   }
}
