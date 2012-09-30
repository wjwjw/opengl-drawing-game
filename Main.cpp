#include "Head.h"
#include "geometry.h"
#include <vector>
#include "GLFont.h"
#include "3ds.h"
#include "Number.h"
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

/************************************************************************/
/* �����ǳ���������                                                     */
/************************************************************************/
const int GL_WIN_WIDTH   = 800; //��Ϸ���
const int GL_WIN_HEIGHT  = 600; //��Ϸ�߶�

const float BACKGROUND_WIDTH  = 8;	 // �������
const float BACKGROUND_HEIGHT = 6;  // �����߶�
GLfloat halfWeight = BACKGROUND_WIDTH/2.0 ;
GLfloat halfHeight = BACKGROUND_HEIGHT/2.0;
const int MAX_PlantLineNum = 500;	//���ɻ�����
const int POINTDISTIME = 80;		//��������ʱ��

int level = 1;						//��Ϸ�ؿ�


const float MAX_PLAINLEN = 12;

//��Ϸ���ء�����˵���������Ϸ
enum { GAME_LOAD,GAME_MENU , GAME_MODE_1 , GAME_MODE_2 , GAME_SET,GAME_PAUSE,GAME_OVER};
const int BtnNums = 6;				//��ť����
bool Buttons[BtnNums] = {0};		//��ť����
const int TEXTURE_NUMS=25;			//����ͼ����
GLuint TextrueNames[TEXTURE_NUMS];	//��������
wchar_t *const MYTEXT = L"�����";
char *TextruefileNames[]={			//����·��
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
const int picNums = 10;							//����ͼƬ��
const int keyScore[] = {100,200,500,1000,3000,5000,10000,30000,50000,99999};	//����
const int picIdx[] = {11,12,13,14,15,16,17,18,19,20};
const int DSMODEL = 5;
CNumber my3ds[DSMODEL];				//3Dģ�ͼ���
/************************************************************************/
/* ȫ�ֱ���������                                         */
/************************************************************************/
bool mouse_l_btn_down = false;
bool mouse_r_btn_down = false;
bool Ispause[picNums];					//�Ƿ��Ѿ����ֹ�ͼƬ
bool isless10;							//�Ƿ�С��10��
bool CloseMusic =false;					//Ĭ�Ͽ�����Ч
int game_state = GAME_MENU;
MyPoint PlantLine[MAX_PlantLineNum * 5]; // Ϊ�˺��淽����㣬��1��ʼ���
POINT a;
int PlantLineNum = 0;
std::vector < MyPoint * > pointsvec;
std::vector < MyPoint * > deletepointvec;


GLint lefttime = 60;   //��Ϸʱ��
GLint lefttime_litle = 0;
GLuint score = 0;			//��ʵ����
GLuint nowscore = 0;		//��ʾ����
CGLFont myFont;
int scoreLevel[4] = {0,1,1,1}; //��Ϸ�÷ֵı��� 1 : �ı��� ��2 : ������ 3 : Բ
int Mag_dely = 5;					//5�����һ�α���
float Load_Roatf = 0.0;
float plainlen;//���ĳ���
int selectEffect = 0 ;		//ѡ�е���Ч
HGLRC hRC;
HDC hDC ;
/************************************************************************/
/* �����Ǹ��ֻ��ƺ�����                                  */
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
	//���Ʊ�������
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

	//���Ʊ�������
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
// 	printf("��ô����� �� %d\n",PlantLineNum);
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
	//���ƾ���
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
		myFont.PrintZh(ALIGN_LEFT,L"��");
		glScalef(0.65,0.65,0.65);
		glPushMatrix();
		{
			glTranslatef(-0.1,-0.0,0.0f);
			myFont.Print(ALIGN_LEFT,"    X%d",scoreLevel[1]);
		}
		glPopMatrix();
		glTranslatef(-0.1,-0.8,0.0f);
		myFont.PrintZh(ALIGN_LEFT,L"��");
		myFont.Print(ALIGN_LEFT,"    X%d",scoreLevel[2]);
		glTranslatef(0.0,-0.8,0.0f);
		myFont.PrintZh(ALIGN_LEFT,L"��");
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
		//��������
		glPushMatrix();
		glTranslatef(-1.5f,2.5f,0.0f);
		DrawDigits();
		glPopMatrix();

		//���ƽ���ƿ
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
/* �����Ǹ����߼�������                                                 */
/************************************************************************/
GLvoid InitZh(wchar_t *text, LPCWSTR zFontName =L"����");
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
	glEnable(GL_DEPTH_TEST);//��ʼ������������ID
	//���ù̶���͸��ͶӰ������
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glClearColor(0.378, 0.563, 0.626 ,1.0);
	//gluOrtho2D(-BACKGROUND_WIDTH / 2, BACKGROUND_WIDTH / 2, -BACKGROUND_HEIGHT / 2, BACKGROUND_HEIGHT / 2);
	glOrtho(-BACKGROUND_WIDTH / 2, BACKGROUND_WIDTH / 2, -BACKGROUND_HEIGHT / 2, BACKGROUND_HEIGHT / 2,-10.0f,10.0f);
	game_state = GAME_LOAD;

	// ���������û��(ʵ�ְ�͸��Ч��)
	glEnable(GL_BLEND);											// �������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			// ���û��ģʽ
	
	for(int i=0;i<TEXTURE_NUMS;i++)
		BuildTexture(TextruefileNames[i],TextrueNames[i]);
	myFont.Init();
	myFont.InitZh(MYTEXT,L"����");
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
					MyPoint *pointrtemp;//ģ������ʱ
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
				MyPoint *pointrtemp;//ģ������ʱ
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
	//������Լ1/10�Ĵ�С,����֮�����������λ��
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
	int num_cross = 0; // ����¼ӵı�������¼�ж��ٸ��ཻ�㣬̫���ཻ�㣬���ǲ��Ϸ���ͼ��
	//���ж�ͼ���ǲ��ǽ��Ʊպϵ�,��ͼ�εĿ�ʼ��ͽ�β����΢�ӳ�һ��
	/*  ��x^2 + ��y^2 = k^2
		��y / ��x = t
		���� k���ӳ��Ľ��Ƶĳ��ȣ�t = (y1 - y2)/(x1 - x2) Ϊб��
		���: (t^2+1)��x^2 = k^2
	*/
	//k��ֵҪ���ձ�����ȷ��
	float k = Cal_k() * 3;
	if(k < 0.3)   k = 0.3;
	printf("k = %lf \n",k);//����ר��
	if(PlantLineNum > 1)
	{
		//���ǿ�ʼ����ӳ�
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

		//ͬ���ӳ�������
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

		//Ȼ���ٿ�ʼ�жϷ���һ���ж������߶�֮����û���㽶
		/*printf("0 :  %lf \t %lf\n",PlantLine[0].x,PlantLine[0].y);
		printf("1 :  %lf \t %lf\n",PlantLine[1].x,PlantLine[1].y);
		printf("2 :  %lf \t %lf\n",PlantLine[2].x,PlantLine[2].y);
		printf("β2 �� %lf \t %lf\n",PlantLine[PlantLineNum-1].x,PlantLine[PlantLineNum-1].y,1);
		printf("β1 �� %lf \t %lf\n",PlantLine[PlantLineNum].x,PlantLine[PlantLineNum].y);
		printf("β0 �� %lf \t %lf\n",PlantLine[PlantLineNum+1].x,PlantLine[PlantLineNum+1].y,1);*/
		for(i=0;i<=PlantLineNum;i++)
		{
			for(j=0;j<=PlantLineNum;j++)
			{
				if( abs(i-j) < 6 ) break;//����ܽ��ĵ㣬�����ཻ��������
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
					/*/�������ר��
					printf("%d �� %d ���㽶\n",i,j);
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
					//�������ר��*/
				}
			}
		}
		//��������ҵ����ཻ�㣬�Ͳ��÷�������
		//Ȼ���ٿ�ʼ�жϷ�������1�����жϵ�һ����߶εľ���
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
					/*/�������ר��
					printf("1���<%d,%d>����ƻ��\n",i-1,i);
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
					//�������ר��*/
				}
			}

			//Ȼ���ٿ�ʼ�жϷ�������2�����ж����һ����߶εľ���
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
					/*/�������ר��
					printf("���һ���<%d,%d>����ƻ��\n",i-1,i);
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
					//�������ר��*/
				}
			}
		}
	}
	else
	{
		printf("�����㶼û��Ҳ����ң�!!\n");
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
	//������������ͻ����
	int cross[1000];           //ͻ�����±�
	MyPoint crossPoint[1000]; //���ˣ���һ����������¼ͻ��㣬�����õ�
	int index = 1;
	int cross_len = 6;//ͻ������
	int lase_cross = 0;
	cross[0] = s;
	printf("���濪ʼ��Ƕȣ�%d %d\n",s,t);
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
			//	//�������ר�� : �����Ǹ���
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
			//	//�������ר��*/

			//	printf("!%012.3lf!  ",temp);
			//}
			//else
			if(temp <= 160)
			{
				/*/�������ר�� : �����Ǹ���
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
				//�������ר��*/

				
				//printf("%d\n",lase_cross);
				//if(i - lase_cross > cross_len && t - cross_len > i)
				if(i - lase_cross > cross_len && t - cross_len > i)
				{
					cross[index] = i;
					index++;
				}
				lase_cross = i;
				printf("<%012.3lf>  ",temp);//����ר��
			}
			else
			{
				printf("%14.3lf  ",temp);//����ר��
			}
		}
		else
			printf("\t   INF  ");
	}
	printf("\n");//����ר��
	printf("����180����� �� %d / %d -- %.4lf\n",sum3,t-s+1,(double(sum3) / (t-s+1)));//����ר��
	printf("����175����� �� %d / %d -- %.4lf\n",sum1,t-s+1,(double(sum1) / (t-s+1)));//����ר��
	printf("160-175����� �� %d / %d -- %.4lf\n",sum2,t-s+1,(double(sum2) / (t-s+1)));//����ר��


	if(index > 6)
		printf("����������ʲôͼ�ΰ�\n");

	printf("ͻ����� �� %d ����ô���ͻ��� %d\n",cross_len,index);
	for (i = 0; i < index ; i++)
	{
		crossPoint[i] = PlantLine[cross[i]];
	}
	
	if( ( (double(sum1) / (t-s))  < 0.45 && (double(sum2) / (t-s)) > 0.45) || index >= 6)//��ʱ�����������Ϊ��һ��Բ
	{
		printf("Բ : \n");
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
		/*/�������ר�� �� ��һ��Բ
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
		//�������ר�� */
		PushCricle(O_mid.x,O_mid.y,O_r);
		JudgeCricleScore(O_mid.x,O_mid.y,O_r);
	}
	else
	{
		/*/�������ר�� �� ����ͻ���
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
		//�������ר�� */

		//������ʱ������
		SortPoint(crossPoint,index);
		//����ר�� ������
		for(int i  = 0 ;i < index ; i++)
		{
			printf(" %d  %.4lf %.4lf\n",i,crossPoint[i].x,crossPoint[i].y);
		}
		//����ר��*/
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

	//����Ч����
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
	//����
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
	//���²����Ƶ�
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
/* �߳���				                                                */
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
	//����ģ��
	if(!judging)
		for(int i=0;i<DSMODEL;i++)
		{
			if(my3ds[i].distime==0)
				my3ds[i].Update();
		}
		//����ʣ��ʱ��
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
/* �����Ǹ��ֻص�������                                                 */
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

	//���Ƶ�
	DrawMovingPoint();

	glutSwapBuffers();
}

