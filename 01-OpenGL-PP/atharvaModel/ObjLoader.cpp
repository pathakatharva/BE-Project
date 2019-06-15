#include<Windows.h> 
#include<stdio.h>
#include<gl/glew.h>
//#include<gl/GLU.h>
//#include<gl/GL.h>

#include<opencv2\imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include<opencv\cv.h>
#include <time.h>

using namespace cv;
using namespace std;

#include"body.h"
#include"glass.h"

#include "pmath.h"
#define WIN_WIDTH 1200
#define WIN_HEIGHT 720

Mat img;
Mat img_threshold;
Mat img_gray;
Mat img_roi;

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#pragma comment(lib,"opencv_world340.lib")
#pragma comment(lib,"opencv_world340d.lib")

enum
{
	LOC_ATTRIBUTE_VERTEX = 0,
	LOC_ATTRIBUTE_TANGENT,
	LOC_ATTRIBUTE_NORMAL,
	LOC_ATTRIBUTE_TEXTURE0,
};
struct MyObjStruct { vector<pmath::vec3> vertices; vector<pmath::vec2> texCoords; vector<pmath::vec3> normals; vector<pmath::vec3> tangents; };
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

FILE *gpFile = NULL;


GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_model;
GLuint gVbo_model_vertices;
GLuint gVbo_model_textureCoord;
GLuint gVbo_model_normal;
GLuint gVbo_model_tangent;


GLuint gTexture_model;
GLuint gTexture_modelNormal;


GLuint gModelxUniform;
GLuint gMVxUniform;
GLuint gProjxUniform;
GLuint gMVPUniform;

GLuint gSamplerUniform;
GLuint gNormSamplerUniform;

GLuint gLightColorUniform;
GLuint gLightPosUniform;
GLuint	gKdUniform;
GLuint gLKeyPressedUniform;
GLuint gShineDamperUniform;
GLuint gReflectivityUniform;

GLfloat gAnglePyramid = 0.0f;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLfloat gPerspectiveProjectionMatrix[16];

bool gbLight;
bool gbAnimate;

MyObjStruct model;

