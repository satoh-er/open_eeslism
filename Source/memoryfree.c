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

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
//#include <iostream>
#include "esize.h"
#include "estr.h"
#include "sched.h"
#include "wthrd.h"
#include "daysum.h"
#include "build.h"
#include "eepath.h"
#include "eqptyp.h"
#include "eps.h"
#include "simcon.h"
#include "mfree.h"

//using namespace std ;

void	SchdlFree(SCHDL* Schdl)
{
	int		N;
	int		i;
	SEASN* Seasn;
	WKDY* Wkdy;
	DSCH* Dsch;
	DSCW* Dscw;
	SCH* Sch;

	free(Schdl->isw);
	free(Schdl->val);

	Seasn = Schdl->Seasn;
	N = Seasn->end;
	for (i = 0; i < N; i++, Seasn++)
	{
		free(Seasn->name);
		free(Seasn->sday);
		free(Seasn->eday);
	}
	free(Schdl->Seasn);

	Wkdy = Schdl->Wkdy;
	N = Wkdy->end;
	for (i = 0; i < N; i++, Wkdy++)
		free(Wkdy->name);
	free(Schdl->Wkdy);

	Dsch = Schdl->Dsch;
	N = Dsch->end;
	for (i = 0; i < N; i++, Dsch++)
	{
		free(Dsch->name);
		free(Dsch->stime);
		free(Dsch->etime);
		free(Dsch->val);
	}
	free(Schdl->Dsch);

	Dscw = Schdl->Dscw;
	N = Dscw->end;
	for (i = 0; i < N; i++, Dscw++)
	{
		free(Dscw->name);
		free(Dscw->stime);
		free(Dscw->etime);
		free(Dscw->mode);
	}
	free(Schdl->Dscw);

	Sch = Schdl->Sch;
	N = Sch->end;
	for (i = 0; i < N; i++, Sch++)
		free(Sch->name);
	free(Schdl->Sch);

	Sch = Schdl->Scw;
	N = Sch->end;
	for (i = 0; i < N; i++, Sch++)
		free(Sch->name);
	free(Schdl->Scw);
}

/***********************************************************************/

void	SimcFree(SIMCONTL* Simc)
{
	int		N, M, i, j;
	VCFILE* V;
	ESTL* E;
	RQLIST* R;
	//	TLIST	*T ;

	//	free ( Simc->File ) ;
	free(Simc->wfname);
	free(Simc->ofname);
	free(Simc->unit);
	free(Simc->unitdy);

	V = Simc->Vcfile;
	N = Simc->Nvcfile;
	for (i = 0; i < N; i++, V++)
	{
		free(V->name);
		free(V->fname);

		E = &V->Estl;
		free(E->flid);
		free(E->title);
		free(E->wdatfile);
		free(E->timeid);
		free(E->wdloc);

		for (j = 0; j < VTYPEMAX; j++)
			free(E->unit[j]);

		M = E->Nrqlist;
		R = E->Rq;
		for (j = 0; j < M; j++, R++)
		{
			free(R->id);
			free(R->name);
			free(R->rname);
		}
	}

	free(Simc->Vcfile);
}

/***********************************************************************/

void	Floutfree(FLOUT* Flout)
{
	int		i;

	for (i = 0; i < FLOUTMX; i++, Flout++)
		free(Flout->idn);
}

/***********************************************************************/

void	Compntfree(int N, COMPNT* c)
{
	int		i;
	extern int	DEBUG;

	for (i = 0; i < N; i++, c++)
	{
		if (DEBUG)
			printf("<Compntfree> Memory Free  i=%d name=%s\n", i, c->name);

		if (c->eqptype != NULL && strcmp(c->eqptype, ROOM_TYPE) != 0
			&& strcmp(c->eqptype, RDPANEL_TYPE) != 0)
		{
			free(c->name);
			c->name = NULL;
		}

		free(c->roomname);
		free(c->envname);
		free(c->exsname);
		free(c->hccname);
		free(c->rdpnlname);
		free(c->idi);
		free(c->ido);
		free(c->tparm);
		free(c->wetparm);
		//		free ( c->eqp ) ;
		free(c->ivparm);
		free(c->eqptype);
		free(c->omparm);
	}
}

