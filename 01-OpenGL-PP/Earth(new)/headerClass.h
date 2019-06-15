#pragma once
#include<vector>
#include"zmath.h"
#include<gl/glew.h>
#include<Windows.h>

typedef struct tagTgaHeader {

	GLbyte  ID_Length;
	GLbyte  ColorMapType;
	GLbyte  ImageType;
	// Color map specifications
	GLbyte firstEntryIndex[2];
	GLbyte colorMapLength[2];

	GLbyte colorMapEntrySize;

	//image specification
	GLshort xOrigin;
	GLshort yOrigin;
	GLshort ImageWidth;
	GLshort ImageHeight;
	GLbyte  PixelDepth;
	GLbyte ImageDescriptor;

}TGA_Header;

typedef struct myMaterialData {
	GLuint diffuseTextures;
	GLuint ambientTexture;
	GLuint bumpTextures;
}MaterialData;

class TextureData
{
public:
	TextureData() {};
	~TextureData() {};

	//getters
	unsigned char* getData(void) {
		return data;
	}
	GLenum getInternalFormatType(void) {
		return internalFormatType;
	}
	GLenum getEnumFormatType(void) {
		return enumFormatType;
	}
	GLsizei getWidth(void) {
		return iWidth;
	}
	GLsizei getHeight(void) {
		return iHeight;
	}
	//setters
	void setData(unsigned char* buffer) {
		data = buffer;
	}

	void setInternalFormatType(GLenum format) {
		internalFormatType = format;
	}
	void setEnumFormatType(GLenum format) {
		enumFormatType = format;
	}
	void setWidth(GLsizei width) {
		iWidth = width;
	}
	void setHeight(GLsizei height) {
		iHeight = height;
	}

private:
	unsigned char* data;
	GLenum internalFormatType,enumFormatType;
	GLsizei iWidth, iHeight;
};

class Camera
{
public:
	Camera() {};
	~Camera() {};

	void moveDirection(float speed, WPARAM direction);
	void moveRotation(float anglePitch, float angleYaw);

	 zmath::vec3 getPosition(void) {
		return position;
	}
	void setPosition_Y(float y)
	{
		position[1] = y;
	}

	void setPosition_Z(float z)
	{
		position[2] = z;
	}

	float getPitch(void) {
		return pitch;
	}
	float getYaw(void) {
		return yaw;
	}
	float getRoll(void) {
		return roll;
	}
	void invertPitch(void)
	{
		pitch =-pitch;
	}

	void setPitch(float fPitch) {
		pitch = fPitch;
	}
	void setYaw(float fYaw) {
		yaw = fYaw;
	}

	void setRoll(float fRoll) {
		roll = fRoll;
	}

private:
	zmath::vec3 position = zmath::vec3(0.0f, 0.0f, 0.0f);
	float pitch=0.0f, yaw = 0.0f, roll = 0.0f;

};

class shaderLoader
{
public:
	shaderLoader() {};
	shaderLoader(char const* vShaderFileName, char const* fShaderFileName);
	~shaderLoader() {};

	GLchar* load_ShaderFile(char const* path);
	GLuint loadThisShader(const char* filename, GLenum fileType);
	void loadThisProgram(void);
	void getAllUniformLocations(void);
	
	void loadModelMatrix(GLfloat* modelMatrix);
	void loadViewMatrix(Camera cam);
	void loadProjectionMatrix(GLfloat* ProjectionMatrix);

	void loadSkyViewMatrix(Camera cam);

	void loadLightColor(GLfloat r, GLfloat g, GLfloat b);
	void loadLightPosition(GLfloat* lightPosition);
	void isLkeyPressed(bool gbLight);
	void loadShineDamperReflectivity(GLfloat damper, GLfloat reflectivity);
	
	void loadModelTextureSampler(GLuint textureModel);
	void loadSecondModelTextureSampler(GLuint SecondTextureModel);
	void loadModelNormalSampler(GLuint normalTextureModel);

	void loadCubeMapSampler(GLuint CubeMapSampler);

	//void loadCameraPosition(GLfloat* camPosition);

//	void prepareRender(Entity EntityName, GLfloat ProjMatrix[]);
//	void loadRender(Camera viewCam, bool gbLight);
//	void multiTextureRender(Entity EntityName, GLfloat clipPlane[]);
//	void NeoRender(Entity EntityName, GLfloat clipPlane[]);
	void cleanUp(void);
	


