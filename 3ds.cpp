#include "3ds.h" 
#include <atlimage.h>
#include "CImage.h"
#include <GL/glut.h>

CLoad3DS::CLoad3DS()
{
	m_CurrentChunk = new tChunk;
	m_TempChunk = new tChunk;
	m_FilePointer=NULL;
}

CLoad3DS::~CLoad3DS()
{
	CleanUp();
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		delete [] g_3DModel.pObject[i].pFaces;
		delete [] g_3DModel.pObject[i].pNormals;
		delete [] g_3DModel.pObject[i].pVerts;
		delete [] g_3DModel.pObject[i].pTexVerts;
	}
}

////////////////////////////////////////////////////////////////////////
void CLoad3DS::Init(const char *filename)
{ 
	Import3DS(&g_3DModel, filename);  
	
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
			CreateTexture(g_Texture, g_3DModel.pMaterials[i].strFile, i);
		
		g_3DModel.pMaterials[i].texureId = i;
	}
}

void CLoad3DS::CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{ 	
	if(!strFileName) 
		return;     
	
	CImage *img = new CImage;							// 新建CImage对象
	HRESULT hResult = img->Load(strFileName);			// 读入图像文件
	if(hResult == E_FAIL){
		char error[255];
		sprintf(error, "%s不存在", strFileName);
		MessageBox(NULL, error, "错误", MB_OK|MB_ICONINFORMATION);
		exit(0);
	}

	int width = img->GetWidth();	// 获取图像宽度
	int height = img->GetHeight();	// 获取图像高度
	GLubyte *image = new GLubyte[width * height * 3];	// 用于保存图像数据的数组
	// 将图像数据读入image数组
	for(int j = 0; j < height; j++){
		for(int i = 0; i < width; i++){
			int index = (j * width + i) * 3;
			RGBQUAD rgb = GetPixelColor(img, i, j);
			image[index] = rgb.rgbRed;
			image[index+1] = rgb.rgbGreen;
			image[index+2] = rgb.rgbBlue;
		}
	}
	delete img;		// CImage对象已无用，数据已读入image数组
	
	glGenTextures(1, &textureArray[textureID]);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// 图像每行数据紧密排列
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	delete image;	// 纹理数据已在显卡纹理内存中了，主机内存中的image已无用
}

void CLoad3DS::show3ds()
{
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	
	GLboolean bTex = glIsEnabled(GL_TEXTURE_2D);

	//绘制模型
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		if(g_3DModel.pObject.size() <= 0) 
		{
			break;
		}
		
		t3DObject *pObject = &g_3DModel.pObject[i];
		
		if(pObject->bHasTexture)
		{
			//如果这个物体有纹理,绑定纹理
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} 
		else
		{
			//没有纹理,关闭贴图
			glDisable(GL_TEXTURE_2D);
		}
		//指定颜色
		glColor3ub(255, 255, 255);
		
		glBegin(GL_TRIANGLES);
		//绘制这个物体的所有面片
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			for(int tex = 0; tex < 3; tex++)    
			{
				int index = pObject->pFaces[j].vertIndex[tex]; 
				//设置法向
				glNormal3f(pObject->pNormals[index].x,
					pObject->pNormals[index].y,  
					pObject->pNormals[index].z); 
				
				if(pObject->bHasTexture)    
				{
					//设置纹理坐标
					if(pObject->pTexVerts)    
						glTexCoord2f(pObject->pTexVerts[index].u, pObject->pTexVerts[index].v);
				}
				else
				{
					//没有纹理,设置颜色
					if(g_3DModel.pMaterials.size() && pObject->materialID>= 0) 
					{
						BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;
						
						glColor3ub(pColor[0],pColor[1],pColor[2]);
					}
				}
				//绘制各顶点
				glVertex3f(pObject->pVerts[index].x,
					pObject->pVerts[index].y,
					pObject->pVerts[index].z);
			}
		}
		glEnd();
	}
	
	if(bTex)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glPopAttrib();
}

