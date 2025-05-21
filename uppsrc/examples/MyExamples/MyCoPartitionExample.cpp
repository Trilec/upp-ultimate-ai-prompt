#include <Core/Core.h>

using namespace Upp;

CONSOLE_APP_MAIN {
    StdLogSetup(LOG_COUT|LOG_FILE);

    Vector<int> numbers;
    for(int i = 1; i <= 100; ++i) { // A larger dataset for better demonstration
        numbers.Add(i);
    }

    Atomic totalSum = 0; // Atomic for thread-safe accumulation

    LOG("Calculating sum of numbers from 1 to 100 in parallel...");

    // CoPartition will divide 'numbers' into sub-ranges and process them in parallel.
    // The lambda function is executed for each sub-range.
    CoPartition(numbers, [&](const SubRange<const Vector<int>>& subrange) {
        long localSum = 0; // Use long for local sum to prevent overflow if subranges are large
        for(int x : subrange) {
            localSum += x;
        }
        totalSum += localSum; // Atomically add the sum of this subrange to the total
        // Optional: Log the processing of a subrange (can be verbose)
        // LOG("Processed subrange, local sum: " << localSum);
    });

    LOG("Parallel sum calculation complete.");
    LOG("Total sum: " << totalSum);

    // Verification (sum of 1 to 100 is 5050)
    long expectedSum = 0;
    for(int i = 1; i <= 100; ++i) {
        expectedSum += i;
    }
    LOG("Expected sum (calculated sequentially): " << expectedSum);
    ASSERT(totalSum == expectedSum);
    
    // Example with a different action: Logging elements in subranges
    LOG("\nIllustrating CoPartition by logging elements in subranges:");
    Vector<String> stringItems = {"alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta"};
    CoPartition(stringItems, [&](const SubRange<const Vector<String>>& subrange) {
        String s;
        for(const String& item : subrange) {
            if(!s.IsEmpty()) s << ", ";
            s << item;
        }
        LOG("Processing subrange: [ " << s << " ]");
    });
}
