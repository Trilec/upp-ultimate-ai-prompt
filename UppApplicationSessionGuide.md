### U++ Application Development Session Guide

We will be developing a C++ application using the U++ development framework . The following is the mandate and coding details along with examples to help ensure that any code aligns with the framework's capabilities and standards.

#### Conformity Mandate

- **Ensure U++ Compatibility**: Build & test with 2025.1 (17799) or newer nightlies (e.g. 17913).
- **Maintain Formatting**: Copy-friendly, minimal prose.
- **CamelCase Identifiers**: Consistent across files.
- **Concise Code & Comments**: Prefer imperative phrases, avoid superfluous adjectives.
- **Stable Signatures**: Change public APIs only by addition, never by substitution.
- **Clean, Sorted Code**: Group includes ? types ? funcs; alphabetical where practical.
- **Inline = 4-line functions**.
- **Animation-ready**: Prefer `Animate(...)`, `AnimateProperty(...)`.
- **Dark-mode Aware**: Derive colours via `AColor/SColor` and `DarkTheme()`.
- **64-bit Formatting**: Use `FormatIntBase(int64 ...)`.
- **Logs**: On POSIX read/write under `~/.local/state/u++/log`.

#### Error Handling

```cpp
try {
    FileIn fin("data.txt");
    if(!fin) throw Exc("Cannot open data.txt");
}
catch(const Exc& e) {
    LOG(e); PromptOK(e);
}
```

Additional information:

### 1. Introduction

U++ significantly reduces the complexity of desktop application development. The following example demonstrates an application that calculates the number of days between two dates, updating as the user edits the input fields.

**Example Code:**

```cpp
#include <CtrlLib/CtrlLib.h>

#define LAYOUTFILE <Days/Days.lay>
#include <CtrlCore/lay.h>

class Days : public WithDaysLayout<TopWindow> {
public:
    typedef Days CLASSNAME;
    Days();
};

Days::Days() {
    CtrlLayout(*this, "Days");
    date1 ^= date2 ^= [=] {
        result = IsNull(date1) || IsNull(date2) ? "" :
                 Format("There is %d day(s) between %` and %`", abs(Date(~date1) - Date(~date2)), ~date1, ~date2);
    };
}

GUI_APP_MAIN {
    Days().Run();
}
```

### 2. Everything Belongs Somewhere

In U++, most objects are tied to a logical scope, reducing the need for `new` and `delete` operators. Pointers are used only for pointing to things, not for managing heap resources. This results in deterministic and automatic resource management, similar to or better than garbage-collected languages like Java or C#.

### 3. U++ Containers

U++ avoids standard C++ library containers for GUI development due to the strict copy-constructor requirement of STL containers. Instead, U++ offers two container types:

- **Vector**: Requires `Moveable` for types, providing fast performance.
- **Array**: No type requirements, offering flexibility at a slight performance cost.

### 4. Widget Ownership

Widgets in U++ are always owned by the client code, not the GUI toolkit. This ensures that widgets are independent entities, accessible even when not part of a visible GUI.

**Example:**

```cpp
struct MyDialog {
    Option option;
    EditField edit;
    Button ok;
};
```

### 5. Dialog Templates as C++ Templates

U++ uses C++ templates for layout designs, which are reflected in code as templates that derive from widget classes. This eliminates the need for widget IDs or names.

**Example:**

```cpp
template <class T>
struct WithMyDialogLayout : public T {
    Option option;
    EditField edit;
    Button ok;
};
```

### 6. Value and Null

U++'s `Value` type allows dynamic data storage and retrieval. `Null` represents an empty value and is supported by most U++ types.

### 7. Display and Convert

`Convert` classes in U++ handle bidirectional `Value` conversions, often between logical and textual representations. `Display` classes define how `Value` objects are presented in the GUI.

### 8. Function

U++ uses `Upp::Function`, similar to `std::function`, for handling output actions with C++ lambdas. Unlike `std::function`, `Upp::Function` does nothing when unassigned, making it safer for GUI events.

### 9. U++ Widgets

U++ provides a comprehensive set of widgets, with flexible options for creating custom widgets as needed.

* * *

**Essential Details from the U++ Core**

