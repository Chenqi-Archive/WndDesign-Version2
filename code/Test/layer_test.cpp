#include "../WndDesign/layer/layer.h"
#include "../WndDesign/layer/desktop_layer.h"
#include "../WndDesign/layer/background_types.h"
#include "../WndDesign/system/metrics.h"
#include "../WndDesign/system/directx/d2d_api.h"

#include <Windows.h>
#include <windowsx.h>

using namespace WndDesign;
using std::shared_ptr;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesignCore.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesignCore.lib")
#endif // _DEBUG


DesktopLayer* layer;
Layer* layer1;
Layer* layer2;
Image* image;
Background* background;
Background* background1;
Background* background2;


void OnPaint();

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	if (FAILED(CoInitialize(NULL))) { return 0; }

	const wchar_t className[] = L"LayerTestWndClass";
	const wchar_t titleName[] = L"LayerTest";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = className;

	if (!RegisterClassExW(&wc)) { throw; }

	HWND hWnd = CreateWindowExW(WS_EX_LAYERED,
						   className, titleName,
						   WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX,
						   200, 200, 800, 500,
						   NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) { throw; }

	background = new SolidColorBackground(ColorSet::Cyan);
	layer = new DesktopLayer(hWnd, *background);

	image = new Image(L"R:\\test.jpg");
	background1 = new ImageTileBackground(*image, 0xF0, Vector{ 20, 20 });
	layer1 = new Layer(Rect(point_zero, Size(200, 200)), *background1);
	layer1->SetCachedRegion(Rect(point_zero, Size(200, 200)));

	background2 = new SolidColorBackground(ColorSet::DarkMagenta);
	layer2 = new Layer(region_infinite, *background2);

	Rect rect(point_zero, GetDesktopSize());
	layer2->SetCachedRegion(rect - (rect.Center() - point_zero));

	ShowWindow(hWnd, SW_SHOW);

	OnPaint();

	MSG msg;

	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	delete layer;
	delete background;
	delete image;

	delete layer1;
	delete background1;

	delete layer2;
	delete background2;

	CoUninitialize();

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
		if (layer != nullptr) { 
			layer->OnResize(Size(LOWORD(lParam), HIWORD(lParam))); 
			OnPaint();
		}
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
		OnPaint();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}


void OnPaint() {
	FigureQueue figure_queue1;
	figure_queue1.Append(Vector{ -50, -50 }, new WndDesign::LayerFigure(*layer1, Rect(point_zero, Size(200, 200)), CompositeEffect{}));
	layer2->ClearRegion(region_infinite);
	layer2->DrawFigureQueue(figure_queue1, vector_zero, Rect(-40, -40, 10, 10));
	layer2->CommitDraw();

	FigureQueue figure_queue2;
	figure_queue2.Append(Vector(100, 100), new WndDesign::Ellipse(20, 20, 5.0, ColorSet::Crimson, ColorSet::Azure));
	figure_queue2.Append(Vector(100, 200), new WndDesign::LayerFigure(*layer1, Rect(point_zero, Size(200, 200)), CompositeEffect{}));
	figure_queue2.Append(Vector(300, 200), new WndDesign::LayerFigure(*layer2, Rect(-200, -50, 300, 50), CompositeEffect{}));
	layer->ClearRegion(region_infinite);
	layer->DrawFigureQueue(figure_queue2, vector_zero, region_infinite);
	layer->CommitDraw();
}
