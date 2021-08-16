#pragma once

void Helminit(char *errkey, char helmkey, int Nroom, ROOM *Room, QETOTAL *Qetotal);
void Helmroom( int Nroom, ROOM *Room, QRM *Qrm, QETOTAL *Qetotal, double dTM, double Ta, double xa);
void Helmprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
		int Nroom, ROOM *Room, QETOTAL *Qetotal);
void helmrmprint( FILE *fo, int id, int Nroom, ROOM *Room, QETOTAL *Qetotal);
void Helmsurfprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day, double time,
		int Nroom, ROOM *Room);
void helmsfprint( FILE *fo, int id, int Nroom, ROOM *Room);
void Helmdy (int day, int Nroom, ROOM *Room, QETOTAL *Qetotal);
void helmdyint ( int Nroom, ROOM *Room, QETOTAL *Qetotal);
void Helmdyprint(FILE *fo, char *mrk, SIMCONTL *Simc, int mon, int day,
		int Nroom, ROOM *Room, QETOTAL *Qetotal);
void helmrmdyprint( FILE *fo, int id, int Nroom, ROOM *Room, QETOTAL *Qetotal);
