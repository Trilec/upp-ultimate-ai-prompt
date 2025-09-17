
### ✅ U++ Ground-Truth Code Snippet Library
*27 snippets from U++ files, unified into one clean, structured*

---

// # Code SNIPPET: Basic U++ application with custom Paint() drawing text and rectangles
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct MyApp : public TopWindow {
    void Paint(Draw& w) override {
        w.DrawRect(GetSize(), SColorWhite);
        w.DrawText(0, 0, "Hello from U++!", Font().Bold(), SColorBlack);
    }
};

GUI_APP_MAIN
{
    MyApp().Run();
}
```

// # Code SNIPPET: Button click handler with THISBACK and dynamic Label update
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct MyApp : public TopWindow {
    Button btn;
    Label status;

    void Click() {
        Beep();
        status.SetLabel("Button clicked!");
    }

    MyApp() {
        btn.SetLabel("Click Me");
        btn <<= THISBACK(Click);
        Add(btn.HSizePos().VSizePos());

        status.SetLabel("Ready");
        Add(status.HSizePos().TopPos(10));
    }
};
GUI_APP_MAIN
{
    MyApp().Run();
}
```

// # Code SNIPPET: Directory comparison UI with TreeCtrl, FileSel, FindFile, and VectorMap for file/directory diffing
```cpp
DlgCompareDir::DlgCompareDir()
{
	CtrlLayout(*this, "Compare directories");
	Sizeable().Zoomable();
	refresh << [=] { CmdRefresh(); };
	splitter.Vert(tree, editor);
	editor << lineedit.SizePos() << qtf.SizePos();
	qtf.Background(White());
	qtf.SetFrame(InsetFrame());
	path_a.AddFrame(browse_a);
	browse_a.SetImage(CtrlImg::right_arrow());
	browse_a << [=] { DoBrowse(&path_a); };
	path_b.AddFrame(browse_b);
	browse_b.SetImage(CtrlImg::right_arrow());
	browse_b <<  [=] { DoBrowse(&path_b); };
	file_mask <<= "*.cpp *.h *.hpp *.c *.C *.cxx *.cc *.lay *.iml *.upp *.sch *.dph";
	tree.WhenCursor = [=] { DoTreeCursor(); };
	lineedit.SetReadOnly();
	lineedit.SetFont(Courier(14));
}

bool DlgCompareDir::FetchDir(String dir, VectorMap<String, FileInfo>& files, VectorMap<String, String>& dirs)
{
	FindFile ff;
	if(!ff.Search(AppendFileName(dir, "*")))
		return false;
	do
		if(ff.IsFile() && PatternMatchMulti(fm, ff.GetName()))
			files.Add(NormalizePathCase(ff.GetName()), FileInfo(ff.GetName(), ff.GetLength(), ff.GetLastWriteTime()));
		else if(ff.IsFolder())
			dirs.Add(NormalizePathCase(ff.GetName()), ff.GetName());
	while(ff.Next());
	return true;
}

int DlgCompareDir::Refresh(String rel_path, int parent)
{
	FindFile ff;
	VectorMap<String, FileInfo> afile, bfile;
	VectorMap<String, String> adir, bdir;
	String arel = AppendFileName(pa, rel_path);
	String brel = AppendFileName(pb, rel_path);
	int done = 0;
	if(!FetchDir(arel, afile, adir))
		done |= 2;
	if(!FetchDir(brel, bfile, bdir))
		done |= 1;

	Index<String> dir_index;
	dir_index <<= adir.GetIndex();
	FindAppend(dir_index, bdir.GetKeys());
	Vector<String> dirs(dir_index.PickKeys());
	Sort(dirs, GetLanguageInfo());
	for(int i = 0; i < dirs.GetCount(); i++) {
		int fa = adir.Find(dirs[i]), fb = bdir.Find(dirs[i]);
		String dn = (fb >= 0 ? bdir[fb] : adir[fa]);
		int dirpar = tree.Add(parent, CtrlImg::Dir(), dn);
		int dirdone = Refresh(AppendFileName(rel_path, dirs[i]), dirpar);
		done |= dirdone;
		switch(dirdone) {
		case 0: tree.Remove(dirpar); break;
		case 1: tree.SetNode(dirpar, TreeCtrl::Node().SetImage(CompDirImg::a_dir()).Set(dn)); break;
		case 2: tree.SetNode(dirpar, TreeCtrl::Node().SetImage(CompDirImg::b_dir()).Set(dn)); break;
		case 3: tree.SetNode(dirpar, TreeCtrl::Node().SetImage(CompDirImg::ab_dir()).Set(dn)); break;
		}
	}
	Index<String> name_index;
	name_index <<= afile.GetIndex();
	FindAppend(name_index, bfile.GetKeys());
	Vector<String> names(name_index.PickKeys());
	Sort(names, GetLanguageInfo());
	for(int i = 0; i < names.GetCount(); i++) {
		int fa = afile.Find(names[i]), fb = bfile.Find(names[i]);
		if(fa < 0) {
			tree.Add(parent, CompDirImg::b_file(), NFormat("%s: B (%`, %0n)", bfile[fb].name, bfile[fb].time, bfile[fb].size));
			done |= 2;
		}
		else if(fb < 0) {
			tree.Add(parent, CompDirImg::a_file(), NFormat("%s: A (%`, %0n)", afile[fa].name, afile[fa].time, afile[fa].size));
			done |= 1;
		}
		else if(afile[fa].size != bfile[fb].size
		|| LoadFile(AppendFileName(arel, names[i])) != LoadFile(AppendFileName(brel, names[i]))) {
			tree.Add(parent, CompDirImg::ab_file(), NFormat("%s: A (%`, %0n), B (%`, %0n)",
				bfile[fb].name, afile[fa].time, afile[fa].size, bfile[fb].time, bfile[fb].size));
			done |= 3;
		}
	}
	return done;
}

