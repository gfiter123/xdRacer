#include <stdio.h>

#include <Windows.h>
#include "XHConfig.h"

int S50CardNumber[12];
vector<APPCONFIG> appConfig;

//´ÓÎÄ¼þ¶Á¿¨ºÅ

static void Trim(char *buf)
{
	int i = 0;
	for (;i<strlen(buf);i++)
	{
		if (buf[i] != ' '&& buf[i] != '\t')
		{
			break;
		}
	}
	for (int j=0;j<strlen(buf)-i+1;j++)
	{
		buf[j]=buf[j+i];
	}
	for (int j=strlen(buf)-1;j>=0;j--)
	{
		if (buf[j] == ' '||buf[j] == '\t' ||buf[j] == '\n')
		{
			buf[j] = '\0';
		}
	}
}


bool ReadConfigFromFile(char* szFilePath)
{
	FILE *fp;
	
	
	if ((fp = fopen(szFilePath,"r+")) == NULL)
	{
		OutputDebugString(TEXT("fopen file fail\n"));
		return false;

	}
	char buf[MAX_PATH]={0};

	while(fgets(buf,MAX_PATH,fp) != NULL)
	{
		if (buf[0]=='/'&& buf[1]=='/' || buf[0] == ' '|| buf[0]==10)
		{
			continue;
		}
		else
		{
			char key[MAX_KEY] = {0};
			char XHvalue[MAX_VALUE] = {0};
			int i = 0;
			for (;i < strlen(buf);i++)
			{
				if(buf[i]== '=')
				{
					break;
				}
			}
			if (i == strlen(buf))
			{
				continue;
			}
			else{
				memcpy(key,buf,i);
				memcpy(XHvalue,buf+i+1,strlen(buf)-i-1);
				Trim(key);
				Trim(XHvalue);
				APPCONFIG tempconfig;
				ZeroMemory(&tempconfig,sizeof(APPCONFIG));
				strcpy(tempconfig.key,key);
				strcpy(tempconfig.XHvalue,XHvalue);
				appConfig.push_back(tempconfig);
			}
			
		}
	}
	fclose(fp);
	return true;
}

bool GetConfigFromMem(const char *key,char *XHvalue)
{	
	if (key == NULL||XHvalue ==NULL)
	{
		return false;
	}
	vector<APPCONFIG>::iterator iter;
	for (iter=appConfig.begin();iter != appConfig.end();iter++)
	{
		if (!strcmp(key,iter->key))
		{
			strcpy(XHvalue,iter->XHvalue);
			return true;
		}
	}
	return false;
}
