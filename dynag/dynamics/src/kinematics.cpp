#include "kinematics.h"

namespace kinematics
{
    // Temporary variables for kinematics calculations.
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> mPsiRot = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>::Zero();
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> mThetaRot = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>::Zero();
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> mPhiRot = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>::Zero();
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> DCM = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>::Zero();
    Eigen::Quaternionf qW, qQ;
    Eigen::Vector4f vNormQuat;
    Eigen::Vector3f vSin;
    Eigen::Vector3f vCos;
}

void euler2DCM(Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& mTarget, const Eigen::Vector3f& vEulerAngles)
{
    kinematics::vSin = vEulerAngles.array().sin();
	kinematics::vCos = vEulerAngles.array().cos();

    kinematics::mPsiRot(0, 0) =  kinematics::vCos[2];
    kinematics::mPsiRot(0, 1) =  kinematics::vSin[2];
    kinematics::mPsiRot(0, 2) =  0.0f;
    kinematics::mPsiRot(1, 0) = -kinematics::vSin[2];
    kinematics::mPsiRot(1, 1) =  kinematics::vCos[2];
    kinematics::mPsiRot(1, 2) =  0.0f;
    kinematics::mPsiRot(2, 0) =  0.0f;
    kinematics::mPsiRot(2, 1) =  0.0f;
    kinematics::mPsiRot(2, 2) =  1.0f;

    kinematics::mThetaRot(0, 0) =  kinematics::vCos[1];
    kinematics::mThetaRot(0, 1) =  0.0f;
    kinematics::mThetaRot(0, 2) = -kinematics::vSin[1];
    kinematics::mThetaRot(1, 0) =  0.0f;
    kinematics::mThetaRot(1, 1) =  1.0f;
    kinematics::mThetaRot(1, 2) =  0.0f;
    kinematics::mThetaRot(2, 0) =  kinematics::vSin[1];
    kinematics::mThetaRot(2, 1) =  0.0f;
    kinematics::mThetaRot(2, 2) =  kinematics::vCos[1];

    kinematics::mPhiRot(0, 0) =  1.0f;
    kinematics::mPhiRot(0, 1) =  0.0f;
    kinematics::mPhiRot(0, 2) =  0.0f;
    kinematics::mPhiRot(1, 0) =  0.0f;
    kinematics::mPhiRot(1, 1) =  kinematics::vCos[0];
    kinematics::mPhiRot(1, 2) =  kinematics::vSin[0];
    kinematics::mPhiRot(2, 0) =  0.0f;
    kinematics::mPhiRot(2, 1) = -kinematics::vSin[0];
    kinematics::mPhiRot(2, 2) =  kinematics::vCos[0];

    mTarget = kinematics::mPhiRot * kinematics::mThetaRot * kinematics::mPsiRot;
}

void DCM2Euler(Eigen::Vector3f& vEulerAngles, const Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& DCM)
{
    vEulerAngles(2) = std::atan2(DCM(0, 1), DCM(0, 0));
    vEulerAngles(1) = std::asin(-DCM(0, 2));
    vEulerAngles(0) = std::atan2(DCM(1, 2), DCM(2, 2));
}

void pqr2EulerDot(Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& mTarget, const Eigen::Vector3f& vEulerAngles)
{    
    kinematics::vSin = vEulerAngles.array().sin();
    kinematics::vCos = vEulerAngles.array().cos();

    mTarget(0, 0) = 1.0f;
    mTarget(0, 1) = kinematics::vSin[0] * kinematics::vSin[1] / (kinematics::vCos[1] + 1e-7f);
    mTarget(0, 2) = kinematics::vCos[0] * kinematics::vSin[1] / (kinematics::vCos[1] + 1e-7f);

    mTarget(1, 0) = 0.0f;
    mTarget(1, 1) =  kinematics::vCos[0];
    mTarget(1, 2) = -kinematics::vSin[0];

    mTarget(2, 0) = 0.0f;
    mTarget(2, 1) = kinematics::vSin[0] / (kinematics::vCos[1] + 1e-7f);
    mTarget(2, 2) = kinematics::vCos[0] / (kinematics::vCos[1] + 1e-7f);
}

void euler2Quaternion(Eigen::Vector<float, 4>& vQuat, const Eigen::Vector3f& vEulerAngles)
{
    Eigen::AngleAxisf yaw(vEulerAngles[2], Eigen::Vector3f::UnitZ());
    Eigen::AngleAxisf pitch(vEulerAngles[1], Eigen::Vector3f::UnitY());
    Eigen::AngleAxisf roll(vEulerAngles[0], Eigen::Vector3f::UnitX());

    kinematics::qQ = yaw * pitch * roll;
    vQuat = kinematics::qQ.coeffs();
}           

