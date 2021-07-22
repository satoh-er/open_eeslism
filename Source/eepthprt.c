//This file is part of EESLISM.
//
//Foobar is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Foobar is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Foobar.If not, see < https://www.gnu.org/licenses/>.

#include <stdio.h>
#include "fesy.h"
#include "fnfio.h"

/* 経路に沿ったシステム要素の出力 */

void Pathprint(FILE *fo, char *title, int mon, int day, double time,
			   int Nmpath, MPATH *Mpath)
{
	PLIST *Pli;
	PELM  *Pelm;
	int   i, j, k;
	char  c, *fm;
	static char *fmt = "%4.1lf", *fmx = "%6.4lf";
	static int  id = 0;
	
	if (id == 0)
	{
		id++ ;
		fprintf(fo, "%s ;\n", title);
		fprintf(fo, "%d\n", Nmpath);
		
		for (i = 0; i < Nmpath; i++, Mpath++)
		{
			fprintf(fo, "%s %c %c %d\n", Mpath->name,
				Mpath->type, Mpath->fluid, Mpath->Nlpath);
			Pli = Mpath->plist;
			Pelm = Pli->pelm;
			if (Pelm->ci == '>')
				fprintf(fo, " >");
			
			for (j = 0; j < Mpath->Nlpath; j++, Pli++)
			{
				/*------ Satoh DEBUG  1998/5/15 --------*/
				
				if ( Pli->name != NULL )
					fprintf(fo, "%s", Pli->name);
				else
					fprintf(fo, "?");
				fprintf(fo, " %c %d\n", Pli->type, Pli->Nelm);
				
				Pelm = Pli->pelm;
				if (Pelm->ci == '>')
					/******* fprintf(fo," %s", Pli->flwin->name);********/
					fprintf(fo," %s", Pelm->cmp->name);
				
				for (k = 0; k < Pli->Nelm; k++, Pelm++)
					fprintf(fo, " %s", Pelm->cmp->name);
				fprintf(fo, "\n");
			}
		}
		Mpath -= Nmpath;      
	}   
	fprintf(fo, "%02d %02d %5.2lf\n", mon, day, time);
	
	for (i = 0; i < Nmpath; i++, Mpath++)
	{
		fm = fmt;
		if (Mpath->fluid == AIRx_FLD)
			fm = fmx;
		
		if ((c = Mpath->control) == 0)
			c = '?';
		fprintf(fo, "[%c]", c);
		
		Pli = Mpath->plist;
		for (j = 0; j < Mpath->Nlpath; j++, Pli++)
		{
			if ((c = Pli->control) == 0)
				c = '?';	    
			fprintf(fo, " %5.3g %c: ", Pli->G, c);
			
			Pelm = Pli->pelm;
			if (Pelm->ci == '>')
				fprintf(fo, fm, Pelm->in->sysvin);
			for (k = 0; k < Pli->Nelm; k++, Pelm++)
			{
				if (Pelm->out != NULL)
				{
					fprintf(fo, fm, Pelm->out->sysv);	    
					if ((c = Pelm->out->control) == 0)
						c = '?';	       
					fprintf(fo, " %c ", c);
				}
				
			}
			fprintf(fo, "\n");
		}
	}
	fprintf(fo, " ;\n");
}




