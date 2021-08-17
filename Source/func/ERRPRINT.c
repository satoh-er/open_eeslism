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
            
                         出力ファイル作成
                                      FILE=ERRPRINT.c
                                      Create Date 1999.10.26
				     
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "MODEL.h"

/*------計算中の面から見える面と見えない面を判別する際に必要となる
                   ベクトルの値を出力する-------------------------*/

void errbekt_printf(int n, int m, bekt *a, char *name)
{
  int i ;
  int j ;
  FILE *fp ;
  char fname[SCHAR];

  strcpy_s(fname, sizeof(fname), name);
  strcat_s(fname, sizeof(fname), ".gchi") ;

  if (fopen_s(&fp, fname,"w") != 0){
     printf("File not open errbekt_printf\n") ;
     exit(1) ;
  }

  fprintf(fp,"%s ",fname) ;

  for(i=0 ;i<n ;i++){
    for(j=0 ;j<m ;j++)
     fprintf(fp,"%f %f %f %f\n",a[i].ps[j][0],a[i].ps[j][1],
                                a[i].ps[j][2],a[i].ps[j][3]) ; 
  }
  fprintf(fp,"\n") ;
  fclose(fp) ;
} 

/*--形態係数の出力--*/
void ffactor_printf(FILE *fp4, int mpn, int lpn, P_MENN *mp, 
                    P_MENN *lp, int Mon, int Day)
{

  int i ;
  int j ;
  int k ;

  k=mpn+lpn ;
 
  fprintf(fp4,"%d/%d\n",Mon,Day) ;
  for (i=0 ;i<mpn ;i++){
    fprintf(fp4,"%s\nFAIA %f\nFAIG %f\n",mp[i].opname, mp[i].faia, mp[i].faig) ;
    for(j=0 ;j<k ;j++){

      if(j<mpn && mp[i].faiwall[j] != 0.0)
	fprintf(fp4,"%s %f\n",mp[j].opname,mp[i].faiwall[j]) ;

      else if(j>=mpn && mp[i].faiwall[j] != 0.0)
	fprintf(fp4,"%s %f\n",lp[j-mpn].opname,mp[i].faiwall[j]) ;
    }

     /*--
     fprintf(fp,"grpfaia=%f\n\n",p[i].grpfaia) ;
     --*/
  }
}

/*-----------法線ベクトルの出力-----------*/
void e_printf(int n, P_MENN *p, char *name)
{
  int i ;
  FILE *fp ;
  char fname[SCHAR];

  strcpy_s(fname, sizeof(fname), name);
  strcat_s(fname, sizeof(fname), ".gchi") ;

  if (fopen_s(&fp, fname,"w") != 0){
    printf("File not open e_printf\n") ;
    exit(1) ;
  }

  for(i=0 ;i<n ;i++)
    fprintf(fp,"%d x=%f y=%f z=%f\n",i,p[i].e.X,p[i].e.Y,p[i].e.Z) ;

  fclose(fp) ;
}

/*---------------影面積の出力--------------------*/
void shadow_printf(FILE *fp,int M, int D, double mt, int mpn, P_MENN *mp)
{
  int i ;

  fprintf(fp,"%d %d %5.2f",M,D,mt) ;
  
  for (i=0 ;i<mpn ;i++)
    fprintf(fp," %1.2f",mp[i].sum) ;   
    
  fprintf(fp,"\n") ; 
}

/*------MP面の情報出力----------------------*/
void mp_printf(int n, P_MENN *mp, char *name)
{
  int i ;
  FILE *fp ;
  char fname[SCHAR];

  strcpy_s(fname, sizeof(fname), name);
  strcat_s(fname, sizeof(fname), ".gchi") ;

  if (fopen_s(&fp, fname,"w") != 0){
     printf("File not open mp_printf\n") ;
     exit(1) ;
  }

  for(i=0 ;i<n ;i++){
    fprintf(fp,"name=%s wlflg=%d\n",mp[i].opname,mp[i].wlflg) ;
    fprintf(fp,"mp[%d]    wb=%f    wa=%f   ref=%f\n",
                                i,mp[i].wb,mp[i].wa,mp[i].ref) ;
    fprintf(fp,"         e.X=%f   e.Y=%f   e.Z=%f\n",
                                mp[i].e.X,mp[i].e.Y,mp[i].e.Z) ;
    fprintf(fp,"       grp.X=%f grp.Y=%f grp.Z=%f\n",
                                mp[i].grp.X,mp[i].grp.Y,mp[i].grp.Z) ; 
    fprintf(fp,"         G.X=%f   G.Y=%f   G.Z=%f\n",
                                mp[i].G.X,mp[i].G.Y,mp[i].G.Z) ;
    fprintf(fp,"\n") ;
  }
  fclose(fp) ;
}   
/*------MP面の前面地面のポイント座標出力---------------------*/
void gp_printf(XYZ **gp,P_MENN *mp, int mpn, int lpn, char *name)
{
  int i ;
  int k ;
  FILE *fp ;
  char fname[SCHAR];

  strcpy_s(fname, sizeof(fname), name);
  strcat_s(fname, sizeof(fname), ".gchi") ;

  if (fopen_s(&fp, fname,"w") != 0){
    printf("File not open gp_printf\n") ;
    exit(1) ;
  }
 
  for(i=0 ;i<mpn ;i++){
    fprintf(fp,"mp[%d] %s\n",i,mp[i].opname) ;
    k=0 ;
    while(gp[i][k].X!=-999){
      fprintf(fp,"%d %f %f %f\n",k,gp[i][k].X,gp[i][k].Y,gp[i][k].Z) ;
      k++ ;
    }
  }
  fclose(fp) ;
}

/*------LP面の情報出力---------------------------*/
void lp_printf(int n, P_MENN *lp, char *name)
{

  int i ;
  FILE *fp ;
  char fname[SCHAR];

  strcpy_s(fname, sizeof(fname), name);
  strcat_s(fname, sizeof(fname), ".gchi") ;

  if (fopen_s(&fp, fname,"w") != 0){
    printf("File not open\n") ;
    exit(1) ;
  }

  for(i=0 ;i<n ;i++){
    fprintf(fp,"mp[%d]    wb=%f    wa=%f   ref=%f\n",
                                i,lp[i].wb,lp[i].wa,lp[i].ref) ;
    fprintf(fp,"         e.X=%f   e.Y=%f   e.Z=%f\n",
                                lp[i].e.X,lp[i].e.Y,lp[i].e.Z) ;
    fprintf(fp,"         G.X=%f   G.Y=%f   G.Z=%f\n",
                                lp[i].G.X,lp[i].G.Y,lp[i].G.Z) ;
    fprintf(fp,"\n") ;
  }

  fclose(fp) ;
}    

/*--------------LP面毎の日射遮蔽率出力--------------*/
void lp_shad_printf(int lpn, P_MENN *lp, char *name)
{
  FILE *fp ;
  int i ;
  int j ;
  char fname[SCHAR];

  strcpy_s(fname, sizeof(fname), name);
  strcat_s(fname, sizeof(fname), ".gchi") ;

  if (fopen_s(&fp, fname,"w") != 0){
    printf("File not open lp_shad_printf\n") ;
    exit(1) ;
  }
 
  for(i=0 ;i<lpn ;i++){
    fprintf(fp,"%s ",lp[i].opname) ;
  
  }
  fprintf(fp,"\n") ;

  for(i=1 ;i<=365 ;i++){
    for(j=0 ;j<lpn ;j++){
      fprintf(fp,"%f ",lp[j].shad[i]) ;
    }
    fprintf(fp,"\n") ;
  }

  fclose(fp) ;
}
