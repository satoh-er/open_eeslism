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

/* esccntldat.c */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "fesy.h"
#include "esize.h"
#include "fnlib.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnmcs.h"

/*  制御、スケジュール設定式の入力  */

void Contrldata(FILE *fi, CONTL **Ct, int *Ncontl, CTLIF **Ci, int *Nctlif,
				CTLST **Cs, int *Nctlst, 
				SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt,
				int Nmpath, MPATH *Mpath, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl)
{
	COMPNT	*loadcmp, *cmp ; //, *Vc ;  
	CONTL	*contl, *Contl, *ctl ;
	CTLIF	*ctlif, *Ctlif, *cti ;
	CTLST	*ctlst, *Ctlst, *cts ;
	VPTR	vptr, vpath;
	char	s[SCHAR], *st, Er[SCHAR+128], *load ; //, ss[SCHAR] ;
	int		err, i, Ni, N, Nm ; //, k ;
	char	Err[SCHAR] ;
	//	VALV	*Valv, *Vb ;


	loadcmp = NULL ;
	Hload = HEATING_LOAD;
	Cload = COOLING_LOAD;
	HCload = HEATCOOL_LOAD;
	sprintf(Err, ERRFMT, "(Contrldata)");

	ContrlCount ( fi, &Ni, &N ) ;

	Nm = N ;
	if ( Nm > 0 )
	{
		//		if (( ctl = ( CONTL * ) malloc ( sizeof ( CONTL ) * Nm )) == NULL )
		if (( *Ct = ( CONTL * ) malloc ( sizeof ( CONTL ) * Nm )) == NULL )
			Ercalloc(Nm, "<Contrldata> Ct" );
		else
		{
			ctl = *Ct ;
			for ( i = 0; i < Nm; i++, ctl++ )
			{
				ctl->lgv = 0 ;
				ctl->type = ' ' ;
				ctl->cif = NULL ;
				ctl->andcif = NULL ;
				ctl->andandcif = NULL ;
				ctl->orcif = NULL ;
				ctl->cst = NULL ;
			}
		}

		if (( *Cs = ( CTLST * ) malloc ( sizeof ( CTLST ) * Nm )) == NULL )
			Ercalloc(Nm, "<Contrldata> Cs" ) ;
		else
		{
			cts = *Cs ;
			for ( i = 0; i < Nm; i++, cts++ )
			{
				cts->type = cts->pathtype = ' ' ;
				cts->path = NULL ;
			}
		}
	}

	Nm = Ni ;
	if ( Ni > 0 )
	{
		if (( *Ci = ( CTLIF * ) malloc ( sizeof ( CTLIF ) * Nm )) == NULL )
			Ercalloc(Nm, "<Contrldata> Ci");
		else
		{
			cti = *Ci ;
			for ( i = 0; i < Nm; i++, cti++ )
			{
				cti->type = cti->op = ' ' ;
				cti->Nlft = 0 ;
			}
		}
	}

	Contl = *Ct ;
	Ctlst = *Cs ;
	Ctlif = *Ci ;
	contl = Contl;
	ctlif = Ctlif;
	ctlst = Ctlst;
	load = NULL;

	while (fscanf(fi, "%s", s), *s != '*')
	{
		if (load != NULL)
			free(load);
		load = NULL;
		VPTRinit ( &vptr ) ;
		VPTRinit ( &vpath ) ;
		/***************
		printf("<<Contrldata>> s=%s\n",s) ; /************/

		Contl->type = ' ';
		Contl->cst = Ctlst;

		do
		{
			if (strcmp(s, "if") == 0)
			{

				Contl->type = 'c';
				Contl->cif = Ctlif;
				fscanf(fi, " (%[^)])", s);

				ctifdecode(s, Ctlif, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl);
				Ctlif++;
				*Nctlif = (int)(Ctlif - ctlif) ;

				/********************
				if ((*Nctlif = Ctlif - ctlif) > NCNTLIFMX)
				{
				sprintf(Er, "Ncntlif=%d [max=%d]", *Nctlif, NCNTLIFMX); 
				Eprint("<Contrldata>", Er);
				}		      
				/*************************/
			}
			else if (strcmp(s, "AND") == 0)
			{

				Contl->type = 'c';
				if ( Contl->andcif == NULL )
					Contl->andcif = Ctlif;
				else
					Contl->andandcif = Ctlif ;

				fscanf(fi, " (%[^)])", s);

				ctifdecode(s, Ctlif, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl);
				Ctlif++;
				*Nctlif = (int)(Ctlif - ctlif) ;

				/********************
				if ((*Nctlif = Ctlif - ctlif) > NCNTLIFMX)
				{
				sprintf(Er, "Ncntlif=%d [max=%d]", *Nctlif, NCNTLIFMX); 
				Eprint("<Contrldata>", Er);
				}		      
				/*************************/
			}
			else if (strcmp(s, "OR") == 0)
			{

				Contl->type = 'c';
				Contl->orcif = Ctlif;
				fscanf(fi, " (%[^)])", s);

				ctifdecode(s, Ctlif, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl);
				Ctlif++;
				*Nctlif = (int)(Ctlif - ctlif) ;

				/********************
				if ((*Nctlif = Ctlif - ctlif) > NCNTLIFMX)
				{
				sprintf(Er, "Ncntlif=%d [max=%d]", *Nctlif, NCNTLIFMX); 
				Eprint("<Contrldata>", Er);
				}		      
				/*************************/
			}
			else if (strncmp(s, "LOAD", 4) == 0)
			{

				loadcmp = NULL;

				if(strchr(s, ':') != NULL)
				{
					if (strlen(s + 5) == 1 && *(s + 5) == HEATING_LOAD)
						load = charalloc(Hload);
					//load = &Hload;
					else if (strlen(s + 5) == 1 && *(s + 5) == COOLING_LOAD)
						load = charalloc(Cload);
						//load = &Cload;
					else
					{
						if((i = idscw(s + 5, Schdl->Scw, NULL)) >= 0)
							load = &Schdl->isw[i];
						else{
							Eprint("<Contrldata>", s);
						}
					}
				}
				else
					load = &HCload;
			}
			else if (strcmp(s, "-e") == 0)
			{

				fscanf(fi, "%s", s);
				cmp = Compnt;
				for (i = 0; i < Ncompnt; i++, cmp++)
				{
					if (strcmp(s, cmp->name) == 0)
						loadcmp = cmp;
				}
			}
			else if ((st = strchr(s, '=')) != NULL)
			{

				*st = '\0' ;
				st++;

				if (load != NULL)
				{
					err = loadptr(loadcmp, load, s, Ncompnt, Compnt, &vptr);

					load = NULL;
				}
				else
				{
					vpath.type = '\0';
					err = ctlvptr(s, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl,
						&vptr, &vpath);

					/*****
					printf("xxxx Contrldata  err=%d vpath.type=%c\n",
					err, vpath.type);
					*****/
				}

				if (err == 0)
				{	       
					Ctlst->type = vptr.type;
					Ctlst->pathtype = vpath.type;
					Ctlst->path = vpath.ptr;

					if (Ctlst->type == VAL_CTYPE)
						Ctlst->lft.v = (double *)vptr.ptr;
					else
						Ctlst->lft.s = (char *)vptr.ptr;

					err = ctlrgtptr(st, &Ctlst->rgt, Simc, Ncompnt, Compnt, Nmpath, Mpath,
						Wd, Exsf, Schdl, Ctlst->type);
				}
				sprintf(Er, " %s = %s", s, st);
				Errprint(err, "<Contrldata>", Er);
			}
			else if ( strcmp ( s, "TVALV" ) == 0 )
			{
				Ctlst-- ;
				Contl-- ;

				ValvControl ( fi,  Ncompnt, Compnt, Schdl, Simc, Wd, &vptr ) ;
				/**************************************
				Ctlst->type = vptr.type ;
				Ctlst->pathtype = vptr.type ;
				Ctlst->path = vpath.ptr ;
				Ctlst->rgt.s = charalloc ( ON_SW ) ;
				Ctlst->lft.s = (char *) vptr.ptr ;
				/*********************************/
			}
			else{
				Eprint("<Contrldata>", s);
			}
		}
		while (fscanf(fi, "%s", s), *s != ';' ); 

		Ctlst++;
		*Nctlst = (int)(Ctlst - ctlst) ;

		/**************************
		if ((*Nctlst = Ctlst - ctlst) > NCNTLSTMX)
		{
		sprintf(Er, "Ncntlstf=%d [max=%d]", *Nctlst, NCNTLSTMX); 
		Eprint("<Contrldata>", Er);
		}	
		/**************************/

		Contl++;
		*Ncontl = (int)(Contl - contl) ;

		/**********************
		if ((*Ncontl = Contl - contl) > NCONTLMX)
		{
		sprintf(Er, "Ncontl=%d [max=%d]", *Ncontl, NCONTLMX); 
		Eprint("<Contrldata>", Er);
		}
		/***************************/
	}
}

