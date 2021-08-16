#pragma once

double	FNScol(double ta, double I, double EffPV, double Ku, double ao, double Eo, double Fs, double RN ) ;
double FNTcoleContrl(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
void	FNBoundarySolarWall(RMSRF *Sd, double *ECG, double *ECt, double *CFc) ;
void  CalcSolarWallTe(RMVLS *Rmvls, WDAT *Wd, EXSFS *Exsfs) ;
double	FNTf(double Tcin, double Tcole, double ECG) ;
double FNSolarWallao(WDAT *Wd, RMSRF *Sd, EXSFS *Exsfs) ;
double VentAirLayerar(double dblEsu, double dblEsd, double dblTsu, double dblTsd) ;
double FNJurgesac(RMSRF *Sd, double dblV, double a, double b) ;
void	FNKc(WDAT *Wd, EXSFS *Exsfs, RMSRF *Sd) ;
void FNTsuTsd(RMSRF *Sd, WDAT *Wd, EXSFS *Exsfs) ;
double	FNVentAirLayerac(double Tsu, double Tsd, double air_layer_t, double Wb);
void	FNBoundarySolarWall(RMSRF *Sd, double *ECG, double *ECt, double *CFc) ;
