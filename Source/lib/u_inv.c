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

/*  inv.c  */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "fesy.h"
#include "fnlib.h"
#include "winerror.h"
#include "lib/u_inv.h"

///* ------------------------------------------------ 
//
// 逆行列の計算
// 
//*/



/* ========= ガウスジョルダン法の関数====================== */

void	matinv(double *a, int n, int m, char *s)
{
	int		ipv, i, j;
	double	inv_pivot, temp;
	double	*mattemp;
	double	big;
	int		pivot_row, *row;
	char	E[SCHAR];

	row = icalloc(m, "<matinv>");
	mattemp = dcalloc(m * m, "<matinv>");

	matcpy(a, mattemp, m * m);

	for (ipv = 0; ipv < m; ipv++)
	{
		/* ---- 最大値探索 ---------------------------- */
		big = 0.0;
		for (i = ipv; i < m; i++)
		{
			if (fabs(a[i*n + ipv]) > big)
			{
				big = fabs(a[i*n + ipv]);
				pivot_row = i;
			}
		}
		if (big == 0.0)
		{
			if (s != NULL)
				sprintf_s(E, sizeof(E), "対角要素に０があります  matrix=%dx%d  i=%d  [%s]", m, m, i, s);
			else
				sprintf_s(E, sizeof(E), "対角要素に０があります  matrix=%dx%d  i=%d", m, m, i);

			matprint("%.2g  ", m, mattemp);
			free(mattemp);
			free(row);
			Eprint("<matinv>", E);
			preexit();
			exit(EXIT_MATINV);
		}
		row[ipv] = pivot_row;

		/* ---- 行の入れ替え -------------------------- */
		if (ipv != pivot_row)
		{
			for (i = 0; i < m; i++)
			{
				temp = a[ipv*n + i];
				a[ipv*n + i] = a[pivot_row*n + i];
				a[pivot_row*n + i] = temp;
			}
		}

		/* ---- 対角成分=1(ピボット行の処理) ---------- */
		inv_pivot = 1.0 / a[ipv*n + ipv];
		a[ipv*n + ipv] = 1.0;
		for (j = 0; j < m; j++)
			a[ipv*n + j] *= inv_pivot;

		/* ---- ピボット列=0(ピボット行以外の処理) ---- */
		for (i = 0; i < m; i++)
		{
			if (i != ipv)
			{
				temp = a[i*n + ipv];
				a[i*n + ipv] = 0.0;
				for (j = 0; j < m; j++)
					a[i*n + j] -= temp*a[ipv*n + j];
			}
		}
	}

	/* ---- 列の入れ替え(逆行列) -------------------------- */
	for (j = m - 1; j >= 0; j--)
	{
		if (j != row[j])
		{
			for (i = 0; i < m; i++)
			{
				temp = a[i*n + j];
				a[i*n + j] = a[i*n + row[j]];
				a[i*n + row[j]] = temp;
			}
		}
	}

	free(mattemp);
	free(row);
}