void	ContrlCount ( FILE *fi, int *Nif, int *N )
{
	long	ad ;
	int		N1 = 0, N2 = 0 ;
	char	s[SCHAR] ;

	*Nif = *N = 0 ;

	ad = ftell ( fi ) ;

	while ( fscanf ( fi, " %s ", s ), s[0] != '*' )
	{
		if ( strcmp ( s, "if" ) == 0 || strcmp ( s, "AND" ) == 0
			|| strcmp ( s, "OR" ) == 0 )
			N1++ ;

		if ( strchr ( s, '=' ) != NULL )
			N2++ ;

		if ( strcmp ( s, "TVALV" ) == 0 )
			N2++ ;
	}

	*N = N2 ;
	*Nif = N1 ;
	fseek ( fi, ad, SEEK_SET ) ;
}

/* ------------------------------------------------------ */

/*  制御条件式 (lft1 - lft2 ? rgt ) に関するポインター */

void ctifdecode(char *s, CTLIF *ctlif, 
				SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt,
				int Nmpath, MPATH *Mpath, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl)
{
	char *st, lft[SCHAR], rgt[SCHAR], op[SCHAR];
	int  err;
	VPTR vptr, vpath;

	sscanf(s, "%s %s %s", lft, op, rgt);
	/**************
	printf("   ctifdecode  %s |  %s  | %s\n", lft, op, rgt);
	**************/

	if((st = strchr(lft, '-')) != NULL)
	{
		*st = '\0' ;
		st++;
	}

	ctlvptr(lft, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl, &vptr, &vpath);

	ctlif->type = vptr.type;
	ctlif->Nlft = 1;

	if (vptr.type == VAL_CTYPE)
		ctlif->lft1.v = (double *)vptr.ptr;
	else
		ctlif->lft1.s = (char *)vptr.ptr;

	if (st != NULL)
	{    
		ctlvptr(st, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl, &vptr, &vpath);

		if (vptr.type == VAL_CTYPE && ctlif->type == vptr.type)
		{
			ctlif->Nlft = 2;
			ctlif->lft2.v = (double *)vptr.ptr;
		}
		else
			Eprint("<ctifdecode>", st + 1);
	}

	err = 0;
	if (strcmp(op, ">") == 0)
		ctlif->op = 'g';
	else if (strcmp(op, ">=") == 0)
		ctlif->op = 'G';
	else if (strcmp(op, "<") == 0)
		ctlif->op = 'l';
	else if(strcmp(op, "<=") == 0)
		ctlif->op = 'L';
	else if (strcmp(op, "==") == 0)
		ctlif->op = 'E';
	else if(strcmp(op, "!=") == 0)
		ctlif->op = 'N';
	else
		err = 1;
	Errprint(err,"<ctifdecode>", s); 

	ctlrgtptr(rgt, &ctlif->rgt, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf,
		Schdl, ctlif->type);
}

