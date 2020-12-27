#pragma once

#include "wnd_base_impl.h"


BEGIN_NAMESPACE(WndDesign)


class Desktop : public WndBase {
private:
	Desktop();
	~Desktop();

public:
	static Desktop& Get();

	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void RemoveChild(IWndBase& child_wnd) override;

	virtual void UpdateChildPosition(WndBase& child_wnd) override;
	virtual void UpdateChildCompositeStyle(WndBase& child_wnd) override;


	int MessageLoop();
	void Finish();

};


END_NAMESPACE(WndDesign)