### 1. Basic Operations

**Logging and Debugging:**

```cpp
LOG("Value of x is " << x);
DUMP(x);  // x = 123
```

**String Operations:**

```cpp
String a = "Hello";
a = a + " world";
DUMP(a.Find('e'));  // Find occurrences
```

### 2. Streams and Serialization

**File Streams:**

```cpp
FileOut fout("output.txt");
fout << "Some text";
fout.Close();
```

**Serialization:**

```cpp
struct MyObject { /* ... */ };
MyObject obj;
String s = StoreAsString(obj);
```

### 3. Containers and Data Structures

**Vector and Array:**

```cpp
Vector<int> v; 
v.Add(1);
Array<String> a; 
a.Add("text");
```

**Index for Fast Retrieval:**

```cpp
Index<String> ndx; 
ndx.Add("item");
```

### 4. Ranges and Algorithms

**Using Ranges:**

```cpp
Vector<int> data{1, 2, 3};
auto subrange = SubRange(data, 1, 2);  // Range from index 1 to 2
```

**Algorithms:**

```cpp
Sort(data);
int index = FindIndex(data, 2);
```

### 5. Value and Polymorphism

**Handling Different Types:**

```cpp
Value v = 123;
int x = v;
```

**Collections of `Value` Objects:**

```cpp
ValueArray va; 
va.Add(1); 
va.Add("text");

ValueMap vm; 
vm.Add("key", 123);
```

### 6. Multithreading

**Basic Thread Operations:**

```cpp
Thread t; 
t.Run([]{ /* ... */ }); 
t.Wait();
```

**Parallel Execution of Tasks:**

```cpp
CoWork co; 
co & []{ /* ... */ }; 
co.Finish();
```

### 7. Parallel Algorithms

**Distribute Tasks Over Multiple Cores:**

```cpp
Vector<int> data{ /* ... */ };
CoPartition(data, [](const auto& subrange){ /* ... */ });
```

* * *

**How to Use U++ Containers Effectively**

**1. Vector:**

```cpp
Vector<int> vec;
vec.Add(1);
vec.Add(2);
std::cout << "Vector: ";
for(int i = 0; i < vec.GetCount(); ++i)
    std::cout << vec[i] << " ";
std::cout << std::endl;
```

**2. Array:**

```cpp
Array<String> arr;
arr.Add("Hello");
arr.Add("World");
std::cout << "Array: ";
for(const auto& item : arr)
    std::cout << item << " ";
std::cout << std::endl;
```

**3. BiVector:**

```cpp
BiVector<double> bivec;
bivec.AddHead(1.1);
bivec.AddTail(2.2);
std::cout << "BiVector: ";
for(const auto& item : bivec)
    std::cout << item << " ";
std::cout << std::endl;
```

**4. Index:**

```cpp
Index<String> idx;
idx.Add("Apple");
idx.Add("Banana");
std::cout << "Index: ";
for(int i = 0; i < idx.GetCount(); ++i)
    std::cout << idx[i] << " ";
std::cout << std::endl;
```

**5. VectorMap:**

```cpp
VectorMap<String, int> vmap;
vmap.Add("One", 1);
vmap.Add("Two", 2);
std::cout << "VectorMap: ";
for(int i = 0; i < vmap.GetCount(); ++i)
    std::cout << vmap.GetKey(i) << "->" << vmap[i] << " ";
std::cout << std::endl;
```

**6. ArrayMap:**

```cpp
ArrayMap<int, String> amap;
amap.Add(1, "One");
amap.Add(2, "Two");
std::cout << "ArrayMap: ";
for(int i = 0; i < amap.GetCount(); ++i)
    std::cout << amap.GetKey(i) << "->" << amap[i] << " ";
std::cout << std::endl;
```

**7. One:**

```cpp
One<String> one;
one.Create() = "Hello One";
std::cout << "One: " << ~one << std::endl;
```

**8. Any:**

```cpp
Any any;
any.Create<String>() = "Hello Any";
std::cout << "Any: " << any.Get<String>() << std::endl;
```

**9. Buffer:**

