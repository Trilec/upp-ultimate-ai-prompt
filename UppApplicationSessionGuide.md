### U++ Application Development Guide

This guide serves as a living document for developing applications with the U++ framework. It consolidates our discoveries, best practices, and coding standards to ensure consistency and provide a head start for any new development work.

### 1. Introduction: The U++ Philosophy

U++ is a C++ rapid application development framework that aims to simplify complex tasks, especially for desktop applications. Its core philosophy revolves around deterministic resource management and tight integration between the library and the build system.

A key principle in U++ is that most objects are tied to a logical scope, which drastically reduces the need for manual memory management with `new` and `delete`. Pointers are primarily for pointing, not for owning heap resources. This results in automatic and deterministic resource management that is often more predictable than garbage-collected languages.

### 2. Project Structure & Build System

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
¦
+-- design/          ? Diagrams, notes, and design documentation
    +-- design.md
```

#### The `.upp` Package File

The `.upp` file is the heart of a U++ package. It declares dependencies, source files, and build configurations.

*   **Location:** Always place the `.upp` file in the root of the package directory. TheIDE will not detect it otherwise.
*   **Dependencies:** Use the `uses` directive to link against other packages like `Core` and `CtrlLib`.
*   **Files:** List all source (`.cpp`), header (`.h`), and layout (`.lay`) files.
*   **Main Package:** The `mainconfig` section marks a package as an executable application, making it appear in TheIDE's "Select main package" dialog.

```upp
description "U++ asynchronous state machine with animation support";

uses
    Core,
    CtrlLib;

file
    ui/MyLayout.lay,
    main.cpp,
    include/statemachine.h,
    src/statemachine.cpp;

mainconfig
    "" = "";   // Marks this as a MAIN package
```

### 3. The Core Library: Your Essential Toolkit

The `Core` library provides the fundamental building blocks for any U++ application.

#### Containers: The U++ Way

U++ provides its own container library, optimized for performance and move semantics.

*   **`Vector<T>`**: A dynamic array that is extremely fast. It requires that the type `T` be moveable. Recent U++ versions enforce this with an `is_upp_guest<T>` check at compile time.
*   **`Array<T>`**: A more flexible dynamic array that has no special requirements for type `T`.
*   **`One<T>`**: A smart pointer for single object ownership. Using `One<T>` inside a container like `Vector<One<MyObject>>` is the preferred way to manage unique ownership of complex objects.
*   **Other Containers**: U++ also provides `BiVector`, `Index`, `VectorMap`, and `ArrayMap`.

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

*   **File Streams**: `FileIn`, `FileOut`, and `FileAppend` provide easy file access.
*   **Serialization**: `StoreAsString` and `LoadFromString` can serialize many U++ objects.
*   **JSON**: For web and configuration tasks, U++ has a built-in JSON parser in `Core/JSON.h`. 
* Do not use third-party JSON libraries, and is a crucial best practice for a pure U++ project
JSON functionality has been consolidated into the Core library. The header is located in  `uppsrc/Core/JSON.h`. The functions and templates  (ParseJSON, AsJSON, Jsonize, etc.) are the correct, modern way to handle JSON within the framework. 

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

### 4. Building Graphical User Interfaces (GUIs)

The `CtrlLib` package contains everything needed to build a modern GUI.

#### Widgets and Ownership

In U++, widgets are plain C++ objects owned by your code, not by a global toolkit.

#### Layouts: From `.lay` to C++

UIs are visually designed in the **Layout Designer** and saved as `.lay` files.

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

**Common Error:** A `fatal error: 'MyLayout.layout' file not found` message means the `.lay` file wasn't saved correctly in TheIDE, or it's missing from the `.upp` file.

#### Events and Callbacks: Using `Upp::Function`

U++ uses `Upp::Function` for callbacks. An unassigned `Upp::Function` does nothing when called, preventing crashes. Lambdas are the modern way to assign callbacks using the `<<=` operator.

#### Animation and Interpolation

U++ 2025.1 introduced powerful animation and interpolation helpers.

*   **`Lerp(a, b, t)`**: Linearly interpolates between two values.
*   **`Animate(...)`**: Animates a widget's properties over time.

***

### 5. Project Coding Standards & Mandate

*   **U++ Version**: Build with 2025.1 (build 17799) or newer.
*   **Identifiers**: Use `CamelCase`.
*   **Formatting**: Keep code clean and sorted.
*   **Conciseness**: Write concise code and comments.
*   **API Stability**: Change public APIs only by addition.
*   **Inlining**: Inline small functions (= 4 lines).
*   **Dark Mode**: Derive colors from the theme using `SColor...` or `AColor...`.

To maintain a robust and self-contained application, we must combat the risk of design based on assumption. Past experience has shown that assuming a library or feature exists can lead to significant wasted effort.

*   **Verify, Don't Assume**: Before designing around any feature (e.g., WebSockets, REST clients), you **must** verify its existence in the official U++ source code for our target version. Do not trust external knowledge sources, including AI assistants, without this verification.
*   **U++ Ecosystem First**: The default is to use the U++ ecosystem. Do not introduce external dependencies (e.g., Boost, `nlohmann/json`) or build tools (e.g., CMake) without explicit team discussion and approval.
*   **Check the Source**: The ultimate source of truth is the `uppsrc` directory. When in doubt, check the headers and examples there.

***

How does this feel? It directly addresses the root cause of the problem you described—the dangerous act of assuming—and elevates the solution to a core project principle. It explicitly warns against the exact failure mode we experienced and tells developers *how* to avoid it.

### 6. Code Cookbook: Practical Examples

A collection of standalone, official examples to demonstrate common U++ patterns.

#### Core Library Snippets (Console Apps)

**Containers Overview**
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

**String Cookbook**
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

**Lerp Utility**
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

#### GUI Snippets (GUI Apps)

**Simple GUI Button**
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

**Animated Button**
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

**ImageView (File Browser + Preview)**
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

### 7. Further Resources

*   **Official Examples**: Explore `examples/AnimateCtrlGeometry` and `examples/LinearInterpolation` in the U++ source tree.
*   **Changelog**: See the Ultimate++ website ? **Roadmap** for a list of all features in the 2025.1 release.
*   **Documentation**: [Official U++ Documentation](https://www.ultimatepp.org/www$uppweb$documentation$en-us.html)
*   **Source Repository**: [U++ GitHub Repository (next2025_1 branch)](https://github.com/ultimatepp/ultimatepp/tree/next2025_1)