//////////////////////////////////////////////////////////////////
//加载模型
bool CLoad3DS::Import3DS(t3DModel *pModel, const char *strFileName)
{
	char strMessage[255] = {0};
	
	m_FilePointer = fopen(strFileName, "rb");
	
	if(!m_FilePointer) 
	{
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	ReadChunk(m_CurrentChunk);
	
	if (m_CurrentChunk->ID != PRIMARY)
	{ 
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	ReadNextChunk(pModel, m_CurrentChunk);
	
	ComputeNormals(pModel);
	
	return true;
}

void CLoad3DS::CleanUp()
{
	if(m_FilePointer!=NULL)
		fclose(m_FilePointer); 
	delete m_CurrentChunk; 
	delete m_TempChunk;  
}

void CLoad3DS::ReadNextChunk(t3DModel *pModel, tChunk *pPreChunk)
{
	t3DObject newObject = {0};		// 用来添加到对象链表
	tMatInfo newTexture = {0};		// 用来添加到材质链表
	unsigned int version = 0;		// 保存文件版本
	int buffer[50000] = {0};		// 用来跳过不需要的数据
	
	m_CurrentChunk = new tChunk;    // 为新的块分配空间  
	// 下面每读一个新块，都要判断一下块的ID，如果该块是需要的读入的，则继续进行
	// 如果是不需要读入的块，则略过
	// 继续读入子块，直到达到预定的长度
	while (pPreChunk->bytesRead < pPreChunk->length)
	{
		// 读入下一个块
		ReadChunk(m_CurrentChunk);
		// 判断块的ID号
		switch (m_CurrentChunk->ID)
		{
		case VERSION:       // 文件版本号
			// 在该块中有一个无符号短整型数保存了文件的版本
			// 读入文件的版本号，并将字节数添加到bytesRead变量中
			m_CurrentChunk->bytesRead += fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			// 如果文件版本号大于3，给出一个警告信息
			if (version > 0x03)
				MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			break;
			
		case OBJECTINFO:      // 网格版本信息
			// 读入下一个块
			ReadChunk(m_TempChunk);
			// 获得网格的版本号
			m_TempChunk->bytesRead += fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);
			// 增加读入的字节数
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;
			// 进入下一个块
			ReadNextChunk(pModel, m_CurrentChunk);
			break;
			
		case MATERIAL:       // 材质信息
			// 材质的数目递增
			pModel->numOfMaterials++;
			// 在纹理链表中添加一个空白纹理结构
			pModel->pMaterials.push_back(newTexture);
			// 进入材质装入函数
			ReadNextMatChunk(pModel, m_CurrentChunk);
			break;
			
		case OBJECT:       // 对象的名称
			// 该块是对象信息块的头部，保存了对象了名称
			// 对象数递增
			pModel->numOfObjects++;
			// 添加一个新的tObject节点到对象链表中
			pModel->pObject.push_back(newObject);
			// 初始化对象和它的所有数据成员
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));
			// 获得并保存对象的名称，然后增加读入的字节数
			m_CurrentChunk->bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);
			// 进入其余的对象信息的读入
			ReadNextObjChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), m_CurrentChunk);
			break;
			
		case EDITKEYFRAME:  
			// 跳过关键帧块的读入，增加需要读入的字节数
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
			
		default: 
			//  跳过所有忽略的块的内容的读入，增加需要读入的字节数
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		// 增加从最后块读入的字节数
		pPreChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	// 释放当前块的内存空间
	delete m_CurrentChunk;
	m_CurrentChunk = pPreChunk;
}

