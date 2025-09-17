# U++ Application Development Guide V07

This guide serves as a living document for developing applications with the U++ framework. It consolidates our discoveries, best practices, and coding standards to ensure consistency and provide a head start for any new development work.

## U++ Philosophy

U++ is a C++ rapid application development framework designed to simplify complex tasks, particularly for desktop applications. Its core philosophy emphasizes deterministic resource management and tight integration between the library and the build system. Most objects are tied to a logical scope, reducing the need for manual memory management with `new` and `delete`. Pointers are primarily for pointing, not owning heap resources, resulting in automatic and deterministic resource management that is often more predictable than garbage-collected languages.

&nbsp;

# Project Setup and Build System

Setting up a U++ project correctly is critical for smooth development. The U++ build is **self-contained** (no CMake/Make), and discovery of packages depends on your **Assembly → Package nests** configuration.

## 1) Assembly & Package Nests (the “gotcha”)

**Where:** TheIDE → **Setup → Select main package…** → right-click your assembly (e.g. “github”) → **Assembly setup…**

**Package nests** must contain **both** your repo root and `uppsrc` (semicolon or separate lines on Windows), e.g.:

```
E:\apps\github;E:\upp-17810\uppsrc
```

Click **OK**, then pick the package you want to build as the **main package**.  
If nests are wrong you’ll see “missing package: Core / CtrlLib / GalleryCtrl”.

> Tip: The **Output directory** (e.g., `E:\apps\github\out`) does **not** affect discovery. Only **Package nests** do.

* * *

## 2) Package types: Main app vs Library package

- **Main app package**: produces an EXE and must have `mainconfig "" = "GUI"` (or `"CONSOLE"`).
    
- **Library package** (what our **GalleryCtrl** is): no `WinMain` / `main`. Keep `mainconfig "" = ""`.
    

**Why it matters:**  
If you press **Build (F7)** on a **library** that’s selected as the main package, you’ll get:

```
ld.lld: error: undefined symbol: WinMain
```

To work with a library package:

- Either **Compile** (hammer icon) it by itself, or
    
- Add a **demo/example** EXE that depends on the library and set the demo as your **main package**.
    

* * *

## 3) Recommended multi-package layout

### A) Single repo while developing (fast iteration)

```
FontStudio/                    ← repo root (in your Package nests)
├─ FontStudio.upp              ← MAIN APP (EXE)
├─ main.cpp
└─ GalleryCtrl/                ← LIB package lives here during dev
   ├─ GalleryCtrl.h
   ├─ GalleryCtrl.cpp
   └─ GalleryCtrl.upp
```

**GalleryCtrl.upp** (library; note empty mainconfig)

```
description "GalleryCtrl — zoomable thumbnail/grid control\377";

uses
    Core,
    CtrlLib;

file
    GalleryCtrl.h,
    GalleryCtrl.cpp;

mainconfig
    "" = "";
```

**FontStudio.upp** (main app that uses GalleryCtrl)

```
description "FontStudio — app scaffold using GalleryCtrl\377";

uses
    Core,
    CtrlLib,
    GalleryCtrl;

include
    .;

file
    main.cpp;

mainconfig
    "" = "GUI";
```

**FontStudio/main.cpp** (minimal working)

```cpp
#include <CtrlLib/CtrlLib.h>
#include <GalleryCtrl/GalleryCtrl.h>
using namespace Upp;

struct MainWin : TopWindow {
    GalleryCtrl gallery;
    SliderCtrl  zoom;
    MainWin() {
        Title("FontStudio (dev)"); Sizeable().Zoomable();

        Add(gallery.HSizePos().VSizePos(36, 0));
        Add(zoom.LeftPos(8, 140).TopPos(6, 24));
        zoom.MinMax(0, 6).SetData(2);
        zoom.WhenAction << [=]{ gallery.SetZoomIndex((int)~zoom); };
        gallery.WhenZoom << [=](int zi){ zoom <<= zi; };

        for(int i=0;i<2000;++i)
            gallery.Add(Format("icon_%04d", i));
    }
};

GUI_APP_MAIN { MainWin().Run(); }
```

