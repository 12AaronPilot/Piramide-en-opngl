#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include "WinApplication.h"

WinApplication *WinApplication::pApp = 0;

WinApplication::WinApplication(void)
	: CLASSNAME(TEXT("DXTALLER"))
{
	assert( pApp == 0 );
	pApp = this;
	appInstance = 0;
	appWindow = 0;
}

WinApplication::~WinApplication(void)
{
	pApp = 0;
	appInstance = 0;
	appWindow = 0;
}

int WinApplication::InternalWinMain()
{	
	appInstance = GetModuleHandle(NULL);

	MSG msg;
	bool done = false;

	if (FAILED(InitWindow()))	{ return 0; }
	if (!AppBegin()) { return 0; }

	while(1) {
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			if( msg.message == WM_QUIT )
				break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else {
			AppUpdate();
		}
	}

	AppEnd();
	return (int)msg.wParam;
}

void WinApplication::CloseWindow()
{
	if(appWindow)
	{
		DestroyWindow(appWindow);
		appWindow = 0;
	}
	if(appInstance)
	{
		UnregisterClass(CLASSNAME, appInstance);
		appInstance = 0;
	}
}

HRESULT WinApplication::InitWindow()
{
	WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX); 
    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = appInstance;
    wcex.hIcon          = LoadIcon( NULL, IDI_APPLICATION );
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = CLASSNAME;
    wcex.hIconSm        = LoadIcon( NULL, IDI_APPLICATION );

    if( !RegisterClassEx(&wcex) )
        return E_FAIL;

    // Create window
	// ver codigo para fullscreen en el GLUS(glus_windows.c)
    
	unsigned int  w, h;
	GetWindowDims( w, h );
	DWORD		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD		dwStyle = WS_OVERLAPPEDWINDOW;

    RECT rc = { 0, 0, static_cast<long>(w), static_cast<long>(h) };
    AdjustWindowRectEx( &rc, dwStyle, FALSE, dwExStyle );
    if(!( appWindow = CreateWindowEx(dwExStyle, CLASSNAME, GetWinTitle(),
								dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								CW_USEDEFAULT, CW_USEDEFAULT, 
								rc.right - rc.left, rc.bottom - rc.top, 
								NULL, NULL, appInstance, NULL) ))
	{
		CloseWindow();
		return E_FAIL;
	}

	ShowWindow(appWindow, SW_SHOW);
	SetForegroundWindow( appWindow );
	SetFocus( appWindow );

	return S_OK;
}

bool WinApplication::SysCommand( HWND AppHwnd, WPARAM Wparam, LPARAM Lparam )
{
	switch (Wparam)							
	{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
		return true;
	}

	return false;
}

LRESULT CALLBACK WinApplication::WndProc(HWND Hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{	
	switch( msg ) {
		case WM_CREATE: {
				if( !WinApplication::pApp->ProcessMessage( msg, wParam, lParam ) )
					return DefWindowProc( Hwnd, msg, wParam, lParam );
			break;
						}
		case WM_SIZE: {
			if (!WinApplication::pApp->Size(Hwnd, wParam, lParam))
				if (!WinApplication::pApp->ProcessMessage(msg, wParam, lParam))
					return DefWindowProc(Hwnd, msg, wParam, lParam);
			break;
		}
		case WM_DISPLAYCHANGE: {
			if (!WinApplication::pApp->DisplayChange(Hwnd, wParam, lParam))
				if (!WinApplication::pApp->ProcessMessage(msg, wParam, lParam))
					return DefWindowProc(Hwnd, msg, wParam, lParam);
			break;
		}							
		case WM_PAINT: {
			if( !WinApplication::pApp->Paint( Hwnd, wParam, lParam ) )
				if( !WinApplication::pApp->ProcessMessage( msg, wParam, lParam ) )
					return DefWindowProc( Hwnd, msg, wParam, lParam );
			break;
					   }
		case WM_SYSCOMMAND: {
			if( !WinApplication::pApp->SysCommand( Hwnd, wParam, lParam ) )
				if( !WinApplication::pApp->ProcessMessage( msg, wParam, lParam ) )
					return DefWindowProc( Hwnd, msg, wParam, lParam );
			break;
							}
		case WM_DESTROY: {
				if( !WinApplication::pApp->ProcessMessage( msg, wParam, lParam ) )
					PostQuitMessage( 0 );
			break;
						 }
		default: {
			if( !WinApplication::pApp->ProcessMessage( msg, wParam, lParam ) ) 
				return DefWindowProc( Hwnd, msg, wParam, lParam );
				 }
	}
	return 0;
}

void WinApplication::StartApplication()
{
	assert(WinApplication::pApp);
	WinApplication::pApp->InternalWinMain();
}

//int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR *t, int nShowCmd ) {
//	assert( WinApplication::pApp );
//	return WinApplication::pApp->InternalWinMain( hInstance, nShowCmd );
//}