# U++ Framework Compressed API Reference v1.1

This document compresses key API elements from the provided U++ headers for AI-assisted coding. 
Focus: enums, structs/classes (key members/methods), functions (signatures, params), macros. 
Omits private/impl details, full bodies, and boilerplate. 
Use for accurate API recall (e.g., method names, params, return types). Structured by file.

## Cham.h (Chameleon Look & Feel System)

### Enums
- `LookOp`: LOOK_PAINT, LOOK_MARGINS, LOOK_PAINTEDGE, LOOK_NOCACHE=0x8000.

### Constants
- `CH_SCROLLBAR_IMAGE = -1000` (Image hotspot for scrollbar paint).
- `CH_EDITFIELD_IMAGE = -1001` (Image hotspot for edit field paint).

### Function Signatures
- `void ChLookFn(Value (*fn)(Draw& w, const Rect& r, const Value& look, int lookop, Color ink));` (Register look function).
- `Image AdjustColors(const Image& img);` (Adjust image colors).
- `void Override(Iml& target, Iml& source, bool colored = false);` (Override Iml with source).
- `void ColoredOverride(Iml& target, Iml& source);` (Colored Iml override).
- `void ChReset();` (Reset Chameleon).
- `void ChFinish();` (Finish Chameleon).
- `void ChPaint(Draw& w, const Rect& r, const Value& look, Color ink = Null);` / Overloads with x,y,cx,cy / NoCache variant.
- `void ChPaintEdge(Draw& w, const Rect& r, const Value& look, Color ink = Null);` / Overloads.
- `void ChPaintBody(Draw& w, const Rect& r, const Value& look, Color ink = Null);` / Overloads.
- `Rect ChMargins(const Value& look);` (Get margins from look).
- `void DeflateMargins(Rect& r, const Rect& margin);` / Overloads with ChLook, Size variants.
- `void InflateMargins(Rect& r, const Rect& m);` / Overloads with ChLook, Size variants.
- `void ChInvalidate();` (Invalidate Chameleon cache).
- `bool ChIsInvalidated();` (Check if invalidated).
- `bool IsLabelTextColorMismatch();` (Check text color mismatch).
- `bool IsDarkColorFace();` (Check if dark face color).
- `Value ChLookWith(const Value& look, const Image& img, Point offset = Point(0, 0));` / Overloads with Color, Color fn.
- `void ChLookWith(Value *look, const Image& image, const Color *color, int n = 4);` (Batch look with colors).

### Structs/Classes
- `template <class T> struct ChStyle { byte status=0; byte registered=0; T& Write() const; void Assign(const T& src); };` (Style manager).
- `CH_STYLE(klass, type, style)` macro: Defines styled struct with Init/InitIt.
- `CH_VAR0(chtype, type, name, init)` / `CH_VAR` macro: Var-style accessors with init.
- `struct ChColor : ChStyle<ChColor> { SColor value; };` / `CH_COLOR(name, init)` macro.
- `struct ChInt : ChStyle<ChInt> { int value; };` / `CH_INT(name, init)` macro.
- `struct ChValue : ChStyle<ChValue> { Value value; };` / `CH_VALUE(name, init)` macro.
- `struct ChImage : ChStyle<ChImage> { Image value; };` / `CH_IMAGE(name, init)` macro.

### Private/Internal
- `void ChRegisterStyle__(byte& state, byte& registered, void (*init)());` (Internal register).
- `Value ChBorder(const ColorF *colors, const Value& face = SColorFace());` (Border value).

## CtrlUtil.h (Control Utilities)

### Function Signatures
- `void Animate(Ctrl& c, const Rect& target, int type = -1);` / Overloads with x,y,cx,cy; Event<double> update variant; Vector<Ptr<Ctrl>> variant.
- `template <class T> void Animate(Vector<T>& data, const Vector<T>& targets, Event<> update, int duration = 100);` (Lerp animation).
- `bool CtrlLibDisplayError(const Value& ev);` (Display error).
- `bool EditText(String& s, const char *title, const char *label, int (*filter)(int) = NULL, int maxlen = 0);` / Overloads: NotNull, WString variants.
- `bool EditNumber(int& n, const char *title, const char *label, int min = INT_MIN, int max = INT_MAX, bool notnull = false);` / double overload.
- `bool EditDateDlg(Date& d, const char *title, const char *label, Date min = Date::Low(), Date max = Date::High(), bool notnull = false);` (Date editor).
- `void Show2(Ctrl& ctrl1, Ctrl& ctrl, bool show = true);` / `Hide2` (Show/hide pair).
- `#ifndef PLATFORM_WINCE: void UpdateFile(const char *filename); void SelfUpdate(); bool SelfUpdateSelf();` (Updater).
- `void WindowsList(); void WindowsMenu(Bar& bar);` (Window utils).

### Classes
- `class DelayCallback : public Pte<DelayCallback> { Event<> target; int delay; void Invoke(); void operator<<=(Event<> x); void SetDelay(int ms); Event<> Get(); /*ctor/dtor*/ };` (Delayed callback).

### PrinterJob (Non-WinCE, Non-VirtualGUI)
- `class PrinterJob { Draw& GetDraw(); bool Execute(); PrinterJob& Landscape(bool b=true); PrinterJob& MinMaxPage(int min, int max); /*etc*/; PrinterJob(const char *name=NULL); ~PrinterJob(); };` (Printing).

### TrayIcon (GUI_X11)
- `class TrayIcon : Ctrl { /* Hooks, Paint, events */; void AddToTray(); void DoMenu(Bar& bar); /* Events: WhenLeftDown, etc. */ };` (System tray).

### FileSel (Platform-specific)
- `class FileSelNative { /* Types, Asking, Multi, etc. */; FileSelNative& Type(const char *name, const char *ext); /*etc*/; };` / `typedef FileSelNative FileSelector;`.

### CtrlMapper
- `class CtrlMapper { bool toctrls=true; CtrlMapper& operator()(Ctrl& ctrl, T& val); CtrlMapper& ToCtrls(); CtrlMapper& ToValues(); };` (Ctrl-value mapper).

### CtrlRetriever
- `class CtrlRetriever { struct Item { virtual void Set(); virtual void Retrieve()=0; }; /* Inner structs */; void Put(Item*); template<class T> void Put(Ctrl& ctrl, T& val); void Set(); void Retrieve(); Event<> operator^=(Event<> cb); };` (Retrieve values from ctrls).

### IdCtrls
- `class IdCtrls { struct Item { Id id; Ctrl *ctrl; }; void Add(Id id, Ctrl& ctrl); bool Accept(); ValueMap Get() const; void Set(const ValueMap& m); /* Events */ };` (ID-based ctrl manager).

### FileSelButton & Variants
- `class FileSelButton : public FileSel { enum MODE {OPEN,SAVE,DIR}; void Attach(Ctrl& parent); Event<> WhenSelected; /*etc*/ };` / `struct OpenFileButton : FileSelButton`; etc.

### Misc Utils
- `Image MakeZoomIcon(double scale); void Set(ArrayCtrl& array, int ii, IdCtrls& m); /*etc*/; void UpdateSetDir(const char *path); /*etc*/; void MemoryProfileInfo(); struct sPaintRedirectCtrl : Ctrl { /* Paint redirect */ };`.

## Color.h (Color System)

### Struct
- `struct RGBA { byte a,r,g,b; /*platform variant*/ };` / Operators: %, ==, !=, hash; `inline Stream& operator%(Stream& s, RGBA& c);`.

### Class Color
- `class Color : public ValueType<Color, COLOR_V, Moveable<Color>> { dword color; /* GetR/G/B, SetNull, Serialize, etc. */; Color(); Color(int r,g,b); operator RGBA(); /*etc*/; static Color FromRaw(dword co); };` (Color handling).

### SColor / AColor
- `struct SColor : Color { static void Refresh(); static void Write(Color c, Color val); SColor(Color (*fn)()=NULL); };` (Static colors).
- `struct AColor : Color { AColor(Color c); AColor(int r,g,b); };` (Auto dark-mode colors).

### Functions
- `RGBA operator*(int alpha, Color c); Color StraightColor(RGBA rgba);` (Ops).
- `typedef Color (*ColorF)(); hash_t GetHashValue(Color c); Color Nvl(Color a, Color b);` (Utils).
- Inline colors: `Black()`, `Gray()`, `LtGray()`, `White()`, `Red()`, etc. / Dark/Lt variants.
- `void RGBtoHSV(double r,g,b, double& h,s,v); void HSVtoRGB(...); Color HsvColorf(double h,s,v);` (HSV).
- `void CMYKtoRGB(...); void RGBtoCMYK(...); Color CmykColorf(double c,m,y,k=0);` (CMYK).
- `double RelativeLuminance(Color color); double ContrastRatio(Color c1, Color c2); Color Blend(Color c1, Color c2, int alpha=128); double Difference(Color c1, Color c2); Color Lerp(Color a, Color b, double t);` (Color math).
- `String ColorToHtml(Color color); Color ColorFromText(const char *s); int Grayscale(const Color& c); bool IsDark/Color c); bool IsLight(Color c); Color DarkTheme(Color c);` (Conversions, themes).
- Operators for Value/ColorF comparisons.

## Callback.h (Callback System - Backward Compat)

### Classes
- `template <class... ArgTypes> class CallbackN : Moveable<...> { typedef Function<void(ArgTypes...)> Fn; Fn fn; CallbackN(); /* Assign, Proxy, <<, operator() */; operator Fn(); };` (N-ary callback).
- `typedef CallbackN<> Callback; template<class P1> using Callback1 = CallbackN<P1>; /*up to 5*/;`.
- `template <class...> class GateN : Moveable<...> { /* Similar, returns bool */; };` / `using Gate0 = GateN<>; /*etc*/;`.
- `#define Res void; #define Cb_ CallbackN; #include "CallbackR.i"` (Generated variants).
- Macros: `THISBACK(x)`, `THISBACK1(x,arg)`, etc.; `PTEBACK`, `STDBACK`.

### CallbackArgTarget
- `template <class T> class CallbackNArgTarget { T result; operator const T&(); Callback operator[](const T& value); /*etc*/; }; using CallbackArgTarget = CallbackNArgTarget<T>;` (Arg target).

## DropGrid.h (Drop-down Grid Ctrl)

### Enums
- In `DropGrid`: BTN_SELECT, BTN_LEFT/RIGHT/UP/DOWN, BTN_PLUS, BTN_CLEAN.

### Inner Class PopUpGrid
- `class PopUpGrid : public GridCtrl { void CloseNoData/Data(); void PopUp(Ctrl *owner, const Rect &r); virtual void Deactivate(); Event<> WhenPopDown/Close/etc.; PopUpGrid(); };` (Popup grid).

