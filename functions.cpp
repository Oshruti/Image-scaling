#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

void initializeImage(Pixel image[][MAX_HEIGHT]) {
  // iterate through columns
  for (unsigned int col = 0; col < MAX_WIDTH; col++) {
    // iterate through rows
    for (unsigned int row = 0; row < MAX_HEIGHT; row++) {
      // initialize pixel
      image[col][row] = {0, 0, 0};
    }
  }
}

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  // TODO: implement image loading
  ifstream imageFS(filename);
  int max_value;
  string type;
  short red, blue, green;
  if(!imageFS.is_open()){
    throw std::runtime_error("Failed to open " + filename);
  }
  imageFS >> type;
  if(!(type=="P3" || type=="p3")){
    throw std::runtime_error("Invalid type " + type);
  }
  imageFS >> width;
  imageFS >> height;
  imageFS >> max_value;
  if(imageFS.fail()||height>MAX_HEIGHT||width>MAX_WIDTH){
    throw std::runtime_error("Invalid dimensions");
  }
  for (unsigned int row = 0; row < height; row++) {
    // iterate through rows
    for (unsigned int col = 0; col < width; col++) {
      imageFS >> red;
      if(imageFS.fail()||red>max_value||red<0){
        //cout << red << "error" << endl;
        throw std::runtime_error("Invalid color value"); //FIXME
      }
      if(imageFS.eof()){
        throw std::runtime_error("Not enough values"); //FIXME
      }
      imageFS >> green;
      if(imageFS.fail()||green>max_value||green<0){
        //cout << green << "error3" << endl;
        throw std::runtime_error("Invalid color value"); //FIXME
      }
      if(imageFS.eof()){
        //cout << green << "error4" << endl;
        throw std::runtime_error("Not enough values"); //FIXME
      }
      imageFS >> blue;
      if(imageFS.fail()||blue>max_value||blue<0){
        //cout << blue << "error5" << endl;
        throw std::runtime_error("Invalid color value"); //FIXME
      }
      if(imageFS.eof() && !(col==width-1 && row==height-1)){
        throw std::runtime_error("Not enough values"); //FIXME
      }
      image[col][row] = {red, green, blue};
    }
  }
  /*
  cout<<blue<<endl;
  cout << imageFS.eof() <<endl;
  */
  string whitespace;
  imageFS >> whitespace; //as extra line is there in some ppm files
  for(unsigned int i=0; i<whitespace.size(); i++){
    if(isspace(whitespace.at(i))==0){
      throw std::runtime_error("Too many values");
    }
  }
  if(!imageFS.eof()){
    throw std::runtime_error("Too many values");
  }
  
}

void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  // TODO: implement writing image to file
  ofstream outputFS(filename);
  if(!outputFS.is_open()){
    throw std::invalid_argument("Failed to open " + filename);
  }
  outputFS << "P3" << "\n";
  outputFS << width << " " << height << "\n";
  outputFS << 255 << "\n";
  for (unsigned int row = 0; row < height; row++) {
    for (unsigned int col = 0; col < width; col++) {
      outputFS << image[col][row].r << " " << image[col][row].g << " " << image[col][row].b << " ";
    }
  }
}

double map_coordinates(unsigned int source_dimension, unsigned int target_dimension, unsigned int pixel_coordinate){
  // TODO: implement mapping function.
  double output = (source_dimension-1.0)/(target_dimension-1.0) * pixel_coordinate;
  return output;
}


Pixel bilinear_interpolation(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, double x, double y) {
  // TODO: Implement bilinear interpolation
  unsigned int x_floor = floor(x);
  unsigned int x_ceil = ceil(x);
  unsigned int y_floor = floor(y);
  unsigned int y_ceil = ceil(y);
  if(x_ceil>=width || y_ceil>=height){
    x_ceil=width-1;
    y_ceil=height-1;
  }

  // ----------------------- Edge cases --------------------------------

  if(x_ceil==x_floor){
    if(x_ceil+1<width){
      x_ceil+=1;
    }
  }
  if(y_ceil==y_floor){
    if(y_ceil+1<height){
      y_ceil+=1;
    }
  }

  //--------------------- CORNER CASES ---------------------------------
  //(0, 0), (0, height - 1), (width-1, 0), (height-1, width-1)
  
  //--------------------- CALCULATION ----------------------------------

  Pixel P1 = image[x_floor][y_floor];
  Pixel P2 = image[x_ceil][y_floor];
  Pixel P3 = image[x_floor][y_ceil];
  Pixel P4 = image[x_ceil][y_ceil];

  if(x_ceil==x_floor&&y_ceil==y_floor){
    return P1;
  }
  if(x_ceil==x_floor){
    double rA = (y_ceil - y) * P1.r + (y - y_floor) * P3.r;
    double gA = (y_ceil - y) * P1.g + (y - y_floor) * P3.g;
    double bA = (y_ceil - y) * P1.b + (y - y_floor) * P3.b;
    short rAvg = round(rA);
    short bAvg = round(bA);
    short gAvg = round(gA);
    return {rAvg, gAvg, bAvg};
  }
  if(y_ceil==y_floor){
    //not going into this branch when y is an integer
    double r1 = (x_ceil - x) * P3.r + (x - x_floor) * P4.r;
    double g1 = (x_ceil - x) * P3.g + (x - x_floor) * P4.g;
    double b1 = (x_ceil - x) * P3.b + (x - x_floor) * P4.b;
    short rAvg = round(r1);
    short bAvg = round(b1);
    short gAvg = round(g1);
    return {rAvg, gAvg, bAvg};
  }

  double r1 = (P2.r)*(x - x_floor) + (P1.r)*(x_ceil - x);
  double r2 = (P4.r)*(x - x_floor) + (P3.r)*(x_ceil - x);
  double rA = (r1)*(y_ceil - y) + (r2)*(y - y_floor);

  double b1 = (P2.b)*(x - x_floor) + (P1.b)*(x_ceil - x);
  double b2 = (P4.b)*(x - x_floor) + (P3.b)*(x_ceil - x);
  double bA = (b1)*(y_ceil - y) + (b2)*(y - y_floor);

  double g1 = (P2.g)*(x - x_floor) + (P1.g)*(x_ceil - x);
  double g2 = (P4.g)*(x - x_floor) + (P3.g)*(x_ceil - x);
  double gA = (g1)*(y_ceil - y) + (g2)*(y - y_floor);

  short rAvg = round(rA);
  short bAvg = round(bA);
  short gAvg = round(gA);
  return {rAvg, gAvg, bAvg};

}

void scale_image(Pixel sourceImage[][MAX_HEIGHT], unsigned int sourceWidth, unsigned int sourceHeight,
                   Pixel targetImage[][MAX_HEIGHT], unsigned int targetWidth, unsigned int targetHeight){
  // TODO: add loops to calculate scaled images
  for (unsigned int col = 0; col < targetWidth; col++) {
    // iterate through rows
    for (unsigned int row = 0; row < targetWidth; row++) {
      // initialize pixel
      double x_coord = map_coordinates(sourceWidth, targetWidth, col);
      double y_coord = map_coordinates(sourceHeight, targetHeight, row);
      targetImage[col][row] = bilinear_interpolation(sourceImage, sourceWidth, sourceHeight, x_coord, y_coord);
    }
  }

}