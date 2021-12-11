#include <Windows.h>
#include <windowsx.h>

HWND hWnd;

extern void Init();
extern void Final();
extern void OnPaint();
extern void OnSize(int, int);
extern void OnWheel(int);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	const wchar_t className[] = L"D2DTestWndClass";
	const wchar_t titleName[] = L"D2DTest";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	//wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 不加的话如果窗口大小变化会出现黑色
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = className;

	if (!RegisterClassExW(&wc)) { throw; }

	hWnd = CreateWindowExW(WS_EX_LAYERED,
						   className, titleName,
						   WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX, // WS_OVERLAPPEDWINDOW, //
						   200, 200, 800, 500,
						   NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) { throw; }

	Init();

	ShowWindow(hWnd, SW_SHOW);
	OnPaint();  // WM_PAINT will never be sent when WS_EX_LAYERED is used.

	MSG msg;

	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	Final();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		OnPaint();
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		if (::hWnd == nullptr) { break; }  // If use WS_POPUP rather than WS_OVERLAPPEDWINDOW, 
										   //   WM_SIZE will be sent before window is created.
		OnSize(LOWORD(lParam), HIWORD(lParam));
		OnPaint();
		break;

	case WM_NCCALCSIZE:  // Process the message to set client region the same as the window region.
		break;
	case WM_NCACTIVATE:  // Do not draw the nonclient area.
		break;

	case WM_ERASEBKGND: return false;
	case WM_LBUTTONDOWN:  // Send the fake mouse messages hit on caption to fool DefWindowProc().
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		break;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		OnWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
