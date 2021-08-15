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

/*  mcstank.c */

/*  95/11/17 rev  */

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "common.h"
#include "fesy.h"
#include "fnmcs.h"
#include "fnesy.h"
#include "fnfio.h" 
#include "fnlib.h"
#include "waterandair.h"


/*　蓄熱槽仕様入力　　　　*/

int Stankdata(FILE *f, char *s, STANKCA *Stankca)
{
	int   id = 0;
	char  *st;
	
	Stankca->gxr = 0.0;
	
	if ((st = strchr(s, '=')) != 0)
	{
		*st='\0'; 
		if (strcmp(s, "Vol") == 0)
			Stankca->Vol = atof(st+1);
		else if (strcmp(s, "KAside") == 0)
			Stankca->KAside = atof(st+1);
		else if (strcmp(s, "KAtop") == 0)
			Stankca->KAtop = atof(st+1);
		else if (strcmp(s, "KAbtm") == 0)
			Stankca->KAbtm = atof(st+1);
		
		else if (strcmp(s, "gxr") == 0)
			Stankca->gxr = atof(st+1);
		
		else 
			id = 1;
	}
	else if (strcmp(s, "-S") == 0)
	{
		strcat(s, "  ");
		st = s + strlen(s);	   
		fscanf(f, "%[^*]*", st); 
		strcat(s, " *"); 	   
		Stankca->tparm = stralloc(s);
	}   
	else
	{
		Stankca->name = stralloc(s);
		Stankca->type = 'C';
		Stankca->tparm = NULL ;
		Stankca->Vol = Stankca->KAside = Stankca->KAtop = Stankca->KAbtm = -999.0 ;
		Stankca->gxr = 0.0 ;
	}
	return(id);
}

/* ------------------------------------------------------- */

/* 蓄熱槽記憶域確保 */

