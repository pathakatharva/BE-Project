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

#include"headerClass.h"
#include"myResources.h"

#include"zmath.h"

using namespace cv;
using namespace std;
using namespace zmath;

#define WIN_WIDTH 1200
#define WIN_HEIGHT 720
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#pragma comment(lib,"opencv_world340.lib")
#pragma comment(lib,"opencv_world340d.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

RECT rt;

FILE *gpFile = NULL;


DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;

bool gbFullscreen = false;
bool gbLights = false;

GLfloat gPerspectiveProjectionMatrix[16];
GLfloat gInfiniteProjectionMatrix[16];


GLfloat gAngleYaw = 0.0f;//-150
GLfloat gAnglePitch = 0.0f;
GLfloat gAngleRoll = 0.0f;
GLfloat zPos = 0.0f;

Mat img;
Mat img_threshold;
Mat img_gray;
Mat img_roi;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	void updateAngle(void);
	void initialize(void);
	void uninitialize(void);
	void spin(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGL GLSL");
	bool bDone = false;

	if (fopen_s(&gpFile, "Log/Log.txt", "w") != 0)
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
	wndclass.hIcon = LoadIcon(hInstance ,MAKEINTRESOURCE(IDI_Planet));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_Planet));
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName,
		TEXT("OpenGL FirstGLSL"),
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100,
		WIN_WIDTH, WIN_HEIGHT,
		NULL, NULL,
		hInstance, NULL);

	ghwnd = hwnd;



	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	initialize();

	fprintf(gpFile, "glsl version=%s\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	Camera cam;
	Camera skyBoxCam;
	Camera earthCam;

	shaderLoader skyShader("res/shaders/skyBox.vs", "res/shaders/skyBox.fs");
	shaderLoader starShader("res/shaders/stars.vs", "res/shaders/stars.fs");

	shaderLoader earthShader("res/shaders/earth.vs", "res/shaders/earth.fs");
	earthCloudShader cloudShader("res/shaders/earth_clouds.vs", "res/shaders/earth_clouds.fs");
	earthAtmoShader atmoShader("res/shaders/earth_atmo.vs","res/shaders/earth_atmo.fs");

	earth myEarth("res/models/MyfinalEarth.obj",true);
	skyBox spaceSky;
	stars myStars;
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
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
			char a[40];
			int count = 0;
			bool b = cam1.read(img);
			Rect roi(100, 100, 270, 270);
			img_roi = img(roi);
			cvtColor(img_roi, img_gray, CV_RGB2GRAY);

			GaussianBlur(img_gray, img_gray, Size(19, 19), 0.0, 0);
			//threshold(img_gray, img_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
			threshold(img_gray, img_threshold, 0, 255, THRESH_TOZERO_INV + THRESH_OTSU); //you have modified here!
			/*cvtColor(img_threshold, img_threshold, CV_GRAY2RGB);*/

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
								//gbLights = false;
							}
							else if (count == 2)
							{//strcpy(a, "TWO");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
								//gbLights = false;
							}
							else if (count == 3)
							{	//strcpy(a, "THREE!");
								gbLights = true;
							}
							else if (count == 4)
							{	//strcpy(a, "FOUR");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
							}
							else if (count == 5)
							{	//strcpy(a, "FIVE");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
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
				/*imshow("Original_image", img);
				imshow("Gray_image", img_gray);
				imshow("Thresholded_image", img_threshold);
				imshow("ROI", img_roi);*/
			}
		
	
			//imshow("Original_image", img);
			/*if (!b) {
				MessageBox(NULL, TEXT("ERROR"), TEXT("CAM NOT READING"), MB_OK);
				return -1;
			}*/
			//rendering  here
			updateAngle();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//	skyBoxCam.setPitch(gAnglePitch);
		//	skyBoxCam.setYaw(gAngleYaw);
			cam.setPitch(40.0f);
			cam.setYaw(gAngleYaw);
//			cam.setRoll(gAngleRoll);

		//	cam.setPosition_Z(-zPos);
		//	earthCam.setPosition_Z(-zPos);
	

			myStars.renderStars(starShader, gPerspectiveProjectionMatrix, cam);
			spaceSky.renderSkyBox(skyShader, gInfiniteProjectionMatrix, cam);
			myEarth.renderEarth(earthShader, cloudShader, atmoShader, gPerspectiveProjectionMatrix, earthCam,gbLights);

			SwapBuffers(ghdc);
	
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

	switch (iMsg)
	{
	case WM_CREATE:
		break;
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
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;
		case 0x46://f
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
		case 0x4C: //L

			if (gbLights == false)
				gbLights = true;
			else
				gbLights = false;

			break;

		default:
			break;
		}
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void initialize(void)
{
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
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	
	
	//namedWindow("Original_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Gray_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Thresholded_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("ROI", CV_WINDOW_AUTOSIZE);
	//char a[40];
	//int count = 0;
	//while (1) {
	//	bool b = cam.read(img);
	//	if (!b) {
	//		cout << "ERROR : cannot read" << endl;
	//		return -1;
	//	}
	
	//		if (waitKey(30) == 27) {
	//			return -1;
	//		}

	//	}

	//}
	////////OPENCV CLOSE

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	myIdentity(gPerspectiveProjectionMatrix);
	
	GetClientRect(ghwnd, &rt);
	resize(rt.right, rt.bottom);


}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	myIdentity(gPerspectiveProjectionMatrix);
	myIdentity(gInfiniteProjectionMatrix);

	myPerspective(gPerspectiveProjectionMatrix, 60.0f, aspect, 0.01f, 8000.0f);
	myInfiniteProj(gInfiniteProjectionMatrix, 60.0f, aspect, 0.01f, 8000.0f);

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

