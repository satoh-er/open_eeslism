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

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "esize.h"
#include "common.h"
#include "eps.h"
#include "wthrd.h"
#include "simcon.h"
#include "eepath.h"
#include "daysum.h"
#include "build.h"
#define	 ALO	23.0
#include "eqptyp.h"
#include "control.h"
#include "sched.h"
#include "fnlib.h"


/*****  SIMCONTL の初期化  *****/

void	Simcinit ( SIMCONTL *S )
{
	int		i ;
	int		*dw, *dp ;

	strcpy ( S->title, "" ) ;
	S->File = S->wfname = S->ofname = S->unit = S->unitdy = NULL ;
	S->fwdata = S->fwdata2 = S->ftsupw = NULL ;
	strcpy ( S->timeid, "     " ) ;
	S->helmkey = S->wdtype = ' ' ;
	S->daystartx = S->daystart = S->dayend = S->Dayntime = 0 ;
	S->Ntimedyprt = S->Ntimehrprt = S->Nhelmsfpri = 0 ;
	S->Nvcfile = S->dTm = S->sttmm = 0 ;
	S->Vcfile = NULL ;
	S->Loc = NULL ;
	S->wdpt.ta = S->wdpt.xa = S->wdpt.rh = S->wdpt.idn = S->wdpt.isky = NULL ;
	S->wdpt.ihor = S->wdpt.cc = S->wdpt.rn = S->wdpt.wv = S->wdpt.wdre = NULL ;
	S->MaxIterate = 5;			// 最大収束回数のデフォルト値

	dw = S->daywk ;
	dp = S->dayprn ;
	for ( i = 0; i < 366; i++, dw++, dp++ )
		dw = dp = 0 ;
}


/*****  COMPNT の初期化  *****/

void	Compinit ( int N, COMPNT *C )
{
	int		i ;
	
	for (i=0; i<N; i++, C++)
	{
		C->name = NULL ;
		C->roomname = NULL ;
		C->eqptype = NULL ;
		C->envname = NULL ;
		C->exsname = NULL ;
		C->hccname = NULL ;
		C->idi = NULL ;
		C->ido = NULL ;
		C->tparm = NULL ;
		C->wetparm = NULL ;
		C->eqp = NULL ;
		C->ivparm = NULL ;
		C->elouts = NULL ;
		C->elins = NULL ;
		C->neqp = C->ncat = C->nivar = 0 ;
		C->eqpeff = 0.0 ;
		C->airpathcpy = C->control = ' ' ;
		C->Nout = C->Nin = 3 ;
		C->valvcmp = NULL ;
		C->rdpnlname = NULL ;
		C->omparm = NULL ;
		C->PVcap = -999.0 ;
		C->Ac = C->Area = -999.0 ;
//		C->x = 1.0 ;
//		C->xinit = -999.0 ;
//		C->org = 'n' ;
		//C->OMfanName = NULL ;
		C->monPlistName = NULL ;
		C->mPCM = -999.0;
	}
}


/*****  ELOUT の初期化  *****/

void	Eloutinit ( ELOUT *Eo, int N )
{
	int		i ;

	for (i=0; i < N; i++, Eo++)
	{
		Eo->Ni = 0 ;
		Eo->G = 0.0 ;
		Eo->Co = 0.0 ;
		Eo->coeffo = 0.0 ;
		Eo->control = ' ' ;
		Eo->id = ' ' ;
		Eo->fluid = Eo->sysld = ' ' ;
		Eo->Q = Eo->sysv = Eo->load = 0.0 ;
		Eo->sv = Eo->sld = 0 ;

		Eo->cmp = NULL ;
		Eo->elins = NULL ;
		Eo->eldobj = Eo->emonitr = NULL ;
		
		Eo->coeffin = NULL ;
		Eo->lpath = NULL ;
		Eo->Pelmoid = 'x' ;
	}
}


/*****  ELIN の初期化  *****/

void	Elininit ( int N, ELIN *Ei )
{
	int		i ;

	for (i=0; i<N; i++, Ei++)
	{
		Ei->id = ' ' ;
		Ei->sysvin = 0.0 ;
		Ei->upo = Ei->upv = NULL ;
		Ei->lpath = NULL ;
	}
}


/*****  PLIST の初期化  *****/

