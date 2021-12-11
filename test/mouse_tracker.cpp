

enum class MouseTrackerMsg{
	Click,         // down - (move < 5px) - up
	DoubleClick,   // down - up - down(<0.5s)
	TripleClick,   // down - up - down(<0.5s) - up - down(<0.5s)
	Drag,          // down - move
	
}


class MouseTracker{
private:
	bool has_mouse_down;
	Point mouse_down_position;
	Point mouse_current_position;
	
public:
}