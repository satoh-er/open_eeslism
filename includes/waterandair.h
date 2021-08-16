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

// waterandair.h

// 水、空気の物性値の計算
// パソコンによる空気調和計算法より作成

// 空気の密度　 dblT[℃]、出力[kg/m3]
double FNarow(double dblT);

// 空気の比熱　 dblT[℃]、出力[J/kgK]
double FNac();

// 空気の熱伝導率　 dblT[℃]、出力[W/mK]
double FNalam(double dblT);

// 空気の粘性係数　 dblT[℃]、出力[Pa s]
double FNamew(double dblT);

// 空気の動粘性係数　 dblT[℃]、出力[m2/s]
double FNanew(double dblT);

// 空気の膨張率　 dblT[℃]、出力[1/K]
double FNabeta(double dblT);

// 水の密度　 dblT[℃]、出力[kg/m3]
double FNwrow(double dblT);

// 水の比熱　 dblT[℃]、出力[J/kgK]
double FNwc(double dblT);

// 水の熱伝導率　 dblT[℃]、出力[W/mK]
double FNwlam(double dblT);

// 水の動粘性係数　 dblT[℃]、出力[m2/s]
double FNwnew(double dblT);

// 水の膨張率　 dblT[℃]、出力[1/K]
double FNwbeta(double dblT);

// 水の粘性係数　 dblT[℃]、出力[Pa s]
double FNwmew(double dblT);

// 空気の熱拡散率　 dblT[℃]、出力[m2/s]
double FNaa(double dblT);

// 水の熱拡散率　 dblT[℃]、出力[m2/s]
double FNwa(double dblT);

// プラントル数の計算
double FNPr(char strF, double dblT);

// グラスホフ数の計算
//   dblTs:表面温度[℃]
//   dblTa:主流温度[℃]
//   dblx :代表長さ[m]
double FNGr(char strF, double dblTs, double dblTa, double dblx);

// 各種定数を入力するとヌセルト数が計算される。
//   Nu = C * (Pr Gr)^m
double FNCNu(double dblC, double dblm, double dblPrGr);

// 管内熱伝達率の計算（流体は水のみ）
// dbld:配管内径[m]
// dblL:管長[m]
// dblv:管内流速[m/s]
// dblT:流体と壁面の平均温度[℃]
double FNhinpipe(double dbld, double dblL, double dblv, double dblT) ;

// 円管外部の自然対流熱伝達率の計算（流体は水のみ）
// dbld:配管内径[m]
// dblT:流体と壁面の平均温度[℃]
double FNhoutpipe(double dbld, double dblTs, double dblTa) ;
