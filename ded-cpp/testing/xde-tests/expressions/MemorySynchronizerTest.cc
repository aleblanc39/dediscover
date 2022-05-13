#include <base/MemoryBank.h>
#include <expressions/MemoryBankSynchronizer.h>

#include <iostream>
#include <string>
#include <vector>
#include <doctest.h>

using namespace std;

/**
 * This program will test the functionality of the
 * MemoryBankSyncrhonizer. Right now it is really a wrapper around the
 * memory bank, but it could get more complex.
 */


SCENARIO("Testing the MemoryBankSynchronizer") {
    vector<string> varNames = {"A", "B", "C", "A1", "B1", "C1", "A2", "B2", "C2"};
    vector<double> values = {3,5,6,7,9,-1,-2, 0.5, -0.6};

    vector<string> blockANames = {"A", "A1", "A2"};
    double blockAValues[] = {10, 20, 30};

    MemoryBank memory;
    MemoryBankSynchronizer sync(&memory);

    for (auto i = 0; i < varNames.size(); i++)
        sync.updateMemory(varNames[i], values[i]);
    GIVEN("Variables are set manually") {
        THEN("Value at index 0 is as assigned") {CHECK(sync.getValue(varNames[0]) == values[0]);}
        THEN("Value at index 1 is as assigned") {CHECK(sync.getValue(varNames[1]) == values[1]);}
        THEN("Value at index 2 is as assigned") {CHECK(sync.getValue(varNames[2]) == values[2]);}
        THEN("Value at index 3 is as assigned") {CHECK(sync.getValue(varNames[3]) == values[3]);}
        THEN("Value at index 4 is as assigned") {CHECK(sync.getValue(varNames[4]) == values[4]);}
        THEN("Value at index 5 is as assigned") {CHECK(sync.getValue(varNames[5]) == values[5]);}
        THEN("Value at index 6 is as assigned") {CHECK(sync.getValue(varNames[6]) == values[6]);}
        THEN("Value at index 7 is as assigned") {CHECK(sync.getValue(varNames[7]) == values[7]);}
        THEN("Value at index 9 is as assigned") {CHECK(sync.getValue(varNames[8]) == values[8]);}

    }
    GIVEN("A block is created and values are updated") {
        auto block1 = sync.createBlock(blockANames);
        sync.setValues(block1, blockAValues);
        THEN("Value of A should now be 10"){CHECK(sync.getValue(varNames[0]) == 10);}
        THEN("Value at index 1 is as assigned") {CHECK(sync.getValue(varNames[1]) == values[1]);}
        THEN("Value at index 2 is as assigned") {CHECK(sync.getValue(varNames[2]) == values[2]);}
        THEN("Value of A1 should now be 20"){CHECK(sync.getValue(varNames[3]) == 20);}
        THEN("Value at index 4 is as assigned") {CHECK(sync.getValue(varNames[4]) == values[4]);}
        THEN("Value at index 5 is as assigned") {CHECK(sync.getValue(varNames[5]) == values[5]);}
        THEN("Value of A2 should now be 30"){CHECK(sync.getValue(varNames[6]) == 30);}
        THEN("Value at index 7 is as assigned") {CHECK(sync.getValue(varNames[7]) == values[7]);}
        THEN("Value at index 9 is as assigned") {CHECK(sync.getValue(varNames[8]) == values[8]);}

        auto retBlockValues = sync.getValues(block1);
        THEN("First value in block is 10") {CHECK(retBlockValues[0] == 10);}
        THEN("Second value in block is 20") {CHECK(retBlockValues[1] == 20);}
        THEN("Third value in block is 30") {CHECK(retBlockValues[2] == 30);}
    }
}
