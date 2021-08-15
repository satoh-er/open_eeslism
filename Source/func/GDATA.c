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

						 壁面の中心点の座標を求める
										FILE=GDATA.c
										Create Date=1999.10.26

*/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include "MODEL.h"


void GDATA(P_MENN OP, XYZ *G)
{
	//double Amx, Amy, Amz, Bmx, Bmy, Bmz;
	int i;
	double x, y, z;

	x = 0; y = 0; z = 0;
	for (i = 0; i < OP.polyd; i++) {
		x = x + OP.P[i].X;
		y = y + OP.P[i].Y;
		z = z + OP.P[i].Z;
	}

	G->X = x / (double)OP.polyd;
	G->Y = y / (double)OP.polyd;
	G->Z = z / (double)OP.polyd;
}
