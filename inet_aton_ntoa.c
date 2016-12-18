#include "redismodule.h"
#include <string.h>
#include <stdlib.h>

const unsigned long int aton_error_code = 4294967296;
char* ntoa_error_str = "256.256.256.256";

unsigned long int _aton(char * ip_dot_i)
{
	unsigned long int ip_int=0,ip_int_temp;
	unsigned short int dot_count = 0;
	char *quads;   
	char ip_dot_a[16];
	char* ip_dot = ip_dot_a;
	strcpy(ip_dot,ip_dot_i);
	
	quads=strtok(ip_dot ,".");
	while(quads!= NULL && dot_count <=4)
	{
		ip_int_temp=strtoul(quads,&ip_dot,0);
		if(ip_int_temp <=255 && ip_int_temp >= 0)
		{
			ip_int=(ip_int << 8) + ip_int_temp;
			quads=strtok(NULL,".");
			dot_count++;
		}
		else
		{
			return aton_error_code;
		}
	}

	if(dot_count !=4 )
		return aton_error_code;	
		
	return ip_int;
}

void _ntoa(unsigned long int ip_int, char ip_dot[])
{
	if(ip_int <0 || ip_int >= aton_error_code)
	{
		strcpy(ip_dot,ntoa_error_str);
	}
	else
	{
		//size_t span_ip_int = strspn(ip_int, "0123456789");
		//if(ip_needle[span_needle] == '\0'
		
		sprintf(ip_dot,"%u.%u.%u.%u",(unsigned short)((ip_int >> 24) & 0xFF), (unsigned short)((ip_int >> 16) & 0xFF), (unsigned short)((ip_int >> 8) & 0xFF), (unsigned short)(ip_int & 0xFF));
	}
}

int aton(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) 
{
  if (argc < 2) 
    return RedisModule_WrongArity(ctx);

  RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ);
	int keytype = RedisModule_KeyType(key);
	char * ip_dot;
	size_t len;
	
	if(keytype == REDISMODULE_KEYTYPE_EMPTY)
	{
		RedisModule_CloseKey(key);
		ip_dot = RedisModule_StringPtrLen(argv[1],&len);
	}
	else if(keytype == REDISMODULE_KEYTYPE_STRING)
	{
		RedisModule_CloseKey(key);
		ip_dot = RedisModule_StringDMA(key,&len,REDISMODULE_READ);
	}
	else
	{
		RedisModule_CloseKey(key);
		return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
	}
	
	unsigned long int ip_int = _aton(ip_dot);
	
	if(ip_int == aton_error_code)
		return RedisModule_ReplyWithError(ctx, "Invalid IPv4 format");
	
	return RedisModule_ReplyWithLongLong(ctx,ip_int);
}

int ntoa(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
	if (argc < 2) 
    return RedisModule_WrongArity(ctx);
  
  RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ);
	int keytype = RedisModule_KeyType(key);
	long long ip_int;
	
	if(keytype == REDISMODULE_KEYTYPE_EMPTY)
	{
		RedisModule_CloseKey(key);
		if (RedisModule_StringToLongLong(argv[1],&ip_int) != REDISMODULE_OK) 
			return RedisModule_ReplyWithError(ctx,"Error occured during conversion");
	}
	else if(keytype == REDISMODULE_KEYTYPE_STRING)
	{
		size_t len;
		char* ip_int_str = RedisModule_StringDMA(key,&len,REDISMODULE_READ);
		RedisModule_CloseKey(key);
		char * buffer;
		
		ip_int = strtoul(ip_int_str,&buffer,0);

		if(strlen(buffer) > 0)
			return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
	}
	else
	{
		RedisModule_CloseKey(key);
		return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
	}
	
	char ip_dot[16] = {'\0'};
	
	_ntoa(ip_int,ip_dot);
	
	if( strcmp(ip_dot, ntoa_error_str) == 0 || ip_dot == NULL)
	{
		return RedisModule_ReplyWithError(ctx,"Invalid IPv4 Address Range");
	}
		
	return RedisModule_ReplyWithStringBuffer(ctx, ip_dot, strlen(ip_dot));
}

