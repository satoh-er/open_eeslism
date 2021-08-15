#pragma once

#include <stdio.h>
#include "MODEL.h"

int Desiccantdata(char *s, DESICA *Desica) ;
void Desiint(int NDesi, DESI *Desi, 
			   SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd) ;
void Desicfv(int NDesi, DESI *Desi) ;
void Desiene(int NDesi, DESI *Desi) ;
int	Desivptr(char **key, DESI *Desi, VPTR *vptr) ;
void Desielm(int Ndesi, DESI *Desi) ;
void Desiprint(FILE *fo, int id, int Ndesi, DESI *Desi) ;
void Desidyprt(FILE *fo, int id, int Ndesi, DESI *Desi) ;
void Desiday(int Mon, int Day, int ttmm, int Ndesi, DESI *Desi, int Nday, int SimDayend) ;
void Desidyint(int Ndesi, DESI *Desi) ;
