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

/* eqptyp.h  */

typedef struct qmeas
{
	char	fluid ;
	int		id ;
	char	*name ;
	COMPNT	*cmp ;
	double	*Th, *Tc, *G, *xh, *xc ;
	PLIST	*PlistG, *PlistTh, *PlistTc, *Plistxh, *Plistxc ;
	double	Qs, Ql, Qt ;
	SVDAY	Thdy, Tcdy, xhdy, xcdy ;
	QDAY	Qdys, Qdyl, Qdyt ;
	SVDAY	mThdy, mTcdy, mxhdy, mxcdy ;
	QDAY	mQdys, mQdyl, mQdyt ;
//	PELM	*Pelmh, *Pelmc ;
	char	idh, idc ;
//	COMPNT	*cmph, *cmpc ;
	int		Nelmh, Nelmc ;
	//PLIST	*Plist ;
} QMEAS ;

typedef struct acs 
{
	double  w, t, x, C;
} ACS;

// Satoh追加　気化冷却器　2013/10/25
typedef struct EvaCoolingca
{
	char	*name ;			// カタログ名
	int		N ;				// 気化冷却器分割数
	double	Awet, Adry,		// Wet側、Dry側の境界壁接触面積[m2]
			hwet, hdry ;	// Wet側、Dry側の境界壁の対流熱伝達率[W/m2K]
	int		Nlayer ;		// 積層数
} EVACCA ;

typedef struct EvaCooling
{
	char	*name ;			// 機器名称
	EVACCA	*cat ;			// 機器仕様
	COMPNT	*cmp ;
	double	Tdryi, Tdryo,	// Dry側出入口温度[℃]
			Tweti, Tweto,	// Wet側出入口温度[℃]
			xdryi, xdryo,	// Dry側出入口絶対湿度[kg/kg']
			xweti, xweto,	// Wet側出入口絶対湿度[kg/kg']
			RHdryi, RHdryo,	// Dri側出入口相対湿度[%]
			RHweti, RHweto,	// Wet側出入口相対湿度[%]
			Gdry, Gwet,		// Dry側、Wet側風量[kg/s]
			*M, *kx,		// i層蒸発量[kg/m2 s]、物質移動係数[kg/m2 s (kg/kg')]
			*Tdry, *Twet,	// Dry側、Wet側の空気温度[℃]
			*xdry, *xwet,	// Dry側、Wet側の空気絶対湿度[kg/kg']
			*xs,			// 濡れ面近傍の絶対湿度（境界面温度における飽和絶対湿度）[kg/kg']
			*Ts,			// 境界面の表面温度[℃]（Wet側、Dry側は同じ温度）
			*RHwet, *RHdry,	// Dry側、Wet側の相対湿度[%]
			Qsdry, Qldry, Qtdry,
							// Dry側顕熱、潜熱、全熱[W]
			Qswet, Qlwet, Qtwet ;
							// Wet側顕熱、潜熱、全熱[W]
	double	*UX, *UXC ;
	int		count ;			// 計算回数。１ステップで最低２回は計算する
	//double	*UXdry, *UXwet, *UXC ;
							// 状態値計算用行列
	SVDAY	Tdryidy, Tdryody, Twetidy, Twetody ;
	QDAY	Qsdrydy, Qldrydy, Qtdrydy, Qswetdy, Qlwetdy, Qtwetdy ;
} EVAC ;

// Satoh追加　バッチ式デシカント空調機 2013/10/20
typedef struct desiccantca
{
	char	*name ;			// カタログ名
	double	r,				// シリカゲル平均直径[cm]
			ms,				// シリカゲル質量[g]
			rows,			// シリカゲル充填密度[g/cm3]
			Uad,			// シリカゲル槽壁面の熱貫流率[W/m2K]
			A,				// シリカゲル槽表面積[m2]
			Vm,				// モル容量[cm3/mol]
			eps,			// シリカゲルの空隙率
			P0,				// シリカゲルの飽和吸湿量[kg(water)/kg(silica gel)]
			kp,				// Polanyi DR 定数[cm3/(mol K)2
			cps ;			// シリカゲルの比熱[J/kgK]
} DESICA ;

