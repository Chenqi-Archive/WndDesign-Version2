#include <Windows.h>
#include <vector>

using std::vector;

int main() {
	vector<RECT> rects = {
		{ 200, 200, 600, 400 },
		{ 400, 300, 800, 600 },
		{ 300, 250, 500, 700 }
	};

	HRGN region1 = CreateRectRgn(200, 200, 600, 400);
	HRGN region2 = CreateRectRgn(400, 300, 800, 600);
	HRGN region3 = CreateRectRgn(300, 250, 500, 700);
	//HRGN region4 = CreateEllipticRgn(700, 400, 900, 600);

	int ret;
	ret = CombineRgn(region1, region1, region2, RGN_OR);
	ret = CombineRgn(region1, region1, region3, RGN_OR);
	//ret = CombineRgn(region1, region1, region4, RGN_OR);

	int size = GetRegionData(region1, 0, NULL);
	char* buffer = new char[size];
	size = GetRegionData(region1, size, (LPRGNDATA)buffer);
	RGNDATA& data = *(LPRGNDATA)buffer;
	vector<RECT> regions((RECT*)(buffer + data.rdh.dwSize), (RECT*)(buffer + data.rdh.dwSize) + data.rdh.nCount);
}