### Class DropGrid : Convert, GridDisplay, Ctrl
- Members: int key_col/find_col/value_col; Vector<int> value_cols; PopUpGrid list; MultiButtonFrame drop; GridButton clear; /*flags*/.
- Methods: `void Close/NoData/Data(); void Drop(); DropGrid& Width/Height/SetKeyColumn/etc. (many setters: DisplayAll, Header, NotNull, etc.); GridCtrl::ItemRect& AddColumn(const char *name, int width=-1, bool idx=false); /*overloads*/; MultiButton::SubButton& AddButton(int type, Event<> &cb); /*variants*/; int AddColumns(int cnt); void GoTop(); int Set/GetIndex() const; int GetCount() const; void Reset/Clear/Ready/ClearValue/DoClearValue(); Value GetData() const; void SetData(const Value& v); /* GetValue/FindValue/etc. */; virtual bool Key(dword k, int); virtual void Paint/Draw& draw); /*events*/; virtual Value Format(const Value& q) const; Event<> WhenLeftDown/Drop; /* AddRow/Add/Separator */; /*operators*/.
- Utils: `bool IsSelected/Empty/Change/Init(); void ClearChange(); int Find(const Value& v, int col=0, int opt=0); /*etc*/; GridCtrl& GetList();`.

## FileTabs.h (File Tab Bar)

### Class FileTabs : public TabBar
- Flags: bool stackedicons/greyedicons; Color filecolor/extcolor.
- Virtuals: `String GetFileGroup(const String &file); String GetStackId(const Tab &a); hash_t GetStackSortOrder(const Tab &a); void ComposeTab(Tab& tab, ...); void ComposeStackedTab(...); Size GetStackedSize(const Tab &t);`.
- Methods: `void AddFile(const WString &file, bool make_active=true); /*overloads with Image, Insert, AddFiles*/; void RenameFile(const WString &from, const WString &to, Image icon=Null); FileTabs& FileColor/ExtColor(Color c); FileTabs& FileIcons(bool normal=true, bool stacked=true, bool stacked_greyedout=true); Vector<String> GetFiles() const; FileTabs& operator<<(const FileTabs &src);`.
- `virtual void Serialize(Stream& s);`.

## BufferPainter.h (Buffer-based Painter)

### Utils
- `force_inline RGBA Mul8(const RGBA& s, int mul);` (Alpha mul).

### Interfaces
- `struct SpanSource { virtual void Get(RGBA *span, int x, int y, unsigned len) const =0; };`.
- `struct PainterTarget : LinearPathConsumer { virtual void Fill(double width, SpanSource *ss, const RGBA& color); };`.

### Inner Structs (ClippingLine, etc.)
- `struct ClippingLine : NoCopy { void Clear/Set/SetFull(); bool IsEmpty/Full(); operator const byte*(); /*ctor/dtor*/ };`.

### Class BufferPainter : public Painter
- Many virtual ops: ClearOp, MoveOp/LineOp/etc., FillOp/StokeOp variants (colors, images, gradients), ClipOp, etc.
- Members: ImageBuffer& ip; Buffer<PathInfo> paths; /*many internals for raster/path/gradients*/.
- Methods: `ImageBuffer& GetBuffer(); BufferPainter& Co(bool b=true); /* PreClip, ImageCache */; void Create(ImageBuffer& ib, int mode=MODE_ANTIALIASED); void Finish(); /*ctors*/; ~BufferPainter();`.
- Internals: Path handling, RenderPath/Image/Radial, Gradient makers, etc.

## Draw.h (Drawing System)

### Includes/Consts
- `#define SYSTEMDRAW 1; const int FONT_V=40;`.

### Interfaces
- `struct FontGlyphConsumer { virtual void Move/Line/Quadratic/Cubic/Close()=0; };`.

### Class Font : ValueType<Font, FONT_V, Moveable<Font>>
- Union: int64 data; struct { word face/flags; int16 height/width; }.
- Enums: FONT_BOLD=0x8000, etc.; FIXEDPITCH=0x0001, etc.
- Statics: `Font AStdFont; Size StdFontSize/A; bool std_font_override; static void SetStdFont0/InitFonts/etc.; Vector<FaceInfo>& FaceList();`.
- Methods: `int GetFace/Height/Width(); bool IsBold/Italic/etc.; String GetFaceName/Std(); dword GetFaceInfo(); int64 AsInt64(); void RealizeStd(); Font& Face/Height/Width/Bold/etc. (chainable);`.
- Utils: `inline bool PreferColorEmoji(int c);` (Emoji check).

### Drawing Classes (High-level)
- `class Drawing; class Draw; class Painting; class SystemDraw; class ImageDraw; /* Many Draw methods: DrawRect, DrawLine, etc. */`.
- `class ImageAnyDraw { /* Image drawing */; ~ImageAnyDraw(); };`.

### Utils
- `void AddNotEmpty/Subtract/Union/Intersect/AddRefreshRect(...);` (Rect ops).
- `void DrawDragFrame/DrawRect/DrawTiles/DrawFatFrame/DrawFrame/DrawBorder/DrawRectMinusRect/DrawHighlightImage(...);` (Drawing primitives).
- Borders: `const ColorF *BlackBorder/WhiteBorder/etc.();`.
- `Color GradientColor(Color fc, Color tc, int i, int n); void DrawTextEllipsis/DrawTLText(...);` (Text/grad).
- Enums: BUTTON_NORMAL/OK/etc.
- `void DrawXPButton(Draw& w, Rect r, int type);`.
- PDF: `typedef String (*DrawingToPdfFnType)(...); /* Setters/Getters */; typedef bool (*IsJPGFnType)(...);`.

### Includes
- Image.h, FontInt.h, Display.h, Cham.h, etc.

## Gtypes.h (Geometry Types)

### Templates
- `template <class T> struct Size_ : Moveable<...> { T cx,cy; void Clear/SetNull(); bool IsEmpty/NullInstance(); /* Ops: +=/*= etc., min/max/Nvl/ScalarProduct/etc. */; hash_t GetHashValue(); String ToString(); /*ctors/conversions*/; operator Value(); };`.
- Similar for `Point_`, `Rect_`.

### Typedefs
- `typedef Size_<int> Size; typedef Point_<int> Point; typedef Rect_<int> Rect; /* f/16 variants */`.

### Operators (Overloads)
- Arithmetic for Size/Point/Rect (e.g., `Size operator+(Size a, Size b);`).
- `inline Sizef operator*(double a, Size sz); /*etc*/; Rect RectC/Sort(...);`.

### Utils
- `Stream& Pack16(Stream& s, Point& p); /*etc*/;`.
- `Size iscale/idivfloor/etc. (scaling/div);`.
- Enum `Alignment {NULL,LEFT/TOP,RIGHT/BOTTOM,CENTER,JUSTIFY};`.
- `Size GetRatioSize/FitSize(...); Sizef GetFitSize(...);`.
- Vector math: `double Squared/Length/Direction/Distance/etc. (Pointf); Pointf Mid/Orthogonal/Normalize/Polar/Lerp(...);`.
- Deprecated: `inline double Bearing(const Pointf& p) { return Direction(p); };`.

## GridCtrl.h (Grid Control)

### Includes/Utils
- GridUtils.h, GridDisplay.h; `#define FOREACH_ROW/etc.; namespace GF { SKIP_CURRENT_ROW=BIT(0), etc. };`.

### Inner Classes
- `class GridFind : public EditString { MultiButtonFrame button; virtual bool Key(...); void Push(); Event<> WhenEnter; Callback1<Bar&> WhenBar; };`.
- `class GridPopUpHeader : public Ctrl { /* Paint, PopUp, Close */; };`.
- `class GridButton : public Ctrl { /* Paint, events, SetButton */; };`.
- `class GridResizePanel : public FrameBottom<Ctrl> { /* Paint, events, SetMinSize */; Event<> WhenClose; };`.
- `class GridPopUp : public Ctrl { /* Many events: Paint, LeftDown/etc., PopUp, Close */; };`.
- `class GridOperation { enum {INSERT,UPDATE,REMOVE,NONE}; void SetOperation(int op); /*etc*/; };`.

### CtrlsHolder
- `class CtrlsHolder : public Ctrl { Ctrl &parent; Point offset; /* Event forwards */; };`.

### Class GridCtrl (Main)
- Massive: Many members (columns, rows, edits, etc.); Virtuals: Paint, LeftDown, etc.
- Key Methods: `void SetColumns(int n); void AddColumn(const char *s, int width=50); /*many Add variants*/; void SetLineCy(int cy); void GoBegin/End/Up/Down/etc.; int GetCurrentRowId() const; bool IsCurrentRow(int ri) const; /*Sorting: GSort, Multisort, etc.*/; void SetClipboard/Paste/etc.; /*Events: WhenMenuBar, WhenLeftDouble, WhenInsertRow, etc.*/; template<> void Xmlize/Jsonize(GridCtrl& g);`.

**Batch Summary**: 10 files compressed. Key themes: UI styling (Cham), utils (CtrlUtil), colors (Color), callbacks, grids (Drop/Grid), drawing (BufferPainter/Draw), geometry (Gtypes), tabs (FileTabs). Paste next 10 for continuation.


## Image.h (Image Handling)

### Enums
- `ImageKind`: IMAGE_UNKNOWN, IMAGE_EMPTY (dep.), IMAGE_ALPHA, IMAGE_MASK (dep.), IMAGE_OPAQUE.

### Inline/Utils
- `void Fill(RGBA *t, RGBA c, size_t n); void FillDown(RGBA *t, int linecy, RGBA c, int cy); void Copy(RGBA *t, const RGBA *s, int n);`.
- `force_inline RGBA Premultiply/Unmultiply(const RGBA& s);` (Alpha premul).
- `int Premultiply/Unmultiply(RGBA *t, const RGBA *s, size_t len);`.
- `void TransformComponents(RGBA *t, const RGBA *s, int len, const byte r/g/b/a[]); void MultiplyComponents(RGBA *t, const RGBA *s, int len, int num, int den=256);`.
- `void AlphaBlend(RGBA *t, const RGBA *s, int len);` / Overloads with Color.
- `void AlphaBlendOpaque/Straight(RGBA *t, const RGBA *s, int len);` / Overloads.
- `int GetChMaskPos32(dword mask); void AlphaBlendOverBgST(RGBA *b, RGBA bg, int len);`.
- `const byte *UnpackRLE(RGBA *t, const byte *src, int len); String PackRLE(const RGBA *s, int len);`.
- `inline int Grayscale(int r,g,b); inline int Grayscale(const RGBA& c);`.

### Class ImageBuffer : NoCopy
- Members: atomic<int> kind; Size size; Buffer<RGBA> pixels; Point hotspot/spot2; Size dots; bool bpp32, alpha, premul.
- Methods: `void Create(Size sz); void SetKind(int k); bool IsSameSize(const ImageBuffer& b) const; RGBA *Begin(); const RGBA *Begin() const; RGBA *Line(int line); const RGBA *Line(int line) const; int Length() const; void SetHotSpot(Point p); void SetHotSpots(Point p1, Point p2); void SetDots(Size d); void SetMatrix(const RGBA *m, int n); Image ToImage(); /* Alloc, Free, etc. */; ImageBuffer(); ~ImageBuffer();`.

