#pragma once

#include "WndDesign.h"


class TagBoxFrame;
class SearchBox;
class DocTableFrame;

class MainWnd : public SplitLayout {
private:
	unique_ptr<TagBoxFrame> tag_box;
	unique_ptr<SearchBox> search_box;
	unique_ptr<DocTableFrame> doc_table;
public:
	MainWnd();
	~MainWnd();
};