void Stankmemloc(char *errkey, STANK *Stank)
{
	int i, j, np, Ndiv, Nin;
	char *st, *stt, ss[SCHAR], *parm[SCHAR];
	char *stp ;
	
	np = 0;
	/**********
	st = Compnt->tparm;**********/
	st = Stank->cat->tparm;
	
	/******* printf("xxxx Stankmemloc  tparm=%s\n", st); *****/
	
	while (sscanf(st, "%s", ss), *ss != '*')
	{
		parm[np] = st;
		np++ ;
		st += strlen(ss);
		while (isspace(*(++st)));
	}
	Stank->pthcon = scalloc(np, errkey);	     
	if (np > 0 && (Stank->batchcon = (char **)malloc(np * sizeof(char *))) == NULL)
		Ercalloc(np, errkey);
	Stank->ihex = scalloc(np, errkey);
	Stank->Jin = icalloc(np, errkey);
	Stank->Jout = icalloc(np, errkey);
	Stank->ihxeff = dcalloc(np, errkey);
	Stank->KA = dcalloc(np, errkey);
	Stank->KAinput = scalloc(np, errkey) ;
	
	i = 0;
	
	// np：タンクへの流入数
	for (j = 0; j < np; j++)
	{
		sscanf(parm[j], "%s", ss);
		
		if (strncmp(ss, "N=", 2) == 0)
			Stank->Ndiv = atoi(ss + 2);
        else if ((st = strchr(ss, ':')) != 0)
        {
			Stank->pthcon[i] = *ss;
			Stank->Jin[i] = atoi(st + 1) - 1;
			
			if ((stt = strchr(st + 1, '-')) != NULL)
			{
				*stt = '\0';
				Stank->ihex[i] = 'n';
				Stank->ihxeff[i] = 1.0;
				Stank->Jout[i] = atoi(stt + 1) - 1;
			}
			else	if ((stt = strchr(st + 1, '_')) != NULL)
			{
				*stt = '\0';
				Stank->ihex[i] = 'y';

				if ( *(stt + 1) == 'e' )			// 温度効率が入力されている場合
					Stank->ihxeff[i] = atof(stt + 5);
				else if ( *(stt + 1) == 'K' )		// 内蔵熱交のKAが入力されている場合
				{
					Stank->KAinput[i] = 'Y' ;
					Stank->KA[i] = atof(stt + 4 ) ;
				}
				else if ( *(stt + 1) == 'd')
				{
					Stank->KAinput[i] = 'C' ;			// 内蔵熱交換器の内径と長さが入力されている場合
					stp = strchr(stt+4, '_') ;
					*stp = '\0' ;
					Stank->dbld0 = atof(stt+4) ;
					Stank->dblL = atof(stp+1) ;
					Stank->Ncalcihex++ ;
				}

				Stank->Jout[i] = Stank->Jin[i];
				
			}
			/***  printf("<< Stankmemloc>> i=%d ihex=%c ihxeff=%lf\n",
			i, Stank->ihex[i], Stank->ihxeff[i]);***/
			
			/********************************	       	       
			for (k = 0; k < ii; k++)
			{
			if (Stank->Jond[k] == Stank->Jout[i])
			break;
			}
			if (k == ii)
			{
			Stank->Jond[ii] = Stank->Jout[i];
			Stank->pthond[ii] = Stank->pthcon[i];
			ii++;
			}
			Stank->Jojond[i] = k;
			************************************/
			i++;
		}
	}
	
	Stank->Nin = Nin = i;
	
	Ndiv = Stank->Ndiv;
	Stank->dtankF = scalloc(Ndiv, errkey);
	
	Stank->B = dcalloc(Ndiv * Ndiv, errkey);
	Stank->R = dcalloc(Ndiv, errkey);
	Stank->d = dcalloc(Ndiv, errkey);
	Stank->Fg = dcalloc(Ndiv * Nin, errkey);
	Stank->Tss = dcalloc(Ndiv, errkey);
	
	Stank->Tssold = dcalloc(Ndiv, errkey);
	Stank->dvol = dcalloc(Ndiv, errkey);
	Stank->Mdt = dcalloc(Ndiv, errkey);
	Stank->KS = dcalloc(Ndiv, errkey);
	Stank->cGwin = dcalloc(Nin, errkey);
	Stank->EGwin = dcalloc(Nin, errkey);
	Stank->Twin = dcalloc(Nin, errkey);
	Stank->Q = dcalloc(Nin, errkey);
	if (Nin > 0 && (Stank->stkdy = (STKDAY *)malloc(Nin * sizeof(STKDAY))) == NULL)
		Ercalloc(Nin, errkey);
	if (Nin > 0 && (Stank->mstkdy = (STKDAY *)malloc(Nin * sizeof(STKDAY))) == NULL)
		Ercalloc(Nin, errkey);
}
/* ------------------------------------------------------- */

/* 蓄熱槽初期設定 */

