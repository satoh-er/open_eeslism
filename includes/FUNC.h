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

/*

                              関数の定義                   
                                      FILE=FUNC.h
                                      Create Date 1999.12.8
				     
*/

#pragma once

#include "wthrd.h"
#include "exs.h"
#include "MODEL.h"

/*-----障害物に当たった点の合計をカウントする---------*/
#include "func/KAUNT.h"

/*-------------乱数の発生-----------------------*/   
#include "func/RAND.h"

/*---------------影の計算-----------------*/ 
#include "func/SHADOW.h"

/*----------形態係数を求める-------------*/
#include "func/MONTE_CARLO.h"

/*----法線ベクトルを求める----------*/
#include "func/HOUSEN.h"

/*-----直線と平面の交点を求める-------*/
#include "func/KOUTEN.h"

/*-----点と面の交点が平面の内か外かを判別する-----*/
#include "func/INOROUT.h"

/*------構造体の初期化--------*/
#include "func/MAT_INIT.h"

#include "func/ZAHYOU.h"

/*------裏側の面を見つける--------*/
#include "func/URA.h"

/*-----壁面の中心点の座標を求める--------*/
#include "func/GDATA.h"

/*-----入力データの計算用構造体への変換-----------*/
#include "func/COORDNT.h"

/*------構造体から別の構造体へ代入------*/
#include "func/DAINYUU.h"

/*-----小数点の切捨て----------*/
#include "func/CAT.h"

/*-----ベクトルの向きを判定する----------*/  
#include "func/RPA.h"

/*------ベクトルの向き---------*/
#include "func/YOGEN.h"

/*-------DATAIN.c--------------*/
#include "func/DATAIN.h"

/*-------壁面に対する入射日射角度-------------------*/
#include "func/CINC.h"

/*-------OP,LPの座標出力（デバッグ用）-------------------*/
#include "func/ZPRINT.h"

/*------前面地面の代表点を求める-------------------*/
#include "func/GRGPOINT.h"

/*--------日射量、長波長放射量の計算------*/
#include "func/OPIhor.h"

/*------計算中の面から見える面と見えない面を判別する際に必要となる
                   ベクトルの値を出力する-------------------------*/
#include "func/ERRPRINT.h"

/*--------壁面の法線ベクトルを求める--------*/
#include "func/EOP.h"

#include "func/STRCUT.h"

/*----年間通日を求める-----*/
#include "func/NENNKANN.h"

/*----LPとOPの位置をCGで確認するための入力ファイルを作成する-----------*/
#include "func/LPOP_replacement.h"