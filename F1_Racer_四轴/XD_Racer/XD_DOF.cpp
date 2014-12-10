#include "XD_DOF.h"

#include <math.h>
#include <string.h>
#include <Windows.h>
#include "XD_OutSim.h"

#define Y_TO_ABC			0.005
#define X_TO_ABC			-0.007
#define ABC_TO_ABC			0.01
static void rotation(float &x,float &y,float a)
{
	float x1 = x*cos(a) - y*sin(a);
	float y1 = x*sin(a) + y*cos(a);
	x = x1;
	y = y1;
}

static float XHSquare(float x)
{
	return x*x;
}


bool GetXD_DOFData(InputVector *input,OutputVector *output)
{

	//处理X,Y,Z轴的加速度
	
	input->a =  input->a*ABC_TO_ABC + input->y*Y_TO_ABC;
	input->b =  input->b*ABC_TO_ABC + input->x*X_TO_ABC;
	
	if(input->a > 0.04)
	{
		input->a = 0.04;
	}
	if (input->a < -0.04)
	{
		input->a = -0.04;
	}
	if (input->b > 0.083)
	{
		input->b = 0.083;
	}
	if (input->b < -0.083)
	{
		input->b = -0.083;
	}
	XH3DPoint variable[4];
	float out[4];
	memcpy(variable,initCoor.XHVariable,4*sizeof(XH3DPoint));
	XH3DPoint temp;
	for (int i = 0; i < 4 ;i++)
	{
		float oldLen = sqrt(XHSquare(variable[i].x - initCoor.XHQuantitative[i].x) + XHSquare(variable[i].y - initCoor.XHQuantitative[i].y) +  XHSquare(variable[i].z - initCoor.XHQuantitative[i].z));
		temp = variable[i];
		
		
		//绕Z逆时针旋转
		rotation(temp.x,temp.y,input->c);
		//绕x逆时针旋转
		rotation(temp.y,temp.z,input->a);
		//绕y逆时针旋转
		rotation(temp.z,temp.x,input->b);
		
		temp.z += input->z;

		variable[i] = temp;
		

		float len;
		len = sqrt(XHSquare(variable[i].x - initCoor.XHQuantitative[i].x) + XHSquare(variable[i].y - initCoor.XHQuantitative[i].y) +  XHSquare(variable[i].z - initCoor.XHQuantitative[i].z));
		
		out[i] = len - oldLen; 

	}
	
	output->l1 = out[0];
	output->l2 = out[1];
	output->l3 = out[2];
	output->l4 = out[3];
	return 1;
}