> **PNG/JPG loading note:** If you call `StreamRaster::LoadFileAny` anywhere (e.g., in `GalleryCtrl::SetThumbFromFile`), make sure the **main app** adds decoders:  
> `uses plugin/png, plugin/bmp` (and `plugin/jpg` if needed). The decoders register at link time.

### B) Split into separate repo later (clean publish)

**Repo 1: upp_GalleryCtrl**

```
upp_GalleryCtrl/
└─ GalleryCtrl/
   ├─ GalleryCtrl.h
   ├─ GalleryCtrl.cpp
   └─ GalleryCtrl.upp
```

**Repo 2: FontStudio**

```
FontStudio/
├─ FontStudio.upp
├─ main.cpp
└─ (optionally add upp_GalleryCtrl as a submodule under GalleryCtrl/)
```

In TheIDE, put **both** repo roots into **Package nests** so `uses GalleryCtrl` resolves.

* * *

## 4) Examples / Demos (recommended)

Give each library its own runnable demo(s). This keeps “Compile vs Build” clean and lets you test perf/UX in isolation.

```
GalleryCtrl/
├─ GalleryCtrl.upp
├─ GalleryCtrl.h
├─ GalleryCtrl.cpp
└─ examples/
   └─ GalleryDemo/
      ├─ GalleryDemo.upp      ← MAIN APP
      └─ main.cpp
```

**GalleryDemo.upp**

```
description "GalleryCtrl Demo\377";

uses
    Core,
    CtrlLib,
    GalleryCtrl,
    plugin/png,     // if loading PNGs in the demo
    plugin/bmp;

file
    main.cpp;

mainconfig
    "" = "GUI";
```

&nbsp;

* * *

## 6) The .upp Package File (quick rules)

- Location: root of the **package** directory.
    
- `uses`: list package dependencies (e.g., `Core`, `CtrlLib`, your other packages).
    
- `file`: list your `.cpp`, `.h`, `.lay` files.
    
- `include`: add extra include search roots (optional; keep it small).
    
- `mainconfig`:
    
    - `"" = "GUI"` or `"CONSOLE"` for an EXE (main package),
        
    - `"" = ""` for a non-main library package.
        
- No comments are allowed in `.upp`.
    

* * *

## 7) Connecting `.upp`, `.h`, and `.lay` (if you use Layout Designer)

Common pitfall: **LAYOUTFILE path must be relative to `.upp`’s `include` paths**, not the filesystem or package name.

- If your `.upp` has:
    
    ```
    include
        /ui;
    ```
    
    and your layout is `ui/MyLayout.lay`, then in the header:
    
    ```cpp
    #define LAYOUTFILE <MyLayout.lay>
    #include <CtrlCore/lay.h>
    ```
    
- **Incorrect** (will double-prepend):  
    `#define LAYOUTFILE <MyPackage/ui/MyLayout.lay>`
    

* * *

## 8) Build workflow tips & pitfalls

- **Missing package** on build: check **Assembly → Package nests**; they must contain your repo root and `uppsrc`.  
    Do **not** prefix assembly name in `uses` (`uses GalleryCtrl`, not `uses myassembly/GalleryCtrl`).
    
- **Undefined WinMain**: you built a library as if it were a main app. Either **Compile** it, or build a demo as the main package.
    
- **PNG/JPG not loading**: add `plugin/png` (and `plugin/jpg`) to the **main app** `uses`.
    
- **THISBACK macro errors**: make sure the callback target is a method of the **current class** and you included the header where it’s declared.
    
- **Header discovery**: if you use `#include <GalleryCtrl/GalleryCtrl.h>`, make sure the **package folder name** is `GalleryCtrl` and it is inside a nest path.
    

* * *

## Core Library Essentials

The `Core` library provides foundational tools for U++ applications.

### Containers

U++ offers optimized containers with move semantics:

- **`Vector<T>`**: A fast dynamic array requiring moveable `T`.
- **`Array<T>`**: A flexible dynamic array with no type restrictions.
- **`One<T>`**: A smart pointer for single object ownership. Use `Vector<One<MyObject>>` for unique ownership of complex objects.
- **Other Containers:** `BiVector`, `Index`, `VectorMap`, `ArrayMap`.

