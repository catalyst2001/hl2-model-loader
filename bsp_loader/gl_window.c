#include "gl_window.h"

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProcA(hwnd, msg, wParam, lParam);
}

bool gl_window_create(gl_window_t *p_dstwindow, const char *p_title, int width, int height)
{
	WNDCLASSA wndclass;
	memset(&wndclass, 0, sizeof(wndclass));
	wndclass.hCursor = LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW));
	wndclass.hInstance = GetModuleHandleA(NULL);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpszClassName = WINDOW_CLASS;
	wndclass.lpfnWndProc = wnd_proc;
	if (!RegisterClassA(&wndclass)) {
		printf("Failed to register window class!\n");
		return false;
	}

	int x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	int y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
	p_dstwindow->h_window = CreateWindowExA(0, WINDOW_CLASS, p_title, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		x, y, width, height,
		HWND_DESKTOP,
		(HMENU)NULL,
		NULL, NULL);

	if (!p_dstwindow->h_window) {
		printf("Failed to create window!\n");
		return false;
	}

	p_dstwindow->h_devctx = GetDC(p_dstwindow->h_window);
	if (!p_dstwindow->h_devctx) {
		printf("Failed to capture device context\n");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.


	return true;
}

void gl_delete_window(gl_window_t *p_dstwindow)
{
}

void gl_window_poll_events(gl_window_t *p_glwindow)
{

}
