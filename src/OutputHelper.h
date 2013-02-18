#ifndef OUTPUT_HELPER_H
#define OUTPUT_HELPER_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

#include "boost/multi_array.hpp"

class OutputHelper {
public:
	OutputHelper();
	
	void printMessage(std::string message = "", unsigned int indentLevel = 0);
	
	void printInlineMessage(std::string message = "",
		unsigned int indentLevel = 0);
		
	void printProgress(std::string message, unsigned int current,
		unsigned int total,	unsigned int indentLevel = 1);
		
	void printResults(std::string message, unsigned int current,
		unsigned int total, int result, float value,
		unsigned int indentLevel = 1);
		
	void printResults(unsigned int correctTrain, unsigned int sizeTrain,
		 unsigned int correctTest, unsigned int sizeTest);
	
	void printConfusionMatrix(const std::vector<std::string>& classes, 
		const boost::multi_array<float, 2>& data);

	std::string clearLine();
	
	std::string indent(int level);
};

#endif
