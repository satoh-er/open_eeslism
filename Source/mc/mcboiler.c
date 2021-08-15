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

/*  boiler.c  */

/*  ボイラ－   */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fesy.h"
#include "fnmcs.h"
#include "fnfio.h"
#include "fnlib.h"
#include "fnesy.h"

/* 機器仕様入力　　　　　　*/

int Boidata (char *s, BOICA *Boica)
{
	char *st;
	double dt;
	int   id=0;
	
	if ((st=strchr(s,'=')) == 0 && *s != '-' )
	{
		Boica->name = stralloc(s);
		Boica->unlimcap = 'n' ;
		Boica->ene = ' ' ;
		Boica->plf = ' ' ;
		Boica->Qo = NULL ;
		Boica->eff = 1.0 ;
		Boica->Ph = -999.0 ;
		Boica->Qmin = -999.0 ;
		//Boica->mode = 'n' ;
		Boica->Qostr = NULL ;
	}
	else if (( strcmp ( s, "-U" )) == 0 )
		Boica->unlimcap = 'y' ;
	else
	{ 
		*st = '\0';
		if ( strcmp(s, "p") == 0)
			Boica->plf = *(st+1);
		else if (strcmp(s, "en") == 0)
			Boica->ene = *(st+1);
		else if (strcmp(s, "blwQmin") == 0)
		{
			// 負荷が最小出力以下のときに最小出力でONとする
			if (strcmp(st+1, "ON") == 0)
				Boica->belowmin = ON_SW ;
			// 負荷が最小出力以下のときにOFFとする
			else if (strcmp(st+1, "OFF") == 0)
				Boica->belowmin = OFF_SW ;
			else
				id = 1 ;
		}
		else
		{
			dt=atof(st+1);
			if (strcmp(s,"Qo") == 0)
			{
				//Boica->Qo= dt;
				Boica->Qostr = stralloc(st+1) ;

				//if (Boica->Qo > 0. )
				//	Boica->mode = 'H' ;
				//else
				//	Boica->mode = 'C' ;
			}
			else if ( strcmp ( s, "Qmin" ) == 0 )
				Boica->Qmin = dt ;
			else if (strcmp(s, "eff") == 0)
				Boica->eff =dt;
			else if (strcmp(s, "Ph") == 0)
				Boica->Ph = dt;
			else
				id=1;
		}
    }
	return (id);
}

void	Boicaint(int NBoica, BOICA *Boica,  SIMCONTL *Simc, int Ncompnt, COMPNT *Compnt, WDAT *Wd, EXSFS *Exsf, SCHDL *Schdl)
{
	int	i ;

	for(i = 0; i < NBoica; i++, Boica++)
	{
		int i, err = 0;

		if (( i = idsch ( Boica->Qostr, Schdl->Sch, NULL )) >= 0 )
			Boica->Qo = Schdl->val + i;
		else
			Boica->Qo = envptr ( Boica->Qostr, Simc, 0, NULL, NULL, NULL ) ;
		
		//Errprint(err, "<ctlvptr>", Boica->Qostr);
	}
}

/* --------------------------- */

/*  特性式の係数  */

