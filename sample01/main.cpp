#include <windows.h>
#include <d3d9.h>
#include <math.h>

#define APP_CLASS L"SampleWindow"
#define MAIN_WINDOW_TITLE L"Sample"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FRAME_RATE 60

#define KEY_DOWN(vk_code)((GetAsyncKeyState(vk_code)&0xB000)?1:0)

LPDIRECT3D9 d3d = NULL; 
LPDIRECT3DDEVICE9 d3ddv = NULL;
LPDIRECT3DSURFACE9 back_buffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;

enum direction
{
	Right,
	Left,
};

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message) {
		case WM_DESTROY: 
			PostQuitMessage(0);
			break;
		default: 
			return DefWindowProc(hWnd, message, wParam,lParam);
	}

	return 0;

}

int GameInit(HINSTANCE hInstance, HWND hWnd) 
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp; 

	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	d3dpp.Windowed   = FALSE; // FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;

	d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&d3ddv);

	if (d3ddv==NULL) 
	{
		return 0;
	}
	
	d3ddv->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back_buffer);

	// Create a small off-screen surface (will fill it contain in GameRun)
	int result =
		d3ddv->CreateOffscreenPlainSurface(
			100,					// width 				
			100,					// height
			D3DFMT_X8R8G8B8,		// format
			D3DPOOL_DEFAULT,		// where? (VRAM or RAM)
			&surface,
			NULL);

	return 1;
}

RECT rect;
direction dir;

void GameRun(HWND hWnd)
{
	if (d3ddv->BeginScene()) 
	{
		// Clear back buffer with BLACK
		d3ddv->ColorFill(back_buffer,NULL,D3DCOLOR_XRGB(255,255,0));
		
		// Random color for the surface
	/*	int r = rand() % 255; 
		int g = rand() % 255; 
		int b = rand() % 255;*/
		int r = 0;
		int g = 255;
		int b =  255;

		// Fill the bitmap
		d3ddv->ColorFill(surface,NULL,D3DCOLOR_XRGB(r,g,b));
		
		// Generate a random area (within back buffer) to draw the surface onto
		//RECT rect; 
	/*	rect.left = rand() % (SCREEN_WIDTH/2);
		rect.top = rand() % (SCREEN_HEIGHT/2);
		rect.right = rect.left + rand() % (SCREEN_WIDTH/2);
		rect.bottom = rect.left + rand() % (SCREEN_HEIGHT/2);*/

	/**/
		if (dir == Right)
		{
			rect.left = rect.left + 10;
			rect.top = 200 + 50 * sin(rect.left*5);
			rect.right = rect.left + 100;
			rect.bottom = rect.top + 100;
			if (rect.right >= SCREEN_WIDTH)
				dir = Left;
		}
		else
		{
			rect.left = rect.left - 10;
			rect.top = 200 + 50 * sin(rect.left * 5);
			rect.right = rect.left + 100;
			rect.bottom = rect.top + 100;
			if (rect.left <= 0)
				dir = Right;
		}
		

		// Draw the surface onto the back buffer
		d3ddv->StretchRect(
			surface,			// from 
			NULL,				// which portion?
			back_buffer,		// to 
			&rect,				// which portion?
			D3DTEXF_NONE);

		d3ddv->EndScene();
	}

	d3ddv->Present(NULL,NULL,NULL,NULL);

	if (KEY_DOWN(VK_ESCAPE))
		PostMessage(hWnd, WM_DESTROY, 0, 0);
}

void GameEnd()
{
	if (d3ddv!=NULL) d3ddv->Release();
	if (d3d!=NULL) d3d->Release();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APP_CLASS;
	wc.hIconSm = NULL;
	
	RegisterClassEx(&wc);

	HWND hWnd = 
		CreateWindow(
			APP_CLASS,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, 
//			WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT, 
			800,
			600,
			NULL,
			NULL,
			hInstance,
			NULL);
	
	if (!hWnd) { 
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);

	GameInit(hInstance, hWnd);

	// Enter game message loop...
	MSG msg;
	int done = 0;
	DWORD frame_start = GetTickCount();;
	DWORD tick_per_frame = 1000 / FRAME_RATE;

	rect.left = 100;
	rect.top = 100;
	rect.right = rect.left + 100;
	rect.bottom = rect.top+ 100;

	dir = Right;

	while (!done) 
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) done=1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);			
		}

		DWORD now = GetTickCount();
		if (now - frame_start >= tick_per_frame) 
		{
			frame_start = now;
			GameRun(hWnd);
		}
	}

	GameEnd();
	return 0;
}

