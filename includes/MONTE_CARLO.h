#pragma once

#include "MODEL.h"

void MONTE_CARLO(int u,int w,int NUM,P_MENN *OP,
                      P_MENN *LP,XYZ **gp,int gpn,int nday,int startday); 
void GR_MONTE_CARLO(P_MENN *mp,int mpn,P_MENN *lp,int lpn,int monten,int nday) ;
void FFACTOR_LP(int lpn,int mpn,int NUM,P_MENN *LP,P_MENN *MP) ;
