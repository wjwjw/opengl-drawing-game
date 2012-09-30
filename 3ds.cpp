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
	
	CImage *img = new CImage;							// �½�CImage����
	HRESULT hResult = img->Load(strFileName);			// ����ͼ���ļ�
	if(hResult == E_FAIL){
		char error[255];
		sprintf(error, "%s������", strFileName);
		MessageBox(NULL, error, "����", MB_OK|MB_ICONINFORMATION);
		exit(0);
	}

	int width = img->GetWidth();	// ��ȡͼ�����
	int height = img->GetHeight();	// ��ȡͼ��߶�
	GLubyte *image = new GLubyte[width * height * 3];	// ���ڱ���ͼ�����ݵ�����
	// ��ͼ�����ݶ���image����
	for(int j = 0; j < height; j++){
		for(int i = 0; i < width; i++){
			int index = (j * width + i) * 3;
			RGBQUAD rgb = GetPixelColor(img, i, j);
			image[index] = rgb.rgbRed;
			image[index+1] = rgb.rgbGreen;
			image[index+2] = rgb.rgbBlue;
		}
	}
	delete img;		// CImage���������ã������Ѷ���image����
	
	glGenTextures(1, &textureArray[textureID]);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// ͼ��ÿ�����ݽ�������
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	delete image;	// �������������Կ������ڴ����ˣ������ڴ��е�image������
}

void CLoad3DS::show3ds()
{
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	
	GLboolean bTex = glIsEnabled(GL_TEXTURE_2D);

	//����ģ��
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		if(g_3DModel.pObject.size() <= 0) 
		{
			break;
		}
		
		t3DObject *pObject = &g_3DModel.pObject[i];
		
		if(pObject->bHasTexture)
		{
			//����������������,������
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} 
		else
		{
			//û������,�ر���ͼ
			glDisable(GL_TEXTURE_2D);
		}
		//ָ����ɫ
		glColor3ub(255, 255, 255);
		
		glBegin(GL_TRIANGLES);
		//������������������Ƭ
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			for(int tex = 0; tex < 3; tex++)    
			{
				int index = pObject->pFaces[j].vertIndex[tex]; 
				//���÷���
				glNormal3f(pObject->pNormals[index].x,
					pObject->pNormals[index].y,  
					pObject->pNormals[index].z); 
				
				if(pObject->bHasTexture)    
				{
					//������������
					if(pObject->pTexVerts)    
						glTexCoord2f(pObject->pTexVerts[index].u, pObject->pTexVerts[index].v);
				}
				else
				{
					//û������,������ɫ
					if(g_3DModel.pMaterials.size() && pObject->materialID>= 0) 
					{
						BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;
						
						glColor3ub(pColor[0],pColor[1],pColor[2]);
					}
				}
				//���Ƹ�����
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
//����ģ��
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
	t3DObject newObject = {0};		// �������ӵ���������
	tMatInfo newTexture = {0};		// �������ӵ���������
	unsigned int version = 0;		// �����ļ��汾
	int buffer[50000] = {0};		// ������������Ҫ������
	
	m_CurrentChunk = new tChunk;    // Ϊ�µĿ����ռ�  
	// ����ÿ��һ���¿飬��Ҫ�ж�һ�¿��ID������ÿ�����Ҫ�Ķ���ģ����������
	// ����ǲ���Ҫ����Ŀ飬���Թ�
	// ���������ӿ飬ֱ���ﵽԤ���ĳ���
	while (pPreChunk->bytesRead < pPreChunk->length)
	{
		// ������һ����
		ReadChunk(m_CurrentChunk);
		// �жϿ��ID��
		switch (m_CurrentChunk->ID)
		{
		case VERSION:       // �ļ��汾��
			// �ڸÿ�����һ���޷��Ŷ��������������ļ��İ汾
			// �����ļ��İ汾�ţ������ֽ������ӵ�bytesRead������
			m_CurrentChunk->bytesRead += fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			// ����ļ��汾�Ŵ���3������һ��������Ϣ
			if (version > 0x03)
				MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			break;
			
		case OBJECTINFO:      // ����汾��Ϣ
			// ������һ����
			ReadChunk(m_TempChunk);
			// �������İ汾��
			m_TempChunk->bytesRead += fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);
			// ���Ӷ�����ֽ���
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;
			// ������һ����
			ReadNextChunk(pModel, m_CurrentChunk);
			break;
			
		case MATERIAL:       // ������Ϣ
			// ���ʵ���Ŀ����
			pModel->numOfMaterials++;
			// ����������������һ���հ������ṹ
			pModel->pMaterials.push_back(newTexture);
			// �������װ�뺯��
			ReadNextMatChunk(pModel, m_CurrentChunk);
			break;
			
		case OBJECT:       // ���������
			// �ÿ��Ƕ�����Ϣ���ͷ���������˶���������
			// ����������
			pModel->numOfObjects++;
			// ����һ���µ�tObject�ڵ㵽����������
			pModel->pObject.push_back(newObject);
			// ��ʼ������������������ݳ�Ա
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));
			// ��ò������������ƣ�Ȼ�����Ӷ�����ֽ���
			m_CurrentChunk->bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);
			// ��������Ķ�����Ϣ�Ķ���
			ReadNextObjChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), m_CurrentChunk);
			break;
			
		case EDITKEYFRAME:  
			// �����ؼ�֡��Ķ��룬������Ҫ������ֽ���
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
			
		default: 
			//  �������к��ԵĿ�����ݵĶ��룬������Ҫ������ֽ���
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		// ���Ӵ����������ֽ���
		pPreChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	// �ͷŵ�ǰ����ڴ�ռ�
	delete m_CurrentChunk;
	m_CurrentChunk = pPreChunk;
}

