#pragma once
#include <Windows.h>
#include <gl/glut.h>
#include "3ds.h"
class CNumber
{
public:
	CNumber(void);
	virtual ~CNumber(void);
	//λ�á�ˮƽ����ֱ�ٶȡ���ת�Ƕȡ����ٶ�
	float x,y,z,vx,vy,rx,ry,rz,vr,scsize,ax,ay,az;
	int value;
	CLoad3DS mds;
	int distime;

	void Reset(double SCsize);
	//����λ��
	void Update();
	void Draw();
	//ģʽת��
	void ChangeMode(int level);
};
