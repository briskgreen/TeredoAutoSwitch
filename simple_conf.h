#ifndef _SIMPLE_CONF_H
#define _SIMPLE_CONF_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_WORDS 256
#define MAX_CONF_FILE_SIZE 1024

typedef struct
{
	char key[MAX_WORDS];
	char value[MAX_WORDS];
}_M;

typedef struct
{
	_M h[MAX_WORDS];
	int n;
}SMC;

void miredo_conf_init(SMC *m);
bool miredo_conf_load(SMC *m,const char *path);
bool miredo_conf_parse(SMC *m,const char *s);
char *miredo_conf_search(SMC *m,const char *key);

#endif
