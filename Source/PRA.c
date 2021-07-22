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

                         ベクトルの向きを判定する
                                    FILE=PRA.c
                                    Create Date=1998.8.15

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



void PRA(double *U,double ls,double ms,double ns,double x,double y,double z)
{

 if (fabs(ls)>1.0*pow(10,-6))    
      *U=x/ls ;
 else if(fabs(ms)>1.0*pow(10,-6))
      *U=y/ms ;
 else if(fabs(ns)>1.0*pow(10,-6))
      *U=z/ns ;
 else{
     printf("ls=%f ms=%f ns=%f\n",ls,ms,ns) ;
     printf("errorPRA\n") ;
     exit(1) ;

    }
}
