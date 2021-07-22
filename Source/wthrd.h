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

/*   wthrd.h    */


typedef struct locat
{
	char   *name;   /*地名*/
	double  Lat,        /*緯度[deg] */
		Lon,        /*経度[deg] */
		Ls;         /*標準子午線[deg] */
	
	/*地中温度計算用*/
	int    daymxert;   
	double  Tgrav,
		DTgr;
	
	/*月毎の給水温度*/
	double  Twsup[12];
} LOCAT ;

typedef struct wdat    /*気象デ－タ         */
{
	double   T,         /*気温                  */
		x,         /*絶対湿度  [kg/kg]     */
		RH, 
		h,         /*エンタルピ [J/kg]     */
		Idn,       /*法線面直逹日射 [W/m2] */
		Isky,      /*水平面天空日射 [W/m2] */
		Ihor,      /*水平面全日射   [W/m2] */
		sunalt, sunazm,
		Sh,Sw,Ss,  /*太陽光線の方向余弦    */
		solh,solA,	// 太陽位置
		CC,        /*雲量                  */
                RN,        /*夜間輻射 [W/m2]       */
                Rsky,      /*大気放射量[W/m2] higuchi 070918 */
		Wv,        /*風速 [m/s]            */
		Wdre;      /*風向　１６方位        */
	
	char   RNtype ;	  /*気象データ項目  C:雲量　R:夜間放射量[W/m2] */
	
	char	Intgtsupw ;
						// 給水温度を補完する場合は'Y'、しない場合は'N'
						// デフォルトは'N'
	double  Twsup;     /*給水温度              */
	double	*EarthSurface ;
						// 地表面温度[℃]
} WDAT ;

typedef struct wdpt    /*気象データ項目のポインター  
VCFILEからの入力時 */
{
	double  *ta,
		*xa,
		*rh,
		*idn,
		*isky,
		*ihor,
		*cc,
		*rn,
		*wv,
		*wdre;
} WDPT;
