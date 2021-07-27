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

/*   build.h  */

#pragma once

#include <string.h>
#include "exs.h"
#include "esize.h"

typedef struct bmlst				/*壁材料リスト */
{
	char	*mcode;					/*名称     */
	double   Cond,					/*熱伝導率  [W/mK] */
		Cro;						/*容積比熱  [J/m3K] */
} BMLST;

typedef struct pvwallcat
{
	double	//PVcap,					// 太陽電池容量[W]
		KHD,					// 日射量年変動補正係数[-]
		KPD,					// 経時変化補正係数[-]
		KPM,					// アレイ負荷整合補正係数[-]
		KPA,					// アレイ回路補正係数[-]
		KConst,					// 温度補正係数以外の補正係数の積（温度補正係数以外は時々刻々変化しない）
		effINO;				// インバータ実行効率[-]
	double	apmax;					// 最大出力温度係数[-]
	double	ap;					// 太陽電池裏面の熱伝達率[W/m2K]
	char	type;					// 結晶系：'C'  アモルファス系：'A'
	double	Rcoloff;	// 集熱ファン停止時の太陽電池から集熱器裏面までの熱抵抗[m2K/W]
	double	Kcoloff;
} PVWALLCAT;

typedef struct pvwall
{
	double	KTotal,					// 太陽電池の総合設計係数[-]
		KPT;					// 温度補正係数[-]
	double	TPV;					// 太陽電池温度[℃]
	double	Power;					// 発電量[W]
	double	Eff;					// 発電効率[-]
	double	PVcap;					// 太陽電池設置容量[W]
} PVWALL;

// エアフローウィンドウカタログ
//typedef struct airflowcat
//{
//	double	ti,						// 室内側ガラスの透過率
//			to,						// 室外側ガラスの透過率
//			ai, 					// 室内側ガラスの日射吸収率
//			ao,						// 屋外側ガラスの日射吸収率
//			aci,					// 室内側ガラスの中空層表面対流熱伝達率[W/m2K]
//			aco,					// 室外側ガラスの中空層表面対流熱伝達率[W/m2K]
//			acioff,					// 停止時室内側ガラスの中空層表面対流熱伝達率[W/m2K]
//			acooff,					// 停止時室外側ガラスの中空層表面対流熱伝達率[W/m2K]
//			alr ;					// 中空層の放射熱伝達率[W/m2K]
//} AIRFLOWCAT ;

// エアフローウィンドウ
//typedef struct airflow
//{
//	char	*name ;					// エアフローウィンドウ名称
//	char	*loadt ;
//	struct room		*rm ;			// 室のポインタ
//	struct rmsrf	*sd ;			// 表面のポインタ
//	struct compnt	*cmp ;			// 機器ポインタ
//	char	control ;				// 運転状態
//	double	Wh,						// =(aci+aco)/(ca*Ga)
//			Ew,						// =exp(-Wh)
//			EWh,					// =1-(1-Ew)/Wh
//			Toset,					// 出口設定温度
//			cG,						// =c*G
//			Twai,					// 入口温度
//			FP, FO, FI,				// 室熱取得計算時の係数
//			Epw, Ept, EPC,			// エアフローウィンドウの内部温度計算用係数
//			Q ;						// エアフローウィンドウの除去熱量[W]
//	struct svday	Twaidy ;		// 入口温度の日集計
//	struct qday		Qdy ;			// エアフローウィンドウの除去熱量日集計
//} AIRFLOW ;

typedef struct pcmstate
{
	char	*name;
	double	tempPCMNodeL,			// PCM温度（左側節点）
		tempPCMNodeR,				// 同（右）
		tempPCMave,					// PCM温度（平均温度）
		//capm,					// PCM比熱[J/kgK]
		//lamda;					// PCM熱伝導率[W/mK]
		capmL,						// PCM見かけの比熱（左側）[J/kgK]
		capmR,						// PCM見かけの比熱（右側）[J/kgK]
		lamdaL,						// PCM熱伝導率（左側）[W/mK]
		lamdaR;						// PCM熱伝導率（右側）[W/mK]
	double	oldcapmL,				// 前時刻PCM見かけの比熱（左側）
		oldcapmR,				// 前時刻PCM見かけの比熱（右側）
		oldlamdaL,				// 前時刻PCM熱伝導率（左側）
		oldlamdaR;				// 前時刻PCM熱伝導率（右側）
} PCMSTATE;