void Stankint(double dTM, int Nstank, STANK *Stank, 
			  SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd)
{
	int		i, j;
	char	*s, ss[SCHAR], mrk, Err[SCHAR], E[SCHAR] ;
	double	Tso;
	
	strcpy ( E, "Stankint" ) ;

	for (i = 0; i < Nstank; i++, Stank++)
	{ 
		// 内蔵熱交換器の熱伝達率計算用温度の初期化
		Stank->dblTa = Stank->dblTw = 20.0 ;

	/*** printf("<<Stankint>> i=%d %s  tparm=%s\n",
		i, Stank->name, Stank->cmp->tparm);****/      
		/***Stank->Twinit = *Stank->cmp->ivparm;***/
		
		s = Stank->cmp->tparm;
		if ( s != NULL )
		{
			if (*s == '(')
			{
				s++;
				for (j = 0; j < Stank->Ndiv; j++)
				{
					sscanf(s, " %s ", ss);
					
					if (*ss == TANK_EMPTY)
					{
						Stank->dtankF[j] = TANK_EMPTY;
						Stank->Tssold[j] = TANK_EMPTMP;
					}
					else
					{
						Stank->dtankF[j] = TANK_FULL;
						Stank->Tssold[j] = atof(ss);
					}
					s += strlen(ss);
					while(isspace(*(++s)));
				}
			}
			else
			{
				if (*s == TANK_EMPTY)
				{
					mrk = TANK_EMPTY;
					Tso = TANK_EMPTMP;
				}
				else
				{
					mrk = TANK_FULL;
					Tso = atof(s);
				}
				for (j = 0; j < Stank->Ndiv; j++)
				{
					Stank->dtankF[j] = mrk;
					Stank->Tssold[j] = Tso;
				}
			}
		}
		
		/*********
		printf("<< Stankint>> Tsinit\n");
		for (j = 0; j < Stank->Ndiv; j++)
		printf("     Tssold[%d]=%lf\n", j, Stank->Tssold[j]);
		
		 printf("<<Stankint>>  env=%s  Tinit=%lf  %lf\n", Stank->cmp->envname, 
		 *Stank->cmp->ivparm, Stank->Twinit);
		*************/
		Stank->Tenv = envptr(Stank->cmp->envname, Simc, Ncompnt, Compnt, Wd, NULL);
		stoint(dTM, Stank->Ndiv, Stank->cat->Vol, Stank->cat->KAside, 
			Stank->cat->KAtop, Stank->cat->KAbtm, Stank->dvol, 
			Stank->Mdt, Stank->KS, Stank->Tss, Stank->Tssold,
			&Stank->Jva, &Stank->Jvb);
		//Stank->Ncalcihex = 0 ;

		if ( Stank->cat->Vol < 0.0 )
		{
			sprintf ( Err, "Name=%s  Vol=%.4g", Stank->cmp->name, Stank->cat->Vol ) ;
			Eprint ( E, Err ) ;
		}
		if ( Stank->cat->KAside < 0.0 )
		{
			sprintf ( Err, "Name=%s  KAside=%.4g", Stank->cmp->name, Stank->cat->KAside ) ;
			Eprint ( E, Err ) ;
		}
		if ( Stank->cat->KAtop < 0.0 )
		{
			sprintf ( Err, "Name=%s  KAtop=%.4g", Stank->cmp->name, Stank->cat->KAtop ) ;
			Eprint ( E, Err ) ;
		}
		if ( Stank->cat->KAbtm < 0.0 )
		{
			sprintf ( Err, "Name=%s  KAbtm=%.4g", Stank->cmp->name, Stank->cat->KAbtm ) ;
			Eprint ( E, Err ) ;
		}
	}
}

/* ------------------------------------------------------- */ 

/* 蓄熱槽特性式係数 */

