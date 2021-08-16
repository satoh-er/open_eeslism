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

/* fesy.h */

#pragma once

#define  SIMUL_BUILDG  1

#include "esize.h"
#include "common.h"
#include "wthrd.h"
#include "sched.h"
#include "control.h"
#include "eps.h"

#include "simcon.h"
#include "daysum.h"

#if    SIMUL_BUILDG
#include "build.h"
#else
#include "buildex.h"
#endif

#include "eepath.h"
#include "eqptyp.h"

