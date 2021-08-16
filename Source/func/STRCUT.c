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

#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "common.h"
#include "fesy.h"
#include "fnlib.h"
#include "winerror.h"

char *STRCUT(char *DATA, char a)
{
	int len, i ;
	char *ret, *rev, *rev2, *res, *RET, *name ;

	len = (int)strlen(DATA);
	//printf("len=%d\n",len) ;
	rev = ( char * ) malloc ( sizeof ( char ) * ( len + 1 )) ;
	rev2 = ( char * ) malloc ( sizeof ( char ) * ( len + 1 )) ;
	for(i=0 ;i<len ;i++)
		rev[i]=DATA[(len-1)-i] ;

	//printf ( "KKKK1\n" ) ;
	rev[len]='\0' ;
	//printf ( "KKKK2\n" ) ;
	res=strchr(rev,a);
	//printf ( "KKKK3\n" ) ;
	name=stralloc(res+1) ;
	//printf ( "KKKK4\n" ) ;

	strcpy(rev2,name) ;
	//printf ( "KKKK5\n" ) ;
	ret=rev2 ;
	len = (int)strlen(rev2);
	//printf ( "KKKK6\n" ) ;
	RET=(char *)malloc(sizeof(char)*(len+1)) ;
	//printf ( "KKKK7\n" ) ;

	for(i=0 ;i<len ;i++)
		RET[i]=ret[(len-1)-i] ;
	RET[len]='\0' ;
	//printf ( "KKKK8\n" ) ;

	free ( rev ) ;
	free ( rev2 ) ;
	free ( name ) ;
	return(RET) ;
	//free(RET) ;
}

