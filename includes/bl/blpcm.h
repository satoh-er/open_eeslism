#pragma once


void PCMdata(FILE *fi, char *dsn, PCM **pcm, int *Npcm, char *pcmiterate);
int		PCMcount ( FILE *fi ) ;
double	FNPCMState(int Ctype, double Ss, double Sl, double Ql, double Ts, double Tl, double Tp, double T, PCMPARAM PCMp);
double	FNPCMStatefun(int Ctype, double Ss, double Sl, double Ql, double Ts, double Tl, double Tp, double oldT, double T, int DivTemp, PCMPARAM PCMp);
void TableRead(CHARTABLE *ct);
//double FNPCMspcheat_outtemp(CHARTABLE *ct, char flg);
double FNPCMenthalpy_table_lib(CHARTABLE *ct, double T);
double FNPCMstate_table(CHARTABLE *ct, double Told, double T, int Ndiv);