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

/*   binit.c   */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include "common.h"
#include "fesy.h"
#include "esize.h"
#include "fnbld.h"
#include "lib/u_psy.h"
#include "fnlib.h"
#include "fnfio.h"

/*  壁体デ－タの入力  */

void PCMdata(FILE *fi, char *dsn, PCM **pcm, int *Npcm, char *pcmiterate)
{
	static char   s[SCHAR], *st, *ce, E[SCHAR];
	double  dt;
	//char	c;
	PCM		*PCMa;
	int		N, j;

	PCMa = NULL;
	sprintf_s(E, sizeof(E), ERRFMT, dsn);

	N = PCMcount(fi);

	strcpy_s(s, SCHAR, "PCMdata --");

	if (N > 0)
	{
		if ((*pcm = (PCM *)malloc(N * sizeof(PCM))) == NULL)
			Ercalloc(N, s);

		PCMa = *pcm;
		for (j = 0; j < N; j++, PCMa++)
		{
			PCMa->name = NULL;
			PCMa->Condl = PCMa->Conds = PCMa->Crol = PCMa->Cros = PCMa->Ql = PCMa->Tl = PCMa->Ts = PCMa->Tp = -999.0;
			PCMa->iterate = 'n';	// 収束なしがデフォルト
			//PCMa->iterateTemp = 'n';	// デフォルトは見かけの比熱だけで収束
			PCMa->iterateTemp = 'y';
			PCMa->nWeight = 0.5;	// 収束計算時の現在ステップ温度の重み係数
			PCMa->AveTemp = 'y';
			PCMa->DivTemp = 1;
			PCMa->Ctype = 2;		// 二等辺三角形がデフォルト
			// パラメータの初期化
			PCMa->PCMp.a = PCMa->PCMp.B = PCMa->PCMp.b = PCMa->PCMp.bl
				= PCMa->PCMp.bs = PCMa->PCMp.c = PCMa->PCMp.d = PCMa->PCMp.e
				= PCMa->PCMp.f = PCMa->PCMp.omega = PCMa->PCMp.skew = PCMa->PCMp.T = -999.;
			PCMa->IterateJudge = 0.05;		//	収束判定条件は前ステップ見かけの比熱の5%
			PCMa->spctype = 'm';			// モデル形式をデフォルトとする
			PCMa->condtype = 'm';
			int i;
			CHARTABLE *ct;
			ct = PCMa->chartable;
			ct[0].PCMchar = 'E';			// 引数0はエンタルピー
			ct[1].PCMchar = 'C';			// 引数1は熱伝導率
			for (i = 0; i < 2; i++, ct++)
			{
				ct->itablerow = 0;
				ct->T = ct->Chara = NULL;
				ct->filename = NULL;
				ct->tabletype = 'e';
				ct->fp = NULL;
				ct->lowA = ct->lowB = ct->upA = ct->upB = 0.0;
				ct->minTempChng = 0.5;		// 最低温度変動幅
			}
		}
	}

	PCMa = *pcm;
	while (fscanf_s(fi, " %s ", s, sizeof(s)) != EOF)
	{
		//printf( "<WAlldata> 1.... s=%s\n", s ) ;

		if (*s == '*')
			break;

		PCMa->name = stralloc(s);							// PCM名称

		while (fscanf_s(fi, "%s", s, sizeof(s)), *s != ';')
		{
			//printf ( "<Walldata> 2..... s=%s\n", s ) ;

			ce = strchr(s, ';');

			if (ce)
				*ce = '\0';
			if ((st = strchr(s, '=')) != NULL)
			{
				dt = atof(st + 1);
				if (strncmp(s, "Ql", 2) == 0)					// 潜熱量[J/m3]
					PCMa->Ql = dt;
				else if (strncmp(s, "spcheattable", 12) == 0)
				{
					PCMa->chartable[0].filename = stralloc(st + 1);
					PCMa->spctype = 't';
				}
				else if (strncmp(s, "table", 5) == 0)
				{
					if (*(st + 1) == 'e')
						PCMa->chartable[0].tabletype = 'e';
					else if (*(st + 1) == 'h')
						PCMa->chartable[0].tabletype = 'h';
				}
				else if (strncmp(s, "conducttable", 12) == 0)
				{
					PCMa->chartable[1].filename = stralloc(st + 1);
					PCMa->condtype = 't';
				}
				else if (strncmp(s, "minTempChange", 13) == 0)
					PCMa->chartable[0].minTempChng = PCMa->chartable[1].minTemp = dt;
				else if (strncmp(s, "Condl", 5) == 0)			// 液相熱伝導率[W/mK]
					PCMa->Condl = dt;
				else if (strncmp(s, "Conds", 5) == 0)			// 固相熱伝導率[W/mK]
					PCMa->Conds = dt;
				else if (strncmp(s, "Crol", 4) == 0)			// 液相容積比熱[J/m3K]
					PCMa->Crol = dt;
				else if (strncmp(s, "Cros", 4) == 0)			// 固相容積比熱[J/m3K]
					PCMa->Cros = dt;
				else if (strncmp(s, "Tl", 2) == 0)			// 液体から凝固が始まる温度[℃]
					PCMa->Tl = dt;
				else if (strncmp(s, "Ts", 2) == 0)			// 固体から融解が始まる温度[℃]
					PCMa->Ts = dt;
				else if (strncmp(s, "Tp", 2) == 0)				// 見かけの比熱のピーク温度[℃]
					PCMa->Tp = dt;
				else if (strncmp(s, "DivTemp", 7) == 0)		// 比熱数値積分時の温度分割数
					PCMa->DivTemp = (int)dt;
				else if (strncmp(s, "Ctype", 5) == 0)		// 見かけの比熱の特性曲線番号
					PCMa->Ctype = (int)dt;
				// これ以降は見かけの比熱計算のためのパラメータ
				else if (strncmp(s, "a", 1) == 0)
					PCMa->PCMp.a = dt;
				else if (strncmp(s, "b=", 2) == 0)
					PCMa->PCMp.b = dt;
				else if (strncmp(s, "c", 1) == 0)
					PCMa->PCMp.c = dt;
				else if (strncmp(s, "d", 1) == 0)
					PCMa->PCMp.d = dt;
				else if (strncmp(s, "e", 1) == 0)
					PCMa->PCMp.e = dt;
				else if (strncmp(s, "f", 1) == 0)
					PCMa->PCMp.f = dt;
				else if (strncmp(s, "B", 1) == 0)
					PCMa->PCMp.B = dt;
				else if (strncmp(s, "T", 1) == 0)
					PCMa->PCMp.T = dt;
				else if (strncmp(s, "bs", 2) == 0)
					PCMa->PCMp.bs = dt;
				else if (strncmp(s, "bl", 2) == 0)
					PCMa->PCMp.bl = dt;
				else if (strncmp(s, "skew", 4) == 0)
					PCMa->PCMp.skew = dt;
				else if (strncmp(s, "omega", 5) == 0)
					PCMa->PCMp.omega = dt;
				else if (strncmp(s, "nWeight", 7) == 0)
					PCMa->nWeight = dt;
				else if (strncmp(s, "IterateJudge", 12) == 0)
					PCMa->IterateJudge = dt;
				else
					Eprint("<PCMdata>", s);
			}
			else
			{
				if (strcmp(s, "-iterate") == 0)
				{
					PCMa->iterate = 'y';
					*pcmiterate = 'y';
				}
				//else if (strcmp(s, "-iterateTemp") == 0)
				//	PCMa->iterateTemp = 'y';
				else if (strcmp(s, "-pcmnode") == 0)
					PCMa->AveTemp = 'n';
				else
					Eprint("<PCMdata>", s);
			}

			if (ce) break;
		}

		// テーブルの読み込み（見かけの比熱）
		if (PCMa->spctype == 't')
			TableRead(&PCMa->chartable[0]);

		// テーブルの読み込み（熱伝導率）
		if (PCMa->condtype == 't')
			TableRead(&PCMa->chartable[1]);

		// 入力情報のチェック
		if (PCMa->spctype == 'm')
		{
			int	Tin, Bin, bsin, blin, skewin, omegain, ain, bin, cin, din, ein, fin;
			int	Qlin, Condlin, Condsin, Crolin, Crosin, Tlin, Tsin, Tpin;
			int	dparaminit(double A);
			Tin = dparaminit(PCMa->PCMp.T);
			Bin = dparaminit(PCMa->PCMp.B);
			bsin = dparaminit(PCMa->PCMp.bs);
			blin = dparaminit(PCMa->PCMp.bl);
			skewin = dparaminit(PCMa->PCMp.skew);
			omegain = dparaminit(PCMa->PCMp.omega);
			ain = dparaminit(PCMa->PCMp.a);
			bin = dparaminit(PCMa->PCMp.b);
			cin = dparaminit(PCMa->PCMp.c);
			din = dparaminit(PCMa->PCMp.d);
			ein = dparaminit(PCMa->PCMp.e);
			fin = dparaminit(PCMa->PCMp.f);
			Qlin = dparaminit(PCMa->Ql);
			Condlin = dparaminit(PCMa->Condl);
			Condsin = dparaminit(PCMa->Conds);
			Crolin = dparaminit(PCMa->Crol);
			Crosin = dparaminit(PCMa->Cros);
			Tlin = dparaminit(PCMa->Tl);
			Tsin = dparaminit(PCMa->Ts);
			Tpin = dparaminit(PCMa->Tp);

			// 必須入力項目のチェック
			if (Condlin + Condsin + Crolin + Crosin + Tlin + Tsin != 0)
			{
				printf("<PCMdata> name=%s Condl=%lf Conds=%lf Crol=%lf Cros=%lf Tl=%lf Ts=%lf\n",
					PCMa->name, PCMa->Condl, PCMa->Conds, PCMa->Crol, PCMa->Cros, PCMa->Tl, PCMa->Ts);
			}

			// モデルごとに入力値をチェックする
			if (PCMa->Ctype == 1 || PCMa->Ctype == 2)
			{
				if (Qlin + Tsin + Tlin != 0)
					printf("<PCMdata> name=%s Ql=%lf Ts=%lf Tl=%lf\n",
						PCMa->name, PCMa->Ql, PCMa->Ts, PCMa->Tl);
			}
			else if (PCMa->Ctype == 3)
			{
				if (Qlin + Tpin + Tin + Bin != 0)
					printf("<PCMdata> name=%s Ql=%lf Tp=%lf T=%lf B=%lf\n",
						PCMa->name, PCMa->Ql, PCMa->Tp, PCMa->PCMp.T, PCMa->PCMp.B);
			}
			else if (PCMa->Ctype == 4)
			{
				if (Tpin + ain + bin != 0)
					printf("<PCMdata> name=%s Tp=%lf a=%lf b=%lf\n",
						PCMa->name, PCMa->Tp, PCMa->PCMp.a, PCMa->PCMp.b);
			}
			else if (PCMa->Ctype == 5)
			{
				if (Tpin + bsin + blin + ain != 0)
					printf("<PCMdata> name=%s Tp=%lf bs=%lf bl=%lf a=%lf\n",
						PCMa->name, PCMa->Tp, PCMa->PCMp.bs, PCMa->PCMp.bl, PCMa->PCMp.a);
			}
			else if (PCMa->Ctype == 6)
			{
				if (Qlin + Tpin + skewin + omegain != 0)
					printf("<PCMdata> name=%s Ql=%lf Tp=%lf skew=%lf omega=%lf\n",
						PCMa->name, PCMa->Ql, PCMa->Tp, PCMa->PCMp.skew, PCMa->PCMp.omega);
			}
			else if (PCMa->Ctype == 7)
			{
				if (ain + bin + cin + din + ein + fin != 0)
					printf("<PCMdata> name=%s a=%lf b=%lf c=%lf d=%lf e=%lf f=%lf\n",
						PCMa->name, PCMa->PCMp.a, PCMa->PCMp.b, PCMa->PCMp.c, PCMa->PCMp.d, PCMa->PCMp.e, PCMa->PCMp.f);
			}
		}
		PCMa++;
	}

	*Npcm = (int)(PCMa - *pcm);
}

