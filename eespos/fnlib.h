/* fnlib.h */


/*  ees3lib.c  */
int isstrdigit(char *s);
char *stralloc(char *s);
char *scalloc(unsigned int n, char *errkey);
char *charalloc(char c);
int *icalloc(unsigned int n, char *errkey);
float *fcalloc(unsigned int n, char *errkey);
double *dcalloc(unsigned int n, char *errkey);
void Errprint(int err, char *key, char *s);
void Eprint(char *key, char *s);
void Ercalloc(unsigned int n, char *errkey);

/*  inv.c  */
void matinv(double *A, int n, int m);
void gausei ( double *A, double *C, int m, int n, double *B ) ;
void gauss ( double *A, double *C, int m, int n, double *B ) ;
void matprint(char *fmt, int N, double *a);
void matfprint(char *fmt, int N, double *a);
void seqprint(char *fmt, int N, double *a, char *fmt2, double *c);
void matmalv(double *A, double *V, int N,int n, double *T);

/*  minmax.c  */
double fmin(double a, double b);
double fmax(double a, double b);

/*  mlib.c  */
double spcheat(char fluid);

/*  wreadlib.c   */
void monthday(int *Mon, int *Day, int mo, int dayo);

FILE *esfidat(int Narg, char *arg[], char **finame) ;
void fofmt(ESTL *Estl, TLIST *Tlist) ;
void esondat(FILE *fi, ESTL *Estl) ;
void esoint(FILE *fi, char *err, int Ntime, 
			ESTL *Estl, TLIST *Tlist) ;
void tmdout(int init, FILE *fo, ESTL *Estl, TMDT *Tmdt) ;
int tmdata(FILE *fi, ESTL *Estl, TMDT *Tmdt) ;
void esdatgt(FILE *fi, int i, int Ndata, TLIST *Tlist) ;
int	Rqcount ( FILE *fi ) ;
int	catcount ( FILE *fi ) ;
void	Estlinit ( ESTL *Estl ) ;