### Class Image : ValueType<Image, IMAGE_V, MoveableAndDeepCopyOption<Image>>
- Members: One<ImageBuffer> data; /* kind, etc. */.
- Methods: `int GetWidth/Height/Kind() const; bool IsEmpty() const; Size GetSize() const; Point GetHotSpot() const; RGBA GetPixel(int x, int y) const; void SetHotSpot(Point p); Image& Co(); Image& SetKind(int k); Image& SetHotSpots(Point p1, Point p2); Image& SetDots(Size d); void Paint(Draw& w, Point p, Color c=Null) const; /* Overloads */; Image& operator=(const Image& src); /* DeepCopy */; Image(); Image(Size sz); Image(Size sz, const RGBA *data); /* Load/Save */; ~Image();`.

### IML Flags
- `enum { IML_IMAGE_FLAG_EMPTY=1, FIXED_SIZE=4, UHD=8, DARK=16, S3=32 };`.

### Class Iml
- Inner: `struct IImage : Moveable { atomic<bool> loaded; Image image; }; struct Data : Moveable { const char *data; int len/count; };`.
- Members: Vector<Data> data[4]; VectorMap<String, IImage> map; const char **name; dword global_flags=0; bool premultiply; Index<String> ex_name[3].
- Methods: `void Reset/Skin(); int GetCount() const; String GetId(int i) const; Image Get(int i); int Find(const String& id) const; void Set(int i, const Image& img); ImageIml GetRaw(int mode, int i); ImageIml GetRaw(int mode, const String& id); /* AddData/Id, Premultiplied, GlobalFlag */; static void ResetAll/SkinAll(); Iml(const char **name, int n); /* Legacy */;`.

### Utils
- `Image MakeImlImage(const String& id, Function<ImageIml (int, const String&)> GetRaw, dword global_flags);`.
- `void Register(const char *imageclass, Iml& iml); int GetImlCount(); String GetImlName(int i); Iml& GetIml(int i); int FindIml(const char *name); Image GetImlImage(const char *name); void SetImlImage(const char *name, const Image& m);`.
- `String StoreImageAsString(const Image& img); Image LoadImageFromString(const String& s); Size GetImageStringSize/Dots(const String& src);`.

### Includes
- Raster.h, ImageOp.h, SIMD.h.

## Painter.h (Painter Abstraction)

### Struct Xform2D
- Members: Pointf x,y,t.
- Methods: `Pointf Transform(double px, double py) const; Pointf Transform(const Pointf& f) const; Pointf GetScaleXY() const; double GetScale() const; bool IsRegular() const; static Xform2D Identity/Translation/Scale/Rotation/Sheer/Map(...); Xform2D();`.
- Utils: `Xform2D operator*(const Xform2D& a, const Xform2D& b); Xform2D Inverse(const Xform2D& m);`.

### Enums
- `PainterOptions`: LINECAP_BUTT/SQUARE/ROUND, LINEJOIN_MITER/ROUND/BEVEL, FILL_EXACT/HPAD/HREPEAT/HREFLECT/VPAD/VREPEAT/VREFLECT/PAD/REPEAT/REFLECT/FAST, GRADIENT_PAD/REPEAT/REFLECT.

### Class Painter : public Draw
- Virtuals: `dword GetInfo() const; void OffsetOp(Point p); bool ClipOp/ClipoffOp/ExcludeClipOp/IntersectClipOp(const Rect& r); bool IsPaintingOp(const Rect& r) const; void DrawRectOp(int x,y,cx,cy, Color color); void DrawImageOp(int x,y,cx,cy, const Image& img, const Rect& src, Color color); void DrawLineOp(int x1,y1,x2,y2, int width, Color color); /* Many Draw variants */; void DrawPolyPolylineOp(const Point *vertices, int vertex_count, const int *counts, int count_count, int width, Color color, Color doxor); void DrawArcOp(const Rect& rc, Point start, Point end, int width, Color color); void DrawEllipseOp(const Rect& r, Color color, int pen, Color outline); void DrawTextOp(int x, int y, int angle, const wchar *text, Font fnt, Color ink, int n, const int *dx); /* PolyPolyPolygon, etc. */; void DrawPolyPolyPolygonOp(const Point *vertices, int vertex_count, const int *subpolygon_counts, int scc, const int *disjunct_polygon_counts, int dpcc, Color color, int width, Color outline, uint64 pattern, Color doxor); void DrawDotsOp(const Pointf *pts, int count, const int *levels, const RGBA *colors); void DrawPolyPolyLineOp(const Pointf *vertices, int vertex_count, const int *subpolygon_counts, int subpolygon_count_count, const int *disjunct_polygon_counts, int disjunct_polygon_count_count, double width, const RGBA *colors, double *dashes, int dash_count, double dash_phase); void DrawPathOp(const Vector<byte>& path, const RGBA& color); /* Many more: DrawPathCapper, FillPathOp, etc. */; void DrawGradientOp(const Pointf& f, const RGBA& color1, const Pointf& c, double r, const RGBA& color2, int style); void DrawTilingOp(const Image& image, const Xform2D& transsrc, dword flags); void StrokeOp(double width, const Pointf& p1, const RGBA& color1, const Pointf& p2, const RGBA& color2, int style); /* Many Stroke variants */; void ClipOp(); void CharacterOp(const Pointf& p, int ch, Font fnt); void TextOp(const Pointf& p, const wchar *text, Font fnt, int n=-1, const double *dx=NULL); void ColorStopOp(double pos, const RGBA& color); void ClearStopsOp(); void OpacityOp(double o); void LineCapOp(int linecap); void LineJoinOp(int linejoin); void MiterLimitOp(double l); void EvenOddOp(bool evenodd); void DashOp(const Vector<double>& dash, double start); void InvertOp(bool invert); void ImageFilterOp(int filter); void TransformOp(const Xform2D& m); void BeginOp/EndOp(); void BeginMaskOp(); void BeginOnPathOp(double q, bool abs);`.

### Utils
- `bool RenderSVG(Painter& p, const char *svg, Event<String, String&> resloader, Color ink=SBlack());` / Overload without resloader.
- `void GetSVGDimensions(const char *svg, Sizef& sz, Rectf& viewbox); Rectf GetSVGBoundingBox(const char *svg); Rectf GetSVGPathBoundingBox(const char *path);`.
- `Image RenderSVGImage(Size sz, const char *svg, Event<String, String&> resloader, Color ink=SBlack());` / Overload.
- `bool IsSVG(const char *svg);`.

## Ptr.h (Smart Pointers)

### Class PteBase
- Inner: `struct Prec { PteBase *ptr; Atomic n; };`.
- Members: volatile Prec *prec.
- Methods: `Prec *PtrAdd(); static void PtrRelease(Prec *prec); PteBase(); ~PteBase();`.

### Class PtrBase
- Members: PteBase::Prec *prec.
- Methods: `void Set/Release/Assign(PteBase *p); ~PtrBase();`.

### Template Class Pte<T> : public PteBase { friend class Ptr<T>; };

### Template Class Ptr<T> : public PtrBase, Moveable<Ptr<T>>
- Methods: `T *Get() const; T *operator->/~() const; operator T*() const; Ptr& operator=(T *ptr); Ptr& operator=(const Ptr& ptr); Ptr(); Ptr(T *ptr); Ptr(const Ptr& ptr); String ToString() const;`.
- Operators: `==/!=` with T*/Ptr.

## ImageOp.h (Image Operations)

### Functions
- `Image CreateImage(Size sz, const RGBA& rgba); Image CreateImage(Size sz, Color color); Image SetColorKeepAlpha(const Image& img, Color c);`.
- `void SetHotSpots(Image& m, Point hotspot, Point hotspot2=Point(0,0)); Image WithHotSpots(const Image& m, ...); /* Overloads */; void ScanOpaque(Image& m);`.
- `void DstSrcOp(ImageBuffer& dest, Point p, const Image& src, const Rect& srect, void (*op)(RGBA *t, const RGBA *s, int n), bool co=false); void Copy/ImageBuffer& dest, ...); void Over(ImageBuffer& dest, ...); void Over(Image& dest, ...); void Fill(ImageBuffer& dest, const Rect& rect, RGBA color); void Copy(Image& dest, ...); Image GetOver(const Image& dest, const Image& src); void Fill(Image& dest, ...); Image Copy(const Image& src, const Rect& srect);`.
- `void OverStraightOpaque(ImageBuffer& dest, ...); void OverStraightOpaque(Image& dest, ...);`.
- `void Crop(RasterEncoder& tgt, Raster& img, const Rect& rc); Image Crop(const Image& img, ...); /* Overloads */; Image AddMargins(const Image& img, int left,top,right,bottom, RGBA color=RGBAZero());`.
- `Rect FindBounds(const Image& m, RGBA bg=RGBAZero()); Image AutoCrop(const Image& m, RGBA bg=RGBAZero()); void AutoCrop(Image *m, int count, RGBA bg=RGBAZero()); void ClampHotSpots(Image& m);`.
- `Image ColorMask(const Image& src, Color transparent); void CanvasSize(RasterEncoder& tgt, Raster& img, int cx,cy); Image CanvasSize(const Image& img, int cx,cy); Image AssignAlpha(const Image& img, const Image& alpha);`.
- `Image MirrorHorz/Vert(const Image& src); Image Rotate(int angle, const Image& src); Image Mirror(const Image& src, bool horz, bool vert); Image Flip(const Image& src, bool horz, bool vert); Image RotateFlip(const Image& src, int angle, bool horz, bool vert);`.
- `Image Grayscale(const Image& src); Image Sepia(const Image& src); Image Invert(const Image& src); Image Posterize(const Image& src, int level); Image Solarize(const Image& src, int threshold); Image Sharpen(const Image& src, double radius, double amount); Image Blur(const Image& src, double radius); Image Emboss(const Image& src, double strength); Image EdgeDetect(const Image& src); Image Dilate(const Image& src); Image Erode(const Image& src); Image Noise(const Image& src, double amount); Image Tint(const Image& src, Color color); Image AdjustContrast(const Image& src, double contrast); Image AdjustBrightness(const Image& src, double brightness); Image AdjustGamma(const Image& src, double gamma); Image AdjustHue(const Image& src, double hue); Image AdjustSaturation(const Image& src, double saturation); Image AdjustLightness(const Image& src, double lightness); Image AdjustColorBalance(const Image& src, double red, double green, double blue); Image AdjustCurves(const Image& src, const Vector<Pointf>& curve);`.
- `Image Rescale(const Image& m, Size sz, int filter=Null); Image Rescale(const Image& m, int cx, int cy, int filter=Null); Image RescalePaintOnly(const Image& m, Size sz, const Rect& src, int filter=Null); /* Overloads */; Image CachedRescale(const Image& m, Size sz, int filter=Null); /* Overloads */; Image CachedSetColorKeepAlpha(const Image& img, Color color); /* PaintOnly */;`.
- `Image Magnify(const Image& img, const Rect& src, int nx,ny, bool co); Image Magnify(const Image& img, int nx,ny, bool co=false); Image Minify(const Image& img, int nx,ny, bool co=false); Image MinifyCached(const Image& img, int nx,ny, bool co=false);`.
- `Image DownSample3x/2x(const Image& src, bool co=false); Image Upscale2x/Downscale2x(const Image& src);`.
- `void SetUHDMode(bool b=true); bool IsUHDMode(); void SyncUHDMode(); Image DPI(const Image& img, int expected); inline int/DPI(double/Size a); inline Image DPI(const Image& a, const Image& b);`.