**Example:**

```cpp
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
```

### String Manipulation

The `String` class offers robust methods for manipulation, with `StringBuffer` for direct character modification.

**Example:**

```cpp
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
```

### Value, Null, and Polymorphism

The `Value` type is a versatile variant-like class, with `Null` indicating an empty or invalid value. `ValueArray` and `ValueMap` support heterogeneous data collections.

### Error Handling

U++ uses the `Exc` class for exceptions.

**Example:**

```cpp
try {
    FileIn fin("non_existent_file.txt");
    if(!fin) throw Exc("Cannot open data.txt");
}
catch(const Exc& e) {
    LOG(e);
    PromptOK(e);
}
```

### Streams and Serialization

U++ provides stream interfaces and serialization capabilities:

- **File Streams:** `FileIn`, `FileOut`, `FileAppend` for file operations.
- **Serialization:** `StoreAsString` and `LoadFromString` for object serialization.
- **JSON:** Use `Core/JSON.h` for parsing (`ParseJSON`, `AsJSON`, `Jsonize`). Avoid third-party JSON libraries.

### Ranges and Algorithms

U++ algorithms operate on containers and ranges.

**Example:**

```cpp
Vector<int> data {3, 1, 2};
Sort(data);
int index = FindIndex(data, 2); // index is 1
auto subrange = SubRange(data, 1, 2); // Range containing {2, 3}
```

### Multithreading and Parallelism

`CoWork` and `Thread` simplify concurrent and parallel tasks. `CoPartition` distributes tasks across cores.

**Example:**

```cpp
Vector<int> data{ /* ... */ };
CoPartition(data, [](const auto& subrange){ /* process subrange */ });
```

## GUI Development with CtrlLib

The `CtrlLib` package provides tools for building modern graphical user interfaces.

### Widgets and Ownership

Widgets are plain C++ objects owned by your code, not a global toolkit.

### Events and Callbacks

U++ uses `Upp::Function` for callbacks, with lambdas assigned via `<<=` for safe execution.

**Example: Simple GUI Button**

```cpp
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
```

### Animation and Interpolation

Introduced in U++ 2025.1, animation helpers enhance UI dynamics:

- **`Lerp(a, b, t)`**: Linearly interpolates between two values.
- **`Animate(...)`**: Animates widget properties over time.

**Example: Animated Button**

```cpp
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
```

### Layout System

The U++ Layout Designer saves UI designs as `.lay` files, focusing on arrangement, not parent-child relationships.

**Workflow:**

1.  Design UI in TheIDE, saving as `ui/MyLayout.lay`.
2.  Add the `.lay` file to the `.upp` file.
3.  Use `LAYOUTFILE` macro in the header before including `<CtrlCore/lay.h>`.

**Example:**

```cpp
// In MyWindow.h
#define LAYOUTFILE <MyApp/ui/MyLayout.lay>
#include <CtrlCore/lay.h>

class MyWindow : public WithMyLayout<TopWindow> { /* ... */ };
```

**Layout Rules:**

- **Parenting:** Define parent-child relationships in C++ code, not `.lay` files.
- **One Layout per File:** Use one `LAYOUT(...)` block per `.lay` file.
- **Dynamic Containers:** Declare `Splitter` or `TabCtrl` panes as C++ member variables and assign in the constructor.
- **ParentCtrl:** Use as a programmable container, not for defining relationships in `.lay`.
- **Naming:** Ensure `LAYOUT(ClassName, ...)` matches the class name in `WithClassName<...>`.
- **Panel Switching:** Use `ParentCtrl` with `.Show()`/`.Hide()` for dynamic visibility.

**Example: Image Viewer**

```cpp
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
```

### RichText and RichEdit

For `RichEdit`, include `<RichEdit/RichEdit.h>` and manage text via a `RichText` object.

**Example Workflow:**

- Append formatted text to `RichText`.
- Update `RichEdit` with `Set()`.
- Scroll to the bottom with `MoveEnd()`.

**Custom Object Example:**

