#pragma once

void panelwp (RDPNL *rdpnl);
void Panelcf (RDPNL *rdpnl);
double Panelce (RDPNL *rdpnl);
int rdpnlldsptr(char *load, char **key, RDPNL *Rdpnl, VPTR *vptr, char *idmrk) ;
void rdpnlldsschd(RDPNL *Rdpnl) ;
int rdpnlvptr(char **key, RDPNL *Rdpnl, VPTR *vptr) ;
