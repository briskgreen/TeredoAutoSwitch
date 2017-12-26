#include "simple_conf.h"

void miredo_conf_init(SMC *m)
{
	m->n=0;
}

bool miredo_conf_load(SMC *m,const char *path)
{
	FILE *fp;
	char buf[MAX_CONF_FILE_SIZE]={0};

	if(!(fp=fopen(path,"rb")))
		return false;

	if(fread(buf,sizeof(char),sizeof(buf)-1,fp) <= 0)
		return false;
	fclose(fp);

	return miredo_conf_parse(m,buf);
}

bool miredo_conf_parse(SMC *m,const char *s)
{
	enum type
	{
		miredo_key,
		miredo_key_finish,
		miredo_value,
		miredo_value_finish
	};
	int status=miredo_value_finish;
	char *p;

	while(*s)
	{
		switch(*s)
		{
			case '#':
				while(*s && *s != '\n')
					++s;
				break;

			case ' ':
			case '\t':
				if(status == miredo_key_finish)
					status=miredo_value;
			case '\r':
			case '\n':
				while(*s && isspace(*s))
					++s;
				break;

			default:
				if(status != miredo_value_finish)
					return false;
				status=miredo_key;
				break;
		}

		switch(status)
		{
			case miredo_key:
				p=(char *)m->h[m->n].key;
				while(*s && !isspace(*s))
				{
					*p=*s;
					++p;
					if(p-m->h[m->n].key >= MAX_WORDS)
						return false;
					++s;
				}
				*p='\0';
				if(!*s)
					return false;
				status=miredo_key_finish;
				break;

			case miredo_value:
				p=(char *)m->h[m->n].value;
				while(!isspace(*s) && *s)
				{
					*p=*s;
					++p;
					if(p-m->h[m->n].value >= MAX_WORDS)
						return false;
					++s;
				}
				*p='\0';
				++m->n;
				if(m->n >= MAX_WORDS)
					return false;
				status=miredo_value_finish;
				break;
		}
	}

	if(status != miredo_value_finish)
		return false;

	return true;
}

char *miredo_conf_search(SMC *m,const char *key)
{
	int i;

	for(i=0;i < m->n;++i)
	{
		if(strcmp(m->h[i].key,key) == 0)
			return m->h[i].value;
	}

	return NULL;
}