typedef struct desiccant
{
	char	*name ;			// 機器名称
	DESICA	*cat ;			// 機器仕様
	COMPNT	*cmp ;
	double	Tain, Taout,	// 空気の出入口温度[℃]
			xain, xaout,	// 空気の出入口絶対湿度[kg/kg']
			UA,				// デシカント槽の熱損失係数[W/K]
			Asa,			// シリカゲルと槽内空気の熱伝達面積[m2]
			//Ts,				// シリカゲル温度[℃]
			//Xs,				// シリカゲル表面の絶対湿度[kg/kg']
			Tsold, Xsold,	// 前時刻の状態値
			Ta,				// デシカント槽内空気温度[℃]
			Xa,				// デシカント槽内絶対湿度[kg/kg']
			RHold,			// 前時刻の相対湿度状態値
			Pold,			// 前時刻の吸湿量[kg(water)/kg(silica gel)]
			cG,
			Qloss,			// デシカント槽表面からの熱損失[W]
			Qs, Ql, Qt,		// 顕熱、潜熱、全熱[W]
			*Tenv ;			// 周囲温度のポインタ[℃]
	double	*UX, *UXC ;
	//double	d, e, f, g ;	// 係数
	ROOM	*room ;			// デシカント槽設置室構造体
	SVDAY	Tidy, xidy,			// 入口温度日集計
			Tody, xody,			// 出口温度日集計
			Tsdy, xsdy ;			// 蓄熱体温度日集計
	QDAY	Qsdy, Qldy, Qtdy, Qlsdy ;			// 熱量日集計
	//SVDAY	mTidy,				/* 入口温度日集計 */
	//	mTody,					/* 出口温度日集計 */
	//	mTsdy ;					/* 蓄熱体温度日集計 */
	//QDAY	mQdy ;				/* 室供給熱量日集計 */
} DESI ;

/*---- Satoh Debug 熱交換換気  2001/8/22 ----*/

typedef struct thexca
{
	char	*name ;			/* カタログ名 */
//		type ;				/* t:顕熱交換型　h：全熱交換型 */
	double	et,				/* 温度効率 */
		eh ;				/* エンタルピ効率 */
} THEXCA ;

typedef struct thex
{
	char	*name,			/* 機器名称 */
		type ;				/* t:顕熱交換型　h：全熱交換型 */
	THEXCA	*cat ;
	COMPNT	*cmp ;
	double	et,				/* 温度効率 */
		eh ;				/* エンタルピ効率 */
	double	cGe,			/* 還気側熱容量流量 [W/K] */
		Ge,					/* 還気側流量 [kg/s] */
		cGo,				/* 外気側熱容量流量 [W/K] */
		Go,					/* 外気側流量 [kg/s] */
		Tein,				/* 還気側入口温度 [℃] */
		Teout,				/* 還気側出口温度 [℃] */
		Toin,				/* 外気側入口温度 [℃] */
		Toout,				/* 外気側出口温度 [℃] */
		xein,				/* 還気側入口絶対湿度 [kg/kg'] */
		xeout,				/* 還気側出口絶対湿度 [kg/kg'] */
		xoin,				/* 外気側入口絶対湿度 [kg/kg'] */
		xoout,				/* 外気側出口絶対湿度 [kg/kg'] */
		hein,				/* 還気側入口エンタルピー [J/kg] */
		heout,				/* 還気側出口エンタルピー [J/kg] */
		hoin,				/* 外気側入口エンタルピー [J/kg] */
		hoout,				/* 外気側出口エンタルピー [J/kg] */
		xeinold,
		xeoutold,
		xoinold,
		xooutold,
		Qes,				/* 交換顕熱 [W] */
		Qel,				/* 交換潜熱 [W] */
		Qet,				/* 交換全熱 [W] */
		Qos,				/* 交換顕熱 [W] */
		Qol,				/* 交換潜熱 [W] */
		Qot ;				/* 交換全熱 [W] */
	SVDAY	Teidy,			/* 還気側入口温度日集計 */
		Teody,				/* 還気側出口温度日集計 */
		xeidy,
		xeody,
		Toidy,
		Toody,
		xoidy,
		xoody,
		heidy,
		heody,
		hoidy,
		hoody ;
	QDAY	Qdyes, Qdyel, Qdyet, Qdyos, Qdyol, Qdyot ;
	SVDAY	mTeidy,			/* 還気側入口温度日集計 */
		mTeody,				/* 還気側出口温度日集計 */
		mxeidy,
		mxeody,
		mToidy,
		mToody,
		mxoidy,
		mxoody,
		mheidy,
		mheody,
		mhoidy,
		mhoody ;
	QDAY	mQdyes, mQdyel, mQdyet, mQdyos, mQdyol, mQdyot ;
} THEX ;

/*---- Satoh Debug VAV  2000/10/30 ----*/
typedef struct vavca
{
	char	*name,			/* カタログ名 */
		type ;				/* A:VAV  W:VWV */
	double	Gmax,			/* 最大風量 [kg/s] */
		Gmin,			/* 最小風量 [kg/s] */
		dTset ;				/* VWV用設定温度差　[℃] */
} VAVCA ;