```cpp
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
```

## Coding Standards and Best Practices

### Coding Conventions

- **U++ Version:** Use 2025.1 (build 17799) or newer.
- **Naming:** Use `PascalCase` for classes, `camelCase` for methods/variables, `UPPER_CASE_SNAKE_CASE` for constants.
- **Header Guards:** Use `#pragma once`.
- **Formatting:** 4-space indentation, clean and concise code.
- **API Stability:** Only add to public APIs, avoid breaking changes.
- **Inlining:** Inline functions ≤ 4 lines.
- **Dark Mode:** Use `SColor...` or `AColor...` for theme-derived colors.
- **Comments:** Use T++ doc style for formal documentation; `//` for implementation notes. Avoid Doxygen.
- **Patterns:** Avoid the Builder pattern, as it’s not idiomatic in U++.

### Asset Management

- **Images (`.iml`):** Add via TheIDE’s Image Designer; compiled into the application.
- **Application Icon (`.ico`):** Place in the project root. Avoid overwriting with “save .ico and .png” in Image Designer.

### Development Philosophy

- **Verify Features:** Check `uppsrc` for feature existence before implementation.
- **U++ Ecosystem:** Prefer U++ libraries over external dependencies like Boost or `nlohmann/json`.
- **Source of Truth:** Consult `uppsrc` headers and examples for clarity.

### AI Collaboration Directives

- **User Code as Source:** Never regress or remove user-added functionality.
- **No Unrequested Logic:** Avoid adding unrequested features or controls.
- **API Verification:** Confirm U++ class/function usage against `uppsrc` or documentation.

## Smart Pointers and Ownership

Choosing the correct smart pointer ensures proper resource management:

- **`One<T>`:** For unique, movable ownership, equivalent to `std::unique_ptr`.
- **`Ptr<T>` (with `Pte<T>`):** For shared, reference-counted ownership, similar to `std::shared_ptr`.

**Pitfall Example:** Using `One<T>` for shared ownership caused heap corruption. Use `Ptr<T>` when multiple objects need to share a resource’s lifetime.

**Best Practice:**

```cpp
if (live_) live_->anim = nullptr; // Prevent scheduler access after destruction
Scheduler::Inst().Remove(live_);
live_ = nullptr;
```

## Common Pitfalls and Solutions

### General Pitfalls

- **Dangling Ctrl:** Null-check `owner->GetTopWindow()` to avoid crashes.
- **SetTimeCallback:** Use `SetTimeCallback(ms, &TickFn, &cookie)` with `int` cookie.
- **Function Lifetime:** Store `Function<>` in owning objects to prevent heap issues.
- **Mixed Ownership:** Consistently use one ownership strategy (`Ptr`, `One`, or raw pointers).
- **Lambda Captures:** Capture values or use `Ptr<>` to avoid dangling pointers.
- **Global Ctrl Objects:** Use `Single<>` or heap allocation, not `static Ctrl`.
- **Timer Callbacks:** Guard against stale objects in timer loops.

**Crash Prevention Cheat-Sheet:**

| Pattern | Quick Test | Bullet-Proof Habit |
| --- | --- | --- |
| Dangling Ctrl | Crash after window close → stack in `Value::IsNull()` | Null-check `owner->GetTopWindow()`. |
| SetTimeCallback signature | Compiler: “assigning void to void\*” | Use `int` cookie in `SetTimeCallback`. |
| Function<> lifetime | Debug CRT → “invalid heap pointer” | Store `Function<>` in owning object. |
| Mixed ownership | Count `delete` vs constructors | Stick to one ownership strategy. |
| Lambda captures raw pointer | Search for `[=] { use(ptr); }` | Capture values or use `Ptr<>`. |
| Missing U++ type | Error: “Rect is undefined” | Include `<Core/Gtypes.h>` or qualify `Upp::Rect`. |
| Global static Ctrl | Crash on exit | Use `Single<>` or heap allocation. |
| Namespace collision | Error: “reference to non-static member” | Fully qualify U++ types in lambdas. |
| Timer callback after destroy | Crash on window close | Check owner existence in timer loop. |
| Double delete | Debug CRT → “invalid heap pointer” | Assign one owner, use raw pointers for observers. |

