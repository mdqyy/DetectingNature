#include <boost/program_options.hpp>

#include "ClassificationFramework.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv) {
	unsigned int numRuns;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "print this message")
		("dataset", po::value<string>(),
			"folder containing the dataset to use for training")
		("settings", po::value<string>()->default_value("settings.xml"),
			"file containing all the classification parameters")
		("num-runs", po::value<unsigned int>(&numRuns)->default_value(1),
			"number of times to run the classifier")
		("classify", po::value<string>(),
			"folder containing pictures to be classified")
	;
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if(vm.count("help")) {
		cout << desc << endl;
		return 1;
	}

	string datasetPath;
	if(vm.count("dataset")) {
		datasetPath = vm["dataset"].as<string>();
	} else {
		cout << desc << endl;
		return 1;
	}	
	
	Settings settings(vm["settings"].as<string>());
	
	if(vm.count("classify")) {
		ClassificationFramework cf(datasetPath, settings, numRuns != 1);
		map<string, string> results = cf.classify(vm["classify"].as<string>());
		
		map<string, string>::iterator it;
		for(it = results.begin(); it != results.end(); it++) {
			cout << it->first << " = " << it->second << endl;
		}
		
	} else {
		double results[numRuns];
		for(unsigned int i = 0; i < numRuns; i++) {
			ClassificationFramework cf(datasetPath, settings, numRuns != 1);
			results[i] = cf.testRun();
		}
		double sum = accumulate(results, results + numRuns, 0.0);
		double mean = sum / numRuns;

		double sqSum = inner_product(
			results, results + numRuns, results, 0.0);
		double stdev = sqrt(sqSum / numRuns - mean * mean);
  
		if(numRuns > 1) {
			cout << "Average over " << numRuns << " runs: "
				<< mean * 100.0 << " +/- " << stdev * 100.0 << endl;
		}
	}
		
	return 0;
}
