#ifndef __XHCONFIG_H__
#define __XHCONFIG_H__


#include <vector>
using namespace std;



#define MAX_KEY		20
#define MAX_VALUE	128
typedef struct __AppConfig
{
	
	char key[MAX_KEY];
	char XHvalue[MAX_VALUE];
}APPCONFIG;

extern vector<APPCONFIG> appConfig;


extern bool ReadConfigFromFile(char* szFilePath);
extern bool WriteConfigToFile(char* szFilePath);
extern bool GetConfigFromMem(const char *key,char *value);

#endif