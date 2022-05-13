#ifndef XDECSVFILE_H
#define XDECSVFILE_H


#include <string>
#include <vector>
#include <base/XDEBlas.h>
#include <base/XDEBase.h>


class XDECSVFile;

typedef std::shared_ptr<XDECSVFile> XDECSVFilePtr;

/**
 * \brief Class to hold data in CSV format as understood by DEDiscover.
 *
 * Handle data in the CSV format, where the first row is the column names, the 
 * first column is the timepoints. Contains some methods to manipulate the data,
 * and for input/output.
 */


class XDECSVFile: public XDEBase {

 public:
    XDECSVFile();
    XDECSVFile(const std::vector<std::string> &_variableNames,
	       const std::vector<double> &_timepoints,
	       const TDoubleMatrix &_data) : timepoints(_timepoints), data(_data), variableNames(_variableNames){}

    virtual ~XDECSVFile();



    void setData(const TDoubleMatrix &newData);
    void setTimepoints(const std::vector<double> &newTimepoints);
    void setVariableNames(const std::vector<std::string> &newVarNames);

    void addData(const std::vector<std::string> &_variableNames,
		 const std::vector<double> &_timepoints,
		 const TDoubleMatrix &_data);
    void addData(XDECSVFile *otherFile);

    const std::vector<double> &getTimepoints() const {return timepoints;}
    const TDoubleMatrix &getData() const {return data;}
    const auto &getVariableNames() const {return variableNames;}

    std::vector<std::string> getAllHeaders();



    int getNbTimepoints() const {return timepoints.size();}
    int getNbVariables() const {return variableNames.size();}

    void setTimeColumnName(std::string s) {timeColumnName = s;}
    std::string getTimeColumnName() const {return timeColumnName;}


 private:
    std::string filename;
    std::vector<double> timepoints;
    TDoubleMatrix data;
    std::vector<std::string> variableNames;

    std::string timeColumnName;
    static std::string getDefaultTimeColumnName();


};



#endif // XDECSVFILE_H
