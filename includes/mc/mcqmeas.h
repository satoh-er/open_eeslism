#pragma once

#include <stdio.h>
#include "MODEL.h"

void	Qmeasene (int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeaselm ( int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasprint(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasdyint(int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasmonint(int Nqmeas, QMEAS *Qmeas ) ;
void	Qmeasday(int Mon, int Day, int ttmm, int Nqmeas, QMEAS *Qmeas, int Nday, int SimDayend) ;
void	Qmeasdyprt(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas) ;
void	Qmeasmonprt(FILE *fo, int id, int Nqmeas, QMEAS *Qmeas) ;
