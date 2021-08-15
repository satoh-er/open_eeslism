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

/*  u_mlib.c  */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "common.h"
#include "lib/u_mlib.h"
#include "lib/u_ees3lib.h"


/*  水、空気の比熱 */
/* ---------------------- */

double spcheat(char fluid)
{
   extern double ca, cw;
   char		s[SCHAR];

   if ( fluid == 'W' )
      return ( cw ) ;
   else if ( fluid == 'a' )
      return ( ca ) ;
   else
   {
      sprintf(s, "xxx fluid='%c'", fluid);
      Eprint("<spcheat>", s); 
      return(-9999.0);
   }
}
