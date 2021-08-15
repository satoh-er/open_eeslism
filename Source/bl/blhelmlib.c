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

/* helmlib.c */

#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"

/* 外乱要素別表面温度の計算 */

void  helmrmsrt(ROOM *Room, double Ta)
{
	int    i;
	double  Twmp, *Tw;
	BHELM  *WSC, *Ts;
	RMSRF  *Sd;
	MWALL  *Mw;
	RMSB   *Rmsb;
	double *XA;
	
	/**** int m;   
	BHELM *told;
	******/
	
	if(Room->rmqe == NULL)
		return ;
	
	Sd = Room->rsrf ;
	WSC = Room->rmqe->WSCwk;
	Rmsb = Room->rmqe->rmsb;
	
	for ( i = 0; i < Room->N; i++)
	{
		helmclear(WSC);
		
		if (( Mw = Sd->mw) != NULL )
		{	
		/**************************** 
		if (i == 0 || i == 5)
        {
		printf("Told xxxxx r=%s i=%d rmsb=%d\n",
		Room->name, i, Rmsb - Room->rmqe->rmsb);
		told = Rmsb->Told;
		for (m = 0; m < Mw->M; m++, told++)
		{
		printf("m=%d ", m);
		helmxxprint(" Told", told);
		}
		}
			*******************************/
			
			helmsumpd( Mw->M, Mw->UX, Rmsb->Told, WSC);
		}
		WSC->trs += Sd->FI * (Sd->alic / Sd->ali) * Room->Tr;
		
		if ( Sd->rpnl != NULL )
		{
			Tw = Sd->mw->Tw ;
			Twmp = *(Tw+Sd->mw->mp) ;
			WSC->pnl += Sd->FP * ( Sd->rpnl->Tpi - Twmp);	
			WSC->trs += Sd->FP * Twmp ;
		}
		
		switch (Rmsb->type)
		{
		case 'E':
			WSC->trs += Sd->FO * Ta;
			WSC->rn += Sd->FO * Sd->TeErn ;
			if (Sd->ble == 'E')
				WSC->so += Sd->FO * Sd->TeEsol ;
			else if (Sd->ble == 'W')
				WSC->sg += Sd->FO * Sd->TeEsol ;
            break;
		case 'G':
			WSC->trs += Sd->FO * Sd->Te;
			break ;
		case 'i':
			WSC->trs += Sd->FO * Sd->nextroom->Trold;
			break ;
		}
		
		WSC->sg += Sd->FI * Sd->RSsol / Sd->ali ;
		WSC->in += Sd->FI * Sd->RSin  / Sd->ali ;
		/**************      
		printf("helmrmsrt  i=%d  Tr=%4.1lf WSR=%lf \n", i, Room->Tr, Sd->WSR);
		helmxxprint("WSC", WSC);
		helmxxprint("Ts", Ts);
		******************/ 
		Sd++, Rmsb++, WSC++ ;
	}
	
	Sd = Room->rsrf ;
	Rmsb = Room->rmqe->rmsb;
	XA = Room->XA ;
	
	for (i = 0; i < Room->N; i++)
	{      
		Ts = &Rmsb->Ts;
		helmclear(Ts); 
		helmsumpd(Room->N, XA, Room->rmqe->WSCwk, Ts) ;
		
		/**********************
		if (i == 0 || i == 5)
		{
		printf("helmrmsrt  i=%d\n", i);
		helmxxprint("Ts  ", &Rmsb->Ts);
		}
		***************************/
		
		Rmsb++ ;
		XA += Room->N ;
	}
}
/* ---------------------------------------------- */

/* 壁体内温度 */

void helmwall(ROOM *Room, double Ta)
{
	int	i, m;
	BHELM  Tm, Te, Tie, Tpe, *Told, *Tw;
	RMSRF  *Sd;
	MWALL  *Mw ;
	RMSB   *rmsb ;
	double  *alr, Twmp, *Twp ;
	
	if (Room->rmqe == NULL)
		return ;
	
	Sd = Room->rsrf;
	rmsb = Room->rmqe->rmsb;
	alr = Room->alr;   
	for (i = 0; i < Room->N; i++)
	{      
		if ((Mw = Sd->mw) != NULL)
		{
			helmclear(&Tie) ;		      
			helmclear(&Te) ;
			helmclear(&Tpe) ;
			
			helmwlsft(i, Room->N, alr, Room->rmqe->rmsb, &Tm);
			
			/******printf("helmwall  i=%d  M=%d\n", i, Sd->mw->M);
			helmxxprint("helmwall Tm", &Tm);*******/
			
			helmsumpf(1, &Sd->alir, &Tm, &Tie) ;
			Tie.trs += Sd->alic * Room->Tr ;
			
			if ( Sd->rpnl != NULL )
			{
				Twp = Mw->Tw ;
				Twmp = *(Twp+Mw->mp) ;
				Tpe.pnl = Sd->rpnl->Tpi - Twmp ;
				Tpe.trs = Twmp ;
			}
			
			switch (rmsb->type)
			{
			case 'E':
				Te.trs = Ta ;
				Te.so = Sd->TeEsol ;
				Te.rn = Sd->TeErn ;	  
				break;
			case 'G':
				Te.trs = Sd->Te ;
				break ;
            case 'i':
				Te.trs = Sd->nextroom->Trold;
				break ;
			}
			
			Tie.sg += Sd->RSsol;
			Tie.in += Sd->RSin;
			helmdiv(&Tie, Sd->ali);
			/***********
			helmxxprint("helmwall Tie", &Tie);
			helmxxprint("helmwall Te", &Te);***********/
			
			helmwlt(Mw->M, Mw->mp, Mw->UX, Mw->uo,
				Mw->um, Mw->Pc, &Tie, &Te, &Tpe,
				rmsb->Told, rmsb->Tw );
			
			Told = rmsb->Told;
			Tw = rmsb->Tw;
			for (m = 0; m < Mw->M; m++, Told++, Tw++)
				helmcpy(Tw, Told);
			
		}        
		Sd++ ;
		rmsb++ ;
		alr += Room->N ;      
	}
}
/* ---------------------------------------------- */

