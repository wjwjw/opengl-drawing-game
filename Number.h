#pragma once
#include <Windows.h>
#include <gl/glut.h>
#include "3ds.h"
class CNumber
{
public:
	CNumber(void);
	virtual ~CNumber(void);
	//位置、水平和竖直速度、旋转角度、加速度
	float x,y,z,vx,vy,rx,ry,rz,vr,scsize,ax,ay,az;
	int value;
	CLoad3DS mds;
	int distime;

	void Reset(double SCsize);
	//更新位移
	void Update();
	void Draw();
	//模式转换
	void ChangeMode(int level);
};