typedef struct rmsrf   /*壁体　固定デ－タ */
{
	char    //*sname[21],
		*name,   /*壁体名  */
		*sname,   /*RMP名 higuchi 070918 */
		wlpri,      /*壁体内部温度出力指定 'p'　*/
		shdpri,		// 日よけの影面積の出力指定 'p'
		sfepri,     /*要素別壁体表面温度出力指定 'y' */
		ble,        /*部位コ－ド           */
		typ,        /*壁のとき'H', 地中壁のとき'E', 窓のとき'W', 地表面境界の時'e' */
		mwtype,     /*専用壁のとき'I',共用壁のとき'C'*/
		mwside,     /*壁体 0 側のとき'i', M 側のとき'M'*/
		mrk,        /* '*' or '!'  */
		ffix_flg,	/*表面への短波長放射基本吸収比率が定義されている場合'*'
						未定義の場合'!' */
		PCMflg,		// PCMの有無の判定フラグ　毎時係数行列を計算するかどうかを決める
		pcmpri,		// PCMの状態値出力フラグ 'y'
		fnmrk[10];   /*窓変更設定用窓コ－ド　*/

	struct room  *room,
		*nextroom;
	struct rmsrf *nxsd;
	struct mwall *mw;		// 重量壁（差分による非定常計算）
	struct rdpnl *rpnl;   /*輻射パネル用        */
	struct window	*window;	// 窓
	int	dynamicwin;		// 動的に窓を切り替える場合'Y'
	struct window	*ifwin;	// Trueの時の窓
	//*falsewin ;	// Falseの時の窓
	struct ctlif	*ctlif;	// 動的窓の制御
	char		*dynamiccode;
	//AIRFLOW		*airflow ;

	int     rm,         /*室番号                 */
		n,          /*室壁体番号             */
		exs,        /*方位番号               */
		nxrm,       /*隣室番号               */
		nxn,        /*隣室室壁体番号         */
		wd,         /*壁体定義番号           */
		rmw,        /*重量壁体番号           */
		lwd,
		dr,         /*ドア定義番号           */
		//		drd[4], 
		fn,         /*選択窓定義番号         */
		Nfn,        /*窓種類数　　　　　　　 */
		fnd[10],     /*窓定義番号　　　　　　 */
		direct_heat_gain[10],	// 日射熱取得、窓部材熱抵抗を直接指定する場合の番号
		fnsw,       /*窓変更設定番号　　　　 */

		sb;         /*日除け定義番号         */

	double   A,          /*面積                   */
		Eo,         /*外表面輻射率           */
		as,         /*外表面日射吸収率       */
		c,          /*隣室温度係数           */
		tgtn,       /*日射総合透過率         */
		Bn,         /*吸収日射取得率         */
		/*壁体、窓熱抵抗　　　　 */
		*fsol,		/* 短波長放射の基本吸収比率 */

		/*窓透過日射の吸収比率     */
		srg,        // 1次入射比率（隣接室への透過やガラスから屋外への放熱は無視）
		srg2,		// 最終的に室の日射熱取得の内の吸収比率
		srh,        /*人体よりの輻射の吸収比率 */
		srl,        /*照明よりの輻射の吸収比率 */
		sra,        /*機器よりの輻射の吸収比率 */
		alo,
		ali,
		alic,
		alir,
		Qc,			/* 対流による熱取得 [W] */
		Qr,			/* 放射による熱取得 [W] */
		Qi,			/* 壁体貫流熱取得 [W] */
		Qgt,		// 透過日射熱取得[W]
		Qga,		// 吸収日射熱取得[W]
		Qrn,		// 夜間放射熱取得[W]
		K,
		Rwall,		// 熱抵抗[m2K/W] 表面熱伝達抵抗は除く
		CAPwall,	// 単位面積当たり熱容量[J/m2K]
		*alicsch,
		*alirsch,
		FI,
		FO,
		FP,
		CF,
		WSR,
		*WSRN,
		*WSPL,
		WSC,
		Fsdworg,	 // SUNBRKで定義した日よけの影面積率
		Fsdw,        /* 影面積  higuchi 070918 */ // KAGE-SUN用
		Ihor,        /*higuchi 070918 */
		Idre,        /*higuchi 070918 */
		Idf,         /*higuchi 070918 */
		Iw,          /*higuchi 070918 */
		rn,          /*higuchi 070918 */
		RS,          /*室内表面に吸収される短波長輻射 [W/m2] */
		RSsol,		/*室内表面に吸収される日射（短波長）[W/m2] */
		RSsold,		// 室内表面に入射する日射（短波長）（隣接室への透過を考慮する前）
		RSli,		/*室内表面に吸収される照明（短波長）[W/m2] */
		RSin,
		TeEsol,
		TeErn,
		Te,          /*外表面の相当外気温                    */
		Tmrt,        /*室内表面の平均輻射温度                */
		Ei,
		Ts,
		/*設備機器発熱*/
		eqrd;
	int     end;
	//double	ColTe ;		// 建材一体型空気集熱器の相当外気温度[℃]
	double	Tcole;		// 建材一体型空気集熱器の相当外気温度[℃] 記号変更
	double	Tcoleu, Tcoled;
	double	Tf;		// 建材一体型空気集熱器の熱媒平均温度[℃]
	QDAY	SQi;	/* 日積算壁体貫流熱取得 */
	SVDAY	Tsdy;
	PVWALL	PVwall;	// 太陽電池一体型壁体
	QDAY	mSQi;
	SVDAY	mTsdy;

	// 集熱器一体型壁体用計算結果
	int		Ndiv;		// 流れ方向温度分布の分割数
	double	ColCoeff,
		// 建材一体型集熱器計算時の後退差分要素URM
		*Tc,
		//Scol,	// 放射熱取得量[W/m2]
		oldTx;	// 前時刻の集熱器と躯体の境界温度（集熱器相当外気温度計算用）
	// 太陽電池一体型
	double	Iwall;
	char	PVwallFlg;	// 太陽電池一体型なら'Y'
	double	dblWsu, dblWsd;
	// 屋根一体型空気集熱器の通気層上面、下面の幅[m]
	double	dblKsu, dblKsd, dblKc, dblfcu, dblfcd, dblKcu, dblKcd;
	double	dblb11, dblb12, dblb21, dblb22;
	double	dblacr, dblacc, dblao;
	double	dblTsu, dblTsd, dblTf;
	double	dblSG;
	double	ku, kd;
	double	ras, Tg;

	PCMSTATE	*pcmstate;			// PCM状態値収録構造体
	int			Npcm;				// PCM設置レイヤー数

	double	tnxt;		// 当該部位への入射日射の隣接空間への日射分配（連続空間の隣室への日射分配）
	char	RStrans;	// 室内透過日射が窓室内側への入射日射を屋外に透過する場合'y'
} RMSRF;