/***********************************************************************/

void	Eloutfree(int N, ELOUT* Eo)
{
	int		i;

	for (i = 0; i < N; i++, Eo++)
	{
		/**********************************
		printf ( "i=%d N=%d name=%s id=%c coeffin=%lf\n",
			i, N, Eo->cmp->name, Eo->id, Eo->coeffin ) ;

		if ( i == 13 )
			printf ( "aaaa\n" ) ;
			/****************************************/
			//free ( Eo->coeffin ) ;
	}
}

/***********************************************************************/

void	Plistfree(int N, PLIST* P)
{
	int		i;

	for (i = 0; i < N; i++, P++)
	{
		//		printf ( "i=%d\n", i ) ;
		free(P->name);
		P->name = NULL;
	}
}

/***********************************************************************/

void	Mpathfree(int N, MPATH* M)
{
	int		i;

	for (i = 0; i < N; i++, M++)
	{
		free(M->name);
		M->name = NULL;
		free(M->pl);
		free(M->cbcmp);
	}

	//	free ( M ) ;
}

/***********************************************************************/

void	Eqcatfree(EQCAT* E)
{
	int		i, N;
	VAVCA* VAV;
	STHEATCA* Sth;
	HCCCA* Hcc;
	BOICA* Boi;
	RFCMP* Rfc;
	REFACA* Rf;
	COLLCA* Col;
	PIPECA* Pipe;
	STANKCA* Stank;
	HEXCA* Hex;
	PUMPCA* Pump;
	THEXCA* Thex;
	PFCMP* pfcmp;
	OMVAVCA* omvav;

	//try {
	VAV = E->Vavca;
	N = E->Nvavca;
	for (i = 0; i < N; i++, VAV++)
		free(VAV->name);
	free(E->Vavca);

	Sth = E->stheatca;
	N = E->Nstheatca;
	for (i = 0; i < N; i++, Sth++)
		free(Sth->name);
	free(E->stheatca);

	Hcc = E->Hccca;
	N = E->Nhccca;
	for (i = 0; i < N; i++, Hcc++)
		free(Hcc->name);
	free(E->Hccca);

	Boi = E->Boica;
	N = E->Nboica;
	for (i = 0; i < N; i++, Boi++)
		free(Boi->name);
	free(E->Boica);

	Rfc = E->Rfcmp;
	N = E->Nrfcmp;
	for (i = 0; i < N; i++, Rfc++)
	{
		free(Rfc->name);
		free(Rfc->cname);
		Rfc->name = Rfc->cname = NULL;
	}
	//free ( E->Rfcmp ) ;

	Rf = E->Refaca;
	N = E->Nrefaca;
	for (i = 0; i < N; i++, Rf++)
	{
		free(Rf->name);
		free(Rf->cool);
		free(Rf->heat);
	}
	free(E->Refaca);

	Col = E->Collca;
	N = E->Ncollca;
	for (i = 0; i < N; i++, Col++)
		free(Col->name);
	free(E->Collca);

	Pipe = E->Pipeca;
	N = E->Npipeca;
	for (i = 0; i < N; i++, Pipe++)
		free(Pipe->name);
	free(E->Pipeca);

	Stank = E->Stankca;
	N = E->Nstankca;
	for (i = 0; i < N; i++, Stank++)
	{
		free(Stank->name);
		free(Stank->tparm);
	}
	free(E->Stankca);

	Hex = E->Hexca;
	N = E->Nhexca;
	for (i = 0; i < N; i++, Hex++)
		free(Hex->name);
	free(E->Hexca);

	Pump = E->Pumpca;
	N = E->Npumpca;
	for (i = 0; i < N; i++, Pump++)
	{
		free(Pump->name);
		free(Pump->val);
		free(Pump->type);
	}
	free(E->Pumpca);

	pfcmp = E->pfcmp;
	N = E->Npfcmp;
	for (i = 0; i < N; i++, pfcmp++)
	{
		free(pfcmp->type);
	}
	free(E->pfcmp);

	Thex = E->Thexca;
	N = E->Nthexca;
	for (i = 0; i < N; i++, Thex++)
		free(Thex->name);
	free(E->Thexca);

	omvav = E->OMvavca;
	N = E->Nomvavca;
	for (i = 0; i < N; i++, omvav++)
	{
		free(omvav->name);
		omvav->name = NULL;
	}
	//free ( E->OMvavca ) ;
//}
//catch (int e )
//{
	//cout << "Error Number : " << e << \n ;
//}
}