// Satoh OMVAV  2010/12/16

typedef struct omvavca
{
	char	*name ;
	double	Gmax,			// 最大風量[kg/s]
			Gmin ;			// 最小風量[kg/s]
} OMVAVCA ;

/*---- Satoh Debug 電気蓄熱式暖房器 2001/1/20 ----*/
typedef struct stheatca
{
	char	*name ;			/* 機器名 */
	double	Q,				/* 電気ヒーター容量 [W] */
		Hcap,				/* 熱容量 [J/K] */
		KA,					/* 熱損失係数 [W/K] */
		eff ;				/* 温風吹出温度効率 [-] */
	char	*pcmname;		// 電気蓄熱暖房器内臓PCMのスペック名称
} STHEATCA ;

typedef struct stheat
{
	char	*name ;
	STHEATCA	*cat ;
	COMPNT	*cmp ;
	PCM		*pcm;				// 電気蓄熱暖房器内臓PCMのスペック構造体
	double	cG,					/* 熱容量流量 [W/K] */
		Ts,						/* 蓄熱体温度 [℃] */
		Tsold,					/* 前時間砕石温度 [℃] */
		Tin,					/* 入口（吸込）温度 [℃] */
		Tout,					/* 出口（吹出）温度 [℃] */
		*Tenv,					/* 周囲温度 [℃] */
		xin,					/* 入口絶対湿度 [kg/kg'] */
		xout,					/* 出口絶対湿度 [kg/kg'] */
		Q,						/* 供給熱量 [W] */
		E,						/* 電気ヒーター消費電力 [W] */
		Qls,					/* 熱損失 [W] */
		Qsto,					/* 蓄熱量 [W] */
		Qlossdy,				/* 日積算熱損失 [kWh] */
		Qstody ;				/* 日積算蓄熱量 [kWh] */
	double	mPCM;				// 電気蓄熱暖房器内臓PCMの容量[m3]
	double	Hcap;				// 熱容量（PCM潜熱も含む）
	ROOM	*room ;				/* 蓄熱暖房器設置室構造体 */
	SVDAY	Tidy,				/* 入口温度日集計 */
		Tody,					/* 出口温度日集計 */
		Tsdy ;					/* 蓄熱体温度日集計 */
	QDAY	Qdy ;				/* 室供給熱量日集計 */
	EDAY	Edy ;
	//EDAY	mtEdy[12][24] ;
	SVDAY	mTidy,				/* 入口温度日集計 */
		mTody,					/* 出口温度日集計 */
		mTsdy ;					/* 蓄熱体温度日集計 */
	QDAY	mQdy ;				/* 室供給熱量日集計 */
	EDAY	mEdy ;
	double	mQlossdy,				/* 日積算熱損失 [kWh] */
		mQstody ;				/* 日積算蓄熱量 [kWh] */
	EDAY	mtEdy[12][24] ;		// 月別時刻別消費電力[kWh]
} STHEAT ;

/*---- Satoh Debug VAV  2000/10/30 ----*/
typedef struct vav
{
	char	chmode ;		/* 冷房用、暖房用の設定 */
	char	*name ;			/* 機器名 */
	char	mon ;			/* 制御対象が
							　　コイルの時：c
							　　仮想空調機の時：h
							　　床暖房の時：f
							**************************/
	VAVCA	*cat ;			/* VAVカタログ構造体 */
	struct hcc		*hcc ;	/* VWVの時の制御対象コイル */
	struct hcload	*hcld ;	/* VWVの時の制御対象仮想空調機 */
	RDPNL	*rdpnl ;		/* VWVの時の制御対象放射パネル */
	//ROOM	*room ;			/* 制御室構造体 */
	double	G,				/* 風量 [kg/s] */
			cG,				/* 熱容量流量 [W/K] */
			Q,				/* 再熱計算時の熱量 [W] */
			Qrld,
			Tin, Tout ;		/* 入口、出口空気温度 */
	int		count ;			/* 計算回数 */
	COMPNT	*cmp ;
} VAV ;

// Satoh OMVAV 2010/12/16
typedef struct omvav
{
	char	*name ;
	OMVAVCA	*cat ;
	RMSRF	*omwall ;		// 制御対象とする集熱屋根
	COMPNT	*cmp ;
	PLIST	*Plist ;
	double	G ;
	RDPNL	*rdpnl[4] ;
	int		Nrdpnl ;
} OMVAV ;

typedef struct hccca /* 冷温水コイル機器仕様*/
{
	char    *name;
	double   et,               /*定格温度効率　　　　　　*/
		KA,
		eh;               /*定格エンタルピ効率　　　*/
} HCCCA;

