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

                      OP,LPの座標出力（デバッグ用）
                                 FILE=ZPRINT.c
                                 Create Date=1999.10.26

*/


#include <string.h>
#include <stdlib.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnlib.h"
#include "fnesy.h"
#include "winerror.h"
#include "fnfio.h"
#include "lib/u_psy.h"
#include "MODEL.h"
#include "func/ZPRINT.h"
#include "common.h"

void ZPRINT(P_MENN *lp, P_MENN *op, int lpn, int opn, char *name)
{
int i, k, j ;
FILE *fp ;
char fname[SCHAR];

strcpy_s(fname, sizeof(fname), name);
strcat_s(fname, sizeof(fname), ".gchi") ;

if (fopen_s(&fp, fname,"w") != 0){
   printf("File not open\n") ;
   		preexit ( ) ;
		exit(EXIT_ZPRI); 
   //exit(1) ;
}

for(i=0 ;i<opn ;i++){
  fprintf(fp,"op[%d] %s\n",i,op[i].opname) ;
  for(j=0 ;j<op[i].polyd ;j++){
    fprintf(fp,"    P[%d] X=%f Y=%f Z=%f\n",j,op[i].P[j].X,
                                    op[i].P[j].Y,op[i].P[j].Z) ;
  }
  for(k=0 ;k<op[i].wd ;k++){
    fprintf(fp,"op[%d] opw[%d] %s\n ",i,k,op[i].opw[k].opwname) ;
	for(j=0 ;j<4 ;j++){
      fprintf(fp,"   P[%d] X=%f Y=%f Z=%f\n",j,op[i].opw[k].P[j].X,
                                    op[i].opw[k].P[j].Y,op[i].opw[k].P[j].Z) ;
    }
  }
fprintf(fp,"\n") ;
}
for(i=0 ;i<lpn ;i++){
  fprintf(fp,"lp[%d] %s\n ",i,lp[i].opname) ;
  fprintf(fp,"      e.X=%f e.Y=%f e.Z=%f\n",lp[i].e.X,lp[i].e.Y,lp[i].e.Z) ;
  for(j=0 ;j<lp[i].polyd ;j++){
    fprintf(fp,"   P[%d] X=%f Y=%f Z=%f\n",j,lp[i].P[j].X,
                                          lp[i].P[j].Y,lp[i].P[j].Z) ;
  }
fprintf(fp,"\n") ;
}
fclose(fp) ;
}
