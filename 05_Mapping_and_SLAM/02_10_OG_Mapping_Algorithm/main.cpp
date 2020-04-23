// Compile with:
// g++ main.cpp -o app -std=c++11 -I/usr/include/python2.7 -lpython2.7
#include "src/matplotlibcpp.h"  //Graph Library
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;
namespace plt = matplotlibcpp;

// Sensor characteristic: Min and Max ranges of the beams
double Zmax = 5000, Zmin = 170;
// Defining free cells(lfree), occupied cells(locc), unknown cells(l0) log odds
// values
double l0 = 0, locc = 0.4, lfree = -0.4;
// Grid dimensions
double gridWidth = 100, gridHeight = 100;
// Map dimensions
double mapWidth = 30000, mapHeight = 15000;
// Robot size with respect to the map
double robotXOffset = mapWidth / 5, robotYOffset = mapHeight / 3;
// Defining an l vector to store the lod odds values of each cell
std::vector<std::vector<double>> l(mapWidth / gridWidth,
                                   std::vector<double>(mapHeight / gridHeight));

double inverseSensorModel(double x, double y, double theta, double xi,
                          double yi, double sensorData[]) {
  // You will be coding this section in the upcoming concept

  // Defining sensor characteristics
  double Zk, thetaK, sensorTheta;
  double minDelta = -1;
  double alpha = 200, beta = 20;

  // compute r and phi
  double r = sqrt(pow(xi - x, 2) + pow(yi - y, 2));
  double phi = atan2(yi - y, xi - x) - theta;

  // Scaling measurement to [-90 -37.5 -22.5 -7.5 7.5 22.5 37.5 90]
  for (int i = 0; i < 8; i++) {
    if (i == 0) {
      sensorTheta = -90 * (M_PI / 180);
    } else if (i == 1) {
      sensorTheta = -37.5 * (M_PI / 180);
    } else if (i == 6) {
      sensorTheta = 37.5 * (M_PI / 180);
    } else if (i == 7) {
      sensorTheta = 90 * (M_PI / 180);
    } else {
      sensorTheta = (-37.5 + (i - 1) * 15) * (M_PI / 180);
    }

    // we are finding the measurement or grid lies in front of which sensor
    if (fabs(phi - sensorTheta) < minDelta || minDelta == -1) {
      Zk = sensorData[i];
      thetaK = sensorTheta;
      minDelta = fabs(phi - sensorTheta);
    }
  }

  // Evaluate the three cases
  if (r > min((double)Zmax, Zk + alpha / 2) || fabs(phi - thetaK) > beta / 2 ||
      Zk > Zmax || Zk < Zmin) {
    return l0;
  } else if (Zk < Zmax && fabs(r - Zk) < alpha / 2) {
    return locc;
  } else if (r <= Zk) {
    return lfree;
  }
}

void occupancyGridMapping(double Robotx, double Roboty, double Robottheta,
                          double sensorData[]) {
  // 1 - TODO: Generate a grid (size 300x150) and then loop through all the
  // cells
  // 2- TODO: Compute the center of mass of each cell xi and yi double xi = x *
  // gridWidth + gridWidth / 2 - robotXOffset; double yi = -(y *gridHeight +
  // gridHeight / 2) + robotYOffset;
  // 3- TODO: Check if each cell falls under the perceptual field of the
  // measurements

  for (int x = 0; x < mapWidth / gridWidth; x++) {
    for (int y = 0; y < mapHeight / gridHeight; y++) {
      double xi = x * gridWidth + gridWidth / 2 - robotXOffset;
      double yi = -(y * gridHeight + gridHeight / 2) + robotYOffset;
      if (sqrt(pow(xi - Robotx, 2) + pow(yi - Roboty, 2)) <= Zmax) {
        l[x][y] =
            l[x][y] +
            inverseSensorModel(Robotx, Roboty, Robottheta, xi, yi, sensorData) -
            l0;
      }
    }
  }
}

void visualization() {
  // TODO: Initialize a plot named Map of size 300x150
  plt::title("Occupancy Grid Mapping");
  plt::xlim(0.0, mapWidth / gridWidth);
  plt::ylim(0.0, mapHeight / gridHeight);

  // TODO: Loop over the log odds values of the cells and plot each cell state.
  // Unkown state: green color, occupied state: black color, and free state: red
  // color
  for (double x = 0; x < mapWidth / gridWidth; x++) {
    for (double y = 0; y < mapHeight / gridHeight; y++) {
      if (l[x][y] == 0) {
        plt::plot({x}, {y}, "g.");  // Unkown state: green color
      } else if (l[x][y] > 0) {
        plt::plot({x}, {y}, "k.");  // Occupied state: black color
      } else {
        plt::plot({x}, {y}, "r.");  // Free state: red color
      }
    }
  }

  // TODO: Save the image and close the plot
  plt::save("./Images/Map.png");
  plt::clf();
}

int main() {
  double timeStamp;
  double measurementData[8];
  double robotX, robotY, robotTheta;

  FILE* posesFile = fopen("poses.txt", "r");
  FILE* measurementFile = fopen("measurement.txt", "r");

  // Scanning the files and retrieving measurement and poses at each timestamp
  while (fscanf(posesFile, "%lf %lf %lf %lf", &timeStamp, &robotX, &robotY,
                &robotTheta) != EOF) {
    fscanf(measurementFile, "%lf", &timeStamp);
    for (int i = 0; i < 8; i++) {
      fscanf(measurementFile, "%lf", &measurementData[i]);
    }
    occupancyGridMapping(robotX, robotY, (robotTheta / 10) * (M_PI / 180),
                         measurementData);
  }

  // Displaying the map
  for (int x = 0; x < mapWidth / gridWidth; x++) {
    for (int y = 0; y < mapHeight / gridHeight; y++) {
      std::cout << l[x][y] << " ";
    }
  }
  std::cout << "\n ";

  // Visualize the map
  visualization();
  return 0;
}