typedef struct welm			/* 壁体各層の熱抵抗と熱容量 */
{
	char	*code;
	double	L;				/*各層の材料厚さ        */
	//		res,				/*節点間の熱抵抗 [m2K/W]*/
	//		cap ;				/*節点間の熱容量 [J/m2K]*/
	int		ND;			/*各層の内部分割数      */
	double	Cond,			/*熱伝導率  [W/mK] */
		Cro;			/*容積比熱  [J/m3K] */
} WELM;

typedef struct pcmparam		// PCM見かけの比熱計算用パラメータ
{
	double	T, B, bs, bl, skew, omega, a, b, c, d, e, f;
}PCMPARAM;

typedef struct chartable
{
	char	*filename;		// テーブル形式ファイルのファイル名
	char	PCMchar;		// E:エンタルピー、C:熱伝導率
	double	*T,				// PCM温度[℃]
		*Chara;			// 特性値（エンタルピー、熱伝導率）
	char	tabletype;		// h:見かけの比熱、e:エンタルピー
	double	minTemp, maxTemp;	// テーブルの下限温度、上限温度
	int		itablerow;		// テーブル形式の入力行数
	FILE	*fp;
	double	lowA, lowB, upA, upB;
	// 上下限温度範囲外の特性値計算用線形回帰式の傾きと切片
	double	minTempChng;	// 最低温度変動幅　前時刻からの温度変化がminTempChng以下の場合はminTempChngとして特性値を計算
}CHARTABLE;