void	Plistinit ( int N, PLIST *Pl )
{
	int		i ;

	for (i=0; i<N; i++, Pl++)
	{
		Pl->name = NULL ;
		Pl->type = Pl->control = ' ' ;
		Pl->batch = 'n' ;
		Pl->org = 'y' ;
		Pl->plmvb = Pl->pelm = NULL ;
		Pl->lpair = NULL ;
		Pl->Go = NULL ;
		Pl->G = -999.0 ;
		Pl->Nelm = Pl->lvc = Pl->Nvav = Pl->Nvalv = 0 ;
		//Pl->Npump = 0 ;
		Pl->n = -999 ;
		Pl->valv = NULL ;
		Pl->Mpath = NULL ;
		Pl->plistt = Pl->plistx = NULL ;
		Pl->rate = NULL ;
		Pl->upplist = Pl->dnplist = NULL ;
		Pl->NOMVAV = 0 ;
		//Pl->Pump = NULL ;
		Pl->OMvav = NULL ;
		Pl->UnknownFlow = 1 ;
		Pl->plistname = NULL ;
		Pl->Gcalc = 0.0 ;
	}
}


/*****  PELM の初期化  ******/

void	Pelminit ( int N, PELM *Pe )
{
	int		i ;

	for ( i = 0; i < N; i++, Pe++ )
	{
		Pe->co = Pe->ci = ' ' ;
		Pe->cmp = NULL ;
		Pe->out = NULL ;
		Pe->in = NULL ;
		//Pe->Pelmx = NULL ;
	}
}


/*****  MPATH の初期化  *****/

void	Mpathinit ( int N, MPATH *M )
{
	int		i ; // , j ;

	for ( i=0; i<N; i++, M++ )
	{
		M->name = NULL ;
		M->Nlpath = M->NGv = M->NGv2 = M->Ncv = M->lvcmx = 0 ;
		M->plist = NULL ;
		M->mpair = NULL ;
		M->sys = M->type = M->fluid = M->control = ' ' ;
		M->pl = NULL ;
		M->cbcmp = NULL ;
		M->G0 = NULL ;
		M->rate = 'N' ;

//		for ( j = 0; j < CBCMPMX; j++ )
//		{
//			M->pl[j] = NULL ;
//			M->cbcmp[j] = NULL ;
//		}
	}
}


/*****  EXSF の初期化  ******/

void	Exsfinit ( int Nd, EXSF *e )
{
	int		i ;

	for (  i = 0; i < Nd; i++, e++ )
	{
		e->name = NULL ;
		e->typ = 'S' ;
		e->Wa = e->Wb = 0.0 ;
		e->Rg = e->Fs = e->Wz = e->Ww = e->Ws = 0.0 ;
		e->swb = e->cbsa = e->cbca = e->cwa = 0.0 ;
		e->swa = e->Z = e->erdff = e->cinc = 0.0 ;
		e->tazm = e->tprof = e->Idre = e->Idf = 0.0 ;
		e->Iw = e->rn = e->Tearth = 0.0 ;
		e->erdff = 0.36e-6;
		e->alo = NULL ;
		e->end = 0 ;
		//e->alosch = NULL ;
		e->alotype = 'F' ;
	}
}


/*****  SYSEQ の初期化  *****/

void	Syseqinit ( SYSEQ *S )
{
//	int		i ;
	S->a = ' ' ;
//	matinit ( S->eqcf, (int) SYSEQMX * SYSEQMX ) ;
//	matinit ( S->eqcv, (int) SYSEQMX ) ;
//	matinit ( S->Y, (int) SYSEQMX ) ;
//	strcpy ( S->mrk, "" ) ;

//	for ( i = 0; i < SYSEQMX; i++ )
//	{
//		S->eleq[i] = NULL ;
//		S->elosv[i] = NULL ;
//	}
}


/*****  EQSYS の初期化  *****/

void	Eqsysinit ( EQSYS *E )
{
	E->Ncnvrg = E->Nhcc = E->Nboi = E->Nrefa = E->Ncoll = 0 ;
	E->Npipe = E->Nstank = E->Nhex = E->Npump = E->Nflin = 0 ;
	E->Nhcload = E->Ngload = E->Nvav = E->Nstheat = E->Ndesi = E->Nevac = 0 ;
	E->Nthex = E->Nvalv = E->Nqmeas = 0 ;
	E->Npv = 0 ;
	E->Nomvav = 0 ;

	E->Cnvrg = ( COMPNT ** ) NULL ;
	E->Hcc = NULL ;
	E->Boi = NULL ;
	E->Refa = NULL ;
	E->Coll = NULL ;
	E->Pipe = NULL ;
	E->Stank = NULL ;
	E->Hex = NULL ;
	E->Pump = NULL ;
	E->Flin = NULL ;
	E->Hcload = NULL ;
	E->Gload = NULL ;
	E->vav = NULL ;
	E->stheat = NULL ;
	E->Thex = NULL ;
	E->Valv = NULL ;
	E->Qmeas = NULL ;
	E->PVcmp = NULL ;
	E->OMvav = NULL ;
}