//  ����ĺ����������е��ļ��ж������Ϣ
void CLoad3DS::ReadNextObjChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreChunk)
{ 
	int buffer[50000] = {0};     // ���ڶ��벻��Ҫ������
	// ���µĿ����洢�ռ�
	m_CurrentChunk = new tChunk;
	
	// ��������������ֱ�����ӿ����
	while (pPreChunk->bytesRead < pPreChunk->length)
	{ // ������һ����
		ReadChunk(m_CurrentChunk);
		// ������������ֿ�
		switch (m_CurrentChunk->ID)
		{
		case OBJ_MESH:     // ���������һ���¿�
			// ʹ�õݹ麯�����ã��������¿�
			ReadNextObjChunk(pModel, pObject, m_CurrentChunk);
			break;
		case OBJ_VERTICES:    // �����Ƕ��󶥵�
			ReadVertices(pObject, m_CurrentChunk);
			break;
		case OBJ_FACES:     // ������Ƕ������
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;
		case OBJ_MATERIAL:    // ������Ƕ���Ĳ�������
			// �ÿ鱣���˶�����ʵ����ƣ�������һ����ɫ��Ҳ������һ������ӳ�䡣ͬʱ�ڸÿ���Ҳ������
			// �����������������
			// ����������Ĳ�������
			ReadObjMat(pModel, pObject, m_CurrentChunk);   
			break;
		case OBJ_UV:      // ��������UV��������
			// ��������UV��������
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;
		default:  
			// �Թ�����Ҫ����Ŀ�
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		// ���Ӵ������ж�����ֽ�����ǰ��Ķ�����ֽ���
		pPreChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	// �ͷŵ�ǰ����ڴ�ռ䣬���ѵ�ǰ������Ϊǰ���
	delete m_CurrentChunk;
	m_CurrentChunk = pPreChunk;
}

//  ����ĺ����������еĲ�����Ϣ
void CLoad3DS::ReadNextMatChunk(t3DModel *pModel, tChunk *pPreChunk)
{
	int buffer[50000] = {0};     // ���ڶ��벻��Ҫ������
	// ����ǰ�����洢�ռ�
	m_CurrentChunk = new tChunk;
	// ����������Щ�飬֪�����ӿ����
	while (pPreChunk->bytesRead < pPreChunk->length)
	{ // ������һ��
		ReadChunk(m_CurrentChunk);
		// �ж϶������ʲô��
		switch (m_CurrentChunk->ID)
		{
		case MATNAME:       // ���ʵ�����
			// ������ʵ�����
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
			
		case MATDIFFUSE:      // �����R G B��ɫ
			ReadColor(&(pModel->pMaterials[pModel->numOfMaterials - 1]), m_CurrentChunk);
			break;
			
		case MATMAP:       // ������Ϣ��ͷ��
			// ������һ�����ʿ���Ϣ
			ReadNextMatChunk(pModel, m_CurrentChunk);
			break;
			
		case MATMAPFILE:      // �����ļ�������
			// ������ʵ��ļ�����
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
			
		default:  
			// �ӹ�����Ҫ����Ŀ�
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		// ���Ӵ������ж�����ֽ���
		pPreChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	// ɾ����ǰ�飬������ǰ������Ϊǰ��Ŀ�
	delete m_CurrentChunk;
	m_CurrentChunk = pPreChunk;
}

//  ���溯��������ID�ź������ֽڳ���
void CLoad3DS::ReadChunk(tChunk *pChunk)
{ 
	// ������ID�ţ�ռ����2���ֽڡ����ID����OBJECT��MATERIALһ����˵�����ڿ���������������
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_FilePointer);
	// Ȼ������ռ�õĳ��ȣ��������ĸ��ֽ�
	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_FilePointer);
}

//  ����ĺ�������һ���ַ���
int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;
	// ����һ���ֽڵ�����
	fread(pBuffer, 1, 1, m_FilePointer);
	// ֱ������
	while (*(pBuffer + index++) != 0) {
		// ����һ���ַ�ֱ��NULL
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}
	// �����ַ����ĳ���
	return strlen(pBuffer) + 1;
}

//  ����ĺ�������RGB��ɫ
void CLoad3DS::ReadColor(tMatInfo *pMaterial, tChunk *pChunk)
{
	// ������ɫ����Ϣ
	ReadChunk(m_TempChunk);
	// ����RGB��ɫ
	m_TempChunk->bytesRead += fread(pMaterial->color, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);
	// ���Ӷ�����ֽ���
	pChunk->bytesRead += m_TempChunk->bytesRead;
}

//  ����ĺ������붥������
void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreChunk)
{
	unsigned short index = 0;     // ���ڶ��뵱ǰ�������
	// ����ö����������Ŀ
	pPreChunk->bytesRead += fread(&pObject->numOfFaces, 1, 2, m_FilePointer);
	// ����������Ĵ洢�ռ䣬����ʼ���ṹ
	pObject->pFaces = new tFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);
	// �������������е���
	for(int i = 0; i < pObject->numOfFaces; i++)
	{ for(int j = 0; j < 4; j++)
	{ // ���뵱ǰ��ĵ�һ���� 
		pPreChunk->bytesRead += fread(&index, 1, sizeof(index), m_FilePointer);
		if(j < 3)
		{ // ��������������Ľṹ��
			pObject->pFaces[i].vertIndex[j] = index;
		}
	}
	}
}

