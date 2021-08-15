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
#include "lib/u_sun.h"
#include "lib/spline.h"

// 給水温度のスプライン補間

// 各月の給水温度は15日の給水温度として補間する
// Nday:計算対象の通日、Tsupw:1～12月の給水温度
double	Intgtsup(int Nday, double *Tsupw )
{
	static double h[13], b[13], d[13], g[13], u[13], r[14], x[14], y[14] ;
	static int	ic = 0, n ;

	double	y1 ;
	int		Mo ;

	if (ic == 0)
	{
		n = 13 ;
		x[0] = -15, x[13] = 380 ;
		for(Mo = 1; Mo <= 12; Mo++)
		{
			x[Mo] = FNNday(Mo, 15) ;
			y[Mo] = Tsupw[Mo-1] ;
			y[0] = Tsupw[11], y[13] = Tsupw[0] ;
		}

		ic = 1 ;
	}

	y1 = spline(n, x, y, Nday, h, b, d, g, u, r ) ;
	return (y1) ;
}

/**************************************/
/*     ３次スプライン関数による補間   */
/*          n : 区間の数              */
/*          x, y : 点の座標           */
/*          x1 : 補間値を求める値     */
/*          h, b, d, g, u, r : 作業域 */
/*          return : 補間値           */
/*          coded by Y.Suganuma       */
/**************************************/
// http://www.sist.ac.jp/~suganuma/kougi/other_lecture/SE/num/spline/C++/spline.txt

double spline(int n, double *x, double *y, double x1,
              double *h, double *b, double *d, double *g, double *u, double *r)
{
	int i = -1, i1, k;
	double y1, qi, si, xx;
					// 区間の決定
	for (i1 = 1; i1 < n && i < 0; i1++) {
		if (x1 < x[i1])
			i = i1 - 1;
	}
	if (i < 0)
		i = n - 1;
					// ステップ１
	for (i1 = 0; i1 < n; i1++)
		h[i1] = x[i1+1] - x[i1];
	for (i1 = 1; i1 < n; i1++) {
		b[i1] = 2.0 * (h[i1] + h[i1-1]);
		d[i1] = 3.0 * ((y[i1+1] - y[i1]) / h[i1] - (y[i1] - y[i1-1]) / h[i1-1]);
	}
					// ステップ２
	g[1] = h[1] / b[1];
	for (i1 = 2; i1 < n-1; i1++)
		g[i1] = h[i1] / (b[i1] - h[i1-1] * g[i1-1]);
	u[1] = d[1] / b[1];
	for (i1 = 2; i1 < n; i1++)
		u[i1] = (d[i1] - h[i1-1] * u[i1-1]) / (b[i1] - h[i1-1] * g[i1-1]);
					// ステップ３
	k      = (i > 1) ? i : 1;
	r[0]   = 0.0;
	r[n]   = 0.0;
	r[n-1] = u[n-1];
	for (i1 = n-2; i1 >= k; i1--)
		r[i1] = u[i1] - g[i1] * r[i1+1];
					// ステップ４
	xx = x1 - x[i];
	qi = (y[i+1] - y[i]) / h[i] - h[i] * (r[i+1] + 2.0 * r[i]) / 3.0;
	si = (r[i+1] - r[i]) / (3.0 * h[i]);
	y1 = y[i] + xx * (qi + xx * (r[i]  + si * xx));

	return y1;
}