### Struct RGBAV
- Members: dword r,g,b,a.
- Methods: `void Set(dword v); void Clear(); void Put(dword weight, const RGBA& src); void Put(const RGBA& src); RGBA Get(int div) const;`.

### Obsolete
- `Image RescaleBicubic(const Image& src, Size sz, const Rect& src_rc, Gate<int,int> progress=Null); /* Overloads */;`.

## StatusBar.h (Status Bar UI)

### Class InfoCtrl : public FrameLR<Ctrl>
- Inner: `struct Tab { PaintRect info; int width; };`.
- Members: Array<Tab> tab; PaintRect temp; bool right; String defaulttext; TimeCallback temptime.
- Virtuals: `void Paint(Draw& w); void FrameLayout(Rect& r);`.
- Methods: `void Set(int tab, const PaintRect& info, int width); void Set(int tab, const Value& info, int width); void Set(const PaintRect/Value& info); void Temporary(const PaintRect/Value& info, int timeout=2000); void EndTemporary(); int GetTabCount() const; int GetTabOffset(int t) const; int GetRealTabWidth(int tabi, int width) const; void operator=(const String& s); InfoCtrl& SetDefault(const String& d); InfoCtrl& Left/Right(int w); InfoCtrl& LeftZ/RightZ(int w); InfoCtrl();`.

### Class StatusBar : public InfoCtrl
- Inner: `struct Style : public ChStyle<Style> { Value look; }; struct TopFrame : public CtrlFrame { /* FrameLayout/Paint/AddSize */; const Style *style; };`.
- Members: int cy; SizeGrip grip; TopFrame frame.
- Virtuals: `void FrameLayout/FrameAddSize(Paint(Draw& w);`.
- Methods: `void operator=(const String& s); operator Event<const String&>(); Event<const String&> operator~(); StatusBar& Height(int _cy); StatusBar& NoSizeGrip(); static const Style& StyleDefault(); InfoCtrl& SetStyle(const Style& s); StatusBar(); ~StatusBar();`.

### ProgressDisplay
- `Display& ProgressDisplay();`.

### Class ProgressInfo
- Members: InfoCtrl *info; String text; int tw/tabi/cx/total/pos/granularity; dword set_time.
- Methods: `ProgressInfo& Text(const String& s); /* Overloads: TextWidth, Width, Placement, Info, Total */; ProgressInfo& Set(int _pos, int _total); void Set(int _pos); int Get/GetTotal() const; void operator=(int p); void operator++(); operator int(); ProgressInfo(); ProgressInfo(InfoCtrl& f); ~ProgressInfo();`.

## TabBar.h (Tab Bar Control)

### Includes/Macros
- IMAGECLASS TabBarImg; IMAGEFILE <TabBar/TabBar.iml>; #include <Draw/iml_header.h>; #define TABBAR_DEBUG.

### Class AlignedFrame : FrameCtrl<Ctrl>
- Members: int layout/framesize/border.
- Enums: LEFT=0, TOP=1, RIGHT=2, BOTTOM=3.
- Virtuals: `void FrameAddSize(Size& sz); void FramePaint(Draw& w, const Rect& r); void FrameLayout(Rect& r);`.
- Methods: `bool IsVert/Horz/TL/BR() const; AlignedFrame& SetAlign(int align); /* SetLeft/Top/Right/Bottom */; AlignedFrame& SetFrameSize(int sz, bool refresh=true); int GetAlign/FrameSize/Border() const; protected: void Fix(Size&/Point& sz/p); Size Fixed(const Size& sz); Point Fixed(const Point& p); bool HasBorder(); AlignedFrame& SetBorder(int _border); virtual void FrameSet(); AlignedFrame();`.

### Class TabScrollBar : public AlignedFrame
- Members: int total; double pos/ps; int new_pos/old_pos; double start_pos/size/cs/ics; bool ready; Size sz.
- Virtuals: `void Paint(Draw& w); void LeftDown/Up(Point p, dword keyflags); void MouseMove(Point p, dword keyflags); void MouseWheel(Point p, int zdelta, dword keyflags); void Layout();`.
- Methods: `void UpdatePos(bool update=true); void RefreshScroll(); int GetPos() const; void SetPos(int p, bool dontscale=false); void AddPos(int delta); TabScrollBar();`.

### Struct Tab
- Members: String key; WString text; Image icon; Color color; String group; int keypos; dword flags; Vector<CtrlFrame *> frames; int sortorder; bool closable; Event<> whenclose; Callback WhenClose() const;.

### Inner Classes (TabBarCtrl, etc.)
- (Omitted details; focus on main TabBar).

Class TabBar : public Ctrl
- Members: Array<Tab> tabs; int active/highlight; Vector<Group> groups; int group; TabScrollBar sc; const Style *style; int dragdrop; Point dragpoint; int dragtab; Image dragimage; bool closable; bool paintcode; bool draghilite; bool ignoreclose; int dragindex; Point dragoffset; int dragmode; bool multiline; bool closecursor; bool closecursorhl; bool closetab; bool allowreorder; bool allowduplicate; bool ignoretaborder; bool ignoretabmove; bool ignoregroup; bool multiplegroups; bool dragmulti; bool dragout; bool draganddrop; bool closesize; bool closepos; bool closecenter; bool closerect; bool groupcolors; bool groupimages; bool closableall; bool closablegroup; bool closablefirst; bool closablelast; bool closablesingle; bool closablereorder; bool closablereorderfirst; bool closablereorderlast; bool closablereordersingle; bool closablereorderall; bool closablereordergroup; bool closablereordergroupfirst; bool closablereordergrouplast; bool closablereordergroupsingle; bool closablereordergroupall; bool closablereordergroupall; bool closablereordergroupallfirst; bool 


## MultiButton.h (Multi-Button Control)

### Class MultiButton : public Ctrl
- Virtuals: `void Paint(Draw& w); void MouseMove/LeftDown/LeftUp/Up(Point p, dword flags); void MouseLeave(); void CancelMode(); void GotFocus/LostFocus(); void SetData(const Value& data); Value GetData() const; Size GetMinSize() const; int OverPaint() const;`.
- Inner: `struct Style : public ChStyle<Style> { Value edge[4]/coloredge/look[4]/left[4]/lmiddle[4]/right[4]/rmiddle[4]/simple[4]/trivial[4]/sep1/sep2; int border/trivialborder/sepm/stdwidth/loff/roff/overpaint; bool activeedge/trivialsep/usetrivial/clipedge; Rect margin; Color monocolor[4]/fmonocolor[4]/error; Point pressoffset; Value paper; };`.
- Inner: `class SubButton { String label/tip; MultiButton *owner; Image img; int cx; bool main/left/monoimg/enabled/visible; void Refresh(); public: SubButton& Label(const char *s); SubButton& Tip(const char *s); SubButton& Image(const Image& i); SubButton& MonoImage(const Image& i); SubButton& Size(int cx); SubButton& Main(bool b=true); SubButton& Left(bool b=true); SubButton& Enable(bool b=true); SubButton& Visible(bool b=true); bool IsEnabled() const; bool IsVisible() const; String GetLabel() const; Image GetImage() const; };`.
- Members: Array<SubButton> button; const Style *style; int push; Point pushpos; Rect pushrect; Value value; Convert *convert; Display *display; Value error; bool push; Color paper; bool droppush.
- Methods: `static const Style& StyleDefault/Frame(); bool IsTrivial() const; void Reset(); void PseudoPush(int bi); void PseudoPush(); SubButton& AddButton(); SubButton& InsertButton(int i); void RemoveButton(int i); int GetButtonCount() const; const SubButton& GetButton(int i) const; SubButton& GetButton(int i); SubButton& MainButton(); Rect GetPushScreenRect() const; const Display& GetDisplay() const; const Convert& GetConvert() const; const Value& Get() const; void Error(const Value& v); void SetPaper(Color c); MultiButton& SetDisplay(const Display& d); MultiButton& NoDisplay(); MultiButton& SetConvert(const Convert& c); MultiButton& SetValueCy(int cy); MultiButton& Set(const Value& v, bool update=true); MultiButton& Tip(const char *s); MultiButton& NoBackground(bool b=true); MultiButton& SetStyle(const Style& s); void SetupDropPush(); MultiButton();`.

### Class MultiButtonFrame : public MultiButton, public CtrlFrame
- Virtuals: `void FrameLayout(Rect& r); void FrameAddSize(Size& sz); void FrameAdd(Ctrl& parent); void FrameRemove(); bool Frame();`.
- Methods: `void AddTo(Ctrl& w);`.

## Core.h (Core Framework Includes & Defines)

### Defines/Flags
- `UPP_VERSION 0x20250200; _MULTITHREADED; MULTITHREADED;`.
- `#ifdef flagDLL: flagUSEMALLOC; STD_NEWDELETE; _USRDLL;`.
- `#ifdef flagHEAPDBG: HEAPDBG;`.
- `#if defined(flagDEBUG): _DEBUG; TESTLEAKS; HEAPDBG; #else: _RELEASE;`.
- `#if defined(flagSTD_NEWDELETE) && !defined(STD_NEWDELETE): STD_NEWDELETE;`.
- `#ifdef _MSC_VER: #error RTTI must be enabled;`.
- Includes: typeinfo, stddef.h, math.h, limits.h, stdlib.h, stdio.h, string.h, stdarg.h, math.h, ctype.h; CPU_X86: immintrin.h / intrin.h / x86intrin.h.
- `#if defined(PLATFORM_POSIX): __USE_FILE_OFFSET64; DIR_SEP '/'; DIR_SEPS "/"; PLATFORM_PATH_HAS_CASE 1; Includes: errno.h, sys/types.h/stat.h/time.h/file.h, time.h, fcntl.h, unistd.h, pthread.h, semaphore.h, memory.h, dirent.h, signal.h, syslog.h, float.h;`.
- `#ifdef PLATFORM_WIN32: Includes: windows.h, winbase.h, winnt.h, winuser.h, wininet.h, commctrl.h, commdlg.h, shellapi.h, shlwapi.h, winsock2.h, ws2tcpip.h;`.
- `#ifdef PLATFORM_WINCE: Includes: aygshell.h; #define WINCE;`.
- `#ifdef PLATFORM_X11: Includes: X11/Xlib.h, X11/Xutil.h, X11/Xatom.h, X11/keysym.h, X11/Xft/Xft.h; #define PLATFORM_X11;`.
- `#ifdef PLATFORM_MACOS: Includes: CoreFoundation/CFBundle.h, ApplicationServices/ApplicationServices.h, Carbon/Carbon.h; #define PLATFORM_MACOS;`.
- `#ifdef PLATFORM_ANDROID: #define PLATFORM_ANDROID;`.
- `#ifdef PLATFORM_BSD: #define PLATFORM_BSD;`.
- Core includes: CoreCore.h, CoreItem.h, etc. up to ValueCache.h; SIMD: AsString for f32x4/i32x4/etc.
- Utils: `void RegisterTopic__(const char *topicfile, const char *topic, const char *title, const UPP::byte *data, int len); DLLHANDLE LoadDll__(UPP::String& fn, const char *const *names, void *const *procs); void FreeDll__(DLLHANDLE dllhandle); using Upp::byte;`.