void DlgCompareDir::DoBrowse(Ctrl *field)
{
	FileSel fsel;
	fsel.AllFilesType();
	static String recent_dir;
	fsel <<= Nvl((String)~*field, recent_dir);
	if(fsel.ExecuteSelectDir())
		*field <<= recent_dir = ~fsel;
}
```

// # Code SNIPPET: Displaying a static image with ImageCtrl and overlaying drawn shapes
```cpp
#include <CtrlLib/CtrlLib.h>
#include <Draw/Draw.h>

using namespace Upp;

struct MapApp : public TopWindow {
    Image mapImage;

    void Paint(Draw& w) override {
        if (!mapImage)
            return;
        w.DrawImage(0, 0, mapImage);
        w.DrawRect(Rect(100, 100, 50, 50), SColorRed);
        w.DrawText(105, 105, "Target", Font().Bold(), SColorWhite);
    }

    MapApp() {
        mapImage = LoadImage("map.png");
        Sizeable().Zoomable();
    }
};

GUI_APP_MAIN
{
    MapApp().Run();
}
```

// # Code SNIPPET: Drawing an unfilled arc with Draw::Arc using radians and stroke width
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct ArcApp : public TopWindow {
    void Paint(Draw& w) override {
        Rect r = Rect(50, 50, 200, 200);
        w.Arc(r, 0, PI * 1.5, SColorBlue, 5);
    }
};

GUI_APP_MAIN
{
    ArcApp().Run();
}
```

// # Code SNIPPET: Drawing a filled pie slice with gradient brush and clipping
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct PieApp : public TopWindow {
    void Paint(Draw& w) override {
        Rect r = Rect(100, 100, 150, 150);
        w.PushClip(r);
        w.SetFill(Brush(SColorLightBlue, SColorDarkBlue, 1.0f));
        w.Pie(r, 0, PI / 2);
        w.PopClip();
    }
};

GUI_APP_MAIN
{
    PieApp().Run();
}
```

// # Code SNIPPET: Loading an image safely and displaying it scaled in ImageCtrl
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct ImageApp : public TopWindow {
    Image img;

    ImageApp() {
        img = LoadImage("photo.jpg");
        if (!img) {
            MessageBox("Failed to load image", "Error");
            return;
        }

        ImageCtrl ctrl;
        ctrl.SetImage(img.Scaled(300, 200));
        Add(ctrl.HSizePos().VSizePos());
        Sizeable().Zoomable();
    }
};

GUI_APP_MAIN
{
    ImageApp().Run();
}
```

// # Code SNIPPET: Layering images with alpha blending using DrawImage
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct OverlayApp : public TopWindow {
    Image bg, logo;

    void Paint(Draw& w) override {
        if (!bg || !logo) return;
        w.DrawImage(0, 0, bg);
        w.DrawImage(20, 20, logo, 0.7f);
    }

    OverlayApp() {
        bg = LoadImage("bg.png");
        logo = LoadImage("logo.png");
        Sizeable().Zoomable();
    }
};

