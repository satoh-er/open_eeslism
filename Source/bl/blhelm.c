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

/* helm.c */


#include <stdlib.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"
#include "fnfio.h"

/* 要素別熱損失・熱取得（記憶域確保） */

void Helminit(char *errkey, char helmkey, int Nroom, ROOM *Room, QETOTAL *Qetotal)
{
	int		i, j, Nmax = 0, N, k ;
	RMSRF	*Sd;
	ROOM	*Rm;
	RMQELM	*Rq ;
	RMSB	*Rs ;
	BHELM	*Bh ;
	
	Rm = Room;
	
	if (helmkey != 'y')
	{
		for (i = 0; i < Nroom; i++, Room++)
			Room->rmqe = NULL;
		return;
	}
	else
	{  
		Room = Rm;
		for (i = 0; i < Nroom; i++, Room++)
		{
			if ((Room->rmqe = (RMQELM *)malloc(1 * sizeof(RMQELM))) == NULL)
				Ercalloc(1, errkey);

			if ( Room->rmqe != NULL )
			{
				Rq = Room->rmqe ;
				Rq->rmsb = NULL ;
				Rq->WSCwk = NULL ;
			}

			N = Room->N ;
			if(N>0 )
			{
				if ((Room->rmqe->rmsb = (RMSB *)malloc( N * sizeof(RMSB))) == NULL)
					Ercalloc(Room->N, errkey);
			}

			if ( Room->rmqe->rmsb != NULL )
			{
				Rs = Room->rmqe->rmsb ;

				for ( k = 0; k < N; k++, Rs++ )
					Rs->Told = Rs->Tw = NULL ;
			}

			Sd = Room->rsrf;
			Rs = Room->rmqe->rmsb ;
			for (j = 0; j < Room->N; j++, Sd++, Rs++ )
			{
				if (Sd->mw != NULL)
				{	       
				/***********printf("Helminit i=%d  Sd=%d M=%d\n",
				i, Sd - Room->rsrf, Sd->mw->M);
					********************************/
					N = Sd->mw->M ;
					if(N > 0 )
					{
						if ((Rs->Tw = (BHELM *)malloc(N * sizeof(BHELM))) == NULL)
							Ercalloc(Sd->mw->M, errkey);
					}

					if(N > 0 )
					{
						if ((Rs->Told = (BHELM *)malloc(N * sizeof(BHELM))) == NULL)
							Ercalloc(Sd->mw->M, errkey);
					}
				}
				else
				{
					Rs->Tw = NULL;
					Rs->Told = NULL;
				}
				
				switch(Sd->ble)
				{
				case 'E': case 'R': case 'F': case 'W':
					if (Sd->typ != 'E' && Sd->typ != 'e')
						Rs->type = 'E';
					else
						Rs->type = 'G';
					break;
				case 'i': case 'f': case 'c': case 'd':
					Rs->type = 'i';
					break;
				}
			}
			if (Room->N > Nmax)
				Nmax = Room->N;    	    	    
		}   
		Room = Rm;
		for (i = 0; i < Nroom; i++, Room++)
		{
			if (i == 0)
			{
				if(Nmax > 0 )
				{
					if ((Room->rmqe->WSCwk = (BHELM *)malloc(Nmax * sizeof(BHELM))) == NULL)
						Ercalloc(Nmax, errkey);
					Bh = Room->rmqe->WSCwk ;
					Bh->trs = Bh->so = Bh->sg = Bh->rn = Bh->in = Bh->pnl = 0.0 ;
				}
			}
			else
				Room->rmqe->WSCwk = Rm->rmqe->WSCwk;
		}
		Qetotal->name = stralloc("Qetotal");      
	}
}
/* ----------------------------------------------------- */

/* 要素別熱損失・熱取得（計算） */

void Helmroom(int Nroom, ROOM *Room, QRM *Qrm, 
			  QETOTAL *Qetotal, double dTM, double Ta, double xa )
{
	ROOM	*Rm ;
	int	i;
	QHELM	*qe ;
	
	qelmclear(&Qetotal->qelm);
	
	Rm = Room ;
	for (i = 0; i < Nroom; i++, Rm++, Qrm++)
	{
		qe = &Rm->rmqe->qelm ;

		helmrmsrt( Rm, Ta ) ;
		helmq( Rm, dTM, Ta, xa ) ;
		qe->slo = Qrm->solo;
		qe->slw = Qrm->solw;
		qe->asl = Qrm->asl;
		qe->tsol = Qrm->tsol;
		qe->hins = Qrm->hgins;
		
		qelmsum(qe, &Qetotal->qelm);
	}
	
	Rm = Room ;
	for (i = 0; i < Nroom; i++, Rm++)
		helmwall(Rm, Ta) ;
}
/* ----------------------------------------------------- */

