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

/*   sun.c 
-------------------     */
#include <string.h>
#include <math.h>
#include "common.h"

void Sunint(void);
int FNNday(int Mo, int Nd);
double FNDecl(int N);
double FNE(int N);
double FNSro(int N);
double FNTtas(double Tt, double E);
double FNTt(double Ttas, double E);
double FNTtd(double Decl);
void Solpos(double Ttas, double Decl, double *Sh, double *Sw, double *Ss, double *solh, double *solA);
void Srdclr(double Io, double P, double Sh, double *Idn, double *Isky);
void Dnsky(double Io, double Ihol, double Sh, double *Idn, double *Isky);


/* ================================================================

 SUNLIB
 
  太陽位置および日射量計算用ライブラリ－
  （宇田川、パソコンによる空気調和計算法、プログラム4.1の C 言語版, ANSI C 版）
  
---------------------------------------------------------------- */
void Sunint(void)
{
	extern char *UNIT;
	extern double Lat, Slat, Clat, Tlat, Isc;
	double Rd;
	
	Rd=PI/180.;
	Slat=sin(Lat*Rd);
	Clat=cos(Lat*Rd);
	Tlat=tan(Lat*Rd);
	if (strcmp(UNIT,"SI") == 0)
		Isc=1370.;
	else
		Isc=1178.;
}
/* --------------------------------------------- */
int FNNday(int Mo, int Nd)
{
	return( (153*(Mo-1)+2*(Mo<3)-9*(Mo>=3))/5 + Nd);
}
/*  -------------------------------------------- */
double FNDecl(int N)
{
    return( asin(.397949*sin(2.*PI*(N-81.)/365.)) );
}
/*  -------------------------------------------- */
double FNE(int N)
{
	double B;
	B=2.*PI*(N-81.)/365.;
	return( .1645*sin(2.*B)-.1255*cos(B)-.025*sin(B) );
}
/*  -------------------------------------------- */
double FNSro(int N)
{
	extern double Isc;
	return( Isc*(1.0+.033*cos(2.*PI*N/365.)) );
}
/*  -------------------------------------------- */
double FNTtas(double Tt, double E)
{
	extern double Lon, Ls;
	return( Tt+E+(Lon-Ls)/15. );
}
/*  -------------------------------------------- */
double FNTt(double Ttas, double E)
{
	extern double Lon, Ls;
	return( Ttas-E-(Lon-Ls)/15. );
}
/*  -------------------------------------------- */
double FNTtd(double Decl)
{
	extern double Tlat;
	double Cws, Ttd;
	Cws= -Tlat*tan(Decl);
	if ( 1. > Cws && Cws > -1. )
		Ttd=7.6394*acos(Cws); 
	else
    {
		if (Cws >= 1.) 
			Ttd=0.;
		else
			Ttd=24.;
    }
	return ( Ttd );
}
/* -------------------------------------------- */
void Solpos(double Ttas, double Decl, double *Sh, double *Sw, double *Ss, double *solh, double *solA)
{
	extern double Slat, Clat;
	static double  Sdecl, Sld, Cld, Ttprev=25.;
	double  Ch, Ca, Sa, W;
	
	if (Ttas < Ttprev) 
	{
		Sdecl=sin(Decl);
		Sld=Slat*Sdecl;
		Cld=Clat*cos(Decl);
	}
	W=(Ttas-12.)*.2618;
	*Sh = Sld + Cld*cos(W);
	*solh = asin(*Sh) / PI*180.;
	if (*Sh > 0.) 
	{
		Ch=sqrt(1.-*Sh * *Sh);
		Ca=(*Sh * Slat - Sdecl)/(Ch * Clat);
		*solA = (W > 0. ? 1.0 : -1.0)* acos(Ca) / PI*180.;
		Sa=(W/fabs(W))*sqrt(1.-Ca*Ca);
		*Sw=Ch*Sa;
		*Ss=Ch*Ca;
	}  
	else
	{
		*Sh=0.;
		*Sw=0.;
		*Ss=0.;    
		*solh = 0.;
		*solA = 0.;
	}
	Ttprev=Ttas;
}
/* -------------------------------------------- */
void Srdclr(double Io, double P, double Sh, double *Idn, double *Isky)
{
	if ( Sh>.001 ) 
	{
		*Idn=Io*pow(P,(1./Sh));
		*Isky=Sh*(Io - *Idn)*(.66-.32*Sh)*(.5+(.4-.3*P)*Sh);
	}
	else
	{
		*Idn=0.;
		*Isky=0.; 
	}
}
/* -------------------------------------------- */
void Dnsky(double Io, double Ihol, double Sh, double *Idn, double *Isky)
{ 
	double Kt; 
	if ( Sh>.001 ) 
	{
		Kt=Ihol/(Io*Sh);
		if ( Kt >= .5163+(.333+.00803*Sh)*Sh )  
			*Idn=(-.43+1.43*Kt)*Io;
		else
			*Idn=(2.277+(-1.258+.2396*Sh)*Sh)*(Kt*Kt*Kt)*Io;
		
		*Isky=Ihol - *Idn * Sh; 
	}
	else
	{
		*Idn=0.;
		*Isky=Ihol; 
	} 
}  
/* ------------------------------------------- */
