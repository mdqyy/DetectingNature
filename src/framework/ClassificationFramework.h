#ifndef CLASSIFICATION_FRAMEWORK_H
#define CLASSIFICATION_FRAMEWORK_H

#include <fstream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/functional/factory.hpp>

#include "utils/CacheHelper.h"
#include "utils/DatasetManager.h"
#include "images/HSVImageLoader.h"
#include "images/OpponentImageLoader.h"
#include "images/GreyscaleImageLoader.h"
#include "features/LBPFeatureExtractor.h"
#include "features/HOGFeatureExtractor.h"
#include "features/SIFTFeatureExtractor.h"
#include "features/HellingerFeatureTransform.h"
#include "codebook/KMeansCodebookGenerator.h"
#include "codebook/FisherCodebookGenerator.h"
#include "classification/SVMClassifier.h"
#include "classification/LinearClassifier.h"
#include "framework/SettingsManager.h"

/**
 * @brief Main class for the classification of images.
 *
 * This class brings together all the scene classification bits ans pieces and
 * trains an image classifier capable of distinguishing between the different
 * categories in the given dataset.
 */
class ClassificationFramework {
public:
	struct Result {
		std::string filepath;
		std::string category;
		double certainty;
	};

	/**
	 * @brief Initializes all the classification parameters
	 * 
	 * @param datasetPath Relative path to the dataset to be used to
	 * train the classifier.
	 * @param settings Contains the parameters used by the multiple algorithms
	 * involved in the classification process.
	 * @param skipCache If enabled the framework will ignore the cache when
	 * generating the codebook and the histograms. This does NOT regenerate
	 * the image descriptors since these are not random and should not change
	 * between multiple runs.
	 */
	ClassificationFramework(std::string datasetPath,
		const SettingsManager *settings, bool skipCache);
	~ClassificationFramework();
	
	
	/**
	 * @brief Trains a classifier.
	 *
	 * Creates the intermediate representations for all images and then trains
	 * a classifier.
	 */
	void train();
	
	/**
	 * @brief Tests a classifier.
	 * 
	 * Uses a classifier on a set of testing images and returns the average
	 * accuracy of the classifier.
 	 *
	 * @pre A classifier must be trained using train()
	 *
	 * @return The average of the diagonal of the confusion matrix. Returned as
	 * a percentage, between 0 and 1.
	 */
	double testRun();
	
	/**
	 * @brief Trains a classifier and predicts the class of other images.
	 * 
	 * Uses a classifier to predict the classes for all images
	 * in @a imagesFolder.
	 *
	 * @pre A classifier must be trained using train()
	 *
	 * @param imagesFolder The folder containing unclassified images for wich
	 * we want to determine their class.
	 * @return A map relating the file path and the name of its predicted class.
	 */
	std::vector<Result> classify(std::string imagesFolder);

private:
	bool m_skipCache;
	const SettingsManager* m_settings;
	
	CacheHelper* m_cacheHelper;
	DatasetManager* m_datasetManager;
	
	ImageLoader* m_imageLoader;
	FeatureExtractor* m_featureExtractor;	
	std::vector<FeatureTransform*> m_featureTransforms;
	CodebookGenerator* m_codebookGenerator;
	Classifier* m_classifier;
	
	std::vector<std::string> m_imagePaths;
	std::string m_cachePath;
	
	std::vector<Histogram*> m_trainHistograms;
	
	Codebook* prepareCodebook(
		std::vector<std::string> imagePaths, bool skipCache);
	ImageFeatures* extractFeature(std::string imagePath);
	Histogram* generateHistogram(Codebook* codebook, std::string filePath);
	
	std::vector<ImageFeatures*> extractFeatures(
		std::vector<std::string> imagePaths);
	std::vector<Histogram*> generateHistograms(
		std::vector<std::string> imagePaths, bool skipCodebook);
	double trainClassifier();
};

#endif
