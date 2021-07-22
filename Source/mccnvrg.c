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

/* cnvrg.c  */

#include "fesy.h"
#include "fnmcs.h"

/* 合流要素 */

void Cnvrgcfv(int Ncnvrg, COMPNT **Cnvrg)
{
	COMPNT *C;
	ELOUT *E;
	ELIN  *I;
	int i, j;
	double *cfin ; // G ;
	
	for (i = 0; i < Ncnvrg; i++ )
	{
		//G = 0.0 ;
		C = Cnvrg[i];
		E = C->elouts;
		
		// 経路が停止していなければ
		if ( E->control != OFF_SW )
		{
			E->coeffo = E->G;
			E->Co = 0.0 ;
			cfin = E->coeffin;
			I = C->elins;
			
			if ( I->lpath != NULL )
			{
				for (j = 0; j < C->Nin; j++, I++, cfin++)
				{
					//G += I->lpath->G ;
					// *cfin++ = -I->lpath->G;
					*cfin = -I->lpath->G ;
					/***********************
					printf("cfin=%lf G=%lf\n", *cfin, I->lpath->G) ;
					/***************************/
					//cfin++ ;
				}
			}
			//E->coeffo = G ;
		}
	}
}

