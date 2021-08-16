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
#include "MODEL.h"

///*
//
//                         直線と平面の交点を求める
//                                    FILE=KOUTEN.c
//                                    Create Date=1998.10.26
//*/

void KOUTEN(double Qx, double Qy, double Qz, double ls, double ms, double ns,
	double* Px, double* Py, double* Pz, XYZ lp, XYZ E)
{
	double t;
	/*********
	 e.X*(x-lp.X)+e.Y*(y-lp.Y)+e.Z*(z-lp.Z)=0 ;
	  e.X*x-e.X*lp.X+e.Y*y-e.Y*lp.Y+e.Z*z-e.Z*lp.Z=0
	 e.X*x+e.Y*y+e.Z*z=e.X*lp.X+e.Y*lp.Y+e.Z*lp.Z
	 e.X*(t*ls+Qx)+e.Y*(t*ms+Qy)+e.Z*(t*ns+Qz)=e.X*lp.X+e.Y*lp.Y+e.Z*lp.Z
	 t*(e.X*ls+e.Y*ms+e.Z*ns)=e.X*lp.X+e.Y*lp.Y+e.Z*lp.Z-e.X*Qx-e.Y*Qy-e.Z*Qz
	*********/

	t = (E.X * lp.X + E.Y * lp.Y + E.Z * lp.Z - E.X * Qx - E.Y * Qy - E.Z * Qz) / (E.X * ls + E.Y * ms + E.Z * ns);
	*Px = t * ls + Qx;
	*Py = t * ms + Qy;
	*Pz = t * ns + Qz;

}
