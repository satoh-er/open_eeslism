/*  ees3lib.c  */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
//#include "estr.h"
#include "fesy.h"
#include "fnlib.h"

int isstrdigit(char *s)
{
	while (*s != '\0')
	{
		if (isdigit(*s) == 0)
		{
			if (*s != '.' && *s != '-' && *s != '+')
				return 0;
		}     
		s++;
	}
	return 1;
}

/* 文字列データの記憶域確保と代入 */

char *stralloc(char *s)
{
	char *c;
	
	if ((c=(char *)malloc(strlen(s)+1)) != NULL)
		strcpy(c, s);
	else 
		Eprint("xxxxx  stralloc  err=", s);
	return  c;
}

/* 文字列データの記憶域確保 */

char *scalloc(unsigned int n, char *errkey)
{
	char *c;
	char  s[ERSTR];
	
	if ((c=(char *)malloc(n)) == NULL)
	{
		sprintf(s, " -- f.scalloc   n=%d", n);
		Eprint(errkey, s);
	}
	return  c;
}

/* 文字データの記憶域確保と代入 */

char *charalloc(char c)
{
	char *p;
	char  s[ERSTR];
	
	if ((p = scalloc(1, "xxxxx  charalloc")) == NULL)
	{
		sprintf(s, "  errchar = [%c]", c);
		Eprint("charalloc", s); 
	}
	
	*p = c;
	return  p;
}


/* intデータの記憶域確保 */

int *icalloc(unsigned int n, char *errkey)
{
	unsigned int *i, j;
	char  s[ERSTR];
	
	if ((i = (int *)calloc(n, sizeof(int))) == NULL)
	{
		sprintf(s, " -- f.icalloc   n=%d", n);
		Eprint(errkey, s);
	}
	
	for ( j = 0; j < n; j++, i++ )
		*i = 0 ;
	
	return  (int *) i;
}
/*  float データの記憶域確保 */

float *fcalloc(unsigned int n, char *errkey)
{
	float *f ;
	unsigned int   i ;
	char  s[ERSTR];
	
	if ((f = (float *)calloc(n, sizeof(float))) == NULL)
	{
		sprintf(s, " -- f.fcalloc   n=%d", n);
		Eprint(errkey, s);
	}
	
	for ( i = 0; i < n; i++ )
		*( f + i ) = 0.0 ;
	
	return   f;
}

/*  double データの記憶域確保 */

double *dcalloc(unsigned int n, char *errkey)
{
	double *f ;
	unsigned int    i ;
	char  s[ERSTR];
	
	if ((f = (double *)calloc(n, sizeof(double))) == NULL)
	{
		sprintf(s, " -- f.dcalloc   n=%d", n);
		Eprint(errkey, s);
	}
	
	for ( i = 0; i < n; i++ )
		*( f + i ) = 0.0 ;
	
	return  f;
}


/*  入力データエラーの出力  */

void Errprint(int err, char *key, char *s)
{
	extern FILE *ferr;
	
	if (err != 0)
	{
		printf(ERRFMTA, key, s);
		if (ferr != NULL)
			fprintf(ferr, ERRFMTA, key, s);
	}
}


void Eprint(char *key, char *s)
{
	extern FILE *ferr;
	
	printf(ERRFMTA, key, s);
	if (ferr != NULL)
		fprintf(ferr, ERRFMTA, key, s);
}

/*  データの記憶域確保時のエラー出力 */

void Ercalloc(unsigned int n, char *errkey)
{
    char  s[ERSTR];
	
    sprintf(s, " -- f.dcalloc   n=%d", n);
    Eprint(errkey, s);
}
