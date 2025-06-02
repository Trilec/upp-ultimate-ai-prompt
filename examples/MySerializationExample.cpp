#include <Core/Core.h>

using namespace Upp;

// Define a simple struct for serialization
struct MySerializableObject {
    String name;
    int    value;

    // Default constructor needed for LoadFromString
    MySerializableObject() : value(0) {} 

    MySerializableObject(String n, int v) : name(n), value(v) {}

    // Serialization function
    void Serialize(Stream& s) {
        s % name % value; // U++ serialization syntax
    }

    // For DUMP/LOG
    String ToString() const {
        return Sprintf("Name: %s, Value: %d", name, value);
    }
};

CONSOLE_APP_MAIN {
    StdLogSetup(LOG_COUT|LOG_FILE);

    // Create an instance of the object
    MySerializableObject originalObject("TestObject", 123);
    DUMP(originalObject);

    // Serialize the object to a String
    String serializedData = StoreAsString(originalObject);
    LOG("Serialized data: " << serializedData);

    // Deserialize the object from the String
    MySerializableObject deserializedObject;
    if (LoadFromString(deserializedObject, serializedData)) {
        LOG("Deserialized object successfully:");
        DUMP(deserializedObject);

        // Verify
        ASSERT(deserializedObject.name == "TestObject");
        ASSERT(deserializedObject.value == 123);
    } else {
        LOG_ERROR("Failed to deserialize object!");
    }

    // Example of serializing to a file (optional)
    String tempFile = GetTempFileName();
    if (StoreToFile(originalObject, tempFile)) {
        LOG("Stored object to temporary file: " << tempFile);
        MySerializableObject loadedFromFileObject;
        if (LoadFromFile(loadedFromFileObject, tempFile)) {
            LOG("Loaded object from file successfully:");
            DUMP(loadedFromFileObject);
        } else {
            LOG_ERROR("Failed to load object from file!");
        }
        DeleteFile(tempFile); // Clean up
    } else {
        LOG_ERROR("Failed to store object to file!");
    }
}
