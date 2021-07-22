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

/*   rmprint.c   */

#include <stdio.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"


/* ---------------------------------------------------------------- */
/* 室内表面温度の出力 */

void Rmsfprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd)
{
	static int ic=0, N;
	int		i, n, nn;
	ROOM	*Rm ;
	RMSRF	*S ;
	
	if (ic == 0)
	{ 
		ic++ ;
		N = Room->end ;
		
		Rm = Room ;
		for ( n = 0, i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->sfpri == 'p' )
				n++ ;
		}

		fprintf ( fo, "%s;\n %d\n", title, n ) ;
		fprintf ( fo, "Mo\tNd\ttime\t" ) ;
		
		Rm = Room ;
		for ( i = 0; i < N; i++, Rm++ )
		{ 
			if ( Rm->sfpri == 'p' )
			{
				fprintf ( fo, "%s\t", Rm->name ) ;
				
				S = Sd + Rm->brs ;
				for ( n = Rm->brs, nn = 0; nn < Rm->N; nn++, n++, S++ )
				{
					if ( S->name == NULL )
						fprintf ( fo, "%d-%c_Ts\t", n - Rm->brs, S->ble ) ;
					else
						fprintf ( fo, "%s_Ts\t", S->name ) ;
				}
			}
		}
		fprintf ( fo, "\n" ) ;
	}
	fprintf ( fo, "%d\t%d\t%.2lf\t", Mon, Day, time ) ;
	
	Rm = Room ;
	for ( i = 0; i < N; i++, Rm++ )
	{
		if ( Rm->sfpri == 'p' )
		{ 
			fprintf ( fo, "\t" ) ;
			
			S = Sd + Rm->brs ;
			for ( n = Rm->brs, nn = 0; nn < Rm->N; nn++, n++, S++ )
				fprintf(fo, "%.1lf\t", S->Ts );
		}
	}
	fprintf(fo, "\n");
}

/* ---------------------------------------------------------------- */
/* 室内表面熱流の出力 */

void Rmsfqprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd)
{
	static int ic=0, N;
	int		i, n, nn;
	ROOM	*Rm ;
	RMSRF	*S ;
	
	if (ic == 0)
	{ 
		ic++ ;
		N = Room->end ;
		
		Rm = Room ;
		for ( n = 0, i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->sfpri == 'p' )
				n++ ;
		}

		fprintf ( fo, "%s;\n %d\n", title, n ) ;
		fprintf ( fo, "Mo\tNd\ttime\t" ) ;
		
		Rm = Room ;
		for ( i = 0; i < N; i++, Rm++ )
		{ 
			if ( Rm->sfpri == 'p' )
			{
				fprintf ( fo, "%s\t", Rm->name ) ;
				
				S = Sd + Rm->brs ;
				for ( n = Rm->brs, nn = 0; nn < Rm->N; nn++, n++, S++ )
				{
					nn = n - Rm->brs;
					if ( S->name == NULL )
						fprintf ( fo, "%d-%c_Qc\t%d-%c_Qr\t%d-%c_RS\t%d-%c_Qi\t%d-%c_RSsol\t%d-%c_RSli\t%d-%c_tsol\t%d-%c_asol\t%d-%c_rn\t",
						nn, S->ble, nn, S->ble, nn, S->ble,
						nn, S->ble, nn, S->ble, nn, S->ble, nn, S->ble, nn, S->ble, nn, S->ble);
					else
						fprintf ( fo, "%s_Qc\t%s_Qr\t%s_RS\t%s_Qi\t%s_RSsol\t%s_RSli\t%s_tsol\t%s_asol\t%s_rn\t",
						S->name, S->name, S->name, S->name, S->name, S->name, S->name, S->name, S->name);
				}
			}
		}
		fprintf ( fo, "\n" ) ;
	}
	fprintf ( fo, "%d\t%d\t%.2lf\t", Mon, Day, time ) ;
	
	Rm = Room ;
	for ( i = 0; i < N; i++, Rm++ )
	{
		if ( Rm->sfpri == 'p' )
		{ 
			fprintf ( fo, "\t" ) ;
			
			S = Sd + Rm->brs ;

			// 2003/9/10　表面熱取得を負とするために短波長成分RSの符号を変更した。
			for (n = Rm->brs, nn = 0; nn < Rm->N; nn++, n++, S++)
				fprintf(fo, "%.4e\t%.4e\t%.4e\t%.4e\t%.4e\t%.4e\t%.4e\t%.4e\t%.4e\t", S->Qc, S->Qr,
					-S->RS * S->A, S->Qi, -S->RSsol * S->A, -S->RSli * S->A, S->Qgt, S->Qga, S->Qrn);
		}
	}
	fprintf(fo, "\n");
}

