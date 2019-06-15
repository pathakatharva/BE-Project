#include"zmath.h"
#include<vector>
#include<Windows.h>
#include"headerClass.h"
#include<gl/glew.h>

using namespace zmath;
using namespace std;



//shader
shaderLoader::shaderLoader(char const* vShaderFileName, char const* fShaderFileName)
{

	VertexShaderObject = loadThisShader(vShaderFileName, GL_VERTEX_SHADER);
	FragmentShaderObject = loadThisShader(fShaderFileName, GL_FRAGMENT_SHADER);
	loadThisProgram();
}
 GLchar* shaderLoader::load_ShaderFile(char const* path)
{
	 char* buffer = 0;
	long length;
	FILE *f;
	fopen_s(&f, path, "rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		//rewind(f);
		fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc((length + 1) * sizeof(char));//+1 null
		if (buffer)
		{
			fread(buffer, sizeof(char), length, f);
		}
		fclose(f);
	}
	buffer[length] = '\0';
	// for (int i = 0; i < length; i++) {
	//    printf("buffer[%d] == %c\n", i, buffer[i]);
	// }
	//printf("buffer = %s\n", buffer);

	return buffer;
}
GLuint shaderLoader::loadThisShader(const char* filename, GLenum fileType)
{
	FILE* shaderLog;
	fopen_s(&shaderLog, "Log/shaderLog.txt", "w");

	GLuint shaderObj = glCreateShader(fileType);

	const GLchar *ShaderSourceCode = load_ShaderFile(filename);

	glShaderSource(shaderObj, 1, (const GLchar **)&ShaderSourceCode, NULL);
	glCompileShader(shaderObj);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(shaderObj, iInfoLogLength, &written, szInfoLog);
				fprintf(shaderLog, "%s Compilation Log : %s\n", filename, szInfoLog);
				free(szInfoLog);
				cleanUp();
				exit(0);
			}
		}
	}
	if (shaderLog)
		fclose(shaderLog);
	return shaderObj;
	
}
void shaderLoader::loadThisProgram(void)
{
	FILE* shaderLog=NULL;
	

	if (fopen_s(&shaderLog, "Log/shaderProgramLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("ShaderLog File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(shaderLog, "ShaderLog File Is Successfully Opened.\n");
	}
	GLint iInfoLogLength = 0;
	char *szInfoLog = NULL;

	 ShaderProgramObject = glCreateProgram();
	glAttachShader(ShaderProgramObject, VertexShaderObject);
	glAttachShader(ShaderProgramObject, FragmentShaderObject);

	glBindAttribLocation(ShaderProgramObject, 0, "vPosition");
	glBindAttribLocation(ShaderProgramObject, 1, "vTex0Coord");
	glBindAttribLocation(ShaderProgramObject, 2, "vNormal");
	glBindAttribLocation(ShaderProgramObject, 3, "vTangent");

	glLinkProgram(ShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(ShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(ShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(ShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(shaderLog, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				cleanUp();
				exit(0);
			}
		}
	}
	getAllUniformLocations();
	if (shaderLog)
		fclose(shaderLog);
}
void shaderLoader::getAllUniformLocations(void)
{
	// UNIFORMS

	//MVP matrices
	ModelxUniform = glGetUniformLocation(ShaderProgramObject , "uModelX");
	ViewxUniform = glGetUniformLocation(ShaderProgramObject, "uViewX");
	ProjxUniform = glGetUniformLocation(ShaderProgramObject, "uProjX");
//lights
	LightColorUniform = glGetUniformLocation(ShaderProgramObject, "uLightColor");
	LightPosUniform = glGetUniformLocation(ShaderProgramObject, "uLightPos");
	//KdUniform = glGetUniformLocation(ShaderProgramObject, "uKd");
//specular
	ShineDamperUniform = glGetUniformLocation(ShaderProgramObject, "uShineDamper");
	ReflectivityUniform = glGetUniformLocation(ShaderProgramObject, "uReflectivity");

	//sampler texture
	SamplerUniform = glGetUniformLocation(ShaderProgramObject, "tSampler");
	secondSamplerUniform=glGetUniformLocation(ShaderProgramObject, "t2Sampler");
	NormSamplerUniform = glGetUniformLocation(ShaderProgramObject, "normSampler");

	cubeMapSamplerUniform = glGetUniformLocation(ShaderProgramObject, "cubeMapTexture");
	//other
	LKeyPressedUniform = glGetUniformLocation(ShaderProgramObject, "Lkey");

	camPosUniform= glGetUniformLocation(ShaderProgramObject, "cameraPos");

}

//matrices
void shaderLoader::loadModelMatrix(GLfloat* modelMatrix)
{
	glUniformMatrix4fv(ModelxUniform, 1, GL_FALSE, modelMatrix);
}
void shaderLoader::loadViewMatrix(Camera cam)
{
	float viewMatrix[16];
	glUniform3f(camPosUniform, cam.getPosition()[0], cam.getPosition()[1], cam.getPosition()[2]);
	createViewMatrix(viewMatrix, cam);
	glUniformMatrix4fv(ViewxUniform, 1, GL_FALSE, viewMatrix);
	
}
void shaderLoader::loadProjectionMatrix(GLfloat* ProjectionMatrix)
{
	glUniformMatrix4fv(ProjxUniform, 1, GL_FALSE, ProjectionMatrix);
}

void shaderLoader::loadSkyViewMatrix(Camera cam)//update later and delete createSky method
{
	float viewMatrix[16];

	createSkyViewMatrix(viewMatrix, cam);
	glUniformMatrix4fv(ViewxUniform, 1, GL_FALSE, viewMatrix);

}
//lights
void shaderLoader::loadLightColor(GLfloat r, GLfloat g, GLfloat b)
{
	glUniform3f(LightColorUniform, r, g, b);
}
void shaderLoader::loadLightPosition(GLfloat* lightPosition)
{
	glUniform4fv(LightPosUniform, 1, lightPosition);
}
void shaderLoader::isLkeyPressed(bool gbLight)
{
	if (gbLight == true)
		glUniform1i(LKeyPressedUniform, 1);
	else
		glUniform1i(LKeyPressedUniform, 0);
}
void shaderLoader::loadShineDamperReflectivity(GLfloat damper, GLfloat reflectivity)
{
	glUniform1f(ShineDamperUniform, damper);
	glUniform1f(ReflectivityUniform,reflectivity);
}

//texture
void shaderLoader::loadModelTextureSampler(GLuint textureModel)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureModel);
	glUniform1i(SamplerUniform, 0);
}
void shaderLoader::loadSecondModelTextureSampler(GLuint SecondTextureModel)
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SecondTextureModel);
	glUniform1i(secondSamplerUniform, 2);
}
void shaderLoader::loadModelNormalSampler(GLuint normalTextureModel)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTextureModel);
	glUniform1i(NormSamplerUniform, 1);
}
void shaderLoader::loadCubeMapSampler(GLuint CubeMapSampler)
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapSampler);
	glUniform1i(cubeMapSamplerUniform, 2);
}
void shaderLoader::cleanUp(void)
{
	glDetachShader(ShaderProgramObject, VertexShaderObject);
	glDetachShader(ShaderProgramObject, FragmentShaderObject);

	glDeleteShader(VertexShaderObject);
	VertexShaderObject = 0;
	glDeleteShader(FragmentShaderObject);
	FragmentShaderObject = 0;

	glDeleteProgram(ShaderProgramObject);
	ShaderProgramObject = 0;

	glUseProgram(0);
}



