#include <stdio.h>
#include <stdlib.h>
#include "eps.h"

void	Estlinit ( ESTL *Estl )
{
	int	i ;

	Estl->flid = Estl->title = Estl->wdatfile = Estl->timeid = NULL ;
	Estl->wdloc = NULL ;

	for ( i = 0; i < VTYPEMAX; i++ )
		Estl->unit[i] = NULL ;

	Estl->catnm = NULL ;
	Estl->Rq = NULL ;
	Estl->vreq = NULL ;
	Estl->ntimeid = Estl->Ntime = Estl->dtm = 0 ;
	Estl->Nunit = Estl->Nrqlist = Estl->Nvreq = 0 ;
	Estl->Npreq = Estl->Npprd = Estl->Ndata = 0 ;
	Estl->Ncatalloc = Estl->NRq = 0 ;
}
