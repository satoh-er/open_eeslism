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


typedef struct estl  /* �V�~�����[�V�������ʂɊւ��钍�� */
{
	char *flid,            /* �t�@�C����ʋL�� */
        *title,           /* �W�� */
		*wdatfile,        /* �C�ۃf�[�^�t�@�C���� */
						  tid,              /* ���̓f�[�^���  h:������  d:���� M:���� �@****/
						  *unit[VTYPEMAX],
						  *timeid,          /* �����f�[�^�\��  [Y]MD[W]T  *******/
						  *wdloc;           /* �n����@�n���@�ܓx�@�o�x�Ȃ� */
	int		Ncatalloc, NRq ;
	struct catnm *catnm;
	
	int   ntimeid,         /* �����f�[�^�\������ */
		Ntime,           /* ���ڂ��Ƃ̑S�f�[�^�� */
		dtm,             /* �v�Z���ԊԊu[s] */
		Nunit,
		Nrqlist, Nvreq,
		Npreq, Npprd,
		Ndata;
	
	struct rqlist  *Rq;
	struct prqlist *Prq;
	char *vreq ;
} ESTL;

typedef struct catnm  /* �v�f�J�^���O���f�[�^ */
{
	char *name;
	int  N,             /* �@�퐔 */
        Ncdata;        /* �S�f�[�^���ڐ� = �@�퐔 x �@��f�[�^���ڐ� */
} CATNM;

typedef struct tmdt   /* �N�A���A���A�j���A�����f�[�^ */
{
	char year[3],
        mon[3], 
		day[3],
		wkday[3],
        time[10],
		*dat[5];  /* �N�A���A���A�j���A�����̃|�C���^�[ */
	
	int  Year,
        Mon,
		Day,
		Time;
} TMDT;

typedef struct tlist  /* �V�~�����[�V�������� */
{
	char *cname, 
        *name,
		*id,
		*unit,
		vtype,   /* �f�[�^���
				 t:���x  x:��Ύ��x  r:���Ύ��x
				 T:���ω��x  X:���ϐ�Ύ��x  R:���ϑ��Ύ��x
				 h:��������  H:�ώZ����
q:�M��      Q:�ώZ�M��   e:�G�l���M�[ E:�ώZ�G�l���M�[�� */
stype,   /* �f�[�^�������
t:�ώZ�l  a:���ϒl  n:�ŏ��l  m;�ő�l  */

ptype,   /* �f�[�^�^  c:�����^  d:�����^  f:�����^ */
req;
	
	double *fval, *fstat;
	int   *ival, *istat;
	char  *cval, *cstat,
		*fmt;
	
	struct tlist *pair;  
} TLIST;


typedef struct rqlist  /* �I������ */
{
	char *rname,
        *name,
        *id;
} RQLIST;

#if EPOS_PROG
typedef struct statc  /* �W�v���� */
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

typedef struct prqlist /* ��\���ځE���Ԏw�� */
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