/*****  RMVLS の初期化  *****/

void	Rmvlsinit ( RMVLS *R )
{
//	int		i ; //, j ;
//	int		*f ;
//	ROOM	*B ;
//	MWALL	*Mw ;
//	RMSRF	*S ;

	R->Twallinit = 0.0 ;
	R->Nroom = R->Nrdpnl = R->Nwindow = R->Nmwall = R->Nsrf = 0 ;
//	strcpy ( R->Emrk, "" ) ;
	R->Emrk = NULL ;
	R->Wall = NULL ;
	R->Window = NULL ;
	R->Snbk = NULL ;
	R->Rdpnl = NULL ;
	R->Qrm = R->Qrmd = NULL ;
	R->Trdav = NULL ;
	R->Sd = NULL ;
	R->PCM = NULL ;
	//R->airflow = NULL ;
	R->pcmiterate = 'n';
	R->Npcm = 0;

	/********************************************
	S = R->Sd ;
	for ( i = 0; i < NSMX; i++, S++ )
	{
		S->name = NULL ;
		S->room = S->nextroom = NULL ;
		S->nxsd = NULL ;
		S->mw = NULL ;
		S->rpnl = NULL ;
		S->rm = S->n = S->exs = S->nxn = S->nxrm = S->wd = S->rmw = 0 ;
		S->lwd = S->dr = S->fn = S->Nfn = S->fnsw = S->sb = S->end = 0 ;
		S->wlpri = S->sfepri = S->ble = S->typ = S->mwtype = S->mwside = S->mrk = ' ' ;

		f = S->fnd ;
		for ( j = 0; j < 4; j++, f++ )
			*f = 0 ;

		S->A = S->Eo = S->as = S->c = S->tgtn = S->Bn = S->srg = S->srh = S->srl = 0.0 ;
		S->sra = S->alo = S->ali = S->alic = S->alir = S->K = S->Rwall = S->FI = 0.0 ;
		S->FO = S->FP = S->CF = S->WSR = S->WSC = S->RS = S->RSsol = S->RSin = 0.0 ;
		S->TeEsol = S->TeErn = S->Te = S->Tmrt = S->Ei = S->Ts = S->eqrd = 0.0 ;
		S->alicsch = S->WSRN = S->WSPL = NULL ;
	}
	**********************************************/

	R->Mw = NULL ;
	/*********************************
	Mw = R->Mw ;
	for ( i = 0; i < MWALLMX; i++, Mw++ )
	{
		Mw->sd = NULL ;
		Mw->nxsd = NULL ;
		Mw->wall = NULL ;
		Mw->ns = Mw->rm = Mw->n = Mw->nxrm = Mw->nxn
			= Mw->M = Mw->mp = Mw->end = 0 ;
		Mw->UX = Mw->res = Mw->cap = Mw->Tw = Mw->Told = NULL ;
		Mw->uo = Mw->um = Mw->Pc = 0.0 ;
	}
	/************************************************/

	R->Room = NULL ;
	R->Qetotal.name = NULL ;
	/************************************
	B = R->Room ;
	for ( i = 0; i < ROOMMX; i++, B++ )
	{
		B->name = NULL ;
		B->N = B->brs = B->Nachr = B->Ntr = B->Nrp = 0 ;
		B->Nisidermpnl = B->Nasup = 0 ;
		B->rsrf = NULL ;
		B->achr = NULL ;
		B->trnx = NULL ;
		B->rmpnl = NULL ;
		B->arsp = NULL ;
		B->cmp = NULL ;
		B->elinasup = NULL ;
		B->elinasupx = NULL ;
		B->rmld = NULL ;
		B->rmqe = NULL ;
		B->F = NULL ;
		B->alr = NULL ;
		B->XA = NULL ;
		B->Wradx = NULL ;
		B->rsrnx = 'n' ;
		B->fij = B->sfpri = B->eqpri = ' ' ;
		B->mrk = ' ' ;
		B->VRM = B->GRM = B->MRM = B->Area = B->FArea = 0.0 ;
		B->flrsr = 0.3 ;
		B->alrbold = B->Hcap = B->Mxcap = 0.0 ;
		B->Ltyp = ' ' ;
		B->Nhm = B->Light = B->Apsc = B->Apsr = B->Apl = 0.0 ;
		B->Gve = B->Gvi = 0.0 ;
		B->alc = NULL ;
		B->vesc = B->visc = B->lgtsc = B->hmnsc = B->hmwksc = 0 ;
		B->apssc = B->aplsc = 0 ;
		B->metsc = B->closc = B->wvsc = -1 ;
		B->Hc = B->Hr = B->HL = B->Lc = B->Lr = B->Ac = B->Ar = 0.0 ;
		B->AL = 0.0 ;
		B->eqcv = 0.5 ;
		B->Qeqp = B->Gvent = B->RMt = 0.0 ;
		B->ARN = NULL ;
		B->RMP = NULL ;
		B->RMC = B->RMx = B->RMXC = B->Tr = B->Trold = B->xr = 0.0 ;
		B->xrold = B->RH = B->Tsav = B->Tot = B->PMV = 0.0 ;
		B->end = 0 ;
	}
	*******************************************/
}

