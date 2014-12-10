#ifndef __XH_GAME_UPDATA_HARDWARE_CODE_H__
#define __XH_GAME_UPDATA_HARDWARE_CODE_H__
enum sign{REQUEST,CHECK_ONE,CHECK_TWO,DOWNLOAD,FINISH};
extern int commandSign;
//请求
static void XHUpdateHardwareRequest(char *command,int &len)
{
	command[0] = 0x61;
	command[1] = 0xB2;
	command[2] = 0x03;
	len = 3;
	return ;
	
}
static bool XHIsUpdateHardwareRequest(char *command)
{
	if (command[0] == (char)0x61&&command[1] == (char)0xB2&&command[2] == (char)0x00)
	{
		return true;
	}
	return false;
}
//检查1
static void XHUpdateHardwareCheckFlashOne(char *command,int &len)
{
	command[0] = 0x61;
	command[1] = 0x00;
	command[2] = 0x03;
	len = 3;
	return ;
}
static bool XHIsUpdateHardwareCheckFlashOne(char *command)
{
	if (command[0] == (char)0x61&&command[1] == (char)0x00&&command[2] == (char)0x00)
	{
		return true;
	}
	return false;

}
//检查2
static void XHUpdateHardwareCheckFlashTwo(char *command,int &len)
{

	command[0] = 0x61;
	command[1] = 0x01;
	command[2] = 0x03;
	len = 3;
	return ;
}

static bool XHIsUpdateHardwareCheckFlashTwo(char *command)
{
	if (command[0] == (char)0x61&&command[1] == (char)0x01&&command[2] == (char)0x00)
	{
		return true;
	}
	return false;
}
//开始下载
static void  XHUpdateHardwareDownload(char *command,int &len,char *data)
{
	command[0] = 0x61;
	command[1] = 0x02;
	int i = 0;
	for (;i< 1024;i++)
	{
		command[i+2] = data[i];
	}
	command[i+2]=0x03;
	len = i+3;
	return;
}


static bool XHIsUpdateHardwareDownload(char *command)
{
	if (command[0] == (char)0x61&&command[1] == (char)0x02&&command[2] == (char)0x00)
	{
		return true;
	}
	return false;
}

//下载完成
static void  XHUpdateHardwareFinish(char *command,int& len)
{
	command[0] = 0x61;
	command[1] = 0x03;
	command[2] = 0x03;
	len = 3;
	return ;
}


static bool XHIsUpdateHardwareFinish(char *command)
{
	if (command[0] == (char)0x61&&command[1] == (char)0x03&&command[2] == (char)0x00)
	{
		return true;
	}
	return false;
}

#endif