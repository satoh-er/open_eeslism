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

/*      schdlr.c     */

#include "common.h"
#include "fesy.h"
#include "fnesy.h"


/*  １日スケジュ－ルから設定値の選択   */

double	schval (int nday, int ttmm, SCH *Sch, DSCH *Dsch)
{
	int		sc, k, N, *stime, *etime ;
	DSCH	*Ds ;
	double	*val ;
	
	//printf ( "aaaaaaaa\n" ) ;
	sc = Sch->day[nday];
	
	if ( sc < 0 )
		return ( FNOP ) ;

	Ds = Dsch + sc ;
	N = Ds->N;
	//printf ( "bbbbbbbbbbb\n" ) ;
	stime = Ds->stime ;
	etime = Ds->etime ;
	val = Ds->val ;
	for ( k = 0; k < N; k++, stime++, etime++, val++ )
	{
		//printf ("k=%d  MAX=%d  ", k, N ) ;
		//printf ("stime=%d etime=%d  ", *stime, *etime ) ;
		//printf ("val=%g\n", *val ) ;
		if ( *stime <= ttmm && ttmm <= *etime )
			return ( *val );
	}
	return ( FNOP );
}
/* ----------------------------------------------------- */

/*  １日スケジュ－ルから設定モ－ドの選択   */

char	scwmode (int nday, int ttmm, SCH *Scw, DSCW *Dscw)
{
	int		sw, k, N, *stime, *etime ;
	DSCW	*Ds ;
	char	*mode ;
	
	sw = Scw->day[nday];
	Ds = Dscw + sw ;
	N = Ds->N;
	
	stime = Ds->stime ;
	etime = Ds->etime ;
	mode = Ds->mode ;
	for ( k = 0; k < N; k++, stime++, etime++, mode++ )
	{
		//printf("k=%d stime=%d etime=%d mode=%c\n", k, *stime, *etime, *mode);
		if ( *stime <= ttmm && ttmm <= *etime )
			return ( *mode ) ;
	}
	return ('x');
}
/* ----------------------------------------------------- */

/*  スケジュ－ルモ－ドから設定番号の検索   */

int iswmode (char c, int N, char *mode)
{
	int i;
	
	/********************* 
	printf("***** iswmode  c=%c  N=%d  \n", c, N);
	for (i=0; i<N; i++)
	printf("mode[%d]=%c\n", i, mode[i]);
	**************************************/
	
	if ( N == 1 )
		return(0);
	else
	{
		for ( i = 0; i < N; i++, mode++ )
		{
			if ( c == *mode )
				return ( i ) ; 
		}
		return ( -1 ) ;
	}
}    
/* ----------------------------------------------------- */

