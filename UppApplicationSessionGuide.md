### U++ Application Development Guide V05

This guide serves as a living document for developing applications with the U++ framework. It consolidates our discoveries, best practices, and coding standards to ensure consistency and provide a head start for any new development work.

### 1\. Introduction: The U++ Philosophy

U++ is a C++ rapid application development framework that aims to simplify complex tasks, especially for desktop applications. Its core philosophy revolves around deterministic resource management and tight integration between the library and the build system.

A key principle in U++ is that most objects are tied to a logical scope, which drastically reduces the need for manual memory management with `new` and `delete`. Pointers are primarily for pointing, not for owning heap resources. This results in automatic and deterministic resource management that is often more predictable than garbage-collected languages.

### 2\. Project Setup & Build System

U++ uses a self-contained build system that does not rely on external tools like Make or CMake. Understanding its components is key to setting up a project correctly.

#### Basic Folder Structure

A well-organized project is easier to navigate. Here is a recommended structure:

```
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
-- assets/           ? assets like icons or json files etc
¦   +-- myicon.png
¦
+-- design/          ? Diagrams, notes, and design documentation
    +-- design.md
```

#### The `.upp` Package File

The `.upp` file is the heart of a U++ package. It declares dependencies, source files, and build configurations.

-   **Location:** Always place the `.upp` file in the root of the package directory. TheIDE will not detect it otherwise.
-   **Dependencies:** Use the `uses` directive to link against other packages like `Core` and `CtrlLib`.
-   **Files:** List all source (`.cpp`), header (`.h`), and layout (`.lay`) files.
-   **Main Package:** The `mainconfig` section marks a package as an executable application, making it appear in TheIDE's "Select main package" dialog.

**Example `.upp` File:**
*Important: the .upp file DOES NOT support comments.*
```upp
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
```

#### The `.upp` / `.h` / `.lay` Connection: A Critical Link

A common and difficult-to-diagnose build failure occurs when the build system cannot find a `.lay` file. This is caused by a path mismatch between the `.upp` file and the C++ header.

-   **Rule:** The path in `#define LAYOUTFILE <path>` must be **relative to an `include` path** defined in the `.upp` file.
-   **Example:** If your `.upp` file contains `include /ui;`, and your layout is at `ui/MyLayout.lay`, then your header file must use `#define LAYOUTFILE <MyLayout.lay>`.
-   **Incorrect:** `#define LAYOUTFILE <MyPackage/ui/MyLayout.lay>` will fail because the build system will prepend the include path, resulting in a search for `ui/MyPackage/ui/MyLayout.lay`.

### 3\. The Core Library: Your Essential Toolkit

The `Core` library provides the fundamental building blocks for any U++ application.

#### Containers: The U++ Way
U++ provides its own container library, optimized for performance and move semantics.
-   **`Vector<T>`**: A dynamic array that is extremely fast. It requires that the type `T` be moveable.
-   **`Array<T>`**: A more flexible dynamic array that has no special requirements for type `T`.
-   **`One<T>`**: A smart pointer for single object ownership. Using `One<T>` inside a container like `Vector<One<MyObject>>` is the preferred way to manage unique ownership of complex objects.
-   **Other Containers**: U++ also provides `BiVector`, `Index`, `VectorMap`, and `ArrayMap`.

#### String Manipulation
`String` is a fundamental U++ type with a rich set of methods for common tasks. For direct character modification, use a `StringBuffer`.

#### Value, Null, and Polymorphism
The `Value` type is a powerful variant-like class that can hold data of almost any type. `Null` represents an empty or invalid value. `ValueArray` and `ValueMap` allow for collections of heterogeneous data.

#### Error Handling
U++ uses an exception model based on the `Exc` class.
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

#### Streams and Serialization (including JSON)
U++ provides a stream interface for I/O and a built-in serialization framework.
-   **File Streams**: `FileIn`, `FileOut`, and `FileAppend` provide easy file access.
-   **Serialization**: `StoreAsString` and `LoadFromString` can serialize many U++ objects.
-   **JSON**: For web and configuration tasks, U++ has a built-in JSON parser in `Core/JSON.h`. Do not use third-party JSON libraries.

#### Ranges and Algorithms
U++ provides a set of algorithms that operate on its containers and ranges.
```cpp
Vector<int> data {3, 1, 2};
Sort(data);
int index = FindIndex(data, 2); // index is 1
auto subrange = SubRange(data, 1, 2); // Range containing {2, 3}
```