typedef struct hcc /* システム使用冷温水コイル　*/
{ 
	char    *name, 
		wet;
	char	etype ;			/* 温度効率の入力方法
							　　e:et
								k:KA	*/
	struct hccca  *cat;
	struct compnt  *cmp;
	double   et,               /*温度効率　　　　　　*/
		eh;               /*エンタルピ効率　　　*/      
	struct  acs  Et, 
		Ex, 
		Ew;      
	double   cGa,
		Ga,
		cGw,
		Gw,
		Tain,
		Taout,
		xain,
		Twin,
		Twout,
		Qs,
		Ql,
		Qt;
	struct svday  Taidy, xaidy, Twidy;
	struct qday   Qdys, Qdyl, Qdyt;
	struct svday  mTaidy, mxaidy, mTwidy;
	struct qday   mQdys, mQdyl, mQdyt;
} HCC;

/* -------------------------------------------- */

typedef struct boica    /*ボイラ－機器仕様*/
{ 
	char	*name,         /*名称　　　　　　　　　　*/
		ene,              /*使用燃料　G:ガス、O:灯油、E:電気*/
		unlimcap,         /*エネルギー計算で機器容量上限無いとき 'y' */ 
		belowmin,		  /* 最小出力以下の時にOFFかONかを指示 */
						  /*      ON : ON_SW    OFF : OFF_SW   */
						  /* ただし、Qmin > 0 の時のみ有効 */ 
		plf;              /*部分負荷特性コ－ド　　　*/      
	//char	mode ;		// 温熱源の時は 'H'、冷熱源の時は 'C'
	char	*Qostr ;	// 定格能力条件
	double	*Qo,               /*定格加熱能力　　　　　　*/
						// Qo<0 の場合は冷水チラー
		Qmin,
		eff,              /*ボイラ－効率　　　　　　*/
		Ph;               /*温水ポンプ動力 [W] */
} BOICA;

typedef struct boi     /*システム使用ボイラ－*/
{
	char   *name,
		mode,			  /* 負荷制御以外での運転モード
								最大能力：M
								最小能力：m        */
		HCmode,			// 冷房モート゛、暖房モード
		*load;
	struct boica *cat;
	struct compnt  *cmp;
	double  Do, D1,
		cG,
		Tin,
		Toset;
	double  Q, E, Ph;
	struct svday  Tidy;
	struct qday   Qdy;
	struct eday   Edy, Phdy;
	struct eday   mtEdy[12][24], mtPhdy[12][24] ;
	struct svday  mTidy;
	struct qday   mQdy;
	struct eday   mEdy, mPhdy;
} BOI;

/* -------------------------------------------- */

typedef struct rfcmp  /*標準圧縮機特性　*/
{
	char    *name,            /*名称　　　　　　　　　　*/
		*cname;           /*圧縮機タイプ説明　　　　*/
	double   e[4],             /*蒸発器係数　　　　　　　*/
		d[4],             /*凝縮器係数　　　　　　　*/
		w[4],             /*軸動力係数　　　　　　　*/
		Teo[2],           /*蒸発温度範囲　　　　　　*/
		Tco[2],           /*凝縮温度範囲　　　　　　*/
		Meff;             /*モ－タ－効率　　　　　　*/
} RFCMP;  

typedef struct hpch   /* ヒートポンプ定格能力　*/
{
	double   Qo,               /*定格冷却能力（加熱能力）*/
		Go,               /*定格冷（温）水量、風量　　　*/
		Two,              /*定格冷（温）水出口温度（チラ－）*/
		eo,               /*定格水冷却（加熱）器、空調機コイル温度効率*/
		
		Qex,              /*定格排出（採取）熱量　　*/
		Gex,              /*定格冷却風量、水量　　　　　*/
		Tex,              /*定格外気温（冷却水入口水温）*/
		eex,              /*定格凝縮器（蒸発器）温度効率*/
		
		Wo;               /*定格軸動力　　　　　　　*/  
} HPCH;


typedef struct refaca    /*ヒートポンプ（圧縮式冷凍機）機器仕様*/
{ 
	char    *name,            /*名称　　　　　　　　　　*/
		awtyp,            /*空冷（空気熱源）=a、冷却塔使用=w */ 
		
		plf,              /*部分負荷特性コ－ド　　　*/
		unlimcap,         /*エネルギー計算で機器容量上限無いとき 'y' */
		mode[2];          /*冷房運転: C、暖房運転時: H */ 
	int     Nmode;            /*mode[]の数。1 のとき冷房専用または暖房専用　*/
	/*            2 のとき冷・暖 切換運転　*/
	struct  rfcmp *rfc;
	double   Ph;               /*定格冷温水ポンプ動力 [W] */
	
	struct hpch   *cool,      /* 冷房運転時定格能力　*/
		*heat;      /* 暖房運転時定格能力　*/      
} REFACA;