typedef struct pcm			// 潜熱蓄熱材
{
	char	*name;			// PCM名称
	char	spctype;		// 見かけの比熱　m:モデルで設定、t:テーブル形式
	char	condtype;		// 熱伝導率　m:モデルで設定、t:テーブル形式
	double	Ql,				// 潜熱量[J/m3]
		Condl,			// 液相の熱伝導率[W/mK]
		Conds,			// 固相の熱伝導率[W/mK]
		Crol,			// 液相の容積比熱[J/m3K]
		Cros;			// 固相の容積比熱[J/m3K]
	double	Ts,				// 固体から融解が始まる温度[℃]
		Tl;			// 液体から凝固が始まる温度[℃]
	double	Tp;			// 見かけの比熱のピーク温度
	char	iterate;	// PCM状態値を収束計算させる場合は'y'
	char	iterateTemp;	// 収束条件に温度も加える場合は'y'（通常は比熱のみ）
	int		DivTemp;	// 比熱の数値積分時の温度分割数
	int		Ctype;		// 比熱
	//	double	T, B, bs, bl, skew, omega, a, b, c, d, e, f;
	PCMPARAM	PCMp;		// 見かけの比熱計算用パラメータ
	char	AveTemp;	// PCM温度を両側の節点温度の平均で計算する場合は'y'（デフォルト）
	double	nWeight;	// 収束計算時の現在ステップの重み係数
	double	IterateJudge;	// 収束計算時の前ステップ見かけの比熱の収束判定[%]
	CHARTABLE chartable[2];		// 0:見かけの比熱またはエンタルピー、1:熱伝導率
} PCM;

typedef struct wall       /*壁体　定義デ－タ       */
{
	char   ble,         /*部位コ－ド      */
		*name;    /*名前            */
	char	PCMflg;		// 部材構成にPCMが含まれる場合は毎時係数行列を作成するので
	// PCMが含まれるかどうかのフラグ
	int    N,           /*材料層数≠節点数        */
		Ip;          /*発熱面のある層の番号  */
	//	char   code[12][5]; /*各層の材料コ－ド      */
	double  *L;       /*節点間の材料厚さ        */
	//	int    *ND;      /*各層の内部分割数      */
	double  Ei,          /*室内表面輻射率        */
		Eo,          /*外表面輻射率          */
		as,          /*外表面日射吸収率      */
		Rwall,       /*壁体熱抵抗   [m2K/W]  */
		CAPwall,	// 単位面積当たりの熱容量[J/m2K]
		*CAP, *R,	// 登録された材料（≠節点）ごとの熱容量、熱抵抗
		effpnl;        /*パネル効率　　　　　　*/
	double	tnxt;		// 当該部位への入射日射の隣接空間への日射分配（連続空間の隣室への日射分配）
	int    M,           /*節点数                */
		mp;          /*発熱面のある節点番号  */
	double  *res,     /*節点間の熱抵抗 [m2K/W]*/
		*cap;     /*節点間の熱容量 [J/m2K]*/
	int    end;
	WELM	welm[WELMMX];
	double	tra,	// τα
		Ksu,	// 通気層上部から屋外までの熱貫流率[W/m2K]
		Ksd,	// 通気層下部から集熱器裏面までの熱貫流率[W/m2K]
		fcu,
		fcd;
	double	ku, kd;
	double  Ru, Rd;	// 通気層上部から屋外、通気層下部から集熱器裏面までの熱抵抗[m2K/W]
	double	Kc,
		Kcu,
		Kcd;
	double	Ko;
	double	air_layer_t;
	// 通気層の厚さ[m]
	double	dblEsu, dblEsd;
	// 通気層上面、下面の放射率
	double	ta, Eg, Eb, ag;
	// 空気層の厚さ[m]、ガラスの放射率、集熱板放射率、ガラスの日射吸収率
	char	chrRinput;	// 熱抵抗が入力されている場合は'Y'
	// 熱貫流率が入力されている場合は'N'
	//double	ta,		// 建材一体型空気集熱器の透過体透過率[-]
	//				// 2009/01/19 Satoh Debug
	//		Kdd,	// 集熱媒体から集熱器裏面までの熱貫流率[W/m2K]
	//		Kud,	// 集熱媒体から集熱器表面までの熱貫流率[W/m2K]
	//		Ku,		// 建材一体型空気集熱器の集熱板から屋外までの熱貫流率[W/m2K]
	//		Kd,		// 　　　　　　　　〃　　　　　　　裏面までの熱貫流率[W/m2K]
	//		Ko,		// Ku + Kd
	//		KdKo,	// Kd / Ko
	//		//KcKu,	// Kc / Ku
	//		Kc ;	// 集熱器全体の熱貫流率[W/m2K] ( = 1 / αA + Ko )
	//				// 2009/01/26 Satoh Debug
	char	WallType;
	// 建材一体型空気集熱器の場合：'C'
	// 床暖房等放射パネルの場合：'P'
	// 一般壁体の場合：'N'
	//char	PVwall ;
	// 太陽電池一体型建材（裏面通気）：'Y'
	char	*ColType;
	// 集熱器タイプ　JSES2009大会論文（宇田川先生発表）のタイプ
	PVWALLCAT	PVwallcat;
	PCM		*PCM[WELMMX],
		**PCMLyr;		// 潜熱蓄熱材
	double	PCMrate[WELMMX], *PCMrateLyr;
} WALL;

