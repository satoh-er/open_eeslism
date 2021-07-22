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

/*   sched.h    */

typedef struct seasn   /*季節設定*/
{
	char    *name;
//	int     N,sday[12],eday[12];
	int		N, *sday, *eday ;
	int     end;
} SEASN ;

typedef struct wkdy    /*曜日設定*/
{
	char    *name;
	int     wday[15];
	int     end;
} WKDY ;

typedef struct dsch    /*一日の設定量スケジュ－ル*/
{
	char    *name;
//	int     N, stime[SCDAYTMMAX],etime[SCDAYTMMAX];
	int     N, *stime, *etime ;
	double	*val ;
	int     end;
} DSCH ;

typedef struct dscw    /*一日の切り替えスケジュ－ル*/
{
	char    *name, dcode[10];
	int     N, *stime, *etime, Nmod;
	char    *mode ;
	int     end;
} DSCW ;

typedef struct sch     /*スケジュ－ル*/
{
	char    *name, type;
	int     day[366];
	int     end;
} SCH ;

typedef struct schdl
{
	int    Nsch, Nscw;
	SEASN   *Seasn ;
	WKDY    *Wkdy ;
	DSCH    *Dsch ;
	DSCW    *Dscw ;
	SCH		*Sch, *Scw ;
	double   *val;
	char    *isw;
} SCHDL;
