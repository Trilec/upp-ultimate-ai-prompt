U++ Application Development Guide V07

This guide serves as a living document for developing applications with the U++ framework. It consolidates our discoveries, best practices, and coding standards to ensure consistency and provide a head start for any new development work.

U++ Philosophy

U++ is a C++ rapid application development framework designed to simplify complex tasks, particularly for desktop applications. Its core philosophy emphasizes deterministic resource management and tight integration between the library and the build system. Most objects are tied to a logical scope, reducing the need for manual memory management with new and delete. Pointers are primarily for pointing, not owning heap resources, resulting in automatic and deterministic resource management that is often more predictable than garbage-collected languages.

Project Setup and Build System

Setting up a U++ project correctly is critical for smooth development. The framework uses a self-contained build system that does not rely on external tools like Make or CMake.

Project Folder Structure

A well-organized project enhances navigability. Below is the recommended structure:

upp-baseapp/         ? Root folder for your U++ application
+-- BaseApp.upp      ? U++ package file (must be in root)
+-- main.cpp         ? Main application entry point
¦
+-- include/         ? Public headers for your library/app
¦   +-- statemachine.h
¦
+-- src/             ? Source file implementations
¦   +-- statemachine.cpp
¦
+-- ui/              ? UI layout files from the Layout Designer
¦   +-- MyLayout.lay
¦
+-- assets/          ? Assets like icons or JSON files
¦   +-- myicon.png
¦
+-- design/          ? Diagrams, notes, and design documentation
    +-- design.md
The .upp Package File

The .upp file defines the package’s dependencies, source files, and build configurations.

Location: Must be in the root of the package directory for TheIDE detection.
Dependencies: Use the uses directive to link packages like Core and CtrlLib.
Files: List all .cpp, .h, and .lay files.
Main Package: The mainconfig section marks the package as an executable, visible in TheIDE’s “Select main package” dialog.
Note: The .upp file does not support comments.

Example .upp File:

description "Comprehensive UI test harness\377";

uses
    Core,
    CtrlLib;

include
    ,
    /assets,
    /include;

file
    upp_widgetgallery.lay,
    upp_widgetgallery.iml,
    upp_widgetgallery.h,
    upp_widgetgallery.cpp,
    main.cpp,
    \src\tools_panel.cpp,
    include\tools_panel.h;

mainconfig
    "" = "GUI";
Connecting .upp, .h, and .lay Files

A frequent build issue arises from path mismatches between the .upp file and the C++ header’s LAYOUTFILE macro.

Rule: The path in #define LAYOUTFILE <path> must be relative to an include path in the .upp file.
Example: If the .upp file includes include /ui;, and the layout is at ui/MyLayout.lay, use #define LAYOUTFILE <MyLayout.lay> in the header.
Incorrect Usage: #define LAYOUTFILE <MyPackage/ui/MyLayout.lay> causes a build failure as the build system prepends the include path, searching for ui/MyPackage/ui/MyLayout.lay.
Core Library Essentials

The Core library provides foundational tools for U++ applications.

Containers

U++ offers optimized containers with move semantics:

Vector<T>: A fast dynamic array requiring moveable T.
Array<T>: A flexible dynamic array with no type restrictions.
One<T>: A smart pointer for single object ownership. Use Vector<One<MyObject>> for unique ownership of complex objects.
Other Containers: BiVector, Index, VectorMap, ArrayMap.

Example:

#include <Core/Core.h>
using namespace Upp;

CONSOLE_APP_MAIN
{
    Vector<int>          vec {1, 2};                 DUMP(vec);
    Index<String>        idx {"Apple", "Orange"};    DUMP(idx.Find("Apple"));
    VectorMap<String,int>map {{"Apple",1},{"Orange",2}}; DUMP(map.Get("Apple"));
    BiVector<int>        biv; biv.AddHead(1); biv.AddTail(2); DUMP(biv);
    One<int>             one; *one.Create() = 123;   DUMP(*one);
    Any                  any;  any.Create<int>() = 456; DUMP(any.Get<int>());
    Buffer<int>          buf(3); buf[0]=1; buf[1]=2; buf[2]=3; DUMP(buf[0]);
}
String Manipulation