void pqr2QuaternionDot(Eigen::Vector<float, 4>& vQuatDot, const Eigen::Vector<float, 4>& vQuat, const Eigen::Vector3f& vPQR)
{   
    kinematics::qW.x() = vPQR(0);
    kinematics::qW.y() = vPQR(1);
    kinematics::qW.z() = vPQR(2);
    kinematics::qW.w() = 0.0f;

    kinematics::qQ.x() = vQuat(0);
    kinematics::qQ.y() = vQuat(1);
    kinematics::qQ.z() = vQuat(2);
    kinematics::qQ.w() = vQuat(3);

    vQuatDot = 0.5f * (kinematics::qQ * kinematics::qW).coeffs();
}


void quaternion2DCM(Eigen::Matrix<float, 3, 3, Eigen::RowMajor>& mTarget, const Eigen::Vector<float, 4>& vQuat)
{
    kinematics::vNormQuat = vQuat.normalized();
    mTarget(0, 0) = kinematics::vNormQuat(3) * kinematics::vNormQuat(3) + kinematics::vNormQuat(0) * kinematics::vNormQuat(0) - kinematics::vNormQuat(1) * kinematics::vNormQuat(1) - kinematics::vNormQuat(2) * kinematics::vNormQuat(2);
    mTarget(0, 1) = 2.0f * (kinematics::vNormQuat(0) * kinematics::vNormQuat(1) + kinematics::vNormQuat(3) * kinematics::vNormQuat(2));
    mTarget(0, 2) = 2.0f * (kinematics::vNormQuat(0) * kinematics::vNormQuat(2) - kinematics::vNormQuat(3) * kinematics::vNormQuat(1));
    
    mTarget(1, 0) = 2.0f * (kinematics::vNormQuat(0) * kinematics::vNormQuat(1) - kinematics::vNormQuat(3) * kinematics::vNormQuat(2));
    mTarget(1, 1) = kinematics::vNormQuat(3) * kinematics::vNormQuat(3) - kinematics::vNormQuat(0) * kinematics::vNormQuat(0) + kinematics::vNormQuat(1) * kinematics::vNormQuat(1) - kinematics::vNormQuat(2) * kinematics::vNormQuat(2);
    mTarget(1, 2) = 2.0f * (kinematics::vNormQuat(1) * kinematics::vNormQuat(2) + kinematics::vNormQuat(3) * kinematics::vNormQuat(0));
    
    mTarget(2, 0) = 2.0f * (kinematics::vNormQuat(0) * kinematics::vNormQuat(2) + kinematics::vNormQuat(3) * kinematics::vNormQuat(1));
    mTarget(2, 1) = 2.0f * (kinematics::vNormQuat(1) * kinematics::vNormQuat(2) - kinematics::vNormQuat(3) * kinematics::vNormQuat(0));
    mTarget(2, 2) = kinematics::vNormQuat(3) * kinematics::vNormQuat(3) - kinematics::vNormQuat(0) * kinematics::vNormQuat(0) - kinematics::vNormQuat(1) * kinematics::vNormQuat(1) + kinematics::vNormQuat(2) * kinematics::vNormQuat(2);
}

void quaternion2Euler(Eigen::Vector3f& vEulerAngles, const Eigen::Vector<float, 4>& vQuat)
{
    quaternion2DCM(kinematics::DCM, vQuat);
    DCM2Euler(vEulerAngles, kinematics::DCM);
}

void unitQuaternion2Euler(Eigen::Vector3f& vEulerAngles, const Eigen::Vector<float, 4>& vQuat)
{
    kinematics::vNormQuat = vQuat.normalized();
    float sqx = kinematics::vNormQuat(0) * kinematics::vNormQuat(0);
    float sqy = kinematics::vNormQuat(1) * kinematics::vNormQuat(1);
    float sqz = kinematics::vNormQuat(2) * kinematics::vNormQuat(2);
    float sqw = kinematics::vNormQuat(3) * kinematics::vNormQuat(3);

    vEulerAngles(0) = std::atan2(2.0f * (kinematics::vNormQuat(1) * kinematics::vNormQuat(2) + kinematics::vNormQuat(3) * kinematics::vNormQuat(0)), sqw + sqz - sqx - sqy);
    vEulerAngles(1) = -std::asin(2.0f * (kinematics::vNormQuat(0) * kinematics::vNormQuat(2) - kinematics::vNormQuat(3) * kinematics::vNormQuat(1)));
    vEulerAngles(2) = std::atan2(2.0f * (kinematics::vNormQuat(0) * kinematics::vNormQuat(1) + kinematics::vNormQuat(3) * kinematics::vNormQuat(2)), sqw + sqx - sqy - sqz);
}