GUI_APP_MAIN
{
    OverlayApp().Run();
}
```

// # Code SNIPPET: Freehand drawing canvas with mouse events and Draw::PolyLine for Scribble-style painting
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct Scribble : public Ctrl {
	Vector<Point> points;
	bool drawing;

	void Paint(Draw& w) override {
		if(points.GetCount() > 1)
			w.PolyLine(points, SColorBlack, 1);
	}

	void MouseMove(int x, int y, int z) override {
		if(drawing) {
			points.Add(Point(x, y));
			Refresh();
		}
	}

	void LeftDown(int x, int y, int z) override {
		drawing = true;
		points.Clear();
		points.Add(Point(x, y));
		Refresh();
	}

	void LeftUp(int x, int y, int z) override {
		drawing = false;
	}

	Scribble() {
		drawing = false;
	}
};

GUI_APP_MAIN
{
	Scribble scribble;
	TopWindow win;
	win.Add(scribble.SizePos());
	win.Sizeable().Zoomable();
	win.SetRect(0, 0, 800, 600);
	win.Run();
}
```

// # Code SNIPPET: Simple window with Timer-driven animation and dynamic text update
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct MyApp : public TopWindow {
	Label label;
	int count;

	void Timer() {
		count++;
		label.SetLabel(NFormat("Ticks: %", count));
	}

	MyApp() {
		SetRect(0, 0, 300, 100);
		Add(label.HSizePos().VSizePos());
		TimerSet(1000);
		count = 0;
	}
};

GUI_APP_MAIN
{
	MyApp().Run();
}
```

// # Code SNIPPET: Command-line word counter using FindFile, LoadFile, and string iteration over words
```cpp
#include <Core/Core.h>
#include <fstream>

using namespace Upp;

int main()
{
	String file = GetArg(1);
	if(file.IsEmpty()) {
		Print("Usage: wc <filename>\n");
		return 1;
	}

	String content = LoadFile(file);
	if(content.IsEmpty()) {
		Print("Error: Could not read file.\n");
		return 1;
	}

	int words = 0;
	int in_word = 0;

	for(int i = 0; i < content.GetLength(); i++) {
		char c = content[i];
		bool is_space = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
		if(is_space) {
			in_word = 0;
		} else if(!in_word) {
			in_word = 1;
			words++;
		}
	}

	Print("%d\n", words);
	return 0;
}
```

// # Code SNIPPET: Converting a Ctrl-based drawing to SVG output using Draw::Svg
```cpp
#include <CtrlLib/CtrlLib.h>
#include <Draw/Draw.h>

using namespace Upp;

struct ToSvgApp : public TopWindow {
	Image img;

	void Paint(Draw& w) override {
		w.DrawRect(GetSize(), SColorWhite);
		w.DrawText(50, 50, "Hello SVG!", Font().Bold(), SColorBlue);
		w.DrawCircle(100, 100, 40, SColorRed, 2);
		w.Arc(Rect(150, 150, 100, 100), 0, PI * 2, SColorGreen, 3);
	}

	void SaveAsSvg() {
		Svg svg;
		svg.Begin(GetSize().cx, GetSize().cy);
		Paint(svg);
		svg.End();
		SaveFile("output.svg", svg.ToString());
	}

	ToSvgApp() {
		Sizeable().Zoomable();
		SaveAsSvg();
	}
};

GUI_APP_MAIN
{
	ToSvgApp().Run();
}
```

// # Code SNIPPET: Simple rich text editor using RichEdit with basic formatting and file I/O
```cpp
#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>

using namespace Upp;

struct UWord : public TopWindow {
	RichEdit edit;

	void FileOpen() {
		FileSel fs;
		fs.AllFilesType();
		if(fs.ExecuteOpen()) {
			edit.Load(fs.Get());
		}
	}

	void FileSave() {
		FileSel fs;
		fs.AllFilesType();
		if(fs.ExecuteSave()) {
			edit.Save(fs.Get());
		}
	}

	UWord() {
		CtrlLayout(*this, "UWord");

		MenuBar menu;
		menu.Add("File", L"Open" <<= THISBACK(FileOpen) + "Open" + Key(F1) +
		                      "Save" <<= THISBACK(FileSave) + "Save" + Key(F2));
		SetMenuBar(menu);

		Add(edit.SizePos());

		edit.SetFont(Courier(12));
		edit.SetFrame(InsetFrame());
		Sizeable().Zoomable();
	}
};

