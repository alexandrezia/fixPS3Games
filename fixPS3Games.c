/*
 * fixPS3Games.c
 *
 *  Created on: Jun 12, 2011
 *      Author: arz
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

#include "fixPS3Games.h"

// -----------------------------------------------------------------------------

int  main (int argc, char * argv[] )
{
  struct dirent *dp;
  DIR *dirp;
  struct st_game game;
  char *fileName;

  dirp = opendir(".");
  while ((dp = readdir(dirp)) != NULL)
  {

//		printf("----------------------------\n"
//		       "Dir name: %s\n", dp->d_name);

    if ( strcmp( dp->d_name, "." ) == 0  )
      continue;


    if ( strcmp( dp->d_name, ".." ) == 0)
      continue;

    if ( checkDir(dp->d_name) == -1 )
      continue;

    fileName = (char *) calloc(strlen(dp->d_name)+19, sizeof(char *));
    sprintf(fileName, "%s/PS3_GAME", dp->d_name);

    if ( checkDir(fileName) == -1 )
    {
      free(fileName);
      continue;
    }

    sprintf(fileName, "%s"SFOPATH, dp->d_name);

// printf( "1 %s\n", fileName);

    if ( (checkParamSFO(fileName)) == 0 )
    {
//printf( "2 %s\n", fileName);
// fprintf(stderr, "FOUND\n");

      char newName[1024];

      getField(fileName, "TITLE_ID", &game.id);
      getField(fileName, "TITLE", &game.name);
      normalizeName( &game.name );

//			fprintf(stderr, "ID ..: %s\n", game.id);
//			fprintf(stderr, "Name : %s\n", game.name);

      sprintf(newName, "%s-[%s]", game.id, game.name);

      if ( strcmp ( dp->d_name, newName ) != 0 )
      {
        fprintf(stderr, "--------------------------------------------\n"
            "From: %s\n"
            "To .: %s\n", dp->d_name, newName );
        if ( (rename ( dp->d_name, newName )) == -1 )
        {
          fprintf(stderr, "ERROR 7: %s\n", strerror(errno));
        }
      }
    }
    else
    {
      fprintf(stderr, "--------------------------------------------\n"
          "ERROR 2: %s -> %s\n", fileName, strerror(errno));
    }

    free(fileName);

  }
  (void)closedir(dirp);
//	printf("NOT FOUND\n");

//	free(name);
  return 0;
}
// -----------------------------------------------------------------------------

int getField ( char *fileName, char *field, char *value  )
{
  FILE *fp;

  fp = fopen(fileName, "rb");
  if (fp != NULL)
  {
    unsigned len, pos, str;
    unsigned char *mem = NULL;

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);

    mem = (unsigned char *) malloc(len + 16);
    if (!mem)
    {
      fclose(fp);
      return 0;
    }

    memset(mem, 0, len + 16);

    fseek(fp, 0, SEEK_SET);
    fread((void *) mem, len, 1, fp);
    fclose(fp);

    // posicao dos nomes das variaveis
    str = (mem[8] + (mem[9] << 8));

    // posicao dos valores das variaveis
    pos = (mem[0xc] + (mem[0xd] << 8));

//fprintf(stderr, "8 |%x|\n", mem[8] + (mem[9] << 8), mem[8] + (mem[9] << 8));
//fprintf(stderr, "c |%x|\n", mem[0xc] +( mem[0xd] << 8), mem[0xc] + (mem[0xd]<<8));

    int indx = 0;

    while (str < len)
    {
      if (mem[str] == 0)
        break;

      // se achou o campo
      if ( ! strcmp( (char *) &mem[str], field ) && mem[ str + strlen(field) ] == 0 )
      {
        memcpy(value, &mem[pos], 256);
        value[256] = 0;
        free(mem);
// fprintf(stderr, "Nome: %s\n", value);
        return 1;
      }

      while (mem[str])
        str++;
      str++;
      pos += (mem[0x1c + indx] + (mem[0x1d + indx] << 8));
      indx += 16;
    }
    if (mem)
      free(mem);
  }
  return 0;
}

// -----------------------------------------------------------------------------

int checkDir( char *dirName )
{
  struct stat st;

  if ( stat( dirName, &st) == -1 )
    return -1;

  if ( (st.st_mode & S_IFDIR) )
    return 0;

  return -1;
}

// -----------------------------------------------------------------------------

int checkParamSFO ( char *fileName )
{
  struct stat st;

  if ( (stat( fileName, &st)) < 0 )
    return -1;

  return 0;
}

// -----------------------------------------------------------------------------
int normalizeName( char *name )
{
  int n=0;
  int o=0;

  char aux[1024];

//fprintf(stderr, "antes : %s\n", name);

  // (TM) = E2 84 A2 | 28 54 4D 29
  memset(aux, 0, strlen(aux));
  o=0;
  for(n=0; n<(strlen(name)); n++)
  {
//fprintf(stderr, ": |%x|%c|\n", name[n], name[n] );

    if(    name[n]   == 0xffffffe2
       &&  name[n+1] == 0xffffff84
       &&  name[n+2] == 0xffffffa2
       )
    {
      n+=2;
    }
    else
    {
      aux[o] = name[n];
      o++;
    }

  }
  memset( name, 0, strlen(name));
  memcpy( name, aux, strlen(aux) );

  // (:) = 3a
  memset(aux, 0, strlen(aux));
  o=0;
  for(n=0; n<(strlen(name)); n++)
  {
    if( name[n] == 0x3a  )
    {
    }
    else
    {
      aux[o] = name[n];
      o++;
    }

  }
  memset( name, 0, strlen(name));
  memcpy( name, aux, strlen(aux) );

  // (R) = c2 ae
  memset(aux, 0, strlen(aux));
  o=0;
  for(n=0; n<(strlen(name)); n++)
  {
    if(    name[n]   == 0xffffffc2
       &&  name[n+1] == 0xffffffae
       )
    {
      n++;
    }
    else
    {
      aux[o] = name[n];
      o++;
    }

  }
  memset( name, 0, strlen(name));
  memcpy( name, aux, strlen(aux) );

//	fprintf(stderr, "depois: %s\n", aux);
//	fprintf(stderr, "depois: %s\n", name);

//  exit (0);
  return(0);
}
// -----------------------------------------------------------------------------

