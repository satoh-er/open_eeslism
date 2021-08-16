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

/* ================================================================

 SUNLIB
 
  太陽位置および日射量計算用ライブラリ－
  （宇田川、パソコンによる空気調和計算法、プログラム4.1の C 言語版, ANSI C 版）
  
---------------------------------------------------------------- */

#pragma once

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

extern double  Lat,Slat,Clat,Tlat,Lon,Ls,Isc;
