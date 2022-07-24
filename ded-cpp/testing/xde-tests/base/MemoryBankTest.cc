#include <base/MemoryBank.h>

#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "doctest.h"
using namespace std;

SCENARIO("Test Basic Memory Bank Functionality")
{

    GIVEN("A Basic Assignment")
    {
        MemoryBank memory;

        vector<string> vars = {
            "A", "B", "C", "A1", "B1", "C1", "A2", "B2", "C2"};

        vector<string> blockA = {
            "A", "A1", "A2"};

        for (unsigned i = 0; i < vars.size(); i++)
        {
            memory.setValue(vars[i], -((int)i));
        }

        // int block1 = memory.createBlock(blockA);
        // memory.setValues(block1, blockAValues);

        THEN("Value of A should be correctly assigned") { CHECK(memory.getValue("A") == 0); }
        THEN("Value for B should be correctly assigned") { CHECK(memory.getValue("C") == -2); }
    }

    GIVEN("A Block Assignment")
    {
        MemoryBank memory;

        vector<string> vars = {
        "A", "B", "C", "A1", "B1", "C1", "A2", "B2", "C2"};

        vector<string> blockA = {
        "A", "A1", "A2"};
        double blockAValues[] = {10, 20, 30};

        for (unsigned i = 0; i < vars.size(); i++)
        {
            memory.setValue(vars[i], -((int)i));
        }

        int block1 = memory.createBlock(blockA);
        memory.setValues(block1, blockAValues);
        THEN("Value for A should be correctly assigned") { CHECK(memory.getValue("A") == 10); }
        THEN("Value for B should be correctly assigned") { CHECK(memory.getValue("B") == -1); }
        THEN("Value for C should be correctly assigned") { CHECK(memory.getValue("C") == -2); }
        THEN("Value for A1 should be correctly assigned") { CHECK(memory.getValue("A1") == 20); }
        THEN("Value for B1 should be correctly assigned") { CHECK(memory.getValue("B1") == -4); }
        THEN("Value for C1 should be correctly assigned") { CHECK(memory.getValue("C1") == -5); }
        THEN("Value for A2 should be correctly assigned") { CHECK(memory.getValue("A2") == 30); }
        THEN("Value for B2 should be correctly assigned") { CHECK(memory.getValue("B2") == -7); }
        THEN("Value for C2 should be correctly assigned") { CHECK(memory.getValue("C2") == -8); }
    }
}
