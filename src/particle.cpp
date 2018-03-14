#include "particle.h"

//Constructor
Particle::Particle(Mat &_imagePatch, Mat &_inputFrame, int _nParticles, Size _patchSize, Point2f &_ROI)
: nParticles(_nParticles), patchSize(_patchSize), ROI(_ROI)
{
    imagePatch = _imagePatch.clone();
    inputFrame = _inputFrame.clone();

    //Parameter settings for histogram calculation
    hranges[0] = 0.0;
    hranges[1] = 255.0;
    ranges[0] = hranges;
    ranges[1] = hranges;
    ranges[2] = hranges;
    channels[0] = 0;
    channels[1] = 1;
    channels[2] = 2;
    histSize[0] = 16;
    histSize[1] = 16;
    histSize[2] = 16;
}


//Destructor
Particle::~Particle()
{

}

// The core of the particle filtering algorithm
void Particle::particleAlgo()
{
    weight.clear();
    random_device rd;
    mt19937 generator(rd());

    normal_distribution<double> norm(0.0,30.0); //Generate the gaussian nosie
    double noiseOffset = 0;
    int xTmp, yTmp;
    vector<Point2f> particlePosTmp;
    Mat imageROI;
    double weightTemp;


    for(int i=0 ;i<nParticles; i++)
    {
        noiseOffset = norm(generator);
        xTmp = particlePos[i].x + (int) ceil(noiseOffset);
        noiseOffset = norm(generator);
        yTmp = particlePos[i].y + (int) ceil(noiseOffset);
        particlePosTmp.push_back(Point2f(xTmp, yTmp));

        //Calculate weight
        if((xTmp+patchSize.width)<inputFrame.size().width && (yTmp+patchSize.height)<inputFrame.size().height && (xTmp)>=0 && (yTmp)>=0)
        {
            imageROI=inputFrame(Rect(xTmp,yTmp,patchSize.width,patchSize.height));
            calcHist(&imageROI, 1, channels, Mat(), imageHist, 3, histSize, ranges, true, false);
            weightTemp = compareHist(patchHist, imageHist, CV_COMP_CORREL);
            weightTemp = (weightTemp > 0)? weightTemp : 0;
            weight.push_back(weightTemp);
            //rectangle(initialParticleImage, Point2f(xTmp,yTmp), Point2f(xTmp+patchSize.width,yTmp+patchSize.height),Scalar(0,255,255),2);
        }
        else
        {
            weightTemp = 0.0;
            weight.push_back(weightTemp);
        }
    }
    particlePos.clear();
    particlePos.assign(particlePosTmp.begin(),particlePosTmp.end());
    normalization();
    reSampling();

}



//Particle filter initialization for the first frame
void Particle::generateParticlesInit()
{
    srand( time(NULL) );
    particlePos.clear();
    weight.clear();
    int xRandom, yRandom;
    Mat imageROI;
    double weightTemp;

    //Calculate the patch histogram
    imageROI = imagePatch(Rect(ROI.x-(patchSize.width)/2,ROI.y-(patchSize.height)/2,(patchSize.width),(patchSize.height)));
    calcHist(&imageROI, 1, channels, Mat(), patchHist, 3, histSize, ranges, true, false);
    initialParticleImage = inputFrame.clone();

    for(int i = 0 ; i < nParticles; i++)
    {
        xRandom = rand() % (inputFrame.size().width);
        yRandom = rand() % (inputFrame.size().height);
        particlePos.push_back(Point2f(xRandom,yRandom));
        if((xRandom+patchSize.width)<inputFrame.size().width && (yRandom+patchSize.height)<inputFrame.size().height)
        {
            imageROI=inputFrame(Rect(xRandom,yRandom,patchSize.width,patchSize.height));
            calcHist(&imageROI, 1, channels, Mat(), imageHist, 3, histSize, ranges, true, false);
            weightTemp = compareHist(patchHist, imageHist, CV_COMP_CORREL);
            weightTemp = (weightTemp > 0)? weightTemp : 0;
            weight.push_back(weightTemp);
            rectangle(initialParticleImage, Point2f(xRandom,yRandom), Point2f(xRandom+patchSize.width,yRandom+patchSize.height),Scalar(0,255,255),2);
        }
        else
        {
            weightTemp = 0.0;
            weight.push_back(weightTemp);
        }
    }
    normalization();
    reSampling();
}


//Weight normalization
void Particle::normalization()
{
    double sumWeight = 0.0;
    sumWeight = accumulate(weight.begin(), weight.end(), 0.0);
    if(sumWeight == 0.0)
    {
        generateParticlesInit();
    }
    else
    {
        for(int i = 0; i<nParticles ; i++)
        {
            weight[i] /= sumWeight;
        }
        maxProb = distance(weight.begin(), max_element(weight.begin(),weight.end()));
        inputFrameTarget = inputFrame.clone();
        rectangle(inputFrameTarget, Point2f(particlePos[maxProb].x,particlePos[maxProb].y), Point2f(particlePos[maxProb].x+patchSize.width,particlePos[maxProb].y+patchSize.height),Scalar(0,0,255),3);
    }

}


//Resampling to avoid particle degeneracy
void Particle::reSampling()
{
    int totalParticleCount = 0;
    double samplingCount = 0;
    vector<Point2f> particlePosTmp;
    for(int i=0; i<nParticles; i++)
    {
        samplingCount = floor(nParticles * weight[i]);
        for(int j=0; j<(int)samplingCount; j++)
        {
            particlePosTmp.push_back(particlePos[i]);
            totalParticleCount++;
        }
    }
    if((nParticles-totalParticleCount) > 0)
    {
        for(int i=0; i<(nParticles-totalParticleCount); i++)
        {
            particlePosTmp.push_back(particlePos[maxProb]);
        }
    }

    particlePos.clear();
    particlePos.assign(particlePosTmp.begin(),particlePosTmp.end());
    //particlePosTmp.clear();
}



//To get the image which labels the most possible location of the target patch
Mat Particle::getTarget()
{
    return inputFrameTarget;
}


//To see the location of all particles
Mat Particle::getParticleImage()
{
    int xTmp, yTmp;
    Mat ParticleImage;
    ParticleImage = inputFrame.clone();
    for(int i =0; i<nParticles; i++)
    {
        xTmp = particlePos[i].x;
        yTmp = particlePos[i].y;

        if((xTmp+patchSize.width)<inputFrame.size().width && (yTmp+patchSize.height)<inputFrame.size().height)
        {
            rectangle(ParticleImage, Point2f(xTmp,yTmp), Point2f(xTmp+patchSize.width,yTmp+patchSize.height),Scalar(0,255,255),2);
        }
    }
    return ParticleImage;
}


//To get the initial distribution of the particles
Mat Particle::getinitialParticleImage()
{
    return initialParticleImage;
}


//Transfer input frame into algorithm
void Particle::setInputImage(Mat &image)
{
    inputFrame = image.clone();
}