void Stankcfv(int Nstank, STANK *Stank) 
{ 
	ELOUT  *Eo;
	ELIN   *elin;
	int    i, j, k;
	double  *cfin, *cGwin, *EGwin;
	double	*ihxeff, NTU ;
	char	*ihex ;
	double *fg;
	double	ho, hi ;
	double	dblT, dblv ;
	extern double row;
	
	for (i = 0; i < Nstank; i++, Stank++)
	{
	/********************
	if (Itr == 0 || (Itr > 0 && Stank->cfcalc == 'y'))
	{
		***********************/
		elin = Stank->cmp->elins;
		cGwin = Stank->cGwin;
		EGwin = Stank->EGwin;
		ihxeff = Stank->ihxeff ;
		ihex = Stank->ihex ;
		for (j = 0; j < Stank->Nin; j++, elin++, ihxeff++, ihex++)
		{
			if ( elin->lpath->batch == 'y')
				*cGwin = 0.0;
			else
				*cGwin = spcheat('W') * elin->lpath->G;
			
				/***printf("<<Stankcfv>> j=%d con=%c cGwin=%lf\n",
			j, *Stank->batchcon[j],*cGwin);***/
			
			// 内蔵熱交のKAが入力されている場合
			if(*ihex == 'y' && *cGwin > 0.0 )
			{
				// 内蔵熱交換器の内径、管長が入力されている場合
				if ( Stank->KAinput[j] == 'C')
				{
					dblT =(Stank->dblTa + Stank->dblTw) / 2. ;
					// 内蔵熱交換器の表面温度は内外流体の平均温度で代用
					ho = FNhoutpipe(Stank->dbld0, dblT, Stank->dblTw) ;
					// 流速の計算
					dblv = elin->lpath->G / row / (PI * pow(Stank->dbld0 / 2., 2.)) ;
					hi = FNhinpipe(Stank->dbld0, Stank->dblL, dblv, dblT) ;
					Stank->KA[j] = 1. / ( 1. / ho + 1. / hi ) * PI * Stank->dbld0 * Stank->dblL ;
				}
				if ( Stank->KAinput[j] == 'Y' || Stank->KAinput[j] == 'C' )
				{
					NTU = Stank->KA[j] / *cGwin ;
					*ihxeff = 1. - exp(- NTU) ;
				}
			}
			//printf("ihexeff=%lf\n", *ihxeff) ;
			*EGwin = *cGwin * *ihxeff;
			
			cGwin++;
			EGwin++;
		}
		/***********************
		printf("\nxxx Stankcfv  Nin=%d Jin/Jout=", Stank->Nin);
		for (j = 0; j < Stank->Nin; j++)
		printf("   %d/%d", Stank->Jin[j], Stank->Jout[j]);
		printf("\n");
		**************************/
		
		stofc(Stank->Ndiv, Stank->Nin, Stank->Jin, 
			Stank->Jout, Stank->ihex, Stank->ihxeff, Stank->Jva, Stank->Jvb,
			Stank->Mdt, Stank->KS, Stank->cat->gxr, Stank->Tenv, 
			Stank->Tssold, Stank->cGwin, Stank->EGwin, Stank->B, Stank->R, Stank->d, Stank->Fg);
		Eo = Stank->cmp->elouts;
		fg = Stank->Fg;
		
		for (j = 0; j < Stank->Nin; j++)
		{
			Eo->coeffo = 1.0;
			Eo->Co = Stank->d[Stank->Jout[j]];
			/***************
			printf("<<Stankcfv>>  j=%d Co=%lf\n", j, Eo->Co);**********/
			
			cfin = Eo->coeffin;
			for (k = 0; k < Stank->Nin; k++)
			{
				*cfin = - *fg;
				cfin++ ;
				fg++ ;
			}
			Eo++;
		}
		/***************
		}
		*****************/
	}
}
/* ------------------------------------------------------- */ 

/*  蓄熱槽内部水温のポインター  */

int stankvptr(char **key, STANK *Stank, VPTR *vptr)
{
	int err = 0, i;
	char  *s;
	
	if (strcmp(key[1], "Ts") == 0)
	{
		s =  key[2];
		if (isalpha(*s))
		{
			if (*s == 't')
			{
				vptr->ptr = &Stank->Tssold[0];
				vptr->type = VAL_CTYPE;
			}
			else if (*s == 'b')
			{
				vptr->ptr = &Stank->Tssold[Stank->Ndiv - 1];
				vptr->type = VAL_CTYPE;
			}
			else
				err = 1;
		}
		else
		{   
			i = atoi(s);
			if ((i >= 0) && (i < Stank->Ndiv))
			{
				vptr->ptr = &Stank->Tssold[i];
				vptr->type = VAL_CTYPE;
			}
			else
				err = 1;
		}
	}
	else
		err = 1;
	
	return err;
}
/* ------------------------------------------------------- */ 

/* 槽内水温、水温分布逆転の検討 */

