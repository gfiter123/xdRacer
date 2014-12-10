#ifndef __XD_DOF_H__
#define __XD_DOF_H__

typedef struct{
	float x;
	float y;
	float z;
}XH3DPoint;


typedef struct _InputVector
{
	float a;   //ÈÆXÄæÊ±Õë
	float b;   //ÈÆYÄæÊ±Õë
	float c;  //ÈÆZÄæÊ±Õë
	float x;
	float y;
	float z;
	_InputVector operator +(const _InputVector &A)
	{
		_InputVector temp;
		temp.a = a + A.a;
		temp.b = b + A.b;
		temp.c = c + A.c;
		temp.x = x + A.x;
		temp.y = y + A.y;
		temp.z = z + A.z;
		return temp;
	}

	_InputVector operator -(const _InputVector &A)
	{
		_InputVector temp;
		temp.a = a - A.a;
		temp.b = b - A.b;
		temp.c = c - A.c;
		temp.x = x - A.x;
		temp.y = y - A.y;
		temp.z = z - A.z;
		return temp;
	}
	_InputVector operator *(const _InputVector &A)
	{
		_InputVector temp;
		temp.a = a * A.a;
		temp.b = b * A.b;
		temp.c = c * A.c;
		temp.x = x * A.x;
		temp.y = y * A.y;
		temp.z = z * A.z;
		return temp;
	}
	
	_InputVector operator /(const _InputVector &A)
	{
		_InputVector temp;
		temp.a = a / A.a;
		temp.b = b / A.b;
		temp.c = c / A.c;
		temp.x = x / A.x;
		temp.y = y / A.y;
		temp.z = z / A.z;
		return temp;
	}

	_InputVector operator *(const float &A)
	{
		_InputVector temp;
		temp.a = a * A;
		temp.b = b * A;
		temp.c = c * A;
		temp.x = x * A;
		temp.y = y * A;
		temp.z = z * A;
		return temp;
	}
	_InputVector operator +(const float &A)
	{
		_InputVector temp;
		temp.a = a + A;
		temp.b = b + A;
		temp.c = c + A;
		temp.x = x + A;
		temp.y = y + A;
		temp.z = z + A;
		return temp;
	}

	_InputVector operator /(const float &A)
	{
		_InputVector temp;
		temp.a = a / A;
		temp.b = b / A;
		temp.c = c / A;
		temp.x = x / A;
		temp.y = y / A;
		temp.z = z / A;
		return temp;
	}


}InputVector;

typedef struct
{
	float l1;	//length of axis 1 
	float l2;	//length of axis 1
	float l3;
	float l4;

}OutputVector;

typedef struct
{
	XH3DPoint XHQuantitative[4]; //quantitative 
	XH3DPoint XHVariable[4]; //variable
	
}InitCoordinates;
extern InitCoordinates initCoor;
extern bool GetXD_DOFData(InputVector *input,OutputVector *output);


#endif