#### Multithreading and Parallelism
The `CoWork` and `Thread` classes provide high-level abstractions for concurrent and parallel execution. `CoPartition` can distribute tasks over multiple cores.
```cpp
// Distribute a task over multiple cores
Vector<int> data{ /* ... */ };
CoPartition(data, [](const auto& subrange){ /* process subrange */ });
```

### 4\. Building Graphical User Interfaces (GUIs)

The `CtrlLib` package contains everything needed to build a modern GUI.

#### Widgets and Ownership
In U++, widgets are plain C++ objects owned by your code, not by a global toolkit.

#### Events and Callbacks
U++ uses `Upp::Function` for callbacks. An unassigned `Upp::Function` does nothing when called, preventing crashes. Lambdas are the modern way to assign callbacks using the `<<=` operator.

#### Animation and Interpolation
U++ 2025.1 introduced powerful animation and interpolation helpers.
-   **`Lerp(a, b, t)`**: Linearly interpolates between two values.
-   **`Animate(...)`**: Animates a widget's properties over time.

#### The U++ Layout System: Principles and Best Practices
UIs are visually designed in the **Layout Designer** and saved as `.lay` files. The single most important principle to understand is that the `.lay` file is a tool for **arrangement**, not for defining **parent-child relationships**.

**Basic Workflow:**
1.  **Create Layout:** Design your UI in TheIDE and save it (e.g., `ui/MyLayout.lay`).
2.  **List in `.upp`:** Add the `.lay` file to your package's `.upp` file.
3.  **Include in Code:** Use the `LAYOUTFILE` macro in your header file *before* including `<CtrlCore/lay.h>`.

```cpp
// In MyWindow.h
#define LAYOUTFILE <MyApp/ui/MyLayout.lay>
#include <CtrlCore/lay.h>

// This creates a template: template<class T> struct WithMyLayout<T>
class MyWindow : public WithMyLayout<TopWindow> { /* ... */ };
```

**Layout Rules:**

-   **Rule 1: Parenting is a C++ Responsibility**
    -   **DO:** Define all parent-child widget relationships exclusively in C++ code.
    -   **DO NOT:** Assume the `.lay` file defines parent-child relationships.

-   **Rule 2: One Layout Block Per File**
    -   **DO:** Enforce a strict one-to-one mapping: one C++ component class corresponds to one `.lay` file with one `LAYOUT(...)` block.
    -   **DO NOT:** Place multiple `LAYOUT(...)` blocks in a single `.lay` file. TheIDE will delete all but one upon saving.

-   **Rule 3: Handling Dynamic Containers (`Splitter`, `TabCtrl`)**
    -   **DO:** Declare the manager widget (`Splitter`) and its panes (`ParentCtrl`, etc.) as member variables in the C++ header.
    -   **DO:** Assign the panes to their roles in the C++ constructor (e.g., `splitter.Horz(leftPane, rightPane);`).
    -   **DO NOT:** Define the panes as `ITEM`s in the same `.lay` file as their manager.

-   **Rule 4: The Purpose of `ParentCtrl`**
    -   **DO:** Use `ParentCtrl` as a generic, programmable container in your C++ class, populating it with `panelHost.Add(...)`.
    -   **DO NOT:** Mistake `ParentCtrl` for a tool to create parent-child relationships within the `.lay` file.

-   **Rule 5: Layout Naming Must Be Exact**
    -   **DO:** Ensure the name in `LAYOUT(ClassName, ...)` exactly matches the class name in `class MyClass : public WithClassName<...>;`.
    -   **DO NOT:** Use different names. This will break `CtrlLayout` and cause `undeclared identifier` errors.

-   **Rule 6: Use Real Widgets for Panel Switching**
    -   **DO:** Use a standard `ParentCtrl` as a host and manage visibility by calling `.Show()` and `.Hide()` on the child panels directly.
    -   **DO NOT:** Assume a specialized "switcher" widget exists. The assistant may hallucinate class names like `StaticSwitcher`. Stick to verified components.

### 5\. Project Mandates & Best Practices

This section outlines the specific rules and philosophies governing our project.

#### Coding Standards
-   **U++ Version**: Build with 2025.1 (build 17799) or newer.
-   **Naming Conventions:** `PascalCase` for classes/structs, `camelCase` for methods/variables, `UPPER_CASE_SNAKE_CASE` for constants.
-   **Header Guards:** Utilize `#pragma once`.
-   **Formatting:** Use 4-space indentation. Keep code clean, sorted, and concise.
-   **API Stability**: Change public APIs only by addition.
-   **Inlining**: Inline small functions (= 4 lines).
-   **Dark Mode**: Derive colors from the theme using `SColor...` or `AColor...`.
-   **Comments**: Use T++ doc style for formal documentation; simple `//` comments are fine for implementation notes. Avoid Doxygen.
-   **Patterns**: Avoid the Builder pattern as it is not idiomatic in U++.

