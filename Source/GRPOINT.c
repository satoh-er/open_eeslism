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
#include <math.h>
#include "wthrd.h"
#include "MODEL.h"
#include "exs.h"
#include "FUNC.h"

///*
//
//                      前面地面の代表点を求める
//                      GDATA():MP面の中心点を求める
//                                        FILE=GRGPOINT.c
//                                        Create Date=1999.11.1
//
//*/

void GRGPOINT(P_MENN *mp, int mpn)
{
  int i ;
  double t=0.0 ;
  double ex=0.0 ;
  double ey=0.0 ;
  double ez=0.0 ;

  for(i=0 ; i<mpn ; i++){
    GDATA(mp[i],&mp[i].G) ;

    ex=mp[i].P[1].X-mp[i].P[0].X ;
    ey=mp[i].P[1].Y-mp[i].P[0].Y ;
    ez=mp[i].P[1].Z-mp[i].P[0].Z ;

    if(fabs(ez)<1.0*pow(10.0,-6)){
      mp[i].grp.X=0.0 ;
      mp[i].grp.Y=0.0 ;
      mp[i].grp.Z=0.0 ;
      continue ;
    }

    else{
      t=-mp[i].G.Z/ez ;
      mp[i].grp.X=t*ex+mp[i].G.X-mp[i].grpx*sin(mp[i].wa*rad) ;
      mp[i].grp.Y=t*ey+mp[i].G.Y-mp[i].grpx*cos(mp[i].wa*rad) ;
      mp[i].grp.Z=0.0 ;
      t=0.0 ;
      ex=0.0 ; ey=0.0 ; ez=0.0 ;
    }
  }
}
