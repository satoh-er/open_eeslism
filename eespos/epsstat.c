/* epsstat.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "eps.h"
#include "fnlib.h"
//#include "sun.h"


/* ----------------------------------------------------------- */

/* 日積算データ集計期間の設定 */

void statmrk(TMDT *Tmdt, STATC **Statcp, int *Nstatc, int Ntime,
			 int mostart, int daystart, int moend, int dayend,
			 TLIST *Tlist, int Ndata, char *err)
{
	int  Nm, m, mon, day, n, i;
	char *c, Mo[3];
	STATC *Stc, *Statc;
	
	
	if ((Nm = moend - mostart + 1) <= 0)
		Nm += 12;

	if (( *Statcp = calloc ( sizeof ( STATC * ), Nm + 1 )) == NULL )
		Ercalloc ( Nm + 1, "<statmrk> Statc allocate" ) ;
	else
	{
		Statc = *Statcp ;
		for ( i = 0; i < Nm + 1; i++, Statc++ )
		{
			Statc->name = NULL ;
			Statc->yrend = Statc->yrstart = Statc->mostart = Statc->daystart = 0 ;
			Statc->moend = Statc->dayend = Statc->nday = 0 ;
			for ( n = 0; n < 366; n++ )
				Statc->dymrk[n] = ' ' ;
		}
	}

	mon = mostart;
	Stc = Statc = *Statcp;
	for (m = 0; m < Nm; m++, mon++, Statc++)
	{
		if (mon == 13)
			mon = 1;
		sprintf(Mo, "%2d", mon);
		Statc->name = stralloc(Mo);
		Statc->mostart = Statc->moend = mon;
		
		if (m == 0)
			day = daystart;
		else
			day = 1;
		Statc->daystart = day;
		
		if (m == Nm - 1)
			Statc->dayend = dayend;
		else
			Statc->dayend = modayend(mon);
		Statc->nday = Statc->dayend - day +1;
	}
	Statc->name = stralloc("Total");
	
	Statc->mostart = mostart;
	Statc->daystart = daystart;
	Statc->moend = moend;
	Statc->dayend = dayend;
	
	if ((Statc->nday = FNNday(moend, dayend) - FNNday(mostart, daystart) + 1) < 0)
		Statc->nday += 365;
	Statc++;
	
	*Nstatc = Statc - Stc;
	Statc = Stc;
	
	for (m = 0; m < *Nstatc; m++, Statc++)
	{
		c = Statc->dymrk;
		for (i = 0; i < Ntime; i++, c++)
			*c = 'n';
		
		for (i = 0; i < Ntime; i++, c++)
		{
			if (Statc->mostart == Tmdt[i].Mon && Statc->daystart == Tmdt[i].Day)
			{
				c = &Statc->dymrk[i];	    
				for (n = 0; n < Statc->nday; n++, c++)
					*c = 'y';
				break;
			}
		}
	}
	
	for (i = 0; i < Ndata; i++, Tlist++)
	{
		switch (Tlist->ptype)
		{
		case 'f':
			Tlist->fstat = dcalloc(*Nstatc, err);
			break;
			
		case 'd':
			Tlist->istat = icalloc(*Nstatc, err);
			break;
		case 'c':
			Tlist->cstat = scalloc(*Nstatc, err);
			break;
		} 
	}
}

/* ----------------------------------------------------------- */

/* 日積算データの集計処理 */

