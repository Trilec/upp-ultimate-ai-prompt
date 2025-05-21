#include <CtrlLib/CtrlLib.h>

using namespace Upp;

// Define the main window for the TreeCtrl example
class MyTreeCtrlWindow : public TopWindow {
public:
    typedef MyTreeCtrlWindow CLASSNAME;

    TreeCtrl tree;

    MyTreeCtrlWindow() {
        Title("TreeCtrl Example");
        SetRect(0, 0, 300, 300);

        // Add a root node
        // Add(parent_id, image, value)
        // parent_id 0 is the virtual root.
        // We can get the ID of the added node to add children to it.
        int rootNodeId = tree.Add(0, Null, "Root Node");

        // Add a child node to the rootNodeId
        tree.Add(rootNodeId, Null, "Child Node 1");
        tree.Add(rootNodeId, Null, "Child Node 2");

        // Add a grandchild
        int childNode2Id = tree.Add(rootNodeId, Null, "Child Node 3 with children");
        tree.Add(childNode2Id, Null, "Grandchild 3.1");
        tree.Add(childNode2Id, Null, "Grandchild 3.2");
        
        // Open the root node to show its children by default
        tree.Open(rootNodeId);
        tree.Open(childNode2Id);

        Add(tree.SizePos());
    }
};

// Main function for the GUI application
GUI_APP_MAIN {
    MyTreeCtrlWindow().Run();
}
