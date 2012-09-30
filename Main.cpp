#include "Head.h"
#include "geometry.h"
#include <vector>
#include "GLFont.h"
#include "3ds.h"
#include "Number.h"
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

/************************************************************************/
/* 下面是常量设置区                                                     */
/************************************************************************/
const int GL_WIN_WIDTH   = 800; //游戏宽度
const int GL_WIN_HEIGHT  = 600; //游戏高度

const float BACKGROUND_WIDTH  = 8;	 // 背景宽度
const float BACKGROUND_HEIGHT = 6;  // 背景高度
GLfloat halfWeight = BACKGROUND_WIDTH/2.0 ;
GLfloat halfHeight = BACKGROUND_HEIGHT/2.0;
const int MAX_PlantLineNum = 500;	//最大可画点数
const int POINTDISTIME = 80;		//星星消亡时间

int level = 1;						//游戏关卡


const float MAX_PLAINLEN = 12;

//游戏加载、进入菜单、进入游戏
enum { GAME_LOAD,GAME_MENU , GAME_MODE_1 , GAME_MODE_2 , GAME_SET,GAME_PAUSE,GAME_OVER};
const int BtnNums = 6;				//按钮数量
bool Buttons[BtnNums] = {0};		//按钮控制
const int TEXTURE_NUMS=25;			//纹理图数量
GLuint TextrueNames[TEXTURE_NUMS];	//纹理名称
wchar_t *const MYTEXT = L"○△□";
char *TextruefileNames[]={			//纹理路径
	"res/background.jpg",
	"res/btn_normalmode.jpg",
	"res/btn_challengemode.jpg",
	"res/btn_musicon.jpg",
	"res/btn_musicoff.jpg",	
	"res/btn_exit.jpg",
	"res/about_this_game.jpg",
	"res/font.jpg",
	"res/LoadingGround.jpg",
	"res/Loading.jpg",
	"res/bg2.jpg",
	"res/100.jpg",
	"res/200.jpg",
	"res/500.jpg",
	"res/1000.jpg",
	"res/3000.jpg",
	"res/5000.jpg",
	"res/10000.jpg",
	"res/30000.jpg",
	"res/50000.jpg",
	"res/99999.jpg",
	"res/music.jpg",
	"res/closeMusic.jpg",
	"res/btn_restar.jpg",
	"res/btn_return.jpg"
};
enum {
	TEX_BACKGROUND,
	TEX_BTN_NORMAL,
	TEX_BTN_CHALLENGE,
	TEX_BTN_MUSICON,
	TEX_BTN_MUSICOFF,
	TEX_BTN_EXIT,
	TEX_BTN_ABOUT,
	TEX_FONT,
	TEX_LGND,
	TEX_LOAD,
	TEX_BG2,
	TEX_MUSIC = 21,
	TEX_CLOSEMUSIC = 22,
	TEX_RESTART = 23,
	TEX_RETURN = 24
};
const int picNums = 10;							//弹窗图片数
const int keyScore[] = {100,200,500,1000,3000,5000,10000,30000,50000,99999};	//分数
const int picIdx[] = {11,12,13,14,15,16,17,18,19,20};
const int DSMODEL = 5;
CNumber my3ds[DSMODEL];				//3D模型加载
/************************************************************************/
/* 全局变量设置区                                         */
/************************************************************************/
bool mouse_l_btn_down = false;
bool mouse_r_btn_down = false;
bool Ispause[picNums];					//是否已经出现过图片
bool isless10;							//是否小于10秒
bool CloseMusic =false;					//默认开启音效
int game_state = GAME_MENU;
MyPoint PlantLine[MAX_PlantLineNum * 5]; // 为了后面方便计算，从1开始存点
POINT a;
int PlantLineNum = 0;
std::vector < MyPoint * > pointsvec;
std::vector < MyPoint * > deletepointvec;


GLint lefttime = 60;   //游戏时间
GLint lefttime_litle = 0;
GLuint score = 0;			//真实分数
GLuint nowscore = 0;		//显示分数
CGLFont myFont;
int scoreLevel[4] = {0,1,1,1}; //游戏得分的倍数 1 : 四边形 ，2 : 三角形 3 : 圆
int Mag_dely = 5;					//5秒更新一次倍数
float Load_Roatf = 0.0;
float plainlen;//画的长度
int selectEffect = 0 ;		//选中的音效
HGLRC hRC;
HDC hDC ;
/************************************************************************/
/* 下面是各种绘制函数区                                  */
/************************************************************************/
void DrawBackGround(int tex ,float w = halfWeight,float h = halfHeight ,
					float z=0.0f,float x=0.0,float y=0.0);
struct pic
{
	float x,y,vx,w,h;
	int tex,time;
	bool isupdate;
	bool flag;
	void update()
	{
		if(!isupdate)
			return;
		if(time<=0&&x-w>-4.0)
		{
			x=-4.0+w;
			time = 100;
		}
		else if(time>0)
		{
			if(--time==0)
			{
				flag=true;
				vx=-vx;
				x+=vx;
			}
		}
		else
		{
			x+=vx;
			if(x+w<-4.0)
			{
				if(flag)
					isupdate=false;
			}
		}
	}
	void set(int _tex=11,float _x=-5.0,float _y=1.2,float _vx=0.05
		,float _w=0.4,float _h=0.3,int _time=-1,bool isup=true,bool fl=false)
	{
		x = _x;	vx = _vx;	w = _w;
		y = _y;	h = _h;	time = _time;
		tex  = _tex;
		isupdate=isup;
		flag=fl;
	}
	void Draw()
	{
		if(isupdate)
			DrawBackGround(tex,w,h,0.1,x,y);
	}
}remind;
void DrawStartBtn(float x,float y,bool isDown,
				  float btnWidth,float btnHeight,int TEX_IDX,float z = 0.0)
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D,TextrueNames[TEX_IDX]);
	glPushMatrix();
	glTranslatef(x,y,0.00f);
	if(isDown)
		glScalef(0.9,0.9,1.0);
	//绘制背景矩阵
	glBegin(GL_QUADS);
	glTexCoord2f(0.0,0.0);glVertex3f(-btnWidth,-btnHeight,z);
	glTexCoord2f(1.0,0.0);glVertex3f(btnWidth,-btnHeight,z);
	glTexCoord2f(1.0,1.0);glVertex3f(btnWidth,btnHeight,z);
	glTexCoord2f(0.0,1.0);glVertex3f(-btnWidth,btnHeight,z);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void DrawBackGround(int tex ,float w ,float h,
					float z,float x,float y)
{

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D,TextrueNames[tex]);

	//绘制背景矩阵
	glPushMatrix();
	{
		glTranslatef(x,y,0.0);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);glVertex3f(-w,-h,z);
		glTexCoord2f(1.0,0.0);glVertex3f(w,-h,z);
		glTexCoord2f(1.0,1.0);glVertex3f(w,h,z);
		glTexCoord2f(0.0,1.0);glVertex3f(-w,h,z);
		glEnd();
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void DrawMouseOrbit()
{
// 	glDisable(GL_LIGHTING);
// 	glDisable(GL_TEXTURE_2D);
// 	glLineWidth(10);
// 	glColor3f(1.0,0.0,0.0);
// 	printf("这么多个点 ： %d\n",PlantLineNum);
// 	glBegin(GL_LINES);
// 	{
// 		for(int i = 2;i<=PlantLineNum;i++)
// 		{
// 			glVertex3f(PlantLine[i-1].x,PlantLine[i-1].y,1.0);
// 			glVertex3f(PlantLine[i].x,PlantLine[i].y,1.0);
// 		}
// 	}
// 	glEnd();
// 
// 
// 	glPointSize(5);
// 	glColor3f(0.0,1.0,0.0);
// 	glBegin(GL_POINTS);
// 	{
// 		for(int i = 1;i<=PlantLineNum;i++)
// 		{
// 			glVertex3f(PlantLine[i].x,PlantLine[i].y,1.1);
// 		}
// 	}
// 	glEnd();
	//glPopMatrix();
	//glutSwapBuffers();
}

