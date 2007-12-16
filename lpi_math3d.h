/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LPI_MATH3D_H_INCLUDED
#define LPI_MATH3D_H_INCLUDED

#include <cmath>
#include <iostream>

namespace lpi
{
////////////////////////////////////////////////////////////////////////////////
//                  [ x ]                                                     //
// 3D Vector Class  [ y ]                                                     //
//                  [ z ]                                                     //
////////////////////////////////////////////////////////////////////////////////
class Vector3 //memory: exactly 3 doubles
{
  public:
  
  double x;
  double y;
  double z;
  
  const double& operator[](int i) const { return *(&x + i); }
  double& operator[](int i) { return *(&x + i); }
  
  Vector3();
  Vector3(double x, double y, double z);
  
  double length() const;
  double lengthsq() const;
  double manhattan() const; //manhattan distance
  void normalize(); //normalizes the vector, but lets it remain the 0 vector if it's length is 0
  double distance(const Vector3& v);
  double distancesq(const Vector3& v);
  double manhattan(const Vector3& v); //manhattan distance
  double dot(const Vector3& v);
  Vector3 cross(const Vector3& v);
  void negate();
  
  Vector3& operator+=(const Vector3& v);
  Vector3& operator-=(const Vector3& v);
  Vector3& operator*=(double a);
  Vector3& operator/=(double a);
};

double length(const Vector3& v);
double lengthsq(const Vector3& v);
double manhattan(const Vector3& v); //manhattan distance
Vector3 normalize(const Vector3& v);
double distance(const Vector3& v, const Vector3& w);
double distancesq(const Vector3& v, const Vector3& w);
double manhattan(const Vector3& v, const Vector3& w); //manhattan distance
double dot(const Vector3& v, const Vector3& w);
Vector3 cross(const Vector3& v, const Vector3& w);
Vector3 operator-(const Vector3& v, const Vector3& w);
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v, const Vector3& w);
Vector3 operator*(const Vector3& v, double a);
Vector3 operator*(double a, const Vector3& v);
Vector3 operator/(const Vector3& v, double a);
double vectorAngle(const Vector3& v, const Vector3& w);
double vectorAngleInDirection(const Vector3& v, const Vector3& w, const Vector3& n);
Vector3 rotateAroundArbitrary(const Vector3& v, const Vector3& axis, double angle);
bool operator==(const Vector3& a, const Vector3& b);
bool operator!=(const Vector3& a, const Vector3& b);

std::ostream& operator<<(std::ostream& ostr, const Vector3& v);

#define Vector3_origin Vector3(0.0, 0.0, 0.0)
#define Vector3_0 Vector3(0.0, 0.0, 0.0)
#define Vector3_x Vector3(1.0, 0.0, 0.0)
#define Vector3_y Vector3(0.0, 1.0, 0.0)
#define Vector3_z Vector3(0.0, 0.0, 1.0)

////////////////////////////////////////////////////////////////////////////////
//               [ 0 3 6 ]                                                    //
// Matrix Class  [ 1 4 7 ]                                                    //
//               [ 2 5 8 ]                                                    //
////////////////////////////////////////////////////////////////////////////////
class Matrix3 //memory: exactly 9 doubles
{
  public:
    
  Vector3 a[3];
  
  const Vector3& operator[](int i) const { return a[i]; }
  Vector3& operator[](int i) { return a[i]; }
  
  Matrix3(double a0, double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8);
  Matrix3();
  
  void transpose();
  double determinant() const;
  void invert();
  
  Matrix3& operator+=(const Matrix3& rhs);
  Matrix3& operator-=(const Matrix3& rhs);
  Matrix3& operator*=(double f);
  Matrix3& operator/=(double f);
  Matrix3& operator*=(const Matrix3& rhs);
};

#define Matrix3_identity Matrix3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)
#define Matrix3_unit Matrix3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)
#define Matrix3_invZ Matrix3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0)

Matrix3 transpose(const Matrix3& A);
double determinant(const Matrix3& A);
Matrix3 inverse(const Matrix3& A);
Matrix3 operator+(const Matrix3& A, const Matrix3& B);
Matrix3 operator-(const Matrix3& A, const Matrix3& B);
Matrix3 operator*(const Matrix3& A, double a);
Matrix3 operator*(double a, const Matrix3& A);
Matrix3 operator/(const Matrix3& A, double a);
Vector3 operator*(const Matrix3& A, const Vector3& v);
//Vector3 operator*(const Vector3& v, Matrix3 A);
Matrix3 operator*(const Matrix3& A, const Matrix3& B);

std::ostream& operator<<(std::ostream& ostr, const Matrix3& m);

Matrix3 getRotationMatrix3(const Vector3& axis, double angle);
Matrix3 rotateAroundArbitrary(const Matrix3& m, const Vector3& axis, double angle);

////////////////////////////////////////////////////////////////////////////////
//                        [ u.x v.x dir.x ]   [ pos.x ]                       //
// Transformation3 Class  [ u.y v.y dir.y ] + [ pos.y ]                       //
//                        [ u.z v.z dir.z ]   [ pos.z ]                       //
//                                                                            //
// This class can also be used as Camera                                      //
////////////////////////////////////////////////////////////////////////////////
class Transformation3
{

  public:
  
  enum PlanetGroundPlaneType
  {
    /*
    In most engines XZ is the ground plane of the planet and Y is the up vector
    Sometimes that's confusing and you prefer XY as ground plane with Z as up vector
    */
    XZ,
    XY
  };
  
  PlanetGroundPlaneType planetGroundPlaneType;
  
  
  double nearClip; //near clipping plane
  double farClip; //far clipping plane