/*���ڵ����ص�����*/
void glutResize(int width,int height)
{
	glutReshapeWindow(GL_WIN_WIDTH,GL_WIN_HEIGHT);
}

/*��ͨ�����ص�����*/
void glutKeyboard(unsigned char key/*ASCII��*/,int x,int y)
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
			if(CloseMusic)			//����
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
			if(CloseMusic)			//����
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
	/*�ȴ����*/
}

/*���ⰴ���ص�����*/
void glutSpecial(int value,int x,int y)
{
	/*�ȴ����*/
}

/*���ص�����*/
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

		GLuint selectBuf[100];		// ���ƻ��棬����ʰȡ
		GLint viewport[4];			// ���ڴ洢�ӿ�����
		glGetIntegerv (GL_VIEWPORT, viewport);	// ��ȡ�ӿ�����(����gluPickMatrixҪ��)
		glSelectBuffer (100, selectBuf);		// �������ƻ���
		glRenderMode(GL_SELECT);				// ����ѡ��ģʽ
		glInitNames();							// ��ʼ�����ƶ�ջ
		glPushName(0);							// ��0ѹ���ջ

		// ����ʰȡ���õĹ۲�����
		glMatrixMode(GL_PROJECTION);			// ѡ��ͶӰ����Ϊ��ǰ����
		glPushMatrix();							// ���浱ǰ����(ѹ��ջ)
		glLoadIdentity();						// ����ͶӰ����Ϊ��Ⱦ���

		// �����λ�ø���3��3��������Ϊʰȡ����
		// ע��gluPickMatrix��ǰ��������ΪͶӰ�������꣬���õ��Ǵ�������ϵ(ԭ���ڴ������½ǣ�
		// x���ң�y���ϣ�����glViewport����ϵ��ͬ��ע�Ⲣ���豸����ϵ�����Ҫ��תyֵ)
		gluPickMatrix ((GLdouble)x, (GLdouble)(viewport[3] - y),
			3.0, 3.0, viewport);
		// ���������������	
		glOrtho(-BACKGROUND_WIDTH / 2, BACKGROUND_WIDTH / 2, -BACKGROUND_HEIGHT / 2,
			BACKGROUND_HEIGHT / 2,-10,10);

		// �����ʰȡĿ����ƴ���
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

		// �ָ�֮ǰ��ͶӰ����
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glFlush ();	// ����ִ��ָ��
		
		GLint hits = glRenderMode (GL_RENDER);	// ���س���ģʽ������ֵΪ���д���
		// �������м�¼
		if(hits > 0){
			GLuint *ptr = (GLuint *) selectBuf;	// ��ʼ��ָ��ָ��buffer��һ��ֵ
			ptr += 3;							// �������ж�����Ŀ���������ֵ��ָ��������
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
				if(CloseMusic)			//����
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
			//�ж�
			int temp1 ,temp2;
			JudgeClose(temp1,temp2);
			if(temp1 != -1 && temp2 != -1)
			{
				JudgeShape(temp1,temp2);
			}
		}
		//����
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
/* ������������			                                                */
/************************************************************************/
int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	//�ֱ���
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight= GetSystemMetrics(SM_CYSCREEN);
	//���㴰��λ��
	int xWinPos = (screenWidth - GL_WIN_WIDTH)/2;
	int yWinPos = (screenHeight - GL_WIN_HEIGHT)/2;
	glutInitWindowPosition(xWinPos,yWinPos);
	glutInitWindowSize(GL_WIN_WIDTH,GL_WIN_HEIGHT);
	glutCreateWindow("Our Game Our world");


	//�ص�����
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