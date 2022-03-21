/*
* DynaG Dynamics C++ / Kinematics
*
* Kinematics is an helper header for kinematic functions.
*
* Author : @MGokcayK
*
* C.Date : 17/09/2021
* Update : 17/09/2021 - Create & Implementation // @MGokcayK
* Update : 23/11/2020 - Adding Quaternion Implementations // @MGokcayK
*/

/*
* DIFFERENCE BETWEEN DIRECTION COSINE MATRIX (DCM) & ROTATION MATRIX (ROTMAT)
* 
* Rotmat rotates the point in fixed frame. On the other hand, DCM rotates the 
* frame. This make the difference between them. As a mathematical expression,
* DCM is a conjugate of Rotmat.
*/

#pragma once

#include <Eigen/Eigen/Geometry>
#include <Eigen/Eigen/Dense>
#include <iostream>
#include "utils.h"

// Calculate DCM from Euler angles.
void euler2DCM(Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& mTarget, const Eigen::Vector3f& vEulerAngles);

// Calculate Euler angles from DCM.
void DCM2Euler(Eigen::Vector3f& vEulerAngles, const Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& DCM);

// Calculate Euler angle derivatives.
void pqr2EulerDot(Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& mTarget, const Eigen::Vector3f& vEulerAngles);

// Convert Euler angles to Quaternion.
void euler2Quaternion(Eigen::Vector<float, 4>& vQuat, const Eigen::Vector3f& vEulerAngles);

// Calculate quaternion derivatives w.r.t pqr.
void pqr2QuaternionDot(Eigen::Vector<float, 4>& vQuatDot, const Eigen::Vector<float, 4>& vQuat, const Eigen::Vector3f& vPQR);

// Calculate DCM from Quaternions
void quaternion2DCM(Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& mTarget, const Eigen::Vector<float, 4>& vQuat);

// Calculate Euler angles from quaternion.
void quaternion2Euler(Eigen::Vector3f& vEulerAngles, const Eigen::Vector<float, 4>& vQuat);

// Calculate Euler angles from unit quaternion.
void unitQuaternion2Euler(Eigen::Vector3f& vEulerAngles, const Eigen::Vector<float, 4>& vQuat);

