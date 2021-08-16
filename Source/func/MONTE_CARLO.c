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

                         形態係数を求める
                                    FILE=MONTE_CARLO.c
                                    Create Date=1999.6.7
                061110 FFACTOR_LP$B$r=$@5(B  $B;M3Q7A$7$+G'<1$7$F$$$J$+$C$?(B

*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "wthrd.h"
#include "MODEL.h"
#include "exs.h"
#include "FUNC.h"
#include "MONTE_CARLO.h"
#define test 0
 

void MONTE_CARLO(int mpn,int lpn,int NUM,P_MENN *MP,P_MENN *LP,
                XYZ **GP, int gpn, int nday, int startday)
{

  int  j, h, l, n, i, k, mlpn ;
  double suma, sumg, *sumwall ;
  XYZ G, O, OO, E ;
  P_MENN *mlp ;
  double a, va, FF ;
  int gcnt ; /*--地面の代表点カウンター--*/

#if test
  FILE *fp ;

  if ((fp=fopen("monteOPLP.dat","w"))==NULL){
    printf("File not open\n") ;
    exit(1) ;
  }
#endif

  O.X=0.0 ; O.Y=0.0 ; O.Z=0.0 ;

/*--------メモリーの確保-----------------*/
  mlpn=mpn+lpn ;
  sumwall=(double *)malloc(sizeof(double)*mlpn) ;
  mlp=(P_MENN *)malloc(sizeof(P_MENN)*mlpn) ;

  suma=0 ;
  sumg=0 ;  
  gcnt = 0 ;
  for(i=0 ;i<mlpn ;i++)
    sumwall[i]=0.0 ;

  for (j=0 ;j<mpn ;j++){
 /*-----op面の中心点の座標を求める--------*/
    GDATA(MP[j],&G) ;
	//printf("MP[%d].opname=%s wlflg=%d\n",j,MP[j].opname,MP[j].wlflg) ;
/*-----------初期化---------------*/
	for(i=0 ;i<mlpn ;i++){
	  mlp[i].e.X=0.0 ;  
	  mlp[i].e.Y=0.0 ; 
	  mlp[i].e.Z=0.0 ;
	}
    //MATINIT(mlp,mlpn) ;  
 
/*---------mlp面の座標変換---------*/
    for (h=0 ;h<mlpn ;h++){

  
      if(h<mpn){

		mlp[h].sbflg = MP[h].sbflg ;   /*--higuchi add 080915 */

        mlp[h].polyd = MP[h].polyd ;
        mlp[h].P=(XYZ *)malloc(sizeof(XYZ)*mlp[h].polyd) ;
		for (i=0 ;i<mlp[h].polyd ;i++){
           mlp[h].P[i].X=0.0 ; 
		   mlp[h].P[i].Y=0.0 ; 
		   mlp[h].P[i].Z=0.0 ;
		}

	    for(k=1 ;k<366 ;k++)
	      mlp[h].shad[k] = 1.0 ;

	    for (l=0 ;l<mlp[h].polyd ;l++){
	      ZAHYOU(MP[h].P[l],G,&mlp[h].P[l],MP[j].wa,MP[j].wb) ;
#if test
          fprintf(fp,"mp[%d].P[%d].X=%f mp[%d].P[%d].Y=%f mp[%d].P[%d].Z=%f\n",
                  h,l,mlp[h].P[l].X,h,l,mlp[h].P[l].Y,h,l,mlp[h].P[l].Z) ;
#endif
		}
	  }
      else{
	    mlp[h].polyd = LP[h-mpn].polyd ;

	    mlp[h].sbflg = LP[h-mpn].sbflg ;  /*--higuchi add  0809015  --*/

        mlp[h].P=(XYZ *)malloc(sizeof(XYZ)*mlp[h].polyd) ;

		for (i=0 ;i<mlp[h].polyd ;i++){
           mlp[h].P[i].X=0.0 ; 
		   mlp[h].P[i].Y=0.0 ; 
		   mlp[h].P[i].Z=0.0 ;
		}

	    for(k=1 ;k<366 ;k++)
	      mlp[h].shad[k] = LP[h-mpn].shad[k] ;

	    for (l=0 ;l<mlp[h].polyd ;l++){   
	      ZAHYOU(LP[h-mpn].P[l],G,&mlp[h].P[l],MP[j].wa,MP[j].wb) ;
#if test
          fprintf(fp,"lp[%d].P[%d].X=%f lp[%d].P[%d].Y=%f lp[%d].P[%d].Z=%f sbflg=%d\n",
                  h,l,mlp[h].P[l].X,h,l,mlp[h].P[l].Y,h,l,mlp[h].P[l].Z,mlp[h].sbflg) ;
#endif
		}
	  }
	}

    ZAHYOU(O,G,&OO,MP[j].wa,MP[j].wb) ;
    E.X=0.0 ;
    E.Y=-sin((-MP[j].wb)*M_rad) ;
    E.Z=cos((-MP[j].wb)*M_rad) ;

/*-------op面の法線ベクトルを求める----------*/   
    HOUSEN(mlpn,mlp) ;
 
/*-------------点を射出する-------------------*/
    for (n=0 ;n<NUM ;n++){
      double ls=0.0,ms=0.0,ns=0.0,s=0.0 ;

/*----------乱数の発生--------------*/
      RAND(&a,&va) ;

      ls=sin(va)*cos(a) ;
      ms=sin(va)*sin(a) ;
      ns=cos(va) ;
   
      URA_M(ls,ms,ns,&s,MP[j].wb) ;
      KAUNT(mlpn, ls, ms, ns, &suma, &sumg, sumwall, s,
            mlp, GP[j], OO, E, MP[j].wa, MP[j].wb, G, gpn, 
            nday, &gcnt, startday, MP[j].wlflg) ;
    }

    if(nday == startday){
      if(gcnt>=gpn){
          GP[j][gpn].X=-999 ;
          GP[j][gpn].Y=-999 ;
          GP[j][gpn].Z=-999 ;
      }
      else{
          GP[j][gcnt].X=-999 ;
          GP[j][gcnt].Y=-999 ;
          GP[j][gcnt].Z=-999 ;
      }
    }

    FF=0.0 ;
    for(i=0 ;i<mlpn ;i++){
      MP[j].faiwall[i]=(double)sumwall[i]/(double)NUM ;
      FF=FF+MP[j].faiwall[i] ;
    }
    MP[j].faig = sumg / (double)NUM ;
    MP[j].faia = suma / (double)NUM ;

    /*--*/
    printf("%s faia=%f faig=%f faib=%f\n",MP[j].opname,MP[j].faia,MP[j].faig,FF) ;
    /*--*/

    FF=FF+MP[j].faia+MP[j].faig ;
 
    for(i=0 ;i<mlpn ;i++)
      sumwall[i] = 0 ;

    suma = 0 ;
    sumg = 0 ;
    gcnt = 0 ;
  }
  for(i=0 ;i<mlpn ;i++)
    free(mlp[i].P) ;
free(mlp) ;
free(sumwall) ;
#if test
fclose(fp) ;
#endif

}
  
/*-----------地面から見た天空に対する形態係数-----------------------*/
void GR_MONTE_CARLO(P_MENN *mp,int mpn,P_MENN *lp,int lpn,int monten,int day)
{
  int rp ;  
  int i, n, l, mlpn, k, h ;
  double ls=0.0, ms=0.0, ns=0.0 ;
  double a=0.0, va=0.0 ;
  double Px=0.0, Py=0.0, Pz=0.0, U=0.0, PX=0.0, PY=0.0, PZ=0.0 ;
  double S=0.0, T=0.0 ;
  double shad, sum ;
  P_MENN *mlp ;

  shad = 0.0 ;
  sum = 0.0 ;
  mlpn=mpn+lpn ;
  mlp=(P_MENN *)malloc(sizeof(P_MENN)*mlpn) ;

  /*---MPとLPの結合--*/
  for(i=0 ;i<mlpn ;i++){
    if(i<mpn){
      mlp[i]=mp[i] ;
      for(k=1 ;k<366 ;k++)
          mlp[i].shad[k] = 1.0 ;
    }
    else{
      mlp[i]=lp[i-mpn] ;
      for(k=1 ;k<366 ;k++)
       mlp[i].shad[k] = lp[i-mpn].shad[k] ;   //LPの遮蔽率を代入
    }
  }


  for(i=0 ;i<mpn ;i++){
    for (n=0 ;n<monten ;n++){
      RAND(&a,&va) ;

      ls=sin(va)*cos(a) ;
      ms=sin(va)*sin(a) ;
      ns=cos(va) ;

      for(l=0 ;l<mlpn ;l++){
         KOUTEN(mp[i].grp.X,mp[i].grp.Y,mp[i].grp.Z,ls,ms,ns,
	      &Px,&Py,&Pz,mlp[l].P[0],mlp[l].e) ;
	 PX=Px-mp[i].grp.X ;
	 PY=Py-mp[i].grp.Y ; 
	 PZ=Pz-mp[i].grp.Z ;
	 PRA(&U,ls,ms,ns,PX,PY,PZ) ;

         rp=mlp[l].polyd-2 ;
         /*--多角形ループ　三角形：１回、四角形：２回、、、---*/
         for(h=0 ; h<rp ; h++){
	   INOROUT(Px, Py, Pz, mlp[l].P[0], mlp[l].P[h+1], mlp[l].P[h+2], &S, &T) ; 
	   if (((S >= 0.0 && T >= 0.0) && ((S + T) <= 1.0)) && (U > 0.0)){
	     if(mlp[l].shad[day] > 0.0) {
	       if(shad == 0.0)
		 shad = 1 - mlp[l].shad[day] ; 
               else
		 shad = shad * (1 - mlp[l].shad[day]) ;
               break ;
	     }
	   }
	 }

      }
    sum = sum + shad ;
    }
    mp[i].grpfaia= sum / (double)monten ;
    sum=0 ;
  }
free(mlp) ;
}

/*---------障害物LPから見た天空に対する形態係数-----------*/
void FFACTOR_LP(int lpn,int mpn,int NUM,P_MENN *LP,P_MENN *MP)
{

int flg ; 
int rp ;   /*--多角形のループ回数---*/
int  j, h, l, n, i, mlpn, sum ;
XYZ G ;
P_MENN *mlp ;
double a, va, x=0.0, y=0.0, z=0.0, Px=0.0, Py=0.0, Pz=0.0, U=0.0 ;
double S=0.0, T=0.0 ;
#if test
FILE *fp1 ;

if ((fp1=fopen("monte.dat","w"))==NULL){
   printf("File not open\n") ;
   exit(1) ;
 }

for(h=0 ;h<lpn ;h++){
   for(l=0 ;l<4 ;l++){

     fprintf(fp1,"lp[%d].P[%d].X=%f Y=%f Z=%f\n",
                      h,l,LP[h].P[l].X,LP[h].P[l].Y,LP[h].P[l].Z) ;

   }
 }
for(h=0 ;h<mpn ;h++){
   for(l=0 ;l<4 ;l++){

     fprintf(fp1,"mp[%d].P[%d].X=%f Y=%f Z=%f\n",
                      h,l,MP[h].P[l].X,MP[h].P[l].Y,MP[h].P[l].Z) ;

   }
 }
#endif
/*---------メモリーの確保-----------------*/
 mlpn = lpn + mpn ;
 mlp=(P_MENN *)malloc(sizeof(P_MENN)*mlpn) ;

 sum=0 ;

  for (j=0 ;j<lpn ;j++){
 /*-----lp面の中心点の座標を求める--------*/
    GDATA(LP[j],&G) ; 

/*-------初期化---------------*/
for(i=0 ;i<mlpn ;i++){
	  mlp[i].e.X=0.0 ;  
	  mlp[i].e.Y=0.0 ; 
	  mlp[i].e.Z=0.0 ; 
}
    //MATINIT(mlp,mlpn) ;  

/*---------lp面の座標変換---------*/
     for (h=0 ;h<mlpn ;h++){
        if(h<lpn){

	      mlp[h].polyd = LP[h].polyd ;
          mlp[h].P=(XYZ *)malloc(sizeof(XYZ)*mlp[h].polyd) ;
		  for (i=0 ;i<mlp[h].polyd ;i++){
            mlp[h].P[i].X=0.0 ; 
		    mlp[h].P[i].Y=0.0 ; 
		    mlp[h].P[i].Z=0.0 ;
		  }

	      for (l=0 ;l<mlp[h].polyd ;l++){
	         ZAHYOU(LP[h].P[l],G,&mlp[h].P[l],LP[j].wa,LP[j].wb) ;
		  }
		}
        else{
	      mlp[h].polyd = MP[h-lpn].polyd ;
          mlp[h].P=(XYZ *)malloc(sizeof(XYZ)*mlp[h].polyd) ;
		  for (i=0 ;i<mlp[h].polyd ;i++){
            mlp[h].P[i].X=0.0 ; 
		    mlp[h].P[i].Y=0.0 ; 
		    mlp[h].P[i].Z=0.0 ;
		  }

	      for (l=0 ;l<mlp[h].polyd ;l++)
	        ZAHYOU(MP[h-lpn].P[l], G, &mlp[h].P[l], LP[j].wa, LP[j].wb) ;
		}
	 }
#if test
     for(h=0 ;h<mlpn ;h++){
        for(l=0 ;l<mlp[h].polyd ;l++){

//            printf("mlp[%d].P[%d].X=%f Y=%f Z=%f\n",
//                      h,l,mlp[h].P[l].X,mlp[h].P[l].Y,mlp[h].P[l].Z) ;
//            fprintf(fp1,"mlp[%d].P[%d].X=%f Y=%f Z=%f\n",
//                      h,l,mlp[h].P[l].X,mlp[h].P[l].Y,mlp[h].P[l].Z) ;

	}
     }
#endif
/*-------mlp面の法線ベクトルを求める----------*/   
      HOUSEN(mlpn,mlp) ;
 
/*---------点を射出する----------------*/
      for (n=0 ;n<NUM ;n++){
          double ls=0.0,ms=0.0,ns=0.0,s=0.0 ;

/*----------乱数の発生--------------*/
          RAND(&a,&va) ;
     
          ls=sin(va)*cos(a) ;
          ms=sin(va)*sin(a) ;
          ns=cos(va) ;
   
          URA_M(ls,ms,ns,&s,LP[j].wb) ;
          if(s<0.0){
              sum=sum+1 ;
              continue ;
	  }
          flg = 0 ;   /*--内側フラグ初期化--*/
          for(i=0 ;i<mlpn ;i++){
              KOUTEN(x,y,z,ls,ms,ns,&Px,&Py,&Pz,mlp[i].P[0],mlp[i].e) ;  
              PRA(&U,ls,ms,ns,Px,Py,Pz) ;
              rp=mlp[i].polyd-2 ;
          
              for(h=0 ; h<rp ; h++){
                 INOROUT(Px, Py, Pz, mlp[i].P[0], mlp[i].P[h+1], mlp[i].P[h+2], &S, &T) ;
	         if (((S >= 0.0 && T >= 0.0) && ((S + T) <= 1.0)) && (U > 0.0)){
	             sum = sum + 1 ;
                     flg = 1 ;
                     break ;
		 }
	      }
              if(flg>0)
		break ;
	  }
      }

      LP[j].faia=(double)(NUM-sum)/(double)NUM ;

	  //printf("sum=%d NUM=%d LP[%d].faia=%f\n", sum,NUM,j, LP[j].faia);
      sum=0 ;
  }
  for(i=0 ;i<mlpn ;i++)
    free(mlp[i].P) ;

free(mlp) ;
#if test
fclose(fp1) ;
#endif
}
