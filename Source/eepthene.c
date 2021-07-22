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

/*  ee_pthene.c  */


#include "fesy.h"
#include "fnesy.h"
#include "fnesy.h"
#include "fnlib.h"

/* 経路に沿ったシステム要素の熱量計算 */

void Pathheat(int Nmpath, MPATH *Mpath)
{
	PLIST *Pli;
	PELM  *Pelm;
	int   i, j, k;
	double c, cG;
	
	for (i = 0; i < Nmpath; i++, Mpath++)
	{
		c = spcheat(Mpath->fluid);
		Pli = Mpath->plist;
		for (j = 0; j < Mpath->Nlpath; j++, Pli++)
		{
			Pelm = Pli->pelm;
			cG = c * Pli->G;
			for (k = 0; k < Pli->Nelm; k++, Pelm++)
			{
				//printf("Pathheat i=%d j=%d k=%d (%s)\n", i, j, k, Pelm->cmp->name);
				
				if (strcmp(Pelm->cmp->eqptype, DIVERG_TYPE) == 0 ||
					strcmp(Pelm->cmp->eqptype, CONVRG_TYPE) == 0 ||
					strcmp(Pelm->cmp->eqptype, DIVGAIR_TYPE) == 0 ||
					strcmp(Pelm->cmp->eqptype, CVRGAIR_TYPE) == 0 )
					Pelm->out->Q = 0.0;
				else if (Pelm->out->control == OFF_SW)
					Pelm->out->Q = 0.0;
				else
					Pelm->out->Q = cG * (Pelm->out->sysv - Pelm->in->sysvin);
			}
		}
	}
}



