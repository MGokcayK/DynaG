/*
* DynaG Dynamics C++ / Helicopter Dynamics
*
* Helicopter Dynamics is a class for describing helicopter dynamics 
* which based on DynamicSystem class. 
*
* Author : @MGokcayK
* Author : @ugurcanozalp
*
* C.Date : 17/09/2021
* Update : 17/09/2021 - Class Initialization & Implementation // @MGokcayK
* Update : 20/09/2021 - Adding new methods. // @MGokcayK
* Update : 06/11/2021 - Adding map for yaml nodes. // @MGokcayK
* Update : 23/11/2021 - Adding Quaternion Implementations // @MGokcayK
* Update : 23/12/2021 - Adding Normal Map from normal map image. // @MGokcayK
*/

#pragma once

#include "dynamics.h"
#include <stb/stb_image.h>
#include "lookup.h"
#include <random>

class HelicopterDynamics : public DynamicSystem
{
private :
	static const int iNumberOfAct = 4; // number of action
	static const int iNumberOfObservation = 34; // number of observation
	std::string _observations[iNumberOfObservation] = { "POWER", "LON_AIR_SPD", "LAT_AIR_SPD", "DWN_AIR_SPD", 
		"LON_SPD", "LAT_SPD", "DWN_SPD",
		"U_ACC", "V_ACC", "W_ACC", 
		"N_VEL", "E_VEL", "DES_RATE",
		"ROLL", "PITCH", "YAW", "ROLL_RATE", "PITCH_RATE", "YAW_RATE",
		"N_POS", "E_POS", "ALTITUDE", "GROUND_ALTITUDE",
		"COLL_ANGLE", "LON_ANGLE", "LAT_ANGLE", "PED_ANGLE",
		"COLLECTIVE", "LONGITUDINAL", "LATERAL", "PEDAL", "UWIND", "VWIND", "WWIND"};
	
	// Yaml Nodes
	YAML::Node HELI, ENV, TRIM, MR, TR, FUS, HT, VT, WN, LG, FLG;

	// Trim variables
	float YAW, YAW_RATE, N_VEL, E_VEL, D_VEL, N_POS, E_POS, GR_ALT, PSI_MR, PSI_TR; 

	// Component positions
	float MR_H, MR_D, FUS_H, FUS_D, WN_H, WN_D, HT_H, HT_D, VT_H, VT_D, TR_H, TR_D;
	float MR_R, MR_K1, TR_R, MR_TWST, TR_TWST;
	float WL_CG, FS_CG;

	// Vehicle
	float M, WT; // mass
	float VTRANS; // vertical transform
	float HP_LOSS;
	
	// Gravity Constant
	float GRAV;

	// Main Rotor params
	float MR_OMEGA, MR_V_TIP, MR_FR, MR_SOL, MR_A_SIGMA;
	float MR_GAM_OM16_DRO, MR_DL_DB1, MR_DL_DA1_DRO, MR_COEF_TH;
	float MR_E, MR_IS;

	// Tail Rotor params
	float TR_OMEGA, TR_V_TIP, TR_FR, TR_SOL, TR_COEF_TH;

	// Fuselage params
	float FUS_COR, FUS_XUU, FUS_YVV, FUS_ZWW;

	// Horizontal Tail params
	float HT_ZMAX, HT_ZUU, HT_ZUW;

	// Vertical Tail params
	float VT_YMAX, VT_YUU, VT_YUV;

	// Wing params
	float WN_ZMAX, WN_ZUU, WN_ZUW;

	// Landing Gear params
	float LG_H, LG_D;
	float LG_C, LG_K;
	float LG_B; 
	float LG_MU;

	// Front Landing Gear params
	float FLG_H, FLG_D;
	float FLG_C, FLG_K;
	float FLG_MU;
	
	// Input params
	float COL_OS, COL_H, COL_L, COL_COF;
	float LON_H, LON_L, LON_COF;
	float LAT_H, LAT_L, LAT_COF;
	float PED_OS, PED_H, PED_L, PED_COF;

	// Inertia and Its Reverse;
	Eigen::Matrix<float, 3, 3, Eigen::RowMajor> I, IINV;
	
	// Wind vector in earth frame
	Eigen::Vector3f vWindNed;

	// States for reset
	VectorSpace vsInitState = { std::string{"Init State"} },
				vsInitStateDots = { std::string{"Init StateDots"} };