## CtrlLib.h (Control Library Includes)

### Defines
- `INITIALIZE(CtrlLib); IMAGECLASS CtrlImg; IMAGEFILE <CtrlLib/Ctrl.iml>; #include <Draw/iml_header.h>; IMAGECLASS CtrlsImg; IMAGEFILE <CtrlLib/Ctrls.iml>; #include <Draw/iml_header.h>; #define LAYOUTFILE <CtrlLib/Ctrl.lay>; #include <CtrlCore/lay.h>;`.

### Includes
- CtrlCore.h; LabelBase.h, DisplayPopup.h, StaticCtrl.h, PushCtrl.h, MultiButton.h, ScrollBar.h, HeaderCtrl.h, EditCtrl.h, AKeys.h, Bar.h, StatusBar.h, TabCtrl.h, DlgColor.h, ArrayCtrl.h, DropChoice.h, TreeCtrl.h, Splitter.h, RichText.h, TextEdit.h, SliderCtrl.h, ColumnList.h, DateTimeCtrl.h, SuggestCtrl.h; Progress.h, FileSel.h, CtrlUtil.h, Lang.h; Ch.h.

## SliderCtrl.h (Slider Control)

### Class SliderCtrl : public Ctrl
- Members: int value/min/max/step; bool round_step/jump.
- Methods: `int SliderToClient(int value) const; int ClientToClient(int x) const; int HoVe(int x, int y) const; int Min() const; int Max() const; int ThumbSz() const; int SliderSz() const;`.
- Virtuals: `void Paint(Draw& draw); bool Key(dword key, int repcnt); void LeftDown/Repeat/Up(Point pos, dword keyflags); void MouseMove(Point pos, dword keyflags); void GotFocus/LostFocus(); void MouseEnter/Leave(Point p, dword keyflags); void SetData(const Value& value); Value GetData() const;`.
- Methods: `void Inc/Dec(); SliderCtrl& MinMax(int _min, int _max); SliderCtrl& Range(int max); int GetMin/Max() const; bool IsVert() const; SliderCtrl& Jump(bool v=true); SliderCtrl& Step(int _step, bool _r=true); int GetStep() const; bool IsRoundStep() const; Event<> WhenSlideFinish; SliderCtrl(); ~SliderCtrl();`.

## ColorPusher.cpp (Color Pusher Implementation - Source)

### Class ColorPusher (From Source)
- Members: Color color; bool push/track/withtext/withhex; ColorPopup colors; String nulltext/voidtext; Color saved_color.
- Virtuals: `void Paint(Draw& w); void LeftDown(Point p, dword); bool Key(dword key, int);`.
- Methods: `void Drop(); void AcceptColors(); void CloseColors(); void NewColor(); ColorPusher();`.
- Utils: `auto DrawColor = [&](int x,y,cx,cy) { ... };` (Draws color rect with dark theme handling).

### Class ColorButton
- Members: Color color; Image image/staticimage/nullimage; bool push; const ToolBar::Style *style.
- Virtuals: `Size GetMinSize() const; void Paint(Draw& w); void MouseEnter/Leave(Point p, dword keyflags);`.
- Methods: `ColorButton();`.

## Bar.h (Bar & Menu Controls)

### Class BarPane : public ParentCtrl
- Inner: `struct Item { Ctrl *ctrl; int gapsize; };`.
- Members: Array<Item> item; Vector<int> breakpos; bool horz; Ctrl *phelpctrl; int vmargin/hmargin; bool menu.
- Virtuals: `void LeftDown(Point pt, dword keyflags); void MouseMove(Point p, dword);`.
- Methods: `Event<> WhenLeftClick; void PaintBar(Draw& w, const SeparatorCtrl::Style& ss, const Value& pane, const Value& iconbar=Null, int iconsz=0); void IClear/Clear(); bool IsEmpty() const; void Add(Ctrl *ctrl, int gapsize); void AddBreak(); void AddGap(int gapsize); void Margin(int v, int h); Size Repos(bool horz, int maxsize); Size GetPaneSize(bool _horz, int maxsize) const; int GetCount() const; void SubMenu(); BarPane(); ~BarPane();`.

### Class Bar : public Ctrl
- Inner: `struct Item { virtual Item& Text/Key/Repeat/Image/Check/Radio/Enable/Bold/Tip/Help/Topic/Description(const char *); void FinalSync(); Item& Label(const char *text); Item& RightLabel(const char *text); Item& Sub(Event<Bar&> menu); Item& Sub(Bar& menu); Item& Separator(); Item& Break(); Item& Add(bool brk=false); Item& Add(const char *text, Event<> action); Item& Add(const char *text, Event<Bar&> submenu); Item& Add(const char *text, Callback action); Item& Add(const char *text, Callback1<Bar&> submenu); Item& Add(const Image& img, Event<> action); Item& Add(const Image& img, Event<Bar&> submenu); Item& Add(const Image& img, Callback action); Item& Add(const Image& img, Callback1<Bar&> submenu); };`.
- Methods: `void Execute(Ctrl *owner, Point p); static bool Execute(Ctrl *owner, Event<Bar&> proc, Point p); int GetStdHeight(Font font); void CancelMode(); Bar(); ~Bar();`.

### Class MenuBar : public Bar (From Source - MenuBar.cpp)
- (Compressed from source: Inherits Bar; members: bool action_taken; Ctrl *submenu/parentmenu; int submenui; etc. Methods: PopUp(Ctrl *owner, Point p); bool Execute(Ctrl *owner, Point p); bool Execute(Ctrl *owner, Event<Bar&> proc, Point p); int GetStdHeight(Font font); void CancelMode(); ~MenuBar(); Static styles: CH_STYLE(MenuBar, Style, StyleDefault); Utils: CtrlFrame& MenuFrame(); BorderFrame& XPMenuFrame();).

### Class ToolBar : public Ctrl
- Members: int kind/arealook; Size buttonminsize/maxiconsize; bool nodarkadjust; const Style *style.
- Virtuals: `void Paint(Draw& w); void Layout();`.
- Methods: `ToolBar& SetStyle(const Style& s); ToolBar& ButtonStyle(const Button::Style& bs); ToolBar& ButtonMinSize(Size sz); ToolBar& MaxIconSize(Size sz); ToolBar& ButtonKind(int _kind); ToolBar& AreaLook(int q=1); ToolBar& NoDarkAdjust(bool b=true); ToolBar(); ~ToolBar();`.

### Class StaticBarArea : public Ctrl
- Members: bool upperframe.
- Virtuals: `void Paint(Draw& w);`.
- Methods: `StaticBarArea& UpperFrame(bool b); StaticBarArea& NoUpperFrame(); StaticBarArea();`.

### Class LRUList
- Members: Vector<String> lru; int limit.
- Methods: `static int GetStdHeight(); void Serialize(Stream& stream); void operator()(Bar& bar, Event<const String&> WhenSelect, int count=INT_MAX, int from=0); void NewEntry(const String& path); void RemoveEntry(const String& path); int GetCount() const; LRUList& Limit(int _limit); int GetLimit() const; LRUList();`.

### Class ToolTip : public Ctrl
- Members: String text.
- Virtuals: `void Paint(Draw& w); Size GetMinSize() const;`.
- Methods: `void Set(const char *_text); String Get() const; void PopUp(Ctrl *owner, Point p, bool effect); ToolTip();`.

### Utils
- `void PerformDescription();`.

## ScrollBar.h (Scroll Bar Controls)

### Class ScrollBar : public FrameCtrl<Ctrl>, private VirtualButtons
- Inner: `struct Style : ChStyle<Style> { Color bgcolor; int barsize/arrowsize/thumbmin/overthumb/thumbwidth; bool through; Value vupper[4]/vthumb[4]/vlower[4]/hupper[4]/hthumb[4]/hlower[4]; Button::Style up/down/left/right/up2/down2/left2/right2; bool isup2/isdown2/isleft2/isright2; };`.
- Members: int thumbpos/thumbsize/delta/pagepos/pagesize/totalsize/linesize/minthumb/wheelaccumulator; int8 push/light; bool horz/jump/track/autohide/autodisable/is_active.
- Virtuals: `void Layout(); Size GetStdSize() const; void Paint(Draw& draw); void LeftDown/Move/Enter/Leave/Up/Repeat(Point p, dword); void MouseWheel(Point p, int zdelta, dword keyflags); void HorzMouseWheel(Point p, int zdelta, dword keyflags); void CancelMode(); void FrameLayout(Rect& r); void FrameAddSize(Size& sz); Image MouseEvent(int event, Point p, int zdelta, dword keyflags); int ButtonCount() const; Rect ButtonRect(int i) const; const Button::Style& ButtonStyle(int i) const; bool ButtonEnabled(int i) const; void ButtonPush/Repeat(int i);`.
- Enums: PREV/PREV2/NEXT/NEXT2/BUTTONCOUNT.
- Methods: `const Style *style; Rect Slider(int& cc) const; Rect Slider() const; int& HV(int& h, int& v) const; int GetHV(int h, int v) const; Rect GetSliderRect() const; bool IsHoriz() const; ScrollBar& SetStyle(const Style& s); ScrollBar& Vert(); ScrollBar& Horz(); ScrollBar& SetPage(int page, int total); void Set(int pos); int Get() const; void SetTotal(int total); int GetTotal() const; void SetPage(int page); int GetPage() const; void SetLine(int line); int GetLine() const; void ScrollInto(int pos); ScrollBar(); ~ScrollBar();`.

