#include <doctest.h>

#include <unistd.h>
#include <string>

#include <mapping/DataTableMapping.h>
using namespace std;


SCENARIO("Create a new data table mapping, 3 columns and 5 timepoints") {
    std::string equations = "\n\
        dPrey/dt = (birth - predation * Predator) * Prey\n\
        dPredator/dt = (growth * Prey - death) * Predator\n\
        dGrass/dt = grass * grassGrowth / (Prey * eatingRate)";

    std::vector<ColumnData> columns;
    
    std::vector<string> formulas = {"Prey", "Predator", "Grass**2"};
    std::vector<double> timepoints = {0.0, 0.1, 0.2, 0.3, 0.5};

    columns.push_back(ColumnData(formulas[0], {1.0, -1.1, 1.2, -1.3, 1.4}));
    columns.push_back(ColumnData(formulas[1], {2.1, -2.2, 2.3, -2.4, 1.4}));
    columns.push_back(ColumnData(formulas[2], {-2.3, 3.2, -2.8, 3.4, 0.4}));

    auto dataMapping =  DataTableMappingPtr(new DataTableMapping(timepoints, columns));  

    auto mat = dataMapping->dataAsMatrix();
    THEN("The matrix should be a 5*3"){CHECK(mat.size1() == 5);CHECK(mat.size2() == 3);}
    THEN("First row should contain 1.0, 2.1, -2.3"){CHECK(mat(0,0) == 1.0);CHECK(mat(0,1) == 2.1); CHECK(mat(0,2) == -2.3);}
    THEN("First second should contain -1.1, -2.2, 3.2"){CHECK(mat(1,0) == -1.1);CHECK(mat(1,1) == -2.2); CHECK(mat(1,2) == 3.2);}
    THEN("First third should contain 1.4, 1.4, 0.4"){CHECK(mat(2,0) == 1.2);CHECK(mat(2,1) == 2.3); CHECK(mat(2,2) == -2.8);}
    THEN("First fourth should contain -1.3, -2.4, 3.4"){CHECK(mat(3,0) == -1.3);CHECK(mat(3,1) == -2.4); CHECK(mat(3,2) == 3.4);}
    THEN("First fifth should contain 1.4, 1.4, 0.4"){CHECK(mat(4,0) == 1.4);CHECK(mat(4,1) == 1.4); CHECK(mat(4,2) == 0.4);}

}