//  ����ĺ�����������UV����
void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreChunk)
{
	// Ϊ�˶�������UV���꣬������Ҫ����UV�����������Ȼ��Ŷ�����������
	// ����UV���������
	pPreChunk->bytesRead += fread(&pObject->numTexVertex, 1, 2, m_FilePointer);
	// ���䱣��UV������ڴ�ռ�
	pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
	// ������������
	pPreChunk->bytesRead += fread(pObject->pTexVerts, 1, pPreChunk->length - pPreChunk->bytesRead, m_FilePointer);
}

//  �������Ķ���
void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreChunk)
{
	// �ڶ���ʵ�ʵĶ���֮ǰ�����ȱ���ȷ����Ҫ������ٸ����㡣
	// ���붥�����Ŀ
	pPreChunk->bytesRead += fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);
	// ���䶥��Ĵ洢�ռ䣬Ȼ���ʼ���ṹ��
	pObject->pVerts = new CVector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);
	// ���붥������
	pPreChunk->bytesRead += fread(pObject->pVerts, 1, pPreChunk->length - pPreChunk->bytesRead, m_FilePointer);
	// �����Ѿ����������еĶ��㡣
	// ��Ϊ3D Studio Max��ģ�͵�Z����ָ���ϵģ������Ҫ��y���z�ᷭת������
	// ����������ǽ�Y���Z�ύ����Ȼ��Z�ᷴ��
	// �������еĶ���
	for(int i = 0; i < pObject->numOfVerts; i++)
	{ // ����Y���ֵ
		float fTempY = pObject->pVerts[i].y;
		// ����Y���ֵ����Z���ֵ
		pObject->pVerts[i].y = pObject->pVerts[i].z;
		// ����Z���ֵ����-Y���ֵ 
		pObject->pVerts[i].z = -fTempY;
	}
}

