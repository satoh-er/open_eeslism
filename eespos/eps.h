/* eps.h */

#define EPOS_PROG 1

#define VTYPEMAX 21
//#define CATNMMAX 21

#if EPOS_PROG

#define IDFO     ".efv"
//#define STOTALMAX 1000
//#define MAXPARM   1000
//#define PNAMEMAX  1000
//#define NREQMAX   1000
#define NPREQMAX  1000
//#define NCHAR     256

typedef struct rmprt
{
	char *name;
	int  Ndat, Nrm, Nld, Nasp, Nrp;
} RMPRT;

#endif


typedef struct estl  /* シミュレーション結果に関する注釈 */
{
	char *flid,            /* ファイル種別記号 */
        *title,           /* 標題 */
		*wdatfile,        /* 気象データファイル名 */
						  tid,              /* 入力データ種別  h:時刻別  d:日別 M:月別 　****/
						  *unit[VTYPEMAX],
						  *timeid,          /* 時刻データ表示  [Y]MD[W]T  *******/
						  *wdloc;           /* 地域情報　地名　緯度　経度など */
	int		Ncatalloc, NRq ;
	struct catnm *catnm;
	
	int   ntimeid,         /* 時刻データ表示字数 */
		Ntime,           /* 項目ごとの全データ数 */
		dtm,             /* 計算時間間隔[s] */
		Nunit,
		Nrqlist, Nvreq,
		Npreq, Npprd,
		Ndata;
	
	struct rqlist  *Rq;
	struct prqlist *Prq;
	char *vreq ;
} ESTL;

typedef struct catnm  /* 要素カタログ名データ */
{
	char *name;
	int  N,             /* 機器数 */
        Ncdata;        /* 全データ項目数 = 機器数 x 機器データ項目数 */
} CATNM;

typedef struct tmdt   /* 年、月、日、曜日、時刻データ */
{
	char year[3],
        mon[3], 
		day[3],
		wkday[3],
        time[10],
		*dat[5];  /* 年、月、日、曜日、時刻のポインター */
	
	int  Year,
        Mon,
		Day,
		Time;
} TMDT;

typedef struct tlist  /* シミュレーション結果 */
{
	char *cname, 
        *name,
		*id,
		*unit,
		vtype,   /* データ種別
				 t:温度  x:絶対湿度  r:相対湿度
				 T:平均温度  X:平均絶対湿度  R:平均相対湿度
				 h:発生時刻  H:積算時間
q:熱量      Q:積算熱量   e:エネルギー E:積算エネルギー量 */
stype,   /* データ処理種別
t:積算値  a:平均値  n:最小値  m;最大値  */

ptype,   /* データ型  c:文字型  d:整数型  f:実数型 */
req;
	
	double *fval, *fstat;
	int   *ival, *istat;
	char  *cval, *cstat,
		*fmt;
	
	struct tlist *pair;  
} TLIST;


typedef struct rqlist  /* 選択項目 */
{
	char *rname,
        *name,
        *id;
} RQLIST;

#if EPOS_PROG
typedef struct statc  /* 集計期間 */
{
	char *name;
	int  yrstart,
        mostart,
		daystart, 
        yrend,
		moend,
		dayend,
		nday;
	char dymrk[366];
} STATC;

typedef struct prqlist /* 作表項目・期間指定 */
{
	char mark,
		//        *prname[PNAMEMAX],
		**prname,
		//		*prid[PNAMEMAX];
		**prid ;
	int  Npname, Npid;
} PRQLIST;

#endif



/*****************************/
FILE *esfii(int Narg, char *arg[]);
FILE *esfout(int Narg, char *arg[], char *idfo, char *finame);
//FILE *esfidat(int Narg, char *arg[], char *finame);

void rqlist(int Narg, char *arg[], FILE *fii, ESTL *Estl);

void tmdout(int init, FILE *fo, ESTL *Estl, TMDT *Tmdt);
void esoout(int init, FILE *fi, FILE *fo, int Ndata, TLIST *Tlist);

void statcalc(int Nstatc, STATC *Statc, int Ntime, TMDT *Tmdt, int Ndata, TLIST *Tlist);
void statout(FILE *fo, int Nstatc, STATC *Statc, int Ndata, TLIST *Tlist);
void statmrk(TMDT *Tmdt, STATC **Statc, int *Nstatc, int Ntime,
			 int mostart, int daystart, int moend, int dayend,
			 TLIST *Tlist, int Ndata, char *err);
int modayend(int mo);
int FNNday(int Mo, int Nd);

void prtreqdc(int Narg, char *arg[], FILE *fii, ESTL *Estl, STATC *Pprd);
void prtreqlist(ESTL *Estl, int Ndata, TLIST *Tlist, TLIST *plist[]);
void daystrend(char *s, int *mostart, int *daystart, int *moend, int *dayend);
void prttbl(FILE *fo, ESTL *Estl, STATC *Pprd, TMDT *Tmdt, TLIST *Plist[]);
int prtmdtstart(ESTL *Estl, TMDT *Tmdt, STATC *Pprd);
int prtmdtend(ESTL *Estl, TMDT *Tmdt, STATC *Pprd);
void tblout(char mrk, FILE *fo, TLIST *Plist, int t);
/**********************/



