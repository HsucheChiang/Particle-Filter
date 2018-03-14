#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace std;
using namespace cv;

class Particle
{

private:
    int nParticles; //the total number of particles
    int histSize[3];
    int channels[3];
    int maxProb; //To indicate the particle with the biggest weight
    float hranges[2];
    const float* ranges[3];
    vector<Point2f> particlePos; //the position of the particles.
    vector<double> weight; //the probability that the particle matches the object.
    Point2f ROI; //To indicate the position of the target patch
    Mat imagePatch; //the image which include the target patch
    Mat inputFrame; //the input image to be tracked.
    Mat patchHist; //the histogram of the patch.
    Mat imageHist; //histogram of a region around a certain particle.
    Mat inputFrameTarget; //the image shows the most possible position.
    Mat initialParticleImage; //To get the initial distribution of the particles
    Size patchSize; //Indicate the size of the target patch

    void normalization();
    void reSampling();

public:
    Particle(Mat &_imagePatch, Mat &_inputFrame, int _nParticles, Size _patchSize, Point2f &_ROI);
    ~Particle();
    Mat getTarget();
    Mat getParticleImage();
    Mat getinitialParticleImage();
    void setInputImage(Mat &image);
    void particleAlgo();
    void generateParticlesInit(); //Initialize the particles
};
