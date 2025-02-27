#include "robot.hpp"
#include <algorithm>
#include <functional>
#include <cmath>

// Define RGB structure
struct RGB
{  
    // R, G, and B values are 0-255
    int r; 
    int g;
    int b;
};

// Define HSV structure - Example: https://upload.wikimedia.org/wikipedia/commons/f/f2/HSV_color_solid_cone.png
struct HSV 
{  
    int H;  // Hue, 0 - 360 (what base colour this actually is (sees in the dark!))
    float S;  // Saturation, 0-1 
    float V; // Value, 0-1
};    

// RGB to HSV function I created by adapting the math's algorithm to c++
HSV returnHSV(int r, int g, int b) 
{
	double rRange = r/255.0;
	double gRange = g/255.0;
	double bRange = b/255.0;

	double cMax = std::max(std::max(rRange,gRange),bRange);
	double cMin = std::min(std::min(rRange,gRange),bRange);
	double delta = cMax - cMin;

	int H;
	double S;
	double V;
        
	// Hue calculation
	if(cMax==rRange)
	{
		H=60*fmod((gRange-bRange)/delta,6);
	}
	else if(cMax==gRange)
	{
		H=60*((bRange-rRange)/delta+2);
	}
	else if(cMax==bRange)
	{
		H=60*((rRange-gRange)/delta+4);
	}
	else
	{
		std::cout<<"Error in finding cMax comparison";
	}
	// Saturation calculation
	if(cMax==0.0)
	{
		S=0;
	}
	else
	{
		S=delta/cMax;
	}
	
	V=cMax; // Value calculation

	HSV hsvToReturn;
	hsvToReturn.H = H;
	hsvToReturn.S = S;
	hsvToReturn.V = V;
	
        // Return HSV equivalent!
	return hsvToReturn;
}

int isPixelWhite(int luminosity)
{
	if(luminosity > 240)
	{
		return 1; 
	}
	else 
	{
		return 0;
	}
}

int main(){
    // Declare constants
    const static int WADDLE_WIDTH = 50; 
    const static int WADDLE_HEIGHT = 40;
    const static int PIXEL_THRESHOLD = 5;
    const static double ROBOT_SPEED = 25.0;
	
    if (initClientRobot() !=0){
	std::cout<<" Error initializing robot"<<std::endl;
    }
    double vLeft = ROBOT_SPEED;
    double vRight = ROBOT_SPEED;
    // Variable to store current frame in
    ImagePPM currentFrame; 

    while(1){
	    	// Take picture
		takePicture(); 
	        // Save it as view.ppm
		SavePPMFile("view.ppm",cameraView); 
		// Open view.ppm, and save the contents in currentFrame variable
		OpenPPMFile("view.ppm", currentFrame); 
		
		//std::cout<<"width:"<<currentFrame.width; // Print image stats
		//std::cout<<"height:"<<currentFrame.height;
		int leftWhitePixels = 0;
		int rightWhitePixels = 0;
	        // For each of the bottom 40 rows
		for(int currentRow = currentFrame.height - WADDLE_HEIGHT; currentRow < currentFrame.height; currentRow++) 
		{ 
			int pixelGrayscale;
			// Split into left and right as we are not scanning the middle
			for(int leftColumn = 0; leftColumn < WADDLE_WIDTH; leftColumn++) 
			{
				// Get grayscale of pixel
				pixelGrayscale = get_pixel(currentFrame, currentRow, leftColumn, 3); 
				// Use checker method to see if it is and add the returned value to white pixels var
				leftWhitePixels += isPixelWhite(pixelGrayscale); 
			}
			// Right side (pixels 100-150)
			for(int rightColumn = 2*WADDLE_WIDTH; rightColumn < currentFrame.width; rightColumn++) 
			{
				// Same thing but for right side
				pixelGrayscale = get_pixel(currentFrame, currentRow, rightColumn, 3); 
				rightWhitePixels += isPixelWhite(pixelGrayscale);
			}			
		}
		// Go forward
		if(leftWhitePixels > PIXEL_THRESHOLD && rightWhitePixels > PIXEL_THRESHOLD) 
		{
			vLeft = ROBOT_SPEED;
			vRight = ROBOT_SPEED;
		}
	        // Go left
		else if(leftWhitePixels > PIXEL_THRESHOLD) 
		{
			vLeft = 0.0;
			vRight = ROBOT_SPEED;
		}
	    	// Go right
		else if(rightWhitePixels > PIXEL_THRESHOLD) 
		{
			vLeft = ROBOT_SPEED;
			vRight = 0.0;			
		}

		setMotors(vLeft,vRight);   
		//std::cout<<" vLeft="<<vLeft<<"  vRight="<<vRight<<std::endl;
		//std::cout<<"Left="<<leftWhitePixels<<" Right="<<rightWhitePixels<<std::endl;
		usleep(10000);
  }

}