/***********************************************************************/

void	Eqsysfree(EQSYS* E)
{
	int		i, N;
	REFA* R;
	//	COLL	*Co ;
	//	PIPE	*P ;
	STANK* S;
	HEX* He;
	PUMP* Pu;
	FLIN* F;
	HCLOAD* Hc;
	GLOAD* G;
	VAV* V;
	STHEAT* St;
	THEX* T;
	VALV* Valv;
	QMEAS* Q;

	free(E->Cnvrg);
	free(E->Hcc);
	free(E->Boi);

	R = E->Refa;
	N = E->Nrefa;
	for (i = 0; i < N; i++, R++)
	{
		//		free ( R->name ) ;
		R->name = NULL;
		//		free ( R->load ) ;
		//		free ( R->Ta ) ;
	}
	free(E->Refa);

	free(E->Coll);
	free(E->Pipe);

	S = E->Stank;
	N = E->Nstank;
	for (i = 0; i < N; i++, S++)
	{
		//		free ( S->name ) ;
//		S->name = NULL ;
		free(S->Jin);
		free(S->Jout);
		free(S->pthcon);
		//		free ( S->batchcon ) ;
		free(S->ihex);
		free(S->B);
		free(S->R);
		free(S->d);
		free(S->Fg);
		free(S->Tss);
		free(S->dtankF);
		free(S->Tssold);
		free(S->dvol);
		free(S->Mdt);
		free(S->KS);
		free(S->ihxeff);
		free(S->KA);
		free(S->cGwin);
		free(S->EGwin);
		free(S->Twin);
		free(S->Q);
		//		free ( S->Tenv ) ;
		free(S->stkdy);
		free(S->mstkdy);
		free(S->KAinput);
	}
	free(E->Stank);

	He = E->Hex;
	N = E->Nhex;
	for (i = 0; i < N; i++, He++)
	{
		//		free ( He->name ) ;
		He->name = NULL;
	}
	free(E->Hex);

	Pu = E->Pump;
	N = E->Npump;
	for (i = 0; i < N; i++, Pu++)
	{
		//		free ( Pu->name ) ;
		Pu->name = NULL;
	}
	free(E->Pump);

	F = E->Flin;
	N = E->Nflin;
	for (i = 0; i < N; i++, F++)
	{
		//		free ( F->name ) ;
		F->name = NULL;
		free(F->namet);
		free(F->namex);
		//		free ( F->vart ) ;
		//		free ( F->varx ) ;
	}
	free(E->Flin);

	Hc = E->Hcload;
	N = E->Nhcload;
	for (i = 0; i < N; i++, Hc++)
	{
		//		free ( Hc->name ) ;
		Hc->name = NULL;
		//		free ( Hc->loadt ) ;
		//		free ( Hc->loadx ) ;
	}
	free(E->Hcload);

	G = E->Gload;
	N = E->Ngload;
	for (i = 0; i < N; i++, G++)
	{
		//		free ( G->name ) ;
		G->name = NULL;
		free(G->nameql);
		free(G->nameqs);
		free(G->nameQt);
		free(G->Qs);
		free(G->Ql);
		free(G->Qt);
	}
	free(E->Gload);

	V = E->vav;
	N = E->Nvav;
	for (i = 0; i < N; i++, V++)
	{
		//		free ( V->name ) ;
		V->name = NULL;
	}
	free(E->vav);

	St = E->stheat;
	N = E->Nstheat;
	for (i = 0; i < N; i++, St++)
	{
		//		free ( St->name ) ;
		St->name = NULL;
	}
	free(E->stheat);

	T = E->Thex;
	N = E->Nthex;
	for (i = 0; i < N; i++, T++)
		T->name = NULL;
	free(E->Thex);

	Valv = E->Valv;
	N = E->Nvalv;
	/***************************
	for ( i = 0; i < N; i++, Valv++ )
		free ( Valv->name ) ;
		***************************/
	free(E->Valv);

	N = E->Nqmeas;
	Q = E->Qmeas;
	//for ( i = 0; i < E->Nqmeas; i++, Q++ )
		//free ( Q->name ) ;
	free(E->Qmeas);
}