// PCMの物性値テーブルの読み込み
void TableRead(CHARTABLE *ct)
{
	if (ct->filename != NULL)
	{
		if (fopen_s(&(ct->fp), ct->filename, "r") != 0)
			printf("<PCMdata> xxxx file not found %s xxxx\n", ct->filename);
		else
		{
			// 設定されている行数を数える
			char c;
			int	 row;
			row = 0;
			while (fscanf_s(ct->fp, "%c", &c, 1) != EOF)
			{
				if (c == '\n')
					row++;
			}
			// 一旦ファイルを閉じる
			fclose(ct->fp);
			// メモリの確保
			ct->T = dcalloc(row, "<TableRead> ct->T");
			ct->Chara = dcalloc(row, "<TableRead> ct->Char");
			if (ct->T == NULL || ct->Chara == NULL)
				printf("<PCMdata> メモリの確保に失敗\n");
			// 再度ファイルを開く
			fopen_s(&ct->fp, ct->filename, "r");
			int i;
			int st = 0, tt = 0;
			double spheat, prevheat, prevTemp;
			double *T, *Char;
			T = ct->T;
			Char = ct->Chara;
			prevheat = 0.0;
			prevTemp = -999.;
			spheat = 0.0;
			for (i = 0; i < row; i++, T++, Char++)
			{
				// 温度の読み込み
				fscanf_s(ct->fp, " %lf ", T);
				// テーブルの下限温度
				if (st == 0)
				{
					ct->minTemp = *T;
					prevTemp = *T;
					st = 1;
				}
				// 昇順に並んでいないときのエラー表示
				if (*T <= prevTemp && tt == 1)
				{
					printf("i=%d 温度データが昇順に並んでいません T=%lf preT=%lf\n",
						i, *T, prevTemp);
				}
				double dblTemp;
				// 特性値の読み込み
				fscanf_s(ct->fp, " %lf ", &dblTemp);
				*Char = dblTemp;
				// 見かけの比熱の場合
				if (ct->tabletype == 'h')
					*Char = prevheat + spheat * (*T - prevTemp);
				prevheat = *Char;
				prevTemp = *T;
				spheat = dblTemp;
				// テーブルの上限温度
				ct->maxTemp = *T;
				//printf("i=%d T=%lf enthalpy=%lf\n", i, *T, *Char);
				ct->itablerow++;
				tt = 1;
			}
			fclose(ct->fp);
			// 上下限温度範囲以外の線形回帰式を作成
			// 下限以下
			ct->lowA = (*ct->Chara - *(ct->Chara + 1)) / (*ct->T - *(ct->T + 1));
			ct->lowB = *ct->Chara - ct->lowA**ct->T;
			// 上限以上
			ct->upA = (*(ct->Chara + ct->itablerow - 1) - *(ct->Chara + ct->itablerow - 2))
				/ (*(ct->T + ct->itablerow - 1) - *(ct->T + ct->itablerow - 2));
			ct->upB = *(ct->Chara + ct->itablerow - 1) - ct->upA**(ct->T + ct->itablerow - 1);
		}
	}
}