typedef struct refa    /*システム使用ヒートポンプ*/
{ 
	char    *name,           /*名称　　　　　　　　　　*/
		*load,
		chmode;          /*冷房運転: C、暖房運転時: H */ 
	struct refaca  *cat;
	struct compnt  *cmp;
	struct room    *room ;
	double   c_e[4],             /*冷房運転時蒸発器係数　　*/
		c_d[4],             /*冷房運転時熱源側（凝縮器係数）*/
		c_w[4],             /*冷房運転時軸動力係数　　*/
		
		h_e[4],             /*暖房運転時凝縮器係数　　*/
		h_d[4],             /*暖房運転時熱源側（蒸発器係数）*/
		h_w[4],             /*暖房運転時軸動力係数　　*/
		
		Ho,He,            /*運転時能力特性式係数　　*/
		*Ta,              /*外気温度 */
		Do, D1,
		cG,
		Te,               /*運転時蒸発温度　　　　　*/
		Tc,               /*運転時凝縮温度　　　　　*/
		
		Tin,
		Toset,
		Q, 
		Qmax,
		E,
		Ph;               /*冷温水ポンプ動力 [W] */
	struct svday  Tidy;
	struct qday   Qdy;
	struct eday   Edy, Phdy;
	EDAY		  mtEdy[12][24], mtPhdy[12][24] ;
	struct svday  mTidy;
	struct qday   mQdy;
	struct eday   mEdy, mPhdy;
	
} REFA;

/* -------------------------------------------- */

typedef struct collca     /*太陽熱集熱器機器仕様*/
{
	char    *name,
		type ;			// 水熱源：w、空気熱源：a
	
	double   b0, b1,
		Fd,					// 集熱器効率係数（=Kc / Ko）
		Ko,					// 総合熱損失係数[W/(m2･K)]
		Ac,
		Ag;
} COLLCA;

typedef struct coll        /*システム使用太陽熱集熱器*/
{
	char     *name;
	
	struct collca  *cat;       
	struct  exsf   *sol;
	struct compnt  *cmp;
	double    *Ta;
	double    Do, D1, 
		ec,
		Te,					// 相当外気温度
		Tcb,				// 集熱板温度
		//Ko,					// 総合熱損失係数[W/(m2･K)]
		//Fd,					// 集熱器効率係数（=Kc / Ko）
		Tin,				// 入口温度
		Q,					// 集熱量[W]
		Ac,					// 集熱器面積
		Sol;				// 集熱面日射量[W]（短波のみ）
	struct svday  Tidy;
	struct qday   Qdy;
	struct eday   Soldy;
	struct svday  mTidy;
	struct qday   mQdy;
	struct eday   mSoldy;
} COLL;

/* -------------------------------------------- */

/* -------------------------------------------- */

typedef struct pipeca     /*配管・ダクト仕様*/
{ 
	char   *name,
		type;       /*配管のとき P、ダクトのときD */
	double  Ko;
} PIPECA;

typedef struct pipe   /*システム使用配管・ダクト*/
{
	char   *name,
		*loadt,
		*loadx;
	//char	type ;
	struct pipeca  *cat;
	struct compnt  *cmp;
	struct room    *room ;
	double   L,
	       Ko,
		   *Tenv,
		   ep,
		   Do, D1,
		   Tin,
		   Q,
		   Tout,
		   hout,
		   xout,
		   RHout,
		   
		   Toset,
		   Xoset;
	struct svday  Tidy;
	struct qday   Qdy;
	struct svday  mTidy;
	struct qday   mQdy;
} PIPE;

/* -------------------------------------------- */

typedef struct stankca   /* 蓄熱槽機器仕様 */
{
	char    *name,
		type,      /* 形状  既定値 'C': 縦型 */
		*tparm;     /* 槽分割、流入口、流出口入力データ */
	double   Vol,
		KAside,
		KAtop,
		KAbtm,
		gxr;
} STANKCA;

