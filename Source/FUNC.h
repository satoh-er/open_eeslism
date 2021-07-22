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

/*

                              関数の定義                   
                                      FILE=FUNC.h
                                      Create Date 1999.12.8
				     
*/

//#include "exs.h"
/*-----障害物に当たった点の合計をカウントする---------*/
void KAUNT(int mpn, double ls, double ms, double ns,
           double *suma, double *sumg, double *sumwall, double s,
           P_MENN *mp, XYZ *p, XYZ O, XYZ E, double wa, double wb,
           XYZ G,int gpn,int nday,int *gcnt,int startday, int wlflg) ;

/*-------------乱数の発生-----------------------*/   
void RAND(double *a,double *v);   

/*---------------影の計算-----------------*/ 

void SHADOW(int g, double DE, int opn, int lpn, double ls, double ms, double ns,
            bekt *s, bekt *t, P_MENN *op, P_MENN *OP, P_MENN *LP, 
            double *wap, double *wip, int nday) ;
void SHADOWlp(int g, double DE, int lpn, int mpn, double ls, double ms, double ns,
            bekt *s, bekt *t, P_MENN *op, P_MENN *LP, P_MENN *MP) ;

/*----------形態係数を求める-------------*/
void MONTE_CARLO(int u,int w,int NUM,P_MENN *OP,
                      P_MENN *LP,XYZ **gp,int gpn,int nday,int startday); 
void GR_MONTE_CARLO(P_MENN *mp,int mpn,P_MENN *lp,int lpn,int monten,int nday) ;
void FFACTOR_LP(int lpn,int mpn,int NUM,P_MENN *LP,P_MENN *MP) ;

/*----法線ベクトルを求める----------*/    
void HOUSEN(int w,P_MENN *LP);  
void HOUSEN2(XYZ *p0, XYZ *p1, XYZ *p2, XYZ *e);

/*-----直線と平面の交点を求める-------*/ 
void KOUTEN(double Qx,double Qy,double Qz,
            double ls,double ms,double ns,
            double *Px,double *Py,double *Pz,XYZ lp,XYZ E); 

/*-----点と面の交点が平面の内か外かを判別する-----*/
void INOROUT(double Px,double Py,double Pz,XYZ P0,XYZ P1,XYZ P2,double *S,double *T) ;

 /*------構造体の初期化--------*/  
void MATINIT(P_MENN *q,int n);
void MATINIT_sum(int opn,P_MENN *op) ;   
void MATINIT_sdstr(int mpn,int mtb,SHADSTR *Sdstr) ;


void ZAHYOU(XYZ Op,XYZ G,XYZ *op,double wa,double wb);
void R_ZAHYOU(XYZ Op,XYZ G,XYZ *op,double wa,double wb) ;

/*------裏側の面を見つける--------*/  
void URA(int u, int w, P_MENN *LP, bekt *t, P_MENN *OP); 
void URA_M(double ls,double ms,double ns,double *s,double wb);

/*-----壁面の中心点の座標を求める--------*/
void GDATA(P_MENN OP,XYZ *G);

/*-----入力データの計算用構造体への変換-----------*/     
void OP_COORDNT(int *opn,int bdpn,BBDP *BDP,P_MENN *op,int polyn,POLYGN *poly);
void LP_COORDNT(int *lpn,int bdpn,int obsn,int treen,int polyn, POLYGN *poly,
                                            TREE *tree,OBS *obs,BBDP *BDP,P_MENN *lp);  

/*------構造体から別の構造体へ代入------*/
void DAINYUU_MP(P_MENN *mp,P_MENN *op,int opn, int mpn);
void DAINYUU_GP(XYZ *p,XYZ O,XYZ E,double ls,double ms,double ns);
void DAINYUU_SMO(int opn, int mpn, P_MENN *op, P_MENN *mp) ;
void DAINYUU_SMO2(int opn, int mpn, P_MENN *op, P_MENN *mp, SHADSTR *Sdstr, int dcnt, int tm) ;  // 110413 higuchi add

/*-----小数点の切捨て----------*/    
void CAT(double *a,double *b,double *c);       

/*-----ベクトルの向きを判定する----------*/  
void PRA(double *U,double ls,double ms,double ns,double x,double y,double z);

/*------ベクトルの向き---------*/
void YOGEN(double Qx,double Qy,double Qz,double Px,double Py,double Pz,double *S,XYZ e);

/*-------DATAIN.c--------------*/
void HISASHI(FILE *fi,sunblk *sb) ;
void BARUKO(FILE *fi,sunblk *sb) ;
void SODEK(FILE *fi,sunblk *sb) ;
void SCREEN(FILE *fi,sunblk *sb) ;
void rmpdata(FILE *fi,RRMP *rp,MADO *wp) ;
void rectdata(FILE *fi, OBS *obs) ;
void cubdata(FILE *fi, OBS *obs) ;
void tridata(FILE *fi,OBS *obs) ;
void bdpdata(FILE *fi, int *bdpn, BBDP **bp, EXSFS *Exsf);
void obsdata(FILE *fi,int *obsn,OBS **obs) ;
void treedata(FILE *fi,int *treen,TREE **tree) ;
void polydata(FILE *fi, int *polyn, POLYGN **poly);
int		InputCount(FILE *fi, char *key);
int		SBLKCount(FILE *fi);
int		RMPCount(FILE *fi);
int		WDCount(FILE *fi);
int	OPcount(int Nbdp, BBDP *Bdp, int Npoly, POLYGN *poly);
int LPcount(int Nbdp, BBDP *Bdp, int Nobs, OBS *Obs, int Ntree, int Npoly, POLYGN *poly);

/*-------壁面に対する入射日射角度-------------------*/
void CINC(P_MENN op,double ls,double ms,double ns,double *co);

/*-------OP,LPの座標出力（デバッグ用）-------------------*/
void ZPRINT(P_MENN *lp, P_MENN *op, int lpn, int opn, char *name) ;

/*------前面地面の代表点を求める-------------------*/
void GRGPOINT(P_MENN *mp, int mpn) ;

/*--------日射量、長波長放射量の計算------*/
void OPIhor(FILE *fp, FILE *fp1, int lpn, int mpn, P_MENN *mp, P_MENN *lp, WDAT *Wd, bekt *ullp, bekt *ulmp, XYZ **gp, int nday, int dayprn, int monten) ;

/*------計算中の面から見える面と見えない面を判別する際に必要となる
                   ベクトルの値を出力する-------------------------*/
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

/*---------------------------------------*/
char *STRCUT(char *DATA, char a) ;

/*----年間通日を求める-----*/
int nennkann(int M,int D) ;

/*----LPとOPの位置をCGで確認するための入力ファイルを作成する-----------*/
void HOUSING_PLACE(int lpn, int mpn, P_MENN *lp, P_MENN *mp, char *RET) ;

/*--------------LP面毎の日射遮蔽率出力--------------*/
void lp_shad_printf(int lpn,P_MENN *lp, char *name) ; 
/*-------------多角形の入力--------------*/
void polydata(FILE *fi, int *polyn, POLYGN **poly) ;
/*-------------分割数と射出数の設定--------------*/
void dividdata(FILE *fi, int *monten, double *DE);

