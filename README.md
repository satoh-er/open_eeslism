2021年10月1日

# EESLISMのソースコード公開にあたって


## １．概要
EESLISMは1990年以来開発を行ってきた建築、設備の両方で構成される建築熱環境制御システムの汎用シミュレーションプログラムの名称です。EESLISIM 2.0までは、類型化したサブシステムの組み合わせにより汎用的なシステムのシミュレーションを行うものでしたが、EESLISM 3.5では、システムを構成する建築要素である室や設備の要素機器などのシステム構成要素の定義と要素機器の接続関係と制御方法の指定により、自由に構成されたシステムのシミュレーションを可能としました。EESLISM4.5は、さらに、仮想機器などシステム熱負荷シミュレーションの機能を加えたもので1998年から使用されています。EESLISM5.0は、経路内の流量の扱いに関する機能を拡張しました。
EESLISMは4.5までは、EWS(HP9000/425,HP9000/715およびHP B132L)を用いて開発してきました。オペレーティングシステムはUNIX (HPUX)、使用した言語はＣ言語（ANSI C）です。シミュレーション実行時の入力、出力は文字ファイル形式であるため、他のコンピュータシステムを使用する場合でも移植が容易です。
2001年からは、パーソナルコンピュータの目覚しい普及に対応するため、開発環境をWindowsのVisual C++にも対応させ、EESLISM5.0としました。EESLISM6.0は外部障害物の影による直達日射の減少や天空に対する形態係数の減少による放熱の減少など、周囲環境の影響を考慮できる機能を追加しました。EESLISM6.5は仮想空調機の機能を強化し、EESLISM6.6では太陽光発電や屋根一体型太陽熱集熱器を追加しました。さらに、EESLISM7.0は空気式太陽熱集熱システムの風量制御機能や月間値の出力機能等を強化しました。EESLISM 7.2では、太陽熱利用システムの集熱制御についての機能を強化しました。
2018年には、壁体内部に設置する潜熱蓄熱建材の計算が行えるような機能拡張を加えEESLISM7.3とし、今回、公開することといたしました。

これまでの主な開発状況は以下の通りです。

1989年EESLISM
1992年EESLISM 2.0
1996年EESLISM 3.5
1998年EESLISM 4.5
2001年EESLISM 5.0 (4.6)
2003年EESLISM 5.3
2008年EESLISM 6.0
2010年EESLISM 6.5
2011年EESLISM 6.6
2012年EESLISM 7.0
2014年 EESLISM 7.2
2018年 EESLISM 7.3

## 2.ライセンス
EESLISMの著作権は、3に示す著者らに帰属します。このプログラムはフリーソフトウェアです。利用者は、GNU General Public License version2の条件の下で、それを変更したり、再配布することができます。
EESLISMが広く皆様の役立つことを期待して配布されていますが、まだ把握していない不具合が潜んでいる可能性があります。本プログラムを使用することにより生じた一切の責任は,ユーザー自身が負うものとし、弊会では関知しかねます。詳細については、GNU General Public Licenseを参照してください。

Distributed under the GNU General Public License (GPL), version 2.
See LICENSE for more information. http://www.gnu.org/licenses/gpl-2.0.html

## 3．著者
EESLISM研究会
宇田川 光弘（工学院大学名誉教授）
佐藤 誠（佐藤エネルギーリサーチ株式会社　代表取締役）
樋口 佳樹（日本工業大学建築学科教授）


## 4.　開発環境と動作環境

EESLISM 7.3はMicrosoft社のOS環境であるWindows上にて動作するように作成しています。開発環境はWindows10 ProfessionalとMicrosoft Visual C++ 2019です。

EESLISM 7.3の起動には、おおよそ10 MB程度のメモリを必要とします。ディスク容量は、気象データや計算結果を収録するスペースが別途必要となります。また、実際にシミュレーションする場合には、計算するシステムのサイズに応じた空きメモリも必要となります。

動作を確認しているOSは、以下のとおりです。
Windows10
ただし、公開時点においてコンパイラの最適化オプションによっては計算の再現性に関する不具合が確認されています。ご注意ください。


## 5．フォルダ構成

Home
　|_ Source  		EESLISMのソースファイル
　|_Base			EESLISMの基礎データファイル
  |_Manual		入力ファイルの作成マニュアル
  |_Sample		サンプルの入力ファイル

