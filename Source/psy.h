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

/*  psy.h  */

void Psyint(void);
void Poset(double Po);
double FNPo(void);
double FNPws(double T);
double FNDp(double Pw);

double FNDbrp(double Rh, double Pw);
double FNDbxr(double X, double Rh);
double FNDbxh(double X, double H);
double FNDbxw(double X, double Twb);
double FNDbrh(double Rh, double H);

double FNDbrw(double Rh, double Twb);
double FNDbhw(double H, double Twb);
double FNXp(double Pw);
double FNXtr(double T, double Rh);
double FNXth(double T, double H);

double FNXtw(double T, double Twb);
double FNPwx(double X);
double FNPwtr(double T, double Rh);
double FNRhtp(double T, double Pw);
double FNRhtx(double T, double X);

double FNH(double T, double X);
double FNWbtx(double T, double X);
double FNHc(double Twb);

double R0,Ca,Cv,Rc,Cc,Cw,Pcnv,P;