int exists(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) 
{
  if (argc != 3) 
    return RedisModule_WrongArity(ctx);

  RedisModuleKey *needle_key = RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ);
	int needle_keytype = RedisModule_KeyType(needle_key);
	
	RedisModuleKey *haystack_key = RedisModule_OpenKey(ctx,argv[2],REDISMODULE_READ);
	int haystack_keytype = RedisModule_KeyType(haystack_key);
	
	char * ip_needle = NULL, * ip_haystack = NULL;
	size_t len;
	
	if(needle_keytype == REDISMODULE_KEYTYPE_EMPTY)
	{
		RedisModule_CloseKey(needle_key);
		ip_needle = RedisModule_StringPtrLen(argv[1],&len);
	}
	else if(needle_keytype == REDISMODULE_KEYTYPE_STRING)
	{
		RedisModule_CloseKey(needle_key);
		ip_needle = RedisModule_StringDMA(needle_key,&len,REDISMODULE_READ);
	}
	else
	{
		RedisModule_CloseKey(needle_key);
		return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
	}
	   
	if(haystack_keytype == REDISMODULE_KEYTYPE_EMPTY)
	{
		RedisModule_CloseKey(haystack_key);
		ip_haystack = RedisModule_StringPtrLen(argv[2],&len);	
	}
	else if(haystack_keytype == REDISMODULE_KEYTYPE_STRING)
	{
		RedisModule_CloseKey(haystack_key);
		ip_haystack = RedisModule_StringDMA(haystack_key,&len,REDISMODULE_READ);
	}
	else
	{
		RedisModule_CloseKey(haystack_key);		
		return RedisModule_ReplyWithError(ctx,"Function doesn't work with passed datatype");
	}

	// Check if it is in integer format or no
	size_t span_needle = strspn(ip_needle, "0123456789");
	size_t span_haystack = strspn(ip_haystack, "0123456789");

	// Both integer
	if(ip_needle[span_needle] == '\0' && ip_haystack[span_haystack] == '\0')
	{
		char * buffer;
		if(strtoul(ip_needle,&buffer,0) < aton_error_code )
		{
			if(!strcmp(ip_needle,ip_haystack))
			{
				 return RedisModule_ReplyWithSimpleString(ctx,"TRUE");
			}
			else
			{
				 return RedisModule_ReplyWithSimpleString(ctx,"FALSE");
			}	
		}
		else
		{
			return RedisModule_ReplyWithError(ctx,"Invalid IPv4 address");
		}				
	} // needle integer, haystack dotted
	else if(ip_needle[span_needle] == '\0' && ip_haystack[span_haystack] != '\0')
	{
		unsigned long long ip_haystack_int = _aton(ip_haystack);
		
		if(ip_haystack_int  != aton_error_code) 
		{
			char * buffer;
			if(strtoul(ip_needle,&buffer,0) == ip_haystack_int)
			 return RedisModule_ReplyWithSimpleString(ctx,"TRUE");
			else
				return RedisModule_ReplyWithSimpleString(ctx,"FALSE");
		}
		else
		{
			 return RedisModule_ReplyWithError(ctx,"Invalid Haystack IPv4 IP Address");	
		}		
	} // needle dotted, haystack integer
	else if(ip_needle[span_needle] != '\0' && ip_haystack[span_haystack] == '\0')
	{
		unsigned long long ip_needle_int = _aton(ip_needle);
		
		if(ip_needle_int  != aton_error_code) 
		{
			char * buffer;
			if(ip_needle_int == strtoul(ip_haystack,&buffer,0))
			 return RedisModule_ReplyWithSimpleString(ctx,"TRUE");
		 else
			return RedisModule_ReplyWithSimpleString(ctx,"FALSE");
		}
		else
		{
			 return RedisModule_ReplyWithError(ctx,"Invalid Needle IPv4 IP Address");	
		}
	} // needle dotted, haystack dotted
	else if(ip_needle[span_needle] != '\0' && ip_haystack[span_haystack] != '\0')
	{
		unsigned long long ip_needle_int = _aton(ip_needle);
		unsigned long long ip_haystack_int = _aton(ip_haystack);
		
		if(ip_needle_int  != aton_error_code && ip_haystack_int  != aton_error_code ) 
		{
			if(ip_needle_int == ip_haystack_int)
			 return RedisModule_ReplyWithSimpleString(ctx,"TRUE");
		 else
			return RedisModule_ReplyWithSimpleString(ctx,"FALSE");
		}
		else
		{
			 return RedisModule_ReplyWithError(ctx,"Invalid IPv4 IP Address parameters to function");	
		}
	}
	else
	{
		return RedisModule_ReplyWithError(ctx,"Error occured during comparison 2");	
	}	
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) 
{
  if (RedisModule_Init(ctx, "inet", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) 
  {
    return REDISMODULE_ERR;
  }
  
  if (RedisModule_CreateCommand(ctx, "inet.aton", aton, "readonly", 1,1,1) == REDISMODULE_ERR) 
  {
    return REDISMODULE_ERR;
  }

  if (RedisModule_CreateCommand(ctx, "inet.ntoa", ntoa, "readonly", 1,1,1) == REDISMODULE_ERR) 
  {
    return REDISMODULE_ERR;
  }  
  
  if (RedisModule_CreateCommand(ctx, "inet.exists", exists, "readonly", 1,1,1) == REDISMODULE_ERR) 
  {
    return REDISMODULE_ERR;
  }  
}
