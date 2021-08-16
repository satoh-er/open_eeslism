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
//						   壁面に対する入射日射角度
//						   FILE=CINC.c
//						   Create Date=1999.6.7
//						   */


void CINC(P_MENN op, double ls, double ms, double ns, double *co){

	double Wz, Ww, Ws;

	Wz = cos(op.wb*M_rad);
	Ww = -sin(op.wb*M_rad)*sin(op.wa*M_rad);
	Ws = -sin(op.wb*M_rad)*cos(op.wa*M_rad);
	*co = ns*Wz + ls*Ww + ms*Ws;


	//printf("op.wb=%f ns=%f ls=%f ms=%f co=%f\n",op->wb,ns,ls,ms,*co) ;
}
