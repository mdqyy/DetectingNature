#include "Classifier.h"
using namespace std;

Classifier::Classifier(vector<string> classNames) {
	svm_set_print_string_function(&printSvm);
	
	m_classNames = classNames;
	
	m_svmParams = nullptr;
	m_svmProbs.resize(m_classNames.size(), nullptr);
	m_svmModels.resize(m_classNames.size(), nullptr);
}

Classifier::~Classifier() {
	if(m_svmProbs[0] != nullptr) {
		for(unsigned int i = 0; i < m_trainHistograms.size(); i++) {
			delete[] m_svmProbs[0]->x[i];
		}
		delete[] m_svmProbs[0]->x;
	}
	
	for(unsigned int i = 0; i < m_classNames.size(); i++) {
		if(m_svmModels[i] != nullptr) {
			delete m_svmModels[i];
		}
		
		if(m_svmProbs[i] != nullptr) {
			delete[] m_svmProbs[i]->y;
			delete m_svmProbs[i];
		}
	}

	if(m_svmParams != nullptr) {
		delete m_svmParams;
	}
}

float* Classifier::flattenHistogramData() {
	int histLength = m_trainHistograms[0]->getLength();
	int datasetSize = m_trainHistograms.size() * histLength;
	float* data = new float[datasetSize];
	int currentIndex = 0;
	for(unsigned int i = 0; i < m_trainHistograms.size(); i++) {
		memcpy(&data[currentIndex], m_trainHistograms[i]->getData(), histLength);
		currentIndex += histLength;
	}
	return data;
}

double Classifier::intersectionKernel(Histogram* a, Histogram* b) {
	double kernelVal = 0;
	const double* dataA = a->getData();
	const double* dataB = b->getData();
	
	for(unsigned int i = 0; i < a->getLength(); i++) {
		kernelVal += min(dataA[i], dataB[i]);
	}
	return kernelVal;
}

double* Classifier::buildClassList(unsigned int desiredClass) {
	double* classes = new double[m_trainHistograms.size()];
	for(unsigned int i = 0; i < m_trainHistograms.size(); i++) {
		classes[i] = (unsigned int)m_trainClasses[i] == desiredClass;
	}
	return classes;
}

void Classifier::train(vector<Histogram*> histograms,
		vector<unsigned int> imageClasses, double C) {
		
	m_trainHistograms = histograms;
	for_each(imageClasses.begin(), imageClasses.end(), [&](double imgClass) {
		m_trainClasses.push_back(imgClass);
	});
	
	OutputHelper::printMessage("Training Classifier:");

	m_svmParams = new svm_parameter();
	m_svmParams->svm_type = C_SVC;
	m_svmParams->kernel_type = PRECOMPUTED;
	m_svmParams->cache_size = 1000;
	m_svmParams->C = C;
	m_svmParams->eps = 1e-6;
	m_svmParams->shrinking = 1;
	m_svmParams->probability = 0;
	m_svmParams->nr_weight = 0;

	svm_node** kernel = new svm_node*[m_trainHistograms.size()];
	
	unsigned int currentIter = 0;
	#pragma omp parallel for
	for(unsigned int i = 0; i < m_trainHistograms.size(); i++) {
		kernel[i] = new svm_node[m_trainHistograms.size() + 2];
		kernel[i][0].index = 0;
		kernel[i][0].value = i + 1;
		#pragma omp parallel for
		for(unsigned int j = 0; j < m_trainHistograms.size(); j++) {				
			kernel[i][j+1].index = j + 1;
			kernel[i][j+1].value =
				intersectionKernel(m_trainHistograms[i], m_trainHistograms[j]);
		}
		
		#pragma omp critical
		{
			currentIter++;
			OutputHelper::printProgress("Calculating kernel matrix",
				currentIter, m_trainHistograms.size());
		}
	}
	
	for(unsigned int i = 0; i < m_classNames.size(); i++) {
		m_svmProbs[i] = new svm_problem();
		m_svmProbs[i]->l = m_trainHistograms.size();
		m_svmProbs[i]->y = buildClassList(i);
		m_svmProbs[i]->x = kernel;
	
		m_svmModels[i] = svm_train(m_svmProbs[i], m_svmParams);
		//svm_save_model("model.out", m_svmModel);
		cout << endl;
	}
}

double Classifier::test(vector<Histogram*> testHistograms,
		vector<unsigned int> testClasses) {
			
	OutputHelper::printMessage("Testing Classifier:");
	ConfusionMatrix confMat(m_classNames);
	
	unsigned int currentIter = 0;
	#pragma omp parallel for
	for(unsigned int i = 0; i < testHistograms.size(); i++) {
		pair<unsigned int, double> result = classify(testHistograms[i]);
		
		confMat.addEntry(testClasses[i], result.first);
		
		#pragma omp critical
		{
			currentIter++;
			OutputHelper::printResults("Predicting image", currentIter,
				testHistograms.size(), result.first, result.second);
		}
	}
	confMat.printMatrix();
	return confMat.getDiagonalAverage();
}

pair<unsigned int, double> Classifier::classify(Histogram* histogram) {
	svm_node testNode[m_trainHistograms.size() + 1];
	testNode[0].index = 0;
	testNode[0].value = 0;
	#pragma omp parallel for
	for(unsigned int j = 0; j < m_trainHistograms.size(); j++) {				
		testNode[j+1].index = j + 1;
		testNode[j+1].value =
			intersectionKernel(histogram, m_trainHistograms[j]);
	}
	
	unsigned int predictedClass = 0;
	double predictedValue = 1e6;
	
	for(unsigned int j = 0; j < m_classNames.size(); j++) {
		double thisValue;
		double thisClass =
			svm_predict_values(m_svmModels[j], testNode, &thisValue);
		thisValue = ((thisClass == 0 && thisValue < 0) ||
			(thisClass == 1 && thisValue > 0)) ?
			-thisValue : thisValue;
					
		if(thisValue < predictedValue) {
			predictedValue = thisValue;
			predictedClass = j;
		}			
	}
	
	return make_pair(predictedClass, predictedValue);
}
