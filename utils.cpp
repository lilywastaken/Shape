#include "utils.h"

void selectionSort(vector<Trait>& arr){
    int n(arr.size());
    for(int i=0; i<n-1; i++) {
        int max_idx(i);

        for(int j=i+1; j<n; j++){
            if(arr[j].size>arr[max_idx].size) max_idx = j;
        }

        if(max_idx!=i) swap(arr[max_idx],arr[i]);
    }
}

void extractImage(string path){
	image = imread(path);
	
	//resize(image, image, Size(image.cols/10, image.rows/10));
	
	//cvtColor(image, image, COLOR_BGR2GRAY);
	namedWindow("Image", WINDOW_NORMAL);
	imshow("Image", image);
	waitKey(1);
	
	extendedImage = Mat(image.rows*2-1, image.cols*2-1, CV_8UC3);
	
	auto start_time = chrono::high_resolution_clock::now();
	
	getContrast();
	getTrait();
	
	auto end_time = chrono::high_resolution_clock::now();
	
	auto duration = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);
	//cout << "Time taken: " << duration.count() << " seconds" << endl;
}

void getContrast(){

	namedWindow("Image Extended", WINDOW_NORMAL);
	
	for(int i=0; i<image.rows; i++){
		for(int j=0; j<image.cols; j++){
			// Obtenir le pixel d'origine
			Vec3b originalPixel = image.at<Vec3b>(i, j);
			Vec3b pix1 = image.at<Vec3b>(i, j+1);
			Vec3b pix2 = image.at<Vec3b>(i+1, j);
			Vec3b pix3 = image.at<Vec3b>(i+1, j+1);
			
			int moy1(0), moy2(0), moy3(0);
			
			if(i<image.rows && j<image.cols){
				moy1 = (abs(originalPixel[0]-pix1[0]) + abs(originalPixel[1]-pix1[1]) + abs(originalPixel[2]-pix1[2]))/3;
				moy2 = (abs(originalPixel[0]-pix2[0]) + abs(originalPixel[1]-pix2[1]) + abs(originalPixel[2]-pix2[2]))/3;
				moy3 = (abs(originalPixel[0]-pix3[0]) + abs(originalPixel[1]-pix3[1]) + abs(originalPixel[2]-pix3[2]))/3;
			}
			
			int distMiddle = abs(i-image.rows/2) + abs(j-image.cols/2);
			float ratio = 2*float(distMiddle)/(float(image.rows+image.cols));
			
			/*if(moy1<ratio*255) moy1=0;
			if(moy2<ratio*255) moy2=0;
			if(moy3<ratio*255) moy3=0;*/
			
			if(moy1<shapeContrast) moy1=0;
			if(moy2<shapeContrast) moy2=0;
			if(moy3<shapeContrast) moy3=0;

			//extendedImage.at<Vec3b>(2*i, 2*j) = originalPixel;
			extendedImage.at<Vec3b>(2*i, 2*j) = Vec3b(0, 0, 0);
			
			// Blue, Green, Red
			
			// Right pixel
			if(2*j+1<extendedImage.cols) extendedImage.at<Vec3b>(2*i, 2*j+1) = Vec3b(0, moy1, 0);
			// Down pixel
			if(2*i+1<extendedImage.rows) extendedImage.at<Vec3b>(2*i+1, 2*j) = Vec3b(0, moy2, 0);
			// Down-right pixel
			if(2*j+1<extendedImage.cols && 2*i+1<extendedImage.rows){
				extendedImage.at<Vec3b>(2*i+1, 2*j+1) = Vec3b(0, moy3, 0);
				if(extendedImage.at<Vec3b>(2*i+1, 2*j+1)[1]<moy1) extendedImage.at<Vec3b>(2*i+1, 2*j+1) = Vec3b(0, moy1, 0);
				if(extendedImage.at<Vec3b>(2*i+1, 2*j+1)[1]<moy2) extendedImage.at<Vec3b>(2*i+1, 2*j+1) = Vec3b(0, moy2, 0);
			}
		}
	}
	
	//Make it main without making an image later !!
	for(int i=0; i<extendedImage.rows; i++){
		for(int j=0; j<extendedImage.cols; j++){
			int moy(extendedImage.at<Vec3b>(i, j)[1]);
			if(moy>0) contrastList.push_back({{j,i},moy});
		}
	}
	
	//namedWindow("Image extended", WINDOW_NORMAL);
	//imshow("Image extended", extendedImage);
	
}

