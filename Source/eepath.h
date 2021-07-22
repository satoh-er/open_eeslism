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

/*  eepath.h  */


#define  ROOM_TYPE        "ROOM"
#define  RDPANEL_TYPE     "RPNL"
//#define  AIRFLOW_TYPE     "AIRFLOW"

#define  HCCOIL_TYPE      "HCC"
#define  DXCOIL_TYPE      "DXC"
#define  BOILER_TYPE      "BOI"
#define  COLLECTOR_TYPE   "COL"
#define  ACOLLECTOR_TYPE   "ACOL"
#define  COLLECTOR_PDT    'w'
#define  ACOLLECTOR_PDT   'a'
#define  REFACOMP_TYPE    "REFA"
#define  STANK_TYPE       "STANK"
#define  HEXCHANGR_TYPE   "HEX"

#define  STHEAT_TYPE      "STHEAT"

#define	 THEX_TYPE		  "THEX"

#define  QMEAS_TYPE		  "QMEAS"

#define  VALV_TYPE		  "V"
#define  TVALV_TYPE		  "VT"

// Satoh追加 2013/10/20
#define  DESI_TYPE		"DESICCANT"

// Satoh追加 2013/10/26
#define  EVAC_TYPE		"EVPCOOLING"

/*---- Satoh Debug VAV  2000/10/30 ----*/
#define  VAV_TYPE		  "VAV"
#define  VWV_TYPE		  "VWV"
#define	 VAV_PDT		  'A'
#define  VWV_PDT		  'W'

#define  PIPEDUCT_TYPE    "PIPE"
#define  DUCT_TYPE        "DUCT"
#define   PIPE_PDT        'P'
#define   DUCT_PDT        'D'

#define  PUMP_TYPE        "PUMP"

#define  FAN_TYPE	  "FAN"
#define  PUMP_PF	  'P'
#define  FAN_PF	 	  'F'

#define  PUMP_C			"C"
#define  PUMP_Vv		"Vv"
#define  PUMP_Vr		"Vr"

#define	 FAN_C			"C"
#define  FAN_Vd			"Vd"
#define  FAN_Vs			"Vs"
#define  FAN_Vp			"Vp"
#define  FAN_Vr			"Vr"

#define  PV_TYPE		"PV"

#define  DIVERG_TYPE      "B"
#define  CONVRG_TYPE      "C"
#define  DIVGAIR_TYPE     "BA"
#define  CVRGAIR_TYPE     "CA"

#define  DIVERGCA_TYPE    "_B"
#define  CONVRGCA_TYPE    "_C"

#define  FLIN_TYPE      "FLI"
#define  GLOAD_TYPE     "GLD"

#define  HCLOAD_TYPE     "HCLD"
#define  HCLOADW_TYPE     "HCLDW"
#define  RMAC_TYPE		 "RMAC"
#define  RMACD_TYPE		"RMACD"

#define  OMVAV_TYPE       "OMVAV"
#define  OAVAV_TYPE       "OAVAV"

#define  OUTDRAIR_NAME    "_OA"
#define  OUTDRAIR_PARM    "t=Ta x=xa *"

#define  CITYWATER_NAME   "_CW"
#define  CITYWATER_PARM   "t=Twsup *"

#define  AIR_FLD   'A'
#define  AIRa_FLD  'a'
#define  AIRx_FLD  'x'
#define  WATER_FLD 'W'

#define  HEATING_SYS  'a'
#define  HVAC_SYS     'A'
#define  DHW_SYS      'W'

#define  THR_PTYP  'T'
#define  CIR_PTYP  'C'
#define  BRC_PTYP  'B'

#define  DIVERG_LPTP  'b'
#define  CONVRG_LPTP  'c'
#define  IN_LPTP      'i'
#define  OUT_LPTP     'o'

#define  OFF_SW  'x'
#define  ON_SW   '-'
#define  LOAD_SW 'F'
#define  FLWIN_SW 'I'
#define  BATCH_SW 'B'