void drawGameabout()
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D,TextrueNames[TEX_BTN_ABOUT]);
	glPushMatrix();
	glTranslatef(0.0,1.0,0.0);
	static GLfloat about_Height = 0.0f;
	about_Height-=0.005f;
	//绘制矩阵
	glBegin(GL_QUADS);
	if(about_Height<0.0001f)
		about_Height = 2.0f;
	if(about_Height<1.0001f)
	{
		glTexCoord2f(0.0,0.0);glVertex3f(-1.5,1.0-about_Height*2,0.01f);
		glTexCoord2f(1.0,0.0);glVertex3f(1.5,1.0-about_Height*2,0.01f);
		glTexCoord2f(1.0,about_Height);glVertex3f(1.5,1.0,0.01f);
		glTexCoord2f(0.0,about_Height);glVertex3f(-1.5,1.0,0.01f);
	}
	else if(about_Height<2.0001f)
	{
		glTexCoord2f(0.0,about_Height-1.0);glVertex3f(-1.5,-1.0,0.01f);
		glTexCoord2f(1.0,about_Height-1.0);glVertex3f(1.5,-1.0,0.01f);
		glTexCoord2f(1.0,1.0);glVertex3f(1.5,1.0-(about_Height-1.0)*2.0,0.01f);
		glTexCoord2f(0.0,1.0);glVertex3f(-1.5,1.0-(about_Height-1.0)*2.0,0.01f);
	}
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void DrawPoint(float x,float y)
{	
	float g = 0.2f;
	float r = 0.2f;
	int i=0;
	if((rand()%100)>5)
		glColor3f(0.99, 0.85, 0.19);
	else
		glColor3f(0.933, 0.874, 0.547);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x,y,0.0f); 
	i=0;
	for(float k = 0; k < 2*3.1415927; k += 3.1415927/5)
	{
		if(i==0){
			r = g *3/9;
			i=1;
		}
		else{
			r = g *1.5/9;
			i=0;
		}
		glVertex3f(r*cos(k) + x, r*sin(k) + y,1.0f);
	}
	glVertex3f(g*3/9+x, y,1.0f);
	glEnd();
	glColor3f(1.0f,1.0f,1.0f);
}
void DrawDeletePoint(float x,float y,int shapes)
{	
	if(shapes==0)
		shapes = 10;
	float r = 0.04f;

	if(rand()%20==0)
		r = 0.03f;

	int i=0;
	float k;

	if((rand()%100)>5)
		glColor3f(0.99, 0.99, 0.89);
	else
		glColor3f(0.533, 0.574, 0.547);

	glBegin(GL_TRIANGLE_FAN);
	for(k = 0; k < 2*3.1415927; k += 3.1415927*2/shapes)
	{
		glVertex3f(r*cos(k) + x, r*sin(k) + y,1.0f);
	}
	glVertex3f(r+x, y,1.0f);
	glEnd();

	glColor3f(1.0f,1.0f,1.0f);
}
void DrawDigits()
{
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glBindTexture(GL_TEXTURE_2D,TextrueNames[TEX_FONT]);


	if(game_state == GAME_MODE_1||game_state == GAME_PAUSE)
	{
	glColor4f(1.0,0.0,1.0,1.0);
	glPushMatrix();
 	glTranslatef(0.4f,0.0,1.1f);
 	glScalef(0.5,0.5,0.5);
	myFont.Print(ALIGN_LEFT,"Time: %d",lefttime);
	glPopMatrix();

	if(score!=nowscore)
		nowscore++;

	glColor4f(0.2,0.2,0.8,1.0);
	glPushMatrix();
	glTranslatef(-2.4f,0.0,1.1f);
	glScalef(0.5,0.5,0.5);
	myFont.Print(ALIGN_LEFT,"Score: %5d",nowscore);
	glPopMatrix();


	glColor4f(0.8,0.2,0.2,1.0);
	glPushMatrix();
	glTranslatef(3.0f,0.0,1.1f);
	glScalef(0.5,0.5,0.5);
	myFont.Print(ALIGN_LEFT,"Hint:");
	glPopMatrix();

	glColor4f(0.18,0.6,0.0,1.0);
	glPushMatrix();
	{
		glTranslatef(4.0f,-0.1,1.1f);
		//glScalef(0.65,0.65,0.65);
		myFont.PrintZh(ALIGN_LEFT,L"□");
		glScalef(0.65,0.65,0.65);
		glPushMatrix();
		{
			glTranslatef(-0.1,-0.0,0.0f);
			myFont.Print(ALIGN_LEFT,"    X%d",scoreLevel[1]);
		}
		glPopMatrix();
		glTranslatef(-0.1,-0.8,0.0f);
		myFont.PrintZh(ALIGN_LEFT,L"△");
		myFont.Print(ALIGN_LEFT,"    X%d",scoreLevel[2]);
		glTranslatef(0.0,-0.8,0.0f);
		myFont.PrintZh(ALIGN_LEFT,L"○");
		myFont.Print(ALIGN_LEFT,"    X%d",scoreLevel[3]);
	}
	glPopMatrix();
	}
	else if(game_state == GAME_OVER)
	{
		glPushMatrix();
		{
			glColor4f(0.2,0.2,0.8,1.0);
			glPushMatrix();
			glTranslatef(-1.4f,1.0f,1.1f);
			glScalef(0.5,0.5,0.5);
			myFont.Print(ALIGN_LEFT,"Your score: %5d",nowscore);
			glPopMatrix();

			glColor4f(0.8,0.2,0.2,1.0);
			glPushMatrix();
			glTranslatef(-3.0f,-2.0f,1.1f);
			glScalef(1.5f,1.5f,1.5f);
			myFont.Print(ALIGN_LEFT,"GAME OVER",nowscore);
			glPopMatrix();
		}
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glColor3f(1.0f,1.0f,1.0f);

}


void DrawInk()
{
	float Ink_hight = 0.1f;
	float Ink_weight = 5.0f;
	float Ink_r = 0.8f;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f,1.0f,1.0f);
	// 	if(PlantLineNum>MAX_PlantLineNum/2)
	// 		glColor3f(1.0f,0.0f,0.0f);
	// 	glBegin(GL_LINE_LOOP);
	// 	{
	// 		glVertex3f( 0.0f		,0.0f		,0.0f );
	// 		glVertex3f( Ink_weight	,0.0f		,0.0f );
	// 		glVertex3f( Ink_weight	,Ink_hight	,0.0f );
	// 		glVertex3f( 0.0f		,Ink_hight	,0.0f );
	// 	}		
	// 	glEnd();
	if(plainlen<MAX_PLAINLEN/3){
		glColor3f(0.99, 0.85, 0.19);
	}
	else{
		if(plainlen<2*MAX_PLAINLEN/3){
			glColor3f(0.99, 0.55, 0.09);
		}
		else{
			glColor3f(1.0f,0.0f,0.0f);
		}
	}
	// 	glBegin(GL_TRIANGLE_STRIP);
	// 	{
	// 		glVertex3f( 0.0f		,0.0f			,0.0f );
	// 		glVertex3f( Ink_weight - Ink_weight*((float)PlantLineNum)/((float)MAX_PlantLineNum)	,0.0f			,0.0f );
	// 		glVertex3f( Ink_weight - Ink_weight*((float)PlantLineNum)/((float)MAX_PlantLineNum)	,Ink_hight,0.0f );
	// 		glVertex3f( 0.0f		,Ink_hight		,0.0f );
	// 		glVertex3f( 0.0f		,0.0f			,0.0f );
	// 	}
	// 	glEnd();
	Ink_r -= Ink_r*((float)plainlen)/((float)MAX_PLAINLEN);
	glBegin(GL_TRIANGLE_FAN);
	for(float k = 0; k < 2*3.1415927; k += 3.1415927*2/30)
	{
		glVertex3f(Ink_r*cos(k), Ink_r*sin(k),0.0f);
	}
	glVertex3f(Ink_r, 0,0.0f);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f,1.0f,1.0f);
}


