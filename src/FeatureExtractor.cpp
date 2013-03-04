#include "FeatureExtractor.h"
using namespace std;

FeatureExtractor::FeatureExtractor() {

}

ImageFeatures* FeatureExtractor::extractDsift(Image& img) {
	VlDsiftFilter* filter =
		vl_dsift_new_basic(img.getWidth(), img.getHeight(), 8, 4);
	vl_dsift_set_flat_window(filter, true);
	
	vl_dsift_process(filter, img.getData());
		
	unsigned int descriptorSize = vl_dsift_get_descriptor_size(filter);
	unsigned int numDescriptors = vl_dsift_get_keypoint_num(filter);
	float const* descriptors = vl_dsift_get_descriptors(filter);
	
	vector<pair<int, int> > coordinates;
	const VlDsiftKeypoint* keypoints = vl_dsift_get_keypoints(filter);
	for(unsigned int i = 0; i < numDescriptors; i++) {
		coordinates.push_back(make_pair(keypoints[i].x, keypoints[i].y));
	}
	
	ImageFeatures* imageFeatures =
		new ImageFeatures(descriptors, descriptorSize, numDescriptors,
			img.getWidth(), img.getHeight(), coordinates);
	
	vl_dsift_delete(filter);
	
	return imageFeatures;
}

ImageFeatures* FeatureExtractor::extractHog(Image& img) {
/*
	VlHog* hog = vl_hog_new(VlHogVariantDalalTriggs, 8, false);
	vl_hog_put_image(hog, img.getData(), img.getWidth(), img.getHeight(),
		1, 8);
		
	int descriptorSize = vl_hog_get_dimension(hog);
	int numDescriptors = vl_hog_get_width(hog) * vl_hog_get_height(hog);
	
	float* descriptors =
		new float[descriptorSize * numDescriptors];
	vl_hog_extract(hog, descriptors);
	
	ImageFeatures* imageFeatures =
		new ImageFeatures(descriptors, descriptorSize, numDescriptors);
	
	delete[] descriptors;
	vl_hog_delete(hog);
	
	return imageFeatures;
*/
}