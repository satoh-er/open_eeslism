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

/* egcalc.h */

#pragma once

typedef struct ptotal  /* 系統、機器種別の積算値 */
{
   char  *name;
   int   Ncomp;
   struct compnt **cmp;
   
   double  Qhs, Qhl, Qht,
          Qcs, Qcl, Qct,
	  E, E2,
	  Egen,
          Eprch,
	  Esout,	  
	  G,
	  O;	    
   
   struct qday  Qsdy,
                Qldy,
		Qtdy;
   
   struct eday  Edy, E2dy,
	        Egendy,
                Eprchdy,
	        Esoutdy,	  
	        Gdy,
	        Ody;
} PTOTAL;
   	  