typedef struct Dfwl      /*壁体定義番号既定値  */
{
	int    E,           /*外壁  */
		R,           /*屋根  */
		F,           /*外部に接する床 */
		i,           /*内壁  */
		c,           /*天井  */
		f;           /*隣室に接する床 */
} DFWL;

typedef struct mwall     /*重量壁体デ－タ     */
{
	struct rmsrf  *sd, *nxsd;
	struct wall   *wall;
	int    ns,          /*壁体通し番号          */
		rm,          /*室番号                */
		n,           /*室壁体番号            */
		nxrm,        /*隣室番号              */
		nxn;         /*隣室室壁体番号        */
	/* [UX]の先頭位置       */
	double *UX;

	int    M, mp;
	double  *res, *cap;
	double uo,          /*室内表面のuo          */
		um,          /*外表面のum            */
		Pc;          /*床パネル用係数        */
	double  *Tw,      /*壁体温度              */
		*Told,    /*以前の壁体温度        */
		*Twd,		// 現ステップの壁体内部温度
		*Toldd;		// PCM温度に関する収束計算過程における前ステップの壁体内温度
	int    end;
} MWALL;

typedef struct window   /*窓およびドア定義デ－タ       */
{
	char   *name,	 // 名称
		*Cidtype;
	// 入射角特性のタイプ。 'N':一般ガラス
	double  K, Rwall, Ei,
		Eo,          /*外表面輻射率（ドアのみ）*/
		tgtn,        /*日射総合透過率          */
		Bn,          /*吸収日射取得率          */
		as,          /*日射吸収率（ドアのみ）  */
		Ag,          /*窓ガラス面積            */
		Ao,          /*開口面積                */
		W, H;         /*巾、高さ                */
	char	RStrans;	// 室内透過日射が窓室内側への入射日射を屋外に透過する場合'y'
	int    end;
	//char	AirFlowFlg ;	// エアフローウィンドウなら'Y'
	//AIRFLOWCAT	AirFlowcat ;
} WINDOW;

typedef struct snbk    /* sun breaker */
{
	char    *name;
	int     type, ksi;
	double   W, H,
		D,
		W1, W2,
		H1, H2;
	int     end;
} SNBK;

/* ---------------------------------------------------------- */

typedef struct qrm    /*日射、室内発熱熱取得　　　　*/
{
	double  tsol,      /*透過日射*/
		asol,      /*外表面吸収日射室内熱取得*/
		arn,       /*外表面吸収長波長輻射熱損失*/
		hums,      /*人体顕熱*/
		light,     /*照明*/
		apls,      /*機器顕熱*/
		huml,      /*人体潜熱*/
		apll,      /*機器潜熱*/

		Qinfs,		/* 換気顕熱負荷[W] */
		Qinfl,		/* 換気潜熱負荷[W] */

		Qsto,		/* 室内の顕熱蓄熱量[W] */
		Qstol,		/* 室内の潜熱蓄熱量[W] */

		Qeqp,		/* 室内設置の配管、ボイラからの熱取得[W] */

		solo,      /* 外壁面入射日射量　W */
		solw,      /* 窓面入射日射量　W */
		asl,      /*外表面吸収日射 W*/
		hgins;     /* 室内発熱（顕熱）W */
	double	AE, AG;
	// 消費電力[W]、消費ガス[W]
} QRM;

