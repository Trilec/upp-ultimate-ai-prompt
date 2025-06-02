#include <Core/Core.h>
#include <Draw/Draw.h> // For Color
#include <CtrlCore/CtrlCore.h> // For Display, Value

using namespace Upp;

// Define a simple custom struct
struct MyColorValue {
    Color colorVal;
    String name;

    // Default constructor
    MyColorValue() : colorVal(Black()) {}

    // Constructor
    MyColorValue(Color c, String n) : colorVal(c), name(n) {}

    // For DUMP/LOG
    String ToString() const {
        return Sprintf("%s (%s)", name, colorVal.ToString());
    }
};

// Create a Convert class for MyColorValue
class MyColorValueConvert : public Convert {
public:
    virtual Value Format(const Value& q) const {
        if (q.Is<MyColorValue>()) {
            const MyColorValue& mcv = q.Get<MyColorValue>();
            return Sprintf("%s (%s)", mcv.name, mcv.colorVal.ToString());
        }
        return q; // Or ErrorValue() if strict conversion is needed
    }

    virtual Value Scan(const Value& q) const {
        if (q.IsString()) {
            String s = q.To<String>();
            // This is a simplified parser. A real one would be more robust.
            // Example format: "Red (#FF0000)"
            int obracket = s.Find('(');
            int cbracket = s.Find(')');
            if (obracket != -1 && cbracket != -1 && cbracket > obracket + 1) {
                String namePart = TrimBoth(s.Left(obracket));
                String colorPart = TrimBoth(s.Mid(obracket + 1, cbracket - obracket - 1));
                
                Color c;
                if (c.Scan(colorPart)) { // Color::Scan can parse "#RRGGBB" and color names
                    return RawToValue(MyColorValue(c, namePart));
                }
            }
        }
        return ErrorValue(); // Indicate conversion failure
    }

    // Optional: Filter for UI controls, e.g. to ensure input is valid
    virtual Value Filter(const Value& q) const {
        // For this example, Format is sufficient for display, 
        // and Scan handles conversion from string.
        // A filter could, for instance, ensure that if a MyColorValue is directly edited
        // in a control, it remains valid.
        return q;
    }
};

// Custom Display for MyColorValue
class MyColorValueDisplay : public Display {
public:
    virtual void Paint(Draw& w, const Rect& r, const Value& q,
                       Color ink, Color paper, dword style) const {
        if (q.Is<MyColorValue>()) {
            const MyColorValue& mcv = q.Get<MyColorValue>();
            
            // Simple display: a colored rectangle and the name
            Rect colorBox = r;
            colorBox.right = r.left + r.GetHeight(); // Make a square box for the color
            
            w.DrawRect(colorBox, mcv.colorVal); // Draw the color box
            
            Rect textRect = r;
            textRect.left = colorBox.right + 4; // Add some padding
            
            // Draw the text (name)
            DrawTLText(w, textRect.TopLeft(), GetStdFont(), mcv.name, ink);
        } else {
            // Fallback for other types or if q is not MyColorValue
            StdDisplay().Paint(w, r, q, ink, paper, style);
        }
    }
};

// Example Usage (Console and a simple GUI to show Display)
// For a GUI example, we'd need a TopWindow and a Ctrl that uses the display.
// Let's make a simple Label-like control that uses MyColorValueDisplay.

class MyColorValueCtrl : public Ctrl {
public:
    MyColorValue data;

    virtual void Paint(Draw& w) {
        // Use the registered display for MyColorValue
        // Or directly instantiate MyColorValueDisplay if not registered globally
        // For this example, let's assume it can be fetched or we use a local one.
        MyColorValueDisplay display; 
        display.Paint(w, GetSize(), RawToValue(data), SColorText, SColorPaper, 0);
    }

    voidSetData(const MyColorValue& mcv) {
        data = mcv;
        Refresh(); // Trigger a repaint
    }

    MyColorValueCtrl() {
        // Set initial data if needed
        data = MyColorValue(Cyan(), "Default");
    }
};


// GUI_APP_MAIN for a quick test of the Display
struct MyDisplayTestWindow : TopWindow {
    MyColorValueCtrl myCtrl;
    EditString editField; // To test Convert Scan
    Button convertButton;
    Label resultLabel;