	// Air properties
	float fAirTemperature, fAirDensity;
	float T0, LAPSE, RO_SEA, R;

	// Terrain map props
	float NS_MAX, EW_MAX;
	float MAX_GR_ALT, MIN_GR_ALT;
	float fXPerPixel, fYPerPixel; // terrain x & y size per pixel

	// Ground height parameters, with Landing Gears
	// float fXLocation, fYLocation;
	// int iXIndex, iYIndex;
	// float fHeightMiddle, fHeightNorth, fHeightEast;
	float fGroundHeight, fGroundHeightRightLandingGear, fGroundHeightLeftLandingGear, fGroundHeightFrontLandingGear;
	Eigen::Vector<float, 3> vGroundNormal, vGroundNormalRightLandingGear, vGroundNormalLeftLandingGear, vGroundNormalFrontLandingGear;
	Eigen::Vector<float, 3> vGroundTangent, vGroundTangentRightLandingGear, vGroundTangentLeftLandingGear, vGroundTangentFrontLandingGear;

	// Maps
	unsigned short* shHeightMapPtr;
	unsigned char* cNormalMapPtr;
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mHeightMap;
	std::vector<std::vector<Eigen::Vector<float, 3>>> mNormalMap;

	// Kinematic Matrices
	Eigen::Matrix<float, 3, 3, Eigen::RowMajor> mEarth2Body, mBody2Earth, mPQR2EulerDot;
	Eigen::Vector<float, 4> vQuat, vQuatDot; 

	// // Dynamic function vectors
	Eigen::Vector<float, 3> vEulerAngles, vEulerAnglesDot, vPQR, vNEDVel, vUVW, vUVWAir, 
		vXYZ, vWeight, vGravityForce, vTotalForce, vTotalMoment, vBodyAcceleration; // vXYHObs, 
	Eigen::Vector<float, 3> vUVWDot, vPQRDot, vXYZDot;
	Eigen::Vector<float, 2> vBetas;
	Eigen::Vector<float, 1> vMainRotorVi, vTailRotorVi, vMainRotorPsi, vTailRotorPsi;
	Eigen::Vector<float, 4> vSwashInput, vSwashDeflection, vSwashRate;

	// // Dynamics Function parameters
	float fTotalPower;
	Eigen::Vector<float, 1> vTotalPowerHP, vGroundAltitude;

	// Main Rotor function parameters
	float fMainRotorPower;
	Eigen::Vector<float, 3> vMainRotorForce, vMainRotorMoment;
	Eigen::Vector<float, 2> vBetasDot;
	Eigen::Vector<float, 1> vMainRotorViDot, vMainRotorPsiDot;

	// Tail Rotor function parameters
	float fTailRotorPower;
	Eigen::Vector<float, 3> vTailRotorForce, vTailRotorMoment;
	Eigen::Vector<float, 1> vTailRotorViDot, vTailRotorPsiDot;

	// Fuselage function parameters
	float fFuselagePower;
	Eigen::Vector<float, 3> vFuselageForce, vFuselageMoment;
	
	// Horizontal Tail function parameters
	Eigen::Vector<float, 3> vHorizontalTailForce, vHorizontalTailMoment;

	// Vertical Tail function parameters
	Eigen::Vector<float, 3> vVerticalTailForce, vVerticalTailMoment;

	// Wing function parameters
	float fWingPower;
	Eigen::Vector<float, 3> vWingForce, vWingMoment;

	// Landing Gear parameters
	Eigen::Vector<float, 3> vLandingGearForce, vLandingGearMoment;
	// Left Landing Gear parameters
	Eigen::Vector<float, 3> vLeftLandingGearBodyPosition, vLeftLandingGearEarthPosition, vRightLandingGearBodyForce, vLeftLandingGearEarthForce;
	// Left Landing Gear parameters
	Eigen::Vector<float, 3> vRightLandingGearBodyPosition, vRightLandingGearEarthPosition, vLeftLandingGearBodyForce, vRightLandingGearEarthForce;
	// Front Landing Gear parameters
	Eigen::Vector<float, 3> vFrontLandingGearBodyPosition, vFrontLandingGearEarthPosition, vFrontLandingGearBodyForce, vFrontLandingGearEarthForce; 

