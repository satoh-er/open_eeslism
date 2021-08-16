#pragma once

void Rmsfprint (FILE *fo, char *title,
		int Mon, int Day, double time, ROOM *Room, RMSRF *Sd);
void Rmsfqprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd) ;
void Rmsfaprint (FILE *fo, char *title,
				int Mon, int Day, double time, ROOM *Room, RMSRF *Sd) ;
void Dysfprint(FILE *fo, char *title, int Mon, int Day, ROOM *Room ) ;
void	Shdprint(FILE *fo, char *title, int Mon, int Day, double time,
	int Nsrf, RMSRF *Sd);
void Wallprint(FILE *fo, char *title, int Mon, int Day, double time, 
	      int Nsrf, RMSRF *Sd);
void	PCMprint(FILE *fo, char *title, int Mon, int Day, double time,
	int Nsrf, RMSRF *Sd);
void Qrmprint (FILE *fo, char *title, int Mon, int Day, double time,
	       ROOM *Room, QRM *Qrm);
void Dyqrmprint (FILE *fo, char *title, int Mon, int Day, 
		 ROOM *Room, double *Trdav, QRM *Qrmd);
void Qrmsum (int Day, int Nroom, ROOM *Room, QRM *Qrm, 
	     double *Trdav, QRM *Qrmd);
