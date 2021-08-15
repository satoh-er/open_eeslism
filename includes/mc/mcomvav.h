#pragma once

int OMVAVdata (char *ss, OMVAVCA *OMvavca) ;
double	OMflowcalc(OMVAV *OMvav, WDAT *Wd) ;
int colkey(char *s, char **key) ;
void	OMvavControl(OMVAV *OMvav, COMPNT *Compnt, int NCompnt) ;
int		strCompcount(char *st, char key) ;
