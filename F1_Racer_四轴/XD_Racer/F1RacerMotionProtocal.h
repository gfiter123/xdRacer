#ifndef __F1_RACER_MOTION_PROTOCOL_H__
#define __F1_RACER_MOTION_PROTOCOL_H__
#include "XD_Racer.h"
#include <stdio.h>

enum COMMAND_HEAD{NORMAL_OPT = 0X60,DEBUG_OPT = 0X61,KEEP_ALIVE = 0X62,};
enum COMMAND_TYPE{SET_ZERO = 0XA0,DOWNLOAD_MOVIE_DATA = 0XA1,RUN_MOVIE_DATA = 0XA2,RUN_GAME_DATA = 0XA3,LOAD_SAVE = 0XA4};
enum ERROR_CODE{NO_ERROR_CODE = 1,};
enum COMMAND_END{END_COMMAND = 0X03}; 
#define  STOP_COMMAND 0XB0
#define  MAX_SINGLE_PLUSE 3000
typedef struct{
	char command[1024];
	int  len;
	 
}MotionProtocal;

static MotionProtocal MakeSetZero()
{
	MotionProtocal com;
	com.command[0] = (char)NORMAL_OPT;
	com.command[1] = (char)SET_ZERO;
	com.command[2] = (char)END_COMMAND;
	com.len = 3;
	return com;
}

static MotionProtocal MakeEnterGameMode()
{
	MotionProtocal com;
	com.command[0] = (char)0x60;
	com.command[1] = (char)0xa3;
	com.command[2] = (char)END_COMMAND;
	com.len = 3;
	return com;
}
static MotionProtocal MakeExitGameMode()
{
	MotionProtocal com;
	com.command[0] = (char)0x61;
	com.command[1] = (char)0xb0;
	com.command[2] = (char)END_COMMAND;
	com.len = 3;
	return com;
}

static MotionProtocal MakeBeginDownloadMovieData(byte num,int count)
{
	MotionProtocal com;
	com.command[0] = (char)NORMAL_OPT;
	com.command[1] = (char)DOWNLOAD_MOVIE_DATA;
	com.command[2] = num;


	com.command[3] = *((char *)&count + 3);
	com.command[4] = *((char *)&count + 2);
	com.command[5] = *((char *)&count + 1);
	com.command[6] = *((char *)&count + 0);
	com.command[7] = END_COMMAND;
	com.len = 8;
	return com;
}
static MotionProtocal MakeDownloadMovieData(byte num,int index)
{
	MotionProtocal com;
	com.command[0] = (char)NORMAL_OPT;
	com.command[1] = (char)DOWNLOAD_MOVIE_DATA;
	com.command[2] = num;
	

	com.command[3] = *((char *)&index + 3);
	com.command[4] = *((char *)&index + 2);
	com.command[5] = *((char *)&index + 1);
	com.command[6] = *((char *)&index + 0);
	
	static short old_x = 0;
	static short old_y = 0;
	int m = 7;
	for (int i=0;i<200;i++)
	{
		int x;
		int y;
		if(index + i < gMMDataList.size())
		{
			x = gMMDataList[index + i].x2;
			y = gMMDataList[index + i].y2;
		}
		else
		{
			x = 0;
			y = 0;
		}
		short dx = x - old_x;
		short dy = y - old_y;

		com.command[m++] = *((char *)&dx + 1);
		com.command[m++] = *((char *)&dx + 0);
		com.command[m++] = *((char *)&dy + 1);
		com.command[m++] = *((char *)&dy + 0);
		old_x = x;
		old_y = y;
	}
	
	com.command[m++] = END_COMMAND;
	com.len = m;
	
	return com;
}
static MotionProtocal MakeRunMovieData(int num,byte mode)
{

	MotionProtocal com;
	com.command[0] = (char)NORMAL_OPT;
	com.command[1] = (char)RUN_MOVIE_DATA;
	com.command[2] = (char)num;
	com.command[3] = mode;
	com.command[4] = (char)END_COMMAND;
	com.len = 5;
	return com;
}