### UI and Graphics Pitfalls

#### Name Collisions

Avoid identifiers like `near`, `far`, `min`, `max`, or `GetMessage` due to potential macro conflicts.

**Example:**

```cpp
// BAD
auto near = [&](Pointf a) { /* ... */ };

// GOOD
auto is_near = [&](Pointf a) { /* ... */ };
```

#### Point vs. Pointf

Use explicit casts to avoid narrowing errors with `Pointf`.

**Example:**

```cpp
int half = 40;
Pointf local[4] = {
    Pointf((double)-half, (double)-half),
    Pointf((double) half, (double)-half),
    Pointf((double) half, (double) half),
    Pointf((double)-half, (double) half),
};
```

#### Colors and Alpha

Some backends ignore alpha in `DrawText`. Animate color or position instead.

**Example:**

```cpp
RGBA a{31,41,55, (byte)alpha};
Color c(a);
w.DrawRect(rc, c);
w.DrawText(x, y, "Label", f, c); // Alpha may be ignored
```

#### Clip Stacks

Always pair `Clipoff()` with `End()`.

**Example:**

```cpp
w.Clipoff(0, 0, sz.cx, sz.cy);
w.End();
```

#### Mouse Drag

Use `SetCapture()`/`ReleaseCapture()` for drag operations.

**Example:**

```cpp
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
```

#### Responsive Layouts

Recompute sizes in `TopWindow::Layout()` to avoid clipping.

**Example:**

```cpp
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
```

#### Animation Timing

Use normalized timing helpers for smooth animations.

**Example:**

```cpp
static inline double Seg01(double p, double a0, double a1) {
    if (p <= a0) return 0.0; if (p >= a1) return 1.0; return (p - a0) / max(1e-9, (a1 - a0));
}
static inline int LerpI(int a, int b, double t) { return int(a + (b-a)*t + 0.5); }
```

#### Exception-Safe Timers

Guard timer callbacks against exceptions.

**Example:**

```cpp
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
```

#### Fonts and Metrics

Recalculate text sizes on resize.

**Example:**

```cpp
Font title = StdFont().Bold().Height(clamp(sz.cy/10, 14, 42));
Size ts = GetTextSize("Hello", title);
int x = (sz.cx - ts.cx)/2;
```

#### Pixel-Precise Drawing

Round floats to integers for stable rendering.

**Example:**

```cpp
int x = int(center + radius * cos(theta) + 0.5);
int y = int(center + radius * sin(theta) + 0.5);
```

#### Balanced Transforms

Ensure every transform or clip is closed.

**Example:**

```cpp
w.Clipoff(0, 0, sz.cx, sz.cy);
// Draw operations
w.End();
```

#### Manual Timing

Use manual ticking for deterministic animations.

**Example:**

```cpp
int64 wall_now = msecs();
if (manual_last_now == 0) manual_last_now = wall_now;
int64 dt = wall_now - manual_last_now;
if (max_ms_per_tick > 0) dt = min<int64>(dt, max_ms_per_tick);
manual_last_now += max<int64>(0, dt);
RunFrame(manual_last_now);
```

#### Array Lengths

Use portable alternatives to `__countof`.

**Example:**

```cpp
template <class T, size_t N> constexpr int CountOf(T (&)[N]) { return int(N); }
int n = CountOf(kEases);
```

#### Drag Math

Map pixel coordinates to normalized values.

**Example:**

```cpp
const int inset = 6;
double sx = 200.0 / (sz.cx - 2*inset);
double sy = 200.0 / (sz.cy - 2*inset);
Pointf nf(clamp((p.x - inset) * sx / 200.0, 0.0, 1.0),
          clamp(1.0 - (p.y - inset) * sy / 200.0, 0.0, 1.0));
p0.y = nf.y;
p3.y = nf.y;
```

#### Consistent Layout Styles

Choose either anchored layouts or manual `SetRect()`, not both.

#### Thread Safety

Keep GUI operations on the main thread, using callbacks for background updates.