void getTrait(){
			
	namedWindow("Image Extended", WINDOW_NORMAL);
	imshow("Image Extended", extendedImage);
	//waitKey(0);
	
	vector<Trait> traitList;
	pair<int,int> curPos = {-1,-1};
	vector<int> prevDirList;
	vector<pair<int,int>> contactPoints;
	
	// Points are not proceeded yet
	while(!contrastList.empty()){
	
		// Drawing a shape
		int lastContactPos(0);
		
		//cout << "[ START SHAPE ]" << endl;
		
		if(curPos == make_pair(-1,-1)){
			curPos = contrastList[0].first;
			contrastList.erase(contrastList.begin());
		}
		
		extendedImage.at<Vec3b>(curPos.second, curPos.first) = Vec3b(255, 255, 255);
		
		int length(0);
		
		// Drawing a trait
		Trait newTrait;
		newTrait.startingPos = curPos;
		newTrait.contactPoints.push_back({curPos,0});
		//cout << "START " << curPos.first << "/" << curPos.second << endl;
		vector<int> directionList;
		int currentDir(-1);
		bool starting=true;
		
		bool traitFinished(false);
		while(!traitFinished){
		
			newTrait.size++;
		
			// Locate all points around
			vector<pair<int,int>> nearbyValues = {{curPos.first+1,curPos.second},{curPos.first,curPos.second+1},{curPos.first-1,curPos.second},{curPos.first,curPos.second-1}};
			vector<pair<int,int>> retainedValues; // Pos in list, direction
			for(int i=0; i<contrastList.size(); i++){
				for(int j=0; j<nearbyValues.size(); j++){
					if(nearbyValues[j] == contrastList[i].first){
						retainedValues.push_back({i,j});
					}
				}
			}
			
			// No point around : finish trait
			if(retainedValues.size()==0){
					
				if(starting) newTrait.contactPoints.clear();
				else{
					//Look for contact point at the end
					for(auto contact : contactPoints){
						for(auto nearby : nearbyValues){
							if(nearby == contact){
								newTrait.contactPoints.push_back({contact,newTrait.size});
								newTrait.endingPos = contact;
							}
						}
					}
				}
				
				if(starting && contactPoints.size()>0){
					contactPoints.erase(contactPoints.begin());
					if(contactPoints.size()>0) curPos = contactPoints[0];
					else curPos = {-1,-1};
					break;
				}
				
				if(contactPoints.size()>0) curPos=contactPoints[0];
				else curPos = {-1,-1};
				
				//cout << "[0] Pushed " << length << " to " << currentDir << endl;
				newTrait.elementList.push_back({currentDir,length});
				
				break;
			}
			
			starting=false;
			
			pair<int,int> chosenPath = retainedValues[0];
			for(pair<int,int> retainedVal : retainedValues){
				for(int prevDir : prevDirList){
					//cout << "CHECK: " << retainedVal.second << "->" << prevDir << endl;
					if(retainedVal.second==prevDir) chosenPath = retainedVal;
				}
			}
			
			// Search for known destination
			bool dirFound(false), bestDirFound(false);
			for(pair<int,int> retainedVal : retainedValues){
				for(int dir : directionList){
					if(retainedVal.second==dir){
						dirFound = true;
						chosenPath = retainedVal;
						for(int prevDir : prevDirList){
							if(dir==prevDir) bestDirFound = true;
						}
					}
					if(bestDirFound) break;
				}
				if(bestDirFound) break;
			}
			
			if(currentDir!=chosenPath.second && currentDir!=-1){
				//cout << "[1] Pushed " << length << " to " << currentDir << endl;
				newTrait.elementList.push_back({currentDir,length});
				length=0;
			}
			currentDir=chosenPath.second;
			
			length++;
			
			// Create contact points
			if(retainedValues.size()>1){
				contactPoints.insert(contactPoints.begin(), curPos);
				newTrait.contactPoints.push_back({curPos,newTrait.size});
				//cout << " [[ CP -> " << curPos.first << "/" << curPos.second << endl;
			}
			
			if(!dirFound){
				bool dirKnown(false);
				for(int dir : directionList){
					if(dir == chosenPath.second) dirKnown=true;
				}
				if(!dirKnown) directionList.push_back(chosenPath.second);
				if(directionList.size()>2){
					contactPoints.insert(contactPoints.begin(), curPos);
					newTrait.contactPoints.push_back({curPos,newTrait.size});
					//cout << " [[ CP -> " << curPos.first << "/" << curPos.second << endl;
					
					curPos=contactPoints[0];
					prevDirList=directionList;
					
					break;
				}
			}
			
			curPos = contrastList[chosenPath.first].first;
			contrastList.erase(contrastList.begin() + chosenPath.first);
			extendedImage.at<Vec3b>(curPos.second,curPos.first) = Vec3b(255, 255, 255);
		
		}
		
		if(newTrait.elementList.size()>1){
			//cout << "==== ASSEMBLED ====" << endl;
			for(int i=0; i<newTrait.elementList.size()-1; i++){
			
				int xLength(-1), yLength(-1);
				if(newTrait.elementList[i].first==0 || newTrait.elementList[i].first==2){
					xLength=newTrait.elementList[i].second;
					yLength=newTrait.elementList[i+1].second;
				}
				else{
					xLength=newTrait.elementList[i+1].second;
					yLength=newTrait.elementList[i].second;
				}
				
				float variation = float(xLength)/float(yLength);
				newTrait.variationList.push_back(variation);
			}
		}
		
		if(curPos==make_pair(-1,-1)) continue;
		if(newTrait.endingPos==make_pair(-1,-1)) newTrait.endingPos = curPos;
		if(newTrait.contactPoints.size()>1){
			if(newTrait.contactPoints[0] == newTrait.contactPoints[1]) newTrait.contactPoints.erase(newTrait.contactPoints.begin());
		}
		
		if(newTrait.size>0) traitList.push_back(newTrait);
		
		//imshow("Image Extended", extendedImage);
		//waitKey(0);
	}
	
	selectionSort(traitList);
	
	for(Trait trait : traitList){
		for(auto contact : trait.contactPoints){
			extendedImage.at<Vec3b>(contact.first.second,contact.first.first) = Vec3b(0, 0, 255);
		}
	}
	
	Image newImage = {traitList};
	
	/*int ite(0);
	for(Trait trait : traitList){
		if(ite++==100) break;
		int first(trait.startingPos.second), second(trait.startingPos.first);
		vector<pair<int,int>> valsToChange = {{first,second},{first+1,second},{first+1,second+1},{first,second+1},{first-1,second+1},{first-1,second},{first-1,second-1},{first,second-1},{first+1,second-1}};
		for(pair<int,int> val : valsToChange) extendedImage.at<Vec3b>(val.first, val.second) = Vec3b(255, 0, 255);
	}*/
	
    //newImage.contactPoints = imageContactPoints;
	imageList.push_back(newImage);
}

