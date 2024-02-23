#include "utils.h"

using namespace std;
using namespace cv;

Mat image, extendedImage;
vector<pair<pair<int,int>,int>> contrastList;
vector<Image> imageList;
int shapeContrast = 50;

int main(int argc, char* argv[]){

	//string path = "img/2_E.png";
	//if(argc>1) path = "img/"+string(argv[1])+".png";
	
	extractImage("img/number/1_D.png");
	getIdentifier();
	
	cout << "===" << endl;
	
	extractImage("img/number/2_D.png");
	getIdentifier();
	
	cout << "===" << endl;
	
	extractImage("img/number/3_D.png");
	getIdentifier();
	
	cout << "===" << endl;
	
	extractImage("img/number/hand2_A.png");
	getIdentifier();
	
	destroyAllWindows();
	
	return 0;
}