### Class ScrollBars
- Members: ScrollBar x/y; Ctrl *box; CtrlFrame *frame; SizeGrip *grip; bool fixedbox.
- Virtuals: `void Layout(); Size GetMinSize() const;`.
- Methods: `ScrollBars& Set(Ctrl& x, Ctrl& y); void SetSB(Ctrl& sb); void RemoveSB(Ctrl& sb); Point Get() const; void Set(Point p); void Set(int pos) { Set(Point(pos, pos)); } ScrollBars& AutoHide(bool b=true); /* Many setters: NoAutoHide, AutoDisable, Track, Jump, etc. */; ScrollBars& NormalBox/NoBox/FixedBox/Box(Ctrl& box)/WithSizeGrip(); ScrollBars& SetStyle(const ScrollBar::Style& s); operator Point() const; Point operator=(Point p); ScrollBars(); ~ScrollBars();`.

### Class Scroller
- Members: Point psb.
- Methods: `void Scroll(Ctrl& p, const Rect& rc, Point newpos, Size cellsize=Size(1,1)); void Scroll(Ctrl& p, const Rect& rc, int newpos, int linesize=1); void Scroll(Ctrl& p, Point newpos); void Scroll(Ctrl& p, int newposy); void Set(Point pos); void Set(int pos); void Clear(); Scroller();`.

## MenuBar.cpp (MenuBar Implementation - Source)

### Utils (From Source)
- `static ColorF xpmenuborder[] = { (ColorF)3, &SColorShadow (x4), &SColorMenu (x8) }; BorderFrame& XPMenuFrame(); CtrlFrame& MenuFrame(); CH_STYLE(MenuBar, Style, StyleDefault) { ... };` (Styles for topitem, text, bar, etc.; ImageBuffer for popupframe).
- Defines: LLOG(x), LTIMING(x).