void	VPTRinit ( VPTR *v )
{
	v->type = ' ' ;
	v->ptr = NULL ;
}

void	TMDTinit ( TMDT *t )
{
	int		i ;

	for ( i = 0; i < 5; i++ )
		t->dat[i] = NULL ;

	strcpy ( t->year, "" ) ;
	strcpy ( t->mon, "" ) ;
	strcpy ( t->day, "" ) ;
	strcpy ( t->wkday, "" ) ;
	strcpy ( t->time, "" ) ;

	t->Year = t->Mon = t->Day = t->Time = 0 ;
}

void	Locinit ( LOCAT	*L )
{
	L->name = NULL ;
	L->Lat = L->Lon = L->Ls = L->Tgrav = L->DTgr = -999.0 ;
	L->daymxert = -999 ;

	matinitx ( L->Twsup, 12, -999.0 ) ;
}

void	Floutinit ( FLOUT *f )
{
	int		i ;

	for ( i = 0; i < FLOUTMX; i++, f++ )
	{
		f->f = NULL ;
		f->idn = NULL ;
	}
}

void	Eqcatinit ( EQCAT *E )
{
	E->Nrfcmp = E->Nhccca = E->Nboica = E->Nrefaca = E->Ncollca = E->Npipeca = E->Nthexca
		= E->Nstankca = E->Nhexca = E->Npumpca = E->Nvavca = E->Nstheatca = 0 ;
	E->Npvca = 0 ;
	E->Npfcmp = 0 ;
	E->Nomvavca = 0 ;

	E->Rfcmp = NULL ;
	E->Hccca = NULL ;
	E->Boica = NULL ;
	E->Refaca = NULL ;
	E->Collca = NULL ;
	E->Pipeca = NULL ;
	E->Stankca = NULL ;
	E->Hexca = NULL ;
	E->Pumpca = NULL ;
	E->Vavca = NULL ;
	E->stheatca = NULL ;
	E->Thexca = NULL ;
	E->pfcmp = NULL ;
	E->PVca = NULL ;
	E->OMvavca = NULL ;
}

void	Schdlinit ( SCHDL *S )
{
	S->Nsch = S->Nscw = 0 ;
	S->Seasn = NULL ;
	S->Wkdy = NULL ;
	S->Dsch = NULL ;
	S->Dscw = NULL ;
	S->Sch = S->Scw = NULL ;
	S->val = NULL ;
	S->isw = NULL ;
}

void	mtEdayinit( EDAY mtEday[12][24] )
{
	int	Mo, Tt ;

	for ( Mo = 0; Mo < 12; Mo++ )
	{
		for ( Tt = 0; Tt < 24; Tt++ )
		{
			mtEday[Mo][Tt].d = 0.0 ;
			mtEday[Mo][Tt].hrs = 0 ;
			mtEday[Mo][Tt].mx = 0.0 ;
			mtEday[Mo][Tt].mxtime = 0 ;
		}
	}

}
