# Particle-Filter
A simple particle filter implementation for object tracking
<br />   
  
## Requriement
1. CMake ( version 3.5.1 or later )  
2. OpenCV ( version 3.4.1 or later ) 
<br /> 

## Installation
#### The following building code and installation procedures are for Linux platform only.  
1. Please make sure your current working directories is under the program's root folder.
2. mkdir build
3. cd build
4. cmake ..
5. make
6. make install
7. Finally, the program will be installed under the Particle folder.
8. ./ParticleFilter
<br /> 

## Brief Introduction
The Simple Condensation Filter Algorithm (pseudo code) :  

```
If ( First frame (t=1) ) 
{
  Random select N positions (# of particles) on current frame;
}
Else
{
  Sample the particles based on the transition probability
}


Calculate the weight for each particle;


Normalization();


Resmaple();
```


