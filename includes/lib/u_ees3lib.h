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

void	HeapCheck(char *s) ;
int		isstrdigit(char *s);
char	*stralloc(char *s);
char	*scalloc(unsigned int n, char *errkey);
int		srealloc ( char *s, unsigned int n ) ;
char	*charalloc(char c);
int		*icalloc(unsigned int n, char *errkey);
int		irealloc ( int **A, unsigned int N, unsigned int NN ) ;
double	*dcalloc(unsigned int n, char *errkey);
double	*dcallocinit(unsigned int n, double init, char *errkey);
int		drealloc ( double **A, unsigned int N, unsigned int NN ) ;
void	Errprint(int err, char *key, char *s);
void	Eprint(char *key, char *s);
void	Ercalloc(unsigned int n, char *errkey);
void	preexit ( void ) ;
double	lineardiv ( double A, double B, double dt ) ;
int		ConvertHour( int ttmm ) ;
