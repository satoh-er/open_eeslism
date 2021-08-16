#pragma once

void DAINYUU_MP(P_MENN *mp,P_MENN *op,int opn, int mpn);
void DAINYUU_GP(XYZ *p,XYZ O,XYZ E,double ls,double ms,double ns);
void DAINYUU_SMO(int opn, int mpn, P_MENN *op, P_MENN *mp) ;
void DAINYUU_SMO2(int opn, int mpn, P_MENN *op, P_MENN *mp, SHADSTR *Sdstr, int dcnt, int tm) ;  // 110413 higuchi add