void Stanktss(int Nstank, STANK *Stank, int *TKreset)
{
	int  i, j;
	ELIN *eli;
	
//	*TKreset = 0;
	for (i = 0; i < Nstank; i++, Stank++)
	{
		eli = Stank->cmp->elins;
		
		for (j = 0; j < Stank->Nin; j++, eli++)
			Stank->Twin[j] = eli->sysvin;
		
		stotss(Stank->Ndiv, Stank->Nin, Stank->Jin, Stank->B, Stank->R, Stank->EGwin, Stank->Twin,
			Stank->Tss);
		
			/*****************
			for ( j = 0; j < Stank->Ndiv; j++ )
			printf ( "Tss[%d]=%.1lf\n", j, Stank->Tss[j] ) ;
		************************/
		
		stotsexm(Stank->Ndiv, Stank->Tss, &Stank->Jva, &Stank->Jvb, 
			Stank->dtankF, &Stank->cfcalc);
			/***
			printf("<<Stanktss>> cfcalc=%c i=%d Jva=%d Jvb=%d\n", Stank->cfcalc, i, Stank->Jva, Stank->Jvb);
		***/
		if(Stank->cfcalc == 'y')
			*TKreset = 1;
	}
}
/* ------------------------------------------------------- */ 

/* 供給熱量、損失熱量計算、水温前時間値の置換 */

void Stankene(int Nstank, STANK *Stank)
{
	int  i, j, Jo, k;
	double  Tsm;
	double	*EGwin, *Twin, *Q ;
	char	*ihex ;
	
	for (i = 0; i < Nstank; i++, Stank++)
	{ 
		// バッチ式対応（分割層が満水か空かをチェック
		for (k = 0; k < Stank->Ndiv; k++)
		{
			if (Stank->dtankF[k] == TANK_EMPTY)
				Stank->Tss[k] = TANK_EMPTMP;
		}

		// バッチ式の時の給水
		if (Stank->batchop == BTFILL)
		{
			Tsm = 0.0;
			for (k = 0; k < Stank->Ndiv; k++)
			{
				if (Stank->dtankF[k] == TANK_EMPTY)
				{
					Stank->dtankF[k] = TANK_FULL;
					for (j = 0; j < Stank->Nin; j++)
					{
					/***printf("<<Stankene>>  j=%d Twin=%lf\n",j, Stank->Twin[j]);
						***/
						if (*Stank->batchcon[j] == BTFILL)
							Stank->Tss[k] = Stank->Twin[j];
					}
				}
				Tsm += Stank->Tss[k];
			}
			Tsm /= Stank->Ndiv;
			for (k = 0; k < Stank->Ndiv; k++)
				Stank->Tss[k] = Tsm;
		}
		
		EGwin = Stank->EGwin ;
		Twin = Stank->Twin ;
		Q = Stank->Q ;
		ihex = Stank->ihex ;
		for (j = 0; j < Stank->Nin; j++, EGwin++, Twin++, Q++, ihex++)
		{
			Jo = Stank->Jout[j];
			*Q = *EGwin * (Stank->Tss[Jo] - *Twin);

			// 内蔵熱交換器の場合
			if(Stank->KAinput[j] == 'C')
			{
				Stank->dblTa = Stank->Tss[Jo] ;
				if(*EGwin > 0.)
					Stank->dblTw = *Twin ;
			}
			//if ( j == 1 )
			//	printf("Q=%lf EGwin=%lf Ts=%lf Twin=%lf\n", *Q, *EGwin, Stank->Tss[Jo], *Twin ) ;
		}
		
		Stank->Qloss = 0.0;
		Stank->Qsto = 0.0;
		for (j = 0; j < Stank->Ndiv; j++)
		{
			if (Stank->dtankF[j] == TANK_FULL)
			{
				Stank->Qloss += Stank->KS[j] * ( Stank->Tss[j] - *Stank->Tenv);
				if (Stank->Tssold[j] > -273.0)
					Stank->Qsto += Stank->Mdt[j] * 
					(Stank->Tss[j] - Stank->Tssold[j]);
			}
			Stank->Tssold[j] = Stank->Tss[j];
		}
	}
}
/* ------------------------------------------------------- */ 

// 代表日の出力
void stankcmpprt (FILE *fo, int id, int Nstank, STANK *Stank)