```cpp
Buffer<int> buf(3);
buf[0] = 1; buf[1] = 2; buf[2] = 3;
std::cout << "Buffer: ";
for(int i = 0; i < 3; ++i)
    std::cout << buf[i] << " ";
std::cout << std::endl;
```

**New Interpolation Helpers**

```
Size a(100,50), b(300,150);
Size mid = Lerp(a, b, 0.5);   // ? (200,100)
Color blended = Lerp(White, Black, 0.25);   // 75 % white
```

**Simple Animation Example**

```
Animate(myButton)
    .Time(400)
    .CubicInOut()
    .Pos(Rect(200,200,120,30));
AnimateProperty(myButton, &Button::Ink, Blue, Cyan, 400);

```

* * *

## Containers overview (console)

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

* * *

## 2 · Simple GUI button

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

* * *

## 3 · String cookbook (selected tricks)

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

* * *

## 4 · ImageView (file browser + preview)

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
            if(isz.cx * view.cx < isz.cy * view.cy)
                view.cx = isz.cx * view.cy / isz.cy;
            else
                view.cy = isz.cy * view.cx / isz.cx;
            m = Rescale(m, view);
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

* * *

## 5 · Animated button — using 2025.1 `Animate`

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

* * *

## 6 · `Lerp` utility

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

* * *

#### ?? JSON Support – `Core/JSON.h` Only

- JSON parsing and serialization is handled via:
    
    ```cpp
    Value ParseJSON(CParser& p);
    Value ParseJSON(const char* s);
    String AsJSON(const Value&, bool pretty = false);
    ```
    
- Use `Json`, `JsonArray`, `JsonIO`, and `Jsonize(...)` templates for all JSON encoding/decoding.
    
- Do **not** use any third-party libraries like `nlohmann/json`.
    

* * *

#### ?? Websockets and External Libraries

- There is **no built-in WebSocket library** in U++.
    
- Do not assume support for:
    
    - CMake
        
    - Boost
        
    - REST APIs
        
    - Qt or GTK integration
        
    - Other third-party serialization libraries
        

* * *

#### Package Structure & Build System

- U++ uses `.upp` files to declare packages and dependencies.
    
- Each `.upp` lists source files and layout files.
    
- The U++ build system is **self-contained**. It does not use Makefiles or CMake.
    
- Packages may depend on others via `uses` declarations in `.upp`.
    

* * *

## U++ `.layout` Files: Usage and Integration

- **Location & Usage:**
    
    - Place your `.lay` (layout) file in your package directory.
    - In your package's main header (usually at the top), include the `.lay` file using the `#define LAYOUTFILE <yourfile.lay>` directive, before including `lay.h` or `lay0.h`.
    - Example:
        
        ```cpp
        #define LAYOUTFILE <MyApp/MyLayout.lay>
        #include <CtrlCore/lay.h>
        ```
        
- **Macro Expansion:**
    
    - The `LAYOUTFILE` macro tells the preprocessor which layout file to include.
    - Including `lay.h` or `lay0.h` expands the layout into C++ structures and helper functions.
    - For each layout in the `.lay` file, a struct named `With<LayoutName><BaseClass>` is generated, allowing you to create UI classes that inherit from this struct.
    - Example expansion for a layout named `MyWindow`:
        
        ```cpp
        template<class T> struct WithMyWindow<T> : public T, public MyWindow__layid { ... };
        ```
        
- **Assumptions & Naming:**
    
    - The macro assumes the `.lay` file is correctly referenced by the `LAYOUTFILE` macro.
    - The names given to layouts and controls inside the `.lay` file are used as class and variable names in the generated code.
    - The generated helper functions (e.g., `SetLayout_<LayoutName>`) use the layout's name.
- **.upp File Placement:**
    
    - The `.layout` include and macro expansion should ideally be at the top of your main package header (`.h`) file, not directly in the `.upp` file.
    - The `.upp` file is a package descriptor; it doesn’t process code but should list the `.lay` file under the `file` section so it’s included in the build.
- **Example in Practice:**
    
    ```cpp
    // MyApp.h
    #define LAYOUTFILE <MyApp/MyLayout.lay>
    #include <CtrlCore/lay.h>
    ```
    
    ```cpp
    // MyApp.upp
    file
        MyApp.h
        MyLayout.lay
        ...
    ```
    