/* ---------------------------------------------------------------- */
/* 室内表面熱伝達率の出力 */

void Rmsfaprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd)
{
	static int ic=0, N;
	int		i, n, nn;
	ROOM	*Rm ;
	RMSRF	*S ;
	
	if (ic == 0)
	{ 
		ic++ ;
		N = Room->end ;
		
		Rm = Room ;
		for ( n = 0, i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->sfpri == 'p' )
				n++ ;
		}

		fprintf ( fo, "%s;\n %d\n", title, n ) ;
		fprintf ( fo, "Mo\tNd\ttime\t" ) ;
		
		Rm = Room ;
		for ( i = 0; i < N; i++, Rm++ )
		{ 
			if ( Rm->sfpri == 'p' )
			{
				fprintf ( fo, "%s\t", Rm->name ) ;
				
				S = Sd + Rm->brs ;
				for ( n = Rm->brs, nn = 0; nn < Rm->N; nn++, n++, S++ )
				{
					if ( S->name == NULL )
						fprintf ( fo, "%d-%c_K\t%d-%c_alc\t%d-%c_alr\t",
						n - Rm->brs, S->ble, n - Rm->brs, S->ble, n - Rm->brs, S->ble ) ;
					else
						fprintf ( fo, "%s_K\t%s_alc\t%s_alr\t",
						S->name, S->name, S->name ) ;
				}
			}
		}
		fprintf ( fo, "\n" ) ;
	}
	fprintf ( fo, "%d\t%d\t%.2lf\t", Mon, Day, time ) ;
	
	Rm = Room ;
	for ( i = 0; i < N; i++, Rm++ )
	{
		if ( Rm->sfpri == 'p' )
		{ 
			fprintf ( fo, "\t" ) ;
			
			S = Sd + Rm->brs ;
			for ( n = Rm->brs, nn = 0; nn < Rm->N; nn++, n++, S++ )
				fprintf(fo, "%.3g\t%.3g\t%.3g\t", S->K, S->alic, S->alir );
		}
	}
	fprintf(fo, "\n");
}