void updateAngle(void)
{
	gAnglePitch += 0.01f;
	if (gAnglePitch > 360.0f)
		gAnglePitch -= gAnglePitch;

	gAngleYaw += 0.01f;
	if (gAngleYaw > 360.0f)
		gAngleYaw -= gAngleYaw;

	gAngleRoll += 0.01f;
	if (gAngleRoll > 360.0f)
		gAngleRoll -= gAngleRoll;
		
	zPos += 0.05f;
//	if (zPos > 300.0f)
	//	zPos -= zPos;
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
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

			ShowCursor(TRUE);
	}
}

GLuint LoadTexture(char* textureName, bool anisotropicFilter)
{
	FILE* bmp = NULL;
	tagBITMAPFILEHEADER fh;
	tagBITMAPINFOHEADER infoh;

	GLsizei iWidth, iHeight;
	unsigned char * data;

	GLuint texture;

	ZeroMemory(&fh, sizeof(fh));
	ZeroMemory(&infoh, sizeof(infoh));

	if (fopen_s(&bmp, textureName, "rb") != 0)
	{
		fprintf(gpFile, "texturename in fopen =%s", textureName);
		MessageBox(NULL, TEXT(" BMP file pointer not found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

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
		fclose(bmp);
		MessageBox(NULL, TEXT(" NOT 24BIT BMP format..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	if (fh.bfSize == 0)
	{
		fh.bfSize = (iWidth * iHeight * 3) + fh.bfOffBits;// 3 : one byte for each Red, Green and Blue component //bpp

	}
	fprintf(gpFile, "fh.bfsize =%d", fh.bfSize);


	unsigned int bufferSize = iWidth * iHeight * 3;

	fprintf(gpFile, "\nlength =%d\n", bufferSize);

	//	data = (unsigned char*)malloc(bufferSize);//memory 
	data = new unsigned char[bufferSize];

	if (!data)
	{
		fclose(bmp);
		MessageBox(NULL, TEXT("No texture data found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

	//get data in buffer
	fseek(bmp, fh.bfOffBits, SEEK_SET);//54
	fread(data, bufferSize, 1, bmp);
	//data[bufferSize] = '\0';


	fclose(bmp);

	glGenTextures(1, &texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, data);//BMP 24bit anyway

	//if (anisotropicFilter == true)
	//{
	//	GLfloat fLargest;
	//	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &fLargest);
	//	glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_LOD_BIAS, 0);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, fLargest);
	//	fprintf(gpFile, "fLargest =%f", fLargest);

	//}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateTextureMipmap(texture);//can use glGenerateMipmap(GL_TEXTURE_2D) too..
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
	//free(data);
	delete data;
	data = NULL;

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

	

	if (fopen_s(&tgaLog, "Log/TGAlog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("TGALog File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(tgaLog, "TGALog File Is Successfully Opened.\n");
	}

	fprintf(tgaLog, "texturename in fopen =%s", fileName);

	if (fopen_s(&tgaPtr, fileName, "rb") != 0)
	{
		fprintf(tgaLog, "texturename in fopen =%s", fileName);
		MessageBox(NULL, TEXT(" TGA file pointer not found\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

	fread(&th, sizeof(th), 1, tgaPtr);//TGA header

	fprintf(tgaLog, "\nID_Length=%d \nColorMapType= %d\nImageType= %d\nfirstEntryIndex=%d\ncolorMapLength=%d\ncolorMapEntrySize=%d\nxOrigin=%d\nyOrigin=%d\nImageWidth=%d\nImageHeight =%d\nPixelDepth=%d\nImageDescriptor=%d\n", th.ID_Length, th.ColorMapType, th.ImageType, th.firstEntryIndex[1], th.colorMapLength[1], th.colorMapEntrySize, th.xOrigin, th.yOrigin, th.ImageWidth, th.ImageHeight, th.PixelDepth, th.ImageDescriptor);

	if (th.ImageType != 2 && th.ImageType != 3 || th.ImageWidth <= 0 || th.ImageHeight <= 0)
	{
		fclose(tgaPtr);
		MessageBox(NULL, TEXT(" WRONG TGA format type..\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

	fprintf(tgaLog, "\nID_Length=%d \nColorMapType= %d\nImageType= %d\nfirstEntryIndex=%d\ncolorMapLength=%d\ncolorMapEntrySize=%d\nxOrigin=%d\nyOrigin=%d\nImageWidth=%d\nImageHeight =%d\nPixelDepth=%d\nImageDescriptor=%d\n", th.ID_Length, th.ColorMapType, th.ImageType,th.firstEntryIndex[1],th.colorMapLength[1],th.colorMapEntrySize,th.xOrigin,th.yOrigin,th.ImageWidth,th.ImageHeight,th.PixelDepth,th.ImageDescriptor);

	if (th.PixelDepth == 8)
	{
		InternalFormatType = GL_RED;
		EnumFormatType = GL_RED;
	}
	else if (th.PixelDepth == 24)
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
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormatType, iWidth, iHeight, 0, EnumFormatType, GL_UNSIGNED_BYTE,(const void*) data);

	//if (anisotropicFilter == true)
	//{
	//	GLfloat fLargest;
	//	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &fLargest);
	//	glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_LOD_BIAS, 0);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, fLargest);
	//	fprintf(gpFile, "fLargest =%f", fLargest);

	//}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateTextureMipmap(texture);//can use glGenerateMipmap(GL_TEXTURE_2D) too..
									 //glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);


	fclose(tgaLog);
	tgaLog = NULL;
	return(texture);

	delete data;
	data = NULL;
}