//void	matinv ( double *A, int n, int m, char *s )
//{
//	double	DET, BIGST, PIVOT, PIVI, W ;
//	int		i, j, k, L, N ;
//	int		*NANS ;
//	double	*X, *temp ;
//	char	E[SCHAR] ;
//	
//	//printf ( "%s\n", s ) ;
//
//	L = 0 ;
//	if ( m <= 0 )
//	{
//		Eprint ( "<matinv", "*** error  m <= 0 ! ***" ) ;
//		return ;
//	}
//	
//	NANS = icalloc ( m, "<matinv>" ) ;
//	X = dcalloc ( m, "<matinv>" ) ;
//	temp = dcalloc ( m * m, "<matinv>" ) ;
//	
//	matcpy ( A, temp, m * m ) ;
//	
//	for ( i = 0; i < m; i++ )
//		NANS[i] = -1;
//	
//	/* ピボットサーチ */
//	DET  = 1.0 ;
//	N    = n ;
//	
//	for ( k = 0; k < m; k++ )
//	{
//		BIGST = -1.0 ;
//		
//		for ( i = 0; i < m; i++ )
//		{
//			if ( NANS[i] < 0 && ( BIGST - fabs ( A[k*N+i] )) < 0.0 )
//			{
//				BIGST = fabs ( A[k*N+i] ) ;
//				L = i ;
//			}
//		}
//                /*--
//		printf("BIGST=%lf\n",BIGST) ;
//                --*/
//		if ( BIGST <= 0.0 )
//		{
//			if ( s != NULL )
//			{
//				sprintf ( E, "対角要素に０があります  matrix=%dx%d  i=%d  [%s]",
//					m, m, i, s ) ;
//			}
//			else
//			{
//				sprintf ( E, "対角要素に０があります  matrix=%dx%d  i=%d",
//					m, m, i ) ;
//			}
//			
//			/***********************/
//			matprint ( "%.2g  ", m, temp ) ;
//			/**************************/
//			
//			free ( NANS ) ;
//			free ( X ) ;
//			free ( temp ) ;
//			
//			Eprint ( "<matinv>", E ) ;
//			
//			preexit ( ) ;
//			exit (EXIT_MATINV);
//		}
//		
//		PIVOT = A[k*N+L] ;
//		DET *= PIVOT ;
//		NANS[L] = k;
//		
//		/* 吐き出し */
//		PIVI = -1.0 / PIVOT ;
//		
//		for ( j = 0; j < m; j++ )
//		{
//			if ( j != k )
//			{
//				W = A[j*N+L] * PIVI ;
//				
//				if ( fabs ( W ) > 0.0 )
//				{
//					for ( i = 0; i < m; i++ )
//						A[j*N+i] += A[k*N+i] * W ;
//				}
//				A[j*N+L] = -W ;
//			}
//		}
//		
//		for ( i = 0; i < m; i++ )
//			A[k*N+i] *= PIVI ;
//		
//		A[k*N+L] = - PIVI ;
//	}
//	
//	/* 答えの入れ替え */
//	
//	for ( j = 0; j < m; j++ )
//	{
//		for ( i = 0; i < m; i++ )
//			X[i] = A[j*N+i] ;
//		for ( i = 0; i < m; i++ )
//			A[j*N+NANS[i]] = X[i] ;
//	}
//	
//	if ( m > 1 )
//	{
//		for ( i = 0; i < m; i++ )
//		{
//			for ( j = 0; j < m; j++ )
//				X[j] = A[j*N+i] ;
//			for ( j = 0; j < m; j++ )
//				A[j*N+i] = X[NANS[j]] ;
//		}
//	}
//	
//	free ( X ), free ( NANS ), free ( temp ) ;
//	return ;
//}


///* -----------------------------------------------------*/
// 連立1次方程式の解法
// ガウス・ザイデル法

// [A]{B}={C}
// [A]:係数行列
// {B}:解
// {C}:定数行列

//	 m :未知数の数
//	 n :配列の定義数

//	  参考文献：C言語による科学技術計算サブルーチンライブラリ
//	  pp.104-106
//	  ----------------------------------------------------- */

void	gausei(double *A, double *C, int m, int n, double *B)
{
	int		i, j, k, s, l;
	double	def, sum, w, y, ay, *a, eps = 1.0e-6;

	l = m + 1;

	a = dcalloc(m * l, "<gausei>");

	for (i = 0; i < m * l; i++)
		a[i] = 0.0;

	for (i = 0; i < m; i++)
	{
		for (j = 0; j < m + 1; j++)
		{
			if (j < m)
				a[i*l + j] = A[i*n + j] / A[i*n + i];
			else
				a[i*l + j] = C[i] / A[i*n + i];
		}
	}

	for (i = 0; i < m; i++)
		B[i] = 0.2;

	def = -999.0;
	k = 0;

	while (def > eps)
	{
		def = 0.0;

		for (i = 0; i < m; i++)
		{
			sum = 0.0;
			s = i * l;
			w = a[s + i];

			for (j = 0; j < m; j++)
			if (i != j)
				sum += a[s + j] * B[j];

			y = (a[s + m] - sum) / w;

			ay = fabs(y - B[i]);

			def = dmax(def, ay);

			B[i] = y;
		}

		if (def <= eps)
			break;

		if (k > 100)
		{
			for (i = 0; i < m; i++)
				printf("i=%d  %lf\n", i, B[i]);

			printf("収束せず\n");

			preexit();
			exit(EXIT_MATINV);
		}

		k++;
	}

	free(a);
}

//	  /* -----------------------------------------------------
//	  連立1次方程式の解法
//	  ガウスの消去法
//	  
//	   [A]{B}={C}
//	   [A]:係数行列
//	   {B}:解
//	   {C}:定数行列
//	   
//		m :未知数の数
//		n :配列の定義数
//		
//		 参考文献：C言語による科学技術計算サブルーチンライブラリ
//		 pp.104-106
//	  ----------------------------------------------------- */