GUI_APP_MAIN
{
	UWord().Run();
}
```

// # Code SNIPPET: Minimal U++ application with TopWindow and static text display using Label
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct MyApp : public TopWindow {
	Label label;

	MyApp() {
		label.SetLabel("Hello, U++!");
		Add(label.HSizePos().VSizePos());
		Sizeable().Zoomable();
	}
};

GUI_APP_MAIN
{
	MyApp().Run();
}
```

// # Code SNIPPET: U++ application with MenuBar, Button, and event binding using THISBACK for simple UI interaction
```cpp
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct MyApp : public TopWindow {
	Button btn;
	Label status;

	void OnClick() {
		status.SetLabel("Button clicked at " + Now().AsString());
	}

	MyApp() {
		SetRect(0, 0, 400, 200);

		MenuBar menu;
		menu.Add("File", "Exit" <<= Close);

		btn.SetLabel("Click Me");
		btn <<= THISBACK(OnClick);
		Add(btn.HSizePos().TopPos(50));

		status.SetLabel("Ready");
		Add(status.HSizePos().TopPos(100));

		SetMenuBar(menu);
		Sizeable().Zoomable();
	}
};

GUI_APP_MAIN
{
	MyApp().Run();
}
```

// # Code SNIPPET: Address book UI with TreeCtrl for contacts, EditString for fields, and data persistence via VectorMap
```cpp
#include <CtrlLib/CtrlLib.h>
#include <Draw/Draw.h>

using namespace Upp;

struct AddressBook : public TopWindow {
	TreeCtrl tree;
	EditString name, phone, email;
	Button add, del;

	VectorMap<String, String> contacts;

	void AddContact() {
		if(name.IsEmpty() || phone.IsEmpty()) return;
		String key = name;
		contacts.Add(key, "Phone: " + phone + "\nEmail: " + email);
		tree.Add(0, CtrlImg::Person(), key);
		name.Clear();
		phone.Clear();
		email.Clear();
	}

	void DelContact() {
		int node = tree.GetCursor();
		if(node < 0) return;
		String key = tree.GetText(node);
		contacts.Remove(key);
		tree.Del(node);
	}

	void DoTreeCursor() {
		int node = tree.GetCursor();
		if(node < 0) {
			name.Clear(); phone.Clear(); email.Clear();
			return;
		}
		String key = tree.GetText(node);
		String value = contacts[key];
		int p = value.Find("\n");
		name = key;
		phone = value.Left(p).After("Phone: ");
		email = value.After("\nEmail: ");
	}

	AddressBook() {
		CtrlLayout(*this, "Address Book");

		tree.WhenCursor = THISBACK(DoTreeCursor);
		add.SetLabel("Add");
		add <<= THISBACK(AddContact);
		del.SetLabel("Delete");
		del <<= THISBACK(DelContact);

		Splitter splitter;
		splitter.Vert(tree, VBox()
			<< HBox() << "Name:" << name.SizePos()
			<< HBox() << "Phone:" << phone.SizePos()
			<< HBox() << "Email:" << email.SizePos()
			<< HBox() << add << del
		);

		Add(splitter.SizePos());

		tree.Add(0, CtrlImg::Folder(), "Contacts");

		Sizeable().Zoomable();
	}
};

GUI_APP_MAIN
{
	AddressBook().Run();
}
```

// # Code SNIPPET: Custom StaticCtrl derived class that displays centered text with custom font and color
```cpp
#ifndef StaticCtrl_h
#define StaticCtrl_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class MyStaticCtrl : public StaticCtrl {
	String text;
	Color color;
	Font font;

public:
	MyStaticCtrl& SetText(const String& t) { text = t; Refresh(); return *this; }
	MyStaticCtrl& SetColor(Color c) { color = c; Refresh(); return *this; }
	MyStaticCtrl& SetFont(const Font& f) { font = f; Refresh(); return *this; }

	void Paint(Draw& w) override {
		if(text.IsEmpty()) return;
		Size sz = w.GetTextSize(text, font);
		Rect r = GetSize();
		w.DrawText((r.cx - sz.cx) / 2, (r.cy - sz.cy) / 2, text, font, color);
	}
};

#endif
```