void DrawModel()
{
	remind.Draw();
	remind.update();
	DrawBackGround(TEX_BG2);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	for(int i=0;i<DSMODEL;i++)
	{
		if(my3ds[i].distime>0){
			if(my3ds[i].distime==1){
				my3ds[i].Reset(1.0/(rand()%20+40));
				my3ds[i].distime = 0;
			}
			else
			{
				// 				float x_temp = my3ds[i].x;
				// 				float y_temp = my3ds[i].y;
				// 				my3ds[i].Reset(my3ds[i].scsize*0.8);
				// 				my3ds[i].x = x_temp;
				// 				my3ds[i].y = y_temp;
				my3ds[i].scsize*=0.9;
				my3ds[i].distime--;
			}
		}
		glPushMatrix();
		my3ds[i].Draw();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	if(game_state!=GAME_OVER){
		//绘制文字
		glPushMatrix();
		glTranslatef(-1.5f,2.5f,0.0f);
		DrawDigits();
		glPopMatrix();

		//绘制酱油瓶
		glPushMatrix();
		glTranslatef(-4.0f,-3.0f,0.2f);
		DrawInk();
		glPopMatrix();
	}
}
void DrawGameLoading()
{
	DrawBackGround(TEX_LGND);
	glPushMatrix();
	glRotatef(Load_Roatf*10*(Load_Roatf/360.0),0,0,1);
	DrawBackGround(TEX_LOAD,175.0/400.0,175.0/400.0,0.01);
	glPopMatrix();
}
/************************************************************************/
/* 下面是各种逻辑函数区                                                 */
/************************************************************************/
GLvoid InitZh(wchar_t *text, LPCWSTR zFontName =L"宋体");
DWORD WINAPI Loading( LPVOID lpParameter )
{
	mciSendString("play sounds/menubg.mp3 repeat",NULL,0,NULL);
	plainlen = 0;
	int total = 3000;
	while(total>=0 )
	{
		Sleep(5);
		total-=5;
		Load_Roatf = (total/3000.0)*360.0;
		glutPostRedisplay();
	}
	game_state = GAME_MENU;
	return 0;
}
DWORD WINAPI PlayEffect( LPVOID lpParameter )
{
	char buf[30];
	sprintf(buf,"play  sounds/effect%d.wav",selectEffect);
	selectEffect = (selectEffect+1)%11;
	mciSendString(buf,NULL,0,NULL);
	return 0;
}
void GameInit()
{
	srand(GetTickCount());
	glEnable(GL_DEPTH_TEST);//初始化纹理，绑定纹理ID
	//设置固定的透视投影视域体
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glClearColor(0.378, 0.563, 0.626 ,1.0);
	//gluOrtho2D(-BACKGROUND_WIDTH / 2, BACKGROUND_WIDTH / 2, -BACKGROUND_HEIGHT / 2, BACKGROUND_HEIGHT / 2);
	glOrtho(-BACKGROUND_WIDTH / 2, BACKGROUND_WIDTH / 2, -BACKGROUND_HEIGHT / 2, BACKGROUND_HEIGHT / 2,-10.0f,10.0f);
	game_state = GAME_LOAD;

	// 开启和设置混合(实现半透明效果)
	glEnable(GL_BLEND);											// 开启混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			// 设置混合模式
	
	for(int i=0;i<TEXTURE_NUMS;i++)
		BuildTexture(TextruefileNames[i],TextrueNames[i]);
	myFont.Init();
	myFont.InitZh(MYTEXT,L"宋体");
	for(int i=0; i<DSMODEL ;i++)
	{
		char str[20];
		sprintf(str,"res/NO%d-0.3ds",i);
		my3ds[i].mds.Init(str);
		my3ds[i].Reset(1.0/(rand()%20+40));
	}
	HANDLE hThread = CreateThread(NULL,0,Loading,NULL,0,NULL);
	CloseHandle(hThread);
	memset(Ispause,0,sizeof(Ispause));
	isless10=false;
}
void GameReset()
{
	selectEffect = 0 ;
	mciSendString("stop  sounds/time2.mp3",NULL,0,NULL);
	memset(Ispause,0,sizeof(Ispause));
	isless10=false;
	pointsvec.clear();
	deletepointvec.clear();
	score = 0;
	nowscore = 0;
	lefttime = 60;
	lefttime_litle = 0;
	game_state = GAME_MODE_1;
	mouse_l_btn_down = false;
	mouse_r_btn_down = false;
	Load_Roatf = 0.0f;
	scoreLevel[1]=scoreLevel[2]=scoreLevel[3]=1;
	for(int i=0;i<DSMODEL;i++)
		my3ds[i].Reset(1.0/(rand()%20+40));
}
void JudgeCricleScore(float x,float y,float r)
{
	for(int i = 0 ; i < DSMODEL ; i++)
	{
		if( MyPoint(my3ds[i].x ,my3ds[i].y).Dis_PointToPoint(MyPoint(x,y)) < r)
		{
			if(my3ds[i].distime)
				continue;
			if( MyPoint(my3ds[i].x ,my3ds[i].y).Dis_PointToPoint(MyPoint(x,y)) < r)
			{
// 				score_temp += my3ds[i].value;

				for(int j=0;j<scoreLevel[3];j++){
					MyPoint *pointrtemp;//模型销毁时
					pointrtemp= new MyPoint(my3ds[i].x,my3ds[i].y,0.02 - 0.05/(rand()%20+1),0.05/(rand()%20+1),1,0.0f,0.0f);
					pointrtemp->shape = 0;
					deletepointvec.push_back(pointrtemp);


					if(my3ds[i].distime==0)
						my3ds[i].distime = 20;
				}

			}
		}
	}
}
void JudgeSquareScore(MyPoint MP[] , int numPoint)
{
	for(int i = 0 ; i < DSMODEL ; i++)
	{
		if(my3ds[i].distime)
			continue;
		int j;
		for(j = 0 ; j < numPoint  ; j++)
		{
			if( PointofLine(MyPoint(my3ds[i].x ,my3ds[i].y) , MyLine(MP[j] , MP[j+1])) == POINT_RIGHTofLINE)
			{
				break;
			}
		}
		if(j == numPoint )
		{
			int sum = 0;
			if(numPoint == 3)
				sum = scoreLevel[2];
			else
				sum = scoreLevel[1];
			for(int k=0;k<sum;k++){
				MyPoint *pointrtemp;//模型销毁时
				pointrtemp= new MyPoint(my3ds[i].x,my3ds[i].y,0.02 - 0.05/(rand()%20+1),0.05/(rand()%20+1),1,0.0f,0.0f);
				pointrtemp->shape = numPoint;
				deletepointvec.push_back(pointrtemp);
				if(my3ds[i].distime==0)
					my3ds[i].distime = 20;
			}

		}
	}
}
void PushLine(float _x1,float _y1,float _x2, float _y2,float len)
{
	if( MyPoint(_x1,_y1).Dis_PointToPoint( MyPoint(_x2,_y2)) > (len * 2) )
	{
		MyPoint *pointrtemp;

		pointrtemp= new MyPoint((_x1 + _x2)/2,(_y1 + _y2)/2,0,0,-POINTDISTIME*0.6);
		pointsvec.push_back(pointrtemp);
		PushLine(_x1,_y1,(_x1 + _x2)/2,(_y1 + _y2)/2,len);
		PushLine(_x2,_y2,(_x1 + _x2)/2,(_y1 + _y2)/2,len);
	}
}
void PushCricle(float _x,float _y,float _r)
{
	float i;
	MyPoint *pointrtemp;
	float C = 2 * _r * PI; 
	//星星是约1/10的大小,星星之间留半个星星位置
	int num_star = int(C / 0.15);
	float temp = 360.0 / num_star;
	for (i=0;i<(360);i+=temp)
	{
		pointrtemp= new MyPoint(_x + cos((i)/180.0 * PI) * _r,_y + sin((i)/180.0 * PI) * _r,0,0,-POINTDISTIME*0.6);
		pointsvec.push_back(pointrtemp);
	}
}
void PushFourPoint(float _x1,float _y1,float _x2,float _y2,float _x3,float _y3,float _x4,float _y4)
{
	PushLine(_x1,_y1,_x2,_y2,0.12);
	PushLine(_x2,_y2,_x3,_y3,0.12);
	PushLine(_x3,_y3,_x4,_y4,0.12);
	PushLine(_x4,_y4,_x1,_y1,0.12);
}
void PushThreePoint(float _x1,float _y1,float _x2,float _y2,float _x3,float _y3)
{
	PushLine(_x1,_y1,_x2,_y2,0.12);
	PushLine(_x2,_y2,_x3,_y3,0.12);
	PushLine(_x3,_y3,_x1,_y1,0.12);
}
float Cal_k()
{
	int i;
	float temp = 0;
	for (i=1;i < PlantLineNum ;i++)
	{
		temp += PlantLine[i].Dis_PointToPoint(PlantLine[i+1]);
	}
	return (temp / PlantLineNum);
}

void JudgeClose(int &close1,int &close2)
{
	int i,j;
	int min_cross_point_len = inf;
	float min_oneToLine = inf;
	int temp1,temp2;
	int num_cross = 0; // 这个新加的变量来记录有多少个相交点，太多相交点，就是不合法的图形
	//先判断图形是不是近似闭合的,把图形的开始点和结尾点稍微延长一点
	/*  △x^2 + △y^2 = k^2
		△y / △x = t
		其中 k是延长的近似的长度，t = (y1 - y2)/(x1 - x2) 为斜率
		求得: (t^2+1)△x^2 = k^2
	*/
	//k的值要按照比例来确定
	float k = Cal_k() * 3;
	if(k < 0.3)   k = 0.3;
	printf("k = %lf \n",k);//调试专用
	if(PlantLineNum > 1)
	{
		//这是开始点的延长
		double t = (PlantLine[1].y - PlantLine[2].y)/(PlantLine[1].x - PlantLine[2].x);
		double tx = (double)sqrt((k*k)/(t*t+1));

		if (fabs(PlantLine[1].x - PlantLine[2].x) < 0.00001)
		{
			PlantLine[0].x = PlantLine[1].x;
			if(PlantLine[1].y < PlantLine[2].y)
				PlantLine[0].y = PlantLine[1].y - k;
			else
				PlantLine[0].y = PlantLine[1].y + k;
		}
		else
		{
			if(PlantLine[1].x < PlantLine[2].x)
				PlantLine[0].x = PlantLine[1].x - tx;
			else
				PlantLine[0].x = PlantLine[1].x + tx;

			if(PlantLine[1].y < PlantLine[2].y)
				PlantLine[0].y = PlantLine[1].y - fabs(tx * t);
			else
				PlantLine[0].y = PlantLine[1].y + fabs(tx * t);
		}

		//同理延长结束点
		t = (PlantLine[PlantLineNum].y - PlantLine[PlantLineNum-1].y)/(PlantLine[PlantLineNum].x - PlantLine[PlantLineNum-1].x);
		tx = sqrt((k*k)/(t*t+1));

		if(fabs(PlantLine[PlantLineNum].x - PlantLine[PlantLineNum-1].x) < 0.00001)
		{
			PlantLine[PlantLineNum+1].x = PlantLine[PlantLineNum].x;
			if(PlantLine[PlantLineNum].y < PlantLine[PlantLineNum-1].y)
				PlantLine[PlantLineNum+1].y = PlantLine[PlantLineNum].y - k;
			else
				PlantLine[PlantLineNum+1].y = PlantLine[PlantLineNum].y + k;
		}
		else
		{
			if(PlantLine[PlantLineNum].x < PlantLine[PlantLineNum-1].x)
				PlantLine[PlantLineNum+1].x = PlantLine[PlantLineNum].x - tx;
			else
				PlantLine[PlantLineNum+1].x = PlantLine[PlantLineNum].x + tx;

			if(PlantLine[PlantLineNum].y < PlantLine[PlantLineNum-1].y)
				PlantLine[PlantLineNum+1].y = PlantLine[PlantLineNum].y - fabs(tx * t);
			else
				PlantLine[PlantLineNum+1].y = PlantLine[PlantLineNum].y + fabs(tx * t);
		}

		//然后再开始判断方法一：判断两两线段之间有没有香蕉
		/*printf("0 :  %lf \t %lf\n",PlantLine[0].x,PlantLine[0].y);
		printf("1 :  %lf \t %lf\n",PlantLine[1].x,PlantLine[1].y);
		printf("2 :  %lf \t %lf\n",PlantLine[2].x,PlantLine[2].y);
		printf("尾2 ： %lf \t %lf\n",PlantLine[PlantLineNum-1].x,PlantLine[PlantLineNum-1].y,1);
		printf("尾1 ： %lf \t %lf\n",PlantLine[PlantLineNum].x,PlantLine[PlantLineNum].y);
		printf("尾0 ： %lf \t %lf\n",PlantLine[PlantLineNum+1].x,PlantLine[PlantLineNum+1].y,1);*/
		for(i=0;i<=PlantLineNum;i++)
		{
			for(j=0;j<=PlantLineNum;j++)
			{
				if( abs(i-j) < 6 ) break;//距离很近的点，难免相交，不考虑
				MyPoint temp = isCross(MyLine(PlantLine[i],PlantLine[i+1]),MyLine(PlantLine[j],PlantLine[j+1]));
				if(temp.isExist())
				{
					num_cross ++;
// 					if(max < i || max < j)
// 					{
// 						max = i > j ? i : j;
// 						temp1 = i;
// 						temp2 = j;
// 					}
					if(min_cross_point_len > abs(i-j))
					{
						min_cross_point_len = abs(i-j);
						temp1 = i;
						temp2 = j;
					}
					/*/下面调试专用
					printf("%d 和 %d 有香蕉\n",i,j);
					glMatrixMode(GL_MODELVIEW); 
					glLoadIdentity();
					glLineWidth(10);
					glColor3f(0.0,0.0,1.0);
					glBegin(GL_LINES);
					{
					glVertex3f(PlantLine[i].x,PlantLine[i].y,1.2);
					glVertex3f(PlantLine[i+1].x,PlantLine[i+1].y,1.2);
					glVertex3f(PlantLine[j].x,PlantLine[j].y,1.2);
					glVertex3f(PlantLine[j+1].x,PlantLine[j+1].y,1.2);
					}
					glEnd();
					glutSwapBuffers();
					glFlush();
					//上面调试专用*/
				}
			}
		}
		//如果上面找到了相交点，就不用方法二了
		//然后再开始判断方法二（1）：判断第一点和线段的距离
		if(min_cross_point_len == inf)
		{
			float len_l,len_mid,len_r;
			float l_pointToLine;
			const float LEN = k * 0.8;
			len_r = PlantLine[1].Dis_PointToPoint(PlantLine[19]);
			for(i = 20;i<=PlantLineNum;i++)
			{
				len_l = len_r;
				len_mid = MyLine(PlantLine[i-1],PlantLine[i]).Dis_PointToLine(PlantLine[1]);
				len_r = PlantLine[1].Dis_PointToPoint(PlantLine[i]);
				if( (len_mid < len_l && len_mid < len_r) || (len_mid > len_l && len_mid > len_r) )
				{
					l_pointToLine = len_l < len_r ? len_l : len_r;
				}
				else
				{
					l_pointToLine = len_mid;
				}
				if(l_pointToLine < LEN && l_pointToLine < min_oneToLine)
				{
					min_oneToLine = l_pointToLine;
					temp1 = 1;
					temp2 = i-1;
					/*/下面调试专用
					printf("1点和<%d,%d>线有苹果\n",i-1,i);
					glMatrixMode(GL_MODELVIEW); 
					glLoadIdentity();
					glLineWidth(10);
					glColor3f(0.0,1.0,1.0);
					glBegin(GL_LINES);
					{
					glVertex3f(PlantLine[1].x,PlantLine[1].y,1.2);
					glVertex3f(PlantLine[i-1].x,PlantLine[i-1].y,1.2);
					glVertex3f(PlantLine[i].x,PlantLine[i].y,1.2);
					glVertex3f(PlantLine[1].x,PlantLine[1].y,1.2);
					}
					glEnd();
					glutSwapBuffers();
					glFlush();
					//上面调试专用*/
				}
			}

			//然后再开始判断方法二（2）：判断最后一点和线段的距离
			len_r = PlantLine[PlantLineNum].Dis_PointToPoint(PlantLine[PlantLineNum-19]);
			for(i = PlantLineNum-20;i>0;i--)
			{
				len_l = len_r;
				len_mid = MyLine(PlantLine[i],PlantLine[i+1]).Dis_PointToLine(PlantLine[PlantLineNum]);
				len_r = PlantLine[PlantLineNum].Dis_PointToPoint(PlantLine[i]);
				if( (len_mid < len_l && len_mid < len_r) || (len_mid > len_l && len_mid > len_r) )
				{
					l_pointToLine = len_l < len_r ? len_l : len_r;
				}
				else
				{
					l_pointToLine = len_mid;
				}
				if(l_pointToLine < LEN && l_pointToLine < min_oneToLine)
				{
					min_oneToLine = l_pointToLine;
					temp1 = i;
					temp2 = PlantLineNum;
					/*/下面调试专用
					printf("最后一点和<%d,%d>线有苹果\n",i-1,i);
					glMatrixMode(GL_MODELVIEW); 
					glLoadIdentity();
					glLineWidth(10);
					glColor3f(0.0,1.0,1.0);
					glBegin(GL_LINES);
					{
					glVertex3f(PlantLine[PlantLineNum].x,PlantLine[PlantLineNum].y,1.2);
					glVertex3f(PlantLine[i+1].x,PlantLine[i+1].y,1.2);
					glVertex3f(PlantLine[i].x,PlantLine[i].y,1.2);
					glVertex3f(PlantLine[PlantLineNum].x,PlantLine[PlantLineNum].y,1.2);
					}
					glEnd();
					glutSwapBuffers();
					glFlush();
					//上面调试专用*/
				}
			}
		}
	}
	else
	{
		printf("两个点都没有也想哄我？!!\n");
	}

	if(min_cross_point_len == inf && min_oneToLine == inf)
	{
		close1 = close2 = -1;
	}
	else
	{
		close1 = temp1 > temp2 ? temp2 : temp1;
		close2 = temp2 > temp1 ? temp2 : temp1;
	}
}

void JudgeShape(int s,int t)
{
	MyPoint temppoint = PlantLine[t+1];
	PlantLine[t+1] = PlantLine[s];
	int i;
	float k = 120.0;
	float temp;
	int sum1 = 0,sum2 = 0 , sum3 = 0;
	//这三个用来算突变点的
	int cross[1000];           //突变点的下标
	MyPoint crossPoint[1000]; //晕了，开一个数组来记录突变点，排序用的
	int index = 1;
	int cross_len = 6;//突变点距离
	int lase_cross = 0;
	cross[0] = s;
	printf("下面开始算角度：%d %d\n",s,t);
	for(i=s;i<t;i++)
	{
		temp = AngleofLines(MyLine(PlantLine[i+1],PlantLine[i]) , MyLine(PlantLine[i+1],PlantLine[i+2]));
		if(temp != inf)
		{
			if(temp == 180.0) sum3++;
			if(temp > 175) sum1 ++;
			if(temp <= 175 && temp >160) sum2 ++;
			//if(temp <= 160 && temp >120)
			//{
			//	//下面调试专用 : 画出那个点
			//	glMatrixMode(GL_MODELVIEW); 
			//	glLoadIdentity();
			//	glLineWidth(10);
			//	glColor3f(0.0,0.0,0.0);
			//	glBegin(GL_POINTS);
			//	{
			//	glVertex3f(PlantLine[i].x,PlantLine[i].y,1.3);
			//	}
			//	glEnd();
			//	glutSwapBuffers();
			//	glFlush();
			//	//上面调试专用*/

			//	printf("!%012.3lf!  ",temp);
			//}
			//else
			if(temp <= 160)
			{
				/*/下面调试专用 : 画出那个点
				glMatrixMode(GL_MODELVIEW); 
				glLoadIdentity();
				glLineWidth(10);
				glColor3f(1.0,1.0,1.0);
				glBegin(GL_POINTS);
				{
				glVertex3f(PlantLine[i].x,PlantLine[i].y,1.3);
				}
				glEnd();
				glutSwapBuffers();
				glFlush();
				//上面调试专用*/

				
				//printf("%d\n",lase_cross);
				//if(i - lase_cross > cross_len && t - cross_len > i)
				if(i - lase_cross > cross_len && t - cross_len > i)
				{
					cross[index] = i;
					index++;
				}
				lase_cross = i;
				printf("<%012.3lf>  ",temp);//调试专用
			}
			else
			{
				printf("%14.3lf  ",temp);//调试专用
			}
		}
		else
			printf("\t   INF  ");
	}
	printf("\n");//调试专用
	printf("等于180的情况 ： %d / %d -- %.4lf\n",sum3,t-s+1,(double(sum3) / (t-s+1)));//调试专用
	printf("大于175的情况 ： %d / %d -- %.4lf\n",sum1,t-s+1,(double(sum1) / (t-s+1)));//调试专用
	printf("160-175的情况 ： %d / %d -- %.4lf\n",sum2,t-s+1,(double(sum2) / (t-s+1)));//调试专用


	if(index > 6)
		printf("晕死，这是什么图形啊\n");

	printf("突变距离 ： %d 有这么多个突变点 %d\n",cross_len,index);
	for (i = 0; i < index ; i++)
	{
		crossPoint[i] = PlantLine[cross[i]];
	}
	
	if( ( (double(sum1) / (t-s))  < 0.45 && (double(sum2) / (t-s)) > 0.45) || index >= 6)//暂时把这种情况看为是一个圆
	{
		printf("圆 : \n");
		MyPoint O_mid;
		float O_r;
		float temp_x = 0, temp_y = 0 , temp_r = 0;
		for(i = s ; i <= t ; i++)
		{
			temp_x += PlantLine[i].x;
			temp_y += PlantLine[i].y;
		}
		O_mid.x = temp_x / (t-s+1);
		O_mid.y = temp_y / (t-s+1);
		for (i = s ; i<= t ; i++)
		{
			temp_r += O_mid.Dis_PointToPoint(PlantLine[i]);
		}
		O_r = temp_r / (t-s+1);
		/*/下面调试专用 ： 画一个圆
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		glLineWidth(10);
		glColor3f(1.0,1.0,0.0);
		glBegin(GL_LINES);
		{
			for(i = 0 ;i < 360 ; i ++)
			{

				glVertex3f(O_mid.x + O_r * cos( i / 180.0  * PI),O_mid.y + O_r * sin( i / 180.0 * PI),1.5);
				glVertex3f(O_mid.x + O_r * cos( (i+1) / 180.0 * PI),O_mid.y + O_r * sin( (i+1) / 180.0 * PI),1.5);
			}
		}
		glEnd();
		glutSwapBuffers();
		glFlush();
		//上面调试专用 */
		PushCricle(O_mid.x,O_mid.y,O_r);
		JudgeCricleScore(O_mid.x,O_mid.y,O_r);
	}
	else
	{
		/*/下面调试专用 ： 画出突变点
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		glLineWidth(10);
		glColor3f(1.0,1.0,0.0);
		glBegin(GL_LINES);
		{
			for(i = 0 ;i < index - 1; i ++)
			{
				printf("%d ",cross[i]);
				glVertex3f( PlantLine[ cross[i] ].x,PlantLine[ cross[i] ].y,1.5);
				glVertex3f( PlantLine[ cross[i+1] ].x,PlantLine[ cross[i+1] ].y,1.5);
			}
		}
		glVertex3f( PlantLine[ cross[index-1] ].x,PlantLine[ cross[index-1] ].y,1.5);
		glVertex3f( PlantLine[ cross[0] ].x,PlantLine[ cross[0] ].y,1.5);
		printf("%d\n",cross[index-1]);
		glEnd();
		glutSwapBuffers();
		glFlush();
		//上面调试专用 */

		//给点逆时针排序
		SortPoint(crossPoint,index);
		//调试专用 排序结果
		for(int i  = 0 ;i < index ; i++)
		{
			printf(" %d  %.4lf %.4lf\n",i,crossPoint[i].x,crossPoint[i].y);
		}
		//调试专用*/
		crossPoint[index] =crossPoint[0];
		if(index == 4)
		{
			PushFourPoint(PlantLine[cross[0]].x,PlantLine[cross[0]].y,
						  PlantLine[cross[1]].x,PlantLine[cross[1]].y,
						  PlantLine[cross[2]].x,PlantLine[cross[2]].y,
						  PlantLine[cross[3]].x,PlantLine[cross[3]].y);
			JudgeSquareScore(crossPoint,index);
		}
		if(index == 3)
		{
			PushThreePoint(PlantLine[cross[0]].x,PlantLine[cross[0]].y,
						   PlantLine[cross[1]].x,PlantLine[cross[1]].y,
						   PlantLine[cross[2]].x,PlantLine[cross[2]].y);
			JudgeSquareScore(crossPoint,index);
		}
	}



	PlantLine[t+1] = temppoint;
}



void UpdateAllPoint()
{
	//start
	std::vector <MyPoint*>::iterator it;
	for(it = pointsvec.begin();it!=pointsvec.end();)
	{
		if( (*it)->distime == 0)
		{
			delete (*it);  
			it = pointsvec.erase(it);  
		}
		else
			it++;  
	}
	for(it = pointsvec.begin();it!=pointsvec.end();it++)
		(*it)->Update();

	//销毁效果点
	for(it = deletepointvec.begin();it!=deletepointvec.end();)
	{
		if( (*it)->x<-4.0f|(*it)->y>3.0f)
		{
			delete (*it);  
			it = deletepointvec.erase(it);  
			HANDLE hThread = CreateThread(NULL,0,PlayEffect,NULL,0,NULL);
			CloseHandle(hThread);
			score+=10;
		}
		else
			it++;  
	}
	//弹窗
	for(int i=0;i<10;i++)
	{
		if(score>=keyScore[i]&&!Ispause[i])
		{
			Ispause[i]=true;
			remind.set(picIdx[i]);
		}

	}
	for(it = deletepointvec.begin();it!=deletepointvec.end();it++)
	{
		// 		(*it)->ax = 0.0f;
		// 		(*it)->ay = 0.0f;
		// 		(*it)->v_x = 0.04f*((*it)->x-2.5f);
		// 		(*it)->v_y = 0.03f*((*it)->y+2.5f);
		float temp_x = fabs((*it)->x+2.5f);
		float temp_y = fabs((*it)->y-2.5f);
		(*it)->ax = (*it)->ay = 0.0f;
		if(temp_x-temp_y>0.05)
			(*it)->ax = -0.01f;
		if(temp_y-temp_x>0.05)
			(*it)->ay = 0.01f;
		(*it)->Update();
	}
}
void DrawMovingPoint()
{
	//更新并绘制点
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	std::vector <MyPoint*>::iterator it;
	for(it = pointsvec.begin();it!=pointsvec.end();it++)
	{
		DrawPoint((*it)->x,(*it)->y);
	}
	for(int i=1;i<=PlantLineNum;i++)
	{
		DrawPoint(PlantLine[i].x,PlantLine[i].y);
	}

	for(it = deletepointvec.begin();it!=deletepointvec.end();it++)
	{
		DrawDeletePoint((*it)->x,(*it)->y,(*it)->shape);
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

}

/************************************************************************/
/* 线程区				                                                */
/************************************************************************/
void RunGameLoad()
{
	DrawGameLoading();
}
void RunGameMenu()
{
	 DrawBackGround(TEX_BACKGROUND);
	 DrawStartBtn(-2.7,2.2,Buttons[0],0.4,0.4,TEX_BTN_NORMAL,0.01f);
	 DrawStartBtn(-2.7,1.6,Buttons[1],0.4,0.4,TEX_BTN_CHALLENGE,0.01f);
	 if(!CloseMusic)
		 DrawStartBtn(-2.7,1.0,Buttons[2],0.4,0.4,TEX_BTN_MUSICON,0.01f);
	 else
		 DrawStartBtn(-2.7,1.0,Buttons[2],0.4,0.4,TEX_BTN_MUSICOFF,0.01f);
	 DrawStartBtn(-2.7,0.4,Buttons[3],0.4,0.4,TEX_BTN_EXIT,0.01f);
	 drawGameabout();
}
void RunGameRunGameMode1()
{
	bool judging = false;
	for(int i=0;i<DSMODEL;i++)
	{
		if(my3ds[i].distime>0)
		{
			judging = true;
			break;
		}
	}
	//更新模型
	if(!judging)
		for(int i=0;i<DSMODEL;i++)
		{
			if(my3ds[i].distime==0)
				my3ds[i].Update();
		}
		//更新剩余时间
		lefttime_litle++;
		if(lefttime_litle==60){
			lefttime_litle = 0;
			if(--lefttime<=10&&!isless10)
			{
				isless10=true;
				mciSendString("play sounds/time2.mp3 repeat",NULL,0,NULL);
			}
			if(--Mag_dely ==0)
			{
				scoreLevel[1] = 1+rand()%3;
				scoreLevel[2] = 1+rand()%3;
				scoreLevel[3] = 1+rand()%3;
				Mag_dely = 5+rand()%10;
			}
			if(lefttime <= 0) lefttime = 0;
		}
		if(lefttime == 0&&game_state == GAME_MODE_1)
		{

			mciSendString("stop sounds/time2.mp3",NULL,0,NULL);
			game_state = GAME_OVER;
		}
}
void RunGameOver()
{
	glPushMatrix();
	DrawDigits();
	glPopMatrix();
	glPushMatrix();
		DrawStartBtn(-2.7,2.2,Buttons[4],0.4,0.4,TEX_RESTART,0.5f);
		DrawStartBtn(-2.7,0.4,Buttons[5],0.4,0.4,TEX_RETURN,0.5f);
	glPopMatrix();
}
/************************************************************************/
/* 下面是各种回调函数区                                                 */
/************************************************************************/
void glutDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	if(game_state == GAME_MODE_1||game_state == GAME_PAUSE||game_state == GAME_OVER)
		DrawModel();
	else if(game_state == GAME_LOAD)
		RunGameLoad();
	else if(game_state == GAME_MENU)
		RunGameMenu();
	if(game_state == GAME_OVER)
		RunGameOver();

	//绘制点
	DrawMovingPoint();

	glutSwapBuffers();
}

/*窗口调整回调函数*/
void glutResize(int width,int height)
{
	glutReshapeWindow(GL_WIN_WIDTH,GL_WIN_HEIGHT);
}

/*普通按键回调函数*/
void glutKeyboard(unsigned char key/*ASCII码*/,int x,int y)
{
	if(key == 27)
	{
		if(game_state == GAME_MENU)
			exit(0);
		else if(game_state == GAME_MODE_1 || game_state == GAME_OVER)
		{
			GameReset();
			mciSendString("stop  sounds/gamemode1bg.mp3",NULL,0,NULL);
			mciSendString("play  sounds/menubg.mp3 repeat",NULL,0,NULL);
			game_state = GAME_MENU;
		}
	}
	if(key == 'P' || key == 'p')
	{
		if(game_state==GAME_MODE_1)
			game_state = GAME_PAUSE;
		else if(game_state==GAME_PAUSE)
			game_state = GAME_MODE_1;
	}
	if(key == 'r' || key == 'R')
	{
		PlantLineNum = 0;
	}
	if(key == 'j' || key == 'J')
	{
		remind.set(11);
	}
	if(key == 'R' || key == 'r')
	{
		GameReset();
	}
	if(key == 'O' || key == 'o')
	{
		if(game_state == GAME_MODE_1)
		{
			if(CloseMusic)			//开启
			{
				remind.set(TEX_MUSIC);
				mciSendString("play  sounds/gamemode1bg.mp3 repeat",NULL,0,NULL);
			}
			else
			{
				remind.set(TEX_CLOSEMUSIC);
				mciSendString("pause  sounds/gamemode1bg.mp3",NULL,0,NULL);
			}
			CloseMusic = !CloseMusic;
		}
		if(game_state == GAME_MENU)
		{
			if(CloseMusic)			//开启
			{
				mciSendString("play  sounds/menubg.mp3 repeat",NULL,0,NULL);
			}
			else
			{
				mciSendString("pause  sounds/menubg.mp3",NULL,0,NULL);
			}
			CloseMusic = !CloseMusic;
		}
	}
	/*等待添加*/
}

/*特殊按键回调函数*/
void glutSpecial(int value,int x,int y)
{
	/*等待添加*/
}

/*鼠标回调函数*/
void glutMouse(int button,int state,int x,int y)
{
	
	if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		mouse_l_btn_down = true;
	if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		mouse_l_btn_down = false;

	if(game_state == GAME_LOAD)
		return ;
	if((game_state == GAME_MENU||game_state == GAME_OVER) && button == GLUT_LEFT_BUTTON )
	{

		GLuint selectBuf[100];		// 名称缓存，用于拾取
		GLint viewport[4];			// 用于存储视口属性
		glGetIntegerv (GL_VIEWPORT, viewport);	// 获取视口属性(后面gluPickMatrix要用)
		glSelectBuffer (100, selectBuf);		// 设置名称缓存
		glRenderMode(GL_SELECT);				// 进入选择模式
		glInitNames();							// 初始化名称堆栈
		glPushName(0);							// 将0压入空栈

		// 设置拾取采用的观察条件
		glMatrixMode(GL_PROJECTION);			// 选择投影矩阵为当前矩阵
		glPushMatrix();							// 保存当前矩阵(压入栈)
		glLoadIdentity();						// 重置投影矩阵为恒等矩阵

		// 将鼠标位置附近3×3的区域设为拾取区域
		// 注意gluPickMatrix中前两个参数为投影中心坐标，采用的是窗口坐标系(原点在窗口左下角，
		// x向右，y向上，即和glViewport坐标系相同，注意并非设备坐标系，因此要反转y值)
		gluPickMatrix ((GLdouble)x, (GLdouble)(viewport[3] - y),
			3.0, 3.0, viewport);
		// 待添加视域体设置	
		glOrtho(-BACKGROUND_WIDTH / 2, BACKGROUND_WIDTH / 2, -BACKGROUND_HEIGHT / 2,
			BACKGROUND_HEIGHT / 2,-10,10);

		// 待添加拾取目标绘制代码
		if(game_state == GAME_MENU){
			glLoadName(1);
			DrawStartBtn(-2.7,2.2,false,0.4,0.4,TEX_BTN_NORMAL,0.01f);
			glLoadName(2);
			DrawStartBtn(-2.7,1.6,false,0.4,0.4,TEX_BTN_CHALLENGE,0.01f);
			glLoadName(3);
			if(!CloseMusic){
				DrawStartBtn(-2.7,1.0,false,0.4,0.4,TEX_BTN_MUSICON,0.01f);
			}else
				DrawStartBtn(-2.7,1.0,false,0.4,0.4,TEX_BTN_MUSICOFF,0.01f);
			glLoadName(4);
			DrawStartBtn(-2.7,0.4,false,0.4,0.4,TEX_BTN_EXIT,0.01f);
		}
		else{
			glLoadName(5);
			DrawStartBtn(-2.7,2.2,false,0.4,0.4,TEX_RESTART,0.5f);
			glLoadName(6);
			DrawStartBtn(-2.7,0.4,false,0.4,0.4,TEX_RETURN,0.5f);
		}

		// 恢复之前的投影矩阵
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glFlush ();	// 立即执行指令
		
		GLint hits = glRenderMode (GL_RENDER);	// 返回常规模式，返回值为命中次数
		// 处理命中记录
		if(hits > 0){
			GLuint *ptr = (GLuint *) selectBuf;	// 初始化指针指向buffer第一个值
			ptr += 3;							// 跳过命中对象数目和两个深度值，指向名称项
			GLuint names = *ptr;	
			if(names>=1&&names<=6 &&state == GLUT_DOWN)
				Buttons[names-1]=true;
			glutPostRedisplay();
		}
		if(state == GLUT_UP)
		{
			if(Buttons[0] == true)
			{
				mciSendString("stop sounds/menubg.mp3", NULL, 0, NULL); 
				mciSendString("play sounds/gamemode1bg.mp3 repeat",NULL,0,NULL);
				game_state = GAME_MODE_1;
				lefttime = 60;			
			}
			if(Buttons[1] == true)
				;
			if(Buttons[2] == true)
			{
				if(CloseMusic)			//开启
				{
					mciSendString("play  sounds/menubg.mp3 repeat",NULL,0,NULL);
				}
				else
				{
					mciSendString("pause  sounds/menubg.mp3",NULL,0,NULL);
				}
				CloseMusic = !CloseMusic;
			}
			if(Buttons[3] == true)
				exit(0);
			if(game_state == GAME_OVER && Buttons[4] == true)
				GameReset();
			if(game_state == GAME_OVER && Buttons[5] == true)
			{
				GameReset();
				mciSendString("stop  sounds/gamemode1bg.mp3",NULL,0,NULL);
				mciSendString("play  sounds/menubg.mp3 repeat",NULL,0,NULL);
				game_state = GAME_MENU;

			}
			memset(Buttons,0,sizeof(Buttons));
			glutPostRedisplay();
		}
	}

	if(mouse_l_btn_down == false)
	{
		plainlen = 0;
		if(game_state == GAME_MODE_1)
		{
			//判断
			int temp1 ,temp2;
			JudgeClose(temp1,temp2);
			if(temp1 != -1 && temp2 != -1)
			{
				JudgeShape(temp1,temp2);
			}
		}
		//消亡
		MyPoint *pointrtemp;
		for(int i =1;i<=PlantLineNum;i++)
		{
			pointrtemp= new MyPoint(PlantLine[i].x,PlantLine[i].y,0.02 - 0.05/(rand()%20+1),0.05/(rand()%20+1),POINTDISTIME,0.0,-0.003f);
			pointsvec.push_back(pointrtemp);
		}
		PlantLineNum = 0;
	}
}

void glutMouseMove(int x,int y)
{ 
	if(!mouse_l_btn_down)
		return ;
	if(PlantLineNum>MAX_PlantLineNum-1)
		return ;

	if (plainlen > MAX_PLAINLEN && (game_state == GAME_MODE_1 || game_state == GAME_MODE_2))
		return ;
	printf("x:%d y:%d\n",x,y);
	printf("x:%lf y:%lf\n\n",(x-GL_WIN_WIDTH/2)/100.00,(GL_WIN_HEIGHT/2 - y)/100.00);

	PlantLine[PlantLineNum+1].x = (x-GL_WIN_WIDTH/2)/100.00;
	PlantLine[PlantLineNum+1].y =(GL_WIN_HEIGHT/2 - y)/100.00;
	plainlen += PlantLine[PlantLineNum].Dis_PointToPoint(PlantLine[PlantLineNum+1]);
	PlantLineNum++;
	glutPostRedisplay();

}

void glutTimer(int value)
{
	if(game_state == GAME_MODE_1||game_state == GAME_OVER)
		RunGameRunGameMode1();
	UpdateAllPoint();
	glutTimerFunc(1000/60.0,glutTimer,0);
	glutPostRedisplay();
}


/************************************************************************/
/* 下面是主函数			                                                */
/************************************************************************/
int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	//分辨率
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight= GetSystemMetrics(SM_CYSCREEN);
	//计算窗口位置
	int xWinPos = (screenWidth - GL_WIN_WIDTH)/2;
	int yWinPos = (screenHeight - GL_WIN_HEIGHT)/2;
	glutInitWindowPosition(xWinPos,yWinPos);
	glutInitWindowSize(GL_WIN_WIDTH,GL_WIN_HEIGHT);
	glutCreateWindow("Our Game Our world");


	//回调函数
	glutDisplayFunc(glutDisplay);
	glutTimerFunc(100,glutTimer,0);
	glutReshapeFunc(glutResize);
	glutKeyboardFunc(glutKeyboard);
	glutSpecialFunc(glutSpecial);
	glutMouseFunc(glutMouse);
	glutMotionFunc(glutMouseMove);

	GameInit();

	glutMainLoop();
	return 0;
}