#### Asset Management
-   **Images (`.iml`):** The `.iml` file contains binary image data. Images **must** be added via TheIDE's Image Designer. They are compiled into the application.
-   **Application Icon (`.ico`):** The `.ico` file in the project root is the application icon. Be aware that using "save .ico and .png" in the Image Designer will overwrite this file and save image assets to the root directory. This option should usually be unchecked.

#### Development Philosophy
-   **Verify, Don't Assume**: Before designing around any feature, you **must** verify its existence in the official U++ source code for our target version. Do not trust external knowledge sources without this verification.
-   **U++ Ecosystem First**: The default is to use the U++ ecosystem. Do not introduce external dependencies (e.g., Boost, `nlohmann/json`) or build tools (e.g., CMake) without explicit team approval.
-   **Check the Source**: The ultimate source of truth is the `uppsrc` directory. When in doubt, check the headers and examples there.

#### Directives for AI Collaboration
To maximize productivity and minimize errors when working with an AI assistant, the following directives must be enforced.
-   **The User's Code is the Source of Truth:** The AI must never regress a file or remove functionality the user has added. When providing code, the AI must always start from the last known-good state provided by the user.
-   **No Unrequested Logic:** The AI must not hallucinate or add new logic, controls, or functionality that was not explicitly requested.
-   **API Verification is Mandatory:** The AI must not suggest any U++ class or function without first verifying its existence and correct usage against the official documentation or `uppsrc`. This is non-negotiable.

### 6\. Code Cookbook: Practical Examples

A collection of standalone, official examples to demonstrate common U++ patterns.

#### Core Library Snippets (Console Apps)
<details>
<summary><strong>Containers Overview</strong></summary>

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
</details>
<details>
<summary><strong>String Cookbook</strong></summary>

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
</details>
<details>
<summary><strong>Lerp Utility</strong></summary>

```cpp
#include <Core/Core.h>
using namespace Upp;

CONSOLE_APP_MAIN
{
    Color start = Blue, end = Yellow;
    for(double t=0; t<=1; t+=0.25)
        LOG(Format("t=%0.2f  ->  %s", t, AsString(Lerp(start,end,t))));
}
```
</details>

#### GUI Snippets (GUI Apps)
<details>
<summary><strong>Simple GUI Button</strong></summary>

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
</details>
<details>
<summary><strong>Animated Button</strong></summary>

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
</details>
<details>
<summary><strong>ImageView (File Browser + Preview)</strong></summary>

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
</details>
<details>
<summary><strong>RichText & RichEdit</strong></summary>

To use `RichEdit`, you must `#include <RichEdit/RichEdit.h>`.
-   Maintain a `RichText` object as the data model for the text.
-   To add a line, append formatted text to this `RichText` object.
-   Tell the `RichEdit` control to display the updated model using its public `Set()` method.
-   To scroll to the bottom, use the public `MoveEnd()` method, which moves the cursor to the end.

You can also define custom objects to embed in the text:
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
</details>

### 7\. Appendices: Reference Material

#### Appendix A: General Development Guidelines
1.  **Canonical sources** – Verify answers against `uppsrc` or an official doc page.
2.  **Version-neutral replies** – If an API changed, show both signatures and name the branch or tag.
3.  **Memory & ownership** – Favour value semantics; containers use *pick* transfer (pass-by-value moves and clears). `Null` is lost when copied to plain C++ types.
4.  **Widgets** – Never declare `Ctrl`-derived objects as global/static; use `Single<>` or factories.
5.  **Threading & GUI** – Only the main thread may open/close windows or run message loops; guard other threads with `GuiLock`.
6.  **Static linking default** – Ship static binaries unless shared builds are requested.
7.  **Platform limits** – x86 / AMD64 / ARM / PowerPC; little- or big-endian; 32-/64-bit; `Moveable` support required.
-   **OOM policy** – U++ aborts on allocation failure; don’t wrap constructors in `try/catch` for OOM.
-   **Leak-hunting** – Use `MemoryBreakpoint(serial)` from the `.log`; wrap third-party code in `MemoryIgnoreLeaksBlock`.
-   **Quick pitfall check** –
    -   `x.At(i) = x[q];` reference invalidated by reallocation.
    -   `x.Add(x.Top())` / `x.Add(x[0])` copies a soon-invalid reference.
    -   `void Fn(Array<T> a)` moves data out of the caller.
    -   `INITBLOCK/EXITBLOCK` can be dropped; prefer `INITIALIZE/INITIALIZER`.