/* 面 i についての平均表面温度 */

void helmwlsft(int i, int N, double *alr, RMSB *rmsb, BHELM *Tm )
{
	int    j ;
	double  *Ralr ;
	
	Ralr = alr + i ;
	
	helmclear ( Tm ) ;
	
	for ( j = 0; j < N; j++, alr++, rmsb++ )
	{
		if ( j != i )
			helmsumpf ( 1, alr, &rmsb->Ts, Tm ) ;
	}
	helmdiv ( Tm, *Ralr ) ;
}
/* ---------------------------------------------- */

void helmwlt ( int M, int mp, double *UX, double uo, double um, 
			  double Pc, BHELM *Tie, BHELM *Te, BHELM *Tpe,
			  BHELM *Told, BHELM *Tw )
{
	int   m;
	BHELM *Toldo, *Toldm, *Toldp ;
	
	/*****************   
	printf("helmwlt uo=%lf    um=%lf\n", uo, um);
	helmxxprint("helmwlt  Tie", Tie);
	helmxxprint("helmwlt  Toldo", Told);
	
	 printf("helmwlt uo=%lf    um=%lf\n", uo, um);
	 
	  for (m = 0; m < M; m++)
	  {
      printf("helmwlt  m=%d\n", m);
      helmxxprint("helmwlt  Told",Told + m);
	  }
	*********************/
	
	Toldo = Told ;
	Toldm = Told + M - 1 ;
	Toldp = Told + mp ;
	helmsumpd ( 1, &uo, Tie, Toldo ) ;
	helmsumpd ( 1, &um, Te, Toldm ) ;   
	
	if ( Pc > 0.0 )
		helmsumpd ( 1, &Pc, Tpe, Toldp ) ;
	
	for ( m = 0; m < M; m++, Tw++ )
	{
		helmclear ( Tw ) ;      
		helmsumpd ( M, UX, Told, Tw ) ;
		UX += M ;
	}
}
/* ---------------------------------------------- */

/* 要素別熱損失・熱取得 */

void helmq(ROOM *Room, double dTM, double Ta, double xa )
{
	int    i, j ;
	ROOM *Rm;
	BHELM  *q, *Ts ;
	QHELM  *qh;
	RMSRF  *Sd ;
	RMSB   *rmsb;
	ACHIR  *achr ;
	double  Aalc, qloss;
	extern double ca, ro;
	
	Rm = Room;
	
	qelmclear (&Room->rmqe->qelm) ;
	q = &Room->rmqe->qelm.qe;
	qh = &Room->rmqe->qelm;
	
	qh->loadh = qh->loadc = 0.0;
	qh->loadcl = qh->loadhl = 0.0 ;
	if (Room->rmld != NULL)
	{
		if (Room->rmld->Qs > 0.0)
			qh->loadh = Room->rmld->Qs;
		else
			qh->loadc = Room->rmld->Qs;

		if (Room->rmld->Ql > 0.0 )
			qh->loadhl = Room->rmld->Ql ;
		else
			qh->loadcl = Room->rmld->Ql ;
	}
	
	Sd = Room->rsrf ;
	rmsb = Room->rmqe->rmsb;
	
	for ( i = 0; i < Room->N; i++, Sd++, rmsb++)
	{
		Aalc = Sd->A * Sd->alic ;
		Ts = &rmsb->Ts ;
		qloss = Aalc * (Room->Tr - Ts->trs);
		q->trs -= qloss;
		
		if (rmsb->type == 'E')
		{
			if (Sd->ble == 'E')
				qh->ew -= qloss;
			else if (Sd->ble == 'W')
				qh->wn -= qloss;
		}	    
		else if (rmsb->type == 'G')
			qh->gd -= qloss;
		else if (rmsb->type == 'i')
			qh->nx -= qloss;
		
		if (Sd->ble == 'c' || Sd->ble == 'R')
			qh->c -= qloss;
		else if (Sd->ble == 'f' || Sd->ble == 'F')
			qh->f -= qloss;   
		else if (Sd->ble == 'i' || Sd->ble == 'd')
			qh->i -= qloss;   
		
		q->so  += Aalc * Ts->so  ;
		q->sg  += Aalc * Ts->sg ;
		q->rn += Aalc * Ts->rn;
		q->in += Aalc * Ts->in;
		q->pnl += Aalc * Ts->pnl;
	}
	
	q->in += Room->Hc + Room->Lc + Room->Ac;

	qh->hinl = Room->AL + Room->HL ;
	
	qh->sto = Room->MRM * (Room->Trold - Room->Tr) / dTM ;
	qh->stol = Room->GRM * ro * ( Room->xrold - Room->xr ) / dTM ;
	qh->vo = ca * Room->Gvent * (Ta - Room->Tr);
	qh->vol = ro * Room->Gvent * ( xa - Room->xr ) ;
	
	achr = Room->achr;
	qh->vr = 0.0;
	qh->vrl = 0.0 ;
	for ( j = 0; j < Room->Nachr; j++, achr++ )
	{
		qh->vr += ca * achr->Gvr * ((Rm+achr->rm)->Tr - Room->Tr);
		qh->vrl += ro * achr->Gvr * ((Rm+achr->rm)->xr - Room->xr) ;
	}
}
/* ---------------------------------------------- */

