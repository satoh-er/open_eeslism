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

/* xprmcs.c */

#include "fesy.h"
#include "fnmcs.h"

/*  システム使用機器についての検討用出力  */

void mecsxprint(EQSYS *Eqsys)
{
   hccxprint(Eqsys->Nhcc, Eqsys->Hcc);
   boixprint(Eqsys->Nboi, Eqsys->Boi);
   collxprint(Eqsys->Ncoll, Eqsys->Coll);
   refaxprint(Eqsys->Nrefa, Eqsys->Refa);
   pipexprint(Eqsys->Npipe, Eqsys->Pipe);
}

/* --------------------------- */

void boixprint(int Nboi, BOI *Boi)
{
   int i;
   
   if ( Nboi > 0 )
   {
	   printf("%s N=%d\n", BOILER_TYPE, Nboi); 
	   
	   for (i = 0; i < Nboi; i++, Boi++)
		   printf("[%d] %-10s Do=%5.3lf  D1=%5.3lf Tin=%5.2lf Tout=%5.2lf Q=%4.0lf E=%4.0lf\n",
		   i, Boi->name, Boi->Do, Boi->D1, Boi->Tin, 
		   Boi->cmp->elouts->sysv, Boi->Q, Boi->E);
   }
}
/* ------------------------------------------ */
      
void hccxprint(int Nhcc, HCC *Hcc)
{
   int i;
   ACS   E;
   ELOUT *el;
   
   if ( Nhcc > 0 )
   {
	   printf("%s N=%d\n", HCCOIL_TYPE, Nhcc);
	   
	   for (i = 0; i < Nhcc; i++, Hcc++)
	   {
		   printf("[%d] %-10s et=%5.3lf eh=%5.3lf\n", i, Hcc->name, Hcc->et, Hcc->eh);
		   E = Hcc->Et;
		   printf("     Et w=%7.3lf t=%7.3lf x=%7.3lf C=%7.3lf\n", E.w, E.t, E.x, E.C);
		   E = Hcc->Ex;
		   printf("     Et w=%7.3lf t=%7.3lf x=%7.3lf C=%7.3lf\n", E.w, E.t, E.x, E.C);
		   E = Hcc->Ew;
		   printf("     Et w=%7.3lf t=%7.3lf x=%7.3lf C=%7.3lf\n", E.w, E.t, E.x, E.C);
		   el = Hcc->cmp->elouts;
		   printf("     Tain=%5.2lf  Taout=%5.2lf  Qs=%4.0lf\n", Hcc->Tain, el->sysv, Hcc->Qs);
		   el++;
		   printf("     xain=%5.4lf  xaout=%5.4lf  Qs=%4.0lf\n", Hcc->xain, el->sysv, Hcc->Ql);
		   el++;
		   printf("     Wwin=%5.2lf  Twout=%5.4lf  Qt=%4.0lf\n", Hcc->Twin, el->sysv, Hcc->Qt);
	   }
   }
}
      
/* --------------------------- */

void pipexprint(int Npipe, PIPE *Pipe)
{
	int		i;
	double	Te ;
	
	if ( Npipe > 0 )
	{
		printf("%s N=%d\n", PIPEDUCT_TYPE, Npipe );
		
		if ( Pipe->cmp->envname != NULL )
			Te = *(Pipe->Tenv) ;
		else
			Te = Pipe->room->Tot ;
		
		for (i = 0; i < Npipe; i++, Pipe++)
			printf("[%d] %-10s Do=%6.3lf  D1=%6.3lf Tin=%5.2lf Tout=%5.2lf ep=%5.3lf env=%4.1lf Q=%4.0lf\n",
			i, Pipe->name, Pipe->Do, Pipe->D1, Pipe->Tin,
			Pipe->cmp->elouts->sysv, Pipe->ep, Te, Pipe->Q);
	}
}

/* ------------------------------------------------------------- */

void refaxprint(int Nrefa, REFA *Refa)
{
   int i;
   
   if ( Nrefa > 0 )
   {
	   printf("%s N=%d\n", REFACOMP_TYPE, Nrefa);
	   
	   for (i = 0; i < Nrefa; i++, Refa++)
	   {
		   printf("[%d] %-10s Do=%6.3lf D1=%6.3lf Tin=%5.2lf Tout=%5.2lf Ta=%4.1lf\n",
			   i, Refa->name, Refa->Do, Refa->D1, Refa->Tin, 
			   Refa->cmp->elouts->sysv, *Refa->Ta);
		   printf("     Te=%5.2lf  Tc=%5.2lf  Q=%6.0lf E=%6.0lf Ph=%3.0lf\n",
			   Refa->Te, Refa->Tc, Refa->Q, Refa->E, Refa->Ph);
	   }
   }
}

/* ------------------------------------------------------------- */

void collxprint(int Ncoll, COLL *Coll)
{
   int i;
   
   if ( Ncoll > 0 )
   {
	   printf("%s N=%d\n", COLLECTOR_TYPE, Ncoll);
	   
	   for (i = 0; i < Ncoll; i++, Coll++)
	   {
		   printf("[%d] %-10s Do=%6.3lf  D1=%6.3lf Tin=%5.2lf Tout=%5.2lf Q=%4.0lf Sol=%4.0lf Te=%5.1lf\n",
			   i, Coll->name, Coll->Do, Coll->D1, Coll->Tin, 
			   Coll->cmp->elouts->sysv, Coll->Q, Coll->Sol, Coll->Te);
		   printf("   exs=%s  b0=%5.3lf  b1=%5.3lf ec=%5.3lf\n", Coll->sol->name,
			   Coll->cat->b0, Coll->cat->b1, Coll->ec);
	   }
   }
}