/* 日積算壁体貫流熱取得の出力 */
void	Dysfprint(FILE *fo, char *title, int Mon, int Day, ROOM *Room )
{
	RMSRF	*Sd ;
	int		i, n ;
	static int	ic = 0, N ;
	ROOM	*Rm ;

	if ( ic == 0 )
	{
		ic++ ;
		N = Room->end ;
		Rm = Room ;
		for ( n = 0, i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->sfpri == 'p' )
				n++ ;
		}

		fprintf ( fo, "%s;\n %d\n", title, n ) ;
		fprintf ( fo, "Mo\tNd\t") ;

		Rm = Room ;
		for ( i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->sfpri == 'p' )
			{
				fprintf ( fo, "%s\t", Rm->name ) ;

				Sd = Rm->rsrf ;
				for ( n = 0; n < Rm->N; n++, Sd++)
				{
					if ( Sd->name == NULL )
						fprintf ( fo, "%d-%c_Ts\t%d-%c_Tsmax\t%d-%c_Tsmin\t%d-%c_Qih\t%d-%c_Qic\t",
							n, Sd->ble, n, Sd->ble, n, Sd->ble, n, Sd->ble, n, Sd->ble) ;
					else
						fprintf ( fo, "%s_Ts\t%s_Tsmax\t%s_Tsmin\t%s_Qih\t%s_Qic\t",
							Sd->name, Sd->name, Sd->name, Sd->name, Sd->name) ;
				}
			}
		}
		fprintf ( fo, "\n" ) ;
	}

	fprintf ( fo, "%d\t%d\t", Mon, Day ) ;

	Rm = Room ;
	for ( i = 0; i < N; i++, Rm++ )
	{
		if ( Rm->sfpri == 'p' )
		{
			fprintf ( fo, "\t" ) ;

			Sd = Rm->rsrf ;
			for ( n = 0; n < Rm->N; n++, Sd++ )
			{
				fprintf ( fo, "%.2lf\t%.2lf\t%.2lf\t%.3g\t%.3g\t"
					, Sd->Tsdy.m, Sd->Tsdy.mx, Sd->Tsdy.mn, Sd->SQi.h, Sd->SQi.c ) ;
			}
		}
	}

	fprintf (fo, "\n" ) ;
}

/* ---------------------------------------------------------------- */

/* 日よけの影面積の出力 */

void	Shdprint(FILE *fo, char *title, int Mon, int Day, double time,
	int Nsrf, RMSRF *Sd)
{
	RMSRF	*Sdd;
	static int ic = 0;
	int		i, m;

	if (ic == 0)
	{
		ic++;
		Sdd = Sd;
		for (m = 0, i = 0; i<Nsrf; i++, Sdd++)
		{
			if (Sdd->shdpri == 'p' && Sdd->sb >= 0)
				m++;
		}

		fprintf(fo, "%s;\n %d\n", title, m);

		Sdd = Sd;
		for (i = 0; i < Nsrf; i++, Sdd++)
		{
			if (Sdd->shdpri == 'p' && Sdd->sb >= 0)
			{
				fprintf(fo, "%s\t%d:%s", Sdd->room->name,
					i - Sdd->room->brs, Sdd->name);
				fprintf(fo, "\n");
			}
		}
	}

	fprintf(fo, "%d\t%d\t%.2lf\t", Mon, Day, time);

	Sdd = Sd;
	for (i = 0; i < Nsrf; i++, Sdd++)
	{
		if (Sdd->shdpri == 'p' && Sdd->sb >= 0)
			fprintf(fo, "%.2lf\t", Sdd->Fsdworg);
	}

	fprintf(fo, "\n");
}

/* ---------------------------------------------------------------- */

/* 壁体内部温度の出力 */

void	Wallprint(FILE *fo, char *title, int Mon, int Day, double time, 
				  int Nsrf, RMSRF *Sd)             
{
	RMSRF	*Sdd;
	MWALL	*Mw;
	static int ic = 0 ;
	int		i, m ;
	double	*Tw ;
	
	if ( ic == 0 )
	{
		ic++ ;
		Sdd = Sd;
		for (m=0, i=0; i<Nsrf; i++, Sdd++)
		{
			if (Sdd->wlpri == 'p' && Sdd->wd >= 0)
				m++;
		}

		fprintf(fo, "%s;\n %d\n", title, m);
		
		Sdd = Sd;
		for (i = 0; i < Nsrf; i++, Sdd++)
		{
			if (Sdd->wlpri == 'p' && Sdd->wd >= 0)
			{
				fprintf(fo, "%s\t%d-%c:%s\t%d", Sdd->room->name, 
					i - Sdd->room->brs, Sdd->ble, Sdd->name, Sdd->mw->M);
				fprintf(fo, "\n");	 
			}
		}    
	}
	
	fprintf(fo, "%d\t%d\t%.2lf\t", Mon, Day, time);
	
	Sdd = Sd;
	for (i = 0; i < Nsrf; i++, Sdd++)
	{
		if (Sdd->wlpri == 'p' && Sdd->wd >= 0)
		{ 
			Mw=Sdd->mw;
			
			/*
			printf("------ Wallprint i=%d mwside=%c\n", i, Sdd->mwside);
			*/
			
			// 室内が壁体０側の場合
			if (Sdd->mwside == 'i')	   
			{
			/*
			printf("------ Wallprint i=%d mwside=%c\n", i, Sdd->mwside);
				*/
				
				Tw = Mw->Tw ;
				for (m = 0; m < Mw->M; m++, Tw++)
					fprintf(fo, "\t%.2lf", *Tw);
			}
			// 室内が壁体M側の場合
			else
			{
				for (m = Mw->M - 1; m >= 0; m--)
					fprintf(fo, "\t%.2lf", Mw->Tw[m]);
			}
			
			fprintf(fo, "\t");
		}
    }

	fprintf ( fo, "\n" ) ;
}