{
	ELIN	*Ei;
	ELOUT	*Eo;
	int     m, i;
	char	c;
	double	*Q, *Twin, *Tss ;
	
	switch (id)
	{
	case 0:
		if (Nstank > 0)
            fprintf(fo, "%s %d\n", STANK_TYPE, Nstank);
		
		for (m = 0; m < Nstank; m++, Stank++)
		{
            fprintf(fo, "%s:%d", Stank->name, Stank->Nin);
			for (i = 0; i < Stank->Nin; i++)
				fprintf(fo, "%c", Stank->cmp->idi[i]);
			
			fprintf(fo, " 1 %d\n", Stank->Nin * 5 + 2 + Stank->Ndiv + Stank->Ncalcihex);
		}
		break;
		
	case 1:
		for (m = 0; m < Nstank; m++, Stank++)
		{          
			for (i = 0; i < Stank->Nin; i++)
			{
				c = Stank->cmp->idi[i];
				fprintf(fo, "%s:%c_c c c %s:%c_G m f %s:%c_Ti t f %s:%c_To t f %s:%c_Q q f  ", 
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c);
				if(Stank->KAinput[i] == 'C')
					fprintf(fo, "%s:%c_KA q f  ", Stank->name, c) ;
				fprintf(fo, "\n" ) ;
			}
			fprintf(fo, "%s_Qls q f %s_Qst q f\n ", Stank->name, Stank->name);
			for (i = 0; i < Stank->Ndiv; i++)
				fprintf(fo, "%s_Ts[%d] t f ", Stank->name, i + 1);
			fprintf(fo, "\n");
		}
		break;
		
	default: 
		for (m = 0; m < Nstank; m++, Stank++)
		{ 
			Ei = Stank->cmp->elins;
			Twin = Stank->Twin ;
			Q = Stank->Q ;
			Eo = Stank->cmp->elouts;
            for (i = 0; i < Stank->Nin; i++, Ei++, Twin++, Q++, Eo++ )
			{
				fprintf (fo, "%c %.5g %4.1lf %4.1lf %3.0lf  ", Ei->lpath->control,
					Eo->G, *Twin, Eo->sysv, *Q );

				if(Stank->KAinput[i] == 'C')
				{
					if ( Eo->G > 0.)
						fprintf(fo, "%.2lf  ", Stank->KA[i]) ;
					else
						fprintf(fo, "%.2lf  ", 0.) ;
				}
			}	 
			fprintf(fo, "%2.0lf %3.0lf\n", Stank->Qloss, Stank->Qsto);

			Tss = Stank->Tss ;
			for (i = 0; i < Stank->Ndiv; i++, Tss++)
				fprintf(fo, " %4.1lf", *Tss );
            fprintf(fo, "\n");
		}
		break;
	} 
}

/* ------------------------------------------------------- */ 

void stankivprt (FILE *fo, int id, int Nstank, STANK *Stank)

{
	int     m, i;
	
	if (id == 0 && Nstank > 0)
	{
		for (m = 0; m < Nstank; m++, Stank++)
			fprintf(fo, "m=%d  %s  %d\n", m, Stank->name, Stank->Ndiv);
		
		//fprintf(fo, " *\n");
	}
	else 
	{
		for (m = 0; m < Nstank; m++, Stank++)
		{ 
			fprintf(fo, "m=%d  ", m);
			
			for (i = 0; i < Stank->Ndiv; i++)
				fprintf (fo, " %5.1lf", Stank->Tss[i]);
			fprintf(fo, "\n");
		}
	} 
}

/* --------------------------- */

/* 日積算値に関する処理 */

void stankdyint(int Nstank, STANK *Stank)
{
	int  i, j;
	STKDAY *S;
	
	for (i = 0; i < Nstank; i++, Stank++)
	{
		Stank->Qlossdy = 0.0;
		Stank->Qstody = 0.0;
		S = Stank->stkdy;      
		for (j = 0; j < Stank->Nin; j++, S++)
		{
			svdyint(&S->Tidy);
			svdyint(&S->Tsdy);
			qdyint(&S->Qdy);
		}
	}
}