    MyDisplayTestWindow() {
        Title("MyColorValue Display & Convert Test");
        SetRect(0, 0, 300, 200);

        // Register MyColorValue and its Convert and Display
        // Note: VALUE_MYCOLORVALUE should be unique if used in a larger app context
        // For a simple example, we define it here.
        #define VALUE_MYCOLORVALUE (Value::USER + 1)
        RegisterValueType<MyColorValue>(VALUE_MYCOLORVALUE, new MyColorValueConvert, "MyColorValue", new MyColorValueDisplay);


        myCtrl.SetData(MyColorValue(Magenta(), "Magenta Sample"));
        Add(myCtrl.TopPos(10, 50).LeftPos(10, 280));
        
        editField.SetText("Orange (#FFA500)");
        Add(editField.TopPos(70, 25).HorzPos(10, 10));

        convertButton.SetLabel("Convert String to MyColorValue");
        convertButton << [=] {
            Value scannedVal = Single<MyColorValueConvert>().Scan(editField.GetData());
            if(scannedVal.Is<MyColorValue>()) {
                myCtrl.SetData(scannedVal.Get<MyColorValue>());
                resultLabel.SetLabel("Conversion successful!");
            } else {
                resultLabel.SetLabel("Conversion failed. Try 'Name (ColorSpec)' e.g. 'Lime (#00FF00)'");
            }
        };
        Add(convertButton.TopPos(100, 25).HorzPos(10,10));
        Add(resultLabel.TopPos(130, 20).HorzPos(10,10));
    }
};

GUI_APP_MAIN {
    StdLogSetup(LOG_COUT|LOG_FILE);

    // Console part of the example (from before)
    // Register the MyColorValue type and its Convert class instance
    // VALUE_MYCOLORVALUE is defined in MyDisplayTestWindow for this combined example
    // If running only console, define it here or ensure it's defined.
    // For GUI_APP_MAIN, registration happens in MyDisplayTestWindow constructor.
    // If we were in CONSOLE_APP_MAIN, we would need to uncomment and define VALUE_MYCOLORVALUE here.
    // #define VALUE_MYCOLORVALUE_CONSOLE (Value::USER + 2) // Use a different ID if needed
    // RegisterValueType<MyColorValue>(VALUE_MYCOLORVALUE_CONSOLE, new MyColorValueConvert, "MyColorValueConsole");


    MyColorValue red(Red(), "Red");
    MyColorValue blue(Blue(), "Blue");

    DUMP(red);
    DUMP(blue);

    // Convert MyColorValue to Value
    Value vRed = RawToValue(red);
    DUMP(vRed); // Will use Format to display as String (if registered via ValueType) or ToString()

    // Convert Value (String) back to MyColorValue
    // This part is now better tested via the GUI example's EditString
    Value strValueConsole = "ConsoleGreen (#00FF00)";
    // Assuming VALUE_MYCOLORVALUE_CONSOLE was registered for a console-only context
    // For the combined example, we rely on the registration in MyDisplayTestWindow
    // or we can use the Convert instance directly if no global registration is active for console.
    Value convertedGreenValueConsole = Single<MyColorValueConvert>().Scan(strValueConsole);


    if (convertedGreenValueConsole.Is<MyColorValue>()) {
        MyColorValue green = convertedGreenValueConsole.Get<MyColorValue>();
        DUMP(green);
    } else {
        LOG("Failed to convert string 'ConsoleGreen (#00FF00)' to MyColorValue in console part.");
    }
    
    Value invalidStrValueConsole = "Invalid Format Console";
    Value convertedInvalidConsole = Single<MyColorValueConvert>().Scan(invalidStrValueConsole);
    if(convertedInvalidConsole.IsError()) {
        LOG("Conversion of invalid string (console) failed as expected.");
    }

    // Example of ValueMap usage with custom type
    ValueMap vm;
    // For this to work as expected with DUMP, MyColorValue needs its ValueType registered
    // or DUMP will just show it as an opaque Value.
    vm.Add("primaryColor", RawToValue(MyColorValue(Magenta(), "PrimaryInMap")));
    DUMP(vm); // Output depends on MyColorValue's ValueType registration and its Format method

    Value extractedColorValue = vm["primaryColor"];
    if(extractedColorValue.Is<MyColorValue>()) {
        MyColorValue extractedColor = extractedColorValue.Get<MyColorValue>();
        LOG("Extracted from ValueMap: " << extractedColor.name << " - " << extractedColor.colorVal);
    }

    // Run the GUI Test Window
    MyDisplayTestWindow().Run();
}
