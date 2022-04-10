#pragma once

void errbekt_printf(int n, int m, bekt *a, char *name) ;

/*--形態係数の出力--*/
void ffactor_printf(FILE *fp4, int mpn, int lpn, P_MENN *mp, P_MENN *lp,
                    int Mon, int Day);


/*-----------法線ベクトルの出力-----------*/
void e_printf(int n, P_MENN *p, char *name) ;


/*---------------影面積の出力--------------------*/
void shadow_printf(FILE *fp,int M, int D, double mt, int mpn, P_MENN *mp) ;

/*------MP面の情報出力----------------------*/
void mp_printf(int n, P_MENN *mp, char *name) ;

/*------MP面の前面地面のポイント座標出力---------------------*/
void gp_printf(XYZ **gp,P_MENN *mp, int mpn, int lpn, char *name) ;

/*------LP面の情報出力---------------------------*/
void lp_printf(int n, P_MENN *lp, char *name) ;

/*--------------LP面毎の日射遮蔽率出力--------------*/
void lp_shad_printf(int lpn,P_MENN *lp, char *name) ; 