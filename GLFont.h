// GLFont.h: interface for the CGLFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLFONT_H__2AA0CDA8_62E0_42F4_9278_43D172FB3B77__INCLUDED_)
#define AFX_GLFONT_H__2AA0CDA8_62E0_42F4_9278_43D172FB3B77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>

// �ı����뷽ʽö�ٳ���
enum{ALIGN_LEFT, ALIGN_MIDDLE, ALIGN_RIGHT};

class CGLFont  
{
public:
	CGLFont();
	GLvoid	Init(const char* fontName = "Comic Sans MS");			// ��ʼ��������Ϊ�������ƣ���Ĭ��ֵ
	GLvoid	Print(GLenum align/*���뷽ʽ*/, const char *fmt, ...);	// �������,��ԭ��Ϊ�ο���
	GLfloat GetTextLength(const char * text);						// ��ȡ�ı��Ŀ��(�߼���λ)
	GLvoid	InitZh(wchar_t *text/*���õ��������ַ�*/, LPCWSTR zFontName = L"����"/*����*/);	// ��ʼ���������
	GLvoid	PrintZh(GLenum align/*���뷽ʽ*/, wchar_t *text);		// �������(����������ַ���textһ��Ҫ��textZh��)
	virtual ~CGLFont();
private:
	GLuint base;										// �ַ�����ʾ�б�Ļ�
	GLYPHMETRICSFLOAT gmf[256];							// �洢������Ϣ������
	GLYPHMETRICSFLOAT *gmfZh;							// �洢����������Ϣ������
	GLuint baseZh;										// ������ʾ�б�Ļ�
	GLuint numZh;										// �����ַ�����
	bool   deleteBase;									// �Ƿ���Ҫɾ����ͨ�ַ���ʾ�б�
	bool   deleteBaseZh;								// �Ƿ���Ҫɾ�������ַ���ʾ�б�
	wchar_t *textZh;									// �����ַ���
};

#endif // !defined(AFX_GLFONT_H__2AA0CDA8_62E0_42F4_9278_43D172FB3B77__INCLUDED_)