// 初期化されているかをチェックする
int	dparaminit(double A)
{
	return(fabs(A - (-999.)) < 1.e-5);
}

int		PCMcount(FILE *fi)
{
	int		N = 0;
	long	add;
	char	s[SCHAR];

	add = ftell(fi);

	while (fscanf_s(fi, " %s ", s, sizeof(s)) != EOF)
	{
		if (*s == '*')
			break;
		if (*s == ';')
			N++;
	}

	fseek(fi, add, SEEK_SET);
	return (N);
}

// 固相、液相物性値と潜熱量からPCM温度の物性値を計算する（比熱、熱伝導率共通）
// 熱伝導率の計算時はQl=0とする
double	FNPCMState(int Ctype, double Ss, double Sl, double Ql, double Ts, double Tl, double Tp, double T, PCMPARAM PCMp)
{
	double	Qse, Qla, Tls;

	Tls = Tl - Ts;
	// 顕熱分の補間
	if (T > Ts && T < Tl)
		Qse = Ss + (Sl - Ss) / Tls * (T - Ts);
	else if (T <= Ts)
		Qse = Ss;
	else
		Qse = Sl;

	// 潜熱分の補間
	Qla = 0.;
	// 熱伝導率計算の場合は潜熱ゼロ
	if (Ctype == 0)
		Qla = 0.;
	// 潜熱変化域潜熱比熱一定
	else if (Ctype == 1)
	{
		if (T > Ts && T < Tl)
			Qla = Ql / (Tl - Ts);
	}
	// 二等辺三角形
	else if (Ctype == 2)
	{
		if (T > Ts && T < Tl)
		{
			//Qla = Ql / Tls ;
			if (T < (Tl + Ts) / 2.)
				Qla = 4. * Ql / (Tls * Tls) * (T - Ts);
			else
				Qla = 4. * Ql / (Tls * Tls) * (Tl - T);
		}
	}
	// 双曲線関数
	else if (Ctype == 3)
	{
		double	Temp;
		Temp = cosh((2.0*PCMp.B / PCMp.T)*(T - Tp));
		Qla = 0.5*Ql*(2.0*PCMp.B / PCMp.T) / (Temp*Temp);
	}
	// ガウス関数（対象）
	else if (Ctype == 4)
	{
		double	Temp;
		Temp = (T - Tp) / PCMp.b;
		Qla = PCMp.a*exp(-0.5*Temp*Temp);
	}
	// ガウス関数（非対称）
	else if (Ctype == 5)
	{
		double	Temp;
		Temp = (T <= Tp) ? PCMp.bs : PCMp.bl;
		Qla = PCMp.a*exp(-((T - Tp) / Temp)*((T - Tp) / Temp));
	}
	// 誤差関数歪度
	else if (Ctype == 6)
	{
		double	Temp;
		Temp = exp(-(T - Tp)*(T - Tp) / ((2.*PCMp.omega)*PCMp.omega));
		Qla = Ql / sqrt(2.*PI)*exp(-(T - Tp)*(T - Tp) / ((2.*PCMp.omega)*PCMp.omega))
			* (1. + erf(PCMp.skew*(T - Tp) / (sqrt(2.)*PCMp.omega)));
	}
	// 有理関数
	else if (Ctype == 7)
	{
		if (T < 0)
			Qla = 0.0;
		else
			Qla = pow(T, PCMp.f)*(PCMp.a*T*T + PCMp.b*T + PCMp.c) / (PCMp.d*T*T + PCMp.e*T + 1.);
	}

	return(Qse + Qla);
}