// # Code SNIPPET: Custom TextEdit control with auto-scroll and line number support using ScrollBar and custom Paint
```cpp
#ifndef TextEdit_h
#define TextEdit_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class TextEdit : public Ctrl {
	Vector<String> lines;
	ScrollBar vsb;
	int cursor_line, cursor_pos;
	bool modified;

	void UpdateVsb() {
		vsb.SetRange(0, max(0, lines.GetCount() - 1));
		vsb.SetPos(cursor_line);
	}

	void Paint(Draw& w) override {
		Size sz = GetSize();
		int y = 0;
		int first = vsb.GetPos();
		int last = min(first + sz.cy / 16, lines.GetCount());

		for(int i = first; i < last; i++) {
			String line = lines[i];
			if(i == cursor_line) {
				w.DrawRect(0, y, sz.cx, 16, SColorLightBlue);
				w.DrawText(2, y, line.Left(cursor_pos), Courier(12), SColorBlack);
				w.DrawText(2 + w.GetTextSize(line.Left(cursor_pos), Courier(12)).cx, y, line.Mid(cursor_pos), Courier(12), SColorRed);
			} else {
				w.DrawText(2, y, line, Courier(12), SColorBlack);
			}
			y += 16;
		}
	}

	void MouseMove(int x, int y, int z) override {
		cursor_line = (y / 16) + vsb.GetPos();
		cursor_pos = 0;
		Refresh();
	}

	void KeyDown(int key, int repcnt, int flags) override {
		if(key == K_UP) {
			if(cursor_line > 0) cursor_line--;
			UpdateVsb();
			Refresh();
		} else if(key == K_DOWN) {
			if(cursor_line < lines.GetCount() - 1) cursor_line++;
			UpdateVsb();
			Refresh();
		} else if(key == K_LEFT) {
			if(cursor_pos > 0) cursor_pos--;
			Refresh();
		} else if(key == K_RIGHT) {
			if(cursor_line < lines.GetCount() && cursor_pos < lines[cursor_line].GetLength()) cursor_pos++;
			Refresh();
		} else if(key == '\n') {
			String before = lines[cursor_line].Left(cursor_pos);
			String after = lines[cursor_line].Mid(cursor_pos);
			lines.Insert(cursor_line + 1, after);
			lines[cursor_line] = before;
			cursor_line++;
			cursor_pos = 0;
			UpdateVsb();
			Refresh();
		}
	}

public:
	TextEdit() : cursor_line(0), cursor_pos(0), modified(false) {
		vsb.VSizePos();
		Add(vsb);
		WhenMouseWheel = [=](int delta) {
			vsb.SetPos(max(0, min(vsb.GetPos() - delta, vsb.GetMax())));
			Refresh();
		};
	}

	TextEdit& SetText(const Vector<String>& l) { lines = l; cursor_line = 0; cursor_pos = 0; UpdateVsb(); Refresh(); return *this; }
	Vector<String>& GetText() { return lines; }
};

#endif
```

// # Code SNIPPET: Custom PushCtrl with animated press state and custom drawing using Draw::Rect and color transitions
```cpp
#ifndef PushCtrl_h
#define PushCtrl_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class PushCtrl : public Ctrl {
	bool pressed;
	Color bg_normal, bg_pressed, border_color;
	String text;
	Font font;

public:
	PushCtrl& SetText(const String& t) { text = t; Refresh(); return *this; }
	PushCtrl& SetColors(Color normal, Color pressed, Color border) {
		bg_normal = normal; bg_pressed = pressed; border_color = border;
		Refresh();
		return *this;
	}
	PushCtrl& SetFont(const Font& f) { font = f; Refresh(); return *this; }

	void Paint(Draw& w) override {
		Rect r = GetSize();
		Color bg = pressed ? bg_pressed : bg_normal;
		w.DrawRect(r, bg);
		w.DrawRect(r, border_color, 1);
		if(!text.IsEmpty()) {
			Size sz = w.GetTextSize(text, font);
			w.DrawText((r.cx - sz.cx) / 2, (r.cy - sz.cy) / 2, text, font, SColorBlack);
		}
	}

	void LeftDown(int x, int y, int z) override {
		pressed = true;
		Refresh();
	}

	void LeftUp(int x, int y, int z) override {
		pressed = false;
		Refresh();
		PostCallback([=] { Action(); });
	}

	virtual void Action() {}

	PushCtrl() : pressed(false), bg_normal(SColorLightGray), bg_pressed(SColorDarkGray), border_color(SColorBlack), font(Courier(12)) {}
};

#endif
```

