#include <CtrlLib/CtrlLib.h>

using namespace Upp;

// Define the layout in a .lay file (conceptually)
// For this example, we'll define the layout structure directly in code
// as if it were loaded from a .lay file.

// This simulates what U++ generates from a .lay file:
// LAYOUTFILE(MyDialogLayoutFile, MyDialogLayout)
//  ITEM(Label, title, SetLabel(t_("My Dialog Title")).HCenterPos(150).TopPos(10))
//  ITEM(EditString, nameEdit, TopPos(40).LeftPos(10).RightPos(10))
//  ITEM(Option, checkOption, SetLabel(t_("Enable feature")).TopPos(70).LeftPos(10))
//  ITEM(Button, okButton, SetLabel(t_("OK")).Ok().BottomPos(10).RightPos(90))
//  ITEM(Button, cancelButton, SetLabel(t_("Cancel")).Cancel().BottomPos(10).RightPos(10))
// END_LAYOUT

// We will define the controls manually for this example,
// but use the With<> template pattern.

// Template for the dialog layout
template <class T>
struct WithMyDialogLayout : public T {
    Label title;
    EditString nameEdit;
    Option checkOption;
    Button okButton, cancelButton;

    typedef WithMyDialogLayout CLASSNAME; // Required for U++ RTTI if T is a Ctrl based class

    WithMyDialogLayout() {
        // Setup controls (normally done by CtrlLayout)
        this->Add(title.SetLabel("My Dialog Title").HCenterPos(150).TopPos(10));
        this->Add(nameEdit.TopPos(40).LeftPos(10).RightPos(10));
        this->Add(checkOption.SetLabel("Enable feature").TopPos(70).LeftPos(10));
        
        this->Add(okButton.SetLabel("OK").Ok().BottomPos(10).RightPos(90));
        this->Add(cancelButton.SetLabel("Cancel").Cancel().BottomPos(10).RightPos(10));

        // For TopWindow based dialogs, setting title and size might be here or in derived class
        this->Title("Dialog From Template");
        this->SetRect(0,0, 300, 150); // Default size
    }
};

// Actual dialog implementation using the layout template
struct MyActualDialog : WithMyDialogLayout<TopWindow> {
    typedef MyActualDialog CLASSNAME;

    MyActualDialog() {
        // CtrlLayout(*this, "My Dialog Title"); // This would load from .lay if we had one
        // If not using a .lay file, the WithMyDialogLayout constructor already added controls.

        // Initialize control values or connect callbacks
        nameEdit.SetData("Default Name");
        checkOption.Set(true);

        okButton << [=] {
            String name = nameEdit.GetData();
            bool featureEnabled = checkOption.Get();
            PromptOK(Format("Name: %s\nFeature Enabled: %s", name, featureEnabled ? "Yes" : "No"));
            Accept(); // Or Close(); depending on desired behavior for OK
        };
        // cancelButton behavior is often handled by .Cancel() setting (rejects dialog)
        // or custom callback: cancelButton << Rejector(IDOK); or cancelButton << [=] { Reject(); };
    }
};

// Main function for the GUI application
GUI_APP_MAIN {
    // To use:
    MyActualDialog().Run(); 
    // If Run returns IDOK (e.g. from okButton.Ok() or Accept()), it means user pressed OK.
    // MyActualDialog dlg;
    // if(dlg.Run() == IDOK) {
    //    LOG("Dialog accepted. Name: " << dlg.nameEdit.GetData());
    // } else {
    //    LOG("Dialog cancelled or closed.");
    // }
}
