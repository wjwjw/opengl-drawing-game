// GLFont.h: interface for the CGLFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLFONT_H__2AA0CDA8_62E0_42F4_9278_43D172FB3B77__INCLUDED_)
#define AFX_GLFONT_H__2AA0CDA8_62E0_42F4_9278_43D172FB3B77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>

// 文本对齐方式枚举常量
enum{ALIGN_LEFT, ALIGN_MIDDLE, ALIGN_RIGHT};

class CGLFont  
{
public:
	CGLFont();
	GLvoid	Init(const char* fontName = "Comic Sans MS");			// 初始化，参数为字体名称，有默认值
	GLvoid	Print(GLenum align/*对齐方式*/, const char *fmt, ...);	// 输出文字,以原点为参考点
	GLfloat GetTextLength(const char * text);						// 获取文本的宽度(逻辑单位)
	GLvoid	InitZh(wchar_t *text/*所用到的中文字符*/, LPCWSTR zFontName = L"宋体"/*字体*/);	// 初始化中文输出
	GLvoid	PrintZh(GLenum align/*对齐方式*/, wchar_t *text);		// 输出中文(输出的中文字符串text一定要在textZh中)
	virtual ~CGLFont();
private:
	GLuint base;										// 字符集显示列表的基
	GLYPHMETRICSFLOAT gmf[256];							// 存储字体信息的数组
	GLYPHMETRICSFLOAT *gmfZh;							// 存储中文字体信息的数组
	GLuint baseZh;										// 中文显示列表的基
	GLuint numZh;										// 中文字符个数
	bool   deleteBase;									// 是否需要删除普通字符显示列表
	bool   deleteBaseZh;								// 是否需要删除中文字符显示列表
	wchar_t *textZh;									// 中文字符串
};

#endif // !defined(AFX_GLFONT_H__2AA0CDA8_62E0_42F4_9278_43D172FB3B77__INCLUDED_)