typedef struct stank     /* システム使用蓄熱槽 */
{
	char    *name,
	batchop;     /* バッチ操作有　給水:'F'  排出:'D'  停止:'-' 
	バッチ操作無:'n' */ 
	struct stankca  *cat;
	struct compnt   *cmp;
	int     Ndiv,      /* 分割層数 */
		Nin,       /* 流入口、流出口数 */
		*Jin,
		*Jout,
		Jva,
		Jvb;
	int	Ncalcihex ;	// 内径と長さから計算される内蔵熱交のモデルの数
	char    *pthcon,
		**batchcon, /* バッチ給水、排出スケジュール　'F':給水 'D':排出 */
		*ihex,    /* 内蔵熱交換器のある経路のとき ihex[i]='y' */
				  cfcalc;   /* cfcalc = 'y':要素モデル係数の計算する。
							'n':要素モデル係数の計算しない。
	(温度分布の逆転時再計算指定のときに使用*/
	double  *B,
		*R,
		*d,
		*Fg,           /* Fg 要素機器の係数 [Ndiv x Nin] */
		*Tss;
	
	char    *dtankF;        /* 分割した槽内の状態　'F':満水　'E':空 */
	char	*KAinput ;		// 内蔵熱交換器の温度効率が入力されていたら'N'
							// KAが入力されていたら'Y'
							// 内径と長さが入力されていたら'C'
	double	dbld0,			// 内蔵熱交の内径[m]
			dblL,			// 内蔵熱交の長さ[m]
			dblTw,			// 熱伝達率計算用の配管内温度[℃]
			dblTa ;			// 熱伝達率計算用タンク温度[℃]
	double   *Tssold,
		*dvol,
		*Mdt,
		*KS,
		
		*KA,			// 内蔵熱交換器のKA[W/K]
		*ihxeff,       /* 内蔵熱交換器の熱交換器有効率　サイズは[Nin] */
		*cGwin,        /* cGwin, *EGwin, Twin, Q のサイズは[Nin] */
		*EGwin,        /* EGwin = eff * cGwin  */
		*Twin,   
		*Q,
		
		Qloss,          /* 槽熱損失　*/
		Qsto,           /*  槽蓄熱量 */
		
		*Tenv;         /* 周囲温度のアドレス */
	struct stkday  *stkdy;
	struct stkday  *mstkdy;
	double   Qlossdy,
		Qstody;
	double   mQlossdy,
		mQstody;
} STANK;

typedef struct stkday
{
	struct svday  Tidy, Tsdy;
	struct qday   Qdy;
} STKDAY;

/* -------------------------------------------- */ 

typedef struct hexca    /* 熱交換器機器仕様 */
{
	char   *name;
	double  eff;          /* 熱交換器有効率 */
	double	KA ;
} HEXCA;

typedef struct hex       /* システム使用熱交換器 */
{
	int		id ;
	char   *name;
	char	etype ;			/* 温度効率の入力方法
							　　e:et
								k:KA	*/
	struct hexca  *cat;
	struct compnt *cmp;
	double  eff,
		eCGmin,
		CGc, CGh,
		Tcin,
		Thin,
		Qci, Qhi;
	struct svday  Tcidy, Thidy;
	struct qday   Qcidy, Qhidy;
	struct svday  mTcidy, mThidy;
	struct qday   mQcidy, mQhidy;
} HEX;

/* -------------------------------------------- */ 

typedef struct pfcmp     /* ポンプ・ファンの部分負荷特性の近似式係数 */
{
	char	pftype,		/* 'P' ポンプ  'F' ファン */
			*type ;		/* ポンプ・ファンのタイプ */
	double	dblcoeff[5] ;
} PFCMP ;

//typedef struct compostca	// コンポスト機器仕様
//{
//	char* name;
//	double	UA,			// 熱損失係数[W/(m2･K)]
//		Mos,		// 初期有機物量[kg]
//		Mmo,		// 初期微生物量[kg]
//		Mw,			// 初期水分量[kg]
//		Mu;			// 初期灰分量[kg]
//}COMPOSTCA;
//
//typedef struct compost
//{
//	char* name;
//	double	UA,			// 熱損失係数[W/(m2･K)]
//		Mosi,			// 初期有機物量[kg]
//		Mmoi,			// 初期微生物量[kg]
//		Mwi,			// 初期水分量[kg]
//		Mui;			// 初期灰分量[kg]
//	char* type;			// 投入物の種類（P:糞尿、G:ごみ）
//	double* hos,		// 単位質量当たりの発熱量[J/kg]
//		* Mpoo;			// 投入量[kg]
//	int* B;				// ブリストルスケール
//	double	Tcm,		// 内部温度[℃]
//		xs,				// 内部絶対湿度[kg/kg']
//		Mw,				// 槽内水分量[kg]
//		Mmo,			// 槽内微生物量[kg]
//		Mos;			// 槽内有機物量[kg]
//	double* U, * C;
//}COMPOST;

