#include <CtrlLib/CtrlLib.h>

using namespace Upp;

// Define the main window for the ColorButton example
class MyColorButtonWindow : public TopWindow {
public:
    typedef MyColorButtonWindow CLASSNAME;

    ColorButton colorBtn;
    Label selectedColorLabel;

    void ColorChanged() {
        Color selected = colorBtn.GetData(); // Or colorBtn.Get()
        selectedColorLabel.SetLabel(Format("Selected Color: %s", selected.ToString()));
        // Log() could also be used here for debugging/demonstration
        LOG("Color changed: " << selected.ToString());
    }

    MyColorButtonWindow() {
        Title("ColorButton Example");
        SetRect(0, 0, 250, 100);

        // Initialize ColorButton
        colorBtn.SetColor(Blue()); // Set an initial color
        colorBtn.WhenAction = THISBACK(ColorChanged); // Callback for when color changes

        selectedColorLabel.SetLabel(Format("Selected Color: %s", colorBtn.GetData().ToString()));
        selectedColorLabel.AlignCenter();

        // Layout
        // A simple vertical layout
        Add(colorBtn.HCenterPos(100).TopPos(10, 30));
        Add(selectedColorLabel.HCenterPos(200).TopPos(50, 20));
    }
};

// Main function for the GUI application
GUI_APP_MAIN {
    StdLogSetup(LOG_COUT|LOG_FILE); // To see LOG output
    MyColorButtonWindow().Run();
}
