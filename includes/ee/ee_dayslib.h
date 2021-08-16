#pragma once

void svdyint(SVDAY *vd);
void svdaysum(long ttmm, char control, double v, SVDAY *vd);
void svmonsum(int Mon, int Day, int time, char control, double v, SVDAY *vd, int Dayend, int SimDayend) ;
void qdyint(QDAY *Qd);
void qdaysum(int ttmm, char control, double Q, QDAY *Qd);
void qmonsum(int Mon, int Day, int time, char control, double Q, QDAY *Qd, int Dayend, int SimDayend);
void qdaysumNotOpe(int time, double Q, QDAY *Qd);
void qmonsumNotOpe(int Mon, int Day, int time, double Q, QDAY *Qd, int Dayend, int SimDayend);
void edyint(EDAY *Ed);
void edaysum(int ttmm, char control, double E, EDAY *Ed);
void emonsum(int Mon, int Day, int time, char control, double E, EDAY *Ed, int Dayend, int SimDayend);
void emtsum(int Mon, int Day, int time, char control, double E, EDAY *Ed) ;
void minmark(double *minval, long *timemin, double v, long time);
void maxmark(double *maxval, long *timemax, double v, long time);