#define  COOLING_LOAD 'C'
#define  HEATING_LOAD 'H'
#define  HEATCOOL_LOAD 'L'

#define  COOLING_SW 'C'
#define  HEATING_SW 'H'

#define  TANK_FULL  'F'
#define  TANK_EMPTY 'E'
#define  TANK_EMPTMP  -777.0
#define  BTFILL  'F'
#define  BTDRAW  'D'

#define SYSV_EQV   'v'
#define LOAD_EQV   'L'

typedef struct compnt
{
	char *name,			// 機器名称
		*roomname,		// 機器の設置室名称（-room）
		*eqptype,		// 機器タイプ（"PIPE"など）
		*envname,		// 配管等の周囲条件名称（-env）
		*exsname,		// 方位名称
		*hccname,		// VWV制御するときの制御対象熱交換器名称
		*rdpnlname,		// VWV制御するときの制御対象床暖房（未完成）
		*idi,			// 入口の識別記号
		*ido,			// 出口の識別記号（熱交換器の'C'、'H'や全熱交換器の'E'、'O'など）
		*tparm,			// SYSCMPで定義された"-S"や"-V"以降の文字列を収録する
		*wetparm,		// 湿りコイルの除湿時出口相対湿度の文字列を収録
		*omparm,		// 
		//*pcmname,		// 電気蓄熱暖房器内臓PCMのスペック名称
		airpathcpy,		// 空気経路の場合は'Y'（湿度経路用にpathをコピーする）
        control;
	void *eqp;			// 機器特有の構造体へのポインタ
	int  neqp,
        ncat,
		Nout,			// 出口の数
		Nin,			// 入口の数
        nivar;
	double Ac ;			// 集熱器面積[m2]
	double PVcap ;		// 太陽電池容量[W]
	double Area ;		// 太陽電池アレイ面積[m2]
	double *ivparm;
	double eqpeff;		/* ボイラ室内置き時の室内供給熱量率 [-] */
	struct elout *elouts;
						// 機器出口の構造体へのポインタ（Nout個）
	struct elin  *elins;
						// 機器入口の構造体へのポインタ（Nin個）
//	struct valv	*valv ;
	struct compnt *valvcmp ;
						// 三方弁の対となるValvのCOMPNTへのポインタ
	//					/* バルブ用機器構造体 */
	//double	x,			/* バルブ開度 */
	//		xinit ;
	//char	org ;		/* CONTRLで指定されているとき'y' それ以外は'n' */
	//char	*OMfanName ;	// Valvが参照するファン風量
	char	*monPlistName ;
						// VALVで分岐などを流量比率で行う場合の観測対象のPlist名称
	double	mPCM;				// 電気蓄熱暖房器内臓PCMの容量[m3]
}  COMPNT;


typedef struct elout	// 機器の出口構造体
{
	char   id,			// 出口の識別番号（熱交換器の'C'、'H'や全熱交換器の'E'、'O'など）
		Pelmoid,			// 終端の割り当てが完了していれば '-', そうでなければ 'x'
		fluid,			// 通過する流体の種類（a:空気（温度）、x:空気（湿度）、W:水））
		control,		// 経路の制御
		sysld;			// 負荷を計算する場合は'y'、成り行きの場合は'n'
	double  G,			// 流量
		Q,				// 熱量
		sysv,			// 連立方程式の答え
		load,
		Co,				// 連立方程式の定数
		coeffo,			// 出口の係数
		*coeffin;		// 入口の係数（入口複数の場合はそれぞれの係数）
	int    Ni,			// 入口の数
		sv,				
		sld;
	struct compnt  *cmp;
						// 機器出口の構造体が属する機器
	struct elin    *elins;
						// 機器出口の構造体が関連する機器入口
	struct plist   *lpath;
						// 機器出口が属する末端経路
	struct elout   *eldobj,
		*emonitr;
}  ELOUT;