	// Turbulence parameters
	float fEtaNorm, fTurbulenceLevel, fWindDirection, fWindSpeed, fWindRandomness, fVelocityInfinity;
	float fWindSpeed20feet, fLengthU, fLengthV, fLengthW, fSigmaU, fSigmaV, fSigmaW, fTurbulenceAzimuth;

	// Turbulence vector
	Eigen::Vector<float, 3> vVelocityInfinityNED, vTurbulenceVelocity, vWindMeanNED, vWind;
	Eigen::Vector<float, 3> vEta;
	Eigen::Vector<float, 1> vUsWind, vUsWindDot; 
	Eigen::Vector<float, 2> vVsWind, vVsWindDot;
	Eigen::Vector<float, 2> vWsWind, vWsWindDot;

	// Turbulence Exceedence Probability Lookup Table
	LookUpTable TurbulenceExceedenceProbability = LookUpTable(7, 12);

	// Trim Vectors and Matrixes;
	VectorSpace vsStatePerturbation, vsActionPerturbation;
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mJacobian, mA, mB, mC, mD, mI;
	Eigen::Vector<float, Eigen::Dynamic> vStep, vStateDotForward, vStateDotBackward, vObservationForward, vObservationBackward;
	Eigen::Vector<float, Eigen::Dynamic> vTrimOutputs, vTrimOutputsNew, vTrimOutputsForward, vTrimOutputsBackward;

	// Some calculation before starting to calculate dynamics.
	virtual void preCalculations();

	// // Registering states which update numerically in class.
	void registerStates();
 
	// Registering observations.
	void registerObservations();
 
	// Registering actions
	void registerActions();

	// Calculate air properties w.r.t altitude.
	void calculateAirProperties();

	// Calculate wind / turbulence 
	void calculateWindTurbulence();

	// Getting ground height values from heigh map of terrain.
	void getGroundHeightNormal(const Eigen::Vector<float, 3>& vLocation, float& fGroundHeight, Eigen::Vector<float, 3>& vGroundNormal);

	// Calculate main rotor force and moments.
	void calculateMainRotorForceAndMoment();

	// Calculate tail rotor force and moments.
	void calculateTailRotorForceAndMoment();

	// Calculate fuselage force and moments.
	void calculateFuselageForceAndMoment();

	// Calculate horizontal tail force and moments.
	void calculateHorizontalTailForceAndMoment();

	// Calculate vertical tail force and moments.
	void calculateVerticalTailForceAndMoment();

	// Calculate wing force and moments.
	void calculateWingForceAndMoment();

	// Calculate Landing Gear force moments
	void calculateLandingGearForceMoment();

	// Set HeightMap Matrix from Height Map Image.
	void setHeightMapMatrixFromHeightMapImage(Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& mTarget, unsigned short* shImagePtr, int& iH, int& iW);

	// Set NormalMap Matrix from Normal Map Image.
	void setNormalMapMatrixFromNormalMapImage(unsigned char* cImagePtr, int& iH, int& iW, int& iC);

	// get observations of helicopter dynamics.
	virtual void getActions();

	// get observations of helicopter dynamics.
	virtual void getStates();

	// set observations of helicopter dynamics.
	virtual void setActions();

	// set observations of helicopter dynamics.
	virtual void setStates();

	// set observations of helicopter dynamics.
	virtual void setStateDots();

	// set observations of helicopter dynamics.
	virtual void setObservations();

public :
	VectorSpace vsDefaultTrimCondition = { std::string{"Default Trim Condition"} };

	// Contructor of Helicopter Dynamics.
	HelicopterDynamics(char* cHeliYamlPathPtr, float* fDtPtr);

	// Resetting Helicopter Dynamics w.r.t trim condition.
	void reset(const VectorSpace& vsTrimCondition = {});

	// Getting number of observation of helicopter dynamics. 
	virtual int getNumberOfObservations();

	// Overriding the `stepEnd` method for bounding the variables.
	virtual void stepStart();

	// Overriding the `stepEnd` method for bounding the variables.
	virtual void stepEnd();

	// Calculation of dynamics of helicopter dynamics.
	virtual void dynamics();

	// Trimming the helicopter w.r.t trim condition.
	void trim();

	// Trim targets of trimming.
	void getTrimTargets(Eigen::VectorXf& vTrimOutputs);

};


