#pragma once

void Walli(int Nbm, BMLST *W, WALL *Wl, PCM *pcm, int Npcm);
void Wallfdc(int M, int mp, double *res, double *cap, double Wp, double *UX,
	double *uo, double *um, double *Pc, char WallType, RMSRF *Sd, WDAT *Wd,
	EXSFS *Exsf, WALL *Wall, double *Told, double *Twd, PCMSTATE *pcmstate);
void Twall(int M, int mp, double *UX, double uo, double um,
	double Pc, double Ti, double To, double WpT, double *Told, double *Tw, RMSRF *Sd, PCM **pcm);
void Twalld(int M, int mp, double *UX, double uo, double um,
	double Pc, double Ti, double To, double WpT, double *Told, double *Tw, RMSRF *Sd);
