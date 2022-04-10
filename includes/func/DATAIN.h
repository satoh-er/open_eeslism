#pragma once
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

/*-------------多角形の入力--------------*/
void polydata(FILE *fi, int *polyn, POLYGN **poly);

int		InputCount(FILE *fi, char *key);
int		SBLKCount(FILE *fi);
int		RMPCount(FILE *fi);
int		WDCount(FILE *fi);
int	OPcount(int Nbdp, BBDP *Bdp, int Npoly, POLYGN *poly);
int LPcount(int Nbdp, BBDP *Bdp, int Nobs, OBS *Obs, int Ntree, int Npoly, POLYGN *poly);


/*-------------分割数と射出数の設定--------------*/
void dividdata(FILE *fi, int *monten, double *DE);