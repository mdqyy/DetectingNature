#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Image.h"
#include "Codebook.h"
#include "FeatureExtractor.h"

struct Settings {
	Settings();
	Settings(std::string filename);
	
	// Image settings
	Image::Colourspace colourspace;
	
	// Feature settings
	FeatureExtractor::Type featureType;
	unsigned int gridSpacing;
	unsigned int patchSize;
	
	// Codebook settings
	unsigned int textonImages;
	unsigned int codewords;
	
	// Histogram settings
	Codebook::Type histogramType;
	unsigned int pyramidLevels;
	
	// Classifier settings
	double C;
	unsigned int trainImagesPerClass;
};

#endif
