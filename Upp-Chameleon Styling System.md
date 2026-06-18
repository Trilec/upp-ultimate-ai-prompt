### The U++ Chameleon Styling System: An Architectural Guide

This document provides a concise architectural overview of the U++ Chameleon styling system. It is intended for developers looking to create new controls or layouts that are fully themeable and integrate seamlessly with the framework's design principles.

#### 1. Core Concepts

The Chameleon system is a **data-driven, state-based rendering engine**. Its core principle is the complete separation of a control's logic (its behavior) from its appearance (its rendering). This is achieved by defining all visual attributes as simple data, which is then interpreted by a set of standardized painting functions.

*   **`Chameleon_Style` Struct:** This is the heart of the system. It is a simple C-style struct that holds a two-dimensional array of `Value` objects. Think of this as a lookup table or a spreadsheet:
    *   **Rows represent control states:** `NORMAL`, `HOT` (mouse hover), `PUSHED` (clicked), `DISABLED`, etc.
    *   **Columns represent visual attributes (Looks):** `FACECOLOR`, `BORDERCOLOR`, `TEXTCOLOR`, `FONT`, `EFFECT`, etc.
    *   Each cell in this table holds a `Value` (e.g., a `Color`, an `Image`, an `int` for an effect) that defines a specific visual aspect for a given state.

*   **`CH_STYLE` Macro:** This is a declarative macro used to define and instantiate a `Chameleon_Style` struct. It creates a **static, singleton instance** of the style data at compile time. This is the key to its memory efficiency and adherence to the U++ philosophy—it completely avoids dynamic `new`/`delete` for style objects.

*   **Standardized Painting Functions:** A set of global functions, like `ChPaint`, `ChPaintEdge`, and `ChPaintBody`, perform the actual rendering. These functions are the "interpreters" of the `Chameleon_Style` data. They take a `Draw` context, a rectangle, a pointer to a `Chameleon_Style` struct, and the control's current state. They then look up the appropriate values in the style struct and draw the control's frame, body, and text.

#### 2. Architectural Approach for New Controls

When developing a new control that requires styling, follow this architectural pattern to ensure it is robust, themeable, and idiomatic.

**Step 1: Define Your Control's Style with `CH_STYLE`**

First, declare the visual properties for your new control in a header file using the `CH_STYLE` macro. This creates the default "look" for your control.

```cpp
// In MyControl.h

// Declare the style for MyControl
CH_STYLE(MyCtrl, Style, StyleDefault)
{
    // For the NORMAL state
    for(int i = 0; i < 4; i++) {
        // Define face color, border color, etc. for each state (i)
        face[i] = SColorFace;
        border[i] = SColorBorder;
        ink[i] = SColorText;
    }
    // Customize specific states
    face[CTRL_HOT] = SColorHighlight;
    ink[CTRL_HOT] = SColorHighlightText;
    face[CTRL_PUSHED] = SColorPushed;
    
    // Define other properties
    font = StdFont();
    effect[0] = CtrlsImg::EFE_NORMAL; // 3D effect, for example
}
```

**Step 2: Integrate the Style into Your Control**

Your control should store a non-owning pointer to a `Chameleon_Style`. It should not own the style data itself.

```cpp
// In MyControl.h

class MyCtrl : public Ctrl {
public:
    typedef MyCtrl CLASSNAME;

    MyCtrl();

    virtual void Paint(Draw& w) override;
    // ... other overrides for mouse events, etc.

protected:
    const Chameleon_Style *style; // Non-owning pointer to the style data

public:
    // Method to allow users to change the style at runtime
    MyCtrl& SetStyle(const Chameleon_Style& s) { style = &s; Refresh(); return *this; }
};
```

**Step 3: Implement the `Paint` Method**

The `Paint` method is where you connect the control's current state to the style data. Its sole responsibility is to determine the state and delegate the rendering to a `ChPaint` function.

```cpp
// In MyControl.cpp

#include "MyControl.h"

MyCtrl::MyCtrl()
{
    // Set the default style in the constructor
    style = &StyleDefault(); 
}

void MyCtrl::Paint(Draw& w)
{
    Size sz = GetSize();
    
    // 1. Determine the current state
    int state = CTRL_NORMAL;
    if(!IsShowEnabled())
        state = CTRL_DISABLED;
    else if(HasMouse() || HasFocus()) // Or whatever logic defines "hot"
        state = CTRL_HOT;
    // ... add logic for PUSHED if it's a button-like control
    
    // 2. Delegate rendering to a Chameleon paint function
    bool pushed = HasCapture(); // Example for a button
    ChPaint(w, sz, *style, state, pushed);
    
    // 3. Draw any custom content (like text or an icon)
    // The font and color are retrieved from the style data itself.
    String text = "My Control";
    Font font = style->font;
    Color ink = style->ink[state];
    Size tsz = GetTextSize(text, font);
    
    w.DrawText((sz.cx - tsz.cx) / 2, (sz.cy - tsz.cy) / 2, text, font, ink);
}
```

**Step 4: Manage State and Refresh**

Your control's event handlers (`MouseEnter`, `MouseLeave`, `LeftDown`, `GotFocus`, etc.) should only manage the control's state and call `Refresh()` or `Update()`. They should never contain drawing code.

```cpp
// In MyControl.h (or .cpp)

virtual void MouseEnter() override { Refresh(); }
virtual void MouseLeave() override { Refresh(); }
virtual void GotFocus() override   { Refresh(); }
virtual void LostFocus() override  { Refresh(); }
```

#### 3. Best Practices and U++ Philosophy

*   **No Dynamic Memory for Styles:** The `CH_STYLE` macro ensures styles are static data. Your controls simply hold a pointer to this data. This is extremely fast and avoids all memory management overhead, fitting the U++ philosophy perfectly.
*   **Stateless `Paint` Method:** The `Paint` method should be completely stateless. It reads the control's current state and the style data, and renders the output. All state changes happen in event handlers.
*   **Enable Theming:** By exposing a `SetStyle()` method, you allow users of your control to easily change its appearance. An application can define a central theme and apply it to all controls, including yours, by simply calling `myControl.SetStyle(MyApplicationTheme::StyleForMyCtrl())`.
*   **Leverage Existing Styles:** Before creating a new style from scratch, check `uppsrc/CtrlLib/Chameleon.cpp`. You can often reuse or derive from existing styles like `Button::StyleNormal()`, `EditField::StyleDefault()`, etc.

By following this architecture, you create controls that are not only functional but also good citizens of the U++ ecosystem—efficient, easily maintainable, and fully themeable.