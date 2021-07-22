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

/*   bl_roomcf.c  */

//#define		DEBUG	0
#include <stdio.h>
#include "fesy.h"
#include "fnbld.h"

// 熱伝達率の計算
void eeroomcf(WDAT *Wd, EXSFS *Exs, RMVLS *Rmvls, int nday, int mt)
{
	extern int	DEBUG ;
	//int		i;
	
	//printf("ali=%lf\n", Rmvls->Sd->ali);
	// 熱伝達率の計算

	// 表面熱伝達率（対流・放射））の計算
	Rmhtrcf (Exs, Rmvls->Emrk, Rmvls->Room, Rmvls->Sd, Wd);
	
	if ( DEBUG )
	{
		// 表面熱伝達率の表示
		xpralph (Rmvls->Nroom, Rmvls->Room, Rmvls->Sd);
	}

	//printf("ali=%lf\n", Rmvls->Sd->ali);
	// 熱貫流率の計算
	Rmhtrsmcf(Rmvls->Nsrf, Rmvls->Sd);

	/*****
	printf("eeroomcf 1\n");
	/*******/

	//printf("ali=%lf\n", Rmvls->Sd->ali);
	// 透過日射、相当外気温度の計算
	Rmexct(Rmvls->Room, Rmvls->Nsrf, Rmvls->Sd, Wd, Exs->Exs, 
		Rmvls->Snbk, Rmvls->Qrm, nday, mt);
	
        /*-----higuchi change 070918 ----
	Rmexct(Rmvls->Room, Rmvls->Nsrf, Rmvls->Sd, Wd, Exs->Exs, 
                Rmvls->Snbk, Rmvls->Qrm);
		---------------------------------*/	
	/*****
	printf("eeroomcf 2\n");
	/*********/
	//RMSRF	*Sd;
	//Sd = Rmvls->Sd;
	//for (i = 0; i < 1; i++, Sd++)
	//	printf("ali=%lf\n", Sd->ali);

	// 室の係数（壁体熱伝導等））、定数項の計算
	Roomcf (Rmvls->Nmwall, Rmvls->Mw, Rmvls->Nroom, Rmvls->Room, 
		Rmvls->Nrdpnl, Rmvls->Rdpnl, Wd, Exs);

//	xprroom ( Rmvls->Nroom, Rmvls->Room ) ;
//	xprxas ( Rmvls->Nroom, Rmvls->Room, Rmvls->Sd ) ;
}
