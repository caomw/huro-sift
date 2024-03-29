#include <iostream>

#include "ObjectRecognition\StarFeature.h"
#include "ObjectRecognition\LocalSettings.h"

using namespace std;
using namespace cv;

namespace ObjectRecognition
{


StarFeature::StarFeature(const string& name, const string& type)
:	LocalFeature(name, type)
{
	LoadSettingsFromFileStorage();

    starDetector_ = new StarFeatureDetector(maxSize_, responseThreshold_, 
        lineThresholdProjected_, lineThresholdBinarized_, suppressNonmaxSize_);
}


StarFeature::~StarFeature(void)
{
    delete starDetector_;
}


void StarFeature::LoadSettingsFromFileStorage(void)
{
	string fileName = LocalSettingsPtr->GetFeatureExtractorDirectory() + "Settings." + name_ + ".xml";
	FileStorage fileStorage(fileName, FileStorage::READ, "UTF-8");

	if(!fileStorage.isOpened())
		CV_Error(1, "Setting XML does not exist for " + name_ + "!");

	fileStorage["maxSize"] >> maxSize_;
	fileStorage["responseThreshold"] >> responseThreshold_;
	fileStorage["lineThresholdProjected"] >> lineThresholdProjected_;
	fileStorage["lineThresholdBinarized"] >> lineThresholdBinarized_;
	fileStorage["suppressNonmaxSize"] >> suppressNonmaxSize_;
}


void StarFeature::Process(void)
{
    starDetector_->detect(frame_, keyPoints);
}


void StarFeature::DrawFeatures(void)
{
    drawKeypoints(frame_, keyPoints, frame_, Scalar::all(-1), DrawMatchesFlags::DEFAULT /*| DrawMatchesFlags::DRAW_RICH_KEYPOINTS*/); 
}


}
