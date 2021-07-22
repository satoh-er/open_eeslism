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

/*  ees3lib.c  */

#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include "estr.h"
#include "fesy.h"
#include "fnlib.h"
#include "winerror.h"

/* Check heap status */
void HeapCheck(char *s)
{
	int i ;
	extern int DEBUG;

	if (DEBUG)
	{
		//printf("HeatCheck  %s\n", s) ;
		i = _heapchk();
		switch (i)
		{
		case _HEAPOK:
			//printf("<%s> OK - heap is fine\n", s );
			break;
		case _HEAPEMPTY:
			//printf("<%s> OK - heap is empty\n", s );
			break;
		case _HEAPBADBEGIN:
			printf("<%s> ERROR - bad start of heap\n", s);
			break;
		case _HEAPBADNODE:
			printf("<%s> ERROR - bad node in heap\n", s);
			break;
		}
	}
}

int isstrdigit(char *s)
{
	while (*s != '\0')
	{
		if (isdigit(*s) == 0)
		{
			if (*s != '.' && *s != '-' && *s != '+')
				return 0;
		}     
		s++;
	}
	return 1;
}

/* 文字列データの記憶域確保と代入 */

char *stralloc(char *s)
{
	char	*c;
	int		N ;

	c = NULL ;
	/***************************
	printf ("<stralloc> s=%s\n", s ) ;
	/*****************************/

	if ( s != NULL && strcmp ( s, "" ) != 0 )
	{
		N = (int)strlen(s) + 1 ;

		if ( N > 0 )
			c=(char *)calloc(N,sizeof(char)) ;
		//c = ( char * ) _alloca (( strlen(s) + 1 ) * sizeof ( char )) ;

		if ( c != NULL)
			strcpy(c, s);
		else
		{
			Eprint("xxxxx  stralloc  err=", s);

			preexit ( ) ;
			exit (EXIT_ALLOC) ;
		}
	}
	else
		c = NULL ;

	return  c;
}

/* 文字列データの記憶域確保 */

char *scalloc(unsigned int n, char *errkey)
{
	char	*c, *st ;
	char	s[SCHAR];
	int		i ;
	
	c = NULL ;

	if ( n > 0 )
		c=(char *)malloc(n * sizeof (char)) ;
	else
		return ( c ) ;

	if ( c == NULL)
	{
		sprintf(s, " -- f.scalloc   n=%d", n);
		Eprint(errkey, s);

		preexit ( ) ;
		exit (EXIT_ALLOC) ;
	}

	st = c ;
	for ( i = 0; i < ( int ) n; i++, st++ )
		*st = ' ' ;

	strcpy ( c, "" ) ;
	return  c;
}

/* 文字列データのメモリ領域の最確保 */
int	srealloc ( char *s, unsigned int n )
{
	char	*a ;
	int		c ;

	c = 0 ;
	a = NULL ;

	if ( n > 0)
		a = ( char * ) realloc ( s, sizeof ( char ) * n ) ;
	
	if ( a == NULL )
		c = 1 ;

	s = a ;

	return c ;
}

/* 文字データの記憶域確保と代入 */

char *charalloc(char c)
{
	char *p;
	char  s[SCHAR];
	
	if ((p = scalloc(1, "xxxxx  charalloc")) == NULL)
	{
		sprintf(s, "  errchar = [%c]", c);
		Eprint("charalloc", s); 

		preexit ( ) ;
		exit (EXIT_ALLOC) ;
	}
	
	*p = c;
	return  p;
}


/* intデータの記憶域確保 */

int *icalloc(unsigned int n, char *errkey)
{
	int		*i, *I ;
	unsigned int	j;
	char	s[SCHAR];
	
	i = NULL ;
	if (n > 0 && (i = (int *)malloc(n * sizeof(int))) == NULL)
	{
		sprintf(s, " -- f.icalloc   n=%d", n);
		Eprint(errkey, s);

		preexit ( ) ;
		exit (EXIT_ALLOC) ;
	}
	
	I = i ;
	for ( j = 0; j < n; j++, I++ )
		*I = 0 ;
	
	return  i;
}

/* intデータのメモリ領域の最確保 */

