#include "Number.h"

float Vpos[][2] = {{-5.0,2.0},{5.0,2.0},{0.0,-4.0},{0.0,4.0}};
float Spos[][2]={{-5.0,2.0},{-3.5,-4.0},{3.5,4.0},{5.0,2}};
float VLevel = 0.8;
CNumber::CNumber(void)
{
}

CNumber::~CNumber(void)
{
}

void CNumber::Draw()
{
	//DRAW
	glPushMatrix();
	glTranslatef(x,y,1.0);
	glRotatef(rx,1.0,0.0,0.0);
	glRotatef(ry,0.0,1.0,0.0);
	glRotatef(rz,0.0,0.0,1.0);
	glScalef(scsize,scsize,scsize);
	mds.show3ds();
	glPopMatrix();
}

void CNumber::Update()
{
	x+=vx;
	y+=vy;
	vx+=ax;
	vy+=ay;
	rx+=fabs(vr*30);
	ry+=fabs(vr*30);
	rz+=fabs(vr*30);
	if(x>6||x<-6)
		ChangeMode(3);
	if(y>5||y<-5)
		ChangeMode(3);
}

void CNumber::ChangeMode(int level)
{
	int res;
	switch(level)
	{
	case 1:								//直线
		res = rand()%4;
		x = Vpos[res][0];
		y = Vpos[res][1];
		switch(res)
		{
		case 0:							//R
			vx = 1.0/(rand()%5+5);
			vy = 0;
			break;
		case 1:
			vx = -1.0/(rand()%5+5);//L
			vy = 0;
			break;
		case 2:							//U
			vy = 1.0/(rand()%5+5);
			vx = 0;
			break;
		case 3:
			vy = -1.0/(rand()%5+5);//D
			vx = 0;
			break;
		}
		vr = 1.0/(rand()%20+10);		//旋转速度
		ax=ay=az=0.0;					//加速度置零
		break;
	case 2:								//带加速度
		res = rand()%4;
		x = Vpos[res][0];
		y = Vpos[res][1];
		switch(res)
		{
		case 0:							//R
			vx = 1.0/(rand()%5+7);
			ax = -vx/100.0;
			ay = 0.0;
			vy = 0;
			break;
		case 1:
			vx = -1.0/(rand()%5+7);//L
			ax = -vx/100.0;
			ay = 0.0;
			vy = 0;
			break;
		case 2:							//U
			vy = 1.0/(rand()%5+7);
			ay = -vy/100.0;
			ax = 0.0;
			vx = 0;
			break;
		case 3:
			vy = -1.0/(rand()%5+7);//D
			ay = -vy/100.0;
			ax = 0.0;
			vx = 0;
			break;
		}
		vr = 1.0/(rand()%20+10);		//旋转速度
		break;
	case 3:								//抛物线
		res = rand()%4;
		x = Spos[res][0];
		y = Spos[res][1];
		switch(res)
		{
		case 0:							//L
			vx = 1.0/(rand()%20+10);
			vy = 1.0/50.0;
			ax = 0.0;
			ay = -0.001;
			break;
		case 1:							//DL
			vx = 1.0/(rand()%10+25);
			vy = 1.0/(rand()%3+10);
			ax = 0.0;
			ay = -0.001;
			break;
		case 2:							//DR
			vx = -1.0/(rand()%10+25);
			vy = 1.0/(rand()%3+10);
			ax = 0.0;
			ay = -0.0011;
			break;
		case 3:							//R
			vx = -1.0/(rand()%20+10);
			vy = 1.0/50.0;
			ax = 0.0;
			ay = -0.0011;
			break;
		}
		vr = 1.0/(rand()%20+10);		//旋转速度
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	}
}

void CNumber::Reset(double SCsize)
{
	z=1.0;
	scsize=SCsize;
	value = 10;
	ChangeMode(3);
}