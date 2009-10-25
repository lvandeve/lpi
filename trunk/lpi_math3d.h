/*
Copyright (c) 2005-2008 Lode Vandevenne
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
  Vector3(const Vector3& other);
  
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
  void clamp(double value); //set maximum length to value
  
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
Vector3 operator-(const Vector3& v, const Vector3& w); //a - b is b----->a
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

static const Vector3 Vector3_origin = Vector3(0.0, 0.0, 0.0);
static const Vector3 Vector3_0 =      Vector3(0.0, 0.0, 0.0);
static const Vector3 Vector3_x =      Vector3(1.0, 0.0, 0.0);
static const Vector3 Vector3_y =      Vector3(0.0, 1.0, 0.0);
static const Vector3 Vector3_z =      Vector3(0.0, 0.0, 1.0);

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

static const Matrix3 Matrix3_identity(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
static const Matrix3 Matrix3_unit(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
static const Matrix3 Matrix3_invZ(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0);

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
  const Vector3& getU() const; //the "right" vector of the camera, x coordinate of screen
  const Vector3& getV() const; //the "up" vector of the camera, y coordinate of screen
  const Vector3& getDir() const; //the direction of the camera, vector that points into the screen (z direction)
  const Vector3& getPos() const; //the position of the camera in world coordinates
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
  double getDist(const Vector3& point) const;
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
  double getFOVU(double w, double h) const;
  double getFOVV(double w, double h) const;
  void setFOVU(double angle); //angle given in radians
  void setFOVV(double angle); //angle given in radians
  void setFOV(double angle) //angle given in radians
  {
    setFOVU(angle);
    setFOVV(angle);
  }
  
  //get and set pitch, yaw and roll (these are NOT native parameters of the camera and should normally never be needed!)   
  double getYaw() const;
  double getPitch() const;
  double getRoll() const;
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
  double getRatioUV() const;
  double getRatioVU() const;
  void setRatioUV(double ratio);
  void setRatioVU(double ratio);
  
  //scale U, V and Dir without changing what you see
  double getScale() const;
  void setScale(double dirLength);
  void scale(double factor);
  
  //generate, get and use the camera matrix to transform points
  void generateMatrix();
  const Matrix3& getMatrix() const;
  Matrix3 getInvMatrix() const;
  void setMatrix(const Matrix3& matrix);   //sets the u, v and dir vector to given matrix (and generates the actual matrix too of course)
  Vector3 transformWithoutPos(const Vector3& v) const;
  Vector3 transform(const Vector3& v) const; //subtract position from v and transform with the matrix
  
  template<typename T>
  bool projectOnScreen(const Vector3& point, T& x, T& y, double& z, int w, int h) const
  {
    Vector3 a = transform(point); //Transform to camera space
    z = a.z;
    return camSpaceToScreen(a, x, y, w, h); //Project on screen
  }
  
  template<typename T>
  bool projectOnScreenWithoutPos(const Vector3& point, T& x, T& y, double& z, int w, int h) const
  {
    Vector3 b = transformWithoutPos(point); //the transformation without position translation
    z = b.z;
    return camSpaceToScreen(b, x, y, w, h); //Projection on screen
  }
  
  template<typename T>
  bool projectOnScreen(const Vector3& point, T& x, T& y, int w, int h) const
  {
    double z = 0.0;
    return projectOnScreen(point, x, y, z, w, h);
  }
  
  template<typename T>
  bool projectOnScreenWithoutPos(const Vector3& point, T& x, T& y, int w, int h) const
  {
    double z = 0.0;
    return projectOnScreenWithoutPos(point, x, y, z, w, h);
  }
  
  //camSpaceToScreen does the z divide, to be called on a point in camera coordinates
  template<typename T>
  bool camSpaceToScreen(const Vector3& point, T& x, T& y, int w, int h) const
  {
    //NOTE: if point.z is too small, you get integer imprecisions, so don't make near clipping plane too small
    //NOTE: CURRENTLY THIS CODE IS SYNCED WITH THE OPENGL ENGINE. That's why both x and y work with 0.5 * h (where h is the screenheight). I have no idea why both x and y need to be multiplied with 0.5 * h to be in sync with (ie get the same screencoordinates as) the projections of the opengl engine, but it works. THe old version of this function had w for x and h for y, with no multiplication with 0.5.
    
    x = (T)(w / 2.0 + 0.5 * h * point.x / point.z);
    y = (T)(h / 2.0 - 0.5 * h * point.y / point.z); //inversed: y should be "up", while in screen coordinates it's down

    return ((x >= 0 && y >= 0 && x < w && y < h) && point.z >= nearClip);
  }
  
  protected:
  //the camera plane, described by the vectors u and v, is described by "z = 0" in camera space
  Vector3 pos; //the location of the camera
  Matrix3 trans; //the camera matrix, is nothing more than the column vectors u, v and dir in one matrix

  mutable Matrix3 invCamMatrix; //the inverse of the camera matrix
  mutable bool matrixUpToDate;
  mutable bool invMatrixUpToDate;
};


//Auxiliary functions

double radToDeg(double rad); //swap between radians and degrees
double degToRad(double deg);
Vector3 normalOfTriangle(const Vector3& a, const Vector3& b, const Vector3& c); //calculate normal of a triangle
Vector3 projectOnPlane(const Vector3& v, const Vector3& n); //project vector v on the plane of which n is the normal
double rotationAroundAxis(const Vector3& a, const Vector3& b, const Vector3& axis);
//precondition of getComponentInDirection: dir must be a normalized vector!
Vector3 getComponentInDirection(const Vector3& v, const Vector3& dir);
bool hasOppositeDirection(const Vector3& v, const Vector3& w);
bool sideOfPlaneGivenByNormal(const Vector3& p, const Vector3& n);
bool sideOfPlaneGivenByThreePoints(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);
bool sideOfPlaneGivenByTwoVectors(const Vector3& p, const Vector3& X, const Vector3& Y);
double angleWithPlaneGivenByTwoVectors(const Vector3& v, const Vector3& X, const Vector3& Y);
double distance(const Vector3& p, const Vector3& a, const Vector3& b); //get distance from point p to the line given by a and b
double distance(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c); //get distance from point p to the plane given by a, b and c
bool linesegmentThroughSphere(const Vector3& a, const Vector3& b, const Vector3& p, double radius);
Vector3 linePlaneIntersection(const Vector3& a, const Vector3& b, const Vector3& p, const Vector3& q, const Vector3& r);
void planeEquation(const Vector3& a, const Vector3& b, const Vector3& c, double& u, double& v, double& w, double& t);
Vector3 getSomePerpendicularVector(const Vector3& v);
Vector3 barycentric(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& p); //barycentric coordinates on 3D triangle, not for rasterizing
void barycentric_persp(double& alpha, double& beta, double& gamma, /*double alpha2,*/ double beta2, double gamma2, double za, double zb, double zc); //for rasterizing textured triangle
Vector3 rotateWithPlane(const Vector3& p, const Vector3& n);
Vector3 planeSphereTangent(const Vector3& p, const Vector3& o, double radius, const Vector3& v);


/*
deflect, aka Target Leading or Target Prediction System.
Calculates what direction to shoot a slow moving bullet at to make it hit a moving target.
The result is stored in dir.
Return value is true if a solution was found, false if there wasn't a solution, e.g. if the velocity is too high for a bullet of speed to handle.
vel is targetvelocity - shootervelocity.
speed is speed of the slow moving bullet.
*/
bool deflect(Vector3& dir, const Vector3& shooterpos, const Vector3& targetpos, const Vector3& vel, double speed);

} //end of namespace lpi

#endif //math3d_included
