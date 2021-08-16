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

/*

                         構造体LP、OP`の初期化
                                    FILE=MAT_INIT.c
                                    Create Date=1999.5.25

*/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "MODEL.h"


void MATINIT(P_MENN *q,int n)
{
int i,j ;

for (i=0 ;i<n ;i++){
   q[i].e.X=0.0 ;  q[i].e.Y=0.0 ; q[i].e.Z=0.0 ;
  for (j=0 ;j<q[i].polyd ;j++){
 q[i].P[j].X=0.0 ; q[i].P[j].Y=0.0 ; q[i].P[j].Z=0.0 ;
}
}

} 
/*----------------------------------------------------------------*/
void MATINIT_sum(int opn,P_MENN *op){

int j, i ;
 for(j=0 ;j<opn ;j++){
            op[j].sum=0.0 ; 
            for (i=0 ;i<op[j].wd ;i++){
                op[j].opw[i].sumw=0.0 ;                   
	    }
 }
}
/*----------------------------------------------------------------*/
void MATINIT_sdstr(int mpn,int mtb,SHADSTR *Sdstr)
{

int j, i ;
 for(j=0 ;j<mpn ;j++){
	 for(i=0 ;i<mtb ;i++)
        Sdstr[j].sdsum[i]=0.0 ; 
 }

}


