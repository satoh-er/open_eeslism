#pragma once

int Hexdata(char *s, HEXCA *Hexca);
void Hexcfv(int Nhex, HEX *Hex);
void Hexene(int Nhex, HEX *Hex);
void hexprint(FILE *fo, int id, int Nhex, HEX *Hex);
void hexdyint(int Nhex, HEX *Hex);
void hexmonint(int Nhex, HEX *Hex);
void hexday(int Mon, int Day, int ttmm, int Nhex, HEX *Hex, int Nday, int SimDayend);
void hexdyprt(FILE *fo, int id, int Nhex, HEX *Hex);
void hexmonprt(FILE *fo, int id, int Nhex, HEX *Hex);
