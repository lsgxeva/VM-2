#include <config.h>

#ifdef ENABLE_WINAPI

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>

#define WIDTH 80
#define HEIGHT 25

static int x,y;
static char buf[HEIGHT][WIDTH];

static int key;

static void winapi_repaint() {
	int i;
	HWND hWnd = GetActiveWindow();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, 0x00FFFFFF);
	for (i=0; i<HEIGHT; i++) {
		TextOut(hdc, 0, i*15, buf[i], strlen(buf[i]));
	}
	EndPaint(hWnd, &ps);
}

void winapi_putchar(char c) {
	if (c == '\n') {
		y++;
		x=0;
	} else if (c == '\r') x=0;
	else {
		buf[y][x]=c;
		x++;
		if (x>WIDTH) {
			x=0;
			y++;
		}
		InvalidateRect(GetActiveWindow(), 0, 1);
	}
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
		key = wParam;
		break;
	case WM_KEYUP:
		key = wParam;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
	case WM_PAINT:
		winapi_repaint();
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void winapi_printf(char *f,...) {
	va_list args;
	char *s;
	size_t i;
	va_start(args,f);
	s = malloc(strlen(f)+sizeof(args));
	vsprintf(s,f,args);
	for (i=0; i<strlen(s); i++) winapi_putchar(s[i]);
	va_end(args);
}

void winapi_update() {
	MSG msg = {0};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int winapi_getch() {
	while (!key) winapi_update();
	return MapVirtualKey(key,0);
}

void winapi_init() {
	WNDCLASS windowclass = {0};
	HWND window;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	windowclass.lpfnWndProc = (WNDPROC)WndProc;
	windowclass.style = CS_HREDRAW | CS_VREDRAW;
	windowclass.hInstance = hInstance;
	windowclass.lpszClassName = TEXT("Main window");
	windowclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowclass.hbrBackground = GetStockObject(BLACK_BRUSH);
	RegisterClass(&windowclass);
	window = CreateWindow(TEXT("Main window"), TEXT(""), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);
	if (!window) exit(2);
}

#endif
