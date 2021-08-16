#pragma once

#include <stdio.h>

//DEBUGフラグ
extern int DEBUG;

//エラー出力用ファイルポインタ
extern FILE	*ferr;

//円周率
extern double PI;

// 月の末日かどうかをチェックする
int	isEndDay(int Mon, int Day, int Dayend, int SimDayend);



#define  SCHAR    1024
//#define  NCHAR	  128

//#define  IDCHRMX  51


#define NOP   -1
#define FNOP  -999.9
#define TEMPLIMIT -273.16

#define  ERRFMT  "xxxxx %s xxxxx : "



//#define  ERSTR   101
#define  ERRFMTA  "xxx %s xxx %s\n"
