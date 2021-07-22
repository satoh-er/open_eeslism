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

/*   snbklib.c  */

#include <math.h>
#include "MODEL.h" /*----higuchi 070918--*/
#include "fesy.h"
#include "fnbld.h"
#include "fnlib.h"

/*  -----------------------------------------------------  */
double FNFsdw(int Ksdw, int Ksi, double Xazm, double Xprf,
			  double D, double Wr, double Hr, 
			  double Wi1, double Hi1, double Wi2, double Hi2)
{
    double  Da, Dp, Asdw, Fsdw;

	Asdw = 0.0 ;
    
    if (Ksdw == 0)
		return (0.0);
    else
    {
		Da=D*Xazm;
		Dp=D*Xprf;
		if (Ksdw == 2 || Ksdw == 6)
			Da=fabs((double)Da);
		if (Ksdw ==4 || Ksdw == 8)
			Da= -Da;
		
		switch (Ksdw)
		{
        case 1:  Asdw=FNAsdw1(Da,Dp,Wr,Hr,Wi1,Hi1,Wi2);
			break;
        case 2:  case 3:  case 4:
			Asdw=FNAsdw1(Dp,Da,Hr,Wr,Hi1,Wi1,Hi2);
			break;
        case 5:  Asdw=FNAsdw2(Dp,Hr,Wr,Hi1);
			break;
        case 6:  case 7: case 8:
			Asdw=FNAsdw2(Da,Wr,Hr,Wi1);
			break;
        case 9:  Asdw=FNAsdw3(Da,Dp,Wr,Hr,Wi1,Hi1,Wi2,Hi2);
			break;
		}
		Fsdw = Asdw/(Wr*Hr);
    }
    if (Ksi == 1)
		Fsdw = 1.0 - Fsdw;
    return (Fsdw);
}
/*  -----------------------------------------------------  */

double FNAsdw1(double Da, double Dp, 
			   double Wr, double Hr, double Wi1, double Hi, double Wi2)
{
    double  Wi, Daa, Dha, Dhb, Dwa, Dwb, Asdw;
	
    if (Dp <= 0.)
        return ( 0.0);
    else
    {
		Wi=Wi1;
		if( Da<0.)
			Wi=Wi2;
		Daa=fabs(Da);
		Dha=Wi*Dp/dmax(Wi,Daa)-Hi;
		Dha=dmin(dmax(0. ,Dha),Hr);
		Dhb=(Wi+Wr)*Dp/dmax(Wi+Wr,Daa)-Hi;
		Dhb=dmin(dmax(0. ,Dhb),Hr);
		if ( Hi>=Dp) 
			Dwa=0.;
		else
		{
			Dwa=(Wi+Wr)-Hi*Daa/Dp;
			Dwa=dmin(dmax(0. ,Dwa),Wr);
		}
		Dwb=(Wi+Wr)-(Hi+Hr)*Daa/dmax(Hi+Hr,Dp);
		Dwb=dmin(dmax(0. ,Dwb),Wr);
		Asdw=Dwa*Dha+.5*(Dwa+Dwb)*(Dhb-Dha);
    }
    return (Asdw);
}
/*  -----------------------------------------------------  */

double FNAsdw2(double Dp, double Hr, double Wr, double Hi)
{
    double  Dh, Asdw;
	
    if ( Dp<=0.)
		return (0.);
    else 
    {
		Dh=dmin(dmax(0. ,Dp-Hi),Hr);
		Asdw=Wr*Dh;
    }
    return (Asdw); 
}
/*  -----------------------------------------------------  */

double FNAsdw3(double Da, double Dp,
			   double Wr,double Hr, double Wi1, double Hi1, double Wi2, double Hi2)
{
    double  Dw1, Dw2, Dh1, Dh2, Asdw;
    
    Dw1=dmin(dmax(0. ,Da-Wi1),Wr);
    Dw2=dmin(dmax(0. ,-Da-Wi2),Wr);
    Dh1=dmin(dmax(0. ,Dp-Hi1),Hr);
    Dh2=dmin(dmax(0. ,-Dp-Hi2),Hr);
    Asdw=Wr*(Dh1+Dh2)+(Dw1+Dw2)*(Hr-Dh1-Dh2);
    return (Asdw); 
}