/* ------------------------------------------------------ */

// PCMの状態値計算（家具用）
double	FNPCMStatefun(int Ctype, double Ss, double Sl, double Ql, double Ts, double Tl, double Tp, double oldT, double T, int DivTemp, PCMPARAM PCMp)
{
	int	i;
	double	dblTemp = 0.0, dTemp, TPCM;
	double	Qld, Qllst;

	dTemp = (T - oldT) / (double)(DivTemp);
	//double	MinTemp, MaxTemp;

	//MinTemp = dmin(oldT, T);
	//MaxTemp = dmax(oldT, T);
	Qllst = 0.0;
	// 前時刻からの温度変化が小さい場合は時間積分はしない
	if (fabs(T - oldT) < 1.e-4)
		Qllst = FNPCMState(Ctype, Ss, Sl, Ql, Ts, Tl, Tp, (T + oldT)*0.5, PCMp);
	else
	{
		// 見かけの比熱の時間積分
		for (i = 0; i < DivTemp + 1; i++)
		{
			TPCM = oldT + dTemp * (double)i;
			Qld = FNPCMState(Ctype, Ss, Sl, Ql, Ts, Tl, Tp, TPCM, PCMp);
			dblTemp += Qld;
			//printf("TPCM=%lf  Ql=%lf\n", TPCM, Qld);
		}

		Qllst = dblTemp / (double)(DivTemp + 1);
	}

	return(Qllst);
}


