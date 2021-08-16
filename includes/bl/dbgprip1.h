#pragma once

void dprdayweek(int *daywk);
void dprschtable (SEASN *Seasn, WKDY *Wkdy, DSCH *Dsch, DSCW *Dscw);
void dprschdata (SCH *Sch, SCH *Scw);
void dprachv (int Nroom, ROOM *Room);
void dprexsf(EXSF *Exs);
void dprwwdata(WALL *Wall, WINDOW *Window);
void dprroomdata(ROOM *Room, RMSRF *Sd);
void dprballoc (MWALL *Mw, RMSRF *Sd);