void Camera::moveDirection(float speed,WPARAM direction)
{
	if (direction == VK_DOWN)
		position[2] += speed;

	if (direction == VK_UP)
		position[2] -= speed;

	if (direction == VK_LEFT)
		position[0] -= speed;

	if (direction == VK_RIGHT)
		position[0] += speed;
}
void Camera::moveRotation(float anglePitch,float angleYaw)
{
	pitch = anglePitch;
	yaw = angleYaw;
}


//MyModel::MyModel(const char* filename, bool anisotropicFilter)
//{
//
//	flagAnisotropic = anisotropicFilter;
//	myObjLoader(filename);
//	createElementData();
//	toVao();
//
//}
void MyModel::myObjLoader(const char* filePath)
{
	fprintf(myModelLog, "\n\nin myObjLoader-------------------------------------------------------------------------------------\n");
	char ch[100];

	vector< unsigned int > vertexIndices;
	vector< unsigned int > texCoordIndices;
	vector< unsigned int > normalIndices;

	vector< vec3 > temp_vertices;
	vector< vec2 > temp_texCoords;
	vector< vec3 > temp_normals;

	unsigned int faceCnt = 0;

	MaterialData storeMtlTextures;

	float a[3];
	int faces[3];
	float x, y, z;

	char path[100] = "res/models/";
	char *finalPath;
	FILE* objFilePtr = NULL;
	FILE* mtlLib = NULL;


	if (fopen_s(&objFilePtr, filePath, "r") != 0)
	{
		MessageBox(NULL, TEXT("OBJ File Can Not Be Loaded\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(myModelLog, "OBJ File Is Successfully Loaded :).\n");
	}


	while (1)
	{


		int res = fscanf_s(objFilePtr, "%s", ch, _countof(ch));
		if (res == EOF)
			break;
		if (strcmp(ch, "mtllib") == 0)
		{

			fscanf_s(objFilePtr, "%s", ch, _countof(ch));

			strcat_s(path, 100, ch);
			finalPath = path;
			fprintf(myModelLog, "mtlFILEname : %s \n", ch);

			if (fopen_s(&mtlLib, finalPath, "r") != 0)
			{
				MessageBox(NULL, TEXT("MTL File Can Not Be Loaded\n"), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);

			}

			continue;
		}

		if (strcmp(ch, "v") == 0)
		{
			vec3 vertex;

			fscanf_s(objFilePtr, "%f %f %f\n", &x, &y, &z);
			vertex = vec3(x, y, z);
			temp_vertices.push_back(vertex);
			a[0] = x;
			a[1] = y;
			a[2] = z;
		}
		else if (strcmp(ch, "vt") == 0)
		{
			vec2 texCoord;

			fscanf_s(objFilePtr, "%f %f \n", &x, &y);
			texCoord = vec2(x, y);
			temp_texCoords.push_back(texCoord);

		}
		else	if (strcmp(ch, "vn") == 0)
		{
			vec3 normal;

			fscanf_s(objFilePtr, "%f %f %f\n", &x, &y, &z);
			normal = vec3(x, y, z);
			temp_normals.push_back(normal);

		}
		else	if (strcmp(ch, "usemtl") == 0)
		{

			while (1)
			{

				if (strcmp(ch, "usemtl") == 0)
				{
					fscanf_s(objFilePtr, "%s", ch, _countof(ch));
					fprintf(myModelLog, "useMtlname : %s \n", ch);
					storeMtlTextures = getMaterial(mtlLib, ch);
					fseek(mtlLib, 0, SEEK_SET);
					if (storeMtlTextures.diffuseTextures)
					{
						texture_model.push_back(storeMtlTextures.diffuseTextures);
					}

					if (storeMtlTextures.bumpTextures)
						texture_modelNormal = storeMtlTextures.bumpTextures;

					if (storeMtlTextures.ambientTexture)
						texture_ambient = storeMtlTextures.ambientTexture;

					fprintf(myModelLog, "texture <vector> size=%d\n", texture_model.size());



					cntMeshOffset.push_back((faceCnt * 3));

					faceCnt = 0;

				}


				int res = fscanf_s(objFilePtr, "%s", ch, _countof(ch));
				if (res == EOF)
					break;



				if (strcmp(ch, "f") == 0)
				{
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					fscanf_s(objFilePtr, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					texCoordIndices.push_back(uvIndex[0]);
					texCoordIndices.push_back(uvIndex[1]);
					texCoordIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					faceCnt += 1;

				}
			}
			cntMeshOffset.push_back((faceCnt * 3));
		}




	}
	fprintf(myModelLog, "FACECOUNT %d \n", faceCnt);
	fprintf(myModelLog, "TOTAL face COUNT size %d \n", vertexIndices.size());

	if (objFilePtr)
	{
		fclose(objFilePtr);
		objFilePtr = NULL;
	}

	if (mtlLib)
	{
		fclose(mtlLib);
		mtlLib = NULL;
	}

	if (temp_texCoords.size() == 0)
	{
		MessageBox(NULL, TEXT("No UVs found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);

	}

	if (temp_normals.size() == 0)
	{
		MessageBox(NULL, TEXT("No Normals found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);

	}
	if (temp_vertices.size() == 0)
	{
		MessageBox(NULL, TEXT("No Vertices found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	fprintf(myModelLog, " x=%d   y=%d   z=%d \n", faces[0], faces[1], faces[2]);
	fprintf(myModelLog, "temp x=%f  temp y=%f temp  z=%f", a[0], a[1], a[2]);
	fprintf(myModelLog, "meshoffset first  %d second  %d \n", cntMeshOffset[0], cntMeshOffset[1]);


	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndexFix = vertexIndices[i];
		unsigned int texCoordIndexFix = texCoordIndices[i];
		unsigned int normalIndexFix = normalIndices[i];
		//modelIndices.push_back(vertexIndexFix - 1);
		modelIndices.push_back(i);
		vec3 vertex = temp_vertices[vertexIndexFix - 1];
		vec2 uv = temp_texCoords[texCoordIndexFix - 1];
		vec3 normal = temp_normals[normalIndexFix - 1];

		vertices.push_back(vertex);
		texCoords.push_back(uv);
		normals.push_back(normal);

	}
	//----------------------------------------------------------------------------------------//


	//Tangents
	myTangentLoader(vertices, texCoords); //tangent loaderrr
	fprintf(myModelLog, "out myObjLoader-------------------------------------------------------------------------------------\n");
}
void MyModel::myTangentLoader(vector<vec3> vertices, vector<vec2> uvs)
{
	//TANGENT

	vec3 DeltaPos1;
	vec3  DeltaPos2;
	vec2  DeltaUV1;
	vec2  DeltaUV2;
	vec3 PerVerTangent;
	float r = 0;
	int cntVert = 0;
	int cntTextCoord = 0;
	int noOfTriangles = vertices.size() / 3;

	for (int i = 0; i < noOfTriangles; i++)
	{

		DeltaPos1 = vertices[cntVert + 1] - vertices[cntVert];
		DeltaPos2 = vertices[cntVert + 2] - vertices[cntVert];

		DeltaUV1 = uvs[cntTextCoord + 1] - uvs[cntTextCoord];
		DeltaUV2 = uvs[cntTextCoord + 2] - uvs[cntTextCoord];

		if ((DeltaUV1[0] * DeltaUV2[1] - DeltaUV1[1] * DeltaUV2[0]) == 0.00)//divide by zero
		{
			r = 1.0f;
		}
		else
		{
			r = 1.0f / (DeltaUV1[0] * DeltaUV2[1] - DeltaUV1[1] * DeltaUV2[0]);
		}
		/*
		for (int i = 0; i < 3; i++)
		{
		DeltaPos1[i] = DeltaPos1[i] * DeltaUV2[1] * r;//LHS
		DeltaPos2[i] = DeltaPos2[i] * DeltaUV1[1] * r;//
		}
		*/
		PerVerTangent = (DeltaPos1 - DeltaPos2);     //LHS-RHS

		for (int i = 0; i < 3; i++)
		{
			PerVerTangent[i] = r * (DeltaPos1[i] * DeltaUV2[1] - DeltaPos2[i] * DeltaUV1[1]);
		}


		for (int i = 0; i < 3; i++)
		{

			tangents.push_back(PerVerTangent);
		}

		cntTextCoord += 3;
		cntVert += 3;
	}

}
MaterialData MyModel::getMaterial(FILE * mtlPtr, char mtlName[])
{
	GLuint LoadTexture(char* textureName, bool anisotropicFilter);
	GLuint loadTGA(char* fileName, bool anisotropicFilter);


	MaterialData mtlTextures;

	char str[100];

	while (1)
	{
		int res = fscanf_s(mtlPtr, "%s", str, _countof(str));
		if (res == EOF)
		{
			break;

		}


		if (strcmp(str, "newmtl") == 0)
		{


			fscanf_s(mtlPtr, "%s", str, _countof(str));
			fprintf(myModelLog, "mtlname : %s\n", str);


			if (strcmp(str, mtlName) == 0)
			{
				fprintf(myModelLog, "mtl : %s\n", mtlName);
				fprintf(myModelLog, "str : %s\n", str);
				while (1)
				{
					int res = fscanf_s(mtlPtr, "%s", str, _countof(str));

					if (res == EOF || (strcmp(str, "newmtl") == 0))
					{
						break;
					}
					if (strcmp(str, "map_Kd") == 0)
					{
						fprintf(myModelLog, "\nin MAP_kd----------------------------\n");
						fgets(str, 100, mtlPtr);
						fprintf(myModelLog, "mtlfilename : %s", str);
						SlashFormat(str);
						fprintf(myModelLog, "mtlfilenameSlashFormat : %s\n", str);
						unsigned int length = (unsigned int)strlen(str);

						if (str[length - 3] == 'b'&& str[length - 2] == 'm'&&str[length - 1] == 'p')
						{
							mtlTextures.diffuseTextures = (LoadTexture(str, flagAnisotropic));
						}
						else if (str[length - 3] == 't' && str[length - 2] == 'g' &&str[length - 1] == 'a')
						{
							mtlTextures.diffuseTextures = (loadTGA(str, flagAnisotropic));

						}
						else {
							fclose(mtlPtr);
							MessageBox(NULL, TEXT("WRONG texture file extension \nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
							exit(0);
						}
						fprintf(myModelLog, "out MAP_kd----------------------------\n\n");
					}
					if (strcmp(str, "map_Bump") == 0)
					{
						fprintf(myModelLog, "in MAP_BUMP----------------------------\n");
						fgets(str, 100, mtlPtr);
						fprintf(myModelLog, "mtlBumpFilename : %s ", str);
						SlashFormat(str);

						unsigned int length = (unsigned int)strlen(str);

						if (str[length - 3] == 'b'&& str[length - 2] == 'm'&&str[length - 1] == 'p')
						{
							mtlTextures.bumpTextures = (LoadTexture(str, flagAnisotropic));
						}
						else if (str[length - 3] == 't' && str[length - 2] == 'g' &&str[length - 1] == 'a')
						{
							mtlTextures.bumpTextures = (loadTGA(str, flagAnisotropic));
						}
						else
						{
							fclose(mtlPtr);
							MessageBox(NULL, TEXT("WRONG texture file extension \nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
							exit(0);
						}
						fprintf(myModelLog, "out MAP_BUMP----------------------------\n\n");
					}

					if (strcmp(str, "map_Ka") == 0)
					{
						fprintf(myModelLog, "in MAP_Ka----------------------------\n");
						fgets(str, 100, mtlPtr);
						fprintf(myModelLog, "mtlAmbientFilename : %s ", str);
						SlashFormat(str);

						unsigned int length = (unsigned int)strlen(str);

						if (str[length - 3] == 'b'&& str[length - 2] == 'm'&&str[length - 1] == 'p')
						{
							mtlTextures.ambientTexture = (LoadTexture(str, flagAnisotropic));
						}
						else if (str[length - 3] == 't' && str[length - 2] == 'g' &&str[length - 1] == 'a')
						{
							mtlTextures.ambientTexture = (loadTGA(str, flagAnisotropic));
						}
						else
						{
							fclose(mtlPtr);
							MessageBox(NULL, TEXT("WRONG texture file extension \nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
							exit(0);
						}
						fprintf(myModelLog, "out MAP_Ka----------------------------\n\n");
					}
				}
			}

		}
	}
	//fprintf(myModelLog, "no of tex ==== %d\n", mtlTextures.diffuseTextures.size());
	return mtlTextures;
}
char* MyModel::SlashFormat(char* pathStr)
{

	//char fileType[3];
	unsigned int length = (unsigned int)strlen(pathStr);
	if (pathStr[0] == ' ')
	{

		for (unsigned int i = 0; i < length; i++)
		{
			pathStr[i] = pathStr[i + 1];
			if (pathStr[i + 1] == '\\')
				pathStr[i + 1] = '/';
			if (pathStr[i] == '\n')
				pathStr[i] = '\0';
		}

	}

	//return fileType;
	return pathStr;

}
void MyModel::createElementData(void)
{

	for (size_t i = 0; i < vertices.size(); i++)//change '<' to '!='
	{

		modelAllArrayData.push_back(vertices[i][0]);
		modelAllArrayData.push_back(vertices[i][1]);
		modelAllArrayData.push_back(vertices[i][2]);
		modelAllArrayData.push_back(texCoords[i][0]);
		modelAllArrayData.push_back(texCoords[i][1]);
		modelAllArrayData.push_back(normals[i][0]);
		modelAllArrayData.push_back(normals[i][1]);
		modelAllArrayData.push_back(normals[i][2]);
		modelAllArrayData.push_back(tangents[i][0]);
		modelAllArrayData.push_back(tangents[i][1]);
		modelAllArrayData.push_back(tangents[i][2]);

	}
	vertices.clear();
	texCoords.clear();
	normals.clear();
	tangents.clear();

}
void MyModel::toVao(void)
{
	GLsizei stride = (sizeof(vec3) * 3) + sizeof(vec2);
	glGenVertexArrays(1, &vao_model);
	glBindVertexArray(vao_model);


	glGenBuffers(1, &vbo_model_array_data);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_model_array_data);
	glBufferData(GL_ARRAY_BUFFER, modelAllArrayData.size() * sizeof(float), (const GLvoid *)&modelAllArrayData[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)20);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)32);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelIndices.size() * sizeof(unsigned int), (const GLvoid *)&modelIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}
void MyModel::cleanUp(void)
{

	if (vao_model)
	{
		glDeleteVertexArrays(1, &vao_model);
		vao_model = 0;

	}
	if (vbo_index)
	{
		glDeleteBuffers(1, &vbo_index);
		vbo_index = 0;
	}
	if (vbo_model_array_data)
	{
		glDeleteBuffers(1, &vbo_model_array_data);
		vbo_model_array_data = 0;
	}
	//texture_model
	for (unsigned int i = 0; i < texture_model.size(); i += 1)
	{
		if (texture_model[i])
		{
			glDeleteTextures(1, &texture_model[i]);
			texture_model[i] = 0;
		}
	}
	texture_model.clear();


	//texture_normal
	if (texture_modelNormal)
	{
		glDeleteTextures(1, &texture_modelNormal);
		texture_modelNormal = 0;
	}
	//modelIndexData
	modelAllArrayData.clear();
	modelIndices.clear();
	cntMeshOffset.clear();


	vertices.clear();
	texCoords.clear();
	normals.clear();
	tangents.clear();


}