/* ---------------------------------------------------------- */

typedef struct achir  /*室間相互換気  */
{
	int   rm,
		sch;
	struct room *room;
	double Gvr;
}  ACHIR;

/* ---------------------------------------------------------- */

typedef struct trnx  /*隣室　*/
{
	struct room  *nextroom;
	struct rmsrf *sd;        /* room側からみたnextroomと接する表面のポインター */
} TRNX;

/* ---------------------------------------------------------- */


typedef struct rpanel /* 室についての輻射パネル */
{
	struct rdpnl *pnl;
	struct rmsrf *sd;
	int  elinpnl;    /* 放射パネルの入力要素のs先頭位置 */
} RPANEL;

//typedef struct rairflow	// 室についてのエアフローウィンドウ
//{
//	struct airflow	*airflow ;
//	struct rmsrf	*sd ;
//	int				elinairflow ;
//						// エアフローウィンドウの入力要素の先頭位置
//} RAIRFLOW ;

/* ---------------------------------------------------------- */

typedef struct rdpnl  /*輻射パネル  */
{
	char         *name;
	char		 *loadt;
	char		 type;		// 建材一体型空気集熱器の場合：'C'
	// それ以外：'P'
	struct room  *rm[2];
	struct rmsrf *sd[2];
	struct compnt *cmp;
	int          MC,      /*専用壁のとき MC=1, 共用壁のとき MC=2 */
		Ntrm[2],
		Nrp[2],
		elinpnl[2],
		eprmnx,  /*隣室EPRN[]の位置*/
		epwtw;   /*EPWの当該パネル入口水温の位置*/
	char         control;

	double        effpnl,
		Toset,
		Wp,
		Wpold,
		Tpi,
		Tpo,
		FIp[2], FOp[2], FPp,
		Epw,
		EPt[2],
		*EPR[2],
		*EPW[2],
		EPC,
		Q;
	// 2009/01/26 Satoh Debug
	double cG;				// 比熱×流量
	double Ec, FI, FO, FP;
	double Ew;

	/* 日集計 */
	struct svday  Tpody;
	struct svday  Tpidy;
	struct qday   Qdy;
	struct qday	  Scoldy;
	struct qday   PVdy;
	struct svday  TPVdy;

	// 月集計
	SVDAY	mTpody, mTpidy, mTPVdy;
	QDAY	mQdy, mScoldy, mPVdy;
	EDAY	mtPVdy[12][24];

	struct omvav	*OMvav;	// 吹出を制御する変風量ユニット

} RDPNL;

/* ---------------------------------------------------------- */

typedef struct airsup   /* 室への冷温風供給熱量 */
{
	double  Qs,
		Ql,
		Qt,
		G, Tin, Xin;
	struct qday  Qdys,   /* 日積算暖冷房 */
		Qdyl,
		Qdyt;
	struct qday	 mQdys, mQdyl, mQdyt;
} AIRSUP;

/* ---------------------------------------------------------- */

typedef struct rmload   /* 室負荷 */
{
	char   *loadt,
		*loadx,
		tropt,    /* 室温制御方法  'o': OT制御、'a': 空気温度制御 */
		hmopt;    /* 湿度制御設定値
	'x': 絶対湿度、'r': 相対湿度、 'd': 露点温度 */
	double  Tset,
		Xset,
		Qs,
		Ql,
		Qt,

		FOTr,
		*FOTN,
		*FOPL,
		FOC;

	struct qday  Qdys,   /* 日積算暖冷房 */
		Qdyl,
		Qdyt;
	struct qday  mQdys, mQdyl, mQdyt;
} RMLOAD;

/* ---------------------------------------------------------- */

typedef struct rzone   /* ゾーン集計 */
{
	char  *name;
	int   Nroom;
	struct room **rm;
	double Afloor,
		Tr,
		xr,
		RH,
		Tsav,
		Qhs,
		Qhl,
		Qht,
		Qcs,
		Qcl,
		Qct;
	struct svday Trdy,
		xrdy,
		RHdy,
		Tsavdy;
	struct qday  Qsdy,
		Qldy,
		Qtdy;
} RZONE;

/* ---------------------------------------------------------- */
/* 要素別熱損失・熱取得計算用 */