void statcalc(int Nstatc, STATC *Statc, int Ntime, TMDT *Tmdt, int Ndata, TLIST *Tlist)
{
	TLIST *Tml;
	int   mody, i, j, n, nd;
	
	for (j = 0; j < Ndata; j++, Tlist++)
	{
		if (Tlist->vtype != 'H' && Tlist->vtype != 'h')
		{
			if (Tlist->req == 'y')
			{
				Tml = Tlist - 1;
				switch(Tlist->stype)
				{
				case 't': case 'a': 	       
					for (n = 0; n < Nstatc; n++)
					{
						Tlist->fstat[n] = 0;
						if (Tml != NULL)
							if (Tml->vtype == 'H')
								Tml->istat[n] = 0;
							
							if (Tlist->stype == 't')
							{
								for (i = 0; i < Ntime; i++)
								{
									if(Statc[n].dymrk[i] == 'y')
									{
										Tlist->fstat[n] += Tlist->fval[i];
										if (Tml != NULL)
											if (Tml->vtype == 'H')
												Tml->istat[n] += Tml->ival[i];
									}
								}
							}
							else if (Tlist->stype == 'a')
							{
								nd = 0;
								for (i = 0; i < Ntime; i++)
								{
									if(Statc[n].dymrk[i] == 'y')
									{
										nd++;
										if (Tml != NULL)
										{			   
											Tlist->fstat[n] += Tlist->fval[i] * Tml->ival[i];
											if (Tml->vtype == 'H')
												Tml->istat[n] += Tml->ival[i];
										}
										else
											Tlist->fstat[n] += Tlist->fval[i];
										
									}
								}
								if (Tml != NULL)
								{
									if (Tml->istat[n] > 0)
										Tlist->fstat[n] /= Tml->istat[n];
								}
								else
									if (nd > 0)
										Tlist->fstat[n] /= nd;
							}					  
					}
					break;
					
				case 'n': case 'm':
					for (n = 0; n < Nstatc; n++)
					{
						if (Tml != NULL)
							Tml->istat[n] = 0;	
						
						if (Tlist->stype == 'n')
							Tlist->fstat[n] = FLT_MAX;
						else if (Tlist->stype == 'm')
							Tlist->fstat[n] = FLT_MIN;
						
						for (i = 0; i < Ntime; i++)
						{
							if(Statc[n].dymrk[i] == 'y')
							{
								mody = atoi(Tmdt[i].mon) * 100 + atoi(Tmdt[i].day);
								if (Tlist->stype == 'n' && Tlist->fval[i] < Tlist->fstat[n])
								{
									Tlist->fstat[n] = Tlist->fval[i];
									if (Tml != NULL)
										Tml->istat[n] = mody;
								}
								else if (Tlist->stype == 'm' && Tlist->fval[i] > Tlist->fstat[n])
								{
									Tlist->fstat[n] = Tlist->fval[i];
									if (Tml != NULL)
										Tml->istat[n] = mody;
								}
							}
						}				  
					}
					break;
				}
			}
		}
	}
}
/* ----------------------------------------------------------- */

/* 集計値の出力 */

void statout(FILE *fo, int Nstatc, STATC *Statc, int Ndata, TLIST *Tlist)
{
	int    i, j;
	TLIST *Tli;
	
	for (i = 0; i < Nstatc; i++, Statc++)
	{
		fprintf(fo, "%s %3d", Statc->name, Statc->nday);
		Tli = Tlist;
		for (j = 0; j < Ndata; j++, Tli++)
		{
			if (Tli->req == 'y')
			{
				if (Tli->ptype == 'f')
				{
					switch (Tli->vtype)
					{
					case 'T': case 't':
						fprintf(fo, " %4.1f", Tli->fstat[i]);
						break;
					case 'X': case 'x':
						fprintf(fo, " %6.4f", Tli->fstat[i]);
						break;
					default:
						fprintf(fo, " %3.0f", Tli->fstat[i]);
						break;
					}
				}	    
				else if (Tli->ptype == 'd')
				{
					if (Tli->vtype == 'h')
						fprintf(fo, " %04d", Tli->istat[i]);
					else
						fprintf(fo, " %2d", Tli->istat[i]);
				}
			}
		}
		fprintf(fo, "\n");
	}
}

/* ----------------------------------------------------------- */

/* 各月の末日 */

int modayend(int mo)
{
	int ndays;
	
	switch(mo)
	{
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		ndays = 31;
		break;
		
	case 4: case 6: case 9: case 11:
		ndays = 30;
		break;
		
	case 2:
	       ndays = 28;
		   break;
	}
	return(ndays);
}

/* --------------------------------------------- */
int FNNday(int Mo, int Nd)
{ return( (153*(Mo-1)+2*(Mo<3)-9*(Mo>=3))/5 + Nd);
}