### Class MenuBar : public Bar (Implementation Details)
- Members: bool action_taken; Ctrl *submenu/*parentmenu; int submenui; etc. (From source: PopUp, Execute, GetStdHeight, CancelMode, ~MenuBar).
- Static: Vector<Ctrl *> ows; (Prevents repeated opens).

## ColorPopup.cpp (Color Popup Implementation - Source)

### Static Data
- `struct { const char *name; Color color; } s_colors[] = { {"SBlack", Color::Special(0)}, ... {"White", White} }; Color ColorPopUp::hint[18];`.

### Class ColorPopUp
- Inner Classes: `class WheelCtrl : public Ctrl { ... }; class RampCtrl : public Ctrl { ... };` (Paint, mouse events for color wheel/ramp).
- Members: Color color; int colori; bool norampwheel/notnull/withvoid/scolors/animating/hints/open; ColorPopup *popup; Ctrl *owner; Rect rt; String nulltext/voidtext; WheelCtrl wheel; RampCtrl ramp; Button settext.
- Methods: `ColorPopUp& DarkContent(bool b); ColorPopUp& AllowDarkContent(bool b); ColorPopUp& NoRampWheel(); ColorPopUp& NotNull(); ColorPopUp& WithVoid(); ColorPopUp& SColors(); ColorPopUp& Hints(); void Ramp(Color c); void Wheel(Color c); void PopUp(Ctrl *owner, bool effect=true); void Select(); void Layout(); ColorPopUp();`.
- Events: `Event<Color> WhenAction; Event<> WhenSelect/WhenCancel;`.



## StaticCtrl.h (Static Controls)

### Enums (StaticText)
- `enum { ATTR_INK = Ctrl::ATTR_LAST, ATTR_FONT, ATTR_ALIGN, ATTR_IMAGE, ATTR_IMAGE_SPC, ATTR_VALIGN, ATTR_ORIENTATION, ATTR_LAST };`.

### Class StaticText : public Ctrl
- Members: String text; int accesskey=0.
- Virtuals: `void Paint(Draw& w); Size GetMinSize() const;`.
- Methods: `void MakeDrawLabel(DrawLabel& l) const; Size GetLabelSize() const; StaticText& SetFont(Font font); StaticText& SetInk(Color color); StaticText& SetAlign(int align); StaticText& AlignLeft/Center/Right(); StaticText& SetVAlign(int align); StaticText& AlignTop/VCenter/Bottom(); StaticText& SetImage(const Image& img, int spc=0); StaticText& SetText(const char *text); StaticText& SetOrientation(int orientation); String GetText() const; Font GetFont() const; Color GetInk() const; int GetAlign() const; int GetVAlign() const; Image GetImage() const; int GetOrientation() const; StaticText& operator=(const char *s); StaticText();`.

### Class StaticRect : public Ctrl
- Members: Value bg.
- Virtuals: `void Paint(Draw& w); Size GetMinSize() const;`.
- Methods: `StaticRect& Background(Color c); Value GetBackground() const; StaticRect(); ~StaticRect();`.

### Class ImageCtrl : public Ctrl
- Members: Image img.
- Virtuals: `void Paint(Draw& w); Size GetStdSize() const; Size GetMinSize() const;`.
- Methods: `ImageCtrl& SetImage(const Image& _img); ImageCtrl();`.

### Class DisplayCtrl : public Ctrl
- Members: PaintRect pr.
- Virtuals: `void Paint(Draw& w); Size GetMinSize() const; void SetData(const Value& v); Value GetData() const;`.
- Methods: `void SetDisplay(const Display& d);`.

### Class DrawingCtrl : public Ctrl
- Members: Drawing picture; Color background; bool ratio.
- Virtuals: `void Paint(Draw& w);`.
- Methods: `Drawing Get() const; DrawingCtrl& Background(Color color); DrawingCtrl& KeepRatio(bool keep=true); DrawingCtrl& NoKeepRatio(); DrawingCtrl& Set(const Drawing& _picture); DrawingCtrl& operator=(const Drawing& _picture); DrawingCtrl& operator=(const Painting& _picture); DrawingCtrl();`.

### Typedefs (BWC)
- `typedef ImageCtrl Icon; typedef DrawingCtrl Picture;`.

### Class SeparatorCtrl : public Ctrl
- Inner: `struct Style : ChStyle<Style> { Value l1/l2; };`.
- Members: int lmargin/rmargin/size; const Style *style.
- Virtuals: `Size GetMinSize() const; void Paint(Draw& w);`.
- Methods: `static const Style& StyleDefault(); SeparatorCtrl& Margin(int l, int r); SeparatorCtrl& Margin(int w); SeparatorCtrl& SetSize(int w); SeparatorCtrl& SetStyle(const Style& s); SeparatorCtrl();`.

## DisplayPopup.h (Display Popup)

### Inner Class Pop : Pte<Pop>
- Inner: `struct PopCtrl : public Ctrl { virtual void Paint(Draw& w); struct Pop *p; };`.
- Members: Ptr<Ctrl> ctrl; Rect item; Value value; Color paper/ink; dword style; const Display *display; int margin; bool usedisplaystdsize; PopCtrl view/frame; Callback WhenClose.
- Methods: `void Set(Ctrl *ctrl, const Rect& item, const Value& v, const Display *display, Color ink, Color paper, dword style, int margin=0); void Sync(); static Vector<Pop *>& all(); Pop(); ~Pop();`.

### Class DisplayPopup : public Pte<DisplayPopup>
- Members: One<Pop> popup; bool usedisplaystdsize.
- Statics: `bool StateHook(Ctrl *, int reason); bool MouseHook(Ctrl *, bool, int, Point, int, dword); void SyncAll(); Rect Check(Ctrl *ctrl, const Rect& item, const Value& value, const Display *display, int margin);`.
- Methods: `void Set(Ctrl *ctrl, const Rect& item, const Value& v, const Display *display, Color ink, Color paper, dword style, int margin=0); void Cancel(); bool IsOpen(); bool HasMouse(); void UseDisplayStdSize(); DisplayPopup(); ~DisplayPopup();`.

## Report.h (Report Generation)

### Class Report : public DrawingDraw, public PageDraw
- Virtuals: `Draw& Page(int i); Size GetPageSize() const; void StartPage();`.
- Members: Array<Drawing> page; int pagei/y; String header/footer; int headercy/headerspc/footercy/footerspc; Point mg; One<PrinterJob> printerjob.
- Methods: `Callback WhenPage; int GetCount(); Drawing GetPage(int i); Drawing operator[](int i); const Array<Drawing>& GetPages(); void Clear(); Rect GetPageRect(); Size GetPageSize(); void SetY(int _y); int GetY() const; void NewPage(); void RemoveLastPage(); void Put(const RichText& txt, void *context=NULL); void Put(const char *qtf); Report& operator<<(const char *qtf); Report& Header(const char *s); Report& NoHeader(); Report& Footer(const char *s); Report& NoFooter(); Report& HeaderSpace(int cy); Report& FooterSpace(int cy); Report& Margins(int left, int top, int right, int bottom); Report& Margins(int cx); Report& Margins(const Point& p); Report& Margins(const Size& sz); Report& Margins(const Rect& rc); Report& PageSize(Size sz); Report& PageSize(int cx, int cy); Report& PageSize(const Rect& rc); Report& Print(PrinterJob& pj); Report(); ~Report();`.

### Class ReportView : public Ctrl
- Members: ScrollBar sb; Report *report; Image page[64]; int pagei[64]; Size pagesize; int vsize/pm/pvn/numbers/pages.
- Methods: `Image GetPage(int i); void Init(); void Sb(); void Numbers(); Size GetReportSize(); Callback WhenGoPage; enum Pages { PG1/PG2/PG4/PG16 }; ReportView& Pages(int pags); ReportView& Numbers(bool nums); void Set(Report& report); Report *Get(); int GetFirst() const; void ScrollInto(int toppage, int top, int bottompage, int bottom); ReportView();`.

### Class ReportWindow : public WithReportWindowLayout<TopWindow>
- Methods: `void Pages(); void Numbers(); void GoPage(); void Pdf(); void ShowPage(); Array<Button> button; Report *report; ReportView pg; static void SetPdfRoutine(String (*pdf)(const Report& report, int margin)); void SetButton(int i, const char *label, int id); int Perform(Report& report, int zoom=100, const char *caption=t_("Report")); ReportWindow();`.

### Utils
- `String Pdf(Report& report, bool pdfa=false, const PdfSignatureInfo *sign=NULL); void Print(Report& r, PrinterJob& pd); bool DefaultPrint(Report& r, int i, const char *_name=t_("Report")); bool Print(Report& r, int i, const char *name=t_("Report")); bool Perform(Report& r, const char *name=t_("Report")); bool QtfReport(const String& qtf, const char *name="", bool pagenumbers=false); bool QtfReport(Size pagesize, const String& qtf, const char *name="", bool pagenumbers=false);`.

## Display.h (Display System)

### Defines
- `IMAGECLASS DrawImg; IMAGEFILE <Draw/DrawImg.iml>; #include <Draw/iml_header.h>;`.

### Class Display
- Enums: `CURSOR=0x01, FOCUS=0x02, SELECT=0x04, READONLY=0x08;`.
- Virtuals: `void PaintBackground(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const; void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const; Size GetStdSize(const Value& q) const; Size RatioSize(const Value& q, int cx, int cy) const; ~Display();`.

### Struct AttrText : public ValueType<AttrText, 151, Moveable<AttrText>>
- Members: WString text; Value value; Font font; Color ink/normalink/paper/normalpaper; int align; Image img; int imgspc.
- Methods: `AttrText& Set(const Value& v); AttrText& operator=(const Value& v); AttrText& Text(const String/WString/char *txt); AttrText& Ink/NormalInk/Paper/NormalPaper(Color c); AttrText& SetFont(Font f); AttrText& Bold/Italic/Underline/Strikeout(bool b=true); AttrText& Align(int a); AttrText& Image(const Image& i, int spc=0); AttrText();`.

### Class StdDisplay : public Display
- Methods: `virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const; private: String nulltext;`.

### Class DisplayWithIcon : public Display
- Members: const Display *display; Image icon; int lspc.
- Virtuals: `void PaintBackground/Draw& w, ...); void Paint(Draw& w, ...); Size GetStdSize(const Value& q) const;`.
- Methods: `void SetIcon(const Image& img, int spc=4); void SetDisplay(const Display& d); void Set(const Display& d, const Image& m, int spc=4); DisplayWithIcon();`.

### Class PaintRect : Moveable<PaintRect>
- Members: Value value; const Display *display.
- Methods: `void Paint(Draw& w, const Rect& r, Color ink=SColorText, Color paper=SColorPaper, dword style=0) const; void Paint(Draw& w, int x,y,cx,cy, ... ) const; Size GetStdSize() const; Size RatioSize(int cx, int cy) const; Size RatioSize(Size sz) const; void SetDisplay(const Display& d); void SetValue(const Value& v); void Set(const Display& d, const Value& v); void Clear(); const Value& GetValue() const; const Display& GetDisplay() const; operator bool() const; PaintRect(); PaintRect(const Display& display); PaintRect(const Display& display, const Value& val);`.

## FontInt.h (Font Internals)

### Struct FaceInfo : Moveable<FaceInfo>
- Members: String name; dword info=0.

### Struct CommonFontInfo
- Members: int ascent/descent/external/internal/overhang/avewidth/maxwidth/firstchar/charcount/default_char/spacebefore/spaceafter/aux/colorimg_cy; bool fixedpitch/scaleable/ttf; char path[256]; int fonti=0.

### Struct GlyphInfo
- Members: int16 width/lspc/rspc; word glyphi=0.
- Methods: `bool IsNormal() const; bool IsComposed() const; bool IsComposedLM() const; bool IsReplaced() const; bool IsMissing() const;`.

### Utils
- `void Std(Font& font); GlyphInfo GetGlyphInfo(Font font, int chr); const CommonFontInfo& GetFontInfo(Font font); bool IsNormal_nc(Font font, int chr); void GlyphMetrics(GlyphInfo& f, Font font, int chr); void InvalidateFontList(); CommonFontInfo GetFontInfoSys(Font font); GlyphInfo GetGlyphInfoSys(Font font, int chr); Vector<FaceInfo> GetAllFacesSys(); String GetFontDataSys(Font font, const char *table, int offset, int size); void RenderCharacterSys(FontGlyphConsumer& sw, double x, double y, int ch, Font fnt);`.

## PushCtrl.h (Push Controls)

### Class Pusher : public Ctrl
- Members: bool push/keypush/clickfocus; int accesskey; String label; Font font.
- Virtuals: `void CancelMode(); void LeftDown/Move/Leave/Repeat/Up(Point, dword); void GotFocus/LostFocus(); void State(int); String GetDesc() const; bool Key(dword key, int); bool HotKey(dword key); dword GetAccessKeys() const; void AssignAccessKeys(dword used);`.
- Methods: `void EndPush(); void KeyPush(); bool IsPush() const; bool IsKeyPush(); bool FinishPush(); virtual void RefreshPush/Focus(); virtual void PerformAction(); Pusher& SetFont(Font fnt); Pusher& SetLabel(const char *text); Pusher& ClickFocus(bool cf=true); Pusher& NoClickFocus(); bool IsClickFocus() const; Font GetFont() const; String GetLabel() const; void PseudoPush(); int GetVisualState() const; Event<> WhenPush/WhenRepeat; Pusher(); ~Pusher();`.

### Class Button : public Pusher
- Inner: `struct Style : ChStyle<Style> { Value look[4]; Color monocolor[4]/textcol[4]/ftextcol[4]; Point pressoffset; Value edge; int border; bool activeedge; };`.
- Virtuals: `void Paint(Draw& draw); bool Key(dword key, int); bool HotKey(dword key); void MouseEnter/Leave(Point, dword); dword GetAccessKeys() const; void AssignAccessKeys(dword used); void Layout(); void GotFocus/LostFocus(); int OverPaint() const;`.
- Methods: `static const Style& StyleDefault/3D/Flat/3DFlat(); Button& SetStyle(const Style& s); Button& SetLabel(const char *s); Button& SetImage(const Image& img); Button& SetMonoImage(const Image& img); Button& SetImage(const Image& img, const Image& disabled); Button& SetMonoImage(const Image& img, const Image& disabled); Button& SetImage(const Image& img, const Image& over, const Image& push); Button& SetMonoImage(const Image& img, const Image& over, const Image& push); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush, const Image& checkedhighlight); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush, const Image& checkedhighlight); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush, const Image& checkedhighlight, const Image& disabledchecked); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush, const Image& checkedhighlight, const Image& disabledchecked); Button& SetImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush, const Image& checkedhighlight, const Image& disabledchecked, const Image& disabledcheckedover); Button& SetMonoImage(const Image& img, const Image& disabled, const Image& over, const Image& push, const Image& highlight, const Image& checked, const Image& checkedover, const Image& checkedpush, const Image& checkedhighlight, const Image& disabledchecked, const Image& disabledcheckedover); Button();`.

### Class DataPusher : public Pusher
- Members: const Display *display; const Convert *convert; Value value; String nulltext; Color nullink; Font nullfont.
- Virtuals: `void SetData(const Value& v); Value GetData() const; void SetDataAction(const Value& value);`.
- Methods: `void Set(const Value& value); DataPusher& NullText(const char *text=t_("(default)"), Color ink=Brown); DataPusher& NullText(const char *text, Font fnt, Color ink); DataPusher(); DataPusher(const Convert& convert, const Display& display); DataPusher(const Display& display);`.

### Class SpinButtons : public CtrlFrame
- Inner: `struct Style : ChStyle<Style> { Button::Style inc/dec; int width/over; bool onsides; };`.
- Members: bool visible; const Style *style; Button inc/dec.
- Virtuals: `void FrameLayout(Rect& r); void FrameAddSize(Size& sz); void FrameAdd(Ctrl& ctrl); void FrameRemove();`.
- Methods: `void Show(bool s=true); bool IsVisible() const; static const Style& StyleDefault/OnSides(); SpinButtons& SetStyle(const Style& s); SpinButtons& OnSides(bool b=true); bool IsOnSides() const; SpinButtons(); ~SpinButtons();`.

### Struct VirtualButtons
- Virtuals: `int ButtonCount() const; Rect ButtonRect(int i) const; const Button::Style& ButtonStyle(int i) const; Image ButtonImage(int i) const; bool ButtonMono(int i) const; bool ButtonEnabled(int i) const; void ButtonPush/Repeat/Action(int i);`.
- Members: int8 pushi=-1/mi=-1; bool buttons_capture=false.
- Methods: `int FindButton(Point p) const; void EndPush(Ctrl *ctrl); void ButtonsCancelMode(); bool ButtonsMouseEvent(Ctrl *ctrl, int event, Point p); void PaintButtons(Draw& w, Ctrl *ctrl); int ButtonVisualState(Ctrl *ctrl, int i); void RefreshButton(Ctrl *ctrl, int i);`.

## DropChoice.h (Drop-down Choice)

### Class PopUpTable : public ArrayCtrl (Deprecated)
- Virtuals: `void LeftUp(Point p, dword keyflags); bool Key(dword key, int);`.
- Members: int droplines/inpopup; bool open; One<Popup> popup.
- Inner: `struct Popup : Ctrl { PopUpTable *table; virtual void Deactivate/CancelMode(); };`.
- Methods: `void PopupDeactivate/CancelMode(); void DoClose(); void PopUp(Ctrl *owner, int x, int top, int bottom, int width); void PopUp(Ctrl *owner, int width); void PopUp(Ctrl *owner); Event<> WhenCancel/WhenSelect; PopUpTable& SetDropLines(int _droplines); void Normal(); PopUpTable(); ~PopUpTable();`.

### Class PopUpList
- Inner: `struct PopupArrayCtrl : ArrayCtrl { PopUpList *list; virtual void LeftUp/ bool Key(...); }; struct Popup : Ctrl { PopUpList *list; PopupArrayCtrl ac; bool closing=false; virtual void Deactivate/CancelMode(); Popup(PopUpList *list); };`.
- Members: Vector<Value> items; Vector<word> lineinfo; Vector<const Display *> linedisplay; One<Popup> popup; const ScrollBar::Style *sb_style; const Display *display; const Convert *convert; int linecy; int cursor=-1; int16 droplines/inpopup; bool permanent.
- Methods: `void PopupDeactivate/CancelMode(); void DoSelect/Cancel/Close(); Event<> WhenCancel/WhenSelect; void Add(const Value& val); void Add(const Value& val, const Display& d); void SetDisplay(const Display& d); void SetConvert(const Convert& c); void SetLineCy(int cy); int GetLineCy() const; void Clear(); int GetCount() const; int GetCursor() const; void SetCursor(int c); void KillCursor(); void GoUp/Down(bool select); void PageUp/Down(bool select); void Home(bool select); void End(bool select); void PopUp(Ctrl *owner, int x, int y, int width); void PopUp(Ctrl *owner, int width); void PopUp(Ctrl *owner); void Normal(); bool IsOpen() const; void Permanent(bool b=true); void SetSbStyle(const ScrollBar::Style& s); PopUpList(); ~PopUpList();`.

### Class DropChoice : public Ctrl
- Virtuals: `void LeftDown/Up/Repeat(Point p, dword keyflags); void MouseMove(Point p, dword keyflags); void MouseWheel(Point p, int zdelta, dword keyflags); void LeftDrag(Point p, dword keyflags); void CancelMode(); bool Key(dword key, int count); void GotFocus/LostFocus(); void SetData(const Value& v); Value GetData() const;`.
- Members: PopUpList list; const Convert *convert; const Display *display; Value value; int dropwidth/dropfocus; bool notnull/alwaysdrop/hidedrop/rdonlydrop/updownkeys.
- Methods: `void DoSelect(); void DoDrop(); void DoKey(dword key); void DoWheel(int z); void RefreshDisplay(); DropChoice& SetDisplay(const Display& d); DropChoice& ValueDisplay(const Display& d); DropChoice& SetConvert(const Convert& c); DropChoice& DropWidth(int w); DropChoice& DropWidthZ(int w); DropChoice& DropLines(int lines); DropChoice& SetDropLines(int n); DropChoice& AlwaysDrop(bool b=true); DropChoice& HideDrop(bool b=true); DropChoice& RdOnlyDrop(bool b=true); DropChoice& UpDownKeys(bool b=true); DropChoice& NoUpDownKeys(); Event<> WhenDrop/WhenSelect; DropChoice();`.

### Template Class WithDropChoice<T> : public T
- Methods: `WithDropChoice(); bool Key(dword key, int repcnt); void MouseWheel(Point p, int zdelta, dword keyflags); void MouseEnter(Point p, dword keyflags); void MouseLeave(); void GotFocus/LostFocus(); void DoWhenDrop/Select(); WithDropChoice& SetConvert(const Convert& d); WithDropChoice& AlwaysDrop(bool b=true); WithDropChoice& HideDrop(bool b=true); WithDropChoice& RdOnlyDrop(bool b=true); WithDropChoice& WithWheel(bool b=true); WithDropChoice& NoWithWheel(); WithDropChoice& DropWidth(int w); WithDropChoice& DropWidthZ(int w); WithDropChoice& UpDownKeys(bool b=true); WithDropChoice& NoUpDownKeys(); WithDropChoice();`.

## GLDraw.h (OpenGL Drawing)

### Defines
- `GLEW_STATIC; #include <plugin/glew/glew.h>; #include <plugin/tess2/tess2.h>; #ifdef PLATFORM_WIN32: #include <plugin/glew/wglew.h>; #define GL_USE_SHADERS; #define GL_COMB_OPT; #include <GL/gl.h>;`.

### Enums
- `TEXTURE_LINEAR=0x01, TEXTURE_MIPMAP=0x02, TEXTURE_COMPRESSED=0x04; ATTRIB_VERTEX=1, ATTRIB_COLOR, ATTRIB_TEXPOS, ATTRIB_ALPHA;`.

### Utils
- `GLuint CreateGLTexture(const Image& img, dword flags); GLuint GetTextureForImage(dword flags, const Image& img, uint64 context=0); inline GLuint GetTextureForImage(const Image& img, uint64 context=0);`.

### Class GLProgram
- Members: GLuint vertex_shader/fragment_shader/program; int64 serialid.
- Methods: `void Compile(const char *vertex_shader_, const char *fragment_shader_); void Link(); void Create(const char *vertex_shader, const char *fragment_shader, Tuple2<int, const char *> *bind_attr=NULL, int bind_count=0); /* Overloads with attr1/2/3 */; void Clear(); int GetAttrib(const char *name); int GetUniform(const char *name); void Use(); GLProgram(); ~GLProgram();`.

### Globals
- `extern GLProgram gl_image/gl_image_colored/gl_rect;`.

### Class GLDraw : public SDraw
- Inner: `#ifdef GL_COMB_OPT: struct RectColor : Moveable<RectColor> { Rect rect; Color color; }; Vector<RectColor> put_rect; #endif`.
- Members: uint64 context.
- Methods: `void SetColor(Color c); void FlushPutRect(); void Flush(); virtual void PutImage(Point p, const Image& img, const Rect& src); #ifdef GL_USE_SHADERS: virtual void PutImage(Point p, const Image& img, const Rect& src, Color color); #endif virtual void PutRect(const Rect& r, Color color); void Init(Size sz, uint64 context=0); void Finish(); static void ClearCache/ResetCache(); GLDraw(); GLDraw(Size sz); ~GLDraw();`.

