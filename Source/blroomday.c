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

/* roomday.c */

#include "fesy.h"
#include "fnesy.h"
#include "fnfio.h"
#include "fnlib.h"

void Roomday(int Mon, int Day, int Nday, int ttmm, int Nroom, ROOM *Rm, int Nrdpnl, RDPNL *Rdp, int Simdayend)
{
	ROOM   *Room;
	RDPNL  *Rdpnl;
	RMLOAD *R;   
	AIRSUP *A;
	int i, j;
	static int oldday = -999 ;
	static int oldMon = -999 ;
	RMSRF	*Sd ;
	int Mo, tt ;
	void svmonsum(int Mon, int Day, int time, char control, double v, SVDAY *vd, int Dayend, int SimDayend) ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	
	// 日集計
	if (Nday != oldday)
	{
		Room = Rm;
		for (i = 0; i < Nroom; i++, Room++)
		{
			svdyint(&Room->Trdy);
			svdyint(&Room->xrdy);
			svdyint(&Room->RHdy);
			svdyint(&Room->Tsavdy);
			
			if ((R = Room->rmld) != NULL)
			{
				qdyint(&R->Qdys);
				qdyint(&R->Qdyl);
				qdyint(&R->Qdyt);	    
			}
			if ((A = Room->arsp) != NULL)
			{
				for (j = 0; j < Room->Nasup; j++, A++)
				{
					qdyint(&A->Qdys);
					qdyint(&A->Qdyl);
					qdyint(&A->Qdyt);	    
				}
			}
			
			Sd = Room->rsrf ;
			for (j = 0; j < Room->N; j++, Sd++ )
			{
				qdyint(&Sd->SQi);
				svdyint(&Sd->Tsdy);
			}
		}
		
		Rdpnl = Rdp;
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			svdyint(&Rdpnl->Tpody);
			svdyint(&Rdpnl->Tpidy);
			qdyint(&Rdpnl->Qdy);
			qdyint(&Rdpnl->Scoldy);
			svdyint(&Rdpnl->TPVdy);
			qdyint(&Rdpnl->PVdy);
		}

		oldday = Nday;
	}

	// 月集計
	if (Mon != oldMon)
	{
		//printf("リセット\n") ;
		Room = Rm;
		for (i = 0; i < Nroom; i++, Room++)
		{
			svdyint(&Room->mTrdy);
			svdyint(&Room->mxrdy);
			svdyint(&Room->mRHdy);
			svdyint(&Room->mTsavdy);
			
			if ((R = Room->rmld) != NULL)
			{
				qdyint(&R->mQdys);
				qdyint(&R->mQdyl);
				qdyint(&R->mQdyt);	    
			}
			if ((A = Room->arsp) != NULL)
			{
				for (j = 0; j < Room->Nasup; j++, A++)
				{
					qdyint(&A->mQdys);
					qdyint(&A->mQdyl);
					qdyint(&A->mQdyt);	    
				}
			}
			
			Sd = Room->rsrf ;
			for (j = 0; j < Room->N; j++, Sd++ )
			{
				qdyint(&Sd->mSQi);
				svdyint(&Sd->mTsdy);
			}
		}
		
		Rdpnl = Rdp;
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			svdyint(&Rdpnl->mTpody);
			svdyint(&Rdpnl->mTpidy);
			qdyint(&Rdpnl->mQdy);
			qdyint(&Rdpnl->mScoldy);
			svdyint(&Rdpnl->mTPVdy);
			qdyint(&Rdpnl->mPVdy);
		}

		oldMon = Mon;
	}
	
	// 日集計
	Room = Rm;
	for (i = 0; i < Nroom; i++, Room++)
	{
		svdaysum(ttmm, ON_SW, Room->Tr, &Room->Trdy);
		svdaysum(ttmm, ON_SW, Room->xr, &Room->xrdy);
		svdaysum(ttmm, ON_SW, Room->RH, &Room->RHdy);
		svdaysum(ttmm, ON_SW, Room->Tsav, &Room->Tsavdy);
      	 
		if ((R = Room->rmld) != NULL)
		{
			qdaysum(ttmm, ON_SW, R->Qs, &R->Qdys);
			qdaysum(ttmm, ON_SW, R->Ql, &R->Qdyl);
			qdaysum(ttmm, ON_SW, R->Qt, &R->Qdyt);
		}
		if ((A = Room->arsp) != NULL)
		{	          	    
			for (j = 0; j < Room->Nasup; j++, A++)
			{
				qdaysum(ttmm, ON_SW, A->Qs, &A->Qdys);
				qdaysum(ttmm, ON_SW, A->Ql, &A->Qdyl);
				qdaysum(ttmm, ON_SW, A->Qt, &A->Qdyt);
			}
		}

		Sd = Room->rsrf ;
		for (j = 0; j < Room->N; j++, Sd++)
		{
			svdaysum(ttmm, ON_SW, Sd->Ts, &Sd->Tsdy);
			qdaysum(ttmm, ON_SW, Sd->Qi, &Sd->SQi ) ;
		}
	}
	Rdpnl = Rdp;
	for (i = 0; i < Nrdpnl; i++, Rdpnl++)
	{
		char control ;

		svdaysum(ttmm,Rdpnl->cmp->control, Rdpnl->Tpo, &Rdpnl->Tpody);
		svdaysum(ttmm, Rdpnl->cmp->control, Rdpnl->Tpi, &Rdpnl->Tpidy);
		qdaysum(ttmm, Rdpnl->cmp->control, Rdpnl->Q, &Rdpnl->Qdy);
		qdaysumNotOpe(ttmm, Rdpnl->sd[0]->Iwall * Rdpnl->sd[0]->A, &Rdpnl->Scoldy);

		control = OFF_SW ;
		if (Rdpnl->sd[0]->PVwall.Power > 0.)
			control = ON_SW ;

		svdaysum(ttmm, control, Rdpnl->sd[0]->PVwall.TPV, &Rdpnl->TPVdy);
		qdaysumNotOpe(ttmm, Rdpnl->sd[0]->PVwall.Power, &Rdpnl->PVdy);
	}

	// 月集計
	Room = Rm;
	//printf("Mon=%d Day=%d ttmm=%d\n", Mon, Day, ttmm ) ;
	for (i = 0; i < Nroom; i++, Room++)
	{
		svmonsum(Mon, Day, ttmm, ON_SW, Room->Tr, &Room->mTrdy, Nday, Simdayend);
		svmonsum(Mon, Day, ttmm, ON_SW, Room->xr, &Room->mxrdy, Nday, Simdayend);
		svmonsum(Mon, Day, ttmm, ON_SW, Room->RH, &Room->mRHdy, Nday, Simdayend);
		svmonsum(Mon, Day, ttmm, ON_SW, Room->Tsav, &Room->mTsavdy, Nday, Simdayend);
      	 
		if ((R = Room->rmld) != NULL)
		{
			qmonsum(Mon, Day, ttmm, ON_SW, R->Qs, &R->mQdys, Nday, Simdayend);
			qmonsum(Mon, Day, ttmm, ON_SW, R->Ql, &R->mQdyl, Nday, Simdayend);
			qmonsum(Mon, Day, ttmm, ON_SW, R->Qt, &R->mQdyt, Nday, Simdayend);
		}
		if ((A = Room->arsp) != NULL)
		{	          	    
			for (j = 0; j < Room->Nasup; j++, A++)
			{
				qmonsum(Mon, Day, ttmm, ON_SW, A->Qs, &A->mQdys, Nday, Simdayend);
				qmonsum(Mon, Day, ttmm, ON_SW, A->Ql, &A->mQdyl, Nday, Simdayend);
				qmonsum(Mon, Day, ttmm, ON_SW, A->Qt, &A->mQdyt, Nday, Simdayend);
			}
		}

		Sd = Room->rsrf ;
		for (j = 0; j < Room->N; j++, Sd++)
		{
			svmonsum(Mon, Day, ttmm, ON_SW, Sd->Ts, &Sd->mTsdy, Nday, Simdayend);
			qmonsum(Mon, Day, ttmm, ON_SW, Sd->Qi, &Sd->mSQi, Nday, Simdayend ) ;
		}
	}
	Rdpnl = Rdp;
	for (i = 0; i < Nrdpnl; i++, Rdpnl++)
	{
		char control ;

		svmonsum(Mon, Day, ttmm,Rdpnl->cmp->control, Rdpnl->Tpo, &Rdpnl->mTpody, Nday, Simdayend);
		svmonsum(Mon, Day, ttmm, Rdpnl->cmp->control, Rdpnl->Tpi, &Rdpnl->mTpidy, Nday, Simdayend);
		qmonsum(Mon, Day, ttmm, Rdpnl->cmp->control, Rdpnl->Q, &Rdpnl->mQdy, Nday, Simdayend);
		qmonsumNotOpe(Mon, Day, ttmm, Rdpnl->sd[0]->Iwall * Rdpnl->sd[0]->A, &Rdpnl->mScoldy, Nday, Simdayend);

		control = OFF_SW ;
		if (Rdpnl->sd[0]->PVwall.Power > 0.)
			control = ON_SW ;

		svmonsum(Mon, Day, ttmm, control, Rdpnl->sd[0]->PVwall.TPV, &Rdpnl->mTPVdy, Nday, Simdayend);
		qmonsumNotOpe(Mon, Day, ttmm, Rdpnl->sd[0]->PVwall.Power, &Rdpnl->mPVdy, Nday, Simdayend);

		// 月・時刻のクロス集計
		emtsum(Mon, Day, ttmm, control, Rdpnl->sd[0]->PVwall.Power, &Rdpnl->mtPVdy[Mo][tt]);
	}
}

