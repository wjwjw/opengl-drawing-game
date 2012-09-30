// GLFont.cpp: implementation of the CGLFont class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <gl/glut.h>
#include "GLFont.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLFont::CGLFont()
{
	deleteBase = false;
	deleteBaseZh = false;
}

CGLFont::~CGLFont()
{
	if(deleteBase)
		glDeleteLists(base, 256);									// 删除256个显示列表
	if(deleteBaseZh){
		glDeleteLists(baseZh, numZh);								// 删除中文字符显示列表
		delete []textZh;
		delete []gmfZh;
	}
}

// 字体初始化函数
void CGLFont::Init(const char* fontName)
{
	if(deleteBase)
		glDeleteLists(base, 256);

	HFONT font;													// 字体句柄
	base = glGenLists(256);										// 申请256个连续的未使用的显示列表ID
	
	// 创建字体
	font = CreateFont(	-12,									// 字体高度
		0,														// Width Of Font
		0,														// Angle Of Escapement
		0,														// Orientation Angle
		FW_BOLD,												// Font Weight
		FALSE,													// Italic
		FALSE,													// Underline
		FALSE,													// Strikeout
		ANSI_CHARSET,											// Character Set Identifier
		OUT_TT_PRECIS,											// Output Precision
		CLIP_DEFAULT_PRECIS,									// Clipping Precision
		ANTIALIASED_QUALITY,									// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,							// Family And Pitch
		fontName);												// 字体名称
	
	HWND hWnd = GetForegroundWindow();							// 获取当前窗口句柄
	HDC hDC = GetWindowDC(hWnd);								// 获取当前窗口DC
	SelectObject(hDC, font);									// 选用所创建的字体
	
	// 创建字体显示列表
	wglUseFontOutlines(	hDC,									// 当前窗口DC
		0,														// 起始字符
		255,													// 创建显示列表的个数
		base,													// 显示列表的基
		0.0f,													// Deviation From The True Outlines
		0.2f,													// 文字z方向厚度
		WGL_FONT_POLYGONS,										// 使用多边形创建字体，而不是线条
		gmf);													// 将字体信息放入gmf

	deleteBase = true;
}

// 自定义GL输出字体函数
GLvoid CGLFont::Print(GLenum align, const char *fmt, ...)		
{
	float length = 0;											// 查询字符串的长度
	char text[256];												// 保存我们想要的文字串
	va_list ap;													// 指向一个变量列表的指针
	
	if (fmt == NULL)											// 如果无输入则返回
		return;												
	
	va_start(ap, fmt);											// 获取可变参数地址
	vsprintf(text, fmt, ap);									// 把参数值写入字符串
	va_end(ap);													// 将ap赋值为空
	
	// 遍历字符串，获取整个字符串的长度(绘制出来后的坐标范围)
	for (unsigned int loop = 0; loop < (strlen(text)); loop++){	
		// 增加一个字符的宽度
		length += gmf[text[loop]].gmfCellIncX;
	}
	
	glPushMatrix();
	switch(align){
	case ALIGN_LEFT:
		// 默认左对齐，无需做任何变换
		break;
	case ALIGN_MIDDLE:
		glTranslatef(-length / 2, 0.0f, 0.0f);					// 把文本居中
		break;
	case ALIGN_RIGHT:
		glTranslatef(-length, 0.0f, 0.0f);						// 把文本右对齐
		break;
	}
	
	glPushAttrib(GL_LIST_BIT);									// 把显示列表属性压入属性堆栈
	glListBase(base);											// 设置显示列表的基为base
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			// 调用显示列表绘制字符串
	glPopAttrib();												// 弹出属性堆栈

	glPopMatrix();
}

// 获取字符串的显示长度
GLfloat CGLFont::GetTextLength(const char * text)
{
	float length = 0;
	for (unsigned int loop = 0; loop < (strlen(text)); loop++){	// 查找整个字符串的长度
		// 增加一个字符的宽度
		length += gmf[text[loop]].gmfCellIncX;
	}
	return length;
}

// 初始化中文输出
GLvoid CGLFont::InitZh(wchar_t *text/*Unicode字符串*/, LPCWSTR zFontName)
{
	if(deleteBaseZh)
		glDeleteLists(baseZh, numZh);

	HFONT font;													// 字体句柄
	numZh = wcslen(text);										// 字符个数
	textZh = new wchar_t[numZh + 1];							// 分配空间
	wcscpy(textZh, text);										// 拷贝字符串

	gmfZh = new GLYPHMETRICSFLOAT[numZh];

	baseZh = glGenLists(numZh);
	
	font = CreateFontW(	-12,									// 字体高度
		0,														// Width Of Font
		0,														// Angle Of Escapement
		0,														// Orientation Angle
		FW_BOLD,												// Font Weight
		FALSE,													// Italic
		FALSE,													// Underline
		FALSE,													// Strikeout
		GB2312_CHARSET,											// Character Set Identifier
		OUT_TT_PRECIS,											// Output Precision
		CLIP_DEFAULT_PRECIS,									// Clipping Precision
		ANTIALIASED_QUALITY,									// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,							// Family And Pitch
		zFontName);												// 字体名称
	
	HWND hWnd = GetForegroundWindow();							// 获取当前窗口句柄
	HDC hDC = GetWindowDC(hWnd);								// 获取当前窗口DC
	SelectObject(hDC, font);									// 选用所创建的字体
	
	for(int i = 0; i < numZh; i++){
		// 创建字体显示列表
		wglUseFontOutlinesW(	hDC,								// 当前窗口DC
			text[i],												// 起始字符
			1,														// 创建显示列表的个数
			baseZh+i,												// 显示列表的基
			0.0f,													// Deviation From The True Outlines
			0.2f,													// 文字z方向厚度
			WGL_FONT_POLYGONS,										// 使用多边形创建字体，而不是线条
			&gmfZh[i]);													// 将字体信息放入gmf
	}
	deleteBaseZh = true;
}

// 在字符串s中查找字符串t
int find(wchar_t s[], wchar_t t[])
{
    int j = 0;
    for (int i = 0; s[i] != '\0'; i++)
    {
        if (t[0] == s[i])
        {
            while (t[j] != '\0' && s[i+j] != '\0')
            {
                if (t[j] != s[i+j])
                {
					j = 0;
                    break;
                }
				j++;
            }
        }
        if (t[j] == '\0')
        {
            return i;
        }
    }
    return -1;	
}

// 输出中文字符
GLvoid CGLFont::PrintZh(GLenum align/*对齐方式*/, wchar_t *text)
{
	// 在textZh中查找text(返回起始位置)
	int start = find(textZh, text);	
	// 未找到则返回
	if(start == -1)					
		return;
	int length = wcslen(text);
	float lengthOut = 0;							// 查询字符串的输出长度

	// 遍历字符串，获取整个字符串的长度(绘制出来后的坐标范围)
	for (unsigned int loop = 0; loop < length; loop++){	
		// 增加一个字符的宽度
		lengthOut += gmfZh[start+loop].gmfCellIncX;
	}
	
	glPushMatrix();
	switch(align){
	case ALIGN_LEFT:
		// 默认左对齐，无需做任何变换
		break;
	case ALIGN_MIDDLE:
		glTranslatef(-lengthOut / 2, 0.0f, 0.0f);					// 把文本居中
		break;
	case ALIGN_RIGHT:
		glTranslatef(-lengthOut, 0.0f, 0.0f);						// 把文本右对齐
		break;
	}

 	for(int i = 0; i < length; i++){
 		glCallList(baseZh + start + i);
 	}

	glPopMatrix();
}