/***********************************************************************/

void	Rmvlsfree(RMVLS* R)
{
	int		i, N, j, ic;
	WALL* w;
	WINDOW* W;
	SNBK* s;
	RMSRF* S;
	MWALL* m;
	ROOM* r;
	RDPNL* p;
	//	QETOTAL	*q ;
	WELM* we;
	RMLOAD* rl;
	RMQELM* rq;

	free(R->Emrk);
	free(R->Trdav);
	free(R->Qrm);
	free(R->Qrmd);

	if (R->Wall != NULL)
	{
		w = R->Wall;
		N = R->Nwall;
		for (i = 0; i < N; i++, w++)
		{
			free(w->R);
			free(w->CAP);
			free(w->name);
			free(w->res);
			free(w->cap);

			we = w->welm;
			for (j = 0; j < WELMMX; j++, we++)
				free(we->code);
		}
		free(R->Wall);
	}

	if (R->Window != NULL)
	{
		W = R->Window;
		N = R->Nwindow;
		for (i = 0; i < N; i++, W++)
		{
			free(W->name);
			free(W->Cidtype);
		}
		free(R->Window);
	}

	if (R->Snbk != NULL)
	{
		s = R->Snbk;
		N = s->end;
		for (i = 0; i < N; i++, s++)
			free(s->name);
		free(R->Snbk);
	}

	if (R->Sd != NULL)
	{
		S = R->Sd;
		N = R->Nsrf;
		for (i = 0; i < N; i++, S++)
		{
			free(S->Tc);
			free(S->name);
			free(S->sname);
			//		free ( S->alicsch ) ;
			free(S->WSRN);
			free(S->WSPL);
			free(S->ctlif);
			free(S->dynamiccode);
		}
	}

	if (R->Mw != NULL)
	{
		m = R->Mw;
		N = R->Nmwall;
		for (i = 0; i < N; i++, m++)
		{
			free(m->UX);
			free(m->Tw);
			free(m->Told);
			free(m->Toldd);
			free(m->Twd);
			free(m->res);
			free(m->cap);
		}
	}
	free(R->Mw);

	ic = 0;
	if (R->Room != NULL)
	{
		r = R->Room;
		N = R->Nroom;
		for (i = 0; i < N; i++, r++, ic++)
		{
			//		printf ( "i=%d\n", i ) ;

			free(r->name);
			//		free ( r->rsrf ) ;
			free(r->achr);
			free(r->trnx);
			free(r->rmpnl);
			free(r->arsp);

			rl = r->rmld;
			if (rl != NULL)
			{
				//			free ( rl->loadt ) ;
				//			free ( rl->loadx ) ;
				free(rl->FOTN);
				free(rl->FOPL);
			}
			free(r->rmld);

			rq = r->rmqe;
			if (rq != NULL)
			{
				free(rq->rmsb->Tw);
				free(rq->rmsb->Told);
				free(rq->rmsb);

				if (ic == 0)
					free(rq->WSCwk);
			}
			free(r->rmqe);

			free(r->F);
			free(r->alr);
			free(r->XA);
			free(r->Wradx);
			//free ( r->alc ) ;
			free(r->ARN);
			free(r->RMP);

			//free ( r->rairflow ) ;
		}
	}
	free(R->Room);

	if (R->Rdpnl != NULL)
	{
		p = R->Rdpnl;
		N = R->Nrdpnl;
		//		for ( i = 0; i < N; i++, p++ )
		//			free ( p->name ) ;
				//free ( R->Rdpnl ) ;
	}

	//if ( R->airflow != NULL )
	//	free ( R->airflow ) ;

	free(R->Qetotal.name);
}

/***********************************************************************/

void	Exsffree(EXSFS* E)
{
	int		i, N;
	EXSF* e;

	N = E->Nexs;
	e = E->Exs;
	for (i = 0; i < N; i++, e++)
		free(e->name);

	free(E->Exs);
}