-   **JSON** – Use `Core/JSON.h` (`ParseJSON`, `AsJSON`, `Jsonize`); never add third-party JSON libs without approval.
-   **Assistant behaviour** – Always verify first, avoid external deps, explain version deltas, return minimal compilable snippets, and warn (gently) when a request clashes with U++ design.

#### Appendix B: Official Documentation Links
-   **Overview** – `https://www.ultimatepp.org/www$uppweb$overview$en-us.html` – High-level goals & RAD ethos.
-   **Docs hub** – `https://www.ultimatepp.org/www$uppweb$documentation$en-us.html` – Master index.
-   **Core tutorial** – `https://www.ultimatepp.org/srcdoc$Core$Tutorial$en-us.html` – `String`, `Vector`, streams, etc.
-   **GUI tutorial** – `https://www.ultimatepp.org/srcdoc$CtrlLib$Tutorial$en-us.html` – First window to callbacks.
-   **Containers / NTL** – `https://www.ultimatepp.org/srcdoc$Core$NTL_en-us.html` – Container specs & move rules.
-   **Caveats** – `https://www.ultimatepp.org/srcdoc$Core$Caveats_en-us.html` – Traps & pitfalls.
-   **Leak guide** – `https://www.ultimatepp.org/srcdoc$Core$Leaks_en-us.html` – Built-in leak detector.
-   **Design decisions** – `https://www.ultimatepp.org/srcdoc$Core$Decisions_en-us.html` – Static linking, OOM abort, endian limits.

#### Appendix C: GitHub Quick-Map
-   **master** – `https://github.com/ultimatepp/ultimatepp/tree/master` – Latest committed code.
-   **next2025_1** – `https://github.com/ultimatepp/ultimatepp/tree/next2025_1` – Upcoming 2025.1 branch.
    -   `uppsrc/Core` – containers, memory, threads, JSON
    -   `uppsrc/CtrlCore` – windowing & event loop
    -   `uppsrc/CtrlLib` – widgets & layouts
    -   `uppsrc/Draw` – 2-D engine
    -   `examples`, `tutorial`, `benchmarks`, `upptst` – demos & tests
    > *Tip:* headers and sources sit in the same folder; press **`t`** in GitHub for fuzzy file search.

#### Appendix D: Verified Widget API Reference
A list of common widgets and their core public methods, with citations from the official U++ documentation.
<details>
<summary><strong>ArrayCtrl</strong></summary>

`ArrayCtrl` displays and edits tabular data.
- `int GetCount() const` – returns the number of rows.
- `void Clear()` – clears all rows.
- `Value Get(int row, int col) const` – retrieves the cell value at (row, col).
- `void Set(int row, int col, const Value& v)` – sets the cell value.
- `void Add(...)` – appends a row with provided values.
- `void Insert(int row, ...)` – inserts a new row at a specific index.
</details>
<details>
<summary><strong>Bar, MenuBar, ToolBar</strong></summary>

`Bar` is the abstract base for `MenuBar` and `ToolBar`.
- `Item& Add(const char* text, const Image& image, const Callback& cb)` – adds a menu or toolbar item.
- `Item& Sub(const char* text, const Function<void(Bar&)>& submenu)` – adds a submenu.
- `void Separator()` – inserts a visual separator.
- `void Break()` – breaks to a new line/column in a `ToolBar`.
</details>
<details>
<summary><strong>Button, ButtonOption</strong></summary>

`Button` is a standard clickable control. `ButtonOption` is a two-state toggle.
- `Button& SetImage(const Image& img)` – sets the button icon.
- `Button& Ok()` / `Cancel()` / `Exit()` – designate default dialog roles.
- `ButtonOption& Set(bool b)` – sets the state of a `ButtonOption`.
- `bool Get() const` – returns the current state of a `ButtonOption`.
</details>
<details>
<summary><strong>Option</strong></summary>

`Option` is a radio-button-style on/off selector.
- `void Set(bool b)` / `operator=(bool b)` – sets the state.
- `bool Get() const` – retrieves the state.
- `void SetGroup(int g)` - Assigns the option to a group, making it mutually exclusive with others in the same group.
</details>
<details>
<summary><strong>Static Widgets</strong></summary>

`StaticRect`, `StaticLine`, `SeparatorCtrl`, etc., draw non-interactive shapes.
- `SeparatorCtrl& Margin(int w)` / `Margin(int l, int r)` – set margins.
- `SeparatorCtrl& SetSize(int w)` – minimal size.
</details>