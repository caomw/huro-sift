#include "HuroAlgorithm.h"

#include <iostream>

#include "SiftFeature.h"
#include "SurfFeature.h"
#include "OrbFeature.h"
#include "StarFeature.h"
#include "MserFeature.h"

#include "ImageFrame.h"
#include "LocalSettings.h"
#include "ExceptionDescriptor.h"

using namespace std;
using namespace cv;

HuroAlgorithm::HuroAlgorithm(void)
{
    LoadSettingsFromFileStorage();
}

HuroAlgorithm::~HuroAlgorithm(void)
{
    for(FeaturePool::iterator fpElem = featurePool_.begin(); fpElem != featurePool_.end(); fpElem++)
        delete fpElem->second;

	delete imageFrame_;
}

void HuroAlgorithm::LoadSettingsFromFileStorage(void)
{
    FileStorage fileStorage(LocalSettingsPtr->GetProcessXmlFileName(), FileStorage::READ, "UTF-8");
    if(!fileStorage.isOpened())
        throw ExceptionError("Process XML does not exist (" + LocalSettingsPtr->GetProcessXmlFileName() + ")!");

    // Loading general settings
    FileNode node = fileStorage["settings"];
    string mediaType;
    string imageName;
    int cameraId;

    node[0]["mediaType"] >> mediaType;
    node[0]["imageName"] >> imageName;
    node[0]["cameraId"] >> cameraId;

    if(mediaType.compare("camera") == 0)
        imageFrame_ = new ImageFrame(cameraId);
    else if(mediaType.compare("image") == 0)
        imageFrame_ = new ImageFrame(imageName);
    else
        throw ExceptionError("Wrong media type in (" + LocalSettingsPtr->GetProcessXmlFileName() + ")!");
    
    // Loading feature extractors
    node = fileStorage["featureExtractors"];
    for(size_t i = 0; i < node.size(); ++i)
    {
        string name;
        node[i]["name"] >> name;

        // Global feature extractors
        if(name.compare("SIFT") == 0)
            featurePool_[name] = new SiftFeature(name);
        else if(name.compare("SURF") == 0)
            featurePool_[name] = new SurfFeature(name);
        else if(name.compare("ORB") == 0)
            featurePool_[name] = new OrbFeature(name);
        else if(name.compare("STAR") == 0)
            featurePool_[name] = new StarFeature(name);
        else if(name.compare("MSER") == 0)
            featurePool_[name] = new MserFeature(name);
        else
            throw ExceptionWarning("Unkonwn feature extractor (" + name + ")!");
    }
}

void HuroAlgorithm::Process(void)
{
    // Initializing the processes
    Mat frame = imageFrame_->GetFrame();
    for(FeaturePool::iterator fpElem = featurePool_.begin(); fpElem != featurePool_.end(); fpElem++)
    {
        fpElem->second->SetFrame(frame);
        fpElem->second->Start();
    }

    // Start working
    for(FeaturePool::iterator fpElem = featurePool_.begin(); fpElem != featurePool_.end(); fpElem++)
        int result = reinterpret_cast<int>(fpElem->second->Join());

	waitKey(0);
}