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

							 ベクトルの向き
								 FILE=YOGEN.c
								 Create Date=1999.6.7
							 内積の計算

*/

#include <stdio.h>
#include <math.h>
#include "MODEL.h"
#include "func/YOGEN.h"

void YOGEN(double Qx, double Qy, double Qz, double Px, double Py, double Pz, double *S, XYZ e)
{
	double PQx, PQy, PQz, PQ, E;
	void CAT(double *a, double *b, double *c);

	PQx = Px - Qx;
	PQy = Py - Qy;
	PQz = Pz - Qz;

	//printf("1PQx=%f PQy=%f PQz=%f\n", PQx, PQy, PQz);

	CAT(&PQx, &PQy, &PQz);  //20170422 higuchi add

	//printf("2PQx=%f PQy=%f PQz=%f\n", PQx, PQy, PQz);

	PQ = sqrt(PQx*PQx + PQy * PQy + PQz * PQz);
	E = sqrt(e.X*e.X + e.Y*e.Y + e.Z*e.Z);

	// ↓条件文にした。　20170422 higuchi add
	if (PQ == 0.0) {
		*S = -777;
	}
	else {
		*S = (PQx*e.X + PQy * e.Y + PQz * e.Z) / (PQ*E);
	}

}
