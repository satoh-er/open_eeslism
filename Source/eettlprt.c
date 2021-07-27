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

/*  ttlprt.c  */

#include <string.h>
#include "fesy.h"
#include "fnfio.h"


/* 標題、注記の出力（時刻別計算値ファイル） */

void __replace_dir_sep(char* path)
{
   //ファイルパスセパレータの統一
   size_t l = strlen(path);
   for (size_t i = 0; i < l; i++)
   {
      if(path[i] == '/'){
         path[i] = '\\';
      }
   }
}

void ttlprint(FILE *fo, char *fileid, SIMCONTL *Simc)
{
   unsigned int i;

   //ファイルパスセパレータの統一
   __replace_dir_sep(Simc->File);
   
   fprintf(fo, "%s#\n", fileid);
   fprintf(fo, "-ver %s\n", EEVERSION);
   fprintf(fo, "-t %s ;\n", Simc->title);
   fprintf(fo, "-dtf %s\n", Simc->File);
   fprintf(fo,"-w %s\n", Simc->wfname);     
   fprintf(fo, "-tid h\n");
   
   fprintf(fo, "-tmid ");
   for (i = 0; i < strlen(Simc->timeid); i++)
      fprintf(fo, "%c", Simc->timeid[i]);
   fprintf(fo, "\n");
    
   fprintf(fo, "-u %s ;\n", Simc->unit);
   fprintf(fo, "-dtm %d\n", Simc->dTm);
   fprintf(fo, "-Ntime %d\n", Simc->Ntimehrprt);

}
/* ---------------------------------------------------- */  

/* 標題、注記の出力（日集計値ファイル） */
   
void ttldyprint(FILE *fo, char *fileid, SIMCONTL *Simc)
{
   unsigned int i;
   
   fprintf(fo, "%s#\n", fileid);
   fprintf(fo, "-ver %s\n", EEVERSION);
   fprintf(fo, "-t %s ;\n", Simc->title);
   fprintf(fo, "-dtf %s\n", Simc->File);
   
   fprintf(fo,"-w %s\n", Simc->wfname);   
   fprintf(fo, "-tid d\n");
   
   fprintf(fo, "-tmid ");
   for (i = 0; i < strlen(Simc->timeid) - 1; i++)
      fprintf(fo, "%c", Simc->timeid[i]);
   fprintf(fo, "\n");
   

   fprintf(fo, "-u %s %s ;\n", Simc->unit, Simc->unitdy);
   fprintf(fo, "-dtm %d\n", Simc->dTm);
   fprintf(fo, "-Ntime %d\n", Simc->Ntimedyprt);
 
}
   
/* ---------------------------------------------------- */  

/* 標題、注記の出力（日集計値ファイル） */
   
void ttlmtprint(FILE *fo, char *fileid, SIMCONTL *Simc)
{
   //unsigned int i;
   
   fprintf(fo, "%s#\n", fileid);
   fprintf(fo, "-ver %s\n", EEVERSION);
   fprintf(fo, "-t %s ;\n", Simc->title);
   fprintf(fo, "-dtf %s\n", Simc->File);
   
   fprintf(fo,"-w %s\n", Simc->wfname);   
   fprintf(fo, "-tid h\n");
   
   fprintf(fo, "-tmid ");
   fprintf(fo, "MT");
   fprintf(fo, "\n");
   

   fprintf(fo, "-u %s %s ;\n", Simc->unit, Simc->unitdy);
   fprintf(fo, "-dtm %d\n", Simc->dTm);
   fprintf(fo, "-Ntime %d\n", 24 * 12);
}
