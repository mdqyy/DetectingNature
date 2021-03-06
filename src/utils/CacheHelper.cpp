#include "CacheHelper.h"
using namespace std;

CacheHelper::CacheHelper(string datasetPath, const SettingsManager* settings) {
	m_datasetPath = datasetPath;
	m_settings = settings;
	m_enabled = m_settings->get<bool>("framework.cacheData");
}

// Generate a cache path. This must be different for different settings in order
// to prevent cache hits on different settings.
string CacheHelper::getCacheFolder(string filename,
		const type_info& dataType) const {
	
	string dataTypeName = dataType.name();
	dataTypeName.erase(
		boost::remove_if(dataTypeName, ::isdigit), dataTypeName.end());
	
	string basePath = "cache/" + m_datasetPath +
			"/" + dataTypeName;
	
	if(dataType == typeid(DatasetManager)) {
		return basePath + "/";
	}
	
	stringstream cacheNameStream;
	cacheNameStream	<< 
		"_" << m_settings->get<string>("image.type") <<
		"_" << m_settings->get<float>("image.smoothingSigma") << 
		"_" << m_settings->get<string>("features.type") <<
		"_" << m_settings->get<vector<int> >("features.gridSpacing")[0] <<
		"_" << m_settings->get<vector<int> >("features.patchSize")[0];
		
	if(dataType == typeid(ImageFeatures)) {
		return basePath + cacheNameStream.str() + "/";
	}
	
	cacheNameStream	<<
		"_" << m_settings->get<int>("codebook.textonImages") <<
		"_" << m_settings->get<int>("codebook.codewords") <<
		"_" << m_settings->get<string>("histogram.type") <<
		"_" << m_settings->get<int>("histogram.pyramidLevels");
	
	return basePath + cacheNameStream.str() + "/";
}
