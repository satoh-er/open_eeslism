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

#include <math.h>

#include "fesy.h"
#include "fnfio.h"


/* -------------------------------------------------- */

/* 地中温度の計算 */

double Tearth (double Z, int n, int nmx, double Tgro, double DTg, double a)
{
	extern double PI;
	
	double Cz ;
	static double	t = 31.536e+6 ;
	
	Cz = Z * sqrt ( PI / ( a * t )) ;
	return ( Tgro + 0.5 * DTg * exp ( - Cz ) * cos (( n - nmx ) * 0.017214 - Cz ));
}

/* -------------------------------------------------- */

void Exsfter (int day, int daymx, double Tgrav, double DTgr, EXSF *Exs,WDAT *Wd, int tt)
{
	int i, Ne ;

	if ( Exs != NULL )
	{
		Ne = Exs->end ;
		
		for ( i = 0; i < Ne; i++, Exs++ )
		{
			if (Exs->typ == 'E')
				Exs->Tearth = Tearth (Exs->Z, day, daymx, Tgrav, DTgr, Exs->erdff);
			else if(Exs->typ == 'e')
				Exs->Tearth = Wd->EarthSurface[day*24+tt] ;
		}
	}
}