	void start(void)
	{
		glUseProgram(ShaderProgramObject);
	}
	void stop(void)
	{
		glUseProgram(0);
	}

	void setVertexShaderObjectFromDerived(GLuint vertexShaderObjectFromDerived)
	{
		VertexShaderObject = vertexShaderObjectFromDerived;
	}

	void setFragmentShaderObjectFromDerived(GLuint fragmentShaderObjectFromDerived)
	{
		FragmentShaderObject = fragmentShaderObjectFromDerived;
	}
	GLuint getShaderProgramObject(void)
	{
		return ShaderProgramObject;
	}

	private:
		FILE* shaderLog;
		
	GLuint VertexShaderObject;
	GLuint FragmentShaderObject;
	GLuint ShaderProgramObject;
	
	GLuint ModelxUniform;
	GLuint ViewxUniform;
	GLuint ProjxUniform;
	
	GLuint SamplerUniform;
	GLuint secondSamplerUniform;//multiTexture
	GLuint NormSamplerUniform;
	GLuint cubeMapSamplerUniform;

	GLuint LightColorUniform;
	GLuint LightPosUniform;
	GLuint KdUniform;
	GLuint LKeyPressedUniform;
	GLuint ShineDamperUniform;
	GLuint ReflectivityUniform;


	GLuint camPosUniform;
	
};

