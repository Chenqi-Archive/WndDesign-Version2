#pragma once

#include "d2d_api.h"

#include <vector>


//////////////////////////////////////////////////////////
////            Window Dependent Resources            ////
//////////////////////////////////////////////////////////


struct IDXGISwapChain1;

using HANDLE = void*;


BEGIN_NAMESPACE(WndDesign)

using std::vector;


class WindowResource : Uncopyable {
private:
	IDXGISwapChain1* swap_chain;
	bool has_presented;

	class WindowTarget : public Target {
	public:
		WindowTarget() : Target(nullptr) {}
		void Create(IDXGISwapChain1& swap_chain);
		void Destroy();
	}target;

public:
	WindowResource(HANDLE hwnd);
	~WindowResource();

	void OnResize(Size size);

	Target& GetTarget() { return target; }

	void Present(vector<Rect>&& dirty_regions);
};


END_NAMESPACE(WndDesign)