* * *

**Summary:**

- Reference your `.lay` file using `LAYOUTFILE` before including `lay.h`.
- The macro and header generate C++ code, using names from the `.lay` file to create UI classes and functions.
- Ensure the `.lay` file is listed in your `.upp` package file for proper inclusion.

#### Layouts: Auto-Generated UI Declarations (`.lay` files)

- Layouts are designed via the **Layout Designer** and saved as `.lay` files.
    
- Each layout must be included using:
    
    ```cpp
    #define LAYOUTFILE <MyWindow.layout>
    #include <CtrlCore/lay.h>  // expands to #include LAYOUTFILE
    ```
    
- The `.lay` file must be:
    
    - Present **at the top** of the `.upp` package file
        
    - Saved in the IDE at least once to generate the corresponding `.layout` file
        
- U++ auto-generates a header file (e.g., `MyWindow.lay.h`) during the build phase.
    

**Failure to save or register the layout file results in:**

```
fatal error: 'MyWindow.layout' file not found
```

This error indicates:

- The layout wasn't saved or generated yet
    
- The `.lay` file wasn’t added to the `.upp` file, or wasn't added early enough
    

? To resolve:

- Ensure `.lay` file is added **first** in `.upp`
    
- Save all files after modifying the layout
    
- Do not rename `.lay` without updating all related declarations
    

### Basic folder structure

upp-baseapp/                 ? Root folder for your U++ application
+-- LICENSE                  ? Open source license (e.g. MIT, BSD)
+-- README.md                ? Project description and build instructions
+-- BaseApp.upp              ? U++ .upp package files (recognized by TheIDE)
¦
+-- include/                 ? Public headers for replication
¦   +-- myheader.h     ? Declares header files
¦
+-- src/                      ? Source files for your app and WebSocket backend
¦   +-- mycpp.cpp     ? declares various .cpp file logic
¦   +-- main.cpp         ? U++ GUI app entry point and layout usage
¦
+-- ui/                      ? UI layout files for U++ Layout Designer
¦   +-- MainLayout.lay       ? Defines GUI: Start button and log output
¦
+-- tests/                   ? Future unit tests (e.g. frame encode/decode, socket tests)
¦   +-- mytestapp.cpp    ? Console-based test harness for core logic
¦
+-- assets/                  ? Optional: Icons, fonts, or bundled data for GUI
¦   +-- icon.png             ? (Optional) App icon if using `TopWindow::Icon()`
¦
+-- design/                  ? Diagrams, notes, or sketches related to API structure or UI
¦   +-- ws_architecture.md   ? (Optional) Dev notes for frame structure, callbacks, etc.
¦
+-- examples/                ? Minimal sample apps or integration cases
    +-- myExamples.cpp       ? (Optional) Simple examples for local testing



### Where to look next

- **`examples/AnimateCtrlGeometry`** in the official examples tree for advanced geometry tweens and easing variants. :contentReference[oaicite:10]{index=10}
- **`examples/LinearInterpolation`** for practical `Lerp()` use cases with points, colours and rectangles. :contentReference[oaicite:11]{index=11}
- Full changelog: Ultimate++ website ? **Roadmap** (list of 2025.1 items). :contentReference[oaicite:12]{index=12}
- Nightly binaries: download page shows the latest build numbers. :contentReference[oaicite:13]{index=13}
- Documentation https://www.ultimatepp.org/www$uppweb$documentation$en-us.html
- Latest repository [https://github.com/ultimatepp/ultimatepp/tree/next2025_1 (to ensure any coding solutions are consistent in the latest U++ version)](https://github.com/ultimatepp/ultimatepp/tree/next2025_1)

&nbsp;

### U++ Widgets Overview

**Examples of Widgets:**

- **ArrayCtrl**: Complex tabular widget.
- **Button**: Basic button.
- **ColorButton**: Color selection with Image display.
- **MenuBar**: Menu bar for applications.
- **TreeCtrl**: Tree widget for hierarchical data.

**Project Initiation:** Begin your U++ application development , optimized for version 17045, by naming your project. This session aims to equip you with essential U++ development  and core operations.

**Let's get started. What's your project name?**