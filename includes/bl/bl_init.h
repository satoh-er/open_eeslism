#pragma once

void Walldata(FILE *fi, FILE *fbmlist, char *dsn,
	WALL **Wall, int *Nwall, DFWL *dfwl, PCM *pcm, int Npcm);
void Windowdata (FILE *fi, char *dsn, WINDOW **Window, int *Nwindow);
void Snbkdata (FILE *fi, char *dsn, SNBK **Snbk);
int	BMLSTrealloc ( BMLST **W, unsigned int N, unsigned int NN ) ;
int	WELMrealloc ( WELM **W, unsigned int N, unsigned int NN ) ;
int	WINDOWrealloc ( WINDOW **W, unsigned int N, unsigned int NN ) ;
int	SNBKrealloc ( SNBK **W, unsigned int N, unsigned int NN ) ;
int	WALLrealloc ( WALL **W, unsigned int N, unsigned int NN ) ;
int		wbmlistcount ( FILE *fi ) ;
int		Wallcount ( FILE *fi ) ;