/* ------------------------------------------------------- */

void Rmdyprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, int Nroom, ROOM *Rm)
{
	ROOM   *Room;
	RPANEL *rpnl;
	RMLOAD *R;
	AIRSUP *A;
	ELIN   *Ei;
	int    i, j, Nload;
	static int id = 0;
	
	if (id == 0 && Nroom > 0)
	{
		id++ ;
		Room = Rm;
		
		ttldyprint(fo, mrk, Simc);
		fprintf(fo, "-cat\n");
		fprintf(fo, "%s %d\n", ROOM_TYPE, Nroom);
		for (i = 0; i < Nroom; i++, Room++)
		{
			if (Room->rmld != NULL)
				Nload = 24;
			else
				Nload = 0;
			fprintf(fo, " %s 5 %d 24 %d %d %d\n", Room->name, 
				24 + Nload + 6*Room->Nasup + 2*Room->Nrp,
				Nload, 6*Room->Nasup, 2*Room->Nrp);
		}
		fprintf(fo, "*\n#\n");	 
	}
	
	if (id == 1 && Nroom > 0)
	{
		id++ ;
		Room = Rm;
		
		for (i = 0; i < Nroom; i++, Room++)
		{
			fprintf(fo, "%s_Ht H d %s_Tr T f %s_ttn h d %s_Trn t f %s_ttm h d %s_Trm t f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			fprintf(fo,  "%s_Hx H d %s_xr X f %s_txn h d %s_xrn x f %s_txm h d %s_xrm x f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			fprintf(fo,  "%s_Hr H d %s_RH R f %s_trn h d %s_RHn r f %s_trm h d %s_RHm r f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			fprintf(fo, "%s_Hs H d %s_Ts T f %s_tsn h d %s_Tsn t f %s_tsm h d %s_Tsm t f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			
			if (Room->rmld != NULL)
			{
				fprintf(fo, "%s_Hsh H d %s_Lsh Q f %s_Hsc H d %s_Lsc Q f ",
					Room->name, Room->name, Room->name, Room->name);
				fprintf(fo, "%s_tsh h d %s_Lqsh q f %s_tsc h d %s_Lqsc q f\n", 
					Room->name, Room->name, Room->name, Room->name);
				
				fprintf(fo, "%s_Hlh H d %s_Llh Q f %s_Hlc H d %s_Llc Q f ",
					Room->name, Room->name, Room->name, Room->name);
				fprintf(fo, "%s_tlh h d %s_Lqlh q f %s_tlc h d %s_Lqlc q f\n", 
					Room->name, Room->name, Room->name, Room->name);
				
				fprintf(fo, "%s_Hth H d %s_Lth Q f %s_Htc H d %s_Ltc Q f ",
					Room->name, Room->name, Room->name, Room->name);		    
				fprintf(fo, "%s_tth h d %s_Lqth q f %s_ttc h d %s_Lqtc q f\n", 
					Room->name, Room->name, Room->name, Room->name);
			}
			
			if (Room->Nasup)
			{
				Ei = Room->cmp->elins;
				Ei += Room->Nachr + Room->Nrp;
				for (j = 0; j < Room->Nasup; j++, Ei++)
				{
					if (Ei->lpath == NULL)
					{
						fprintf(fo, "%s:%d_Qash Q f %s:%d_Qasc Q f ", 
							Room->name, j, Room->name, j);
						fprintf(fo, "%s:%d_Qalh Q f %s:%d_Qalc Q f ", 
							Room->name, j, Room->name, j);
						fprintf(fo, "%s:%d_Qath Q f %s:%d_Qatc Q f\n", 
							Room->name, j, Room->name, j);
					}
					else
					{
						fprintf(fo, "%s:%s_Qash Q f %s:%s_Qasc Q f ",
							Room->name, Ei->lpath->name, Room->name, Ei->lpath->name);
						fprintf(fo, "%s:%s_Qalh Q f %s:%s_Qalc Q f ",
							Room->name, Ei->lpath->name, Room->name, Ei->lpath->name);
						fprintf(fo, "%s:%s_Qath Q f %s:%s_Qatc Q f\n",
							Room->name, Ei->lpath->name, Room->name, Ei->lpath->name);
					}
				}
			}
			rpnl = Room->rmpnl;
			for (j = 0; j < Room->Nrp; j++, rpnl++)
				fprintf(fo, "%s:%s_Qh Q f %s:%s_Qc Q f ", Room->name, rpnl->pnl->name, 
				Room->name, rpnl->pnl->name);
			fprintf(fo, "\n");
		} 
	}
	Room = Rm;
	
	fprintf(fo, "%02d %02d\n", mon, day);
	
	for (i = 0; i < Nroom; i++, Room++)
	{
		fprintf(fo, "%1ld %4.2lf %1ld %4.2lf %1ld %4.2lf ", 
			Room->Trdy.hrs, Room->Trdy.m, Room->Trdy.mntime, 
			Room->Trdy.mn, Room->Trdy.mxtime, Room->Trdy.mx);
		fprintf(fo, "%1ld %5.4lf %1ld %5.4lf %1ld %5.4lf\n", 
			Room->xrdy.hrs, Room->xrdy.m, Room->xrdy.mntime, 
			Room->xrdy.mn, Room->xrdy.mxtime, Room->xrdy.mx);
		fprintf(fo, "%1ld %2.0lf %1ld %2.0lf %1ld %2.0lf ", 
			Room->RHdy.hrs, Room->RHdy.m, Room->RHdy.mntime, 
			Room->RHdy.mn, Room->RHdy.mxtime, Room->RHdy.mx);
		fprintf(fo, "%1ld %4.2lf %1ld %4.2lf %1ld %4.2lf\n", 
			Room->Tsavdy.hrs, Room->Tsavdy.m, Room->Tsavdy.mntime, 
			Room->Tsavdy.mn, Room->Tsavdy.mxtime, Room->Tsavdy.mx);
		
		if ((R = Room->rmld) != NULL)
		{
			fprintf(fo, "%1ld %.2lf ", R->Qdys.hhr, R->Qdys.h);
			fprintf(fo, "%1ld %.2lf ", R->Qdys.chr, R->Qdys.c);
			fprintf(fo, "%4ld %2.0lf ", R->Qdys.hmxtime, R->Qdys.hmx);
			fprintf(fo, "%4ld %2.0lf ", R->Qdys.cmxtime, R->Qdys.cmx);
			
			fprintf(fo, "%1ld %.2lf ", R->Qdyl.hhr, R->Qdyl.h);
			fprintf(fo, "%1ld %.2lf ", R->Qdyl.chr, R->Qdyl.c);
			fprintf(fo, "%4ld %2.0lf ", R->Qdyl.hmxtime, R->Qdyl.hmx);
			fprintf(fo, "%4ld %2.0lf ", R->Qdyl.cmxtime, R->Qdyl.cmx);
			
			fprintf(fo, "%1ld %.2lf ", R->Qdyt.hhr, R->Qdyt.h);
			fprintf(fo, "%1ld %.2lf ", R->Qdyt.chr, R->Qdyt.c);
			fprintf(fo, "%4ld %2.0lf ", R->Qdyt.hmxtime, R->Qdyt.hmx);
			fprintf(fo, "%4ld %2.0lf\n", R->Qdyt.cmxtime, R->Qdyt.cmx);
		}	 
		if (Room->Nasup)
		{	 
			A = Room->arsp;
			for (j = 0; j < Room->Nasup; j++, A++)
			{
				fprintf(fo, "%3.1lf %.2lf ", A->Qdys.h,  A->Qdys.c);
				fprintf(fo, "%3.1lf %.2lf ", A->Qdyl.h,  A->Qdyl.c);
				fprintf(fo, "%3.1lf %.2lf ", A->Qdyt.h,  A->Qdyt.c);
			}
			fprintf(fo, "\n");	 
		}
		if (Room->Nrp)
		{
			rpnl = Room->rmpnl;
			for (j = 0; j < Room->Nrp; j++, rpnl++)
				fprintf(fo, "%.2lf %.2lf\n", -rpnl->pnl->Qdy.c, -rpnl->pnl->Qdy.h);
		}
		fprintf(fo, "\n");
	} 
}

// 月集計結果の出力
void Rmmonprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, int Nroom, ROOM *Rm)
{
	ROOM   *Room;
	RPANEL *rpnl;
	RMLOAD *R;
	AIRSUP *A;
	ELIN   *Ei;
	int    i, j, Nload;
	static int id = 0;
	
	if (id == 0 && Nroom > 0)
	{
		id++ ;
		Room = Rm;
		
		ttldyprint(fo, mrk, Simc);
		fprintf(fo, "-cat\n");
		fprintf(fo, "%s %d\n", ROOM_TYPE, Nroom);
		for (i = 0; i < Nroom; i++, Room++)
		{
			if (Room->rmld != NULL)
				Nload = 24;
			else
				Nload = 0;
			fprintf(fo, " %s 5 %d 24 %d %d %d\n", Room->name, 
				24 + Nload + 6*Room->Nasup + 2*Room->Nrp,
				Nload, 6*Room->Nasup, 2*Room->Nrp);
		}
		fprintf(fo, "*\n#\n");	 
	}
	
	if (id == 1 && Nroom > 0)
	{
		id++ ;
		Room = Rm;
		
		for (i = 0; i < Nroom; i++, Room++)
		{
			fprintf(fo, "%s_Ht H d %s_Tr T f %s_ttn h d %s_Trn t f %s_ttm h d %s_Trm t f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			fprintf(fo,  "%s_Hx H d %s_xr X f %s_txn h d %s_xrn x f %s_txm h d %s_xrm x f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			fprintf(fo,  "%s_Hr H d %s_RH R f %s_trn h d %s_RHn r f %s_trm h d %s_RHm r f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			fprintf(fo, "%s_Hs H d %s_Ts T f %s_tsn h d %s_Tsn t f %s_tsm h d %s_Tsm t f\n",
				Room->name, Room->name, Room->name, Room->name, Room->name, Room->name);
			
			if (Room->rmld != NULL)
			{
				fprintf(fo, "%s_Hsh H d %s_Lsh Q f %s_Hsc H d %s_Lsc Q f ",
					Room->name, Room->name, Room->name, Room->name);
				fprintf(fo, "%s_tsh h d %s_Lqsh q f %s_tsc h d %s_Lqsc q f\n", 
					Room->name, Room->name, Room->name, Room->name);
				
				fprintf(fo, "%s_Hlh H d %s_Llh Q f %s_Hlc H d %s_Llc Q f ",
					Room->name, Room->name, Room->name, Room->name);
				fprintf(fo, "%s_tlh h d %s_Lqlh q f %s_tlc h d %s_Lqlc q f\n", 
					Room->name, Room->name, Room->name, Room->name);
				
				fprintf(fo, "%s_Hth H d %s_Lth Q f %s_Htc H d %s_Ltc Q f ",
					Room->name, Room->name, Room->name, Room->name);		    
				fprintf(fo, "%s_tth h d %s_Lqth q f %s_ttc h d %s_Lqtc q f\n", 
					Room->name, Room->name, Room->name, Room->name);
			}
			
			if (Room->Nasup)
			{
				Ei = Room->cmp->elins;
				Ei += Room->Nachr + Room->Nrp;
				for (j = 0; j < Room->Nasup; j++, Ei++)
				{
					if (Ei->lpath == NULL)
					{
						fprintf(fo, "%s:%d_Qash Q f %s:%d_Qasc Q f ", 
							Room->name, j, Room->name, j);
						fprintf(fo, "%s:%d_Qalh Q f %s:%d_Qalc Q f ", 
							Room->name, j, Room->name, j);
						fprintf(fo, "%s:%d_Qath Q f %s:%d_Qatc Q f\n", 
							Room->name, j, Room->name, j);
					}
					else
					{
						fprintf(fo, "%s:%s_Qash Q f %s:%s_Qasc Q f ",
							Room->name, Ei->lpath->name, Room->name, Ei->lpath->name);
						fprintf(fo, "%s:%s_Qalh Q f %s:%s_Qalc Q f ",
							Room->name, Ei->lpath->name, Room->name, Ei->lpath->name);
						fprintf(fo, "%s:%s_Qath Q f %s:%s_Qatc Q f\n",
							Room->name, Ei->lpath->name, Room->name, Ei->lpath->name);
					}
				}
			}
			rpnl = Room->rmpnl;
			for (j = 0; j < Room->Nrp; j++, rpnl++)
				fprintf(fo, "%s:%s_Qh Q f %s:%s_Qc Q f ", Room->name, rpnl->pnl->name, 
				Room->name, rpnl->pnl->name);
			fprintf(fo, "\n");
		} 
	}
	Room = Rm;
	
	fprintf(fo, "%02d %02d\n", mon, day);
	
	for (i = 0; i < Nroom; i++, Room++)
	{
		fprintf(fo, "%1ld %4.2lf %1ld %4.2lf %1ld %4.2lf ", 
			Room->mTrdy.hrs, Room->mTrdy.m, Room->mTrdy.mntime, 
			Room->mTrdy.mn, Room->mTrdy.mxtime, Room->mTrdy.mx);
		fprintf(fo, "%1ld %5.4lf %1ld %5.4lf %1ld %5.4lf\n", 
			Room->mxrdy.hrs, Room->mxrdy.m, Room->mxrdy.mntime, 
			Room->mxrdy.mn, Room->mxrdy.mxtime, Room->mxrdy.mx);
		fprintf(fo, "%1ld %2.0lf %1ld %2.0lf %1ld %2.0lf ", 
			Room->mRHdy.hrs, Room->mRHdy.m, Room->mRHdy.mntime, 
			Room->mRHdy.mn, Room->mRHdy.mxtime, Room->mRHdy.mx);
		fprintf(fo, "%1ld %4.2lf %1ld %4.2lf %1ld %4.2lf\n", 
			Room->mTsavdy.hrs, Room->mTsavdy.m, Room->mTsavdy.mntime, 
			Room->mTsavdy.mn, Room->mTsavdy.mxtime, Room->mTsavdy.mx);
		
		if ((R = Room->rmld) != NULL)
		{
			fprintf(fo, "%1ld %3.1lf ", R->mQdys.hhr, R->mQdys.h);
			fprintf(fo, "%1ld %3.1lf ", R->mQdys.chr, R->mQdys.c);
			fprintf(fo, "%4ld %2.0lf ", R->mQdys.hmxtime, R->mQdys.hmx);
			fprintf(fo, "%4ld %2.0lf ", R->mQdys.cmxtime, R->mQdys.cmx);
			
			fprintf(fo, "%1ld %3.1lf ", R->mQdyl.hhr, R->mQdyl.h);
			fprintf(fo, "%1ld %3.1lf ", R->mQdyl.chr, R->mQdyl.c);
			fprintf(fo, "%4ld %2.0lf ", R->mQdyl.hmxtime, R->mQdyl.hmx);
			fprintf(fo, "%4ld %2.0lf ", R->mQdyl.cmxtime, R->mQdyl.cmx);
			
			fprintf(fo, "%1ld %3.1lf ", R->mQdyt.hhr, R->mQdyt.h);
			fprintf(fo, "%1ld %3.1lf ", R->mQdyt.chr, R->mQdyt.c);
			fprintf(fo, "%4ld %2.0lf ", R->mQdyt.hmxtime, R->mQdyt.hmx);
			fprintf(fo, "%4ld %2.0lf\n", R->mQdyt.cmxtime, R->mQdyt.cmx);
		}	 
		if (Room->Nasup)
		{	 
			A = Room->arsp;
			for (j = 0; j < Room->Nasup; j++, A++)
			{
				fprintf(fo, "%3.1lf %3.1lf ", A->mQdys.h,  A->mQdys.c);
				fprintf(fo, "%3.1lf %3.1lf ", A->mQdyl.h,  A->mQdyl.c);
				fprintf(fo, "%3.1lf %3.1lf ", A->mQdyt.h,  A->mQdyt.c);
			}
			fprintf(fo, "\n");	 
		}
		if (Room->Nrp)
		{
			rpnl = Room->rmpnl;
			for (j = 0; j < Room->Nrp; j++, rpnl++)
				fprintf(fo, "%3.1lf %3.1lf\n", -rpnl->pnl->mQdy.c, -rpnl->pnl->mQdy.h);
		}
		fprintf(fo, "\n");
	} 
}

/* -------------------------------------------- */


void paneldyprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nrdpnl > 0)
            fprintf(fo, "%s %d\n", RDPANEL_TYPE, Nrdpnl);
		
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			WALL	*Wall ;
			Wall = Rdpnl->sd[0]->mw->wall ;

			if (Wall->WallType == 'P')
				fprintf(fo, " %s 1 20\n", Rdpnl->name);
			else
			{
				if (Rdpnl->sd[0]->PVwallFlg != 'Y' )
					fprintf(fo, " %s 1 28\n", Rdpnl->name);
				else
					fprintf(fo, " %s 1 36\n", Rdpnl->name) ;
			}
		}
		break;
		
	case 1:   
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			WALL	*Wall ;
			Wall = Rdpnl->sd[0]->mw->wall ;

			fprintf(fo,"%s_Hto H d %s_To T f ", Rdpnl->name, Rdpnl->name);
			fprintf(fo,"%s_ttno h d %s_Ton t f %s_ttm h d %s_Tom t f ",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			fprintf(fo,"%s_Ht H d %s_T T f ", Rdpnl->name, Rdpnl->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f ",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);

			if (Wall->WallType == 'C')
			{
				fprintf(fo, "%s_ScolHh H d %s_ScolQh Q f %s_ScolHc H d %s_ScolQc Q f",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
				fprintf(fo, "%s_Scolth h d %s_Scolqh q f %s_Scoltc h d %s_Scolqc q f\n",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);

				if (Rdpnl->sd[0]->PVwallFlg == 'Y')
				{
					fprintf(fo,"%s_PVHt H d %s_TPV T f ", Rdpnl->name, Rdpnl->name);
					fprintf(fo,"%s_PVttn h d %s_TPVn t f %s_PVttm h d %s_TPVm t f ",
						Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
					fprintf(fo, "%s_PVH h d %s_E E f\n", Rdpnl->name, Rdpnl->name);
				}
			}
		}
		break;
		
	default:
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{   
			WALL	*Wall ;
			Wall = Rdpnl->sd[0]->mw->wall ;

            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Rdpnl->Tpody.hrs, Rdpnl->Tpody.m, Rdpnl->Tpody.mntime, 
				Rdpnl->Tpody.mn, Rdpnl->Tpody.mxtime, Rdpnl->Tpody.mx);
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Rdpnl->Tpidy.hrs, Rdpnl->Tpidy.m, Rdpnl->Tpidy.mntime, 
				Rdpnl->Tpidy.mn, Rdpnl->Tpidy.mxtime, Rdpnl->Tpidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Rdpnl->Qdy.hhr, Rdpnl->Qdy.h);
            fprintf(fo, "%1ld %3.1lf ", Rdpnl->Qdy.chr, Rdpnl->Qdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Rdpnl->Qdy.hmxtime, Rdpnl->Qdy.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Rdpnl->Qdy.cmxtime, Rdpnl->Qdy.cmx);

			if (Wall->WallType == 'C')
			{
				fprintf(fo, "%1ld %3.1lf ", Rdpnl->Scoldy.hhr, Rdpnl->Scoldy.h);
				fprintf(fo, "%1ld %3.1lf ", Rdpnl->Scoldy.chr, Rdpnl->Scoldy.c);      
				fprintf(fo, "%1ld %2.0lf ", Rdpnl->Scoldy.hmxtime, Rdpnl->Scoldy.hmx);
				fprintf(fo, "%1ld %2.0lf\n", Rdpnl->Scoldy.cmxtime, Rdpnl->Scoldy.cmx);

				if (Rdpnl->sd[0]->PVwallFlg == 'Y')
				{
					fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
						Rdpnl->TPVdy.hrs, Rdpnl->TPVdy.m, Rdpnl->TPVdy.mntime, 
						Rdpnl->TPVdy.mn, Rdpnl->TPVdy.mxtime, Rdpnl->TPVdy.mx);
					fprintf(fo, "%1ld %.1lf\n", Rdpnl->PVdy.hhr, Rdpnl->PVdy.h) ;
				}
			}
		}
		break;
	}
}

void panelmonprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nrdpnl > 0)
            fprintf(fo, "%s %d\n", RDPANEL_TYPE, Nrdpnl);
		
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			WALL	*Wall ;
			Wall = Rdpnl->sd[0]->mw->wall ;

			if (Wall->WallType == 'P')
				fprintf(fo, " %s 1 20\n", Rdpnl->name);
			else
			{
				if (Rdpnl->sd[0]->PVwallFlg != 'Y' )
					fprintf(fo, " %s 1 28\n", Rdpnl->name);
				else
					fprintf(fo, " %s 1 36\n", Rdpnl->name) ;
			}
		}
		break;
		
	case 1:   
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			WALL	*Wall ;
			Wall = Rdpnl->sd[0]->mw->wall ;

			fprintf(fo,"%s_Hto H d %s_To T f ", Rdpnl->name, Rdpnl->name);
			fprintf(fo,"%s_ttno h d %s_Ton t f %s_ttm h d %s_Tom t f ",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			fprintf(fo,"%s_Ht H d %s_T T f ", Rdpnl->name, Rdpnl->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f ",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);

			if (Wall->WallType == 'C')
			{
				fprintf(fo, "%s_ScolHh H d %s_ScolQh Q f %s_ScolHc H d %s_ScolQc Q f",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
				fprintf(fo, "%s_Scolth h d %s_Scolqh q f %s_Scoltc h d %s_Scolqc q f\n",
					Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);

				if (Rdpnl->sd[0]->PVwallFlg == 'Y')
				{
					fprintf(fo,"%s_PVHt H d %s_TPV T f ", Rdpnl->name, Rdpnl->name);
					fprintf(fo,"%s_PVttn h d %s_TPVn t f %s_PVttm h d %s_TPVm t f ",
						Rdpnl->name, Rdpnl->name, Rdpnl->name, Rdpnl->name);
					fprintf(fo, "%s_PVH h d %s_E E f\n", Rdpnl->name, Rdpnl->name);
				}
			}
		}
		break;
		
	default:
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{   
			WALL	*Wall ;
			Wall = Rdpnl->sd[0]->mw->wall ;

            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Rdpnl->mTpody.hrs, Rdpnl->mTpody.m, Rdpnl->mTpody.mntime, 
				Rdpnl->mTpody.mn, Rdpnl->mTpody.mxtime, Rdpnl->mTpody.mx);
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Rdpnl->mTpidy.hrs, Rdpnl->mTpidy.m, Rdpnl->mTpidy.mntime, 
				Rdpnl->mTpidy.mn, Rdpnl->mTpidy.mxtime, Rdpnl->mTpidy.mx);
            fprintf(fo, "%1ld %3.1lf ", Rdpnl->mQdy.hhr, Rdpnl->mQdy.h);
            fprintf(fo, "%1ld %3.1lf ", Rdpnl->mQdy.chr, Rdpnl->mQdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Rdpnl->mQdy.hmxtime, Rdpnl->mQdy.hmx);
            fprintf(fo, "%1ld %2.0lf\n", Rdpnl->mQdy.cmxtime, Rdpnl->mQdy.cmx);

			if (Wall->WallType == 'C')
			{
				fprintf(fo, "%1ld %3.1lf ", Rdpnl->mScoldy.hhr, Rdpnl->mScoldy.h);
				fprintf(fo, "%1ld %3.1lf ", Rdpnl->mScoldy.chr, Rdpnl->mScoldy.c);      
				fprintf(fo, "%1ld %2.0lf ", Rdpnl->mScoldy.hmxtime, Rdpnl->mScoldy.hmx);
				fprintf(fo, "%1ld %2.0lf\n", Rdpnl->mScoldy.cmxtime, Rdpnl->mScoldy.cmx);

				if (Rdpnl->sd[0]->PVwallFlg == 'Y')
				{
					fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
						Rdpnl->mTPVdy.hrs, Rdpnl->mTPVdy.m, Rdpnl->mTPVdy.mntime, 
						Rdpnl->mTPVdy.mn, Rdpnl->mTPVdy.mxtime, Rdpnl->mTPVdy.mx);
					fprintf(fo, "%1ld %.1lf\n", Rdpnl->mPVdy.hhr, Rdpnl->mPVdy.h) ;
				}
			}
		}
		break;
	}
}

void panelmtprt(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl, int Mo, int tt)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nrdpnl > 0)
            fprintf(fo, "%s %d\n", RDPANEL_TYPE, Nrdpnl);
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
			fprintf(fo, " %s 1 1\n", Rdpnl->name);
		break;
		
	case 1:
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{	 
			fprintf(fo,"%s_E E f \n", Rdpnl->name);
		}
		break;
		
	default:
		for (i = 0; i < Nrdpnl; i++, Rdpnl++)
		{
			fprintf(fo, " %.2lf \n", Rdpnl->mtPVdy[Mo-1][tt-1].d * cff_kWh);
		}
		break;
	}
}