  Transformation3();
  Transformation3(double posx, double posy, double posz,
                  double ux,   double uy,   double uz,
                  double vx,   double vy,   double vz,
                  double dirx, double diry, double dirz,
                  double nearClip = 0.0, double farClip = 0.0);

  //Get and Set the 4 important vectors 
  Vector3& getU(); //the "right" vector of the camera, x coordinate of screen
  Vector3& getV(); //the "up" vector of the camera, y coordinate of screen
  Vector3& getDir(); //the direction of the camera, vector that points into the screen (z direction)
  Vector3& getPos(); //the position of the camera in world coordinates
  void setDir(const Vector3& newDir);
  void setU(const Vector3& newY);
  void setV(const Vector3& newV);  
  void setPos(const Vector3& newPos);
  
  //move and rotate with vectors
  void move(const Vector3& offset);
  void rotate(const Vector3& axis, double angle);
  void rotate(const Vector3& a, const Vector3& b, double angle);
  void setLookDir(const Vector3& newDir);
  void lookAt(const Vector3& lookAtMe);
  void setGroundPlane(const Vector3& n);
  
  //get and set distance to a certain point
  double getDist(const Vector3& point);
  void setDist(const Vector3& point, double dist);
  
  //get and set zoom
  double getZoomU() const;
  double getZoomV() const;
  void setZoomU(double a);
  void setZoomV(double a); 
  void zoom(double a);
  void zoomU(double a);
  void zoomV(double a);
    
  //get and set FOV
  double getFOVU(double w, double h);
  double getFOVV(double w, double h);
  void setFOVU(double angle);
  void setFOVV(double angle);
  void setFOV(double angle)
  {
    setFOVU(angle);
    setFOVV(angle);
  }
  
  //get and set pitch, yaw and roll (these are NOT native parameters of the camera and should normally never be needed!)   
  double getYaw();
  double getPitch();
  double getRoll();
  void setYaw(double angle);
  void setPitch(double angle);
  void setRoll(double angle);
  void yawPlanet(double angle);
  void yawSpace(double angle);
  void pitch(double angle);
  void roll(double angle);
   
  //make camera orthogonal (reset skewing)
  void resetSkewU();
  void resetSkewV();
  
  //set screen ratio of the camera (ratio of length of u and v, e.g. 4:3, 16:9, 640:480, ...)
  double getRatioUV();
  double getRatioVU();
  void setRatioUV(double ratio);
  void setRatioVU(double ratio);
  
  //scale U, V and Dir without changing what you see
  double getScale();
  void setScale(double dirLength);
  void scale(double factor);
  
  //generate, get and use the camera matrix to transform points
  void generateMatrix();
  const Matrix3& getMatrix();
  Matrix3 getInvMatrix();
  void setMatrix(const Matrix3& matrix);   //sets the u, v and dir vector to given matrix (and generates the actual matrix too of course)
  Vector3 transform(const Vector3& v);
  Vector3 transformPos(const Vector3& v);
  bool projectOnScreen(const Vector3& point, int& x, int& y, double& z, int w, int h);
  bool projectOnScreen(const Vector3& point, int& x, int& y, int w, int h);
  bool transformOnScreen(const Vector3& point, int& x, int& y, double& z, int w, int h);
  bool transformOnScreen(const Vector3& point, int& x, int& y, int w, int h);
  bool camSpaceToScreen(const Vector3& point, int& x, int& y, int w, int h);
  
  protected:
  //the camera plane, described by the vectors u and v, is described by "z = 0" in camera space
  Vector3 pos; //the location of the camera
  Matrix3 trans; //the camera matrix, is nothing more than the column vectors u, v and dir in one matrix

  Matrix3 invCamMatrix; //the inverse of the camera matrix
  bool matrixUpToDate;
  bool invMatrixUpToDate;
};


//Auxiliary functions

double radToDeg(double rad); //swap between radians and degrees
double degToRad(double deg);
Vector3 normalOfTriangle(const Vector3& a, const Vector3& b, const Vector3& c); //calculate normal of a triangle
Vector3 projectOnPlane(const Vector3& v, const Vector3& n); //project vector v on the plane of which n is the normal
double rotationAroundAxis(const Vector3& a, const Vector3& b, const Vector3& axis);
Vector3 getComponentInDirection(const Vector3& v, const Vector3& dir);
bool hasOppositeDirection(const Vector3& v, const Vector3& w);
bool sideOfPlaneGivenByNormal(const Vector3& p, const Vector3& n);
bool sideOfPlaneGivenByThreePoints(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);
bool sideOfPlaneGivenByTwoVectors(const Vector3& p, const Vector3& X, const Vector3& Y);
double angleWithPlaneGivenByTwoVectors(const Vector3& v, const Vector3& X, const Vector3& Y);
double distance(const Vector3& p, const Vector3& a, const Vector3& b);
double distance(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);
bool linesegmentThroughSphere(const Vector3& a, const Vector3& b, const Vector3& p, double radius);
Vector3 linePlaneIntersection(const Vector3& a, const Vector3& b, const Vector3& p, const Vector3& q, const Vector3& r);
void planeEquation(const Vector3& a, const Vector3& b, const Vector3& c, double& u, double& v, double& w, double& t);
Vector3 getSomePerpendicularVector(const Vector3& v);
Vector3 barycentric(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& p);
Vector3 rotateWithPlane(const Vector3& p, const Vector3& n);
Vector3 planeSphereTangent(const Vector3& p, const Vector3& o, double radius, const Vector3& v);

} //end of namespace lpi

#endif //math3d_included