#### Performance Optimization

- Mark large types as `Moveable<T>` for efficient `Vector` operations.
- Use `Vector<T>::SetCount(n)` for preallocation to avoid repeated `Add()`.

#### Logging

Use `Cout()` for diagnostics, avoiding debug prints in hot paths.

## Example Applications

### Responsive Tile Grid

```cpp
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
```

### Safe Dragging Control

```cpp
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
```

## Appendices

### General Development Guidelines

- **Canonical Sources:** Verify against `uppsrc` or official documentation.
- **Version-Neutral APIs:** Show both old and new signatures if APIs change.
- **Memory and Ownership:** Favor value semantics; use `Null` carefully.
- **Widgets:** Avoid global/static `Ctrl` objects; use `Single<>` or factories.
- **Threading:** Restrict GUI operations to the main thread with `GuiLock`.
- **Static Linking:** Default to static binaries.
- **OOM Policy:** U++ aborts on allocation failure; avoid `try/catch` for OOM.
- **Leak Detection:** Use `MemoryBreakpoint` and `MemoryIgnoreLeaksBlock`.
- **JSON:** Use `Core/JSON.h` exclusively.

### Official Documentation Links

- **Overview:** https://www.ultimatepp.org/www$uppweb$overview$en-us.html
- **Docs Hub:** https://www.ultimatepp.org/www$uppweb$documentation$en-us.html
- **Core Tutorial:** https://www.ultimatepp.org/srcdoc$Core$Tutorial$en-us.html
- **GUI Tutorial:** https://www.ultimatepp.org/srcdoc$CtrlLib$Tutorial$en-us.html
- **Containers:** https://www.ultimatepp.org/srcdoc$Core$NTL_en-us.html
- **Caveats:** https://www.ultimatepp.org/srcdoc$Core$Caveats_en-us.html
- **Leak Guide:** https://www.ultimatepp.org/srcdoc$Core$Leaks_en-us.html
- **Design Decisions:** https://www.ultimatepp.org/srcdoc$Core$Decisions_en-us.html
- **RichText (QTF):** https://www.ultimatepp.org/srcdoc$RichText$QTF_en-us.html

### GitHub Source Links

- **Master Branch:** https://github.com/ultimatepp/ultimatepp/tree/master
- **Next 2025.1 Branch:** https://github.com/ultimatepp/ultimatepp/tree/next2025_1
- **Key Files:**
    - https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/CtrlCore/CtrlCore.h
    - https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/Core/Function.h
    - https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/Core/One.h
    - https://github.com/ultimatepp/ultimatepp/blob/master/uppsrc/Core/Ptr.h

### Widget API Reference

#### ArrayCtrl

- `int GetCount() const`
- `void Clear()`
- `Value Get(int row, int col) const`
- `void Set(int row, int col, const Value& v)`
- `void Add(...)`
- `void Insert(int row, ...)`

#### Bar, MenuBar, ToolBar

- `Item& Add(const char* text, const Image& image, const Callback& cb)`
- `Item& Sub(const char* text, const Function<void(Bar&)>& submenu)`
- `void Separator()`
- `void Break()`

#### Button, ButtonOption

- `Button& SetImage(const Image& img)`
- `Button& Ok()` / `Cancel()` / `Exit()`
- `ButtonOption& Set(bool b)`
- `bool Get() const`

#### Option

- `void Set(bool b)` / `operator=(bool b)`
- `bool Get() const`
- `void SetGroup(int g)`

#### Static Widgets

- `SeparatorCtrl& Margin(int w)` / `Margin(int l, int r)`
- `SeparatorCtrl& SetSize(int w)`

# DnD acceptance flow:
In DragAndDrop(Point, PasteClip&), call AcceptImage(d)/AcceptFiles(d) and set a visual state (d.IsAccepted()) to cue users. Reset state in DragLeave()/CancelMode(). For file→image drops, add plugin/png/jpg/bmp to the EXE’s uses.

Custom Display for DropList: Subclass Display, override Paint, and attach with SetDisplay(Single<YourDisplay>()). Align text vertically using GetTextSize and add a faint baseline for clarity.