vector<pmath::vec3> finalVertices;
vector<pmath::vec2> finalTexCoords;
vector<pmath::vec3> finalNormals;

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	void NeoRender(void);
	void initialize(void);
	void uninitialize(void);
	void spin(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGL GLSL");
	bool bDone = false;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}


	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL FirstGLSL"),
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL, NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;



	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	initialize();

	///////OPENCV START
	VideoCapture cam1(0);
	if (!cam1.isOpened()) {
		MessageBox(NULL, TEXT("ERROR"), TEXT("CAM NOT OPENED"), MB_OK);
		return -1;
	}

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//rendering  here

			NeoRender();
			//spin();

			if (gbAnimate == true)
				spin();

			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
			char a[40];
			int count = 0;
			bool b = cam1.read(img);
			Rect roi(340, 100, 270, 270);
			img_roi = img(roi);
			cvtColor(img_roi, img_gray, CV_RGB2GRAY);

			GaussianBlur(img_gray, img_gray, Size(19, 19), 0.0, 0);
			threshold(img_gray, img_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

			vector<vector<Point> >contours;
			vector<Vec4i>hierarchy;
			findContours(img_threshold, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
			if (contours.size() > 0) {
				size_t indexOfBiggestContour = -1;
				size_t sizeOfBiggestContour = 0;

				for (size_t i = 0; i < contours.size(); i++) {
					if (contours[i].size() > sizeOfBiggestContour) {
						sizeOfBiggestContour = contours[i].size();
						indexOfBiggestContour = i;
					}
				}
				vector<vector<int> >hull(contours.size());
				vector<vector<Point> >hullPoint(contours.size());
				vector<vector<Vec4i> >defects(contours.size());
				vector<vector<Point> >defectPoint(contours.size());
				vector<vector<Point> >contours_poly(contours.size());
				Point2f rect_point[4];
				vector<RotatedRect>minRect(contours.size());
				vector<Rect> boundRect(contours.size());
				for (size_t i = 0; i < contours.size(); i++) {
					if (contourArea(contours[i])>5000) {
						convexHull(contours[i], hull[i], true);
						convexityDefects(contours[i], hull[i], defects[i]);
						if (indexOfBiggestContour == i) {
							minRect[i] = minAreaRect(contours[i]);
							for (size_t k = 0; k < hull[i].size(); k++) {
								int ind = hull[i][k];
								hullPoint[i].push_back(contours[i][ind]);
							}
							count = 0;

							for (size_t k = 0; k < defects[i].size(); k++) {
								if (defects[i][k][3]>13 * 256) {
									/*   int p_start=defects[i][k][0];   */
									int p_end = defects[i][k][1];
									int p_far = defects[i][k][2];
									defectPoint[i].push_back(contours[i][p_far]);
									circle(img_roi, contours[i][p_end], 3, Scalar(0, 255, 0), 2);
									count++;
								}

							}

							if (count == 1)
							{//strcpy(a, "ONE");
							 //glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
							 
								gbLight = true;
							}
							else if (count == 2)
							{//strcpy(a, "TWO");
							 //glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
							 //gbLights = false;
							}
							else if (count == 3)
							{	//strcpy(a, "THREE!");
								gbAnimate = true;
							}
							else if (count == 4)
							{	//strcpy(a, "FOUR");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
								//gbLight = false;
							}
							else if (count == 5)
							{	//strcpy(a, "FIVE");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
								gbAnimate = false;
							}
							else
							{	//strcpy(a, "Welcome!");
							}
							//putText(img, a, Point(70, 70), CV_FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 0, 0), 2, 8, false);
							drawContours(img_threshold, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
							drawContours(img_threshold, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
							drawContours(img_roi, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
							approxPolyDP(contours[i], contours_poly[i], 3, false);
							boundRect[i] = boundingRect(contours_poly[i]);
							rectangle(img_roi, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
							minRect[i].points(rect_point);
							for (size_t k = 0; k < 4; k++) {
								line(img_roi, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
							}

						}
					}

				}
				imshow("Original_image", img);
				imshow("Gray_image", img_gray);
				imshow("Thresholded_image", img_threshold);
				imshow("ROI", img_roi);
			}

		}

	}
	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	static bool bIsLKeyPressed = false;
	static bool bIsAKeyPressed = false;

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case  VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x41:
			if (bIsAKeyPressed == false)
			{
				gbAnimate = true;
				bIsAKeyPressed = true;
			}
			else
			{
				gbAnimate = false;
				bIsAKeyPressed = false;
			}
			break;
		case 0x4C:
			if (bIsLKeyPressed == false)
			{
				gbLight = true;
				bIsLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				bIsLKeyPressed = false;
			}
			break;
		case 0x46:
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;

		default:
			break;


		}
	case WM_LBUTTONDOWN:
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}



void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	myPerspective(gPerspectiveProjectionMatrix, 45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 5000.0f);
}

void makePFD(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
}

void uninitialize(void)
{
	// PYRAMID
	if (gVao_model)
	{
		glDeleteVertexArrays(1, &gVao_model);
		gVao_model = 0;
	}

	if (gVbo_model_vertices)
	{
		glDeleteBuffers(1, &gVbo_model_vertices);
		gVbo_model_vertices = 0;
	}

	if (gVbo_model_textureCoord)
	{
		glDeleteBuffers(1, &gVbo_model_textureCoord);
		gVbo_model_textureCoord = 0;
	}

	if (gVbo_model_normal)
	{
		glDeleteBuffers(1, &gVbo_model_normal);
		gVbo_model_normal = 0;
	}





	if (gTexture_model)
	{
		glDeleteTextures(1, &gTexture_model);
		gTexture_model = 0;
	}

	if (gTexture_modelNormal)
	{
		glDeleteTextures(1, &gTexture_modelNormal);
		gTexture_modelNormal = 0;
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

GLchar* load_Shader(char const* path)
{
	char* buffer = 0;
	long length=0;
	FILE *f;
	fopen_s(&f, path, "rb");

	if (f)
	{


		fseek(f, 0, SEEK_END);
		length = ftell(f);
		rewind(f);
		//fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc((length + 1) * sizeof(char));
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

void spin(void)
{
	// code
	gAnglePyramid = gAnglePyramid + 0.5f;
	if (gAnglePyramid >= 360.0f)
		gAnglePyramid = gAnglePyramid - 360.0f;


}

int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[])
{
	// variable declarations
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;

	// code
	glGenTextures(1, texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);
	}
	return(iStatus);
}
void ToggleFullscreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	GLuint LoadTexture(char*);
	GLuint loadTGA(char* fileName, bool anisotropicFilter);
	//MyObjStruct myObjLoader(const char*);
//	vector<pmath::vec3> myTangentLoader(vector<pmath::vec3>, vector<pmath::vec2>);
	int LoadGLTextures(GLuint *, TCHAR[]);
	GLchar* load_Shader(char const*);
	void uninitialize(void);
	void makePFD(void);
	void resize(int, int);

	makePFD();

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	//vertex shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//LOAD VERTEX SHADER
	const GLchar *VertexShaderSourceCode = load_Shader("NormalMapModel.vs");

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&VertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//Fragment shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode = load_Shader("NormalMapModel.fs");

	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//shader Program
	gShaderProgramObject = glCreateProgram();
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject, LOC_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, LOC_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(gShaderProgramObject, LOC_ATTRIBUTE_TEXTURE0, "vTex0Coord");
	glBindAttribLocation(gShaderProgramObject, LOC_ATTRIBUTE_TANGENT, "vTangent");

	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// UNIFORMS
	gModelxUniform = glGetUniformLocation(gShaderProgramObject, "uModelX");
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "uMVPx");
	gMVxUniform = glGetUniformLocation(gShaderProgramObject, "uModelViewX");
	gProjxUniform = glGetUniformLocation(gShaderProgramObject, "uProjX");

	gLightColorUniform = glGetUniformLocation(gShaderProgramObject, "uLightColor");
	gLightPosUniform = glGetUniformLocation(gShaderProgramObject, "uLightPos");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "uKd");
	gShineDamperUniform= glGetUniformLocation(gShaderProgramObject, "uShineDamper");
	gReflectivityUniform= glGetUniformLocation(gShaderProgramObject, "uReflectivity");

	gSamplerUniform = glGetUniformLocation(gShaderProgramObject, "tSampler");
	gNormSamplerUniform = glGetUniformLocation(gShaderProgramObject, "normSampler");
	
	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "Lkey");

	//triangle

	const GLfloat pyramidVertices[] =
	{
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f,-1.0f,
		-1.0f, -1.0f, -1.0f,

		1.0f, -1.0f,-1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

	};
	int nOfTriangles = (sizeof(pyramidVertices) / sizeof(*pyramidVertices)) / 9;

	const GLfloat pyramidTexcoords[] =
	{
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	const GLfloat pyramidNormals[] =
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

	};
	for (unsigned int i = 0; i != 3704; i++)
	{

		for (unsigned int j = 0; j < 3; j++)
		{
			unsigned int vertexIndexFix = (unsigned int)face_indicies[i][j];
			unsigned int normalIndexFix = (unsigned int)face_indicies[i][j + 3];
			unsigned int texCoordIndexFix = (unsigned int)face_indicies[i][j + 6];


			pmath::vec3 vertex;
			vertex[0] = vertices[vertexIndexFix ][0];
			vertex[1] = vertices[vertexIndexFix ][1];
			vertex[2] = vertices[vertexIndexFix ][2];

			pmath::vec2 uv;
			uv[0] = textures[texCoordIndexFix ][0];
			uv[1] = textures[texCoordIndexFix ][1];

			pmath::vec3 normal;
			normal[0] = normals[normalIndexFix ][0];
			normal[1] = normals[normalIndexFix ][1];
			normal[2] = normals[normalIndexFix ][2];


			finalVertices.push_back(vertex);
			finalTexCoords.push_back(uv);
			finalNormals.push_back(normal);
		}

	}
	
	
	


	//model= myObjLoader("MyfinalEarth.obj");
	//model.tangents =  myTangentLoader(model.vertices, model.texCoords);
	//mkc=myTrialVectorLoader("own.obj");

	//fprintf(gpFile, "vervec size=%d", model.vertices.size());

	

	glGenVertexArrays(1, &gVao_model);
	glBindVertexArray(gVao_model);

	glGenBuffers(1, &gVbo_model_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_model_vertices);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, finalVertices.size() * sizeof(pmath::vec3), &finalVertices[0], GL_STATIC_DRAW);
	

	glVertexAttribPointer(LOC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(LOC_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVbo_model_textureCoord);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_model_textureCoord);
	glBufferData(GL_ARRAY_BUFFER, finalTexCoords.size() * sizeof(pmath::vec2),&finalTexCoords[0], GL_STATIC_DRAW);

	glVertexAttribPointer(LOC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(LOC_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_model_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_model_normal);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, finalNormals.size() * sizeof(pmath::vec3),&finalNormals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(LOC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(LOC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, &gVbo_model_tangent);
	//glBindBuffer(GL_ARRAY_BUFFER, gVbo_model_tangent);
	////glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, model.tangents.size() * sizeof(pmath::vec3), &model.tangents[0], GL_STATIC_DRAW);

	//glVertexAttribPointer(LOC_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//glEnableVertexAttribArray(LOC_ATTRIBUTE_TANGENT);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	

	glBindVertexArray(0);
	//square

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	//LoadGLTextures(&gTexture_model, MAKEINTRESOURCE(IDB_barrel));
	//LoadGLTextures(&gTexture_modelNormal, MAKEINTRESOURCE(IDB_barrelNormal));
	gTexture_model = loadTGA("body.tga", false);
	//gTexture_modelNormal = LoadTexture("barrelNormal.bmp");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	myIdentity(gPerspectiveProjectionMatrix);

	gbLight = false;
	gbAnimate = false;

	resize(WIN_WIDTH, WIN_HEIGHT);
}



void NeoRender(void)
{
	//variable declaarations
	float modelViewMatrix[16];
	float modelViewProjectionMatrix[16];
	float rotationMatrix[16];
	float modelMatrix[16];
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//lights
	glUniform3f(gLightColorUniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(gKdUniform, 0.7f, 0.7f, 0.7f);

	float lightPosition[] = { -500.0f, 500.0f, 500.0f, 1.0f };
	glUniform4fv(gLightPosUniform, 1, (GLfloat *)lightPosition);

	glUniform1f(gShineDamperUniform, 100.0f);
	glUniform1f(gReflectivityUniform, 1.0f);

	if (gbLight == true)
	{
		glUniform1i(gLKeyPressedUniform, 1);

	}
	else
	{
		glUniform1i(gLKeyPressedUniform, 0);
	}

	// triangle
	myIdentity(modelMatrix);
	myIdentity(modelViewMatrix);
	myIdentity(modelViewProjectionMatrix);
	myIdentity(rotationMatrix);
	
	myTranslate(modelViewMatrix, 0.0f, 0.0f, -500.0f);
	//myRotate(modelViewMatrix,90.0f, 0.0f, 0.0f, 1.0f);
	//myRotate(modelViewMatrix, 90.0f, 1.0f, 0.0f, 0.0f);
	myRotate(rotationMatrix, -90.0f, 0.0f, 0.0f, 1.0f);
	multiplyMatrices1(modelViewMatrix, rotationMatrix);
	myIdentity(rotationMatrix);
	myRotate(rotationMatrix, -90.0f, 0.0f, 1.0f, 0.0f);
	multiplyMatrices1(modelViewMatrix, rotationMatrix);
	myIdentity(rotationMatrix);
	myRotate(rotationMatrix, gAnglePyramid, 0.0f, 1.0f, 0.0f);
	
	multiplyMatrices1(modelViewMatrix, rotationMatrix);
	multiplyMatrices2(modelViewProjectionMatrix, modelViewMatrix, gPerspectiveProjectionMatrix);

	glUniformMatrix4fv(gModelxUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniformMatrix4fv(gMVxUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(gProjxUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture_model);
	glUniform1i(gSamplerUniform, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gTexture_modelNormal);
	glUniform1i(gNormSamplerUniform, 1);


	glBindVertexArray(gVao_model);

	glDrawArrays(GL_TRIANGLES, 0,3704*9);



	glBindVertexArray(0);
	//square


	//
	glUseProgram(0);

	SwapBuffers(ghdc);
}



GLuint LoadTexture(char* textureName)
{
	FILE* bmp = NULL;
	tagBITMAPFILEHEADER fh;
	tagBITMAPINFOHEADER infoh;
	GLsizei iWidth, iHeight;
	unsigned char * data;
	GLuint texture;
	fprintf(gpFile, "texturename =%s", textureName);
	if (fopen_s(&bmp,textureName, "rb") != 0)
	{
		fprintf(gpFile, "texturename in fopen =%s", textureName);
		MessageBox(NULL, TEXT(" BMP file pointer not found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	fprintf(gpFile, "texturename out fopen =%s", textureName);

	fread(&fh, sizeof(fh), 1, bmp);
	if (fh.bfType != 0x4D42)
	{
		MessageBox(NULL, TEXT(" WRONG BMP format type..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

	fread(&infoh, sizeof(infoh), 1, bmp);
	iWidth = infoh.biWidth;
	iHeight = infoh.biHeight;
	fprintf(gpFile, "\nbitmap width=%d\nbitmap height=%d\n", iWidth, iHeight);

	if (infoh.biBitCount != 24)
	{

		MessageBox(NULL, TEXT(" NOT 24BIT BMP format..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	if (fh.bfSize == 0)
	{
		fh.bfSize = (iWidth * iHeight * 3) + fh.bfOffBits;// 3 : one byte for each Red, Green and Blue component

	}
	fprintf(gpFile, "bfsize =%d", fh.bfSize);
	unsigned int bufferSize = (fh.bfSize - fh.bfOffBits);


	fprintf(gpFile, "\nlength =%d", bufferSize);

	data = (unsigned char*)malloc(bufferSize);//memory 

											  //get data in buffer
	fseek(bmp, fh.bfOffBits, SEEK_SET);
	fread(data, 1, bufferSize, bmp);
	data[bufferSize] = '\0';

	fprintf(gpFile, "\n%x\n", data[23]);
	if (!data)
	{
		MessageBox(NULL, TEXT("No texture data found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	fclose(bmp);

	glGenTextures(1, &texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
	free(data);

}

char* SlashFormat(char* pathStr)
{
	if (pathStr[0] = ' ')
	{
		unsigned int length = (unsigned int)strlen(pathStr);
		for (unsigned int i = 0; i < length; i++)
		{
			pathStr[i] = pathStr[i + 1];
			if (pathStr[i + 1] == '\\')
				pathStr[i + 1] = '/';
		}
		pathStr[(unsigned int)strlen(pathStr)-1] = '\0';

	}
	fprintf(gpFile, "bmp path : %d \n", (unsigned int)strlen(pathStr));
	return pathStr;
}

GLuint loadTGA(char* fileName, bool anisotropicFilter) {

	FILE* tgaPtr = NULL;
	FILE* tgaLog = NULL;

	GLsizei iWidth, iHeight;

	GLuint texture;
	unsigned char* data;

	GLenum InternalFormatType;
	GLenum EnumFormatType;

	TGA_Header th;
	ZeroMemory(&th, sizeof(th));

	fopen_s(&tgaLog, "TGAlog.txt", "w");

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

	fprintf(tgaLog, "\nID_Length=%d \nColorMapType= %d\nImageType= %d\nfirstEntryIndex=%d\ncolorMapLength=%d\ncolorMapEntrySize=%d\nxOrigin=%d\nyOrigin=%d\nImageWidth=%d\nImageHeight =%d\nPixelDepth=%d\nImageDescriptor=%d\n", th.ID_Length, th.ColorMapType, th.ImageType,th.firstEntryIndex[1],th.colorMapLength[1],th.colorMapEntrySize,th.xOrigin,th.yOrigin,th.ImageWidth,th.ImageHeight,th.PixelDepth,th.ImageDescriptor);

	if (th.PixelDepth == 24)
	{
		InternalFormatType = GL_RGB;
		EnumFormatType = GL_BGR;
	}
	else if (th.PixelDepth == 32)
	{
		InternalFormatType = GL_RGBA;
		EnumFormatType = GL_BGRA;
	}
	else
	{
		fclose(tgaPtr);
		MessageBox(NULL, TEXT(" WRONG TGA format type..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

	iWidth = th.ImageWidth;
	iHeight = th.ImageHeight;

	GLuint bytesPerPixel = th.PixelDepth / 8;
	GLuint bufferSize = th.ImageWidth * th.ImageHeight * bytesPerPixel;

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

	glGenTextures(1, &texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormatType, iWidth, iHeight, 0, EnumFormatType, GL_UNSIGNED_BYTE, data);

	if (anisotropicFilter == true)
	{
		GLfloat fLargest;
		glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_LOD_BIAS, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
	//	fprintf(gpFile, "fLargest =%f", fLargest);

	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateTextureMipmap(texture);//can use glGenerateMipmap(GL_TEXTURE_2D) too..
									 //glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	if(tgaLog)
	fclose(tgaLog);
	tgaLog = NULL;
	return(texture);

	delete data;
	data = NULL;
}