The String class offers robust methods for manipulation, with StringBuffer for direct character modification.

Example:

#include <Core/Core.h>
using namespace Upp;

CONSOLE_APP_MAIN
{
    String s = "lorem ipsum dolor sit amet";
    s.Cat('!');                 DUMP(s.Last());           // '!'
    DUMP(s.StartsWith("lorem")); DUMP(s.EndsWith("!"));
    s.Replace("dolor","DOLOR"); DUMP(s);
    s.Trim(5).TrimLast();       DUMP(s);                 // "lorem"
    s.Insert(0,"String ");      DUMP(s);                 // "String lorem"
    WString ws = s.ToWString(); ws << "°";               DUMP(ws);
    s = ws.ToString();          DUMP(s);
    StringBuffer sb(s); *sb = 'C'; s = sb;               DUMP(s); // "Cring …"
}
Value, Null, and Polymorphism

The Value type is a versatile variant-like class, with Null indicating an empty or invalid value. ValueArray and ValueMap support heterogeneous data collections.

Error Handling

U++ uses the Exc class for exceptions.

Example:

try {
    FileIn fin("non_existent_file.txt");
    if(!fin) throw Exc("Cannot open data.txt");
}
catch(const Exc& e) {
    LOG(e);
    PromptOK(e);
}
Streams and Serialization

U++ provides stream interfaces and serialization capabilities:

File Streams: FileIn, FileOut, FileAppend for file operations.
Serialization: StoreAsString and LoadFromString for object serialization.
JSON: Use Core/JSON.h for parsing (ParseJSON, AsJSON, Jsonize). Avoid third-party JSON libraries.
Ranges and Algorithms

U++ algorithms operate on containers and ranges.

Example:

Vector<int> data {3, 1, 2};
Sort(data);
int index = FindIndex(data, 2); // index is 1
auto subrange = SubRange(data, 1, 2); // Range containing {2, 3}
Multithreading and Parallelism

CoWork and Thread simplify concurrent and parallel tasks. CoPartition distributes tasks across cores.

Example:

Vector<int> data{ /* ... */ };
CoPartition(data, [](const auto& subrange){ /* process subrange */ });
GUI Development with CtrlLib

The CtrlLib package provides tools for building modern graphical user interfaces.

Widgets and Ownership

Widgets are plain C++ objects owned by your code, not a global toolkit.

Events and Callbacks

U++ uses Upp::Function for callbacks, with lambdas assigned via <<= for safe execution.

Example: Simple GUI Button

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct ButtonApp : TopWindow {
    int    count = 0;
    Button clickMe;
    Label  info;

    void Refresh() { info = Format("Clicks: %d", count); }

    ButtonApp() {
        Title("Button demo");
        clickMe <<= [=]{ ++count; Refresh(); };
        clickMe.SetLabel("Click Me!");
        Add(clickMe.VCenterPos(24).HCenterPos(120));
        Add(info.BottomPos(8,20).HCenterPos(200));
        info.SetAlign(ALIGN_CENTER);
        Sizeable().Zoomable();
        Refresh();
    }
};
GUI_APP_MAIN { ButtonApp().Run(); }
Animation and Interpolation

Introduced in U++ 2025.1, animation helpers enhance UI dynamics:

Lerp(a, b, t): Linearly interpolates between two values.
Animate(...): Animates widget properties over time.

Example: Animated Button

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct AniWin : TopWindow {
    Button b;
    AniWin() {
        Title("Animate demo");
        b.SetLabel("Hover"); Add(b.CenterPos(Size(80,28)));
        b.WhenMouseEnter = [=]{
            Animate(b).Time(300).Cubic().Pos(Rect(b.Left()-20,b.Top()-10,120,36));
        };
        b.WhenMouseLeave = [=]{
            Animate(b).Time(300).Cubic().Pos(Rect(b.Left()+20,b.Top()+10,80,28));
        };
    }
};
GUI_APP_MAIN { AniWin().Run(); }
Layout System

The U++ Layout Designer saves UI designs as .lay files, focusing on arrangement, not parent-child relationships.

Workflow:

Design UI in TheIDE, saving as ui/MyLayout.lay.
Add the .lay file to the .upp file.
Use LAYOUTFILE macro in the header before including <CtrlCore/lay.h>.