JSON best practice: Prefer Value/ValueMap/ValueArray with type checks; wrap access in small typed helpers. For large inputs, use CParser to stream segments.

ImageDraw basics:
Create an offscreen raster with ImageDraw w(cx, cy); using Draw ops. Convert to Image by assignment (Image img = w;). Use theme-aware colors (SColor...) when appropriate.

Choosing a renderer:

# ImageDraw → quick raster icons/bitmaps.

BufferPainter(ImageBuffer, MODE_ANTIALIASED) → antialiased vectors/gradients to raster.

PaintingPainter + DrawPainting → resolution-independent recorded vector scenes (printing/export).

Pixel alignment:
When drawing text onto small images, compute GetTextSize() and position with integer coordinates to avoid blur.

Performance tip:
Cache Image results and blit in TopWindow::Paint; avoid regenerating raster content every frame unless inputs change.

If you’ve got more snippets like this (especially around ImageBuffer, Painter filters, or printing/Report), drop them in and I’ll fold them into the assistant and propose concise guide entries.

# Http server scaffolding:
server.Listen(), per-connection socket.Accept(server), HttpHeader http; http.Read(socket);, optional body via socket.GetAll(len), reply with HttpResponse(socket, http.scgi, ...). If using multiple worker threads around a single listening socket, serialize Accept (like your StaticMutex) or split per-thread TcpSocket with Listen() on one and Accept() from that one under a lock.

DnD image pattern: override DragAndDrop, call AcceptImage/GetImage, track d.IsAccepted() for hover feedback; reset state in DragLeave/CancelMode.

GridCtrl cookbook: columns/rows, .Edit(...) per column, DropList with .SetConvert(...), summary row helpers (DoSum/DoMin/DoCount), fixed rows/cols, wrap text, toolbar/dragging, and XML round-trip via StoreAsXML/LoadFromXML.

ArrayCtrl per-column embedded controls using ColumnAt(i).Ctrls(factory) + .Edit(editor).

# Setup & Packaging

Use Assemblies to include your repo root + uppsrc. .upp lives at package root.

LAYOUTFILE <.../foo.lay> + #include <CtrlCore/lay.h> to bind layouts.

EXE: mainconfig "" = "GUI" or "CONSOLE".
Library: empty mainconfig. Build EXE separately that depends on it.

Image IO: add plugin/png, plugin/jpg, plugin/bmp in EXE uses if loading common formats.

# Painter & Draw

Prefer BufferPainter over an ImageBuffer inside TopWindow::Paint.
For vector caching/printing: PaintingPainter + DrawPainting/PrinterJob.

Path ops: Move/Line/Close, Quadratic/Cubic/Arc, or SVG Path("M...").

Fills: solid + linear/radial gradients (ColorStop for multi-stop).
Gradient stroke: Stroke(width, x1,y1,c1, x2,y2,c2).

Image-as-brush: Fill(img, x1,y1, x2,y2, flags) with FILL_PAD/REPEAT/REFLECT/HREFLECT/VPAD/FAST.

Filters: FILTER_NEAREST/BILINEAR/BSPLINE/COSTELLA/BICUBIC_MITCHELL/CATMULLROM/LANCZOS3.

Text on path: BeginOnPath(t[,abs]) ... End(), align using FontInfo ascent/descent.

Scoping: Always balance Begin/End, BeginMask/End, Clip scopes. Use EvenOdd/Div for overlaps.

# Controls & Display

Static: StaticText (ink/font/align/valign/orientation/image), StaticRect, ImageCtrl, DrawingCtrl, SeparatorCtrl(Style).

Display system: subclass Display::Paint, use AttrText builder. DisplayWithIcon decorates, PaintRect renders Value with Display.

Push controls: Pusher, Button(Style), DataPusher, SpinButtons frame.

DropChoice: PopUpList, custom Display & Convert, DropLines, DropWidth, AlwaysDrop, RdOnlyDrop, HideDrop, UpDownKeys.

HeaderCtrl: Split/drag, modes Proportional/Reduce*/Absolute/Fixed. Works as CtrlFrame.