/* ------------------------------------------------------ */

/*  条件式、設定式の右辺（定数、またはスケジュール設定値のポインター） */

int ctlrgtptr(char *s, CTLTYP *rgt, 
			  SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt,
			  int Nmpath, MPATH *Mpath, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl, char type)
{
	VPTR vptr, vpath;
	int  err = 1;


	if ((type == VAL_CTYPE) && isstrdigit(s))
	{
		rgt->v = dcalloc(1, "<ctlrgtptr>");
		*rgt->v = atof(s);
		err = 0;

	}
	else
	{
		if (strcmp(s, "OFF") == 0)
		{
			rgt->s = charalloc(OFF_SW);
			err = 0;
		}
		else if (strcmp(s, "ON") == 0)
		{
			rgt->s = charalloc(ON_SW);
			err = 0;
		} 

		else if (strcmp(s, "COOL") == 0)
		{
			rgt->s = charalloc(COOLING_SW);
			err = 0;
		} 
		else if (strcmp(s, "HEAT") == 0)
		{
			rgt->s = charalloc(HEATING_SW);
			err = 0;
		} 

		else if (type == SW_CTYPE && *s == '\'')
		{
			rgt->s = charalloc(*(s + 1));
			err = 0;
		}
	}
	if (err)
	{
		err = ctlvptr(s, Simc, Ncompnt, Compnt, Nmpath, Mpath, Wd, Exsf, Schdl, &vptr, &vpath);
		if (type == vptr.type)
		{
			if (type == VAL_CTYPE)
				rgt->v = (double *)vptr.ptr;
			else 
				rgt->s = (char *)vptr.ptr;
		}
		else
			err = 1;
	}
	Errprint(err, "<ctlrgtptr>", s); 
	return err;
}

