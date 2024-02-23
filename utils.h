#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstring>
#include <algorithm>
#include <random>
#include <utility>

using namespace std;
using namespace cv;

//STRUCTURES
////////////////////////

struct Trait{
	int size = -1;
	pair<int,int> startingPos;
	pair<int,int> endingPos = {-1,-1};
	vector<pair<int,int>> elementList; //Direction, length
	vector<pair<pair<int,int>,int>> contactPoints; //Position, place in line
	vector<int> variationList;
	
	bool operator==(const Trait& other) const{
		return (startingPos==other.startingPos && endingPos==other.endingPos);
	}
	
};

struct CPoint{
	pair<int,int> pointPos; //Position
	pair<float,float> pointChar; //Angle, distance
	vector<pair<float,float>> refPoint; //Angle, distance (calibrated with point char)
};

struct Image{
	vector<Trait> traitList; //Sorted by length
	vector<CPoint> contactPoints; //Serve as identifier
};

//FUNCTIONS
////////////////////////

void selectionSort(vector<Trait>& arr);
void extractImage(string path);
void getContrast();
void getTrait();
void drawPoint(int first, int second, Vec3b color);
float distanceBetweenPoints(pair<int, int> point1, pair<int, int> point2);
float angleBetweenPoints(pair<int, int> point1, pair<int, int> point2);
float angleBetweenRelativePoints(pair<int, int> point1, pair<int, int> point2, int refAngle);
float rounded(float value);
void getIdentifier();


//VARIABLES
////////////////////////

extern Mat image;
extern Mat extendedImage;
extern vector<pair<pair<int,int>,int>> contrastList;
extern vector<Image> imageList;
extern int shapeContrast;


#endif