Example:

// In MyWindow.h
#define LAYOUTFILE <MyApp/ui/MyLayout.lay>
#include <CtrlCore/lay.h>

class MyWindow : public WithMyLayout<TopWindow> { /* ... */ };

Layout Rules:

Parenting: Define parent-child relationships in C++ code, not .lay files.
One Layout per File: Use one LAYOUT(...) block per .lay file.
Dynamic Containers: Declare Splitter or TabCtrl panes as C++ member variables and assign in the constructor.
ParentCtrl: Use as a programmable container, not for defining relationships in .lay.
Naming: Ensure LAYOUT(ClassName, ...) matches the class name in WithClassName<...>.
Panel Switching: Use ParentCtrl with .Show()/.Hide() for dynamic visibility.

Example: Image Viewer

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

class ImageView : public TopWindow {
    ImageCtrl        img;
    FileList         files;
    Splitter         splitter;
    String           dir;
    FrameTop<Button> dirUp;

    void Load(const String& fn) {
        Image m = StreamRaster::LoadFileAny(fn);
        if(IsNull(m)) return;
        Size view = img.GetSize(), isz = m.GetSize();
        if(isz.cx > view.cx || isz.cy > view.cy) {
            m = GetFitSize(m.GetSize(), GetSize()) == m.GetSize() ? m : Rescale(m, GetFitSize(m.GetSize(), GetSize()));
        }
        img.SetImage(m);
    }

    void LoadDir(const String& d) {
        dir = d; files.Clear(); Title(dir);
        ::Load(files, dir, "*.*"); SortByExt(files);
    }

    void DoDir()       { if(files.IsCursor() && files.Get(files.GetCursor()).isdir)
                           LoadDir(AppendFileName(dir, files.GetKey())); }
    void Enter()       { if(files.IsCursor() && !files.Get(files.GetCursor()).isdir)
                           Load(AppendFileName(dir, files.GetKey())); }
    void DirUpClick()  { LoadDir(DirectoryUp(dir)); }

public:
    typedef ImageView CLASSNAME;
    ImageView() {
        Title("Image viewer"); Sizeable().Zoomable();
        splitter.Horz(files, img).SetPos(2600);
        Add(splitter.SizePos());
        files.WhenEnterItem  = THISBACK(Enter);
        files.WhenLeftDouble = THISBACK(DoDir);
        dirUp.SetImage(CtrlImg::DirUp()).NormalStyle();
        dirUp <<= THISBACK(DirUpClick);
        files.AddFrame(dirUp);
        LoadDir(GetCurrentDirectory());
    }
    virtual bool Key(dword k, int) override { return k==K_ENTER && DoDir(), true; }
};

GUI_APP_MAIN { ImageView().Run(); }
RichText and RichEdit

For RichEdit, include <RichEdit/RichEdit.h> and manage text via a RichText object.

Example Workflow:

Append formatted text to RichText.
Update RichEdit with Set().
Scroll to the bottom with MoveEnd().

Custom Object Example:

struct MyRichObjectType : public RichObjectType
{
    virtual String GetTypeName(const Value&) const;
    virtual void   Paint(const Value& data, Draw& w, Size sz) const;
    virtual bool   IsText() const;
    virtual void   Menu(Bar& bar, RichObject& ex, void *context) const;
    virtual void   DefaultAction(RichObject& ex) const;
    void Edit(RichObject& ex) const;
    typedef MyRichObjectType CLASSNAME;
};
Coding Standards and Best Practices
Coding Conventions
U++ Version: Use 2025.1 (build 17799) or newer.
Naming: Use PascalCase for classes, camelCase for methods/variables, UPPER_CASE_SNAKE_CASE for constants.
Header Guards: Use #pragma once.
Formatting: 4-space indentation, clean and concise code.
API Stability: Only add to public APIs, avoid breaking changes.
Inlining: Inline functions = 4 lines.
Dark Mode: Use SColor... or AColor... for theme-derived colors.
Comments: Use T++ doc style for formal documentation; // for implementation notes. Avoid Doxygen.
Patterns: Avoid the Builder pattern, as it’s not idiomatic in U++.
Asset Management
Images (.iml): Add via TheIDE’s Image Designer; compiled into the application.
Application Icon (.ico): Place in the project root. Avoid overwriting with “save .ico and .png” in Image Designer.
Development Philosophy
Verify Features: Check uppsrc for feature existence before implementation.
U++ Ecosystem: Prefer U++ libraries over external dependencies like Boost or nlohmann/json.
Source of Truth: Consult uppsrc headers and examples for clarity.
AI Collaboration Directives
User Code as Source: Never regress or remove user-added functionality.
No Unrequested Logic: Avoid adding unrequested features or controls.
API Verification: Confirm U++ class/function usage against uppsrc or documentation.
Smart Pointers and Ownership

