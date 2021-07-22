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

                        壁面の法線ベクトルを求める
                                      FILE=EOP.c
                                      Create Date 1999.6.15
				     
 */

#include <stdio.h>
#include <math.h>
#include "esize.h"
#include "fesy.h"
#include "MODEL.h"
//#include "exs.h"
#include "FUNC.h" 

void EOP(int u,P_MENN *p)
{
int j ;

for (j=0 ;j<u ;j++){
 p[j].e.Z=cos(p[j].wb*rad) ;
 p[j].e.Y=-sin(p[j].wb*rad)*cos(p[j].wa*rad) ;
 p[j].e.X=-sin(p[j].wb*rad)*sin(p[j].wa*rad) ;
 CAT(&p[j].e.Z,&p[j].e.Y,&p[j].e.X) ;
}
}
