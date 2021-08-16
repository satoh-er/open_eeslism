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

#include "fesy.h"
#include "fnfio.h"


#define MAXINT_DAY  -999.0
#define MININT_DAY  999.0


void svdyint(SVDAY *vd)
{
	vd->m = 0.0;
	vd->mn = MININT_DAY;
	vd->mx = MAXINT_DAY;
	vd->hrs = 0;
	vd->mntime = vd->mxtime = -1;
}

void svdaysum(long time, char control, double v, SVDAY *vd)
{
	if (control != OFF_SW)
	{
		vd->m += v;
		vd->hrs++;
		minmark(&vd->mn, &vd->mntime, v, time);
		maxmark(&vd->mx, &vd->mxtime, v, time);   
	}
	if (time == 2400 && vd->hrs > 0)
		vd->m /= vd->hrs;
}

void svmonsum(int Mon, int Day, int time, char control, double v, SVDAY *vd, int Dayend, int SimDayend)
{
	long	MoNdTt ;
	int	isEndDay(int Mon, int Day, int Dayend, int SimDayend) ;

	MoNdTt = 1000000L * Mon + 10000L * Day + time ;
	if (control != OFF_SW)
	{
		vd->m += v;
		vd->hrs++;
		minmark(&vd->mn, &vd->mntime, v, MoNdTt);
		maxmark(&vd->mx, &vd->mxtime, v, MoNdTt);   
	}
	if (isEndDay(Mon, Day, Dayend, SimDayend) == 1 && vd->hrs > 0 && time == 2400)
	{
		//printf("Mon=%d Day=%d hrs=%d time=%d\n", Mon, Day, vd->hrs, time) ;
		//printf("平均値の計算\n") ;
		vd->m /= (double) vd->hrs;
	}
}

/* ------------------------------------ */

void qdyint(QDAY *Qd)
{
	Qd->h = Qd->c = 0.0;
	Qd->hmx = Qd->cmx = 0.0;
	Qd->hhr = Qd->chr = 0;
	Qd->hmxtime = Qd->cmxtime = -1;
}

void qdaysum(int time, char control, double Q, QDAY *Qd)
{
	extern double  cff_kWh;

	if (control != OFF_SW)
	{

		if (Q > 0.0)
		{
			Qd->h += Q;
			maxmark(&Qd->hmx, &Qd->hmxtime, Q, time);
			Qd->hhr++;
		}
		else if (Q < 0.0)
		{
			Qd->c += Q;
			minmark(&Qd->cmx, &Qd->cmxtime, Q, time);
			Qd->chr++;
		}
	}

	if (time == 2400)
		Qd->h *= cff_kWh, Qd->c *= cff_kWh;
}

void qmonsum(int Mon, int Day, int time, char control, double Q, QDAY *Qd, int Dayend, int SimDayend)
{
	extern double  cff_kWh;
	int	isEndDay(int Mon, int Day, int Dayend, int SimDayend) ;
	//int isEndDay(int Mon, int Day) ;
	long	MoNdTt ;
	MoNdTt = 1000000L * Mon + 10000L * Day + time ;
	if (control != OFF_SW)
	{
		if (Q > 0.0)
		{
			Qd->h += Q;
			maxmark(&Qd->hmx, &Qd->hmxtime, Q, MoNdTt);
			Qd->hhr++;
		}
		else if (Q < 0.0)
		{
			Qd->c += Q;
			minmark(&Qd->cmx, &Qd->cmxtime, Q, MoNdTt);
			Qd->chr++;
		}
	}

	if (isEndDay(Mon, Day, Dayend, SimDayend) == 1 && time == 2400)
		Qd->h *= cff_kWh, Qd->c *= cff_kWh;
}

// 日集計関数。非運転時にも集計を行う
void qdaysumNotOpe(int time, double Q, QDAY *Qd)
{
	extern double  cff_kWh;

	if (Q > 0.0)
	{
		Qd->h += Q;
		maxmark(&Qd->hmx, &Qd->hmxtime, Q, time);
		Qd->hhr++;
	}
	else if (Q < 0.0)
	{
		Qd->c += Q;
		minmark(&Qd->cmx, &Qd->cmxtime, Q, time);
		Qd->chr++;
	}

	if (time == 2400)
		Qd->h *= cff_kWh, Qd->c *= cff_kWh;
}

void qmonsumNotOpe(int Mon, int Day, int time, double Q, QDAY *Qd, int Dayend, int SimDayend)
{
	extern double  cff_kWh;
	int	isEndDay(int Mon, int Day, int Dayend, int SimDayend) ;
	//int isEndDay(int Mon, int Day) ;
	long	MoNdTt ;
	MoNdTt = 1000000L * Mon + 10000L * Day + time ;

	if (Q > 0.0)
	{
		Qd->h += Q;
		maxmark(&Qd->hmx, &Qd->hmxtime, Q, MoNdTt);
		Qd->hhr++;
	}
	else if (Q < 0.0)
	{
		Qd->c += Q;
		minmark(&Qd->cmx, &Qd->cmxtime, Q, MoNdTt);
		Qd->chr++;
	}

	if (isEndDay(Mon, Day, Dayend, SimDayend) == 1 && time == 2400)
		Qd->h *= cff_kWh, Qd->c *= cff_kWh;
}

/* ------------------------------------ */

void edyint(EDAY *Ed)
{
	Ed->d = 0.0;
	Ed->mx = 0.0;
	Ed->hrs = 0;
	Ed->mxtime = -1;
}

void edaysum(int time, char control, double E, EDAY *Ed)
{ 
	extern double  cff_kWh;

	if (control != OFF_SW)
	{
		Ed->d += E;
		maxmark(&Ed->mx, &Ed->mxtime, E, time);
		Ed->hrs++;
	}

	if (time == 2400)
		Ed->d *= cff_kWh;
}

void emonsum(int Mon, int Day, int time, char control, double E, EDAY *Ed, int Dayend, int SimDayend)
{ 
	extern double  cff_kWh;
	int	isEndDay(int Mon, int Day, int Dayend, int SimDayend) ;
	//int isEndDay(int Mon, int Day) ;
	long	MoNdTt ;
	MoNdTt = 1000000L * Mon + 10000L * Day + time ;

	if (control != OFF_SW)
	{
		Ed->d += E;
		maxmark(&Ed->mx, &Ed->mxtime, E, MoNdTt);
		Ed->hrs++;
	}

	if (isEndDay(Mon, Day, Dayend, SimDayend) == 1 && time == 2400)
		Ed->d *= cff_kWh;
}

void emtsum(int Mon, int Day, int time, char control, double E, EDAY *Ed)
{ 
	if (control != OFF_SW)
		Ed->d += E;
}


/* ======================= */

void minmark(double *minval, long *timemin, double v, long time)
{ 
	if (v <= *minval)
	{
		*timemin = time;
		*minval = v;
	}
}
/* -------------------- */

void maxmark(double *maxval, long *timemax, double v, long time)
{ 
	if (v >= *maxval)
	{
		*timemax = time;
		*maxval = v;
	}
}