void Boicfv(int Nboi, BOI *Boi)
{
	ELOUT *Eo;
	double cG, Qocat, Temp;
	int i;
	
	for (i = 0; i < Nboi; i++, Boi++)
	{   
		if (Boi->cmp->control != OFF_SW)
		{
			Temp = fabs(*Boi->cat->Qo - (-999.9)) ;
			if (fabs(Temp) < 1.e-3)
				Qocat = 0. ;
			else
				Qocat = *Boi->cat->Qo ;

			if (Qocat > 0. )
				Boi->HCmode = 'H' ;
			else
				Boi->HCmode = 'C' ;

			Boi->Do = Qocat;

			if ( Boi->Do < 0.0 && Boi->HCmode == 'H' 
				|| Boi->Do > 0.0 && Boi->HCmode == 'C' 
				|| Boi->HCmode == 'n' )
				printf ( "<BOI> name=%s  Qo=%.4g\n", Boi->cmp->name, Boi->Do ) ;

			Boi->D1 = 0.0;
			
			Eo = Boi->cmp->elouts;
			Boi->cG = cG = spcheat(Eo->fluid) * Eo->G;
			Eo->coeffo = cG;
			
			if (Eo->control != OFF_SW)
			{
				// 出口を設定温度に制御
				if (Eo->sysld == 'y')
				{
				/******************
				printf ( "Boiler capacity OK!!\n" ) ;
					***********************/
					
					Eo->Co = 0.0;
					*(Eo->coeffin) = -cG;
				}
				// 固定能力で計算
				else
				{
				/************************
				printf ( "Boiler capacity limit!!\n" ) ;
					/*********************/
					// 最大能力
					if ( Boi->mode == 'M' )
						Eo->Co = Boi->Do;
					// 最小能力
					else
						Eo->Co = Boi->cat->Qmin ;

					*(Eo->coeffin) = Boi->D1 - cG;
				}
			}
		} 
		else
		{
			//機器が停止
			Eo = Boi->cmp->elouts;
			Eo->Co = 0.0;
			Eo->coeffo = 1.0;
			*(Eo->coeffin) = -1.0;
		}
	}
}

/* --------------------------- */

/*  供給熱量、エネルギーの計算 */

void Boiene(int Nboi, BOI *Boi, int *BOIreset)
{
	int		i;
	ELOUT	*Eo;
	int		reset, reset0 ;
	double	Qmin, Qocat, Qcheck ;
	
	for (i = 0; i < Nboi; i++, Boi++)
	{
		Boi->Tin = Boi->cmp->elins->sysvin;
		Qmin = Boi->cat->Qmin ;
		if(fabs(Qmin - (-999.0)) < 1.0e-5)
			Qmin = 0. ;

		Eo = Boi->cmp->elouts;
		reset = 0 ;
		
		if (Eo->control != OFF_SW)
		{
			Boi->Q = Boi->cG * (Eo->sysv - Boi->Tin);

			// 次回ループの機器制御判定用の熱量
			//Qcheck = Boi->cG * (Boi->Toset - Boi->Tin);
			Qcheck = Boi->Q ;

			// 加熱設定での冷却、冷却設定での加熱時はボイラを止める
			if ((Qcheck < 0.0 && Boi->HCmode == 'H' )
				|| ( Qcheck > 0.0 && Boi->HCmode == 'C' ))
			{
				//Boi->cmp->elouts->control = OFF_SW ;
				Boi->cmp->control = OFF_SW ;
				Eo->control = ON_SW ;
				//if(Eo->emonitr != NULL )
					Eo->emonitr->control = ON_SW ;
				Eo->sysld = 'n' ;

				reset = 1 ;
			}

			// 最小出力のチェック
			else if (Qmin > 0.0 && Qcheck < Qmin )
			{
				// 最小出力以下はOFFにする場合
				if (Boi->cat->belowmin == OFF_SW)
				{
					Boi->cmp->elouts->control = OFF_SW ;
					Boi->cmp->control = OFF_SW ;
					Eo->control = ON_SW ;
					//if(Eo->emonitr != NULL )
						Eo->emonitr->control = ON_SW ;
					Eo->sysld = 'n' ;
				}
				else
				{
					Eo->control = ON_SW ;
					Eo->emonitr->control = ON_SW ;
					Eo->sysld = 'n' ;
					Boi->mode = 'm' ;
				}

				reset = 1 ;
			}

			// 過負荷状態のチェック
			else if (Boi->cat->unlimcap == 'n')
			{
				if (fabs(*Boi->cat->Qo - (-999.9)) < 1.e-3)
					Qocat = 0. ;
				else
					Qocat = *Boi->cat->Qo ;

				reset0 = maxcapreset(Qcheck, Qocat, Boi->HCmode, Eo) ;

				if ( reset == 0 )
					reset = reset0 ;
				
				/************************
				printf ( "=== Boiene reset=%d\n", reset ) ;
				****************************/
			}
			
			//if (Qmin > 0.0 && Boi->Q < Qmin || ( Boi->Q < 0.0 && Boi->cat->mode == 'H' )
			//	|| ( Boi->Q > 0.0 && Boi->cat->mode == 'C' ))
			//{
			//	if ( Boi->cat->belowmin == OFF_SW || ( Boi->Q < 0.0 && Boi->cat->mode == 'H' )
			//		|| ( Boi->Q > 0.0 && Boi->cat->mode == 'C' ) )
			//	{
			//		Boi->cmp->elouts->control = OFF_SW ;
			//		Boi->cmp->control = OFF_SW ;
			//		Eo->control = ON_SW ;
			//		if(Eo->emonitr != NULL )
			//			Eo->emonitr->control = ON_SW ;
			//		Eo->sysld = 'n' ;
			//	}
			//	else
			//	{
			//		Eo->control = ON_SW ;
			//		Eo->emonitr->control = ON_SW ;
			//		Eo->sysld = 'n' ;
			//		Boi->mode = 'm' ;
			//	}
			//	
			//	/******************/
			//	//Boicfv(1, Boi) ;
			//	/*********************/
			//	
			//	//(*BOIreset)++ ;
			//	reset = 1 ;
			//}
			
			//else if (Boi->cat->unlimcap == 'n')
			//{
			//	reset = maxcapreset(Boi->Q, Boi->cat->Qo, Boi->cat->mode, Eo) ;
			//	
			//	/************************
			//	printf ( "=== Boiene reset=%d\n", reset ) ;
			//	****************************/
			//}

			if ( reset == 1 )
			{
				//printf("name=%s Boiene reset\n", Boi->name) ;
				Boicfv(1, Boi) ;
				(*BOIreset)++ ;
			}
			
			Boi->E = Boi->Q / Boi->cat->eff;
			Boi->Ph = Boi->cat->Ph;
		}
		else
		{
			Boi->Q = 0.0;
			Boi->E = Boi->Ph = 0.0 ;
		}
		
		/*****************************
		printf ( "CAP=%lf  Q=%lf\n", Boi->cat->Qo, Boi->Q ) ;
		*******************************/
	}
}

