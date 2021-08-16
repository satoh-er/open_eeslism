#pragma once

void Roomene(RMVLS *Rmvls, int Nroom, ROOM *Room, int Nrdpnl, RDPNL *Rdpnl, EXSFS *Exsfs, WDAT *Wd);
void	PCMwlchk(int i, RMVLS *Rmvls, EXSFS *Exsfs, WDAT *Wd, int *LDreset);
void	PCMfunchk(int Nroom, ROOM *Room, WDAT *Wd, int *LDreset);
void Roomload(int Nroom, ROOM *Room, int *LDreset);
void rmqaprint(FILE *fo, int id, int Nroom, ROOM *Room);
void panelprint(FILE *fo, int id, int Nrdpnl, RDPNL *Rdpnl);