### 5.1　Sourceフォルダ
以下のソースプログラムが収録されています。
CAT.c		CINC.c		COORDNT.c	DAINYUU.c	DATAIN.c
EOP.c	ERRPRINT.c	GDATA.c	GRPOINT.c	HOUSEN.c
INOROUT.c	INPUT.h	KAUNT.c	KOUTEN.c	LPOP_placement.c
MAT_INIT.c	MONTE_CARLO.c	NENNKANN.c	OPIhor.c	PRA.c
RAND.c	SHADOW.c	STRCUT.c	URA.c	YOGEN.c
ZAHYOU.c	ZPRINT.c			
bdhpri.c	blhcflib.c	blhelm.c		blhelmlib.c	
blinit.c	blpanel.c	blpcm.c	blpvsystem.c	blrmaceqcf.c
blrmdata.c	blrmprint.c	blrmqprt.c	blrmresid.c	blrmschd.c
blrmvent.c	blroom.c	blroomcf.c	blroomday.c	blroomene.c
blroomvar.c	blroomvptr.c	blrzone.c	blsnbklib.c	blsolarwall.c
blsrprint.c	bltcomfrt.c	blwall.c	dbgpri1.c	dbgpri2.c
dbgprip1.c	e79_pcm.c	eebslib.c	eecmpdat_s.c	eecmpday_s.c
eecmpprt_s.c	eecpprint.c	eedayslib.c	eefiles.c	eegndata.c
eeinput_s.c	eenvptr.c	eepathdat.c	eepathlib_s.c	eepflow.c
eeprint_s.c	eepthene.c	eepthprt.c	eerthlib.c	eespre.c
eesyselmalc_s.c
eevcdat.c	eesyseqv.c
epsinput.c	eesysupv_s.c
eschdata.c	eesysvar.c
eschdlr_s.c	eettlprt.c
eschval.c
escntldat.c	escntllb_s.c	escntlsch_s.c	escntrlfactor.c	esidcode_s.c
evcwdfl.c	mcaclib.c	mcboiler.c	mccnvrg.c	mcdesiccant.c
mceqcadat.c	mceqpcat.c	mcevcooling.c	mchccoil.c	mchcload.c
mchexchgr.c	mcmecsys.c	mcomvav.c	mcpipe.c	mcpump.c
mcpv.c	mcqmeas.c	mcrefas.c	mcreflib.c	mcsolrcol.c
mcstank.c	mcstanklb.c	mcstheat.c	mcthex.c	mcvalv.c
mcvav.c	mcxprmcs.c	memoryfree.c	spline.c	u_ees3lb.c
u_inv.c	u_minmax.c	u_mlib.c	u_psy.c	u_sun.c
valinit.c	waterandair.c	wdprint.c	wdread.c	
FUNC.h		MODEL.h	blhys_pcm.h	build.h	buildex.h
control.h	daysum.h	eepath.h	egcalc.h	eps.h
eqptyp.h	esize.h	estr.h	exs.h	fbld.h
fesy.h	fmcs.h	fnbld.h	fnbldg.h	fnesy.h
fnfio.h	fnlib-sv2.h	fnlib.h	fnmcs.h	mfree.h
psy.h	sched.h	simcon.h	sun.h	waterandair.h
winerror.h	wthrd.h			


### 5-2. Baseフォルダ
EESLISMの実行には、建物データ以外の基礎データファイルとして、材料・機器特性、曜日設定、給水温・地中温、気象に関するファイルが必要です。気象データファイル以外のデータファイルはシミュレーションの実行時に読み込まれるときの名前はあらかじめ次のように決められています。
それぞれのファイルを作成する場合は、Manual\ESLISM7.2入力データ作成マニュアル.pdfを参照してください。

Base\
|＿ wbmlist.efl 	壁材料リスト 
|＿ reflist.efl 	圧縮機特性リスト 
|＿ dayweek.efl 	曜日設定 
|＿ supwfile.efl 	給水温度、地中温度

気象データファイルは、80バイト×7レコードを１日分とするカードイメージであるＨＡＳＰ用気象データの書式をそのまま用い、先頭に80バイトの地名、緯度、経度などのデータを追加した独自の形式です。作成方法は、Manual\ESLISM7.2入力データ作成マニュアル.pdfを参照してください。

### 5-3．Manualフォルダ
入力ファイル(Inputfile)の作成方法は、以下のファイルに準じて作成して下さい。

Manual\
|＿ EESLISM7.2入力データ作成マニュアル.pdf

## 6．実行
コマンドラインから、コンパイルで作成された実行ファイルを指定し、いくつかの引数を指定します。以下に、Windows環境を想定した時の実行コマンドの例を示します。

eeslis78.exe␣Inpufile␣[curpath]␣[-nstop]␣[-delay]
[]は省略可能

Inputifle：入力ファイル
curpath：カレントフォルダ
-nstop：不具合があった場合に、入力待ちで止まるのを回避します
-delay：表示の遅れを回避するときに使用してください。（-delayが設定されるとsetvbuf(stdout, (char *)NULL, _IONBF, 0);が有効になります）

Inputfile、curpath中に半角スペースが含まれる場合は、""で囲ってください