// PCMの温度から見かけの比熱を求める（テーブル形式）
// Told:前時刻のPCM温度、T:暫定現在時刻PCM温度
double FNPCMstate_table(CHARTABLE *ct, double Told, double T, int Ndiv)
{
	double oldEn, En, Chara;

	// 前時刻と暫定現在時刻のPCM温度が同温の場合
	if (fabs(T - Told) < ct->minTempChng)
	{
		double	Tave;
		Tave = 0.5*(T + Told);
		// 見かけの比熱の計算
		if (ct->PCMchar == 'E')
		{
			oldEn = FNPCMenthalpy_table_lib(ct, Tave - 0.5*ct->minTempChng);
			En = FNPCMenthalpy_table_lib(ct, Tave + 0.5*ct->minTempChng);
			// 見かけの比熱の計算
			Chara = (En - oldEn) / ct->minTempChng;
		}
		// 熱伝導率の計算
		else
			Chara = FNPCMenthalpy_table_lib(ct, Tave);
	}
	else
	{
		if (ct->PCMchar == 'E')
		{
			oldEn = FNPCMenthalpy_table_lib(ct, Told);
			En = FNPCMenthalpy_table_lib(ct, T);
			// 見かけの比熱の計算
			Chara = (En - oldEn) / (T - Told);
		}
		// 熱伝導率の計算
		else
		{
			int i;
			double dblTemp, Tpcm, dTemp;
			dblTemp = 0.0;
			dTemp = (T - Told) / (double)(Ndiv);
			// ToldからTまでを積分する
			for (i = 0; i < Ndiv; i++)
			{
				Tpcm = Told + dTemp * (double)i;
				dblTemp += FNPCMenthalpy_table_lib(ct, Tpcm);
			}
			Chara = dblTemp / (double)(Ndiv + 1);
		}
	}

	return(Chara);
}

