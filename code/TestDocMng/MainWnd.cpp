//#include "MainWnd.h"
//#include "SearchBox.h"
//
//
//MainWnd::MainWnd() {
//	tag_box = std::make_unique<TagBoxFrame>();
//	search_box = std::make_unique<SearchBox>();
//	doc_table = std::make_unique<DocTableFrame>();
//
//	Node& root = GetRootNode();
//	SplitLayout::NodeStyle split_line_style;
//	split_line_style.line_width(10px).line_color(ColorSet::Coral).direction(false).which(false).position(30pct);
//	root.Split(split_line_style);
//
//	Node& left = root.GetFirst();
//	SetChild(left, *tag_box);
//
//	Node& right = root.GetSecond();
//	split_line_style.line_width(5px).line_color(color_transparent).direction(true).which(false).position(30px);
//	right.Split(split_line_style);
//
//	SetChild(right.GetFirst(), *search_box);
//	SetChild(right.GetSecond(), *doc_table);
//}
//
//MainWnd::~MainWnd() {
//}