void drawPoint(int first, int second, Vec3b color){
	vector<pair<int,int>> valsToChange = {{first,second},{first+1,second},{first+1,second+1},{first,second+1},{first-1,second+1},{first-1,second},{first-1,second-1},{first,second-1},{first+1,second-1}};
	for(pair<int,int> val : valsToChange) extendedImage.at<Vec3b>(val.first, val.second) = color;
	imshow("Image Extended", extendedImage);
}

float distanceBetweenPoints(pair<int, int> point1, pair<int, int> point2){
    int deltaX = point2.first - point1.first;
    int deltaY = point2.second - point1.second;
    double distanceSquared = deltaX * deltaX + deltaY * deltaY;
    double distance = sqrt(distanceSquared);
    return distance;
}

float angleBetweenPoints(pair<int, int> point1, pair<int, int> point2){
    double deltaY = point2.first - point1.first;
    double deltaX = point2.second - point1.second;
    double angleRad = atan2(deltaY, deltaX);
    double angleDeg = (angleRad * 180.0 / M_PI);
    if(angleDeg<0) angleDeg += 360.0;
    return angleDeg;
}

float angleBetweenRelativePoints(pair<int, int> point1, pair<int, int> point2, int refAngle){
    double deltaY = point2.first - point1.first;
    double deltaX = point2.second - point1.second;
    double angleRad = atan2(deltaY, deltaX);
    double angleDeg = (angleRad * 180.0 / M_PI);
    if(angleDeg<0) angleDeg += 360.0;
    angleDeg-=refAngle;
    if(angleDeg<0) angleDeg += 360.0;
    
    if(angleDeg >= -90.0 && angleDeg <= 0.0) angleDeg -= 90.0;
    else if(angleDeg > 0.0 && angleDeg <= 90.0) angleDeg -= 90.0;
    else if(angleDeg > 90.0 && angleDeg <= 180.0) angleDeg -= 180.0;
    else if(angleDeg > 180.0 && angleDeg <= 270.0) angleDeg -= 180.0;
    else if(angleDeg > 270.0 && angleDeg <= 360.0) angleDeg -= 270.0;
    return angleDeg;
}