static MotionProtocal MakeRunGameData_1(int  &x,int  &y,bool &bChaoCheng)
{
	bChaoCheng = false;
	static int old_x = 0;
	static int old_y = 0;
	int dx = x - old_x;
	int dy = y - old_y;
	char temp[200];
	MotionProtocal com = {0};
	if(dx > MAX_SINGLE_PLUSE)   
	{
		bChaoCheng = true;
		sprintf(temp,"x-%d\n",dx - MAX_SINGLE_PLUSE);
		OutputDebugString(temp);
		dx = MAX_SINGLE_PLUSE;
		//return com;
	}
	else if (dx < -MAX_SINGLE_PLUSE)
	{
		bChaoCheng = true;
		sprintf(temp,"x-%d\n",dx + MAX_SINGLE_PLUSE);
		OutputDebugString(temp);
		dx = -MAX_SINGLE_PLUSE;
		//return com;
	}
	if(dy > MAX_SINGLE_PLUSE) 
	{
		bChaoCheng = true;
		sprintf(temp,"y-%d\n",dy - MAX_SINGLE_PLUSE);
			OutputDebugString(temp);
			dy = MAX_SINGLE_PLUSE;
			//return com;
			
	}
	else if(dy < -MAX_SINGLE_PLUSE)
	{
		bChaoCheng = true;
		dy = -MAX_SINGLE_PLUSE;
		OutputDebugString("-dy\n");
		//turn com;
	}
	FILE *fp;
	if ((fp = fopen("d:\\2.txt","a+")) == NULL)
	{
		OutputDebugString("open file 2.txt fail\n");
	}
	
	static int count=0;
	fprintf(fp,"%d\t%d\t%d\n",count,dx,dy);
	count++;
	fclose(fp);
	int dsx = old_x + dx;//dx;
	int dsy = old_y + dy;//dy;
	
	com.command[0] = NORMAL_OPT;
	com.command[1] = RUN_GAME_DATA;
	com.command[2] = 0xff &(*((char *)&dsx + 3));
	com.command[3] = 0xff &(*((char *)&dsx + 2));
	com.command[4] = 0xff &(*((char *)&dsx + 1));
	com.command[5] = 0xff &(*((char *)&dsx + 0));
	
	com.command[6] = 0xff &(*((char *)&dsy + 3));
	com.command[7] = 0xff &(*((char *)&dsy + 2));
	com.command[8] = 0xff &(*((char *)&dsy + 1));
	com.command[9] = 0xff &(*((char *)&dsy + 0));
	
	com.command[10] = END_COMMAND;
	com.len = 11;
	
	old_x = dsx;
	old_y = dsy;
	
	return com;
}
static MotionProtocal MakeRunGameData_2(int &x,int &y,bool &bChaoCheng)
{
	MotionProtocal com = {0};
	bChaoCheng = false;
	static int old_x = 0;
	static int old_y = 0;
	int dx = x - old_x;
	int dy = y - old_y;
	if(dx > MAX_SINGLE_PLUSE)   
	{
			bChaoCheng = true;
			dx = MAX_SINGLE_PLUSE;
			
	}
	
	else if (dx < -MAX_SINGLE_PLUSE) 
	{
			dx = -MAX_SINGLE_PLUSE;
			bChaoCheng = true;
			

	}
	if(dy > MAX_SINGLE_PLUSE)
	{
			dy = MAX_SINGLE_PLUSE;
			bChaoCheng = true;
			
	}
	else if(dy < -MAX_SINGLE_PLUSE) 
	{
			dy = -MAX_SINGLE_PLUSE;
			bChaoCheng = true;
			
	}
	FILE *fp;
	if ((fp = fopen("d:\\3.txt","a+")) == NULL)
	{
		OutputDebugString("open file 3.txt fail\n");
	}

	static int count=0;
	fprintf(fp,"%d\t%d\t%d\n",count,dx,dy);
	count++;
	fclose(fp);
	int dsx = old_x + dx;//dx;
	int dsy = old_y + dy;//dy;
	
	com.command[0] = NORMAL_OPT;
	com.command[1] = RUN_GAME_DATA;
	com.command[2] = 0xff &(*((char *)&dsx + 3));
	com.command[3] = 0xff &(*((char *)&dsx + 2));
	com.command[4] = 0xff &(*((char *)&dsx + 1));
	com.command[5] = 0xff &(*((char *)&dsx + 0));
	com.command[6] = 0xff &(*((char *)&dsy + 3));
	com.command[7] = 0xff &(*((char *)&dsy + 2));
	com.command[8] = 0xff &(*((char *)&dsy + 1));
	com.command[9] = 0xff &(*((char *)&dsy + 0));

	com.command[10] = END_COMMAND;
	com.len = 11;
	
	old_x = dsx;
	old_y = dsy;
	return com;
}

static MotionProtocal MakeDebugCommand(int type)
{
	MotionProtocal com;
	com.command[0] = (char)DEBUG_OPT;
	com.command[1] = (char)type;
	com.command[2] = (char)END_COMMAND;
	com.len = 3;
	return com;
}

static MotionProtocal MakeKeepCommand(int count)
{
	MotionProtocal com;
	com.command[0] = (char)KEEP_ALIVE;
	com.command[1] = *((char *)&count + 3);;
	com.command[2] = *((char *)&count + 2);
	com.command[3] = *((char *)&count + 1);
	com.command[4] = *((char *)&count + 0);
	com.command[5] = (char)END_COMMAND;
	com.len = 6;
	return com;

}

static MotionProtocal MakeStopCommand()
{
	MotionProtocal com;
	com.command[0] = (char)DEBUG_OPT;
	com.command[1] = (char)STOP_COMMAND;
	com.command[2] = (char)END_COMMAND;
	com.len = 3;
	return com;
}
static MotionProtocal MakeSaveCommand(int num)
{
	MotionProtocal com;
	com.command[0] = (char)NORMAL_OPT;
	com.command[1] = (char)LOAD_SAVE;
	com.command[2] = (char)num;
	com.command[3] = (char)0x00;
	com.command[4] = (char)END_COMMAND;
	com.len = 5;
	return com;

}
static MotionProtocal MakeLoadCommand(int num)
{
	MotionProtocal com;
	com.command[0] = (char)NORMAL_OPT;
	com.command[1] = (char)LOAD_SAVE;
	com.command[2] = (char)num;
	com.command[3] = (char)0x01;
	com.command[4] = (char)END_COMMAND;
	com.len = 5;
	return com;
}
#endif