void stankmonint(int Nstank, STANK *Stank)
{
	int  i, j;
	STKDAY *S;
	
	for (i = 0; i < Nstank; i++, Stank++)
	{
		Stank->mQlossdy = 0.0;
		Stank->mQstody = 0.0;
		S = Stank->mstkdy;      
		for (j = 0; j < Stank->Nin; j++, S++)
		{
			svdyint(&S->Tidy);
			svdyint(&S->Tsdy);
			qdyint(&S->Qdy);
		}
	}
}

// 日集計、月集計
void stankday(int Mon, int Day, int ttmm, int Nstank, STANK *Stank, int Nday, int SimDayend)
{
	int  i, j;
	STKDAY *S;
	ELIN   *Ei;
	
	for (i = 0; i < Nstank; i++, Stank++)
	{
		// 日集計
		double Ts ;
		Ts = 0.;

		S = Stank->stkdy; 
		for (j = 0; j < Stank->Ndiv; j++)
			Ts += Stank->Tss[j] / (double) Stank->Ndiv ;
		svdaysum(ttmm, ON_SW, Ts, &S->Tsdy) ;

		Stank->Qlossdy += Stank->Qloss;
		Stank->Qstody += Stank->Qsto;
		
		Ei = Stank->cmp->elins;
		for (j = 0; j < Stank->Nin; j++, S++, Ei++)
		{	 
			svdaysum(ttmm, Ei->lpath->control, Stank->Twin[j], &S->Tidy);
			qdaysum(ttmm, Ei->lpath->control, Stank->Q[j], &S->Qdy);
		}

		// 月集計
		S = Stank->mstkdy; 
		svmonsum(Mon, Day, ttmm, ON_SW, Ts, &S->Tsdy, Nday, SimDayend) ;

		Stank->mQlossdy += Stank->Qloss;
		Stank->mQstody += Stank->Qsto;
		
		Ei = Stank->cmp->elins;
		for (j = 0; j < Stank->Nin; j++, S++, Ei++)
		{	 
			svmonsum(Mon, Day, ttmm, Ei->lpath->control, Stank->Twin[j], &S->Tidy, Nday, SimDayend);
			qmonsum(Mon, Day, ttmm, Ei->lpath->control, Stank->Q[j], &S->Qdy, Nday, SimDayend);
		}
	}
}

// 日集計の出力
void stankdyprt(FILE *fo, int id, int Nstank, STANK *Stank)
{
	int  i, j, m;
	char c;
	STKDAY *S;   
	extern double  cff_kWh;
	
	
	switch (id)
	{
	case 0:
		if (Nstank > 0)
            fprintf(fo, "%s %d\n", STANK_TYPE, Nstank);
		
		for (m = 0; m < Nstank; m++, Stank++)
		{
            fprintf(fo, "%s:%d", Stank->name, Stank->Nin);
			for (i = 0; i < Stank->Nin; i++)
				fprintf(fo, "%c", Stank->cmp->idi[i]);
			
			fprintf(fo, " 1 %d\n", Stank->Nin * 14 + 2 + 1);
		}
		break;
		
	case 1:      
		for (m = 0; m < Nstank; m++, Stank++)
		{  
			fprintf(fo,"%s_Ts t f \n", Stank->name) ;

			for (i = 0; i < Stank->Nin; i++)
			{
				c = Stank->cmp->idi[i];
				fprintf(fo,"%s:%c_Ht H d %s:%c_T T f ", Stank->name, c, Stank->name, c);
				fprintf(fo,"%s:%c_ttn h d %s:%c_Tn t f %s:%c_ttm h d %s:%c_Tm t f\n",
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c) ;
				fprintf(fo, "%s:%c_Hh H d %s:%c_Qh Q f %s:%c_Hc H d %s:%c_Qc Q f\n",
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c);
				fprintf(fo, "%s:%c_th h d %s:%c_qh q f %s:%c_tc h d %s:%c_qc q f\n",
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c);
			}
			fprintf(fo, "%s_Qls Q f %s_Qst Q f\n\n", Stank->name, Stank->name);
		}
		break;
		
	default:
		for (i = 0; i < Nstank; i++, Stank++)
		{
            S = Stank->stkdy; 

			fprintf(fo, "%.1lf\n", S->Tsdy.m) ;
            for (j = 0; j < Stank->Nin; j++, S++)
            {	 
				fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
					S->Tidy.hrs, S->Tidy.m, 
					S->Tidy.mntime, S->Tidy.mn, 
					S->Tidy.mxtime, S->Tidy.mx);

				fprintf(fo, "%1ld %3.1lf ", S->Qdy.hhr, S->Qdy.h);
				fprintf(fo, "%1ld %3.1lf ", S->Qdy.chr, S->Qdy.c);      
				fprintf(fo, "%1ld %2.0lf ", S->Qdy.hmxtime, S->Qdy.hmx);
				fprintf(fo, "%1ld %2.0lf ", S->Qdy.cmxtime, S->Qdy.cmx);
			}
            fprintf(fo, " %3.1lf %3.1lf\n", 
				Stank->Qlossdy * cff_kWh, Stank->Qstody * cff_kWh);	    
		}
		break;
	}   
}