typedef struct pumpca    /* ポンプ・ファン機器仕様 */
{
	char   *name,
		pftype,   /* 'P' ポンプ  'F' ファン */
		*type;     /* 'C' 定流量　　'P' 太陽電池駆動　*/
	
	double  Wo,       /* モーター入力 */
		Go,       /* 定格流量 */
		qef,      /* 発熱比率（流体加熱量= gef * Wo）*/
		*val;     /* 特性式係数など */	 
	struct pfcmp  *pfcmp ;
} PUMPCA;

typedef struct pump       /* システム使用熱交換器 */
{
	char   *name;
	struct pumpca *cat;
	struct compnt *cmp;
	//struct pfcmp  *pfcmp ;
	struct exsf   *sol;
	double  Q,
		G,
		cG,
		Tin,
		E; 
	double	PLC ;		// 部分負荷特性を考慮した入力率
	struct eday Qdy, Gdy, Edy;
	EDAY		mtEdy[12][24] ;
	struct eday mQdy, mGdy, mEdy;
} PUMP;

/* -------------------------------------------- */ 

typedef struct flin    /*  境界条件設定用仮想機器 */
{
	char *name,     
        *namet,   /* 変数名（温度、顕熱） */   
		*namex,   /* 変数名（湿度、潜熱） */   
				  awtype;   /* 'W':１変数のとき（nametの変数名のみ使用）、 
	'A':２変数のとき（namexの変数も使用） */
	double *vart,   /* nametで示された変数の値 */
		*varx;   /* namexで示された変数の値 */
	
	struct compnt *cmp;
} FLIN;

/* -------------------------------------------- */ 

typedef struct hcload   /* 空調機負荷仮想機器　*/
{ 
	char    *name,
		*loadt,
		*loadx,
		RMACFlg,		// ルームエアコンなら'Y'
		chmode,			// スケジュール等によって設定されている運転モード
//		opmode,			// 実際の運転時のモード
		type,    /* 'D':直膨コイル想定
		'W':冷温水コイル想定　*/
		wetmode,	/* 実際のコイル状態 */
		wet;     /*'y': wet coil  'n':dry coil */
	
	double   cGa,
		Ga,	     
		Tain,
		xain,
		Toset,
		Xoset,

		/*---- Roh Debug for a constant outlet humidity model of wet coil  2003/4/25 ----*/
		RHout,
		
		cGw,
		Gw,
		Twin,
		Twout,
		
		Qfusoku,
		Ele,
		COP,

		Qs,
		Ql,
		Qt;
	double	Qcmax, Qhmax, Qc, Qh, Qcmin, Qhmin,
		COPc, COPh,					// COP（定格）
		Ec, Eh, Ecmax, Ecmin,		// 消費電力[W]
		COPcmax, COPcmin,			// COP（最大能力時、最小能力時
		Gi, Go ;					// 室内機、室外機風量[kg/s]
	double	COPhmax, COPhmin, Ehmin, Ehmax ;
	double	Rc[3], Rh[3],			// 理論COPと実働COPの比の2次式回帰係数
		Pcc, Pch ;					// ファン等消費電力[W]
	double	BFi, BFo ;				// 室内機、室外機のバイパスファクタ
	double	rh, rc ;	// 定格能力と最大能力の比
	struct svday  Taidy, xaidy;
	struct qday   Qdys, Qdyl, Qdyt;
	struct qday	  Qdyfusoku, Edy ;
	EDAY		  mtEdy[12][24] ;
	struct svday  mTaidy, mxaidy;
	struct qday   mQdys, mQdyl, mQdyt;
	struct qday	  mQdyfusoku, mEdy ;
	
	struct compnt *cmp;      
} HCLOAD;

typedef struct gload  /* 入力負荷仮想機器 */
{
	char *name,
        *nameqs,
		*nameql,
		*nameQt;
	double *Qs,
		*Ql,
		*Qt;
	
	struct compnt *cmp;
} GLOAD;

typedef struct pvca
{
	char	*name ;
	double	PVcap,					// 太陽電池容量[W]
			Area,					// アレイ面積[m2]
			KHD,					// 日射量年変動補正係数[-]
			KPD,					// 経時変化補正係数[-]
			KPM,					// アレイ負荷整合補正係数[-]
			KPA,					// アレイ回路補正係数[-]
			effINO ;				// インバータ実行効率[-]
	double	apmax ;					// 最大出力温度係数[-]
	double	ap ;					// 太陽電池裏面の熱伝達率[W/m2K]
	char	type ;					// 結晶系：'C'  アモルファス系：'A'
	double	A, B ;					// 設置方式別の太陽電池アレイ温度計算係数
	char	InstallType ;			// 太陽電池パネル設置方法
									// 'A':架台設置形、'B':屋根置き形、'C':屋根材形（裏面通風構造があるタイプ）
	//double	PVcap ;					// 太陽電池設置容量[W]
} PVCA ;

