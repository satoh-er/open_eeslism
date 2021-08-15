#pragma once

int roomvptr(int Nk, char **key, ROOM *Room, VPTR *vptr);
int roomldptr(char *load, char **key, ROOM *Room, VPTR *vptr, char *idmrk);
void roomldschd(ROOM *Room);
