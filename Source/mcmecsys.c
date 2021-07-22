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

/*  mecsys.c  */

#define _CRT_SECURE_NO_WARNINGS
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"



/*  システム使用機器の初期設定  */

void	Mecsinit(double dTM, EQSYS *Eqsys, 
				 SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, 
				 int Nexsf, EXSF *Exsf, WDAT *Wd,RMVLS *Rmvls)
{   
	char	hptest[SCHAR] ;
	void	HeapCheck(char *s);

	Refaint ( Eqsys->Nrefa, Eqsys->Refa, Wd, Ncompnt, Compnt ) ;
	Collint ( Eqsys->Ncoll, Eqsys->Coll, Nexsf, Exsf, Wd ) ;
	Pipeint ( Eqsys->Npipe, Eqsys->Pipe, Simc, Ncompnt, Compnt, Wd ) ;
	/////////////////////////////////
	sprintf(hptest, "Mecsinit") ;
	HeapCheck(hptest) ;
	/////////////////////////////////
	Stankint ( dTM, Eqsys->Nstank, Eqsys->Stank, Simc, Ncompnt, Compnt, Wd ) ;
	Pumpint ( Eqsys->Npump, Eqsys->Pump, Nexsf, Exsf ) ;
	Stheatint ( Eqsys->Nstheat, Eqsys->stheat, Simc, Ncompnt, Compnt, Wd, Rmvls->Npcm, Rmvls->PCM ) ;
	Flinint ( Eqsys->Nflin, Eqsys->Flin, Simc, Ncompnt, Compnt, Wd ) ;
	VWVint ( Eqsys->Nvav, Eqsys->vav, Ncompnt, Compnt ) ;
	Thexint ( Eqsys->Nthex, Eqsys->Thex ) ;
	PVint ( Eqsys->Npv, Eqsys->PVcmp, Nexsf, Exsf, Wd) ;

	// Satoh追加　デシカント槽 2013/10/23
	Desiint(Eqsys->Ndesi, Eqsys->Desi, Simc, Ncompnt, Compnt, Wd) ;
	//Valvinit ( Eqsys->Nvalv, Eqsys->Valv, Eqsys ) ;

	// Satoh追加　気化冷却器　2013/10/31
	Evacint ( Eqsys->Nevac, Eqsys->Evac ) ;
}


/*  システム使用機器特性式係数の計算  */

void	Mecscf(EQSYS *Eqsys)
{
	Cnvrgcfv(Eqsys->Ncnvrg, Eqsys->Cnvrg);
	
	/******
	printf("  Mecscf cnv\n");
	***********/   
	Hccdwint(Eqsys->Nhcc, Eqsys->Hcc);
	Hcccfv(Eqsys->Nhcc, Eqsys->Hcc);
	
	/**********
	printf("  Mecscf Hcc\n"); 
	printf("   Nboi=%d\n", Eqsys->Nboi);
	********/
	Boicfv(Eqsys->Nboi, Eqsys->Boi);
	
	/**********
	printf("  Mecscf Boi\n"); *******/
	
	Collcfv(Eqsys->Ncoll, Eqsys->Coll);
	
	/**********
	printf("  Mecscf Coll\n");  *****/  
	
	Refacfv(Eqsys->Nrefa, Eqsys->Refa);
	
	/*********
	printf("  Mecscf Refa\n"); ******/
	
	Pipecfv(Eqsys->Npipe, Eqsys->Pipe);
	
	/********
	printf("  Mecscf Pipe\n");  ******/  
	
	/*******  Stankcfv(Eqsys->Nstank, Eqsys->Stank, Itr); *********/
	/*************
	printf("  Mecscf Stank\n"); ************/
	
	Hexcfv(Eqsys->Nhex, Eqsys->Hex);
	Pumpcfv(Eqsys->Npump, Eqsys->Pump);
	
	/*************
	printf("<<Mecscf>> Hcldcfv  Nhcload=%d\n", Eqsys->Nhcload);
	*************/
	
//	Hcldcfv(Eqsys->Nhcload, Eqsys->Hcload);
	
	/**********
	printf("<<Mecscf>> Hcldcfv\n");
	**********/
	
	/*---- Satoh Debug VAV  2000/12/5 ----*/
	VAVcfv(Eqsys->Nvav, Eqsys->vav);
	
	Stheatcfv(Eqsys->Nstheat, Eqsys->stheat) ;

	Thexcfv ( Eqsys->Nthex, Eqsys->Thex ) ;

	// Satoh追加　デシカント槽　2013/10/23
	Desicfv(Eqsys->Ndesi, Eqsys->Desi) ;

	// Satoh追加　気化冷却器　2013/10/26
	Evaccfv(Eqsys->Nevac, Eqsys->Evac) ;
}

/*  システム使用機器の供給熱量、エネルギーの計算  */

void Mecsene(EQSYS *Eqsys)
{
	Hccene(Eqsys->Nhcc, Eqsys->Hcc);
	/****************
	Boiene(Eqsys->Nboi, Eqsys->Boi);
	*********************/
	Collene(Eqsys->Ncoll, Eqsys->Coll);
	Refaene2(Eqsys->Nrefa, Eqsys->Refa);
	
	/*****************
	printf("Hc Boi Coll end\n" ) ;
	**********************/
	
	/***   Refaene(Eqsys->Nrefa, Eqsys->Refa); ***/
	
	Pipeene(Eqsys->Npipe, Eqsys->Pipe);
	
	/*******************
	printf ( "Pipe end\n" ) ;
	*********************/
	
	Hexene(Eqsys->Nhex, Eqsys->Hex);
	/****************/
	
	/***************
	printf("Hex end\n" ) ;
	*******************/
	
	Stankene(Eqsys->Nstank, Eqsys->Stank);
	/****************/
	Pumpene(Eqsys->Npump, Eqsys->Pump);
	
	/**********************
	printf("Stank Pump end\n" ) ;
	*********************/
	
	Stheatene(Eqsys->Nstheat, Eqsys->stheat) ;

	// Satoh追加　デシカント槽　2013/10/23
	Desiene(Eqsys->Ndesi, Eqsys->Desi) ;

	// Satoh追加　気化冷却器 2013/10/26
	//Evacene(Eqsys->Nevac, Eqsys->Evac) ;
	
	/***************
	Hcldene(Eqsys->Nhcload, Eqsys->Hcload);
	****************/

	Thexene ( Eqsys->Nthex, Eqsys->Thex ) ;

	Qmeasene (Eqsys->Nqmeas, Eqsys->Qmeas ) ;

	PVene (Eqsys->Npv, Eqsys->PVcmp ) ;
}



