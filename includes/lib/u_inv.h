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

#pragma once

/*--
void    matinv ( double *A, int n, int m) ;
--*/
void	matinv ( double *A, int n, int m, char *s );
void	gausei ( double *A, double *C, int m, int n, double *B ) ;
void	gauss ( double *A, double *C, int m, int n, double *B ) ;
void	matprint(char *fmt, int N, double *a);
void	matfprint(char *fmt, int N, double *a);
void	seqprint(char *fmt, int N, double *a, char *fmt2, double *c);
void	matmalv(double *A, double *V, int N,int n, double *T);
void	matinit ( double *A, int N ) ;
void imatinit(int *A, int N);
void	matinitx ( double *A, int N, double x ) ;
void	matfiprint ( FILE *f, char *fmt, int N, double *a ) ;
void	matcpy ( double *A, double *B, int N ) ;
