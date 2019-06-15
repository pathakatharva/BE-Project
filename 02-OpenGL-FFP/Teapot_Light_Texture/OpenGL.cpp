#include<Windows.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#include<stdio.h>


#include<opencv2\imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include<opencv\cv.h>
#include <time.h>
#include<vector>

#include"OGL.h"
#include"resource.h"

using namespace cv;
using namespace std;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

Mat img;
Mat img_threshold;
Mat img_gray;
Mat img_roi;

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"GDI32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#pragma comment(lib,"opencv_world340.lib")
#pragma comment(lib,"opencv_world340d.lib")

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

HWND ghwnd;
HDC ghdc;
HGLRC ghrc;

GLuint Texture_Glass;

GLfloat angleModel;
GLfloat LightAmbient[]={0.5f,0.5f,0.5f,1.0f};
GLfloat LightDiffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat LightPosition[]={0.0f,0.0f,2.0f,1.0f};

DWORD dwStyle;

WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};

bool gbFullscreen=false;
bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;
bool gbIsTextured=false;
bool gbLight=false;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow)
{
	void initialize(void);
	void display(void);
	void uninitialize(void);
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("OpenGL Texture");
	bool bDone=false;

	wndclass.cbSize=sizeof(WNDCLASSEX);
	wndclass.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.lpszClassName=szClassName;
	wndclass.lpszMenuName=NULL;
	wndclass.lpfnWndProc=WndProc;
	wndclass.hInstance=hInstance;
	wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);

	RegisterClassEx(&wndclass);

	hwnd=CreateWindowEx(WS_EX_APPWINDOW,szClassName,TEXT("OPENGL TEXTURE"),WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,0,0,WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	if(hwnd==NULL)
	{
		MessageBox(hwnd,TEXT("CREATE WINDOW FAILED"),TEXT("ERROR"),MB_OK);
		exit(0);
	}

	ghwnd=hwnd;

	initialize();

	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	///////OPENCV START
	VideoCapture cam1(0);
	if (!cam1.isOpened()) {
		MessageBox(NULL, TEXT("ERROR"), TEXT("CAM NOT OPENED"), MB_OK);
		return -1;
	}

	while(bDone==false)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
				bDone=true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			display();
			if(gbActiveWindow==true)
			{
				if(gbEscapeKeyIsPressed==true)
					bDone=true;
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
								//gbLights = true;
								gbIsTextured = true;
							}
							else if (count == 4)
							{	//strcpy(a, "FOUR");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
								gbLight = false;

							}
							else if (count == 5)
							{	//strcpy(a, "FIVE");
								//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
								gbIsTextured = false;
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

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	void display(void);
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	switch(iMsg)
	{
	case WM_ACTIVATE:
		if(HIWORD(wParam)==0)
			gbActiveWindow=true;
		else
			gbActiveWindow=false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			if(gbEscapeKeyIsPressed==false)
				gbEscapeKeyIsPressed=true;
			break;
		case 0x46:
			if(gbFullscreen==false)
			{
				ToggleFullscreen();
				gbFullscreen=true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen=false;
			}
			break;
		case 0x4C:
			if(gbLight==false)
			{
				glEnable(GL_LIGHTING);
				gbLight=true;
			}
			else
			{
				glDisable(GL_LIGHTING);
				gbLight=false;
			}
			break;
		case 0x54:
			if(gbIsTextured==false)
			{
				glEnable(GL_TEXTURE_2D);
				gbIsTextured = true;
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				gbIsTextured = false;
			}
			break;
		default:
			break;
		}
	break;
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
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void initialize(void)
{
	void resize(int,int);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=32;
	pfd.cRedBits=8;
	pfd.cGreenBits=8;
	pfd.cBlueBits=8;
	pfd.cAlphaBits=8;
	pfd.cDepthBits=32;

	ghdc=GetDC(ghwnd);

	iPixelFormatIndex=ChoosePixelFormat(ghdc,&pfd);
	if(iPixelFormatIndex==0)
	{
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd)==false)
	{
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	ghrc=wglCreateContext(ghdc);
	if(ghrc==NULL)
	{
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	if(wglMakeCurrent(ghdc,ghrc)==false)
	{
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	int LoadGLTextures(GLuint*,TCHAR[]);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	LoadGLTextures(&Texture_Glass,MAKEINTRESOURCE(IDB_BITMAP1));
	glBindTexture(GL_TEXTURE_2D,Texture_Glass);
	glEnable(GL_TEXTURE_2D);
	
	glLightfv(GL_LIGHT1,GL_AMBIENT,LightAmbient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,LightDiffuse);
	glLightfv(GL_LIGHT1,GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);

	resize(WIN_WIDTH,WIN_HEIGHT);
}

void ToggleFullscreen(void)
{
	MONITORINFO mi={sizeof(MONITORINFO)};
	if(gbFullscreen==false)
	{
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if(GetWindowPlacement(ghwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right-mi.rcMonitor.left,mi.rcMonitor.bottom-mi.rcMonitor.top,SWP_NOZORDER|SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle|WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_FRAMECHANGED);
		
		ShowCursor(TRUE);
	}
}

void display(void)
{
	void spin(void);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if (gbIsTextured == false)
	{
		glEnable(GL_TEXTURE_2D);
		//gbIsTextured = true;
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		//gbIsTextured = false;
	}

	if (gbLight == false)
	{
		glEnable(GL_LIGHTING);
		//gbLight = true;
	}
	else
	{
		glDisable(GL_LIGHTING);
		//gbLight = false;
	}

	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-1.5f);
	glRotatef(angleModel,0.0f,1.0f,0.0f);
	glBegin(GL_TRIANGLES);
	for(int i=0;i<sizeof(face_indicies)/sizeof(face_indicies[0]);i++)
	{
		for(int j=0;j<3;j++)
		{
			int vi=face_indicies[i][j];
			int ni=face_indicies[i][j+3];
			int ti=face_indicies[i][j+6];
			glNormal3f(normals[ni][0],normals[ni][1],normals[ni][2]);
			glTexCoord2f(textures[ti][0],textures[ti][1]);
			glVertex3f(vertices[vi][0],vertices[vi][1],vertices[vi][2]);
		}
	}
	glEnd();
	
	spin();

	SwapBuffers(ghdc);
}

void resize(int width,int height)
{
	void spin(void);
	if(height==0)
		height=1;
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void spin(void)
{
	angleModel=angleModel+0.1f;
	if(angleModel>=360.0f)
		angleModel=angleModel-360.0f;

}

int LoadGLTextures(GLuint *texture,TCHAR imageResourceId[])
{
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus=FALSE;

	glGenTextures(1,texture);
	hBitmap=(HBITMAP)LoadImage(GetModuleHandle(NULL),imageResourceId,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	if(hBitmap)
	{
		iStatus=TRUE;
		GetObject(hBitmap,sizeof(bmp),&bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		glBindTexture(GL_TEXTURE_2D,*texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,3,bmp.bmWidth,bmp.bmHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,bmp.bmBits);
		DeleteObject(hBitmap);
	}
	return(iStatus);
}

void uninitialize(void)
{
	if(gbFullscreen==true)
	{
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle|WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;

	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;

	DestroyWindow(ghwnd);
}