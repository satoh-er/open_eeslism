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

/* exs.h */

#pragma once

typedef struct exsf   /*外表面方位デ－タ                */
{
	char   *name,
		alotype,		// 外表面熱伝達率の設定方法
						// V:風速から計算、F:23.0固定、S:スケジュール
		typ;          /*一般外表面'S',地下'E', 地表面'e'  */
	double  Wa,           /*方位角                 */
		Wb,           /*傾斜角                 */
		Rg,           /*前面の日射反射率       */
		Fs,           /*天空を見る形態係数     */
		Wz,
		Ww,
		Ws,     /*傾斜面の方向余弦       */
		swb,
		cbsa,
		cbca,
		//*alosch,		// 外表面の熱伝達率スケジュール
		cwa,
		swa,
		/* tprof, tazm計算用係数  */
		*alo,          /*外表面総合熱伝達率　　　*/
		Z,            /*地中深さ　　　　　　　　*/
		erdff;        /*土の熱拡散率m2/s　　　　*/
	
	/*方位別日射関連デ－タ */
	double  cinc,        /*入射角のcos             */
		tazm,        /*見掛けの方位角のtan     */
		tprof,       /*プロファイル角のtan     */
		Gamma,		 // 見かけの方位角
		Prof,		 // プロファイル角
		Idre,        /*直逹日射  [W/m2]        */
		Idf,         /*拡散日射  [W/m2]        */
		Iw,          /*全日射    [W/m2]        */
		rn,          /*夜間輻射  [W/m2]        */
		Tearth;      /*地中温度　　　　　　　　*/
	int    end;
} EXSF; 

typedef struct exsfst
{
	int		Nexs ;
	EXSF	*Exs ;
	double	*alosch ;
	char	alotype ;		// 外表面熱伝達率の設定方法
						// V:風速から計算、F:23.0固定、S:スケジュール
	char	EarthSrfFlg ;
						// 地表面境界がある場合'Y'
} EXSFS ;