void	gauss(double *A, double *C, int m, int n, double *B)
{
	double	*wfs, tmp, pv;
	int		i, j, k, *num, *pivot;

	num = icalloc(m, "<gauss>");
	pivot = icalloc(m, "<gauss>");
	wfs = dcalloc(m * m, "<gauss>");

	for (i = 0; i < m; i++)
		B[i] = 0.0;

	for (i = 0; i < m; i++)
	{
		B[i] = C[i];

		for (j = 0; j < m; j++)
			wfs[i*m + j] = A[i*n + j];
	}

	for (i = 0; i < m; i++)
		num[i] = i + 1;

	for (k = 0; k < m; k++)
	{
		pv = 0.0;

		for (i = 0; i < m; i++)
		{
			if (num[i])
			{
				if (fabs(wfs[i*m + k]) > fabs(pv))
				{
					pv = wfs[i*m + k];
					pivot[k] = i;
				}
			}
		}

		if (pv == 0.0)
		{
			Eprint("<gauss>", "対角要素に 0 があります");

			preexit();
			exit(EXIT_MATINV);
		}

		for (j = k; j < m; j++)
			wfs[pivot[k] * m + j] /= pv;

		B[pivot[k]] /= pv;

		num[pivot[k]] = 0;

		for (i = 0; i < m; i++)
		{
			if (num[i])
			{
				tmp = wfs[i*m + k];

				for (j = k + 1; j < m; j++)
					wfs[i*m + j] -= wfs[pivot[k] * m + j] * tmp;

				B[i] -= B[pivot[k]] * tmp;
			}
		}
	}

	for (i = m - 2; i >= 0; i--)
	for (j = i + 1; j < m; j++)
		B[pivot[i]] -= B[pivot[i] * m + j];

	free(num), free(pivot), free(wfs);
}

///* ----------------------------------------------------- 
//
// 正方行列の出力
//*/

void	matprint(char *fmt, int N, double *a)
{
	double	*e;
	int		i, j;

	printf("\n");

	e = a;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++, e++)
			printf(fmt, *e);
		printf("\n");
	}
}

///* ----------------------------------------------------- 
//
// 正方行列のファイル出力
//*/

void	matfiprint(FILE *f, char *fmt, int N, double *a)
{
	double	*e;
	int		i, j;

	fprintf(f, "\n");

	for (e = a, i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			fprintf(f, fmt, *e++);
		fprintf(f, "\n");
	}
}

///* ----------------------------------------------------- 
//
// 正方行列の出力 （単精度）
//*/

void	matfprint(char *fmt, int N, double *a)
{
	int    i, j;

	printf("\n");

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			printf(fmt, *a++);
		printf("\n");
	}
}

///* -----------------------------------------------------
//
// 連立一次方程式の係数行列及び右辺の出力
// 
//*/

void	seqprint(char *fmt, int N, double *a, char *fmt2, double *c)
{
	int		i, j;

	printf("--- seqprint\n");

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			printf(fmt, *a++);

		printf("  c=");
		printf(fmt2, *c++);
		printf("\n");
	}
}

///* ---------------------------------------------- */
//
///*
//行列の掛け算  
//
// (T)=[A](V)
// N:宣言寸法　　n:使用寸法
//*/

void	matmalv(double *A, double *V, int N, int n, double *T)
{
	double	*a, *v;
	int		i, j;

	for (i = 0; i < n; i++)
	{
		*T = 0.0;

		for (j = 0, a = A + i * N, v = V; j < n; j++)
			*T += (*a++ * *v++);
		T++;
	}
}

/****************************************************************/
//		行列の０初期化
/****************************************************************/

void	matinit(double *A, int N)
{
	int	i;

	for (i = 0; i < N; i++, A++)
		*A = 0.0;
}

void imatinit(int *A, int N)
{
	int	i;

	for (i = 0; i < N; i++, A++)
		*A = 0;
}

/****************************************************************/
//		行列の数値初期化
/****************************************************************/

void	matinitx(double *A, int N, double x)
{
	int		i;

	for (i = 0; i < N; i++, A++)
		*A = x;
}

/****************************************************************/
//		行列のコピー
/****************************************************************/

void	matcpy(double *A, double *B, int N)
{
	int		i;

	for (i = 0; i < N; i++, A++, B++)
		*B = *A;
}