Choosing the correct smart pointer ensures proper resource management:

One<T>: For unique, movable ownership, equivalent to std::unique_ptr.
Ptr<T> (with Pte<T>): For shared, reference-counted ownership, similar to std::shared_ptr.

Pitfall Example: Using One<T> for shared ownership caused heap corruption. Use Ptr<T> when multiple objects need to share a resource’s lifetime.

Best Practice:

if (live_) live_->anim = nullptr; // Prevent scheduler access after destruction
Scheduler::Inst().Remove(live_);
live_ = nullptr;
Common Pitfalls and Solutions
General Pitfalls
Dangling Ctrl: Null-check owner->GetTopWindow() to avoid crashes.
SetTimeCallback: Use SetTimeCallback(ms, &TickFn, &cookie) with int cookie.
Function Lifetime: Store Function<> in owning objects to prevent heap issues.
Mixed Ownership: Consistently use one ownership strategy (Ptr, One, or raw pointers).
Lambda Captures: Capture values or use Ptr<> to avoid dangling pointers.
Global Ctrl Objects: Use Single<> or heap allocation, not static Ctrl.
Timer Callbacks: Guard against stale objects in timer loops.

Crash Prevention Cheat-Sheet:

Pattern	Quick Test	Bullet-Proof Habit
Dangling Ctrl	Crash after window close ? stack in Value::IsNull()	Null-check owner->GetTopWindow().
SetTimeCallback signature	Compiler: “assigning void to void*”	Use int cookie in SetTimeCallback.
Function<> lifetime	Debug CRT ? “invalid heap pointer”	Store Function<> in owning object.
Mixed ownership	Count delete vs constructors	Stick to one ownership strategy.
Lambda captures raw pointer	Search for [=] { use(ptr); }	Capture values or use Ptr<>.
Missing U++ type	Error: “Rect is undefined”	Include <Core/Gtypes.h> or qualify Upp::Rect.
Global static Ctrl	Crash on exit	Use Single<> or heap allocation.
Namespace collision	Error: “reference to non-static member”	Fully qualify U++ types in lambdas.
Timer callback after destroy	Crash on window close	Check owner existence in timer loop.
Double delete	Debug CRT ? “invalid heap pointer”	Assign one owner, use raw pointers for observers.
UI and Graphics Pitfalls
Name Collisions

Avoid identifiers like near, far, min, max, or GetMessage due to potential macro conflicts.

Example:

// BAD
auto near = [&](Pointf a) { /* ... */ };

// GOOD
auto is_near = [&](Pointf a) { /* ... */ };
Point vs. Pointf

Use explicit casts to avoid narrowing errors with Pointf.

Example:

int half = 40;
Pointf local[4] = {
    Pointf((double)-half, (double)-half),
    Pointf((double) half, (double)-half),
    Pointf((double) half, (double) half),
    Pointf((double)-half, (double) half),
};
Colors and Alpha

Some backends ignore alpha in DrawText. Animate color or position instead.

Example:

RGBA a{31,41,55, (byte)alpha};
Color c(a);
w.DrawRect(rc, c);
w.DrawText(x, y, "Label", f, c); // Alpha may be ignored
Clip Stacks

Always pair Clipoff() with End().

Example:

w.Clipoff(0, 0, sz.cx, sz.cy);
w.End();
Mouse Drag

Use SetCapture()/ReleaseCapture() for drag operations.

Example:

virtual void LeftDown(Point p, dword) override {
    SetCapture();
    dragging = true;
}
virtual void MouseMove(Point p, dword) override {
    if (!dragging) return;
    if (!GetMouseLeft()) { dragging = false; ReleaseCapture(); Refresh(); return; }
}
virtual void LeftUp(Point, dword) override {
    dragging = false;
    ReleaseCapture();
}
virtual void MouseLeave() override {
    if (!GetMouseLeft() && dragging) { dragging = false; ReleaseCapture(); Refresh(); }
}
Responsive Layouts

Recompute sizes in TopWindow::Layout() to avoid clipping.

Example:

virtual void Layout() override {
    Size sz = GetSize();
    int rightX = 210, gap = 10, cols = 2, rows = 4;
    int rightW = max(240, sz.cx - rightX - gap);
    int rightH = max(200, sz.cy - 20);
    int tileW = max(220, (rightW - (cols - 1) * gap) / cols);
    int tileH = max(120, (rightH - (rows - 1) * gap) / rows) - 20;
    for (int i = 0; i < demos.GetCount(); ++i) {
        int r = i / cols, c = i % cols;
        int x = rightX + c * (tileW + gap);
        int y = 10 + r * (tileH + 20 + gap);
        demos[i].caption.LeftPos(x, tileW).TopPos(y, 18);
        demos[i].canvas.LeftPos(x, tileW).TopPos(y + 20, tileH);
    }
}
Animation Timing

Use normalized timing helpers for smooth animations.

Example:

static inline double Seg01(double p, double a0, double a1) {
    if (p <= a0) return 0.0; if (p >= a1) return 1.0; return (p - a0) / max(1e-9, (a1 - a0));
}
static inline int LerpI(int a, int b, double t) { return int(a + (b-a)*t + 0.5); }
Exception-Safe Timers

Guard timer callbacks against exceptions.

Example:

struct Stepper {
    TimeCallback t;
    void Start() { t.Set(16, THISBACK(Tick)); }
    void Tick() {
        bool ok = true;
        try { /* update state */ }
        catch(...) { ok = false; }
        if(ok) t.Set(16, THISBACK(Tick));
    }
};
Fonts and Metrics

Recalculate text sizes on resize.

Example:

Font title = StdFont().Bold().Height(clamp(sz.cy/10, 14, 42));
Size ts = GetTextSize("Hello", title);
int x = (sz.cx - ts.cx)/2;
Pixel-Precise Drawing

Round floats to integers for stable rendering.

Example:

int x = int(center + radius * cos(theta) + 0.5);
int y = int(center + radius * sin(theta) + 0.5);
Balanced Transforms

Ensure every transform or clip is closed.

Example:

w.Clipoff(0, 0, sz.cx, sz.cy);
// Draw operations
w.End();
Manual Timing

Use manual ticking for deterministic animations.

Example:

int64 wall_now = msecs();
if (manual_last_now == 0) manual_last_now = wall_now;
int64 dt = wall_now - manual_last_now;
if (max_ms_per_tick > 0) dt = min<int64>(dt, max_ms_per_tick);
manual_last_now += max<int64>(0, dt);
RunFrame(manual_last_now);
Array Lengths

Use portable alternatives to __countof.

Example:

template <class T, size_t N> constexpr int CountOf(T (&)[N]) { return int(N); }
int n = CountOf(kEases);
Drag Math

Map pixel coordinates to normalized values.

Example:

const int inset = 6;
double sx = 200.0 / (sz.cx - 2*inset);
double sy = 200.0 / (sz.cy - 2*inset);
Pointf nf(clamp((p.x - inset) * sx / 200.0, 0.0, 1.0),
          clamp(1.0 - (p.y - inset) * sy / 200.0, 0.0, 1.0));
p0.y = nf.y;
p3.y = nf.y;
Consistent Layout Styles

Choose either anchored layouts or manual SetRect(), not both.

Thread Safety

Keep GUI operations on the main thread, using callbacks for background updates.

Performance Optimization
Mark large types as Moveable<T> for efficient Vector operations.
Use Vector<T>::SetCount(n) for preallocation to avoid repeated Add().
Logging

Use Cout() for diagnostics, avoiding debug prints in hot paths.