/* --------------------------- */

/* 負荷計算指定時の設定値のポインター */

int boildptr(char *load, char **key, BOI *Boi, VPTR *vptr)
{
	int err = 0;
	
	if (strcmp(key[1], "Tout") == 0)
	{
		vptr->ptr = &Boi->Toset;
		vptr->type = VAL_CTYPE;
		Boi->load = load;
	}
	else
		err = 1;
	return err;
}

/* --------------------------- */

/* 負荷計算指定時のスケジュール設定 */

void boildschd(BOI *Boi)
{
	ELOUT *Eo;
	
	if (Boi->load != NULL)
	{
		Eo = Boi->cmp->elouts; 
		if (Eo->control != OFF_SW)
		{
			if (Boi->Toset > TEMPLIMIT)
			{
				Eo->control = LOAD_SW;
				Eo->sysv = Boi->Toset;
			}
			else
				Eo->control = OFF_SW;
		}	 
	}
}
/* --------------------------- */

void boiprint(FILE *fo, int id, int Nboi, BOI *Boi)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nboi > 0)
            fprintf(fo, "%s  %d\n", BOILER_TYPE, Nboi); 
		for (i = 0; i < Nboi; i++, Boi++)
			fprintf(fo," %s 1 7\n", Boi->name);
		break;
		
	case 1:	 
		for (i = 0; i < Nboi; i++, Boi++)
		{
			fprintf(fo,"%s_c c c %s_G m f %s_Ti t f %s_To t f ",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo,"%s_Q q f  %s_E e f %s_P e f\n",
				Boi->name, Boi->name, Boi->name);
		}
		break;
		
	default:
		for (i = 0; i < Nboi; i++, Boi++)
            fprintf(fo, "%c %.4g %4.2lf %4.2lf %4.0lf %4.0lf %2.0lf\n",
			Boi->cmp->elouts->control, Boi->cmp->elouts->G,
			Boi->Tin, Boi->cmp->elouts->sysv, Boi->Q, Boi->E, Boi->Ph);
		break;
	}
}
/* --------------------------- */

