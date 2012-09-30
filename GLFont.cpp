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
		glDeleteLists(base, 256);									// ɾ��256����ʾ�б�
	if(deleteBaseZh){
		glDeleteLists(baseZh, numZh);								// ɾ�������ַ���ʾ�б�
		delete []textZh;
		delete []gmfZh;
	}
}

// �����ʼ������
void CGLFont::Init(const char* fontName)
{
	if(deleteBase)
		glDeleteLists(base, 256);

	HFONT font;													// ������
	base = glGenLists(256);										// ����256��������δʹ�õ���ʾ�б�ID
	
	// ��������
	font = CreateFont(	-12,									// ����߶�
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
		fontName);												// ��������
	
	HWND hWnd = GetForegroundWindow();							// ��ȡ��ǰ���ھ��
	HDC hDC = GetWindowDC(hWnd);								// ��ȡ��ǰ����DC
	SelectObject(hDC, font);									// ѡ��������������
	
	// ����������ʾ�б�
	wglUseFontOutlines(	hDC,									// ��ǰ����DC
		0,														// ��ʼ�ַ�
		255,													// ������ʾ�б�ĸ���
		base,													// ��ʾ�б�Ļ�
		0.0f,													// Deviation From The True Outlines
		0.2f,													// ����z������
		WGL_FONT_POLYGONS,										// ʹ�ö���δ������壬����������
		gmf);													// ��������Ϣ����gmf

	deleteBase = true;
}

// �Զ���GL������庯��
GLvoid CGLFont::Print(GLenum align, const char *fmt, ...)		
{
	float length = 0;											// ��ѯ�ַ����ĳ���
	char text[256];												// ����������Ҫ�����ִ�
	va_list ap;													// ָ��һ�������б��ָ��
	
	if (fmt == NULL)											// ����������򷵻�
		return;												
	
	va_start(ap, fmt);											// ��ȡ�ɱ������ַ
	vsprintf(text, fmt, ap);									// �Ѳ���ֵд���ַ���
	va_end(ap);													// ��ap��ֵΪ��
	
	// �����ַ�������ȡ�����ַ����ĳ���(���Ƴ���������귶Χ)
	for (unsigned int loop = 0; loop < (strlen(text)); loop++){	
		// ����һ���ַ��Ŀ��
		length += gmf[text[loop]].gmfCellIncX;
	}
	
	glPushMatrix();
	switch(align){
	case ALIGN_LEFT:
		// Ĭ������룬�������κα任
		break;
	case ALIGN_MIDDLE:
		glTranslatef(-length / 2, 0.0f, 0.0f);					// ���ı�����
		break;
	case ALIGN_RIGHT:
		glTranslatef(-length, 0.0f, 0.0f);						// ���ı��Ҷ���
		break;
	}
	
	glPushAttrib(GL_LIST_BIT);									// ����ʾ�б�����ѹ�����Զ�ջ
	glListBase(base);											// ������ʾ�б�Ļ�Ϊbase
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			// ������ʾ�б�����ַ���
	glPopAttrib();												// �������Զ�ջ

	glPopMatrix();
}

// ��ȡ�ַ�������ʾ����
GLfloat CGLFont::GetTextLength(const char * text)
{
	float length = 0;
	for (unsigned int loop = 0; loop < (strlen(text)); loop++){	// ���������ַ����ĳ���
		// ����һ���ַ��Ŀ��
		length += gmf[text[loop]].gmfCellIncX;
	}
	return length;
}

// ��ʼ���������
GLvoid CGLFont::InitZh(wchar_t *text/*Unicode�ַ���*/, LPCWSTR zFontName)
{
	if(deleteBaseZh)
		glDeleteLists(baseZh, numZh);

	HFONT font;													// ������
	numZh = wcslen(text);										// �ַ�����
	textZh = new wchar_t[numZh + 1];							// ����ռ�
	wcscpy(textZh, text);										// �����ַ���

	gmfZh = new GLYPHMETRICSFLOAT[numZh];

	baseZh = glGenLists(numZh);
	
	font = CreateFontW(	-12,									// ����߶�
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
		zFontName);												// ��������
	
	HWND hWnd = GetForegroundWindow();							// ��ȡ��ǰ���ھ��
	HDC hDC = GetWindowDC(hWnd);								// ��ȡ��ǰ����DC
	SelectObject(hDC, font);									// ѡ��������������
	
	for(int i = 0; i < numZh; i++){
		// ����������ʾ�б�
		wglUseFontOutlinesW(	hDC,								// ��ǰ����DC
			text[i],												// ��ʼ�ַ�
			1,														// ������ʾ�б�ĸ���
			baseZh+i,												// ��ʾ�б�Ļ�
			0.0f,													// Deviation From The True Outlines
			0.2f,													// ����z������
			WGL_FONT_POLYGONS,										// ʹ�ö���δ������壬����������
			&gmfZh[i]);													// ��������Ϣ����gmf
	}
	deleteBaseZh = true;
}

// ���ַ���s�в����ַ���t
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

// ��������ַ�
GLvoid CGLFont::PrintZh(GLenum align/*���뷽ʽ*/, wchar_t *text)
{
	// ��textZh�в���text(������ʼλ��)
	int start = find(textZh, text);	
	// δ�ҵ��򷵻�
	if(start == -1)					
		return;
	int length = wcslen(text);
	float lengthOut = 0;							// ��ѯ�ַ������������

	// �����ַ�������ȡ�����ַ����ĳ���(���Ƴ���������귶Χ)
	for (unsigned int loop = 0; loop < length; loop++){	
		// ����һ���ַ��Ŀ��
		lengthOut += gmfZh[start+loop].gmfCellIncX;
	}
	
	glPushMatrix();
	switch(align){
	case ALIGN_LEFT:
		// Ĭ������룬�������κα任
		break;
	case ALIGN_MIDDLE:
		glTranslatef(-lengthOut / 2, 0.0f, 0.0f);					// ���ı�����
		break;
	case ALIGN_RIGHT:
		glTranslatef(-lengthOut, 0.0f, 0.0f);						// ���ı��Ҷ���
		break;
	}

 	for(int i = 0; i < length; i++){
 		glCallList(baseZh + start + i);
 	}

	glPopMatrix();
}