// # Code SNIPPET: HeaderCtrl implementation with resizable columns, click sorting, and custom painter for header cells
```cpp
#ifndef HeaderCtrl_h
#define HeaderCtrl_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class HeaderCtrl : public Ctrl {
	Vector<String> labels;
	Vector<int> widths;
	int hover_col, pressed_col;
	bool sorted_asc;
	int sort_col;

	void Paint(Draw& w) override {
		int x = 0;
		for(int i = 0; i < labels.GetCount(); i++) {
			Rect r(x, 0, widths[i], GetSize().cy);
			Color bg = (i == pressed_col) ? SColorDarkGray : (i == hover_col ? SColorLightGray : SColorWhite);
			w.DrawRect(r, bg);
			w.DrawRect(r, SColorBlack, 1);

			Size sz = w.GetTextSize(labels[i], Font());
			int tx = x + (widths[i] - sz.cx) / 2;
			int ty = (GetSize().cy - sz.cy) / 2;
			w.DrawText(tx, ty, labels[i], Font(), SColorBlack);

			if(i == sort_col) {
				String arrow = sorted_asc ? "▲" : "▼";
				w.DrawText(tx + sz.cx + 4, ty, arrow, Font(), SColorBlack);
			}

			x += widths[i];
		}
	}

	void MouseMove(int x, int y, int z) override {
		int col = GetColumnAt(x);
		if(col != hover_col) {
			hover_col = col;
			Refresh();
		}
	}

	void LeftDown(int x, int y, int z) override {
		int col = GetColumnAt(x);
		if(col >= 0) {
			pressed_col = col;
			Refresh();
		}
	}

	void LeftUp(int x, int y, int z) override {
		if(pressed_col >= 0) {
			sort_col = pressed_col;
			sorted_asc = !sorted_asc;
			pressed_col = -1;
			Action(sort_col);
			Refresh();
		}
	}

	int GetColumnAt(int x) const {
		int sum = 0;
		for(int i = 0; i < widths.GetCount(); i++) {
			if(x >= sum && x < sum + widths[i])
				return i;
			sum += widths[i];
		}
		return -1;
	}

public:
	HeaderCtrl& AddColumn(const String& label, int width) {
		labels.Add(label);
		widths.Add(width);
		Refresh();
		return *this;
	}

	virtual void Action(int col) {}

	HeaderCtrl() : hover_col(-1), pressed_col(-1), sort_col(-1), sorted_asc(true) {}
};

#endif
```

// # Code SNIPPET: DropChoice control with popup list, mouse selection, and dynamic sizing based on content
```cpp
#ifndef DropChoice_h
#define DropChoice_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class DropChoice : public Ctrl {
	Vector<String> items;
	String selected;
	int hover_item;
	bool dropdown_open;
	Timer timer;

	void Paint(Draw& w) override {
		Size sz = GetSize();
		w.DrawRect(sz, SColorWhite);
		w.DrawRect(sz, SColorBlack, 1);
		w.DrawText(4, (sz.cy - 16) / 2, selected.IsEmpty() ? "(select)" : selected, Font(), SColorBlack);
		w.DrawText(sz.cx - 16, (sz.cy - 16) / 2, "▼", Font(), SColorBlack);
	}

	void LeftDown(int x, int y, int z) override {
		if(x > GetSize().cx - 16) {
			dropdown_open = !dropdown_open;
			if(dropdown_open)
				ShowPopup();
			else
				HidePopup();
		} else if(dropdown_open) {
			int item = y / 16;
			if(item >= 0 && item < items.GetCount()) {
				selected = items[item];
				dropdown_open = false;
				HidePopup();
				Action();
			}
		}
		Refresh();
	}

	void ShowPopup() {
		Size sz = GetSize();
		TopWindow popup;
		popup.SetRect(GetScreenRect().LeftTop() + Point(GetRect().left, GetRect().bottom), Size(sz.cx, items.GetCount() * 16 + 4));
		VBox vbox;
		for(int i = 0; i < items.GetCount(); i++) {
			Label lbl;
			lbl.SetLabel(items[i]);
			vbox << lbl.HSizePos().VSizePos(16);
		}
		popup.Add(vbox.SizePos());
		popup.SetFrame(InsetFrame());
		popup.SetBgColor(SColorWhite);
		popup.Run();
	}

	void HidePopup() {
		dropdown_open = false;
	}

public:
	DropChoice& AddItem(const String& item) {
		items.Add(item);
		if(selected.IsEmpty()) selected = item;
		Refresh();
		return *this;
	}

	DropChoice& SetSelected(const String& s) {
		selected = s;
		Refresh();
		return *this;
	}

	String GetSelected() const { return selected; }

	virtual void Action() {}

	DropChoice() : hover_item(-1), dropdown_open(false) {}
};

#endif
```