typedef struct bhelm
{
	double  trs,  /* 貫流 */
		so,   /* 外壁入射日射 */
		sg,   /* 窓入射日射 */
		rn,   /* 大気放射 */
		in,   /* 室内発熱 */
		pnl;  /* 放射暖・冷房パネル */
} BHELM;

typedef struct qhelm
{
	struct bhelm  qe;
	double  slo,	/* 外壁面入射日射量　W */
		slw,		/* 窓面入射日射量　W */
		asl,		/* 外壁面吸収日射量 W */
		tsol,		/* 窓透過日射量　W */
		hins,		/* 室内発熱（顕熱） W */
		hinl,		/* 室内発熱（潜熱） */

		nx,			/* 隣室熱損失 */
		gd,			/* 窓熱損失 */
		ew,			/* 外壁熱損失 */
		wn,			/* 窓熱損失 */
		i,			/* 内壁熱損失 */
		c,			/* 天井、屋根 */
		f,			/* 床（内・外）*/
		vo,			/* 換気 */
		vol,		/* 換気（潜熱） */
		vr,			/* 室間換気 */
		vrl,		/* 室間換気（潜熱） */
		sto,		/* 室内空気蓄熱 */
		stol,		/* 室内空気蓄熱（潜熱） */
		loadh,
		loadhl,
		loadc,
		loadcl;
} QHELM;

typedef struct rmqelm
{
	struct rmsb   *rmsb;
	struct bhelm  *WSCwk;
	struct qhelm  qelm,
		qelmdy;
} RMQELM;

typedef struct rmsb
{
	char          type;  /* 'E':外気に接する面、'G':地面に接する面、'i':内壁 */
	struct bhelm  Ts,
		*Tw,
		*Told;
} RMSB;

typedef struct qetotal
{
	char          *name;
	struct qhelm  qelm,
		qelmdy;
} QETOTAL;