### Utils
- `void GLOrtho(float left, float right, float bottom, float top, float near_, float far_, GLuint u_projection);`.

### Includes
- `"GLPainter.h"`.

## HeaderCtrl.h (Header Control)

### Class HeaderCtrl : public Ctrl, public CtrlFrame
- Inner: `struct Style : ChStyle<Style> { Value look[4]; int gridadjustment; bool pressoffset; }; class Column : public LabelBase { ... Event<> WhenLeftClick/LeftDouble/Action; Event<Bar&> WhenBar; Column& Min/Max/MinMax/Fixed(int); Column& Tip(const char *s); Column& SetPaper(Color c); Column& SetRatio(double ratio); Column& SetMargin(int m); void Show(bool b=true); void Hide(); int GetMin/Max() const; double GetRatio() const; int GetMargin() const; bool IsVisible() const; void Paint(...); };`.
- Virtuals: `void CancelMode(); void Paint(Draw& draw); Image CursorImage(Point p, dword keyflags); void LeftDown/Double/Drag/Move/Leave/Up/RightDown(Point p, dword keyflags); void Serialize(Stream& s); void Layout(); void FrameAdd/Remove(Ctrl& parent); void FrameLayout(Rect& r); void FrameAddSize(Size& sz);`.
- Members: Array<Column> col; ScrollBar sb; const Style *style; int mode/sbtotal/sb; bool track/moving/autohidesb.
- Methods: `Event<> WhenDragFinish; void Add(Column& c); void Insert(int i, Column& c); void SetCount(int n); int GetCount() const; void Remove(int i); void Clear(); Column& operator[](int i); const Column& operator[](int i) const; void SetVisible(int i, bool b); bool IsVisible(int i) const; void SetTabRatio(int i, double ratio); double GetTabRatio(int i) const; void SetTabWidth(int i, int cx); int GetTabWidth(int i); void SwapTabs(int first, int second); void MoveTab(int from, int to); int GetTabIndex(int i) const; int FindIndex(int ndx) const; void StartSplitDrag(int s); int GetSplit(int x); int GetScroll() const; bool IsScroll() const; void SetHeight(int cy); int GetHeight() const; int GetMode() const; static const Style& StyleDefault(); HeaderCtrl& Invisible(bool inv); HeaderCtrl& Track(bool _track=true); HeaderCtrl& NoTrack(); HeaderCtrl& Proportional/ReduceNext/ReduceLast/Absolute/Fixed(); HeaderCtrl& SetStyle(const Style& s); HeaderCtrl& Moving(bool b=true); HeaderCtrl& AutoHideSb(bool b=true); HeaderCtrl& NoAutoHideSb(); HeaderCtrl& HideSb(bool b=true); HeaderCtrl& SetScrollBarStyle(const ScrollBar::Style& s); static int GetStdHeight(); HeaderCtrl(); ~HeaderCtrl();`.

## TextEdit.h (Text Editing)

### Class TextCtrl : public Ctrl, protected TextArrayOps
- Inner: `struct UndoRec { int serial/pos/size; String data; bool typing; void SetText(const String& text); String GetText() const; }; struct UndoData { int undoserial; BiArray<UndoRec> undo/redo; void Clear(); }; struct EditPos : Moveable<EditPos> { int sby; int64 cursor; void Serialize(Stream& s); void Clear(); EditPos(); };`.
- Enums: `INK_NORMAL/DISABLED/SELECTED, PAPER_NORMAL/READONLY/SELECTED, WHITESPACE/WARN_WHITESPACE, COLOR_COUNT;`.
- Virtuals: `void SetData(const Value& v); Value GetData() const; void CancelMode(); String GetSelectionData(const String& fmt) const; void MiddleDown(Point p, dword flags);`.
- Methods: `Event<> WhenLeftUp; TextCtrl();`.

### Class DocEdit : public TextCtrl
- Members: WString text; int total/anchor/cursor/after/posy; bool overtype/eofline/updownleave; ScrollBar sb; Font font; int (*filter)(int c); int tabsize=4; Color color[COLOR_COUNT]; Rect caret; struct Fmt { FontInfo fi; int len; Buffer<wchar> text; Buffer<int> width; Vector<int> line; int LineEnd(int i); }; Fmt Format(const WString& text) const;.
- Virtuals: `int64 GetTotal() const; int GetCharAt(int64 i) const; void DirtyFrom(int line); void SelectionChanged(); void ClearLines(); void InsertLines(int line, int count); void RemoveLines(int line, int count); void PreInsert/PostInsert(int pos, const WString& text); void PreRemove/PostRemove(int pos, int size); void SetSb(); void PlaceCaret(int64 newcursor, bool sel=false); void InvalidateLine(int i); int RemoveRectSelection(); WString CopyRectSelection(); int PasteRectSelection(const WString& s); String GetPasteText(); int64 GetChar(int64 i) const; wchar GetChar64(int64 i) const; void Insert(int64 pos, const WString& s); void Remove(int64 pos, int64 size); void GatherFurture(int line); void SetFont(int chr, Font f); void SetInk(int chr, Color ink); void SetPaper(int chr, Color paper); void SetLoading(); bool IsModified() const; void ClearModify(); void NextUndo() const; void PrevUndo() const; void PickUndo(); void Undo(); void Redo(); void SerializeInsertOp(Stream& s, int pos, const WString& text, bool typing); void SerializeRemoveOp(Stream& s, int pos, int size, bool typing); void SerializeOp(Stream& s, const UndoRec& u, bool redo); void MouseTip(Point p); void LeftDown(Point p, dword); void LeftDouble(Point p, dword); void RightDown(Point p, dword); void LeftUp(Point p, dword); void LeftDrag(Point p, dword); void MouseMove(Point p, dword); bool Key(dword key, int); void MouseWheel(Point p, int zdelta, dword); void LostFocus(); void GotFocus(); void SetFocus0(); void KillCursor(); void CenterCursor(); void Set(const WString& s); void PasteString(const String& s); void PasteText(const WString& s); void Print(Draw& w, const Rect& page, const Value& q, Color ink, Color paper, dword style) const; Size GetStdSize(const Value& q) const; void Paint(Draw& w); Size GetMinSize() const; void Layout(); void Invalidate(); int GetHeight(int i); void Scroll(); void PlaceCaret(bool scroll); int GetY(int parai); int GetCursorPos(Point p); Point GetCaret(int pos); void VertMove(int delta, bool select, bool scs); void HomeEnd(int x, bool select); void RefreshStyle(); Rect DropCaret(); void RefreshDropCaret(); int GetMousePos(Point p); DocEdit& After(int a); DocEdit& SetFont(Font f); DocEdit& SetFilter(int (*f)(int c)); DocEdit& AutoHideSb(bool b=true); bool IsAutoHideSb() const; DocEdit& UpDownLeave(bool u=true); DocEdit& NoUpDownLeave(); bool IsUpDownLeave() const; DocEdit& SetScrollBarStyle(const ScrollBar::Style& s); DocEdit& EofLine(bool b=true); DocEdit& NoEofLine(); bool IsEofLine() const; EditPos GetEditPos() const; void SetEditPos(const TextCtrl::EditPos& pos); DocEdit(); ~DocEdit();`.