// # Code SNIPPET: DisplayPopup utility class for transient tooltip-style popups with auto-hide and positioning
```cpp
#ifndef DisplayPopup_h
#define DisplayPopup_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class DisplayPopup : public TopWindow {
	Label label;
	Timer timer;

	void Timer() override {
		Close();
	}

public:
	DisplayPopup& SetText(const String& txt) {
		label.SetLabel(txt);
		return *this;
	}

	DisplayPopup& SetTimeout(int ms) {
		timer.Set(ms);
		return *this;
	}

	DisplayPopup& SetPosition(Point p) {
		SetRect(p, Size(200, 40));
		return *this;
	}

	DisplayPopup() {
		SetFrame(InsetFrame());
		Add(label.HSizePos().VSizePos());
		SetBgColor(SColorYellow);
		SetBorder(1, SColorBlack);
		Sizeable().Zoomable(false);
		timer.Set(3000);
	}

	void Show() {
		Show();
		timer.Start();
	}
};

#endif
```

// # Code SNIPPET: Advanced key handling system with global accelerator keys and context-aware key filtering in Ctrl
```cpp
#ifndef AKeys_h
#define AKeys_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class AKeys : public Ctrl {
	Map<int, Function<void()>> accelerators;
	Ctrl* target;

	void KeyDown(int key, int repcnt, int flags) override {
		int mod = flags & (K_CTRL | K_ALT | K_SHIFT);
		int combo = key | mod;

		if(accelerators.Find(combo)) {
			accelerators[combo]();
			return;
		}

		if(target && target->IsFocused())
			target->KeyDown(key, repcnt, flags);
	}

public:
	AKeys& AddAccelerator(int key, Function<void()> action) {
		accelerators.Set(key, action);
		return *this;
	}

	AKeys& SetTarget(Ctrl* ctrl) {
		target = ctrl;
		return *this;
	}

	AKeys() : target(nullptr) {}
};

#endif
```

// # Code SNIPPET: Display class for managing screen resolution, fullscreen toggle, and display mode switching via system APIs
```cpp
#ifndef Display_h
#define Display_h

#include <Core/Core.h>

using namespace Upp;

class Display {
	int width, height, bpp;
	bool fullscreen;
	static Display instance;

public:
	static Display& Get() { return instance; }

	void SetMode(int w, int h, int b = 32) {
		width = w; height = h; bpp = b;
		// Platform-specific implementation would go here (Win32/OSX/Linux)
	}

	void ToggleFullscreen() {
		fullscreen = !fullscreen;
		// System call to enter/exit fullscreen mode
	}

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetBPP() const { return bpp; }
	bool IsFullscreen() const { return fullscreen; }

	Display() : width(800), height(600), bpp(32), fullscreen(false) {}
};

Display Display::instance;

#endif
```

// # Code SNIPPET: Raster class for low-level pixel buffer manipulation with direct memory access and color format conversion
```cpp
#ifndef Raster_h
#define Raster_h

#include <Core/Core.h>

using namespace Upp;

class Raster {
	byte* data;
	int width, height, stride;
	ColorFormat format;

public:
	Raster(int w, int h, ColorFormat f = RGBA) : width(w), height(h), format(f) {
		stride = (w * GetBytesPerPixel(f) + 3) & ~3;
		data = new byte[stride * h];
		memset(data, 0, stride * h);
	}

	~Raster() { delete[] data; }

	byte* GetLine(int y) { return data + y * stride; }

	void SetPixel(int x, int y, Color c) {
		if(x < 0 || x >= width || y < 0 || y >= height) return;
		byte* p = GetLine(y) + x * GetBytesPerPixel(format);
		switch(format) {
			case RGBA:
				*(int*)p = c;
				break;
			case RGB:
				p[0] = GetRValue(c); p[1] = GetGValue(c); p[2] = GetBValue(c);
				break;
			case Gray:
				p[0] = GetRValue(c);
				break;
		}
	}

	Color GetPixel(int x, int y) {
		if(x < 0 || x >= width || y < 0 || y >= height) return SColorBlack;
		byte* p = GetLine(y) + x * GetBytesPerPixel(format);
		switch(format) {
			case RGBA: return *(int*)p;
			case RGB: return RGB(p[0], p[1], p[2]);
			case Gray: return RGB(p[0], p[0], p[0]);
			default: return SColorBlack;
		}
	}

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	Raster& Clear(Color c = SColorWhite) {
		int n = stride * height;
		for(int i = 0; i < n; i++) data[i] = (byte)c;
		return *this;
	}
};

#endif
```

