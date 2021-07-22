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

                         年間通日を求める
                                    FILE=NENNKANN.c
                                    Create Date=1998.8.15

*/

#include <stdio.h>

int nennkann(int M,int D){
  int n ;

 if (M==1)
   n=D ;
 else if(M==2)
   n=31+D ;
 else if(M==3)
   n=31+28+D ;
 else if(M==4)
   n=31+28+31+D ;
 else if(M==5)
   n=31+28+31+30+D ;
 else if(M==6)
   n=31+28+31+30+31+D ;
 else if(M==7)
   n=31+28+31+30+31+30+D ;
 else if(M==8)
   n=31+28+31+30+31+30+31+D ;
 else if(M==9)
   n=31+28+31+30+31+30+31+31+D ;
 else if(M==10)
   n=31+28+31+30+31+30+31+31+30+D ;
 else if(M==11)
   n=31+28+31+30+31+30+31+31+30+31+D ; 
 else if(M==12)
   n=31+28+31+30+31+30+31+31+30+31+30+D ;

 return(n) ;
}  
/*-----------------------------------------------------*/
