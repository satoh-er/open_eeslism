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

/*  ee_pathdat.c   */

/* システム要素の接続経路の入力 */

//#define	 DEBUG  0


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "fesy.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"
#include "fnmcs.h"
//#include "eepath.h"

void Pathdata (FILE *f, char *errkey, SIMCONTL *Simc, WDAT *Wd, int  Ncompnt,  COMPNT *Compnt, SCHDL *Schdl,
			   MPATH **M, int *Nmpath, PLIST **Plst, PELM **Plm, int *Npelm, int *Nplist, int ID, EQSYS *Eqsys )
{
	MPATH   *mpi, *Mpath ;
	COMPNT  *cmp, *C ;
	STANK   *stank;
	PLIST	*Plist, *Pl ;
	PELM	*Pelm, *Pelmpre ;
	QMEAS	*Qmeas ;
	//THEX	*Thex ;

	char	s[SCHAR], ss[SCHAR], sss[SCHAR], *etyp,  ci=' ', co=' ', *cp, *elm, *stv;
	int		i, j, m, ncv, idci, idco, err, iswc, Np, *N, *Nplst, k ;
	double	Go;
	int		Npl, Nm, Nplm ;
	//	int		Nin, Nout ;
	int		id = 0 ;
	int		iPlist = 0 ;
	//ELOUT	*Eo, *Eo2 ;
	//ELIN	*Eli, *Eli2 ;

	if ( DEBUG )
	{
		printf ( "\n" ) ;
		C = Compnt ;
		for ( i = 0; i < Ncompnt; i++, C++ )
			printf ( "name=%s Nin=%d  Nout=%d\n", C->name, C->Nin, C->Nout ) ;
	}

	Nm = Mpathcount ( f, &Npl ) ;
	*Nplist = Npl * 2 ;

	if ( Nm > 0 )
	{
		if (( *M = ( MPATH * ) malloc (( Nm * 2 ) * sizeof ( MPATH ))) == NULL )
			Ercalloc ( Nm * 2, "<Pathdata> Mpath alloc" ) ;

		Mpathinit ( Nm * 2, *M ) ;

		for ( i = 0; i < Nm * 2; i++, mpi++ )
			mpi = M[i] ;
	}

	if ( Npl > 0 )
	{
		if (( *Plst = ( PLIST * ) malloc (( Npl * 2 ) * sizeof ( PLIST ))) == NULL )
			Ercalloc ( Npl * 2, "<Pathdata> Plist alloc" ) ;

		Plistinit ( Npl * 2, *Plst ) ;

		for ( i = 0; i < Npl * 2; i++, Pl++ )
			Pl = Plst[i] ;
	}

	Mpath = *M ;
	Plist = *Plst ;

	N = icalloc ( Nm, "<Pathdata>  N alloc" ) ;

	Plcount ( f, N ) ;

	Nplm = Pelmcount ( f ) ;

	//printf ( "<Pathdata> Pelm=%d\n", Nplm ) ;

	if ( Nplm > 0 )
	{
		if (( *Plm = ( PELM * ) malloc ( Nplm * sizeof ( PELM ))) == NULL )
			Ercalloc ( Nplm, "<Pathdata> Pelm alloc" ) ;
		Pelminit ( Nplm, *Plm ) ;
	}

	Pelm = *Plm ;
	/************
	PELM  *pelminit;
	pelminit = Pelm;
	**********/

	Nplst = N ;
	mpi = Mpath;
	*Npelm = 0;
	//Eo = Eqsys->Hcload->cmp->elouts + 1 ;

	//Eo = Eqsys->Desi->cmp->elouts + 1 ;
	//Eo = Eqsys->Hcload->cmp->elouts + 2 ;
	//Eli = Eo->elins + 1 ;
	//Eo2 = Eqsys->Desi->cmp->elouts + 1 ;
	//Eli2 = Eo2->elins ;

	if ( ID == 0 )
	{
		while (fscanf_s(f, "%s", ss, sizeof(ss)), *ss != '*')
		{
			if ( DEBUG )
			{
				printf ( "eepathdat.c  ss=%s\n", ss ) ;
			}

			if ( *Nplst > 0 )
			{
				if (( Mpath->pl = ( PLIST ** ) malloc ( *Nplst * sizeof ( PLIST * ))) == NULL )
					Ercalloc ( *Nplst, "<Pathdata> Mpath->pl  alloc" ) ;

				for ( i = 0; i < *Nplst; i++ )
					Mpath->pl[i] = NULL ;
			}

			if ( *Nplst > 0 )
			{
				if (( Mpath->cbcmp = ( COMPNT ** ) malloc (( *Nplst * 2 ) * sizeof ( COMPNT * ))) == NULL )
					Ercalloc ( *Nplst * 2, "<Pathdata> Mpath->cbcmp  alloc" ) ;

				for ( i = 0; i < *Nplst * 2; i++ )
					Mpath->cbcmp[i] = NULL ;
			}

			Mpath->name = stralloc(ss);
			Mpath->NGv = 0 ;
			Mpath->NGv2 = 0 ;
			Mpath->Ncv = 0 ;
			Mpath->Nlpath = 0 ;
			Mpath->plist = Plist;
			Pelmpre = NULL ;

			while (fscanf_s(f, "%s", s, sizeof(s)), *s != ';')
			{
				if ( DEBUG )
				{
					printf ( "eepathdat.c  s=%s\n", s ) ;
				}

				if (*s == '-')
				{
					fscanf_s(f, "%s", ss, sizeof(ss));

					if (strcmp(s + 1, "sys") == 0)
						Mpath->sys = *ss;
					else if (strcmp(s + 1, "f") == 0)
						Mpath->fluid = *ss;
					else
						Errprint(1, errkey, s);	  
				}     
				else if (*s == '>')
				{
					sprintf_s(sss, sizeof(sss), "Path%d", iPlist) ;
					Plist->plistname = stralloc(sss) ;
					Plist->pelm = Pelm;

					while (fscanf_s(f, "%s", s, sizeof(s)), *s != '>')
					{
						if ( DEBUG )
						{
							printf ( "eepathdat.c  s=%s\n", s ) ;
						}

						if (*s == '(' )
						{
							i = sscanf_s(s + 1, "%lf", &Go);
							if (i == 1)
							{
								Plist->Go = dcalloc(1, errkey);
								*Plist->Go = Go;
								if ( DEBUG )
								{
									printf ("Go=%lf\n", *Plist->Go) ;
								}
							}
							else
							{
								sscanf_s(s + 1, "%[^)])", ss, sizeof(ss));

								if ( DEBUG )
								{
									printf ( "s=%s ss=%s\n", s, ss ) ;
								}

								if ((j = idsch(ss, Schdl->Sch, NULL)) >= 0)
									Plist->Go = &Schdl->val[j];
								else
									Plist->Go = envptr ( ss, Simc, Ncompnt, Compnt, Wd, NULL ) ;

								if ( DEBUG )
								{
									printf ( "Go=%lf\n", *Plist->Go ) ;
								}
							}
						}
						else if (*s == '[')
						{
							// 流量比率設定フラグのセット
							Mpath->rate = 'Y' ;

							i = sscanf_s(s + 1, "%lf", &Go);
							if (i == 1)
							{
								Plist->rate = dcalloc(1, errkey);
								*Plist->rate = Go;
								if ( DEBUG )
								{
									printf ("rate=%lf\n", *Plist->rate) ;
								}
							}
							else
							{
								sscanf_s(s + 1, "%[^)])", ss, sizeof(ss));

								if ( DEBUG )
								{
									printf ( "s=%s ss=%s\n", s, ss ) ;
								}

								if ((j = idsch(ss, Schdl->Sch, NULL)) >= 0)
									Plist->rate = &Schdl->val[j];
								else
									Plist->rate = envptr ( ss, Simc, Ncompnt, Compnt, Wd, NULL ) ;

								if ( DEBUG )
								{
									printf ( "rate=%lf\n", *Plist->rate ) ;
								}
							}
						}
						else
						{
							// 末端経路名称
							if(strncmp(s,"name=",5) == 0)
							{
								sscanf_s(s, "%*[^=]=%s", ss, sizeof(ss)) ;
								free(Plist->plistname) ;
								Plist->plistname = NULL ;
								Plist->plistname = stralloc(ss) ;
							}
							else
							{
								if ((stv = strchr(s, '/')) != NULL)
								{
									*stv = '\0';
									stv++;
								}		 
								if ((cp = strchr(s, ':')) != NULL)
								{
									Plist->name = stralloc(s);
									*cp = '\0';
									elm = s;
								}		     
								else
								{
									if ((cp = strchr(s, '[')) != NULL)
									{
										*cp ='\0';
										elm = s;
										co = ci = *(cp + 1);
									}
									else
										elm = s, co = ci ='\0';
								}
								err = 1;

								for (cmp=Compnt; cmp < Compnt + Ncompnt; cmp++)
								{
									C = cmp ;
									if (cmp->name != NULL && strcmp(cmp->name, elm) == 0)
									{		       
										err=0;
										if (strcmp(cmp->eqptype, FLIN_TYPE) == 0 &&
											Plist->pelm == Pelm)
											Plist->type = IN_LPTP;
										else if (strcmp(cmp->eqptype, VALV_TYPE) == 0
											|| strcmp(cmp->eqptype, TVALV_TYPE) == 0)
										{
											Plist->Nvalv++ ;
											Plist->valv = ( VALV * ) cmp->eqp ;
											Plist->valv->Plist = Plist ;
										}
										// Satoh OMVAV 2010/12/16
										else if (strcmp(cmp->eqptype, OMVAV_TYPE) == 0)
										{
											Plist->NOMVAV++ ;
											Plist->OMvav = ( OMVAV * ) cmp->eqp ;
											Plist->OMvav->Plist = Plist ;
										}
										/*******************************
										else if (strcmp(cmp->eqptype, PUMP_TYPE) == 0)
										{
										Plist->Npump++;
										Plist->Pump = (PUMP *) cmp->eqp ;
										}
										/*********************************/

										/*---- Satoh Debug VAV  2000/12/6 ----*/
										else if (strcmp(cmp->eqptype, VAV_TYPE) == 0
											|| strcmp(cmp->eqptype, VWV_TYPE) == 0)
											Plist->Nvav++ ;
										/*---- Satoh Debug QMEAS  2003/6/2 ----*/
										else if (strcmp(cmp->eqptype, QMEAS_TYPE) == 0)
										{
											Qmeas = ( QMEAS * ) cmp->eqp ;
											if ( co == 'G' )
											{
												Qmeas->G = &Plist->G ;
												Qmeas->PlistG = Plist ;
												Qmeas->fluid = Mpath->fluid ;
											}
											else if ( co == 'H' )
											{
												/*******************
												//Qmeas->PlistTh = Plist ;
												Qmeas->Pelmh = Pelmpre ;

												Qmeas->idh = 'o' ;
												if ( Pelmpre != NULL && Pelmpre->ci == '>' )
												{
												//Qmeas->Th = &Pelm->in->sysvin ;
												Qmeas->idh = 'i' ;
												}
												//else
												//Qmeas->Th = &Pelm->out->sysv ;
												*************************/
												Qmeas->PlistTh = Plist ;
												Qmeas->Nelmh = id ;
											}
											else if ( co == 'C' )
											{
												/********************
												//Qmeas->PlistTc = Plist ;
												Qmeas->Pelmc = Pelmpre ;

												Qmeas->idc = 'o' ;
												if ( Pelmpre != NULL && Pelmpre->ci == '>' )
												{
												//Qmeas->Tc = &Pelm->in->sysvin ;
												Qmeas->idc = 'i' ;
												}
												//else
												//Qmeas->Tc = &Pelm->out->sysv ;
												***********************/
												Qmeas->PlistTc = Plist ;
												Qmeas->Nelmc = id ;
											}
										}
										else if (strcmp(cmp->eqptype, STANK_TYPE) == 0)
										{
											if (stv != NULL)
											{
												Plist->batch = 'y';
												stank = (STANK *)cmp->eqp;
												for (i = 0; i < stank->Nin; i++)
												{
													if (stank->pthcon[i] == co)
													{
														if ((iswc = idscw(stv, Schdl->Scw, NULL)) >= 0)
															stank->batchcon[i] = Schdl->isw + iswc;
													}
												}
											}
										}
										/*********************
										else if (strcmp(cmp->eqptype, THEX_TYPE) == 0)
										{
										Thex = (THEX *) cmp->eqp ;
										s
										}
										************************/

										if ( strcmp ( cmp->eqptype, VALV_TYPE ) != 0
											&& strcmp ( cmp->eqptype, TVALV_TYPE ) != 0
											&& strcmp ( cmp->eqptype, QMEAS_TYPE ) != 0
											&& strcmp ( cmp->eqptype, OMVAV_TYPE ) != 0
											)
										{
											(*Npelm)++;
											Pelm->out = NULL ;
											Pelm->cmp = cmp;
											Pelm->ci = ci;
											Pelm->co = co;
											Pelmpre = Pelm ;
											//id++ ;
											Pelm++;
										}
										if ( strcmp ( cmp->eqptype, VALV_TYPE ) != 0
											&& strcmp ( cmp->eqptype, TVALV_TYPE ) != 0
											&& strcmp ( cmp->eqptype, QMEAS_TYPE ) != 0
											&& strcmp ( cmp->eqptype, DIVERG_TYPE ) != 0
											&& strcmp ( cmp->eqptype, CONVRG_TYPE ) != 0
											&& strcmp ( cmp->eqptype, DIVGAIR_TYPE ) != 0
											&& strcmp ( cmp->eqptype, CVRGAIR_TYPE ) != 0
											&& strcmp ( cmp->eqptype, OMVAV_TYPE ) != 0
											)
										{
											id++ ;
										}
										break;
									}
								}

								Errprint(err, errkey, elm);

								if ( DEBUG )
								{
									printf("<<Pathdata>> Mp=%s  elm=%s Npelm=%d\n",
										Mpath->name, elm, *Npelm);
								}
							}
						}
					}

					Plist->Nelm = (int)(Pelm - Plist->pelm) ;
					Plist->Mpath = Mpath ;
					Pelmpre = NULL ;
					id = 0 ;
					Plist++;
					iPlist++ ;
				}
				else
					Errprint(1, errkey, s);
			}
			Mpath->Nlpath = (int)(Plist - Mpath->plist);

			if ( DEBUG )
			{
				printf("<<Pathdata>>  Mpath=%Ild fliud=%c\n", Mpath - mpi, Mpath->fluid);	
			}

			if (Mpath->fluid == AIR_FLD)
			{
				if ( DEBUG )
				{
					printf("<<Pathdata  a>> Mp=%s  Npelm=%d\n",
						Mpath->name, *Npelm);
				}

				Mpath++;

				if ( *Nplst > 0 )
				{
					if (( Mpath->pl = ( PLIST ** ) malloc ( *Nplst * sizeof ( PLIST * ))) == NULL )
						Ercalloc ( *Nplst, "<Pathdata> Mpath->pl  alloc" ) ;

					for ( k = 0; k < *Nplst; k++ )
						Mpath->pl[k] = NULL ;
				}

				if ( *Nplst > 0 )
				{
					if (( Mpath->cbcmp = ( COMPNT ** ) malloc (( *Nplst * 2 ) * sizeof ( COMPNT * ))) == NULL )
						Ercalloc ( *Nplst * 2, "<Pathdata> Mpath->cbcmp  alloc" ) ;

					for ( k = 0; k < *Nplst * 2; k++)
						Mpath->cbcmp[k] = NULL ;
				}

				Np = *Npelm;

				// 空気系統用の絶対湿度経路へのコピー
				plistcpy(Mpath, Npelm, Pelm, Plist, Ncompnt, Compnt);

				Pelm += *Npelm - Np;
				Plist += Mpath->Nlpath;
			}
			Mpath++;
			Nplst++ ;
		}
	}
	*Nmpath = (int)(Mpath - mpi);
	//Eo = Eqsys->Hcload->cmp->elouts + 1 ;

	if ( DEBUG )
	{
		if ( *Nmpath > 0 )
		{
			mpi = *M ;
			plistprint(*Nmpath, mpi, mpi->plist->pelm, Compnt->elouts, Compnt->elins);
		}

		printf ( "SYSPTH  Data Read end\n" ) ;
		printf ( "Nmpath=%d\n", *Nmpath ) ;
	}

	//printf ( "<Pathdata> Pelm=%d\n", *Npelm ) ;
	/* ============================================================================ */

	Mpath = mpi;
	for (i = 0; i < *Nmpath; i++, Mpath++)
	{
		if ( DEBUG )
		{
			printf ( "1----- MAX=%d  i=%d\n", *Nmpath, i ) ;
		}

		ncv = 0;
		Plist = Mpath->plist;

		for (j = 0; j < Mpath->Nlpath; j++, Plist++)
		{
			//Eo = Eqsys->Hcload->cmp->elouts + 1 ;
			if ( DEBUG )
			{
				printf ( "eepath.c  Mpath->Nlpath=%d\n", Mpath->Nlpath ) ;
				printf("<<Pathdata>>  i=%d Mpath=%Ild  j=%d Plist=%Ild\n", i, Mpath - mpi, 
					j, Plist - Mpath->plist); 
			}

			Pelm = Plist->pelm;
			for (m = 0; m < Plist->Nelm; m++, Pelm++)
			{
				if ( DEBUG )
				{
					printf("<<Pathdata>>  m=%d  pelm=%Ild  %s\n", m, Pelm - Plist->pelm,
						Pelm->cmp->name);

					printf ( "MAX=%d  m=%d\n", Plist->Nelm, m ) ;
				}

				idci = idco = 1;
				etyp = Pelm->cmp->eqptype;

				if (m == 0 && strcmp(etyp, FLIN_TYPE) == 0)
					idci = 0;	  

				if (m == 0 && (strcmp(etyp, CONVRG_TYPE) == 0
					|| strcmp(etyp, DIVERG_TYPE) == 0))
					idci = 0;	  
				else if (m == 0 && (strcmp(etyp, CVRGAIR_TYPE) == 0
					|| strcmp(etyp, DIVGAIR_TYPE) == 0))
					idci = 0;

				if (m == Plist->Nelm - 1 && (strcmp(etyp, CONVRG_TYPE) == 0 
					|| strcmp(etyp, DIVERG_TYPE) == 0))
					idco = 0;
				else if (m == Plist->Nelm - 1 && (strcmp(etyp, CVRGAIR_TYPE) == 0 
					|| strcmp(etyp, DIVGAIR_TYPE) == 0))
					idco = 0;

				if (idci == 1)
				{
					pelmci(Mpath->fluid, Pelm, errkey);
					Pelm->in->lpath = Plist;
				}	  
				if (idco == 1)
				{
					pelmco(Mpath->fluid, Pelm, errkey);

					Pelm->out->lpath = Plist;
					Pelm->out->fluid = Mpath->fluid;

				}
			}
		}
		//Eo = Eqsys->Hcload->cmp->elouts + 1 ;

		if ( DEBUG )
		{
			plistprint(1, Mpath, Mpath->plist->pelm, Compnt->elouts, Compnt->elins); 

			printf ( "i=%d\n", i ) ;
		}

		Plist = Mpath->plist;
		if (Mpath->Nlpath == 1)
		{       
			Pelm = Plist->pelm;

			if ( DEBUG )
			{
				printf("<<Pathdata>>   Plist->type=%c\n", Plist->type);
			}


			if (Plist->type == IN_LPTP)
			{
				Mpath->type = THR_PTYP;

				if ( DEBUG )
				{
					printf("<<Pathdata>>   Mpath->type=%c\n",Mpath->type);
				}

			}
			else
			{
				Mpath->type = Plist->type = CIR_PTYP;
				Pelm->in->upo = (Pelm + Plist->Nelm -1)->out;	  
			}

			if ( DEBUG )
			{
				printf("<<Pathdata>> test end\n"); 
			}

			Pelm++;
			for (m = 1; m < Plist->Nelm; m++, Pelm++)
			{
				//printf("m=%d \n", m);
				//if((Pelm-1)->out != NULL)
					//printf("name=%s\n", (Pelm-1)->out->cmp->name) ;
				Pelm->in->upo = (Pelm - 1)->out;
			}
		}
		else
		{
			Mpath->type = BRC_PTYP;

			if ( DEBUG )
			{
				printf("<<Pathdata>> Mpath i=%d  type=%c\n", i, Mpath->type);
			}

			for (j = 0; j < Mpath->Nlpath; j++, Plist++)
			{

				Pelm = Plist->pelm;
				etyp = Pelm->cmp->eqptype;

				if ( DEBUG )
				{
					printf("<<Pathdata>> Plist j=%d name=%s eqptype=%s\n", j,
						Pelm->cmp->name, etyp);
				}

				if (strcmp(etyp, DIVERG_TYPE) == 0 ||
					strcmp(etyp, DIVGAIR_TYPE) == 0)
					Plist->type = DIVERG_LPTP;

				if (strcmp(etyp, CONVRG_TYPE) == 0 ||
					strcmp(etyp, CVRGAIR_TYPE) == 0)
				{
					Plist->type = CONVRG_LPTP ;
					ncv++;
				}

				Pelm += Plist->Nelm - 1;
				etyp = Pelm->cmp->eqptype;
				if (strcmp(etyp, DIVERG_TYPE) != 0 && strcmp(etyp, CONVRG_TYPE) != 0 &&
					strcmp(etyp, DIVGAIR_TYPE) != 0 && strcmp(etyp, CVRGAIR_TYPE) != 0)
					Plist->type = OUT_LPTP;

				Pelm = (Plist->pelm) + 1;
				for (m = 1; m < Plist->Nelm; m++, Pelm++)
					Pelm->in->upo = (Pelm - 1)->out;

				if ( DEBUG )
				{
					printf("<<Pathdata>> Plist MAX=%d  j=%d  type=%c\n", Mpath->Nlpath, j, Plist->type);
				}
			}
		}
		Mpath->Ncv = ncv;

		if ( DEBUG )
		{
			printf ( "2----- MAX=%d  i=%d\n", *Nmpath, i ) ;
		}
	}
	Mpath = mpi;
	//Eo = Eqsys->Hcload->cmp->elouts + 1 ;
	//Eli = Eo->elins ;

	//   plevel(*Nmpath, Mpath, Eqsys->Ncnvrg, Eqsys->Cnvrg); 

	if ( DEBUG )
	{
		if ( *Nmpath > 0 )
		{
			mpi = *M ;
			plistprint(*Nmpath, mpi, mpi->plist->pelm, Compnt->elouts, Compnt->elins);
		}
	}

	// バルブがモニターするPlistの検索
	Valvinit(Eqsys->Nvalv, Eqsys->Valv, *Nmpath, *M ) ;
	
	// 未知流量等の構造解析
	pflowstrct ( *Nmpath, Mpath ) ;
	//Eo = Eqsys->Hcload->cmp->elouts + 1 ;

	//Eo = Eqsys->Desi->cmp->elouts + 1 ;

	if ( DEBUG )
	{
		if ( *Nmpath > 0 )
		{
			mpi = *M ;
			plistprint(*Nmpath, mpi, mpi->plist->pelm, Compnt->elouts, Compnt->elins);
		}
	}

	if ( DEBUG )
	{
		printf ( "\n" ) ;
		C = Compnt ;
		for ( i = 0; i < Ncompnt; i++, C++ )
			printf ( "name=%s Nin=%d  Nout=%d\n", C->name, C->Nin, C->Nout ) ;
	}
}