/* ---------------------------------------------------------------- */

/* 潜熱蓄熱材の状態値の出力 */

void	PCMprint(FILE *fo, char *title, int Mon, int Day, double time,
	int Nsrf, RMSRF *Sd)
{
	RMSRF	*Sdd;
	MWALL	*Mw;
	static int ic = 0;
	int		i, m, j;
	//double	*Tw;
	PCMSTATE	*pcmstate;

	if (ic == 0)
	{
		ic++;
		Sdd = Sd;
		for (m = 0, i = 0; i<Nsrf; i++, Sdd++)
		{
			if (Sdd->pcmpri == 'y' && Sdd->wd >= 0)
				m += Sdd->Npcm;
		}

		fprintf(fo, "%s;\n %d\n", title, m);

		Sdd = Sd;
		j = 0;
		for (i = 0; i < Nsrf; i++, Sdd++)
		{
			if (Sdd->mwside == 'i')
			{
				if (Sdd->pcmpri == 'y' && Sdd->wd >= 0)
				{
					pcmstate = Sdd->pcmstate;
					for (m = 0; m < Sdd->mw->M; m++, pcmstate++)
					{
						if (pcmstate != NULL && pcmstate->name != NULL)
						{
							//printf("[%d] name=%d-%c:%s\n", j, i - Sdd->room->brs, Sdd->ble, Sdd->name);
							//j++;
							fprintf(fo, "%s\t%d-%c:%s\t%s\tTpcm\tcp\tLamda", Sdd->room->name,
								i - Sdd->room->brs, Sdd->ble, Sdd->name, pcmstate->name);
							fprintf(fo, "\n");
						}
					}
				}
			}
		}
	}

	fprintf(fo, "%d\t%d\t%.2lf\t", Mon, Day, time);

	Sdd = Sd;
	m = 0;
	for (i = 0; i < Nsrf; i++, Sdd++)
	{
		if (Sdd->pcmpri == 'y' && Sdd->wd >= 0)
		{
			WALL	*Wall;
			Mw = Sdd->mw;
			Wall = Mw->wall;

			/*
			printf("------ Wallprint i=%d mwside=%c\n", i, Sdd->mwside);
			*/

			// 室内が壁体０側の場合
			if (Sdd->mwside == 'i')
			{
				/*
				printf("------ Wallprint i=%d mwside=%c\n", i, Sdd->mwside);
				*/

				pcmstate = Sdd->pcmstate;
				for (j = 0; j < Mw->M; j++, pcmstate++)
				{
					PCM		*PCM, *PCM1;
					// PCM構造体
					PCM = Wall->PCMLyr[j];
					PCM1 = Wall->PCMLyr[j + 1];
					if (pcmstate->name != NULL)
					{
						//if (m == 11)
						//	printf("sssss\n");
						//printf("-[%d] name=%s\n", m, Sdd->name);
						//m++;
						fprintf(fo, "\t%.3lf\t%.3lf\t%.3lf\t%.0lf\t%.0lf\t%.4g\t%.4g", pcmstate->tempPCMNodeL, pcmstate->tempPCMNodeR, pcmstate->tempPCMave,
							pcmstate->capmL, pcmstate->capmR, pcmstate->lamdaL, pcmstate->lamdaR);
					}
				}
			}
			// 室内が壁体M側の場合
			//else
			//{

			//	for (m = 0; m < Sdd->Npcm; m++)
			//	{
			//		pcmstate = Sdd->pcmstate + Mw->M;
			//		for (j = Mw->M - 1; j >= 0; j--, pcmstate--)
			//		{
			//			if (pcmstate->name != NULL)
			//				fprintf(fo, "\t%.1lf\t%.0lf\t%.4g", pcmstate->tempPCM, pcmstate->capm, pcmstate->lamda);
			//		}
			//	}
			//}

			fprintf(fo, "\t");
		}
	}

	fprintf(fo, "\n");
}

