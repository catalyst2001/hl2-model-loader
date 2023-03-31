#include <stdio.h>
#include <stddef.h> //offsetof
#include "smd_loader.h"
#include "glwnd.h"

smd_model model;

INT Width, Height;

void fn_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, -35.0f, -150.0f);
	glRotatef(-90.0f, 1.0, 0.0, 0.0);

	for (int i = 0; i < model.meshes.size(); i++) {
		glVertexPointer(3, GL_FLOAT, sizeof(smd_vertex), ((const char *)model.meshes[i].vertices.data() + offsetof(smd_vertex, vertex)));
		glNormalPointer(GL_FLOAT, sizeof(smd_vertex), ((const char *)model.meshes[i].vertices.data() + offsetof(smd_vertex, normal)));
		glDrawArrays(GL_TRIANGLES, 0, model.meshes[i].vertices.size());
	}
}

void fn_window_resize(HWND hWnd, int width, int height)
{
	if (!height)
		height = 1;

	Width = width;
	Height = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, width / (double)height, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void fn_mousemove(HWND hWnd, int x, int y)
{
}

void fn_mouseclick(HWND hWnd, int x, int y, int button, int state)
{
}

void fn_charinput(HWND hWnd, char symbol)
{
}

//https://docs.microsoft.com/ru-ru/windows/win32/inputdev/wm-keydown
void fn_keydown(HWND hWnd, INT state, WPARAM wparam, LPARAM lparam)
{
	INT key = (INT)wparam;
	if (state == KEY_DOWN) {
		switch (key) {
		case 27:
			PostQuitMessage(0);
			break;
		}
	}
}

//Add this GL functions
void fn_windowcreate(HWND hWnd)
{
	typedef BOOL (__stdcall *wglSwapIntervalEXTPfn)(int interval);
	wglSwapIntervalEXTPfn wglSwapIntervalEXT = (wglSwapIntervalEXTPfn)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(60); //VSYNC

	RECT rct;
	GetClientRect(hWnd, &rct);
	glViewport(0, 0, (GLsizei)rct.right, (GLsizei)rct.bottom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, rct.right / (double)rct.bottom, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

void fn_windowclose(HWND hWnd)
{
	exit(0);
}

int main()
{
	int status = smd_load_model(&model, "model/ct_gign_reference.smd");
	if (status != SMDLDR_STATUS_OK) {
		printf("Loading SMD model failed. Error: %d\n", status);
		return 1;
	}

	printf("\n\n\n\n" "Initializing OpenGL renderer...\n");

	create_window("Test Valve SMD model view before anim", "TestSmdViewer",
		24,					  //Colors bits
		32,					  //Depth bits
		fn_draw,			  //Draw function
		fn_window_resize,	  //Window resize function
		fn_mousemove,		  //Mouse move function
		fn_mouseclick,		  //Mouse click function
		fn_charinput,		  //Char input function
		fn_keydown,			  //Keydown function
		fn_windowcreate,	  //Window create function
		fn_windowclose,		  //Window close function
		0, 0,
		800,				  //Window width
		600);				  //Window height

	return 0;
}