/***********************************************************************/

int		Mpathcount ( FILE *fi, int *Pl )
{
	int		N = 0 ;
	long	ad ;
	char	s[SCHAR] ;

	ad = ftell ( fi ) ;
	*Pl = 0 ;

	while ( fscanf_s ( fi, "%s", s, sizeof(s) ) != EOF && *s != '*' )
	{
		if ( strcmp ( s, ";" ) == 0 )
			N++ ;
		if ( strcmp ( s, ">" ) == 0 )
			(*Pl)++ ;
	}

	*Pl /= 2 ;

	fseek ( fi, ad, SEEK_SET ) ;

	return ( N ) ;
}

/***********************************************************************/

void	Plcount ( FILE *fi, int *N )
{
	char	s[SCHAR] ;
	int		i, *M ;
	long	ad ;

	i = 0 ;
	M = N ;
	ad = ftell ( fi ) ;

	while ( fscanf_s ( fi, "%s", s, sizeof(s) ) != EOF && *s != '*' )
	{
		if ( strcmp ( s, ";" ) == 0 )
		{
			*N = i ;
			N++ ;
			i = 0 ;
		}

		if ( strcmp ( s, ">" ) == 0 )
		{
			i++ ;
			fscanf_s ( fi, "%*[^>] %*c" ) ;
		}
	}

	/***************************************
	for ( i = 0; i < 61; i++, M++ )
	printf ( "i=%d  pl=%d\n", i, *M ) ;
	****************************************/

	fseek ( fi, ad, SEEK_SET ) ;
}

