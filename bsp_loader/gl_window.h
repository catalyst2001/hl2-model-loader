#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdbool.h>
#pragma comment(lib  "opengl32.lib")
#pragma comment(lib  "glu32.lib")

#define WINDOW_CLASS "GlWindow"

typedef struct gl_window_s {
	HWND h_window;
	HDC h_devctx;
	HGLRC h_glctx;
} gl_window_t;

bool gl_window_create(gl_window_t *p_dstwindow, const char *p_title, int width, int height);
void gl_delete_window(gl_window_t *p_dstwindow);
void gl_window_poll_events(gl_window_t *p_glwindow);