Example Applications
Responsive Tile Grid
class Tile : public Ctrl {
public:
    virtual void Paint(Draw& w) override {
        Size sz = GetSize();
        w.DrawRect(sz, SColorFace());
        int pad = max(8, sz.cx/40);
        Rect inner = RectC(pad, pad, sz.cx-2*pad, sz.cy-2*pad);
        w.DrawRect(inner, White());
        w.DrawRect(inner.left, inner.top, inner.Width(), 1, Color(220,225,235));
    }
};

class Window : public TopWindow {
    Tile tiles[4];
public:
    Window() { Title("Responsive Grid").Sizeable(); for(auto& t: tiles) Add(t.SizePos()); }
    virtual void Layout() override {
        Size sz = GetSize(); int gap=10; int cols=2; int rows=2;
        int W = (sz.cx - (cols+1)*gap)/cols;
        int H = (sz.cy - (rows+1)*gap)/rows;
        for(int i=0;i<4;++i){
            int r=i/cols, c=i%cols;
            tiles[i].SetRect(gap + c*(W+gap), gap + r*(H+gap), W, H);
        }
    }
};
Safe Dragging Control
class Draggable : public Ctrl {
    bool dragging=false; Point start;
public:
    virtual void LeftDown(Point p, dword) override { dragging=true; start=p; SetCapture(); Refresh(); }
    virtual void LeftUp(Point, dword) override { dragging=false; ReleaseCapture(); Refresh(); }
    virtual void MouseMove(Point p, dword) override {
        if(!dragging) return;
        if(!GetMouseLeft()) { dragging=false; ReleaseCapture(); Refresh(); return; }
        // use p - start
    }
};
Appendices
General Development Guidelines
Canonical Sources: Verify against uppsrc or official documentation.
Version-Neutral APIs: Show both old and new signatures if APIs change.
Memory and Ownership: Favor value semantics; use Null carefully.
Widgets: Avoid global/static Ctrl objects; use Single<> or factories.
Threading: Restrict GUI operations to the main thread with GuiLock.
Static Linking: Default to static binaries.
OOM Policy: U++ aborts on allocation failure; avoid try/catch for OOM.
Leak Detection: Use MemoryBreakpoint and MemoryIgnoreLeaksBlock.
JSON: Use Core/JSON.h exclusively.
Official Documentation Links
Overview: https://www.ultimatepp.org/www$uppweb$overview$en-us.html
Docs Hub: https://www.ultimatepp.org/www$uppweb$documentation$en-us.html
Core Tutorial: https://www.ultimatepp.org/srcdoc$Core$Tutorial$en-us.html
GUI Tutorial: https://www.ultimatepp.org/srcdoc$CtrlLib$Tutorial$en-us.html
Containers: https://www.ultimatepp.org/srcdoc$Core$NTL_en-us.html
Caveats: https://www.ultimatepp.org/srcdoc$Core$Caveats_en-us.html
Leak Guide: https://www.ultimatepp.org/srcdoc$Core$Leaks_en-us.html
Design Decisions: https://www.ultimatepp.org/srcdoc$Core$Decisions_en-us.html
RichText (QTF): https://www.ultimatepp.org/srcdoc$RichText$QTF_en-us.html
GitHub Source Links
Master Branch: https://github.com/ultimatepp/ultimatepp/tree/master
Next 2025.1 Branch: https://github.com/ultimatepp/ultimatepp/tree/next2025_1
Key Files:
https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/CtrlCore/CtrlCore.h
https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/Core/Function.h
https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/Core/One.h
https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/Core/Ptr.h
Widget API Reference
ArrayCtrl
int GetCount() const
void Clear()
Value Get(int row, int col) const
void Set(int row, int col, const Value& v)
void Add(...)
void Insert(int row, ...)
Bar, MenuBar, ToolBar
Item& Add(const char* text, const Image& image, const Callback& cb)
Item& Sub(const char* text, const Function<void(Bar&)>& submenu)
void Separator()
void Break()
Button, ButtonOption
Button& SetImage(const Image& img)
Button& Ok() / Cancel() / Exit()
ButtonOption& Set(bool b)
bool Get() const
Option
void Set(bool b) / operator=(bool b)
bool Get() const
void SetGroup(int g)
Static Widgets
SeparatorCtrl& Margin(int w) / Margin(int l, int r)
SeparatorCtrl& SetSize(int w)