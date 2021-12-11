// Reference : 
//   https://github.com/chromium/chromium/commit/df813ac7b79f6c935df34dc82d8d6018d0a3d032
//   https://github.com/chromium/chromium/blob/df813ac7b79f6c935df34dc82d8d6018d0a3d032/webkit/glue/webinputevent_win.cc
		
#pragma once

#include <Windows.h>

void Win32ScrollToWheel(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    // Get key state, coordinates, and wheel delta from event.
    typedef SHORT(WINAPI* GetKeyStateFunction)(int key);
    GetKeyStateFunction get_key_state;
    UINT key_state;
    float wheel_delta;
    bool horizontal_scroll = false;

    if ((message == WM_VSCROLL) || (message == WM_HSCROLL)) {
        // Synthesize mousewheel event from a scroll event.  This is needed to
        // simulate middle mouse scrolling in some laptops.  Use GetAsyncKeyState
        // for key state since we are synthesizing the input event.
        get_key_state = GetAsyncKeyState;
        key_state = 0;
        if (get_key_state(VK_SHIFT))
            key_state |= MK_SHIFT;
        if (get_key_state(VK_CONTROL))
            key_state |= MK_CONTROL;

        POINT cursor_position = { 0 };
        GetCursorPos(&cursor_position);
        global_x = cursor_position.x;
        global_y = cursor_position.y;

        switch (LOWORD(wparam)) {
        case SB_LINEUP:    // == SB_LINELEFT
            wheel_delta = WHEEL_DELTA;
            break;
        case SB_LINEDOWN:  // == SB_LINERIGHT
            wheel_delta = -WHEEL_DELTA;
            break;
        case SB_PAGEUP:
            wheel_delta = 1;
            scroll_by_page = true;
            break;
        case SB_PAGEDOWN:
            wheel_delta = -1;
            scroll_by_page = true;
            break;
        default:  // We don't supoprt SB_THUMBPOSITION or SB_THUMBTRACK here.
            wheel_delta = 0;
            break;
        }

        if (message == WM_HSCROLL) {
            horizontal_scroll = true;
            wheel_delta = -wheel_delta;
        }
    } else {
        // Non-synthesized event; we can just read data off the event.
        get_key_state = GetKeyState;
        key_state = GET_KEYSTATE_WPARAM(wparam);

        global_x = static_cast<short>(LOWORD(lparam));
        global_y = static_cast<short>(HIWORD(lparam));

        wheel_delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam));
        if (((message == WM_MOUSEHWHEEL) || (key_state & MK_SHIFT)) &&
            (wheel_delta != 0))
            horizontal_scroll = true;
    }

    // Set modifiers based on key state.
    if (key_state & MK_SHIFT)
        modifiers |= SHIFT_KEY;
    if (key_state & MK_CONTROL)
        modifiers |= CTRL_KEY;
    if (get_key_state(VK_MENU) & 0x8000)
        modifiers |= (ALT_KEY | META_KEY);

    // Set coordinates by translating event coordinates from screen to client.
    POINT client_point = { global_x, global_y };
    MapWindowPoints(NULL, hwnd, &client_point, 1);
    x = client_point.x;
    y = client_point.y;

    // Convert wheel delta amount to a number of lines/chars to scroll.
    float scroll_delta = wheel_delta / WHEEL_DELTA;
    if (horizontal_scroll) {
        unsigned long scroll_chars = kDefaultScrollCharsPerWheelDelta;
        SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &scroll_chars, 0);
        scroll_delta *= static_cast<float>(scroll_chars);
    } else {
        unsigned long scroll_lines = kDefaultScrollLinesPerWheelDelta;
        SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scroll_lines, 0);
        if (scroll_lines == WHEEL_PAGESCROLL)
            scroll_by_page = true;
        if (!scroll_by_page)
            scroll_delta *= static_cast<float>(scroll_lines);
    }

    // Set scroll amount based on above calculations.
    if (horizontal_scroll) {
        // Scrolling up should move left, scrolling down should move right.  This is
        // opposite Safari, but seems more consistent with vertical scrolling.
        delta_x = scroll_delta;
        delta_y = 0;
    } else {
        delta_x = 0;
        delta_y = scroll_delta;
    }
}