/* 要素別熱損失・熱取得（時刻別出力） */

void Helmprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
			   int Nroom, ROOM *Room, QETOTAL *Qetotal)
{
	int j;
	static int id = 0;
	
	if (id == 0)
	{      
		ttlprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if (j == 0)
				fprintf(fo, "-cat\n");
			helmrmprint(fo, id, Nroom, Room, Qetotal);
			if (j == 0)
				fprintf(fo, "*\n#\n");
			id++;
		}
	}
	
	fprintf(fo, "%02d %02d %5.2lf\n", mon, day, time);
	helmrmprint(fo, id, Nroom, Room, Qetotal);
}   
/* ----------------------------------------------------- */

void helmrmprint( FILE *fo, int id, int Nroom, ROOM *Room, QETOTAL *Qetotal)
{
	int	  i;
	BHELM  *q;
	QHELM  *qh;
	char   *name;
	
	switch ( id )
	{
	case 0:
		if ( Nroom > 0 )
			fprintf ( fo, "%s %d\n", ROOM_TYPE, Nroom + 1);
		
		for ( i = 0; i < Nroom; i++, Room++ )
		{
			if ( Room->rmqe != NULL )
				fprintf ( fo, "%s 1 %d\n", Room->name, 29) ;
		}
		fprintf ( fo, "%s 1 %d\n", Qetotal->name, 29) ;
		break ;
		
	case 1:
		for ( i = 0; i < Nroom + 1; i++, Room++)
		{
			if (i < Nroom)
				name = Room->name;
			else
				name = Qetotal->name;
			
			fprintf (fo, "%s_qldh q f %s_qldc q f ", name, name);
			fprintf (fo, "%s_slo q f %s_slw q f %s_asl q f %s_tsol q f %s_hins q f\n",
				name, name, name, name, name);
			fprintf (fo, "%s_so q f %s_sw q f %s_rn q f %s_in q f %s_pnl q f\n", 
				name, name, name, name, name);
			fprintf (fo, "%s_trs q f %s_qew q f %s_qwn q f %s_qgd q f %s_qnx q f ",
				name, name, name, name, name);
			fprintf (fo, "%s_qi q f %s_qc q f %s_qf q f\n",
				name, name, name);
			fprintf (fo, "%s_vo q f %s_vr q f %s_sto q f\n",name, name, name);
			fprintf (fo, "%s_qldhl q f %s_qldcl q f %s_hinl q f\n", name, name, name ) ;
			fprintf (fo, "%s_vol q f %s_vrl q f %s_stol q f\n", name, name, name ) ;
		}
		break ;
		
	default:
		for ( i = 0; i < Nroom + 1; i++, Room++ )
		{
			if (i < Nroom)
			{
				q = &(Room->rmqe->qelm.qe);
				qh = &Room->rmqe->qelm;       
				
				fprintf (fo, "%3.0lf %3.0lf ", qh->loadh, qh->loadc);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ", 
					qh->slo, qh->slw, qh->asl, qh->tsol, qh->hins);
				
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ", 
					q->so, q->sg, q->rn, q->in, q->pnl);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ",
					q->trs, qh->ew, qh->wn, qh->gd, qh->nx);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ",
					qh->i, qh->c, qh->f, qh->vo, qh->vr, qh->sto);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf %3.0lf\n",
					qh->loadhl, qh->loadcl, qh->hinl, qh->vol, qh->vrl, qh->stol ) ;
			}
			
			
			else
			{
				q = &Qetotal->qelm.qe;
				qh = &Qetotal->qelm; 
				fprintf (fo, "%3.0lf %3.0lf ", qh->loadh, qh->loadc);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ", 
					qh->slo, qh->slw, qh->asl, qh->tsol, qh->hins);
				
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ", 
					q->so, q->sg, q->rn, q->in, q->pnl);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ",
					q->trs, qh->ew, qh->wn, qh->gd, qh->nx);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf %3.0lf ",
					qh->i, qh->c, qh->f, qh->vo, qh->vr, qh->sto);
				fprintf (fo, "%3.0lf %3.0lf %3.0lf %3.0lf %3.0lf %3.0lf\n",
					qh->loadhl, qh->loadcl, qh->hinl, qh->vol, qh->vrl, qh->stol ) ;
			}	     
		}
		break ;
	}
}
/* ----------------------------------------------------- */

/* 要素別熱損失・熱取得（時刻別出力） */

void Helmsurfprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
				   int Nroom, ROOM *Room)
{
	int j;
	static int id = 0;
	
	if (id == 0)
	{      
		ttlprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if(j == 0)
				fprintf(fo, "-cat\n");
			helmsfprint(fo, id, Nroom, Room);
			if (j == 0)
				fprintf(fo, "*\n#\n");
			id++;
		}
	}
	
	fprintf(fo, "%02d %02d %5.2lf\n", mon, day, time);
	helmsfprint(fo, id, Nroom, Room);
}   
/* ----------------------------------------------------- */

void helmsfprint( FILE *fo, int id, int Nroom, ROOM *Room)
{
	int	  i, j, Nsf ;
	RMSRF  *Sd;
	RMSB   *rmsb;
	BHELM  *Ts;
	char   s[SCHAR];
	
	switch ( id )
	{
	case 0:
		if (Nroom > 0 )
			fprintf ( fo, "%s %d\n", ROOM_TYPE, Nroom ) ;
		
		for ( i = 0; i < Nroom; i++, Room++ )
		{
			Nsf = 0;
			Sd = Room->rsrf;
			for (j = 0; j < Room->N; j++, Sd++)
			{
				if (Sd->sfepri == 'y')
					Nsf++;
			}
			fprintf ( fo, "%s 1 %d\n", Room->name, 6 * Nsf);
			
		}
		break ;
		
	case 1:
		for ( i = 0; i < Nroom; i++, Room++)	    
		{
			Sd = Room->rsrf;
			for (j = 0; j < Room->N; j++, Sd++)
			{
				if (Sd->sfepri == 'y')
				{
					if (strlen(Sd->name) == 0)
						sprintf_s(s, sizeof(s), "%s-%d-%c", Room->name, j, Sd->ble);
					else
						sprintf_s(s, sizeof(s), "%s-%s", Room->name, Sd->name);
					
					fprintf (fo, "%s_trs t f %s_so f %s_sg t f ", s, s, s);
					fprintf (fo, "%s_rn t f %s_in t f %s_pnl t f\n", s, s, s);
				}
			}
		}
		break ;
		
	default:
		for ( i = 0; i < Nroom; i++, Room++ )
		{
			Sd = Room->rsrf;
			rmsb  = Room->rmqe->rmsb;
			for (j = 0; j < Room->N; j++, rmsb++, Sd++)
			{
				if (Sd->sfepri == 'y')
				{
					Ts = &rmsb->Ts;
					fprintf (fo, "%5.2lf %5.2lf %5.2lf ", Ts->trs, Ts->so, Ts->sg) ;
					fprintf (fo, "%5.2lf %5.2lf %5.2lf\n", Ts->rn, Ts->in, Ts->pnl);
				}
			}
			
		}
		break ;
	}
}
/* ----------------------------------------------------- */

/* 要素別熱損失・熱取得（日積算値） */

void Helmdy (int day, int Nroom, ROOM *Room, QETOTAL *Qetotal)
{
	int    i;
	RMQELM *rmq ;
	static int oldday = -1;
	
	if (day != oldday)
	{
		helmdyint(Nroom, Room, Qetotal);
		oldday = day;
	}
	
	for ( i = 0; i < Nroom; i++, Room++)
	{
		rmq = Room->rmqe ;
		
		if ( rmq != NULL )
			qelmsum (&rmq->qelm, &rmq->qelmdy);
	}
	qelmsum(&Qetotal->qelm, &Qetotal->qelmdy);
}

/* ----------------------------------------------------- */

void helmdyint ( int Nroom, ROOM *Room , QETOTAL *Qetotal)
{
	int   i;
	
	for ( i = 0; i < Nroom; i++, Room++ )
	{
		if ( Room->rmqe != NULL )
		{
			qelmclear(&Room->rmqe->qelmdy);
		}      
	}
	qelmclear(&Qetotal->qelmdy);
	
	
}
/* ----------------------------------------------------- */

/* 要素別熱損失・熱取得（日積算値出力） */

void Helmdyprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day,
				 int Nroom, ROOM *Room, QETOTAL *Qetotal)
{
	int j;
	static int id = 0;
	
	if (id == 0)
	{      
		ttldyprint(fo, mrk, Simc);
		
		for (j = 0; j < 2; j++)
		{
			if (j == 0)
				fprintf(fo, "-cat\n");
			helmrmdyprint(fo, id, Nroom, Room, Qetotal);
			if (j == 0)
				fprintf(fo, "*\n#\n");
			id++;
		}
	}
	
	fprintf(fo, "%02d %02d\n", mon, day);
	helmrmdyprint(fo, id, Nroom, Room, Qetotal);
}
/* ----------------------------------------------------- */

