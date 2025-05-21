#include <CtrlLib/CtrlLib.h>

using namespace Upp;

// Define the main window for the MenuBar example
class MyMenuBarWindow : public TopWindow {
public:
    typedef MyMenuBarWindow CLASSNAME;

    MenuBar menu;

    void ExitProgram() {
        Close(); // Close the main window, which will end the application loop
    }

    void SubMenuCb() {
        PromptOK("Submenu item clicked!");
    }

    MyMenuBarWindow() {
        Title("MenuBar Example");
        SetRect(0, 0, 300, 200);

        // Setup the menu
        menu.Set([=](Bar& bar) {
            bar.Add("File", [=](Bar& subBar) {
                subBar.Add("Sub Item 1", [=] { SubMenuCb(); });
                subBar.Add("Exit", [=] { ExitProgram(); }).Help("Exits the application");
            });
            bar.Add("Help", [=](Bar& subBar) {
                subBar.Add("About", [=] { PromptOK("A simple MenuBar Example!"); });
            });
        });

        // Add the MenuBar to the TopWindow.
        // MenuBars are typically added at the top.
        AddFrame(menu); // Add menu to the frame, usually at the top.

        // Add some content to the window (optional)
        Label label("Check the menu bar at the top!");
        label.AlignCenter();
        Add(label.VCenter().SizePos());
    }
};

// Main function for the GUI application
GUI_APP_MAIN {
    MyMenuBarWindow().Run();
}
