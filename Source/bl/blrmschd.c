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

/*   rmschd.c      */


#include "common.h"
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnesy.h"
#include "fnlib.h"

/* --------------------------------------------- */

/*  窓の熱抵抗の変更 */

void	Windowschdlr(char* isw, WINDOW* Window, int N, RMSRF* Sd)
{
	int		n, j, Nsw;
	WINDOW* W;
	//DIRECT_HEAT_GAIN* dhg;

	for (n = 0; n < N; n++, Sd++)
	{
		if (Sd->ble == 'W')
		{
			// 窓の選択は条件による切り替えが優先される
			// スケジュールによる窓の変更
			//if ( Sd->ifwin == NULL)
			//{
			if ((Nsw = Sd->Nfn) > 1)
				j = iswmode(isw[Sd->fnsw], Nsw, Sd->fnmrk);
			else
				j = 0;

			j = Sd->fnd[j];
			W = Window + j;
			//}
			// 動的な窓の変更
			if (Sd->ifwin != NULL)
			{
				// 条件の確認
				j = contrlif(Sd->ctlif);

				if (j != 0)
					W = Sd->ifwin;
			}

			Sd->Eo = W->Eo;
			Sd->Ei = W->Ei;
			Sd->tgtn = W->tgtn;
			Sd->Bn = W->Bn;
			Sd->Rwall = W->Rwall;
			Sd->CAPwall = 0.0;

			//printf ( "Rwindow=%lf\n", Sd->Rwall ) ;

			Sd->fn = j;
		}
	}
}
/* --------------------------------------------- */

/*  室内発熱の計算    */

void Qischdlr(int Nroom, ROOM* Room)
{
	static double Ht[9] = { 92,106,119,131,145,198,226,264,383 },
		Hs24[9] = { 58, 62, 63, 64, 69, 76, 83, 99,137 },
		d[9] = { 3.5,3.6,4.0,4.2,4.4,6.5,7.0,7.3,6.3 };
	int		i, wk;
	double	N, Q;
	char	s[SCHAR];

	for (i = 0; i < Nroom; i++, Room++)
	{
		Room->Hc = Room->Hr = Room->HL = 0.0;
		Room->Lc = Room->Lr = 0.0;
		Room->Ac = Room->Ar = 0.0;
		Room->AL = 0.0;

		//j = Room->hmnsc ;

		if (DEBUG)
			printf("<Qischdlr> Name=%s\n", Room->name);

		//v = *(val + j) ;
		if (Room->Hmsch != NULL && *Room->Hmsch > 0.0)
		{
			N = Room->Nhm * *Room->Hmsch;

			if (N > 0 && Room->Hmwksch != NULL && *Room->Hmwksch > 0.0)
			{
				wk = (int)* Room->Hmwksch - 1;

				if (wk < 0 || wk > 8)
				{
					sprintf_s(s, sizeof(s), "Room=%s wk=%d", Room->name, wk);
					Eprint("<Qischdlr>", s);
				}

				if (DEBUG)
					printf("wk=%d\n", wk);

				struct elout* Eo;
				double	Tr;
				Eo = Room->cmp->elouts;
				Tr = Room->Tr;	// 自然室温計算時は前時刻の室温で顕熱・潜熱分離する
				// 室温が高温となるときに、顕熱が負になるのを回避
				if (Eo->control == LOAD_SW)
					Tr = Room->rmld->Tset;

				//printf("name=%s control=%c Tr=%lf\n", Room->name, Eo->control, Tr);
				//Q = dmax((Hs24[wk] - d[wk] * (Room->Tr - 24.0)), 0.) * N;
				Q = dmax((Hs24[wk] - d[wk] * (Tr - 24.0)), 0.) * N;
				Room->Hc = Q * 0.5;
				Room->Hr = Q * 0.5;
				Room->HL = Ht[wk] * N - Q;

				if (DEBUG)
					printf("Hc=%lf Hr=%lf HL=%lf\n", Room->Hc, Room->Hr, Room->HL);
			}
		}

		//j = Room->lgtsc ;
		//v = *(val+j) ;
		if (Room->Lightsch != NULL && *Room->Lightsch > 0.0)
		{
			//Q = Room->Light * v ;
			Q = Room->Light * *Room->Lightsch;
			Room->Lc = Q * 0.5;
			Room->Lr = Q * 0.5;
		}
		else
			Room->Lc = Room->Lr = 0.0;

		//j = Room->apssc;
		//v = *(val+j) ;
		if (Room->Assch != NULL && *Room->Assch > 0.0)
		{
			//Room->Ac = Room->Apsc * v ;
			Room->Ac = Room->Apsc * *Room->Assch;
			//Room->Ar = Room->Apsr * v ;
			Room->Ar = Room->Apsr * *Room->Assch;
		}
		else
			Room->Ac = Room->Ar = 0.0;

		//j = Room->aplsc ;
		//v = *(val+j) ;
		if (Room->Alsch != NULL && *Room->Alsch > 0.0)
		{
			//Room->AL = Room->Apl * v ;
			Room->AL = Room->Apl * *Room->Alsch;
		}
		else
			Room->AL = 0.0;
	}
}
/* -------------------------------------------------------- */

/*  換気量の設定     */


void	Vtschdlr(int Nroom, ROOM* Room)
{
	int		i;
	double	Gvi, Gve;
	ROOM* Rmi;

	Rmi = Room;
	Gve = Gvi = 0.0;

	for (i = 0; i < Nroom; i++, Room++)
	{
		Gve = Gvi = 0.0;

		//j = Room->visc ;
		//v = *(val+j) ;
		if (Room->Visc != NULL && *Room->Visc > 0.0)
			Gvi = Room->Gvi * *Room->Visc;
		else
			Gvi = 0.0;

		//j = Room->vesc ;
		//v = *(val+j) ;
		if (Room->Vesc != NULL && *Room->Vesc > 0.0)
			Gve = Room->Gve * *Room->Vesc;
		else
			Gve = 0.0;

		Room->Gvent = Gvi + Gve;
	}
}