typedef struct room				/*室デ－タ */
{
	char    *name;			/*室名     */
	int     N,					/*周壁数   */
		brs;					/*Sd[],S[]の先頭位置*/
	double	*MCAP, *CM, TM, oldTM, HM, QM;
	double	*fsolm;				// 家具の日射吸収割合
	double	Srgm2;				// 家具の最終的な日射吸収割合
	double	Qsolm;				// 家具の日射吸収量[W]
	char	*PCMfurnname;		// PCM内臓家具の部材名称
	PCM		*PCM;				// PCM内臓家具の場合
	double	mPCM;				// PCM内臓家具の容積[m3]
	double	PCMQl;				// PCM内臓家具の見かけの比熱[J/m3K]
	double	FunHcap;			// 家具の熱容量（顕熱とPCM内臓家具の合計）
	// 室空気に加算したものは除く
	double	FMT, FMC;
	// 家具の計算用パラメータ
	double	Qgt;				// 透過日射熱取得
	int     Nachr,
		Ntr,					/*内壁を共有する隣室数*/
		Nrp,					/*輻射パネル数 */
		Nflr,					/*床の部位数 */
		Nfsolfix,				/*短波長放射吸収比率が定義されている面数*/
		Nisidermpnl,
		Nasup;
	//int	Nairflow ;				// エアフローウィンドウの総数

	RMSRF		*rsrf;
	ACHIR		*achr;
	TRNX		*trnx;
	RPANEL		*rmpnl;
	//RAIRFLOW	*rairflow ;
	AIRSUP		*arsp;
	struct compnt	*cmp;
	struct elin	*elinasup,
		*elinasupx;
	RMLOAD		*rmld;
	RMQELM		*rmqe;

	double   *F,
		*alr;
	double  *XA,
		*Wradx;

	char    rsrnx,     /*隣室裏面の短波長放射考慮のとき 'y'　（床、天井のみ） */
		fij,       /*形態係数 'F':外部入力、'A':面積率　*/
		sfpri,     /*表面温度出力指定 'p' */
		eqpri,     /*日射、室内発熱取得出力指定 'p' */
		mrk;       /* '*', 'C', '!'   */

	double   VRM,       /*室容積      */
		GRM,       /*室内空気質量*/
		MRM,       /*室空気熱容量*/
		Area,      /*室内表面総面積*/
		FArea,     /*床面積　　　　*/
		*flrsr,     /*床に直接吸収される短波長放射の比率*/
		tfsol,		/*部位に直接吸収される短波長放射比率の既定値合計（Sd->fsol、Rm->flrsr、Rm->fsolmの合計）*/
		alrbold,  /*		*/

		Hcap,      /* 室内熱容量   J/K */
		Mxcap;     /* 室内湿気容量 kg/(kg/kg) */

	char    Ltyp;      /*照明器具形式*/
	double   Nhm,       /*人数*/
		Light,     /*照明器具容量*/
		Apsc,      /*機器対流放熱容量*/
		Apsr,      /*機器輻射放熱容量*/
		Apl,       /*機器潜熱放熱容量*/
		Gve,       /*換気量*/
		Gvi,       /*隙間風量*/
		AE,			// 消費電力容量[W]
		AG,			// 消費ガス容量[W]
		*AEsch,		// 消費電力スケジュール
		*AGsch,		// 消費ガススケジュール
		*Lightsch,
		*Assch,
		*Alsch,
		*Hmsch,
		*Metsch,
		*Closch,
		*Wvsch,
		*Hmwksch,
		*Vesc,
		*Visc,
		*alc;      /*室内側対流熱伝達率のスケジュール設定値 */

	//int     vesc,      /*換気設定番号*/
	//	visc ;      /*隙間風設定番号*/
	//lgtsc,     /*照明設定番号*/
	//hmnsc,     /*在室人数設定番号*/
	//hmwksc,    /*在室者作業強度設定番号*/
	//apssc,     /*機器顕熱発熱設定番号*/
	//aplsc,     /*機器潜熱発熱設定番号*/

	/*温熱環境指標計算用*/
	//metsc,     /*代謝量（met)設定番号*/
	//closc,     /*着衣量設定番号*/
	//wvsc;      /*室内風速設定番号*/

	/*室内発熱*/
	double   Hc,         /*人体よりの対流  [W]  */
		Hr,         /*人体よりの輻射  [W]  */
		HL,         /*人体よりの潜熱　[W]  */
		Lc,         /*照明よりの対流  [W]  */
		Lr,         /*照明よりの輻射  [W]  */
		Ac,         /*機器よりの対流  [W]  */
		Ar,         /*機器よりの輻射  [W]  */
		AL;         /*機器よりの潜熱  [W]  */

	/*設備機器発熱*/
	double   eqcv,       /*設備機器発熱の対流成分比率*/
		Qeqp;      /*設備機器からの発熱[W] */
	double   Gvent;

	double   RMt,
		*ARN,
		*RMP,
		RMC,
		RMx,
		RMXC;

	double   Tr,
		Trold,
		xr,
		xrold,
		RH,
		Tsav,
		Tot,
		hr,				// エンタルピー
		PMV;
	double	SET;
	int		setpri;

	struct svday  Trdy,
		xrdy,
		RHdy,
		Tsavdy;

	SVDAY	mTrdy, mxrdy, mRHdy, mTsavdy;
	int     end;
	struct vav	*VAVcontrl;
	double	*OTsetCwgt;	// 作用温度設定時の対流成分重み係数
	// デフォルトは0.5
	double	HGc, CA, AR;
	double	Qsab,			// 吸収日射熱取得
		Qrnab;				// 夜間放射による熱損失
} ROOM;

/* --------------------------------
室計算用データ
--------------------------------*/

typedef struct rmvls
{
	double			Twallinit;
	int				Nroom, Nrdpnl, Nwall, Nwindow, Nmwall, Nsrf;
	int				Npcm;
	//int				Nairflow ;
	//	char			Emrk[ROOMMX] ;
	char			*Emrk;
	struct wall		*Wall;
	struct window	*Window;
	struct snbk		*Snbk;
	PCM				*PCM;
	//	struct rmsrf   Sd[NSMX];
	RMSRF			*Sd;
	//	struct mwall	Mw[MWALLMX] ;
	struct mwall	*Mw;
	//	struct room    Room[ROOMMX];
	ROOM			*Room;
	struct rdpnl	*Rdpnl;
	//struct airflow	*airflow ;
	struct qrm		*Qrm, *Qrmd;
	struct qetotal	Qetotal;
	double			*Trdav;
	char			pcmiterate;			// PCM建材を使用し、かつ収束計算を行う場合は'y'
} RMVLS;