Text: TextCtrl/DocEdit with undo/redo, selection, printing.

DnD: override TopWindow::DragAndDrop/DragLeave/CancelMode. Use AcceptImage(d)/GetImage(d); hover feedback via d.IsAccepted().

XML browser: FileList + TreeCtrl + XmlParser, banner via FrameTop<StaticRect>; show source in LineEdit at error with SetCursor(GetPos(line,col)).

# GridCtrl & ArrayCtrl

Columns: AddColumn(...).Width(...), Fixed/Min/Max, WrapText.

Options: Indicator, HorzGrid/VertGrid, ResizingCols/Rows, MovingCols/Rows, LiveCursor, DrawFocus, Chameleon, EditMode(Edit row/cell).

Data/editors: per-column EditInt/ EditString/ DropTime/ DropList(SetConvert).
Summary: DoSum/DoCount/DoMin.

Selection/sort: MultiSelect, SelectRow, Sorting, MultiSorting.
Count: SetRowCount/AddRow/Append/RemoveLast.

Persist: StoreAsXML/LoadFromXML.

ArrayCtrl: per-column Ctrls(factory) (e.g., DropList/Button/Edit) + .Edit(...) for inline editors.

Tooling panel pattern: mirror grid options (indicator, grids, resizing, coloring) and call through to .Repaint().

# Image & GL

ImageDraw: Offscreen raster composition → assign to Image → DrawImage in Paint. Use SColor* theme colors.

GLDraw: CreateGLTexture, GetTextureForImage; GLProgram(Compile/Link/Use). GLDraw implements SDraw using shaders (gl_image, etc.). GLOrtho for projection.

# Data: JSON & Reports

JSON parse/build: ParseJSON, AsJSON, Json/JsonArray. Partial streaming with CParser. Index via Value.

Report: Report (DrawingDraw/PageDraw), header/footer/margins/pagesize, ReportView/ReportWindow, Pdf()/Print(), QtfReport(...).

# Networking

Minimal HTTP/SCGI server:

server.Listen(port, backlog).

Worker: TcpSocket s; s.Accept(server).
HttpHeader http; http.Read(s);
Read body via http.GetContentLength() then socket.GetAll(len).
Respond: HttpResponse(s, http.scgi, 200, "OK", "text/html", body).

MT: _MULTITHREADED + Thread::Start(callback(Server));

Serialize Accept with a StaticMutex if needed.

# Concurrency & GUI Safety

GUI must be touched on the main thread. Use PostCallback or dedicated signals; if absolutely needed from workers, wrap with GuiLock briefly. Mind timer/owner lifetimes.

Troubleshooting

Missing CtrlLib/CtrlCore: assembly nests misconfigured.

WinMain undefined: library flagged as EXE or vice versa.

Image load fails: missing plugin/png|jpg|bmp in EXE uses.

Painter glitches: unbalanced Begin/End or Clip; forgot EvenOdd/Div.

Grid editor not activating: .Editing() + correct .EditMode; embedded ctrl needs .WantFocus().

# Example Prompt Starters (for the GPT)

Spin up a minimal multithreaded HTTP server.

Turn GridCtrl into a property editor.

Serialize GridCtrl to XML and load back.

Add summary rows (sum/min/max) to GridCtrl.

Make a DropList column with a custom Display.

control ownership/lifetime (Parent owns children, safe unique/shared patterns with One<>, Ptr<>, Pte<>)

callbacks & capture safety (THISBACK, clearing callbacks before teardown)

frames/layout (FrameTop<StaticRect>, ParentCtrl, Splitter)

Display/Convert/DisplayPopup/PopUpList/DropChoice wiring

DnD with PasteClip (AcceptImage, hover feedback via d.IsAccepted())

XML viewer pattern (TreeCtrl + XmlParser + error banner + caret placement)

GridCtrl advanced configuration, editors, coloring, sorting, XML round-trip

JSON partial parse patterns with CParser

ImageDraw raster composition

HTTP/SCGI server pattern with TcpSocket, HttpHeader, MT & StaticMutex

GUI safety, timers, and memory discipline (value semantics, One<>, GuiLock)