class skyBox
{
public:
	skyBox() {
		toVao();
		/*CubeMapTextureFileNames[0] = "res/skyBoxes/Stars/rightImage.tga"; 
		CubeMapTextureFileNames[1] = "res/skyBoxes/Stars/leftImage.tga"; 
		CubeMapTextureFileNames[2] = "res/skyBoxes/Stars/upImage.tga"; 
		CubeMapTextureFileNames[3] = "res/skyBoxes/Stars/downImage.tga"; 
		CubeMapTextureFileNames[4] = "res/skyBoxes/Stars/backImage.tga"; 
		CubeMapTextureFileNames[5] = "res/skyBoxes/Stars/frontImage.tga";*/
		
		CubeMapTextureFileNames[0] = "res/skyBoxes/galaxy_cubemap_dark/GalaxyTex_NegativeX.tga";
		CubeMapTextureFileNames[1] = "res/skyBoxes/galaxy_cubemap_dark/GalaxyTex_PositiveX.tga"; 
		CubeMapTextureFileNames[2] = "res/skyBoxes/galaxy_cubemap_dark/GalaxyTex_PositiveY.tga";
		CubeMapTextureFileNames[3] = "res/skyBoxes/galaxy_cubemap_dark/GalaxyTex_NegativeY.tga";
		CubeMapTextureFileNames[4] = "res/skyBoxes/galaxy_cubemap_dark/GalaxyTex_NegativeZ.tga";
		CubeMapTextureFileNames[5] = "res/skyBoxes/galaxy_cubemap_dark/GalaxyTex_PositiveZ.tga";
	

		if (fopen_s(&cubemap, "Log/cubemap.txt", "w") != 0)
			{
				MessageBox(NULL, TEXT("cubemapLog File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
				exit(0);
			}
			else
			{
				fprintf(cubemap, "cubemapLog File Is Successfully Opened.\n");
			}
		fprintf(cubemap, "\n%s\n", CubeMapTextureFileNames[5]);
		
		cubeMapTextureID=loadCubeMapTextures(CubeMapTextureFileNames);


	};
	~skyBox() 
	{			
		if (cubemap)
		fclose(cubemap); 
	};

	void toVao(void)
	{
		glGenVertexArrays(1, &vao_skybox);
		glBindVertexArray(vao_skybox);

		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (const GLvoid *)vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void renderSkyBox(shaderLoader skyBoxShader, GLfloat* projectionMatrix, Camera cam)
	{
		skyBoxShader.start();
		skyBoxShader.loadProjectionMatrix(projectionMatrix);
		skyBoxShader.loadSkyViewMatrix(cam);
		skyBoxShader.loadCubeMapSampler(cubeMapTextureID);
		glDepthMask(GL_FALSE);
		glBindVertexArray(vao_skybox);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		skyBoxShader.stop();
	
	}

	GLuint loadCubeMapTextures(char* cubeMapTextures[])
	{
		GLuint testTextureCube;
		glGenTextures(1, &testTextureCube);
		glBindTexture(GL_TEXTURE_CUBE_MAP, testTextureCube);

		for (int i = 0; i < 6; i++)
		{
			TextureData cubeMapData = loadTGAdata(cubeMapTextures[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cubeMapData.getInternalFormatType(), cubeMapData.getWidth(), cubeMapData.getHeight(), 0, cubeMapData.getEnumFormatType(), GL_UNSIGNED_BYTE, cubeMapData.getData());
			fprintf(cubemap, "width=%d,height=%d", cubeMapData.getWidth(), cubeMapData.getHeight());
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return testTextureCube;
	}
	TextureData loadTGAdata(char* fileName) {

		FILE* tgaPtr = NULL;
		FILE* tgaLog = NULL;

		TextureData tDataInfo;
		unsigned char* data;

		TGA_Header th;
		ZeroMemory(&th, sizeof(th));

		

		if (fopen_s(&tgaLog, "Log/TGAlog_skyBox.txt", "w") != 0)
		{
			MessageBox(NULL, TEXT("TgaLog File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
			exit(0);
		}
		else
		{
			fprintf(tgaLog, "TgaLog File Is Successfully Opened.\n");
		}

		fprintf(tgaLog, "texturename in fopen =%s", fileName);

		if (fopen_s(&tgaPtr, fileName, "rb") != 0)
		{
			fprintf(tgaLog, "texturename in fopen =%s", fileName);
			MessageBox(NULL, TEXT(" TGA file pointer not found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
			exit(0);
		}

		fread(&th, sizeof(th), 1, tgaPtr);//TGA header

		if (th.ImageType != 2 || th.ImageWidth <= 0 || th.ImageHeight <= 0)
		{
			fclose(tgaPtr);
			MessageBox(NULL, TEXT(" WRONG TGA format type..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
			exit(0);
		}

		//fprintf(tgaLog, "\nID_Length=%d \nColorMapType= %d\nImageType= %d\nfirstEntryIndex=%d\ncolorMapLength=%d\ncolorMapEntrySize=%d\nxOrigin=%d\nyOrigin=%d\nImageWidth=%d\nImageHeight =%d\nPixelDepth=%d\nImageDescriptor=%d\n", th.ID_Length, th.ColorMapType, th.ImageType,th.firstEntryIndex[1],th.colorMapLength[1],th.colorMapEntrySize,th.xOrigin,th.yOrigin,th.ImageWidth,th.ImageHeight,th.PixelDepth,th.ImageDescriptor);

		if (th.PixelDepth == 24)
		{
			tDataInfo.setInternalFormatType(GL_RGB);
			tDataInfo.setEnumFormatType(GL_BGR);
		}
		else if (th.PixelDepth == 32)
		{
			tDataInfo.setInternalFormatType(GL_RGBA);
			tDataInfo.setEnumFormatType(GL_BGRA);
		}			
		else
		{
			fclose(tgaPtr);
			MessageBox(NULL, TEXT(" WRONG TGA format type..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
			exit(0);
		}
		tDataInfo.setWidth(th.ImageWidth);
		tDataInfo.setHeight(th.ImageHeight);
		 

		GLuint bytesPerPixel = th.PixelDepth / 8;
		GLuint bufferSize = th.ImageWidth * th.ImageHeight * bytesPerPixel;

		fprintf(tgaLog, "wXh ==%d %d , %d ,buffer size=%d", th.ImageHeight, th.ImageWidth,th.PixelDepth,bufferSize);

		data = new unsigned char[bufferSize];

		if (!data)
		{
			fclose(tgaPtr);
			MessageBox(NULL, TEXT("No memory for data found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
			exit(0);
		}

		fseek(tgaPtr, sizeof(th), SEEK_SET);

		fread(data, bufferSize, 1, tgaPtr);

		fclose(tgaPtr);

		fprintf(tgaLog, "\n\nlast element=%d\n", data[bufferSize]);

		if (tgaLog)
		{
			fprintf(tgaLog, "TgaLog File Is Successfully Closed.\n");
			fclose(tgaLog);
			tgaLog = NULL;
		}
		tDataInfo.setData(data);
		return(tDataInfo);

		delete data;
		data = NULL;
	}




private:
	GLuint vao_skybox;
	GLuint vbo_vertices;
	GLuint vbo_texCoords;

	GLuint cubeMapTextureID;
	GLuint testTexture;
	char* CubeMapTextureFileNames[6];

	FILE * cubemap;

	const GLint CubeSize=2000;
	const GLint vertices[108] =
	{
		-CubeSize,  CubeSize, -CubeSize,
		-CubeSize, -CubeSize, -CubeSize,
		 CubeSize, -CubeSize, -CubeSize,
		 CubeSize, -CubeSize, -CubeSize,
		 CubeSize,  CubeSize, -CubeSize,
		-CubeSize,  CubeSize, -CubeSize,

		-CubeSize, -CubeSize,  CubeSize,
		-CubeSize, -CubeSize, -CubeSize,
		-CubeSize,  CubeSize, -CubeSize,
		-CubeSize,  CubeSize, -CubeSize,
		-CubeSize,  CubeSize,  CubeSize,
		-CubeSize, -CubeSize,  CubeSize,


		CubeSize, -CubeSize, -CubeSize,
		CubeSize, -CubeSize,  CubeSize,
		CubeSize,  CubeSize , CubeSize,
		CubeSize,  CubeSize,  CubeSize,
		CubeSize,  CubeSize, -CubeSize,
		CubeSize, -CubeSize, -CubeSize,


		-CubeSize, -CubeSize,  CubeSize,
		-CubeSize,  CubeSize,  CubeSize,
		 CubeSize,  CubeSize,  CubeSize,
		 CubeSize,  CubeSize,  CubeSize,
		 CubeSize, -CubeSize,  CubeSize,
		-CubeSize, -CubeSize,  CubeSize,


		-CubeSize,  CubeSize, -CubeSize,
		 CubeSize,  CubeSize, -CubeSize,
		 CubeSize,  CubeSize , CubeSize,
		 CubeSize,  CubeSize,  CubeSize,
		-CubeSize,  CubeSize,  CubeSize,
		-CubeSize,  CubeSize, -CubeSize,


		-CubeSize, -CubeSize, -CubeSize,
		-CubeSize, -CubeSize,  CubeSize,
		 CubeSize, -CubeSize ,-CubeSize,
		 CubeSize, -CubeSize, -CubeSize,
		-CubeSize, -CubeSize,  CubeSize,
		 CubeSize, -CubeSize,  CubeSize
	};

};

class stars
{
public:
	stars() {
		generateStars();
		toVao(); };
	~stars() {};

	void toVao(void)
	{
		glGenVertexArrays(1, &vao_Stars);
		glBindVertexArray(vao_Stars);

		glGenBuffers(1, &vbo_StarVertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_StarVertices);
		glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr) starsBufferSize, (const GLvoid *)vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void generateStars(void)
	{
		const GLuint countStars = 7000;
		VerticesCount = countStars * 3;//xyz
		starsBufferSize = VerticesCount * sizeof(float);
		vertices = new float[starsBufferSize];

		srand(5);
		for (int i = 0; i < countStars; i += 1)
		{
			vertices[i*3] = ((float)rand() / (float)RAND_MAX)*1000.0f - 500.0f;
			vertices[i*3 +1] = ((float)rand() / (float)RAND_MAX)*1000.0f - 500.0f;
			vertices[i*3 +2] = ((float)rand() / (float)RAND_MAX)*1000.0f - 500.0f;
		}

		/*srand(7);
		for (int i = 0; i < 70; i += 1)
		{
			Xl[i] = ((float)rand() / (float)RAND_MAX)*800.0f - 400.0f;
			Yl[i] = ((float)rand() / (float)RAND_MAX)*600.0f - 300.0f;
			Zl[i] = ((float)rand() / (float)RAND_MAX)*(-1000.0f);
		}*/
		
	}
	void renderStars(shaderLoader starsShader,float * projectionMatrix,Camera cam)
	{
		starsShader.start();
		starsShader.loadProjectionMatrix(projectionMatrix);
		starsShader.loadSkyViewMatrix(cam);

		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glBindVertexArray(vao_Stars);
		glDrawArrays(GL_POINTS, 0, VerticesCount);
		glBindVertexArray(0);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		starsShader.stop();
	}
	GLuint getVao_Stars(void)
	{
		return vao_Stars;
	}

private:
	GLuint vao_Stars;
	GLuint vbo_StarVertices;

	 GLuint VerticesCount;
	 GLuint starsBufferSize;
	GLfloat *vertices;
};

class MyModel
{
public:
	MyModel() 
	{
		if (fopen_s(&myModelLog, "Log/myModel.txt", "w") != 0)
		{
			MessageBox(NULL, TEXT("myModel File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
			exit(0);
		}
		else
		{
			fprintf(myModelLog, "myModel File Is Successfully Opened.\n");
		}
	};
//	MyModel(const char* filename, bool anisotropicFilter);
	~MyModel() { if (myModelLog)fclose(myModelLog); myModelLog = NULL; };
	void myObjLoader(const char* filePath);
	void myTangentLoader(std::vector<zmath::vec3> vertices, std::vector<zmath::vec2> uvs);
	MaterialData getMaterial(FILE * mtlPtr, char mtlName[]);
	char* SlashFormat(char* pathStr);
	void toVao(void);
	void createElementData(void);
	void cleanUp(void);

	std::vector<GLuint> getModelTexture(void) {
		return texture_model;
	}

	GLuint getModelNormalTexture(void) {
		return texture_modelNormal;
	}

	GLuint getVaoModel(void) {
		return vao_model;
	}
	std::vector<unsigned int> getCntMeshOffset(void) {
		return cntMeshOffset;
	}


protected:
	std::vector<zmath::vec3> vertices, normals, tangents;
	std::vector<zmath::vec2> texCoords;

	std::vector<float> modelAllArrayData;
	std::vector<unsigned int> modelIndices;

	std::vector<unsigned int> cntMeshOffset;

	std::vector<GLuint> texture_model;
	GLuint texture_ambient;
	GLuint texture_modelNormal;

	GLuint vao_model;
	GLuint vbo_index;
	GLuint vbo_model_array_data;

	bool flagAnisotropic;

	FILE * myModelLog = NULL;
};

class earthCloudShader :public shaderLoader
{
public:
	earthCloudShader(char const* vShaderFileName, char const* fShaderFileName)
	{
		setVertexShaderObjectFromDerived(loadThisShader(vShaderFileName, GL_VERTEX_SHADER));
		setFragmentShaderObjectFromDerived(loadThisShader(fShaderFileName, GL_FRAGMENT_SHADER));
		loadThisProgram();

		EarthCloudSamplerUniform = glGetUniformLocation(getShaderProgramObject(), "earthCloudSampler");
	}

	~earthCloudShader() {};


	void loadEarthCloudsTextureSampler(GLuint textureEarthClouds)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureEarthClouds);
		glUniform1i(EarthCloudSamplerUniform, 0);
	}


	//	void cleanUp(void);



private:
	GLuint EarthCloudSamplerUniform;

	GLuint LKeyPressedUniform;

};

class earthAtmoShader :public shaderLoader
{
public:
	earthAtmoShader(char const* vShaderFileName, char const* fShaderFileName)
	{
		setVertexShaderObjectFromDerived(loadThisShader(vShaderFileName, GL_VERTEX_SHADER));
		setFragmentShaderObjectFromDerived(loadThisShader(fShaderFileName, GL_FRAGMENT_SHADER));
		loadThisProgram();

	}

	~earthAtmoShader() {};


	//	void cleanUp(void);

private:
	GLuint LKeyPressedUniform;

};

class earth: public MyModel
{
public:
	earth(const char* filename, bool anisotropicFilter)
	{
		GLuint loadTGA(char* fileName, bool anisotropicFilter);
		flagAnisotropic = anisotropicFilter;
		myObjLoader(filename);
		createElementData();
		toVao();
		myIdentity(modelMatrix);
		myIdentity(translateMatrix);
		myIdentity(rotationMatrix);

		earthCloudTextureID = loadTGA("res/textures/Earth_textures/4096_clouds.tga", true);
	}
	~earth() {};

	void renderEarth(shaderLoader earthShader,earthCloudShader CloudShader,earthAtmoShader atmoShader,GLfloat* perspectiveProjMatrix,Camera cam,bool gbLight)
	{
		myIdentity(modelMatrix);
		myIdentity(translateMatrix);
		myIdentity(rotationMatrix);
		myTranslate(translateMatrix, 0.6f, 0.0f, -2.7f);
		multiplyMatrices1(modelMatrix, translateMatrix);
		myRotate(rotationMatrix, myAngle, 0.0f, 1.0f, 0.0f);
		multiplyMatrices1(modelMatrix, rotationMatrix);

		myAngle += 0.024f;
		if (myAngle > 360.0f)
			myAngle -= myAngle;

		earthShader.start();
		earthShader.loadViewMatrix(cam);
		earthShader.loadProjectionMatrix(perspectiveProjMatrix);
		earthShader.loadLightPosition(lightPosition);
		earthShader.loadModelMatrix(modelMatrix);
		earthShader.isLkeyPressed(gbLight);

		earthShader.loadModelTextureSampler(texture_model[0]);
		earthShader.loadSecondModelTextureSampler(texture_ambient);
		earthShader.loadModelNormalSampler(texture_modelNormal);

		glBindVertexArray(vao_model);
		glDrawElements(GL_TRIANGLES, (GLsizei)modelIndices.size(), GL_UNSIGNED_INT, ( void*)0);

		earthShader.stop();

		float scaleMatrix[16];

		myAngleClouds += 0.029f;
		if (myAngleClouds > 360.0f)
			myAngleClouds -= myAngleClouds;

		myIdentity(modelMatrix);
		myIdentity(translateMatrix);
		myIdentity(rotationMatrix);
	myIdentity(scaleMatrix);

		myTranslate(translateMatrix, 0.6f, 0.0f, -2.7f);
		multiplyMatrices1(modelMatrix, translateMatrix);

		myScale(scaleMatrix, 1.002f, 1.002f, 1.002f);//clouds
		multiplyMatrices1(modelMatrix, scaleMatrix);

		myRotate(rotationMatrix,-55.0f+ myAngleClouds, 0.0f, 1.0f, 0.0f);
		multiplyMatrices1(modelMatrix, rotationMatrix);

		CloudShader.start();
		CloudShader.loadViewMatrix(cam);
		CloudShader.loadProjectionMatrix(perspectiveProjMatrix);
		CloudShader.loadModelMatrix(modelMatrix);
		CloudShader.isLkeyPressed(gbLight);
		CloudShader.loadLightPosition(lightPosition);

		CloudShader.loadEarthCloudsTextureSampler(earthCloudTextureID);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawElements(GL_TRIANGLES, (GLsizei)modelIndices.size(), GL_UNSIGNED_INT, (void*)0);
		glDisable(GL_BLEND);
		CloudShader.stop();


		myIdentity(modelMatrix);
		myIdentity(translateMatrix);
		myIdentity(rotationMatrix);
		myIdentity(scaleMatrix);

		myTranslate(translateMatrix, 0.6f, 0.0f, -2.7f);
		multiplyMatrices1(modelMatrix, translateMatrix);

		myScale(scaleMatrix, 1.004f, 1.004f, 1.004f);//atmo
		multiplyMatrices1(modelMatrix, scaleMatrix);


		atmoShader.start();
		atmoShader.loadViewMatrix(cam);
		atmoShader.loadProjectionMatrix(perspectiveProjMatrix);
		atmoShader.loadModelMatrix(modelMatrix);
		atmoShader.isLkeyPressed(gbLight);
		atmoShader.loadLightPosition(lightPosition);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawElements(GL_TRIANGLES, (GLsizei)modelIndices.size(), GL_UNSIGNED_INT, (void*)0);
		glDisable(GL_BLEND);
		atmoShader.stop();
	}

private:
	GLfloat translateMatrix[16];
	GLfloat rotationMatrix[16];
	GLfloat modelMatrix[16];
	float myAngle = 0.0f;
	float myAngleClouds = 0.0f;

	float lightPosition[4] = { -15.0,5.0,1.0,1.0 };

	GLuint earthCloudTextureID = 0;
};