// 月集計の出力
void stankmonprt(FILE *fo, int id, int Nstank, STANK *Stank)
{
	int  i, j, m;
	char c;
	STKDAY *S;   
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nstank > 0)
            fprintf(fo, "%s %d\n", STANK_TYPE, Nstank);
		
		for (m = 0; m < Nstank; m++, Stank++)
		{
            fprintf(fo, "%s:%d", Stank->name, Stank->Nin);
			for (i = 0; i < Stank->Nin; i++)
				fprintf(fo, "%c", Stank->cmp->idi[i]);
			
			fprintf(fo, " 1 %d\n", Stank->Nin * 14 + 2 + 1);
		}
		break;
		
	case 1:      
		for (m = 0; m < Nstank; m++, Stank++)
		{  
			fprintf(fo,"%s_Ts t f \n", Stank->name) ;

			for (i = 0; i < Stank->Nin; i++)
			{
				c = Stank->cmp->idi[i];
				fprintf(fo,"%s:%c_Ht H d %s:%c_T T f ", Stank->name, c, Stank->name, c);
				fprintf(fo,"%s:%c_ttn h d %s:%c_Tn t f %s:%c_ttm h d %s:%c_Tm t f\n",
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c) ;
				fprintf(fo, "%s:%c_Hh H d %s:%c_Qh Q f %s:%c_Hc H d %s:%c_Qc Q f\n",
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c);
				fprintf(fo, "%s:%c_th h d %s:%c_qh q f %s:%c_tc h d %s:%c_qc q f\n",
					Stank->name, c, Stank->name, c, Stank->name, c, Stank->name, c);
			}
			fprintf(fo, "%s_Qls Q f %s_Qst Q f\n\n", Stank->name, Stank->name);
		}
		break;
		
	default:
		for (i = 0; i < Nstank; i++, Stank++)
		{
            S = Stank->mstkdy; 

			fprintf(fo, "%.1lf\n", S->Tsdy.m) ;
            for (j = 0; j < Stank->Nin; j++, S++)
            {	 
				fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
					S->Tidy.hrs, S->Tidy.m, 
					S->Tidy.mntime, S->Tidy.mn, 
					S->Tidy.mxtime, S->Tidy.mx);

				fprintf(fo, "%1ld %3.1lf ", S->Qdy.hhr, S->Qdy.h);
				fprintf(fo, "%1ld %3.1lf ", S->Qdy.chr, S->Qdy.c);      
				fprintf(fo, "%1ld %2.0lf ", S->Qdy.hmxtime, S->Qdy.hmx);
				fprintf(fo, "%1ld %2.0lf ", S->Qdy.cmxtime, S->Qdy.cmx);
			}
            fprintf(fo, " %3.1lf %3.1lf\n", 
				Stank->mQlossdy * cff_kWh, Stank->mQstody * cff_kWh);	    
		}
		break;
	}   
}