/* 日積算値に関する処理 */

void boidyint(int Nboi, BOI *Boi)
{
	int i;
	
	for (i = 0; i < Nboi; i++, Boi++)
	{
		// 日集計のリセット
		svdyint(&Boi->Tidy);
		qdyint(&Boi->Qdy);
		edyint(&Boi->Edy);
		edyint(&Boi->Phdy);
	}
}

/* --------------------------- */

/* 月積算値に関する処理 */

void boimonint(int Nboi, BOI *Boi)
{
	int i;
	
	for (i = 0; i < Nboi; i++, Boi++)
	{
		// 日集計のリセット
		svdyint(&Boi->mTidy);
		qdyint(&Boi->mQdy);
		edyint(&Boi->mEdy);
		edyint(&Boi->mPhdy);
	}
}

void boiday(int Mon, int Day, int ttmm, int Nboi, BOI *Boi, int Nday, int SimDayend)
{
	int i;
	int Mo, tt ;
	
	Mo = Mon - 1 ;
	tt = ConvertHour(ttmm) ;
	for (i = 0; i < Nboi; i++, Boi++)
	{
		// 日集計
		svdaysum(ttmm, Boi->cmp->control, Boi->Tin, &Boi->Tidy);
		qdaysum(ttmm, Boi->cmp->control, Boi->Q, &Boi->Qdy);
		edaysum(ttmm, Boi->cmp->control, Boi->E, &Boi->Edy);
		edaysum(ttmm, Boi->cmp->control, Boi->Ph, &Boi->Phdy);

		// 月集計
		svmonsum(Mon, Day, ttmm, Boi->cmp->control, Boi->Tin, &Boi->mTidy, Nday, SimDayend);
		qmonsum(Mon, Day, ttmm, Boi->cmp->control, Boi->Q, &Boi->mQdy, Nday, SimDayend);
		emonsum(Mon, Day, ttmm, Boi->cmp->control, Boi->E, &Boi->mEdy, Nday, SimDayend);
		emonsum(Mon, Day, ttmm, Boi->cmp->control, Boi->Ph, &Boi->mPhdy, Nday, SimDayend);

		// 月・時刻のクロス集計
		emtsum(Mon, Day, ttmm, Boi->cmp->control, Boi->E, &Boi->mtEdy[Mo][tt]);
		emtsum(Mon, Day, ttmm, Boi->cmp->control, Boi->E, &Boi->mtPhdy[Mo][tt]);
	}
}

void boidyprt(FILE *fo, int id, int Nboi, BOI *Boi)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nboi > 0)
            fprintf(fo, "%s  %d\n", BOILER_TYPE, Nboi); 	 
		for (i = 0; i < Nboi; i++, Boi++)
			fprintf(fo," %s 1 22\n", Boi->name);
		break;
		
	case 1:	    
		for (i = 0; i < Nboi; i++, Boi++)
		{
			fprintf(fo,"%s_Ht H d %s_T T f ", Boi->name, Boi->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_Hp H d %s_P E f %s_tp h d %s_Pm e f\n\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
		}
		break;
		
	default:
		for (i = 0; i < Nboi; i++, Boi++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Boi->Tidy.hrs, Boi->Tidy.m, 
				Boi->Tidy.mntime, Boi->Tidy.mn, 
				Boi->Tidy.mxtime, Boi->Tidy.mx);
			
            fprintf(fo, "%1ld %3.1lf ", Boi->Qdy.hhr, Boi->Qdy.h);
            fprintf(fo, "%1ld %3.1lf ", Boi->Qdy.chr, Boi->Qdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Boi->Qdy.hmxtime, Boi->Qdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Boi->Qdy.cmxtime, Boi->Qdy.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Boi->Edy.hrs, Boi->Edy.d);
            fprintf(fo, "%1ld %2.0lf ", Boi->Edy.mxtime, Boi->Edy.mx);
			
            fprintf(fo, "%1ld %3.1lf ", Boi->Phdy.hrs, Boi->Phdy.d);      
            fprintf(fo, "%1ld %2.0lf\n", Boi->Phdy.mxtime, Boi->Phdy.mx);
		} 
		break;
	}
}

