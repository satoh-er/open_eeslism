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

/* daysum.h */

#pragma once

typedef struct svday /* システム変数の日集 */
{
   double  m,      /* 平均 */
          mn,     /* 最高 */
	  mx;     /* 最低 */
   long    hrs,    /* 平均値の母数 */
          mntime, /* 最高値発生時刻 */
          mxtime; /* 最低値発生時刻 */
} SVDAY;


typedef struct qday
{
   double  h,    /* 加熱積算値 */
          c,    /* 冷却積算値 */
	  hmx,   /* 加熱最大値 */
	  cmx;   /* 冷却最大値 */
   
   long   hhr,   /* 加熱時間回数 */
          chr,   /* 冷却時間回数 */
	  hmxtime,
	  cmxtime;
} QDAY;


typedef struct eday
{
   double  d,     /* 積算値 */
          mx;    /* 最大値 */
   
   long   hrs,   /* 運転時間回数 */
          mxtime;
} EDAY;

