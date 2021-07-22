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

/*   eschdlr_s.c   */

#include <stdlib.h>
#include "MODEL.h"   /*-----higuchi 070918---*/
#include "fesy.h"
#include "fnbld.h"
#include "fnesy.h"
#include "fnlib.h"
#include "fnbldg.h"

/* -------------------------------------------------------------------------- */

void	Eeschdlr(int day, int ttmm, SCHDL *Schdl, RMVLS *Rmvls)
{
	int		j ;
	double	*val ;
	SCH		*Sch, *Scw ;
	char	*isw ;
	extern int	DEBUG ;
	ROOM	*r ;

	val = Schdl->val ;
	Sch = Schdl->Sch ;
	r = Rmvls->Room ;
	for ( j = 0; j < Schdl->Nsch; j++, val++, Sch++ )
	{
		//printf ("j=%d  Name=%s  MAX=%d\n", j, Sch->name, Schdl->Nsch ) ;
		*val = schval ( day, ttmm, Sch, Schdl->Dsch ) ;
		//Schdl->val[j] = schval ( day, ttmm, &Schdl->Sch[j], Schdl->Dsch ) ;
		//printf ("Schdl->val[%d]=%lf\n", j, *val ) ;
	}

	Scw = Schdl->Scw ;
	isw = Schdl->isw ;
	for ( j = 0; j < Schdl->Nscw; j++, Scw++, isw++ )
	{
		//printf ("j=%d  Name=%s  MAX=%d\n", j, Scw->name, Schdl->Nscw ) ;
		*isw = scwmode ( day, ttmm, Scw, Schdl->Dscw ) ;
	}
	
#if SIMUL_BUILDG  
	
	if ( DEBUG )
		xprschval (Schdl->Nsch, Schdl->val, Schdl->Nscw, Schdl->isw);
	
	//printf ( "1--Windowschdlr \n" ) ;
	Windowschdlr(Schdl->isw, Rmvls->Window, Rmvls->Nsrf, Rmvls->Sd);
	//printf("2--Vtschdlr \n");
	Vtschdlr(Rmvls->Nroom, Rmvls->Room);
	//printf("3--Aichschdlr \n");
	// 空調発停が完了したから再設定する 2017/4/19
	//Qischdlr (Rmvls->Nroom, Rmvls->Room);
	//printf ( "4---- Hc=%lf\n", r[1].Hc ) ;
	Aichschdlr (Schdl->val, Rmvls->Nroom, Rmvls->Room);
	//printf ( "5---- Hc=%lf\n", r[1].Hc ) ;
	
	if ( DEBUG )
	{ 
		xprqin (Rmvls->Nroom, Rmvls->Room);
		xprvent(Rmvls->Nroom, Rmvls->Room); 
	}
#endif   
}