//  下面的函数处理所有的文件中对象的信息
void CLoad3DS::ReadNextObjChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreChunk)
{ 
	int buffer[50000] = {0};     // 用于读入不需要的数据
	// 对新的块分配存储空间
	m_CurrentChunk = new tChunk;
	
	// 继续读入块的内容直至本子块结束
	while (pPreChunk->bytesRead < pPreChunk->length)
	{ // 读入下一个块
		ReadChunk(m_CurrentChunk);
		// 区别读入是哪种块
		switch (m_CurrentChunk->ID)
		{
		case OBJ_MESH:     // 正读入的是一个新块
			// 使用递归函数调用，处理该新块
			ReadNextObjChunk(pModel, pObject, m_CurrentChunk);
			break;
		case OBJ_VERTICES:    // 读入是对象顶点
			ReadVertices(pObject, m_CurrentChunk);
			break;
		case OBJ_FACES:     // 读入的是对象的面
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;
		case OBJ_MATERIAL:    // 读入的是对象的材质名称
			// 该块保存了对象材质的名称，可能是一个颜色，也可能是一个纹理映射。同时在该块中也保存了
			// 纹理对象所赋予的面
			// 下面读入对象的材质名称
			ReadObjMat(pModel, pObject, m_CurrentChunk);   
			break;
		case OBJ_UV:      // 读入对象的UV纹理坐标
			// 读入对象的UV纹理坐标
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;
		default:  
			// 略过不需要读入的块
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		// 添加从最后块中读入的字节数到前面的读入的字节中
		pPreChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	// 释放当前块的内存空间，并把当前块设置为前面块
	delete m_CurrentChunk;
	m_CurrentChunk = pPreChunk;
}

//  下面的函数处理所有的材质信息
void CLoad3DS::ReadNextMatChunk(t3DModel *pModel, tChunk *pPreChunk)
{
	int buffer[50000] = {0};     // 用于读入不需要的数据
	// 给当前块分配存储空间
	m_CurrentChunk = new tChunk;
	// 继续读入这些块，知道该子块结束
	while (pPreChunk->bytesRead < pPreChunk->length)
	{ // 读入下一块
		ReadChunk(m_CurrentChunk);
		// 判断读入的是什么块
		switch (m_CurrentChunk->ID)
		{
		case MATNAME:       // 材质的名称
			// 读入材质的名称
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
			
		case MATDIFFUSE:      // 对象的R G B颜色
			ReadColor(&(pModel->pMaterials[pModel->numOfMaterials - 1]), m_CurrentChunk);
			break;
			
		case MATMAP:       // 纹理信息的头部
			// 进入下一个材质块信息
			ReadNextMatChunk(pModel, m_CurrentChunk);
			break;
			
		case MATMAPFILE:      // 材质文件的名称
			// 读入材质的文件名称
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
			
		default:  
			// 掠过不需要读入的块
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		// 添加从最后块中读入的字节数
		pPreChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	// 删除当前块，并将当前块设置为前面的块
	delete m_CurrentChunk;
	m_CurrentChunk = pPreChunk;
}

//  下面函数读入块的ID号和它的字节长度
void CLoad3DS::ReadChunk(tChunk *pChunk)
{ 
	// 读入块的ID号，占用了2个字节。块的ID号象OBJECT或MATERIAL一样，说明了在块中所包含的内容
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_FilePointer);
	// 然后读入块占用的长度，包含了四个字节
	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_FilePointer);
}

//  下面的函数读入一个字符串
int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;
	// 读入一个字节的数据
	fread(pBuffer, 1, 1, m_FilePointer);
	// 直到结束
	while (*(pBuffer + index++) != 0) {
		// 读入一个字符直到NULL
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}
	// 返回字符串的长度
	return strlen(pBuffer) + 1;
}

//  下面的函数读入RGB颜色
void CLoad3DS::ReadColor(tMatInfo *pMaterial, tChunk *pChunk)
{
	// 读入颜色块信息
	ReadChunk(m_TempChunk);
	// 读入RGB颜色
	m_TempChunk->bytesRead += fread(pMaterial->color, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);
	// 增加读入的字节数
	pChunk->bytesRead += m_TempChunk->bytesRead;
}

//  下面的函数读入顶点索引
void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreChunk)
{
	unsigned short index = 0;     // 用于读入当前面的索引
	// 读入该对象中面的数目
	pPreChunk->bytesRead += fread(&pObject->numOfFaces, 1, 2, m_FilePointer);
	// 分配所有面的存储空间，并初始化结构
	pObject->pFaces = new tFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);
	// 遍历对象中所有的面
	for(int i = 0; i < pObject->numOfFaces; i++)
	{ for(int j = 0; j < 4; j++)
	{ // 读入当前面的第一个点 
		pPreChunk->bytesRead += fread(&index, 1, sizeof(index), m_FilePointer);
		if(j < 3)
		{ // 将索引保存在面的结构中
			pObject->pFaces[i].vertIndex[j] = index;
		}
	}
	}
}

//  下面的函数读入对象的UV坐标
void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreChunk)
{
	// 为了读入对象的UV坐标，首先需要读入UV坐标的数量，然后才读入具体的数据
	// 读入UV坐标的数量
	pPreChunk->bytesRead += fread(&pObject->numTexVertex, 1, 2, m_FilePointer);
	// 分配保存UV坐标的内存空间
	pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
	// 读入纹理坐标
	pPreChunk->bytesRead += fread(pObject->pTexVerts, 1, pPreChunk->length - pPreChunk->bytesRead, m_FilePointer);
}

//  读入对象的顶点
void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreChunk)
{
	// 在读入实际的顶点之前，首先必须确定需要读入多少个顶点。
	// 读入顶点的数目
	pPreChunk->bytesRead += fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);
	// 分配顶点的存储空间，然后初始化结构体
	pObject->pVerts = new CVector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);
	// 读入顶点序列
	pPreChunk->bytesRead += fread(pObject->pVerts, 1, pPreChunk->length - pPreChunk->bytesRead, m_FilePointer);
	// 现在已经读入了所有的顶点。
	// 因为3D Studio Max的模型的Z轴是指向上的，因此需要将y轴和z轴翻转过来。
	// 具体的做法是将Y轴和Z轴交换，然后将Z轴反向。
	// 遍历所有的顶点
	for(int i = 0; i < pObject->numOfVerts; i++)
	{ // 保存Y轴的值
		float fTempY = pObject->pVerts[i].y;
		// 设置Y轴的值等于Z轴的值
		pObject->pVerts[i].y = pObject->pVerts[i].z;
		// 设置Z轴的值等于-Y轴的值 
		pObject->pVerts[i].z = -fTempY;
	}
}