//  ����ĺ����������Ĳ�������
void CLoad3DS::ReadObjMat(t3DModel *pModel, t3DObject *pObject, tChunk *pPreChunk)
{
	char strMaterial[255] = {0};   // �����������Ĳ�������
	int buffer[50000] = {0};    // �������벻��Ҫ������
	// ���ʻ�������ɫ�������Ƕ����������Ҳ���ܱ������������ȡ�����ȵ���Ϣ��
	// ������븳�赱ǰ����Ĳ�������
	pPreChunk->bytesRead += GetString(strMaterial);
	// �������е�����
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{ //�������������뵱ǰ����������ƥ��
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{ // ���ò���ID
			pObject->materialID = i;
			// �ж��Ƿ�������ӳ�䣬���strFile��һ�����ȴ���1���ַ�������������
			if(strlen(pModel->pMaterials[i].strFile) > 0) {
				// ���ö��������ӳ���־
				pObject->bHasTexture = true;
			} 
			break;
		}
		else
		{ // ����ö���û�в��ʣ�������IDΪ-1
			pObject->materialID = -1;
		}
	}
	pPreChunk->bytesRead += fread(buffer, 1, pPreChunk->length - pPreChunk->bytesRead, m_FilePointer);
}   
//  �������Щ������Ҫ�������㶥��ķ�����������ķ�������Ҫ�����������
// ����ĺ궨�����һ��ʸ���ĳ���
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))
// ����ĺ��������������ʸ��
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{ 
	CVector3 vVector;       
	vVector.x = vPoint1.x - vPoint2.x;   
	vVector.y = vPoint1.y - vPoint2.y;   
	vVector.z = vPoint1.z - vPoint2.z;   
	return vVector;        
}
// ����ĺ�������ʸ�����
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{ 
	CVector3 vResult;       
	vResult.x = vVector2.x + vVector1.x;  
	vResult.y = vVector2.y + vVector1.y;  
	vResult.z = vVector2.z + vVector1.z;  
	return vResult;        
}
// ����ĺ�������ʸ��������
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{ 
	CVector3 vResult;       
	vResult.x = vVector1.x / Scaler;   
	vResult.y = vVector1.y / Scaler;   
	vResult.z = vVector1.z / Scaler;   
	return vResult;        
}
// ����ĺ�����������ʸ���Ĳ��
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{ 
	CVector3 vCross;        
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));
	return vCross;        
}
// ����ĺ����淶��ʸ��
CVector3 Normalize(CVector3 vNormal)
{ 
	double Magnitude;       
	Magnitude = Mag(vNormal);     // ���ʸ���ĳ���
	vNormal.x /= (float)Magnitude;    
	vNormal.y /= (float)Magnitude;    
	vNormal.z /= (float)Magnitude;    
	return vNormal;        
}
//  ����ĺ������ڼ������ķ�����
void CLoad3DS::ComputeNormals(t3DModel *pModel)
{ 
	CVector3 vVector1, vVector2, vNormal, vPoly[3];
	// ���ģ����û�ж����򷵻�
	if(pModel->numOfObjects <= 0)
		return;
	// ����ģ�������еĶ���
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// ��õ�ǰ�Ķ���
		t3DObject *pObject = &(pModel->pObject[index]);
		// ������Ҫ�Ĵ洢�ռ�
		CVector3 *pNormals  = new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals = new CVector3 [pObject->numOfFaces];
		pObject->pNormals  = new CVector3 [pObject->numOfVerts];
		// ���������������
		for(int i=0; i < pObject->numOfFaces; i++)
		{
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];
			// ������ķ�����
			vVector1 = Vector(vPoly[0], vPoly[2]);  // ��ö���ε�ʸ��
			vVector2 = Vector(vPoly[2], vPoly[1]);  // ��ö���εĵڶ���ʸ��
			vNormal  = Cross(vVector1, vVector2);	// �������ʸ���Ĳ��
			pTempNormals[i] = vNormal;				// ����ǹ淶��������
			vNormal  = Normalize(vNormal);			// �淶����õĲ��
			pNormals[i] = vNormal;					// �����������ӵ��������б���
		}
		//  �����󶥵㷨����
		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;
		// �������еĶ���
		for (int i = 0; i < pObject->numOfVerts; i++)   
		{ 
			for (int j = 0; j < pObject->numOfFaces; j++) // �������е���������
			{   // �жϸõ��Ƿ����������湲��
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{ vSum = AddVector(vSum, pTempNormals[j]);
				shared++;        
				}
			}      
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));
			// �淶�����Ķ��㷨��
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]); 
			vSum = vZero;        
			shared = 0;          
		}
		// �ͷŴ洢�ռ䣬��ʼ��һ������
		delete [] pTempNormals;
		delete [] pNormals;
	}
}