typedef struct pv
{
	char	*name ;
	struct compnt  *cmp;
	PVCA	*cat ;
	double	KTotal,					// 太陽電池の総合設計係数[-]
			KConst,					// 温度補正係数以外の補正係数の積（温度補正係数以外は時々刻々変化しない）
			KPT ;					// 温度補正係数[-]
	double	TPV ;					// 太陽電池温度[℃]
	double	Power ;					// 発電量[W]
	double	Eff ;					// 発電効率[-]
	double	Iarea ;					// 太陽電池入射日射量[W]
	double	PVcap ;					// 太陽電池設置容量[W]
	double	Area ;					// アレイ面積[m2]
	double	*Ta, *V, *I ;			// 外気温、風速、日射量[W/m2]
	struct  exsf   *sol;			// 設置方位
	struct  qday	Edy ;			// 日積算発電量[kWh]
	struct	eday	Soldy ;
	struct  qday	mEdy ;			// 日積算発電量[kWh]
	struct  eday	mtEdy[12][24] ;
	struct	eday	mSoldy ;
} PV ;

/* ========================================= */

typedef struct eqcat   /*機器仕様データ一覧*/
{
	int   Nrfcmp;
	struct rfcmp *Rfcmp;
	
	int   Npfcmp ;
	struct pfcmp *pfcmp ;
	
	int   Nhccca;
	struct hccca  *Hccca;
	
	int   Nboica;
	struct boica  *Boica;
	
	int   Nrefaca;
	struct refaca *Refaca;
	
	int   Ncollca;
	struct collca *Collca;
	
	int   Npipeca;
	struct pipeca *Pipeca;
	
	int   Nstankca;
	struct stankca *Stankca;
	
	int   Nhexca;
	struct hexca *Hexca;
	
	int   Npumpca;
	struct pumpca *Pumpca;

	/*---- Satoh Debug VAV  2000/10/30 ----*/
	int	  Nvavca ;
	VAVCA	  *Vavca ;

	/*---- Satoh Debug 電気蓄熱式暖房器 2001/1/21 ----*/
	int   Nstheatca ;
	STHEATCA  *stheatca ;

	int	  Nthexca ;
	THEXCA	*Thexca ;

	int   Npvca ;
	PVCA		*PVca ;

	// Satoh OMVAV  2010/12/16
	int   Nomvavca ;
	OMVAVCA	*OMvavca ;

	// Satoh追加 2013/10/20
	int		Ndesica ;
	DESICA	*Desica ;

	// Satoh追加 2013/10/26
	int		Nevacca ;
	EVACCA	*Evacca ;

	// Satoh追加 2019/8/29
	//int		Ncompost;
	//COMPOSTCA* Compostca;
} EQCAT;

typedef struct eqsys   /*システム使用機器データ一覧*/
{
	int   Ncnvrg;
	struct compnt **Cnvrg;
	
	
	int   Nhcc;
	struct hcc  *Hcc;
	
	int   Nboi;
	struct boi  *Boi;
	
	int   Nrefa;
	struct refa *Refa;
	
	int   Ncoll;
	struct coll *Coll;
	
	int   Npipe;
	struct pipe *Pipe;
	
	int   Nstank;
	struct stank *Stank;
	
	int   Nhex;
	struct hex *Hex;
	
	int   Npump;
	struct pump *Pump;
	
	int   Nflin;
	struct flin *Flin;
	
	int   Nhcload;
	struct hcload *Hcload;
	
	int   Ngload;
	struct gload *Gload;
	
	/*---- Satoh Debug VAV  2000/10/30 ----*/
	int   Nvav;
	VAV	  *vav;

	/*---- Satoh Debug 電気蓄熱式暖房器 2001/1/21 ----*/
	int   Nstheat ;
	STHEAT	*stheat ;

	int	  Nthex ;
	THEX	*Thex ;

	int		Nvalv ;
	VALV	*Valv ;

	int		Nqmeas ;
	QMEAS	*Qmeas ;

	int		Npv ;
	struct pv	*PVcmp ;

	int		Nomvav ;
	OMVAV	*OMvav ;

	// Satoh追加 2013/10/20
	int		Ndesi ;
	DESI	*Desi ;

	// Satoh追加 2013/10/26
	int		Nevac ;
	EVAC	*Evac ;
} EQSYS;