//  下面的函数读入对象的材质名称
void CLoad3DS::ReadObjMat(t3DModel *pModel, t3DObject *pObject, tChunk *pPreChunk)
{
	char strMaterial[255] = {0};   // 用来保存对象的材质名称
	int buffer[50000] = {0};    // 用来读入不需要的数据
	// 材质或者是颜色，或者是对象的纹理，也可能保存了象明亮度、发光度等信息。
	// 下面读入赋予当前对象的材质名称
	pPreChunk->bytesRead += GetString(strMaterial);
	// 遍历所有的纹理
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{ //如果读入的纹理与当前的纹理名称匹配
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{ // 设置材质ID
			pObject->materialID = i;
			// 判断是否是纹理映射，如果strFile是一个长度大于1的字符串，则是纹理
			if(strlen(pModel->pMaterials[i].strFile) > 0) {
				// 设置对象的纹理映射标志
				pObject->bHasTexture = true;
			} 
			break;
		}
		else
		{ // 如果该对象没有材质，则设置ID为-1
			pObject->materialID = -1;
		}
	}
	pPreChunk->bytesRead += fread(buffer, 1, pPreChunk->length - pPreChunk->bytesRead, m_FilePointer);
}   
//  下面的这些函数主要用来计算顶点的法向量，顶点的法向量主要用来计算光照
// 下面的宏定义计算一个矢量的长度
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))
// 下面的函数求两点决定的矢量
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{ 
	CVector3 vVector;       
	vVector.x = vPoint1.x - vPoint2.x;   
	vVector.y = vPoint1.y - vPoint2.y;   
	vVector.z = vPoint1.z - vPoint2.z;   
	return vVector;        
}
// 下面的函数两个矢量相加
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{ 
	CVector3 vResult;       
	vResult.x = vVector2.x + vVector1.x;  
	vResult.y = vVector2.y + vVector1.y;  
	vResult.z = vVector2.z + vVector1.z;  
	return vResult;        
}
// 下面的函数处理矢量的缩放
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{ 
	CVector3 vResult;       
	vResult.x = vVector1.x / Scaler;   
	vResult.y = vVector1.y / Scaler;   
	vResult.z = vVector1.z / Scaler;   
	return vResult;        
}
// 下面的函数返回两个矢量的叉积
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{ 
	CVector3 vCross;        
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));
	return vCross;        
}
// 下面的函数规范化矢量
CVector3 Normalize(CVector3 vNormal)
{ 
	double Magnitude;       
	Magnitude = Mag(vNormal);     // 获得矢量的长度
	vNormal.x /= (float)Magnitude;    
	vNormal.y /= (float)Magnitude;    
	vNormal.z /= (float)Magnitude;    
	return vNormal;        
}
//  下面的函数用于计算对象的法向量
void CLoad3DS::ComputeNormals(t3DModel *pModel)
{ 
	CVector3 vVector1, vVector2, vNormal, vPoly[3];
	// 如果模型中没有对象，则返回
	if(pModel->numOfObjects <= 0)
		return;
	// 遍历模型中所有的对象
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// 获得当前的对象
		t3DObject *pObject = &(pModel->pObject[index]);
		// 分配需要的存储空间
		CVector3 *pNormals  = new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals = new CVector3 [pObject->numOfFaces];
		pObject->pNormals  = new CVector3 [pObject->numOfVerts];
		// 遍历对象的所有面
		for(int i=0; i < pObject->numOfFaces; i++)
		{
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];
			// 计算面的法向量
			vVector1 = Vector(vPoly[0], vPoly[2]);  // 获得多边形的矢量
			vVector2 = Vector(vPoly[2], vPoly[1]);  // 获得多边形的第二个矢量
			vNormal  = Cross(vVector1, vVector2);	// 获得两个矢量的叉积
			pTempNormals[i] = vNormal;				// 保存非规范化法向量
			vNormal  = Normalize(vNormal);			// 规范化获得的叉积
			pNormals[i] = vNormal;					// 将法向量添加到法向量列表中
		}
		//  下面求顶点法向量
		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;
		// 遍历所有的顶点
		for (int i = 0; i < pObject->numOfVerts; i++)   
		{ 
			for (int j = 0; j < pObject->numOfFaces; j++) // 遍历所有的三角形面
			{   // 判断该点是否与其它的面共享
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{ vSum = AddVector(vSum, pTempNormals[j]);
				shared++;        
				}
			}      
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));
			// 规范化最后的顶点法向
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]); 
			vSum = vZero;        
			shared = 0;          
		}
		// 释放存储空间，开始下一个对象
		delete [] pTempNormals;
		delete [] pNormals;
	}
}
