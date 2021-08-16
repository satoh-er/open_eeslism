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

								  小数点の切捨て
									   FILE=CAT.c
									   Create Date=1999.6.7
*/
#include <stdio.h>
#include <math.h>


void CAT(double *a, double *b, double *c)
{

	*a = floor(((*a) * 10000.0) + 0.5);
	*b = floor(((*b) * 10000.0) + 0.5);
	*c = floor(((*c) * 10000.0) + 0.5);

	/*
	*a = rint(*a) / 10000.0 ;
	*b = rint(*b) / 10000.0 ;
	*c = rint(*c) / 10000.0 ;
	*/

	*a = (*a) / 10000.0;
	*b = (*b) / 10000.0;
	*c = (*c) / 10000.0;

	if ((*a) == -0.0)
		*a = 0.0;
	if ((*b) == -0.0)
		*b = 0.0;
	if ((*c) == -0.0)
		*c = 0.0;
}