/***********************************************************************/

int		Pelmcount ( FILE *fi )
{
	int		i, N ;
	long	ad ;
	char	s[SCHAR], t[SCHAR] ;

	ad = ftell ( fi ) ;
	i = 1 ;
	N = 0 ;

	while ( fscanf_s ( fi, "%s", s, sizeof(s) ) != EOF )
	{
		i = 1 ;
		if ( strcmp ( s, "*" ) == 0 )
			break ;

		while ( fscanf_s ( fi, "%s", s, sizeof(s) ) != EOF )
		{
			if ( *s == ';' )
				break ;

			if ( strcmp ( s, "-f" ) == 0 )
			{
				fscanf_s ( fi, "%s", t, sizeof(t) ) ;

				if ( strcmp ( t, "W" ) == 0 || strcmp ( t, "a" ) == 0 )
					i = 1 ;
				else
					i = 2 ;
			}
			if ( strcmp ( s, "-sys" ) == 0 )
				fscanf_s ( fi, "%*s" ) ;

			if ( strcmp ( s, ">" ) != 0 && *s != '(' && *s != '-' && *s != ';' )
			{
				//printf ( "Pelmcount s=%s\n", s ) ;
				N += i ;
			}
		}
	}

	fseek ( fi, ad, SEEK_SET ) ;
	return ( N ) ;
}

/***********************************************************************/

void	Elcount ( int N, COMPNT *C, int *Nelout, int *Nelin )
{
	int		i ;
	char	*e ;

	*Nelout = *Nelin = 0 ;

	for ( i = 0; i < N; i++, C++ )
	{
		e = C->eqptype ;
		//		printf ( "[%3d] Nin=%d  Nout=%d  Name=%s\n",
		//			i, C->Nin, C->Nout, C->name ) ;
		*Nelout += C->Nout ;
		*Nelin += C->Nin ;

		if ( strcmp ( e, HCLOADW_TYPE ) == 0 )
			*Nelin += 8 ;
		else if ( strcmp ( e, THEX_TYPE ) == 0 )
		{
			*Nelout += 4 ;
			*Nelin += 14 ;
		}
	}
	*Nelout *= 4 ;
	*Nelin *= 4 ;
}