/* ---------------------------------------------------------------- */

/* 日射、室内熱取得の出力 */

void	Qrmprint (FILE *fo, char *title, int Mon, int Day, double time,
				  ROOM *Room, QRM *Qrm)
{
	static int ic=0, N;
	static char	key[][6] = { "tsol", "asol", "arn", "hums", "light", "apls",
		"huml", "apll", "Qeqp", "Qfun", "Qis", "Qil", "Qsto", "Qstol", "AE", "AG" } ;
	int		i, n;
	ROOM	*Rm ;
	
	if ( ic == 0 )
	{
		ic++ ;
		N = Room->end ;
		Rm = Room ;
		for ( n = 0, i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->eqpri == 'p')
				n++ ;
		}
		fprintf ( fo, "%s;\n %d\n", title, n ) ;
		fprintf ( fo, "Mo\tNd\ttt\t" ) ;
		
		Rm = Room ;
		for ( i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->eqpri == 'p' )
			{
				for ( n = 0; n < 16; n++ )
					fprintf ( fo, "%s_%s\t", Rm->name , key[n] ) ;
			}
		}
		fprintf ( fo, "\n" ) ;
	}
	
	fprintf ( fo, "%d\t%d\t%.2lf\t", Mon, Day, time ) ;
	
	Rm = Room ;
	for ( i = 0; i < N; i++, Rm++, Qrm++ )
	{
		if ( Rm->eqpri == 'p' )
		{
			fprintf ( fo, "%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t",
			Qrm->tsol, Qrm->asol, Qrm->arn, Qrm->hums,
			Qrm->light, Qrm->apls, Qrm->huml, Qrm->apll, Qrm->Qeqp, Rm->QM,
			Qrm->Qinfs, Qrm->Qinfl ) ;
			fprintf ( fo, "%.5g\t%.5g\t",
			Qrm->Qsto, Qrm->Qstol ) ;
			// 機器（電力）、機器（ガス）のエネルギー消費量
			fprintf ( fo, "%.5g\t%.5g\t", Qrm->AE, Qrm->AG ) ;
		}
	}
	
	fprintf(fo, "\n");
}

/* ---------------------------------------------------------------- */

/* 日射、室内熱取得の出力 */

