/* esolb.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eps.h"
#include "fnlib.h"
#include "estr.h"

/* 入力ファイル指定（シミュレーション結果ファイル） */

FILE *esfidat(int Narg, char *arg[], char **finame)
{
	int i;
	char s[NCHAR];
	FILE *fi;
	
	fi = NULL;
	s[0] = '\0' ;
    
	for (i = 1; i < Narg; i++) 
	{
		if (strcmp(arg[i], "-fi") == 0)
		{
			if ((fi = fopen(arg[++i], "r")) == NULL)
				printf(" file error fi -- %s\n", arg[i]), exit(0);
			*finame = stralloc ( arg[i]);
			break;	 
		}
	}
	if (fi == NULL)
	{
		printf("-- input file ?\n");
		scanf("%s", s);
		
		if ((fi = fopen(s, "r")) == NULL)
			printf(" file error fi -- %s\n", s), exit(0);
		*finame = stralloc ( s);
	}
	if ( *finame == NULL && s[0] != '\0' )
		*finame = stralloc(s);

	return fi;
}
/* ------------------------------------------------------------ */

/* 出力ファイル指定 */

FILE *esfout(int Narg, char *arg[], char *idfo, char *finame)
{
	int i;
	char *st, *fname ;
	FILE *fo;
	
	fo = NULL;
	fname = scalloc ( strlen ( finame) + 5, "<esfout> fname allocate" ) ;
	strcpy ( fname, finame ) ;
	
	for (i = 1; i < Narg; i++) 
	{
		if (strcmp(arg[i], "-fo") == 0) 
		{
			if ((fo = fopen(arg[++i], "w")) == NULL)
				printf(" file error fo -- %s\n", arg[i]), exit(0);
			break;
		}
	}	 
	if (fo == NULL)
	{
		if ((st = strrchr(fname, '.')) != NULL)
		{
			*st = '\0';	 
			strcat ( fname, idfo);
		}
		if ((fo = fopen( fname, "w")) == NULL)
			printf(" file error fo -- %s\n", fname), exit(0);
	}

	free ( fname ) ;
	return fo;
}
/* ----------------------------------------------------------- */

/* 処理内容指定ファイル */

FILE *esfii(int Narg, char *arg[])
{
	int i;
	FILE *fii;
	
	fii = NULL;
    
	for (i = 1; i < Narg; i++) 
	{
		if (strcmp(arg[i], "-fii") == 0) 
		{
			if ((fii = fopen(arg[++i], "r")) == NULL)
				printf(" file error fii -- %s\n", arg[i]), exit(0);
			break;
		}
	}
	return fii;
}