void helmrmdyprint( FILE *fo, int id, int Nroom, ROOM *Room, QETOTAL *Qetotal)
{
	int	  i;
	BHELM  *q;
	QHELM  *qh;
	char   *name;
	
	extern double  cff_kWh;
	
	switch ( id )
	{
	case 0:
		if ( Nroom > 0 )
			fprintf ( fo, "%s %d\n", ROOM_TYPE, Nroom + 1);
		
		for (i = 0; i < Nroom; i++, Room++)
		{
			if ( Room->rmqe != NULL )
				fprintf ( fo, "%s 1 %d\n", Room->name, 29);
		}
		fprintf(fo, "%s 1 %d\n", Qetotal->name, 29);
		break ;
		
	case 1:
		for ( i = 0; i < Nroom + 1; i++, Room++)
		{
			if (i < Nroom)
				name = Room->name;
			else
				name = Qetotal->name;
			
			fprintf (fo, "%s_qldh Q f %s_qldc Q f ", name, name);
			fprintf (fo, "%s_slo Q f %s_slw Q f %s_asl Q f %s_tsol Q f %s_hins Q f\n",
				name, name, name, name, name);
			fprintf (fo, "%s_so Q f %s_sw Q f %s_rn Q f %s_in Q f %s_pnl Q f\n", 
				name, name, name, name, name);
			fprintf (fo, "%s_trs Q f %s_qew Q f %s_qwn Q f %s_qgd Q f %s_qnx Q f ",
				name, name, name, name, name);
			fprintf (fo, "%s_qi Q f %s_qc Q f %s_qf Q f\n",
				name, name, name);
			fprintf (fo, "%s_qvo Q f %s_qvr Q f %s_sto Q f\n",name, name, name);
			fprintf (fo, "%s_qldhl Q f %s_qldcl Q f %s_hinl Q f\n", name, name, name ) ;
			fprintf (fo, "%s_vol Q f %s_vrl Q f %s_stol Q f\n", name, name, name ) ;
		}
		break;
		
	default:
		for ( i = 0; i < Nroom + 1; i++, Room++ )
		{
			if (i < Nroom)
			{
				q = &Room->rmqe->qelmdy.qe ;
				qh = &Room->rmqe->qelmdy;  
				fprintf (fo, "%3.1lf %3.1lf ", 
					qh->loadh * cff_kWh, qh->loadc * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf  ", 
					qh->slo * cff_kWh, qh->slw * cff_kWh, qh->asl * cff_kWh, 
					qh->tsol * cff_kWh, qh->hins * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf  ", 
					q->so * cff_kWh, q->sg * cff_kWh, q->rn * cff_kWh,
					q->in * cff_kWh, q->pnl * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf ",
					q->trs * cff_kWh, qh->ew * cff_kWh, 
					qh->wn * cff_kWh, qh->gd * cff_kWh, qh->nx * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf %3.1lf ",
					qh->i * cff_kWh, qh->c * cff_kWh, qh->f * cff_kWh,
					qh->vo * cff_kWh, qh->vr * cff_kWh, qh->sto * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf %3.1lf\n",
					qh->loadhl * cff_kWh, qh->loadcl * cff_kWh, qh->hinl * cff_kWh,
					qh->vol * cff_kWh, qh->vrl * cff_kWh, qh->stol * cff_kWh ) ;
			}	       
			else
			{
				q = &Qetotal->qelmdy.qe;
				qh = &Qetotal->qelmdy; 
				fprintf (fo, "%3.1lf %3.1lf ", 
					qh->loadh * cff_kWh, qh->loadc * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf  ", 
					qh->slo * cff_kWh, qh->slw * cff_kWh, qh->asl * cff_kWh, 
					qh->tsol * cff_kWh, qh->hins * cff_kWh);
				
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf  ", 
					q->so * cff_kWh, q->sg * cff_kWh, q->rn * cff_kWh,
					q->in * cff_kWh, q->pnl * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf ",
					q->trs * cff_kWh, qh->ew * cff_kWh, 
					qh->wn * cff_kWh, qh->gd * cff_kWh, qh->nx * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf %3.1lf ",
					qh->i * cff_kWh, qh->c * cff_kWh, qh->f * cff_kWh,
					qh->vo * cff_kWh, qh->vr * cff_kWh, qh->sto * cff_kWh);
				fprintf (fo, "%3.1lf %3.1lf %3.1lf %3.1lf %3.1lf %3.1lf\n",
					qh->loadhl * cff_kWh, qh->loadcl * cff_kWh, qh->hinl * cff_kWh,
					qh->vol * cff_kWh, qh->vrl * cff_kWh, qh->stol * cff_kWh ) ;
			}	     
		}
		break;
	}
}
/* ----------------------------------------------------- */