double FNPCMenthalpy_table_lib(CHARTABLE *ct, double T)
{
	int i;
	double *enthalpy, *preventhalpy, *Tpcm, *prevTpcm;
	double retVal;
	prevTpcm = Tpcm = enthalpy = preventhalpy = NULL;
	Tpcm = ct->T;
	enthalpy = ct->Chara;
	// テーブルの最低温度よりPCM温度が低い場合は端部の特性値を線形で外挿
	if (T < ct->minTemp)
		retVal = ct->lowA*T + ct->lowB;
	// テーブルの最高温度よりPCM温度が高い場合は端部の特性値を線形で外挿
	else if (T > ct->maxTemp)
	{
		retVal = ct->upA*T + ct->upB;
	}
	else
	{
		for (i = 0; i < ct->itablerow; i++, Tpcm++, enthalpy++)
		{
			if (*Tpcm > T)
				break;
			prevTpcm = Tpcm;
			preventhalpy = enthalpy;
		}
		// 線形補間
		retVal = *preventhalpy + (*enthalpy - *preventhalpy)*(T - *prevTpcm) / (*Tpcm - *prevTpcm);
	}

	return(retVal);
}
//
//// テーブルの下限温度、上限温度での見かけの比熱を求める
//double FNPCMspcheat_outtemp(CHARTABLE *ct, char flg)
//{
//	double Tpcm, prevTpcm, enthalpy, preventhalpy;
//	double *T, *Chara;
//
//	T = ct->T;
//	Chara = ct->Chara;
//	// 上限温度の場合
//	if (flg == 'u')
//	{
//		Tpcm = *(T + ct->itablerow - 1);
//		prevTpcm = *(T + ct->itablerow - 2);
//		enthalpy = *(Chara + ct->itablerow - 1);
//		preventhalpy = *(Chara + ct->itablerow - 2);
//	}
//	else
//	{
//		Tpcm = *(T + 1);
//		prevTpcm = *T;
//		enthalpy = *(Chara +  1);
//		preventhalpy = *Chara;
//	}
//
//	double Temp;
//	Temp = -999.;
//	// 上下限温度での見かけの比熱
//	if (ct->PCMchar == 'E')
//		Temp = (enthalpy - preventhalpy) / (Tpcm - prevTpcm);
//	// 熱伝導率
//	else if (ct->PCMchar == 'C')
//	{
//		if (flg == 'u')
//			Temp = enthalpy;
//		else
//			Temp = preventhalpy;
//	}
//	return(Temp);
//}
