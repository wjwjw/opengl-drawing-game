#include <windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <math.h>
#include <olectl.h>
#include <cstdio>
//#define min(a,b) (a)<(b)?(a):(b)
struct LightingStruct
{
	GLfloat ambient[4];		//环境光
	GLfloat diffuse[4];		//漫反射光
	GLfloat	specular[4];	//镜面反射光
};

//	材质
struct MaterialStruct
{
	GLfloat ambient[4];		//环境光
	GLfloat diffuse[4];		//漫反射光
	GLfloat	specular[4];	//镜面反射光
	GLfloat emission[4];	//发射光
	GLfloat	shininess;		//高光系数
};

LightingStruct whiteLight={
	{0.5,0.5,0.5,1.0},
	{1.0,1.0,1.0,1.0},
	{1.0,1.0,1.0,1.0}
};

LightingStruct dimLight = {
	{0.1,0.1,0.1,1.0},
	{0.1,0.1,0.1,1.0},
	{0.1,0.1,0.1,1.0}
};




void SetLighting(GLenum light,LightingStruct &lighting)
{
	//GLfloat pos[4]={1.0f,1.0f,1.0f,0.0f};
	glLightfv(light ,GL_AMBIENT ,lighting.ambient);
	glLightfv(light ,GL_DIFFUSE ,lighting.diffuse);
	glLightfv(light ,GL_SPECULAR ,lighting.specular);
	//glLightfv(light,GL_POSITION,pos);
}
int BuildTexture(char *szPathName/*纹理图像路径名*/, GLuint &texId/*纹理对象Id的引用*/)
{
	HDC			hdcTemp;								// The DC To Hold Our Bitmap
	HBITMAP		hbmpTemp;							// Holds The Bitmap Temporarily
	IPicture		*pPicture;								// IPicture Interface
	OLECHAR	wszPath[MAX_PATH+1];					// Full Path To Picture (WCHAR)
	char			szPath[MAX_PATH+1];					// Full Path To Picture
	long			lWidth;								// Width In Logical Units
	long			lHeight;								// Height In Logical Units
	long			lWidthPixels;							// Width In Pixels
	long			lHeightPixels;							// Height In Pixels
	GLint		glMaxTexDim ;							// Holds Maximum Texture Size

	if (strstr(szPathName, "http://"))							// If PathName Contains http:// Then...
	{
		strcpy(szPath, szPathName);							// Append The PathName To szPath
	}
	else											// Otherwise... We Are Loading From A File
	{
		GetCurrentDirectory(MAX_PATH, szPath);			// Get Our Working Directory
		strcat(szPath, "\\");							// Append "\" After The Working Directory
		strcat(szPath, szPathName);						// Append The PathName
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH); // Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))												// If Loading Failed
		return FALSE;											// Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));		// Create The Windows Compatible Device Context
	if(!hdcTemp)							// Did Creation Fail?
	{
		pPicture->Release();					// Decrements IPicture Reference Count
		return FALSE;						// Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);	 // 获取所支持的最大纹理尺寸

	pPicture->get_Width(&lWidth);						// Get IPicture Width (Convert To Pixels)
	lWidthPixels	= MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);						// Get IPicture Height (Convert To Pixels)
	lHeightPixels	= MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim) 		// Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else  			// Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim) 		// Is Image Height Greater Than Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else  			// Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;

	//	Create A Temporary Bitmap
	BITMAPINFO	bi = {0};							// The Type Of Bitmap We Request
	DWORD		*pBits = 0;							// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);	// Set Structure Size
	bi.bmiHeader.biBitCount		= 32;					// 32 Bit
	bi.bmiHeader.biWidth		= lWidthPixels;				// Power Of Two Width
	bi.bmiHeader.biHeight		= lHeightPixels;				// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression	= BI_RGB;				// RGB Encoding
	bi.bmiHeader.biPlanes		= 1;						// 1 Bitplane

	//	Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if(!hbmpTemp)									// Did Creation Fail?
	{
		DeleteDC(hdcTemp);							// Delete The Device Context
		pPicture->Release();							// Decrements IPicture Reference Count
		return FALSE;								// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);	// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)		// Loop Through All Of The Pixels
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);	// Grab The Current Pixel
		BYTE  temp		= pPixel[0];		// Store 1st Color In Temp Variable (Blue)
		pPixel[0]		= pPixel[2];			// Move Red Value To Correct Position (1st)
		pPixel[2]		= temp;				// Move Temp Value To Correct Blue Position (3rd)

		// This Will Make Any Black Pixels, Completely Transparent (You Can Hardcode The Value If You Wish)
		if ((pPixel[0]==0) && (pPixel[1]==0) && (pPixel[2]==0))		// Is Pixel Completely Black
			pPixel[3]	=   0;								// Set The Alpha Value To 0
		else												// Otherwise
			pPixel[3]	= 255;								// Set The Alpha Value To 255
	}

	glGenTextures(1, &texId);								// 生成纹理对象，将其ID存入texId

	// 根据位图中的数据创建纹理
	glBindTexture(GL_TEXTURE_2D, texId);								// 绑定纹理对象
	// 纹理图像缩小时的插值方式设为线性插值(可修改)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);		
	// 纹理图像放大时的插值方式设为线性插值(可修改)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);     
	// 创建纹理(如想用Mipmapping可修改此行)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);									// Delete The Object
	DeleteDC(hdcTemp);										// Delete The Device Context

	pPicture->Release();								// Decrements IPicture Reference Count

	return TRUE;									// Return True (All Good)
}

