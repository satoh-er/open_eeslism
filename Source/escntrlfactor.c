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
#include "fnesy.h"
#include "fnmcs.h"
#include "fnbld.h"
#include "fnlib.h"
#include "fnfio.h"

void	CalcControlStatus(EQSYS *Eqsys, RMVLS *Rmvls, WDAT *Wd, EXSFS *Exsfs)
{
	void CalcCollTe(int Ncoll, COLL *Coll);
	// 集熱器の相当外気温度を計算する
	CalcCollTe( Eqsys->Ncoll, Eqsys->Coll ) ;
	CalcSolarWallTe(Rmvls, Wd, Exsfs) ;
}
