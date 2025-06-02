#include <CtrlLib/CtrlLib.h>

using namespace Upp;

// Define the main window for the ArrayCtrl example
class MyArrayCtrlWindow : public TopWindow {
public:
    typedef MyArrayCtrlWindow CLASSNAME; // Required for U++ RTTI

    ArrayCtrl dataList;

    MyArrayCtrlWindow() {
        Title("ArrayCtrl Example");
        SetRect(0, 0, 400, 200); // Set initial size

        // Add columns to the ArrayCtrl
        dataList.AddColumn("ID", 50);      // Column title "ID", width 50
        dataList.AddColumn("Name", 150).HeaderTab().AlignCenter(); // Column "Name", width 150, centered header

        // Add some data rows
        // Method 1: Using Add(Value, Value, ...)
        dataList.Add(1, "Alice");
        dataList.Add(2, "Bob");
        dataList.Add(3, "Charlie");

        // Method 2: Setting row by row, column by column
        dataList.Add(); // Add a new empty row
        int lastRowIndex = dataList.GetCount() - 1;
        dataList.Set(lastRowIndex, 0, 4);          // Row index, Column index, Value
        dataList.Set(lastRowIndex, "Name", "David"); // Row index, Column ID (if AddColumn used ID), Value
                                                 // Note: Using column name "Name" for the second column
                                                 // is possible if we had used dataList.Column("Name").Id("NAME_ID");
                                                 // For simplicity, we'll use column index for the second Set call here
                                                 // or rely on the order. Let's assume we want to set the "Name" column.
                                                 // If AddColumn was just AddColumn("Name", ...), the "id" is the same as title.

        // Correctly setting "Name" for the 4th row using its title as ID (default behavior)
        dataList.Set(lastRowIndex, "Name", "David");


        // Add the ArrayCtrl to the window's layout
        Add(dataList.SizePos()); // Make ArrayCtrl fill the window

        // Optional: Add some more features
        dataList.MultiSelect(); // Allow multiple row selection
        dataList.SetLineCy(20); // Set row height
    }
};

// Main function for the GUI application
GUI_APP_MAIN {
    MyArrayCtrlWindow().Run();
}
