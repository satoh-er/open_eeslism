#pragma once

#include "MODEL.h"

int		Thexdata ( char *s, THEXCA *Thexca ) ;
void	Thexint ( int Nthex, THEX *Thex ) ;
void	Thexcfv ( int Nthex, THEX *Thex ) ;
void	Thexelm(int NThex, THEX *Thex) ;
void	Thexene ( int Nthex, THEX *Thex ) ;
void	Thexprint(FILE *fo, int id, int Nthex, THEX *Thex ) ;
void	Thexdyint(int Nthex, THEX *Thex) ;
void	Thexmonint(int Nthex, THEX *Thex) ;
void	Thexday(int Mon, int Day, int ttmm, int Nthex, THEX *Thex, int Nday, int SimDayend) ;
void	Thexdyprt(FILE *fo, int id, int Nthex, THEX *Thex) ;
void	Thexmonprt(FILE *fo, int id, int Nthex, THEX *Thex) ;