void qelmclear(QHELM *q)
{
	helmclear(&q->qe);
	q->slo = q->slw = q->asl = q->tsol = q->hins = 0.0;
	q->nx = q->gd = q->ew = q->wn = 0.0;
	q->i = q->c = q->f = 0.0;
	q->vo = q->vr = q->sto = q->loadh = q->loadc = 0.0;
	q->hinl = q->vol = q->vrl = q->stol = q->loadcl = q->loadhl = 0.0 ;
}
/* ---------------------------------------------- */

void qelmsum(QHELM *a, QHELM *b)
{
	helmsum(&a->qe, &b->qe);
	
	b->slo += a->slo;
	b->slw += a->slw;
	b->asl += a->asl;
	b->tsol += a->tsol;
	b->hins += a->hins;
	
	b->nx += a->nx  ;
	b->gd += a->gd ;
	b->ew += a->ew ;
	b->wn += a->wn ;
	
	b->i += a->i  ;
	b->c += a->c ;
	b->f += a->f ;
	b->vo += a->vo ;
	b->vr += a->vr ;
	b->sto += a->sto ;
	b->loadh += a->loadh ;
	b->loadc += a->loadc ;

	b->hinl += a->hinl ;
	b->vol += a->vol ;
	b->stol += a->stol ;
	b->vrl += a->vrl ;
	b->loadcl += a->loadcl ;
	b->loadhl += a->loadhl ;
}
/* ---------------------------------------------- */

void helmclear(BHELM *b)
{
	b->trs = b->so = b->sg = 0.0 ;
	b->rn = b->in  = b->pnl = 0.0 ;
}
/* ---------------------------------------------- */

void helmsumpd(int N, double *u, BHELM *a, BHELM *b)
{
	int  i ;
	
	for ( i = 0; i < N; i++, a++, u++ )
	{
		b->trs += *u * a->trs ;
		b->so += *u * a->so  ;
		b->sg += *u * a->sg ;
		b->rn += *u * a->rn ;
		b->in += *u * a->in  ;
		b->pnl += *u * a->pnl ;
	}
}
/* ---------------------------------------------- */

void helmsumpf(int N, double *u, BHELM *a, BHELM *b)
{
	int  i ;
	
	for ( i = 0; i < N; i++, a++, u++ )
	{
		b->trs += *u * a->trs ;
		b->so += *u * a->so  ;
		b->sg += *u * a->sg ;
		b->rn += *u * a->rn ;
		b->in += *u * a->in  ;
		b->pnl += *u * a->pnl ;
	}
}
/* ---------------------------------------------- */

void helmdiv (BHELM *a, double c)
{
	a->trs /= c ;
	a->so /= c;
	a->sg /= c ;
	a->rn /= c ;
	a->in /= c ;
	a->pnl /= c ;
}
/* ---------------------------------------------- */

void helmsum(BHELM *a, BHELM *b)
{
	b->trs += a->trs  ;
	b->so += a->so ;
	b->sg += a->sg ;
	b->rn += a->rn ;
	b->in += a->in  ;
	b->pnl += a->pnl ;
}
/* ---------------------------------------------- */

void helmcpy(BHELM *a, BHELM *b)
{
	b->trs = a->trs  ;
	b->so= a->so ;
	b->sg = a->sg ;
	b->rn = a->rn ;
	b->in = a->in  ;
	b->pnl = a->pnl ;
}
/* ========================================== */

void helmxxprint(char *s, BHELM *a)
{
	
	printf("xxx helmprint xxx %s  trs so sg rn in pnl\n", s);
	printf("%6.2lf %6.2lf %6.2lf %6.2lf %6.2lf %6.2lf\n",
		a->trs, a->so, a->sg, a->rn, a->in, a->pnl);
}