float rounded(float value){
    return roundf(value * 100.0f)/100.0f;
}

void getIdentifier(){
	
	vector<CPoint> contactPoints;
	
	for(auto trait : imageList[imageList.size()-1].traitList){
	
		CPoint contactPoint;
	
		pair<int,int> curPos = trait.startingPos;
		float angle(angleBetweenPoints(trait.startingPos,trait.endingPos));
		float distance(rounded(distanceBetweenPoints(trait.startingPos,trait.endingPos)));
		
		contactPoint.pointPos = curPos;
		contactPoint.pointChar = make_pair(angle,distance);
		
		for(auto otherTrait : imageList[imageList.size()-1].traitList){
		
			if(trait==otherTrait) continue;
			
			float angle1(angleBetweenRelativePoints(trait.startingPos,otherTrait.startingPos,angle));
			float angle2(angleBetweenRelativePoints(trait.startingPos,otherTrait.endingPos,angle));
			float distance1(rounded(distanceBetweenPoints(trait.startingPos,otherTrait.startingPos)/distance));
			float distance2(rounded(distanceBetweenPoints(trait.startingPos,otherTrait.endingPos)/distance));
			
			contactPoint.refPoint.push_back(make_pair(angle1,distance1));
			contactPoint.refPoint.push_back(make_pair(angle2,distance2));
		
		}
		
		extendedImage.at<Vec3b>(curPos.second, curPos.first) = Vec3b(0, 255, 255);
		for(auto element : trait.elementList){
			for(int i=0; i<element.second; i++){
				if(element.first==0) curPos.first++;
				if(element.first==1) curPos.second++;
				if(element.first==2) curPos.first--;
				if(element.first==3) curPos.second--;
				extendedImage.at<Vec3b>(curPos.second, curPos.first) = Vec3b(0, 255, 255);
			}
		}
		
		drawPoint(trait.startingPos.second, trait.startingPos.first, Vec3b(255, 0, 255));
		drawPoint(trait.endingPos.second, trait.endingPos.first, Vec3b(150, 0, 150));
		
		//cout << "Ang: " << angle << endl;
		
		contactPoints.push_back(contactPoint);
		
		waitKey(0);
		
		drawPoint(trait.startingPos.second, trait.startingPos.first, Vec3b(0, 255, 0));
		drawPoint(trait.endingPos.second, trait.endingPos.first, Vec3b(0, 150, 0));
	
	}
	
	//waitKey(0);
	
	////////////////////////////
	// COMPARE TO OTHER IMAGE //
	////////////////////////////
	
	/*imageList[imageList.size()-1].contactPoints = contactPoints;
	
	for(int i=imageList.size()-1; i--; i>0){
		float totalScore(0);
		// For every point of the image
		for(auto newContact : contactPoints){
			float finalScore(-1);
			for(auto newRef : newContact.refPoint){
				float linkBestScore(-1);
				for(auto oldContact : imageList[i].contactPoints){
					float bestScore(-1);
					for(auto oldRef : oldContact.refPoint){
						if(oldRef.first==0 || oldRef.second==0) continue;
						float score = (1-(abs(newRef.first/oldRef.first)) + abs(1-(newRef.second/oldRef.second)));
						if(bestScore==-1 || score>bestScore) bestScore=score;
					}
					if(linkBestScore==-1 || bestScore>linkBestScore) linkBestScore=bestScore;
				}
				//cout << linkBestScore << endl;
				if(finalScore==-1 || linkBestScore>finalScore) finalScore=finalScore;
			}
			cout << finalScore << endl;
			totalScore+=finalScore;
		}
		cout << "Total score: " << totalScore/imageList[i].contactPoints.size() << endl;
	}*/
	
	//exit(1);
}














