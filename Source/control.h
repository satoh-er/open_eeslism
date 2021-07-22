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

/* control.h */

/* 制御、スケジュール設定 */


#define   VAL_CTYPE   'v'
#define   SW_CTYPE    's'
#define   MAIN_CPTYPE  'M'
#define   LOCAL_CPTYPE 'L'


typedef struct contl
{
   char  type;   /* 条件付き 'c'、無条件 ' ' */
   int   lgv;
   struct ctlif *cif;
   struct ctlif *andcif, *andandcif;
   struct ctlif *orcif ;
   struct ctlst *cst;
} CONTL;


typedef union ctltyp
{
   double *v;
   char  *s;
} CTLTYP;


typedef struct ctlif
{
   char  type,    /* 'v', 's'  */
         op;
   int   Nlft;
   union ctltyp  lft1, lft2, rgt;
} CTLIF;


typedef struct ctlst
{
   char  type,
         pathtype;  /* 'M', 'L' */
   void  *path;
   union ctltyp  lft, rgt;
} CTLST;

typedef struct vptr
{
   char  type;
   void  *ptr;
} VPTR;