// # Code SNIPPET: FontInt internal font rendering engine with glyph caching, kerning, and bitmap-based text layout
```cpp
#ifndef FontInt_h
#define FontInt_h

#include <Draw/Draw.h>
#include <Core/Core.h>

using namespace Upp;

class FontInt {
	struct Glyph {
		int width, height, xoff, yoff, advance;
		byte* bitmap;
		Glyph() : bitmap(nullptr) {}
		~Glyph() { delete[] bitmap; }
	};

	Map<String, Glyph> glyphs;
	Font font;
	int size;
	int line_height;

public:
	FontInt(const Font& f) : font(f), size(f.GetSize()), line_height(size * 14 / 10) {}

	int GetTextSize(const String& text) const {
		int w = 0;
		for(int i = 0; i < text.GetLength(); i++) {
			char c = text[i];
			if(glyphs.Find(c)) {
				w += glyphs[c].advance;
			} else {
				// Simulate glyph generation (real version rasterizes TrueType)
				w += size * 5 / 4;
			}
		}
		return w;
	}

	void Draw(Draw& w, int x, int y, const String& text, Color c) {
		for(int i = 0; i < text.GetLength(); i++) {
			char cchar = text[i];
			if(!glyphs.Find(cchar)) {
				// Generate glyph on-demand (simplified)
				Glyph& g = glyphs[cchar];
				g.width = size * 3 / 4;
				g.height = size;
				g.xoff = 0;
				g.yoff = 0;
				g.advance = size * 5 / 4;
				g.bitmap = new byte[g.width * g.height];
				memset(g.bitmap, 255, g.width * g.height);
			}
			const Glyph& g = glyphs[cchar];
			for(int py = 0; py < g.height; py++) {
				for(int px = 0; px < g.width; px++) {
					if(g.bitmap[py * g.width + px]) {
						int dx = x + g.xoff + px;
						int dy = y + g.yoff + py;
						if(dx >= 0 && dx < 10000 && dy >= 0 && dy < 10000)
							w.DrawPoint(dx, dy, c);
					}
				}
			}
			x += g.advance;
		}
	}

	int GetLineHeight() const { return line_height; }
};

#endif
```

// # Code SNIPPET: GLDraw wrapper for OpenGL rendering within U++ Ctrl, integrating context setup, vertex buffers, and shader-based drawing
```cpp
#ifndef GLDraw_h
#define GLDraw_h

#include <CtrlLib/CtrlLib.h>
#include <GL/gl.h>

using namespace Upp;

class GLDraw : public Ctrl {
	GLuint vbo, vao;
	bool initialized;

	void InitGL() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		initialized = true;
	}

	void Paint(Draw& w) override {
		if(!initialized) InitGL();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(vao);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);
		glBindVertexArray(0);

		SwapBuffers();
	}

public:
	GLDraw() : initialized(false) {}
};

#endif
```

// # Code SNIPPET: Report class for generating structured tabular output with headers, alignment, and multi-page pagination
```cpp
#ifndef Report_h
#define Report_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class Report {
	Vector<String> headers;
	Vector<Vector<String>> rows;
	int page_width, page_height;
	String title;

public:
	Report& SetTitle(const String& t) { title = t; return *this; }
	Report& AddHeader(const String& h) { headers.Add(h); return *this; }
	Report& AddRow(const Vector<String>& r) { rows.Add(r); return *this; }

	void Print(Draw& w, int x, int y, int max_width) {
		int col_count = headers.GetCount();
		if(col_count == 0) return;

		int col_width = max_width / col_count;

		// Title
		w.DrawText(x, y, title, Font().Bold(), SColorBlack);
		y += 20;

		// Headers
		w.DrawRect(Rect(x, y, max_width, 20), SColorLightGray);
		for(int i = 0; i < col_count; i++) {
			int tx = x + i * col_width;
			w.DrawText(tx + 4, y + 4, headers[i], Font().Bold(), SColorBlack);
		}
		y += 20;

		// Rows
		for(int i = 0; i < rows.GetCount(); i++) {
			const Vector<String>& row = rows[i];
			if(i % 40 == 0 && i > 0) { // Pagination
				y = 20;
				w.DrawText(x, y, "Page break...", Font().Italic(), SColorGray);
				y += 20;
			}

			for(int j = 0; j < col_count; j++) {
				int tx = x + j * col_width;
				String cell = j < row.GetCount() ? row[j] : "";
				w.DrawText(tx + 4, y, cell, Font(), SColorBlack);
			}
			y += 16;
		}
	}

	Report() : page_width(600), page_height(800) {}
};

#endif
```