void boimonprt(FILE *fo, int id, int Nboi, BOI *Boi)
{
	int i;
	
	switch (id)
	{
	case 0:
		if (Nboi > 0)
            fprintf(fo, "%s  %d\n", BOILER_TYPE, Nboi); 	 
		for (i = 0; i < Nboi; i++, Boi++)
			fprintf(fo," %s 1 22\n", Boi->name);
		break;
		
	case 1:	    
		for (i = 0; i < Nboi; i++, Boi++)
		{
			fprintf(fo,"%s_Ht H d %s_T T f ", Boi->name, Boi->name);
			fprintf(fo,"%s_ttn h d %s_Tn t f %s_ttm h d %s_Tm t f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_Hh H d %s_Qh Q f %s_Hc H d %s_Qc Q f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_th h d %s_qh q f %s_tc h d %s_qc q f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_He H d %s_E E f %s_te h d %s_Em e f\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
			fprintf(fo, "%s_Hp H d %s_P E f %s_tp h d %s_Pm e f\n\n",
				Boi->name, Boi->name, Boi->name, Boi->name);
		}
		break;
		
	default:
		for (i = 0; i < Nboi; i++, Boi++)
		{
            fprintf(fo, "%1ld %3.1lf %1ld %3.1lf %1ld %3.1lf ", 
				Boi->mTidy.hrs, Boi->mTidy.m, 
				Boi->mTidy.mntime, Boi->mTidy.mn, 
				Boi->mTidy.mxtime, Boi->mTidy.mx);
			
            fprintf(fo, "%1ld %3.1lf ", Boi->mQdy.hhr, Boi->mQdy.h);
            fprintf(fo, "%1ld %3.1lf ", Boi->mQdy.chr, Boi->mQdy.c);      
            fprintf(fo, "%1ld %2.0lf ", Boi->mQdy.hmxtime, Boi->mQdy.hmx);
            fprintf(fo, "%1ld %2.0lf ", Boi->mQdy.cmxtime, Boi->mQdy.cmx);
			
            fprintf(fo, "%1ld %3.1lf ", Boi->mEdy.hrs, Boi->mEdy.d);
            fprintf(fo, "%1ld %2.0lf ", Boi->mEdy.mxtime, Boi->mEdy.mx);
			
            fprintf(fo, "%1ld %3.1lf ", Boi->mPhdy.hrs, Boi->mPhdy.d);      
            fprintf(fo, "%1ld %2.0lf\n", Boi->mPhdy.mxtime, Boi->mPhdy.mx);
		} 
		break;
	}
}

void boimtprt(FILE *fo, int id, int Nboi, BOI *Boi, int Mo, int tt)
{
	int i;
	extern double  cff_kWh;
	
	switch (id)
	{
	case 0:
		if (Nboi > 0)
            fprintf(fo, "%s %d\n", BOILER_TYPE, Nboi);
		for (i = 0; i < Nboi; i++, Boi++)
			fprintf(fo, " %s 1 2\n", Boi->name);
		break;
		
	case 1:
		for (i = 0; i < Nboi; i++, Boi++)
		{	 
			fprintf(fo,"%s_E E f %s_Ph E f \n", Boi->name, Boi->name);
		}
		break;
		
	default:
		for (i = 0; i < Nboi; i++, Boi++)
		{
			fprintf(fo, " %.2lf %.2lf\n", Boi->mtEdy[Mo-1][tt-1].d * cff_kWh, Boi->mtPhdy[Mo-1][tt-1].d * cff_kWh);
		}
		break;
	}
}
