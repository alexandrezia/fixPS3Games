/*
 * fixPS3Games.h
 *
 *  Created on: Jun 12, 2011
 *      Author: arz
 */

#ifndef FIXPS3GAMES_H_
#define FIXPS3GAMES_H_

#define SFOPATH "/PS3_GAME/PARAM.SFO"

struct st_game {
  char id[10];
  char name[1024];
};

int checkDir( char *dirName );

int checkParamSFO ( char *fileName );

int getField ( char *fileName, char *field, char *value);

int normalizeName( char *name );

void trim (char *s);

#endif /* FIXPS3GAMES_H_ */