typedef struct elin		// 機器の入口構造体
{
	
	char   id;			// 入口の識別番号（熱交換器の'C'、'H'や全熱交換器の'E'、'O'など）
	double  sysvin;		// 連立方程式の答え
	struct elout  *upo,	// 上流の機器の出口
		*upv;
	struct plist  *lpath;
						// 機器入口が属する末端経路
}  ELIN;



typedef struct pelm		// SYSPTHに記載の機器
{
	char  co,
		ci;
	struct compnt *cmp;	// SYSPTH記載の機器の構造体
//	struct pelm *Pelmx ;
	struct elout  *out;	// 機器の出口
	struct elin   *in;	// 機器の入口
}  PELM;

typedef struct plist	// 末端経路
{
	int	UnknownFlow ;	// 末端経路が流量未知なら1、既知なら0
	char  *name,
		type,			// 貫流経路か循環経路かの判定
		control,		// 経路の制御情報
		batch,    /* バッチ運転を行う蓄熱槽のあるとき'y'、無いとき 'n'*/
				  org;      /* 入力された経路のとき'y'、
	複写された経路（空気系統の湿度経路）のとき'n' */
	char	*plistname ;	// 末端経路の名前
	int   Nelm,			// 末端経路内の機器の数
		lvc,
		Nvalv,	/* 経路中のバルブ数 */
		//Npump ,
		/*---- Satoh Debug VAV  2000/12/6 ----*/
		Nvav,	/* 経路中のVAVユニットの数 */
		NOMVAV,	/* OM用変風量制御ユニット数 */
		n ;	   /* 流量計算の時の番号 */
	double *Go,
		Gcalc,	// 温調弁によって計算された流量を記憶する変数
		G;
	double	*rate ;
				// 流量分配比
	
	//struct pump  *Pump ;		// ポンプ、ファン構造体へのポインタ
	struct pelm  *pelm,			// 末端経路内の機器
		*plmvb;	
	struct plist *lpair;
	struct plist *plistt,		// 空気系当時の温度系統
		*plistx ;				// 空気系当時の湿度系統
	struct valv	*valv ;
	struct mpath *Mpath ;
	struct plist *upplist, *dnplist ;
	struct omvav *OMvav ;
}  PLIST;

typedef struct mpath	// SYSPTHにおける';'で区切られる経路
{
	char  *name,		// 経路名称
		sys,
		type,
		fluid,
		control; 
	int   Nlpath,
		NGv,
		NGv2,
		Ncv,
		lvcmx;
	struct plist *plist,
						// 末端経路
//		*pl[PLMAX] ;
	    **pl ;
	char	rate ;		// 流量比率が入力されている経路なら'Y'
	double	*G0 ;		// 流量比率設定時の既知流量へのポインタ
	struct mpath	*mpair;
//	struct compnt  *cbcmp[CBCMPMX] ;
	struct compnt	**cbcmp ;	// 流量連立方程式を解くときに使用する分岐・合流機器
} MPATH;

typedef struct syseq
{
	char	a ;
//	double  eqcf[SYSEQMX*SYSEQMX], eqcv[SYSEQMX], Y[SYSEQMX];   
//	char    mrk[SYSEQMX];   
//	struct elout	*eleq[SYSEQMX] ;
//	struct elout	*elosv[SYSEQMX];
} SYSEQ;

typedef struct valv
{
	char	*name ;
	int		count ;
	double	x,			/* バルブ開度 */
		*xinit ;			/* バルブ開度の初期値 */
	char	org ;		/* CONTRLで指定されているとき'y' それ以外は'n' */
	struct compnt *cmp, *cmb, *mon ;
	double	*Tin, *Tset, *Tout ;
	double	*MGo ;
	PLIST	*Plist, *monPlist ;
	//struct omvav	*OMfan ;
	//char	*OMfanName ;
} VALV ;

char Hload,  /* Hload = HEATING_LOAD */
Cload,  /* Cload = COOLING_LOAD */
		HCload;  /* HCload = HEATCOOL_LOAD 
used in void Contrldata( )*/