void	Dyqrmprint (FILE *fo, char *title, int Mon, int Day, 
					ROOM *Room, double *Trdav, QRM *Qrmd)
{
	static int ic=0, N;
	static char	key[][6] = { "Tr", "tsol", "asol", "arn", "hums", "light", "apls",
		"huml", "apll", "Qeqp", "Qis", "Qil", "Qsto", "Qstol", "AE", "AG" } ;
	int		i, n;
	ROOM	*Rm ;
	QRM		*Q ;
	
	if ( ic == 0 )
	{
		ic++ ;
		N = Room->end ;
		Rm = Room ;
		for ( n = 0, i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->eqpri == 'p' )
				n++ ;
		}
		fprintf ( fo, "%s;\n %d\n", title, n ) ;
		fprintf ( fo, "Mo\tNd\t" ) ;
		
		Rm = Room ;
		for ( i = 0; i < N; i++, Rm++ )
		{
			if ( Rm->eqpri == 'p' )
			{
				for ( n = 0; n < 16; n++ )
					fprintf ( fo, "%s_%s\t", Rm->name , key[n] ) ;
			}
		}
		fprintf ( fo, "\n" ) ;
	}
	
	fprintf ( fo, "%d\t%d\t", Mon, Day ) ;
	
	Rm = Room ;
	Q = Qrmd ;
	for ( i = 0; i < N; i++, Rm++, Q++, Trdav++ )
	{
		if ( Rm->eqpri == 'p' )
			fprintf(fo, 
			"%.1lf\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t%.4g\t",
			*Trdav, Q->tsol, Q->asol, Q->arn,
			Q->hums, Q->light, Q->apls, Q->huml, Q->apll,
			Q->Qeqp, Q->Qinfs, Q->Qinfl, Q->Qsto, Q->Qstol, Q->AE, Q->AG );
	}
	
	fprintf ( fo, "\n" ) ;
}
/* ---------------------------------------------------------------- */

void Qrmsum (int Day, int Nroom, ROOM *Room, QRM *Qrm, 
			 double *Trdav, QRM *Qrmd)
{
	static int  oldday = 0 ;
	extern double dTM ;
	int		i ;
	QRM		*Q, *Qr ;
	double	*T ;
	
	if (Day != oldday)
	{
		Q = Qrmd ;
		T = Trdav ;
		for ( i = 0; i < Nroom; i++, Q++, T++ )
		{
			*T = 0.0 ;
			Q->tsol = Q->asol = Q->arn = 0.0 ;
			Q->hums = Q->light = Q->apls = 0.0 ;
			Q->huml = Q->apll = 0.0 ;
			Q->Qeqp = Q->Qinfl = Q->Qinfs = 0.0 ;
			Q->Qsto = Q->Qstol = 0.0 ;
			Q->AE = 0.0 ;
			Q->AG = 0.0 ;
		} 
		oldday = Day;
	}
	
	Q = Qrmd ;
	Qr = Qrm ;
	T = Trdav ;
	for ( i = 0; i < Nroom; i++, Q++, Qr++, T++, Room++ )
	{
		*T += Room->Tr * dTM / ( 24.0 * 3600. ) ;
		Q->tsol += ( Qr->tsol * dTM / 3600.0 ) ;
		Q->asol += ( Qr->asol * dTM / 3600.0 ) ;
		Q->arn += ( Qr->arn * dTM / 3600.0 ) ;
		Q->hums += ( Qr->hums * dTM / 3600.0 ) ;
		Q->light += ( Qr->light * dTM / 3600.0 ) ;
		Q->apls += ( Qr->apls * dTM / 3600.0 ) ;
		Q->huml += ( Qr->huml * dTM / 3600.0 ) ;
		Q->apll += ( Qr->apll * dTM / 3600.0 ) ;
		Q->Qinfs += ( Qr->Qinfs * dTM / 3600.0 ) ;
		Q->Qinfl += ( Qr->Qinfl * dTM / 3600.0 ) ;
		Q->Qeqp += ( Qr->Qeqp * dTM / 3600.0 ) ;
		Q->Qsto += ( Qr->Qsto * dTM / 3600.0 ) ;
		Q->Qstol += ( Qr->Qstol * dTM / 3600.0 ) ;

		Q->AE += ( Qr->AE * dTM / 3600.0 ) ;
		Q->AG += ( Qr->AG * dTM / 3600.0 ) ;
	}
}