int	irealloc ( int **A, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	int		*Buf, *B ;

	Buf = NULL ;

	c = 1 ;

	if ( NN > 0 )
	{
		if ( N > 0 && *A != NULL)
			Buf = ( int * ) realloc ( *A, NN * sizeof ( int )) ;
		else
			Buf = ( int * ) malloc ( NN * sizeof ( int )) ;
	}

	if ( Buf == NULL )
		c = 0 ;
	else
	{
		*A = Buf ;
		B = *A + N ;

		for ( i = N; i < NN; i++, B++ )
			*B = 0 ;

		*A = &Buf[0] ;
	}

	return c ;
}

/*  double データの記憶域確保 */

double *dcalloc(unsigned int n, char *errkey)
{
	double *f, *B ;
	unsigned int    i ;
	char  s[SCHAR];

	f = NULL ;

	if ( n > 0 )
		f = (double *)malloc(n * sizeof(double)) ;
	
	if ( n > 0 && f == NULL)
	{
		sprintf(s, " -- f.dcalloc   n=%d", n);
		Eprint(errkey, s);

		preexit ( ) ;
		exit (EXIT_ALLOC) ;
	}
	
	B = f ;

	for ( i = 0; i < n; i++, B++ )
		*B = 0.0 ;
	
	return  f ;
}

// 指定した数値で初期化
double	*dcallocinit(unsigned int n, double init, char *errkey)
{
	double	*f, *B ;
	unsigned int    i ;

	f = dcalloc(n, errkey) ;

	B = f ;
	for(i = 0; i < n; i++, B++)
		*B = init ;

	return f ;
}

/* doubleデータのメモリ領域の最確保 */

int	drealloc ( double **A, unsigned int N, unsigned int NN )
{
	int		c ;
	unsigned int	i ;
	double	*Buf, *B ;

	Buf = NULL ;

	c = 1 ;

	if ( NN > 0 )
	{
		if ( N > 0 && *A != NULL )
			Buf = ( double * ) realloc ( *A, NN * sizeof ( double )) ;
		else
			Buf = ( double * ) malloc ( NN * sizeof ( double )) ;
	}

	if ( Buf == NULL )
		c = 0 ;
	else
	{
		/****************************************
		for ( i = 0; i < NN; i++ )
			printf ( "<drealloc> Buf[%d]=%lf\n", i, Buf[i] ) ;
			*********************************/

		if ( N > 0 )
		{
			B = Buf + N ;
			
			for ( i = N; i < NN; i++, B++ )
				*B = 0.0 ;
		}

		//for ( i = 0; i < NN; i++ )
		//	printf ( "<drealloc> Buf[%d]=%lf\n", i, Buf[i] ) ;

		*A = Buf ;
	}

	return c ;
}

/*  入力データエラーの出力  */

void Errprint(int err, char *key, char *s)
{
	extern FILE *ferr;
	
	if (err != 0)
	{
		printf(ERRFMTA, key, s);
		if ( ferr )
			fprintf(ferr, ERRFMTA, key, s);
	}
}


void Eprint(char *key, char *s)
{
	extern FILE *ferr;
	
	printf(ERRFMTA, key, s);
	if ( ferr )
		fprintf(ferr, ERRFMTA, key, s);
}

/*  データの記憶域確保時のエラー出力 */

void Ercalloc(unsigned int n, char *errkey)
{
    char  s[SCHAR];
	
    sprintf(s, " -- calloc   n=%d", n);
    Eprint(errkey, s);
}


void	preexit ( void )
{
	extern int NSTOP;
	printf ( "Press Hit Return Key .......\n" ) ;
	if (NSTOP == 0)
		getchar();
	else
		exit(1);
}

double	lineardiv ( double A, double B, double dt )
{
	return ( A + ( B - A ) * dt ) ;
}

// ttmmから1時間間隔の時刻へ変換する関数
// 0:01～1:00を1時（ここでは配列番号として0～23にしている）とする
int		ConvertHour( int ttmm )
{
	int	tt ;

	tt =  (int)floor(  ( (double)ttmm - 1. ) / 100. ) ;
	return ( tt ) ;
}
