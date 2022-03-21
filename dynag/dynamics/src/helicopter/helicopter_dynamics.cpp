#define STB_IMAGE_IMPLEMENTATION
#include "helicopter_dynamics.h"

HelicopterDynamics::HelicopterDynamics(char* cHeliYamlPathPtr, float* fDtPtr)
{
	setDt(fDtPtr);
	setActionSize(this->iNumberOfAct);
	
	YAML::Node data = YAML::LoadFile(cHeliYamlPathPtr);
	checkYamlFile(data, cHeliYamlPathPtr);

	HELI = data["HELI"];
	node_map.insert(std::make_pair("HELI", HELI));
	ENV = data["ENV"];
	node_map.insert(std::make_pair("ENV", ENV));
	TRIM = HELI["TRIM"]; 
	node_map.insert(std::make_pair("TRIM", TRIM));
	MR = HELI["MR"];
	node_map.insert(std::make_pair("MR", MR));
	TR = HELI["TR"];
	node_map.insert(std::make_pair("TR", TR));
	FUS = HELI["FUS"];
	node_map.insert(std::make_pair("FUS", FUS));
	HT = HELI["HT"];
	node_map.insert(std::make_pair("HT", HT));
	VT = HELI["VT"];
	node_map.insert(std::make_pair("VT", VT));
	WN = HELI["WN"];
	node_map.insert(std::make_pair("WN", WN));
	LG = HELI["LG"];
	node_map.insert(std::make_pair("LG", LG));
	FLG = HELI["FLG"];
	node_map.insert(std::make_pair("FLG", FLG)); 

	// Fill lookup table
	TurbulenceExceedenceProbability << 500.0f << 1750.0f << 3750.0f << 7500.0f << 15000.0f << 25000.0f << 35000.0f << 45000.0f << 55000.0f << 65000.0f << 75000.0f << 80000.0f 
		<< 1.0f << 3.2f << 2.2f << 1.5f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f 
		<< 2.0f << 4.2f << 3.6f << 3.3f << 1.6f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f 
		<< 3.0f << 6.6f << 6.9f << 7.4f << 6.7f << 4.6f << 2.7f << 0.4f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f 
		<< 4.0f << 8.6f << 9.6f << 10.6f << 10.1f << 8.0 << 6.6f << 5.0f << 4.2f << 2.7f << 0.0f << 0.0f << 0.0f 
		<< 5.0f << 11.8f << 13.0f << 16.0f << 15.1f << 11.6f << 9.7f << 8.1f << 8.2f << 7.9f << 4.9f << 3.2f << 2.1f 
		<< 6.0f << 15.6f << 17.6f << 23.0f << 23.6f << 22.1f << 20.0f << 16.0f << 15.1f << 12.1f << 7.9f << 6.2f << 5.1f 
		<< 7.0f << 18.7f << 21.5f << 28.4f << 30.2f << 30.7f << 31.0f << 25.2f << 23.1f << 17.5f << 10.7f << 8.4f << 7.2f;	

	// Calculated some parameters
	preCalculations();

	// Register states
	registerStates();

	// Register observations
	registerObservations();

	// Register actions
	registerActions();

}

void HelicopterDynamics::registerStates()
{
	registerState("vimr", Eigen::Vector<float, 1>::Constant(40.0f), MR_V_TIP);
	registerState("vitr", Eigen::Vector<float, 1>::Constant(40.0f), TR_V_TIP);
	registerState("psimr", Eigen::Vector<float, 1>::Zero());
	registerState("psitr", Eigen::Vector<float, 1>::Zero());
	registerState("betas", Eigen::Vector<float, 2>::Zero());
	registerState("uvw", Eigen::Vector<float, 3>::Zero(), MR_V_TIP);
	registerState("pqr", Eigen::Vector<float, 3>::Zero(), MR_OMEGA);
	registerState("quat", Eigen::Vector<float, 4> {0.0f, 0.0f, 0.0f, 1.0f});
	registerState("xyz", Eigen::Vector<float, 3> {0.0f, 0.0f, -MAX_GR_ALT+1000.0f}, 200.0f*MR_R);
	registerState("swashdef", Eigen::Vector<float, 4>::Zero());
	registerState("uswind", Eigen::Vector<float, 1>::Zero()); // states related to u wind
	registerState("vswind", Eigen::Vector<float, 2>::Zero()); // states related to v wind
	registerState("wswind", Eigen::Vector<float, 2>::Zero()); // states related to w wind
	// In DynaG, there is a divergence mechanism which generate false `READY` signal
	// to Python side to reset environment if the vsState's normalized values is greater
	// than `NORM_LIMIT`. For wind dynamics, normalized value of derivative of `vs` and `ws` 
	// can be greater than limit. To handle it, normalizer of the derivatives are increased.
	vsState.vNormalizer(25) = 10.0f;
	vsState.vNormalizer(27) = 10.0f;
}

void HelicopterDynamics::registerObservations()
{
	registerObservation("totalhp", Eigen::Vector<float, 1>::Zero(), 1260.0f);
	registerObservation("uvwair", Eigen::Vector<float, 3>::Zero(), MR_V_TIP);
	registerObservation("uvw", Eigen::Vector<float, 3>::Zero(), MR_V_TIP);
	registerObservation("acc", Eigen::Vector<float, 3>::Zero(), MR_V_TIP*MR_OMEGA);
	registerObservation("nedvel", Eigen::Vector<float, 3>::Zero(), MR_V_TIP);
	registerObservation("eulerangles", Eigen::Vector<float, 3>::Zero());
	registerObservation("pqr", Eigen::Vector<float, 3>::Zero(), MR_OMEGA);
	registerObservation("xyz", Eigen::Vector<float, 3>::Zero(), 200.0f*MR_R);
	registerObservation("gralt", Eigen::Vector<float, 1>::Zero(), 200.0f*MR_R);
	registerObservation("swashdef", Eigen::Vector<float, 4>::Zero());
	registerObservation("swashrate", Eigen::Vector<float, 4>::Zero(), MR_OMEGA);
	registerObservation("wind", Eigen::Vector<float, 3>::Zero());
}

void HelicopterDynamics::registerActions()
{
	registerAction("swash", Eigen::Vector<float, 4>::Zero());
}

void HelicopterDynamics::getActions()
{
	vsAction.get(vSwashInput, "swash");
}

void HelicopterDynamics::getStates()
{
	// Get states 
	vsState.get(vSwashDeflection, "swashdef");
	vsState.get(vMainRotorVi, "vimr");
	vsState.get(vTailRotorVi, "vitr");
	vsState.get(vBetas, "betas");
	vsState.get(vUVW, "uvw");
	vsState.get(vPQR, "pqr");
	vsState.get(vQuat, "quat");
	vsState.get(vXYZ, "xyz");
	vsState.get(vMainRotorPsi, "psimr");
	vsState.get(vTailRotorPsi, "psitr");
	vsState.get(vUsWind, "uswind");
	vsState.get(vVsWind, "vswind");
	vsState.get(vWsWind, "wswind");
}

void HelicopterDynamics::setActions()
{
	vsAction.set("swash", vSwashInput);
}

void HelicopterDynamics::setStates()
{
	// Set states 
	vsState.set("swashdef", vSwashDeflection);
	vsState.set("vimr", vMainRotorVi);
	vsState.set("vitr", vTailRotorVi);
	vsState.set("betas", vBetas);
	vsState.set("uvw", vUVW);
	vsState.set("pqr", vPQR);
	vsState.set("quat", vQuat);
	vsState.set("xyz", vXYZ);
	vsState.set("psimr", vMainRotorPsi);
	vsState.set("psitr", vTailRotorPsi);
	vsState.set("uswind", vUsWind);
	vsState.set("vswind", vVsWind);
	vsState.set("wswind", vWsWind);
}

void HelicopterDynamics::setStateDots()
{
	// Set state dots 
	vsStateDot.set("swashdef", vSwashRate);
	vsStateDot.set("vimr", vMainRotorViDot);
	vsStateDot.set("vitr", vTailRotorViDot); 
	vsStateDot.set("betas", vBetasDot);
	vsStateDot.set("uvw", vUVWDot);
	vsStateDot.set("pqr", vPQRDot);
	vsStateDot.set("quat", vQuatDot);
	vsStateDot.set("xyz", vXYZDot);
	vsStateDot.set("psimr", vMainRotorPsiDot);
	vsStateDot.set("psitr", vTailRotorPsiDot);
	vsStateDot.set("uswind", vUsWindDot);
	vsStateDot.set("vswind", vVsWindDot);
	vsStateDot.set("wswind", vWsWindDot);
}

void HelicopterDynamics::setObservations()
{
	// Set current observations.
	vsObservation.set("totalhp", vTotalPowerHP);
	vsObservation.set("uvwair", vUVWAir);
	vsObservation.set("uvw", vUVW);
	vsObservation.set("acc", vBodyAcceleration);
	vsObservation.set("nedvel", vNEDVel);
	vsObservation.set("eulerangles", vEulerAngles);
	vsObservation.set("pqr", vPQR);
	vsObservation.set("xyz", vXYZ);
	vsObservation.set("gralt", vGroundAltitude);
	vsObservation.set("swashdef", vSwashDeflection);
	vsObservation.set("swashrate", vSwashRate);
	vsObservation.set("wind", vWind);
}

void HelicopterDynamics::setHeightMapMatrixFromHeightMapImage(Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& mTarget, unsigned short* shImagePtr, int& iH, int& iW)
{
	for (int y = 0; y < iH; y++)
	{
		for (int x = 0; x < iW; x++)
		{
			mTarget(y, x) = static_cast<float>(shImagePtr[y * iW + x] / (float)USHRT_MAX);
		}
	}
}

void HelicopterDynamics::setNormalMapMatrixFromNormalMapImage(unsigned char* cImagePtr, int& iH, int& iW, int& iC)
{
	/* NOTE : Importing point of the normal map image is the direction axis in normal map image.
	*		It should be same direction with our NED frame axis. This can be explain in this way,
	*		In `stb` library image read start from upper left (w.r.t screen) to bottom right as 
	*		row by row like other image libraries. Image's x axis increase with width of image (to right)
	*       and image's y axis increase with height of image (to bottom). Our NED frame uses the same
	*		direction. RGB values of normal map should have R as X and G as Y axis of NED frame. Z axis
	*		will be negative of NED frame. In NED frame, Z axis is positive towards to downward. On
	*		the other hand in normal vector, Z axis is positive towards to upward. So, G of image
	*		represents -Z axis of NED frame. Therefore, if RGB values of normal map is not match 
    *		with NED frame, normal values can be wrong and calculations will broken. 
	*/
	// Make sure that mNormalMap is already empty.
	mNormalMap.clear();
	Eigen::Vector<float, 3> temp;
	std::vector<Eigen::Vector<float, 3>> row;
	for (int y = 0; y < iH; y++)
	{
		row.clear();
		for (int x = 0; x < iW; x++)
		{
			unsigned char* pixelOffset = cImagePtr + (y * iW + x) * iC;
			temp(0) = (static_cast<float>(pixelOffset[0]) - 127.5f) / 127.5f;
			temp(1) = (static_cast<float>(pixelOffset[1]) - 127.5f) / 127.5f;
			temp(2) = (static_cast<float>(pixelOffset[2]) - 127.5f) / 127.5f;
			temp.normalize();
			row.push_back(temp);
		}
		mNormalMap.push_back(row);
	}
}

void HelicopterDynamics::preCalculations()
{	
	// Trim related
	YAW = TRIM["YAW"].as<float>();
	YAW_RATE = TRIM["YAW_RATE"].as<float>(); 
	N_VEL = TRIM["N_VEL"].as<float>();   
	E_VEL = TRIM["E_VEL"].as<float>();   
	D_VEL = TRIM["D_VEL"].as<float>();   
	N_POS = TRIM["N_POS"].as<float>();   
	E_POS = TRIM["E_POS"].as<float>();   
	GR_ALT = TRIM["GR_ALT"].as<float>();
	PSI_MR = TRIM["PSI_MR"].as<float>();
	PSI_TR = TRIM["PSI_TR"].as<float>();

	// Height map preparation
	NS_MAX = ENV["NS_MAX"].as<float>();
	EW_MAX = ENV["EW_MAX"].as<float>();
	MAX_GR_ALT = ENV["MAX_GR_ALT"].as<float>();
	MIN_GR_ALT = ENV["MIN_GR_ALT"].as<float>();
	
	int iWidthHeightMap, iHeightHeightMap, iNrComponentsHeightMap;
	shHeightMapPtr = stbi_load_16((std::getenv("DYNAG_RESOURCE_DIR") + ENV["HMAP_PATH"].as<std::string>()).c_str(), &iWidthHeightMap, &iHeightHeightMap, &iNrComponentsHeightMap, 0);
	mHeightMap.resize(iHeightHeightMap, iWidthHeightMap);
	setHeightMapMatrixFromHeightMapImage(mHeightMap, shHeightMapPtr, iHeightHeightMap, iWidthHeightMap);
	mHeightMap = mHeightMap.array() * (MAX_GR_ALT - MIN_GR_ALT);
	stbi_image_free(shHeightMapPtr);

	int iWidthNormalMap, iHeightNormalMap, iNrComponentsNormalMap;
	cNormalMapPtr = stbi_load((std::getenv("DYNAG_RESOURCE_DIR") + ENV["NMAP_PATH"].as<std::string>()).c_str(), &iWidthNormalMap, &iHeightNormalMap, &iNrComponentsNormalMap, 3);
	setNormalMapMatrixFromNormalMapImage(cNormalMapPtr, iHeightNormalMap, iWidthNormalMap, iNrComponentsNormalMap);
	stbi_image_free(cNormalMapPtr);


	fXPerPixel = NS_MAX / mHeightMap.rows();
	fYPerPixel = EW_MAX / mHeightMap.cols();
	
	//// Set Constant from yaml file.
	// Gravity
	GRAV = ENV["GRAV"].as<float>();

	// Air Properties
	T0 = ENV["T0"].as<float>();
	LAPSE = ENV["LAPSE"].as<float>();
	RO_SEA = ENV["RO_SEA"].as<float>();
	R = ENV["R"].as<float>();

	// Input params;
	COL_OS = HELI["COL_OS"].as<float>();
	COL_H = HELI["COL_H"].as<float>();
	COL_L = HELI["COL_L"].as<float>();
	COL_COF = HELI["COL_COF"].as<float>();
	LON_H = HELI["LAT_H"].as<float>();
	LON_L = HELI["LAT_L"].as<float>();
	LON_COF = HELI["LAT_COF"].as<float>();
	LAT_H = HELI["LON_H"].as<float>();
	LAT_L = HELI["LON_L"].as<float>();
	LAT_COF = HELI["LON_COF"].as<float>();
	PED_OS = HELI["PED_OS"].as<float>();
	PED_H = HELI["PED_H"].as<float>();
	PED_L = HELI["PED_L"].as<float>();
	PED_COF = HELI["PED_COF"].as<float>();

	// Component positions w.r.t CG Locations
	// 1/12 for converting inch to feet.
	WL_CG = HELI["WL_CG"].as<float>();
	FS_CG = HELI["FS_CG"].as<float>();
	MR_H = (MR["WL"].as<float>() - WL_CG) / 12.0f;
	MR_D = (MR["FS"].as<float>() - FS_CG) / 12.0f;
	MR["MR_H"] = MR_H;
	MR["MR_D"] = MR_D;
	FUS_H = (FUS["WL"].as<float>() - WL_CG) / 12.0f;
	FUS_D = (FUS["FS"].as<float>() - FS_CG) / 12.0f;
	FUS["FUS_H"] = FUS_H;
	FUS["FUS_D"] = FUS_D;
	WN_H = (WN["WL"].as<float>() - WL_CG) / 12.0f;
	WN_D = (WN["FS"].as<float>() - FS_CG) / 12.0f;
	WN["WN_H"] = WN_H;
	WN["WN_D"] = WN_D;
	HT_H = (HT["WL"].as<float>() - WL_CG) / 12.0f;
	HT_D = (HT["FS"].as<float>() - FS_CG) / 12.0f;
	HT["MR_H"] = HT_H;
	HT["MR_D"] = HT_D;
	VT_H = (VT["WL"].as<float>() - WL_CG) / 12.0f;
	VT_D = (VT["FS"].as<float>() - FS_CG) / 12.0f;
	VT["MR_H"] = VT_H;
	VT["MR_D"] = VT_D;
	TR_H = (TR["WL"].as<float>() - WL_CG) / 12.0f;
	TR_D = (TR["FS"].as<float>() - FS_CG) / 12.0f;
	TR["MR_H"] = TR_H;
	TR["MR_D"] = TR_D;
	LG_B = LG["B"].as<float>() / 12.0f;
	LG_H = (LG["WL"].as<float>() - WL_CG) / 12.0f;
	LG_D = (LG["FS"].as<float>() - FS_CG) / 12.0f;
	LG["LG_H"] = LG_H;
	LG["LG_D"] = LG_D;
	FLG_H = (FLG["WL"].as<float>() - WL_CG) / 12.0f;
	FLG_D = (FLG["FS"].as<float>() - FS_CG) / 12.0f;	
	FLG["LG_H"] = FLG_H;
	FLG["LG_D"] = FLG_D;

	// Vehicle General
	WT = HELI["WT"].as<float>();
	// Weight Vector
	vWeight[0] = 0.0f; 	
	vWeight[1] = 0.0f;
	vWeight[2] = WT;
	M = WT / GRAV; // [slug] 
	HELI["M"] = M;
	VTRANS = HELI["VTRANS"].as<float>();
	HP_LOSS = HELI["HP_LOSS"].as<float>();
	// Inertia Matrix and its reverse
	I = Eigen::Matrix3f::Zero();
	I(0, 0) =  HELI["IX"].as<float>();
	I(0, 2) = -HELI["IXZ"].as<float>();
	I(1, 1) =  HELI["IY"].as<float>();
	I(2, 0) = -HELI["IXZ"].as<float>();
	I(2, 2) =  HELI["IZ"].as<float>();
	IINV = I.inverse();

	// Main Rotor
	MR_R = MR["R"].as<float>();
	MR_K1 = MR["K1"].as<float>();
	MR_TWST = MR["TWST"].as<float>();
	MR_E = MR["E"].as<float>();
	MR_IS = MR["IS"].as<float>();
	MR_OMEGA = MR["RPM"].as<float>() * 2.0f * PI / 60.0f; // MR REV SPEED
	MR["MR_OMEGA"] = MR_OMEGA;
	MR_V_TIP = MR_R * MR_OMEGA; // [ft/s] MR TIP SPEED
	MR["MR_V_TIP"] = MR_V_TIP;
	MR_FR = MR["CD0"].as<float>() * MR_R * MR["B"].as<float>() * MR["C"].as<float>(); // MR EFFICIENT FRONTAL AREA 
	MR["MR_FR"] = MR_FR;
	MR_SOL = MR["B"].as<float>() * MR["C"].as<float>() / (MR_R * PI); // MR SOLIDITY (SIGMA)
	MR["MR_SOL"] = MR_SOL;
	MR_A_SIGMA = MR["A"].as<float>() * MR_SOL; // PRODUCTION OF LIFT-CURVE-SLOP WITH SOLIDITY
	MR["MR_A_SIGMA"] = MR_A_SIGMA;
	// ONE SIXTEENTH THE PRODUCT (LOCK# & ROTOR ANG. RATE) DIVIDED BY AIR DENSITY 
	MR_GAM_OM16_DRO = MR["A"].as<float>() * MR["C"].as<float>() * powf(MR_R, 4.0f) / MR["IB"].as<float>();
	MR_GAM_OM16_DRO *= MR_OMEGA / 16.0f * (1.0f + 8.0f / 3.0f * MR_E / MR_R);
	MR["MR_GAM_OM16_DRO"] = MR_GAM_OM16_DRO;
	// PRIMARY (DIRECT) FLAPPING STIFFNESS [rad/sec^2]
	MR_DL_DB1 = MR["B"].as<float>() / 2.0f * (1.5f * MR["IB"].as<float>() * MR_E / MR_R * powf(MR_OMEGA, 2.0f));
	MR["MR_DL_DB1"] = MR_DL_DB1;
	// CORSS (OFF-AXIS) FLAPPING STIFFNESS [rad/sec^2] DIVIDED BY AIR DENSITY
	MR_DL_DA1_DRO = 0.5f * MR["A"].as<float>() * MR["B"].as<float>() * MR["C"].as<float>() * MR_R * powf(MR_V_TIP, 2.0f) * MR_E / 6.0f;
	MR["MR_DL_DA1_DRO"] = MR_DL_DA1_DRO;
	MR_COEF_TH = 0.25f * MR_V_TIP * MR_R * MR["A"].as<float>() * MR["B"].as<float>() * MR["C"].as<float>(); // THRUST COEFFICIENT
	MR["MR_COEF_TH"] = MR_COEF_TH;

	// Tail Rotor
	TR_R = TR["R"].as<float>();
	TR_TWST = TR["TWST"].as<float>();
	TR_OMEGA = TR["RPM"].as<float>() * 2.0f * PI / 60.0f; // TR REV SPEED
	TR["TR_OMEGA"] = TR_OMEGA;
	TR_V_TIP = TR_R * TR_OMEGA; // [ft/s] TR TIP SPEED
	TR["TR_V_TIP"] = TR_V_TIP;
	TR_FR = TR["CD0"].as<float>() * TR_R * TR["B"].as<float>() * TR["C"].as<float>(); // TR EFFICIENT FRONTAL AREA 
	TR["TR_FR"] = TR_FR;
	TR_SOL = TR["B"].as<float>() * TR["C"].as<float>() / (TR_R * PI); // TR SOLIDITY (SIGMA)
	TR["TR_SOL"] = TR_SOL;
	TR_COEF_TH = 0.25f * TR_V_TIP * TR_R * TR["A"].as<float>() * TR["B"].as<float>() * TR["C"].as<float>(); // THRUST COEFFICIENT
	TR["TR_COEF_TH"] = TR_COEF_TH;

	// Fuselage
	FUS_COR = FUS["COR"].as<float>();
	FUS_XUU = FUS["XUU"].as<float>();
	FUS_YVV = FUS["YVV"].as<float>();
	FUS_ZWW = FUS["ZWW"].as<float>();

	// Horizontal Tail
	HT_ZMAX = HT["ZMAX"].as<float>();
	HT_ZUU = HT["ZUU"].as<float>();
	HT_ZUW = HT["ZUW"].as<float>();

	// Vertical Tail
	VT_YMAX = VT["YMAX"].as<float>();
	VT_YUU = VT["YUU"].as<float>();
	VT_YUV = VT["YUV"].as<float>();

	// Wing
	WN_ZMAX = WN["ZMAX"].as<float>();
	WN_ZUU = WN["ZUU"].as<float>();
	WN_ZUW = WN["ZUW"].as<float>();

	// Landing Gear
	LG_C = LG["C"].as<float>();
	LG_K = LG["K"].as<float>();
	LG_MU = LG["MU"].as<float>(); 
	FLG_C = FLG["C"].as<float>();
	FLG_K = FLG["K"].as<float>();	
	FLG_MU = FLG["MU"].as<float>(); 
	vRightLandingGearBodyPosition[0] = -LG_D;
	vRightLandingGearBodyPosition[1] = 0.5f * LG_B;
	vRightLandingGearBodyPosition[2] = -LG_H;
	vLeftLandingGearBodyPosition[0] = -LG_D;
	vLeftLandingGearBodyPosition[1] = -0.5f * LG_B;
	vLeftLandingGearBodyPosition[2] = -LG_H;
	vFrontLandingGearBodyPosition[0] = -FLG_D;
	vFrontLandingGearBodyPosition[1] = 0.0f;
	vFrontLandingGearBodyPosition[2] = -FLG_H;

	// Wind
	fEtaNorm = 1.0f / sqrtf(fDt);
	fTurbulenceLevel = ENV["TURB_LVL"].as<float>();
	fWindDirection = ENV["WIND_DIR"].as<float>() * D2R;
	fWindSpeed = ENV["WIND_SPD"].as<float>();
	fWindRandomness = ENV["WIND_DIR_RND"].as<float>();
	vWindMeanNED[0] = fWindSpeed * cosf(fWindDirection);
	vWindMeanNED[1] = fWindSpeed * sinf(fWindDirection);
	vWindMeanNED[2] = 0.0f;
	fWindSpeed20feet = fTurbulenceLevel / 7.0f * 88.61f; // mean wind speed at 20ft in[ft / s]
}

void HelicopterDynamics::reset(const VectorSpace& vsTrimCondition)
{
	getActions();
	getStates();
	preCalculations();		
		
	vsState.toDefault();
	vsStateDot.toDefault();
	vsAction.toDefault();
	vsObservation.toDefault();

	stepStart();	

	if (fWindRandomness == 1.0f)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		
		fWindDirection = (dis(gen) * PI * 2.0f) - PI;
		vWindMeanNED[0] = fWindSpeed * cosf(fWindDirection);
		vWindMeanNED[1] = fWindSpeed * sinf(fWindDirection);
		vWindMeanNED[2] = 0.0f;
	}

	READY = true;
	trim();
	setActions();
	setStates();
	setStateDots();
	setObservations();
}

void HelicopterDynamics::calculateAirProperties()
{
	fAirTemperature = T0 - LAPSE * vGroundAltitude(0); // [R] Temperature at current altitude
	fAirDensity = RO_SEA * powf(fAirTemperature / T0, ((GRAV / (LAPSE * R)) - 1.0f)); // [slug/ft^3]
}

void HelicopterDynamics::getGroundHeightNormal(const Eigen::Vector<float, 3>& vLocation, float& fGroundHeight, Eigen::Vector<float, 3>& vGroundNormal)
{
	float fXLocation = vLocation[0] / fXPerPixel + mHeightMap.rows() / 2.0f;
	float fYLocation = vLocation[1] / fYPerPixel + mHeightMap.cols() / 2.0f;

	if (fXLocation < 0.0f) fXLocation = 0.0f;
	else if (fXLocation > mHeightMap.rows() - 1) fXLocation = (float)mHeightMap.rows() - 1;

	if (fYLocation < 0.0f) fYLocation = 0.0f;
	else if (fYLocation > mHeightMap.cols() - 1) fYLocation = (float)mHeightMap.cols() - 1;

	int iXIndex = (int)round(fXLocation);
	int iYIndex = (int)round(fYLocation);

	iXIndex = (iXIndex == mHeightMap.rows() - 1) ? iXIndex = (int)mHeightMap.rows() - 1 : iXIndex;
	iYIndex = (iYIndex == mHeightMap.cols() - 1) ? iYIndex = (int)mHeightMap.cols() - 1 : iYIndex;
	
	float fHeightMiddle = mHeightMap(iYIndex, iXIndex);
	vGroundNormal = mNormalMap[iYIndex][iXIndex];
	vGroundNormal.normalize();
	fGroundHeight = fHeightMiddle + vGroundNormal[0] * (fXLocation - iXIndex) + vGroundNormal[1] * (fYLocation - iYIndex);	
}

void HelicopterDynamics::calculateWindTurbulence()
{
	vVelocityInfinityNED = vNEDVel + vWindMeanNED;
	fVelocityInfinity = vVelocityInfinityNED.norm();

	// MIL - HDBK - 1797 and MIL - HDBK - 1797B
	if (vGroundAltitude(0) <= 1000.0f) // Low - altitude turbulence
	{
		float h = fmaxf(vGroundAltitude(0), 10.0f);
		fLengthU = h / powf(0.177f + 0.000823f * h, 1.2f);
		fLengthV = 0.5f * fLengthU;
		fLengthW = 0.5f * h;
		fSigmaW = 0.1f * fWindSpeed20feet;
		fSigmaU = fSigmaW / powf(0.177f + 0.000823f * h, 0.4f);
		fSigmaV = fSigmaU;
		fTurbulenceAzimuth = fWindDirection;
	}
	else if (vGroundAltitude(0) >= 2000.0) // High - altitude turbulence
	{
		fLengthU = 1750.0f;
		fLengthV = 0.5f * fLengthU;
		fLengthW = 0.5f * fLengthU;
		float sigma = TurbulenceExceedenceProbability.get_value_2D(fTurbulenceLevel, vGroundAltitude(0));
		sigma = (fTurbulenceLevel<=0.0f) ? 0.0f : sigma;
		fSigmaU = sigma;
		fSigmaV = sigma;
		fSigmaW = sigma;
		fTurbulenceAzimuth = std::atan2(vVelocityInfinityNED[1], vVelocityInfinityNED[0]);
	}
	else
	{
		// Medium - altitude turbulence which is interpolation of 1000 ft(Low - altitude) and 2000 ft(high - altitude)
		fLengthU = 1000.0f + (vGroundAltitude(0) - 1000.0f) / 1000.0f * 750.0f;
		fLengthV = 0.5f * fLengthU;
		fLengthW = fLengthU;
		float sigma = 0.1f * fWindSpeed20feet + (vGroundAltitude(0) - 1000.0f) / 1000.0f * (TurbulenceExceedenceProbability.get_value_2D(fTurbulenceLevel, vGroundAltitude(0)) - 0.1f * fWindSpeed20feet);
		sigma = (fTurbulenceLevel<=0.0f) ? 0.0f : sigma;
		fSigmaU = sigma;
		fSigmaV = sigma;
		fSigmaW = sigma;
		float r = (vGroundAltitude(0) - 1000.0f) / 1000.0f;
		fTurbulenceAzimuth = std::atan2(vVelocityInfinityNED[1] * r + vWindMeanNED[1] * (1 - r), vVelocityInfinityNED[0] * r + vWindMeanNED[0] * (1 - r));
	}

	float t_u = fLengthU / (fVelocityInfinity + EPS);
	float t_v = fLengthV / (fVelocityInfinity + EPS);
	float t_w = fLengthW / (fVelocityInfinity + EPS);

	float K_u = fSigmaU * sqrtf(TWO_D_PI * t_u);
	float K_v = fSigmaV * sqrtf(TWO_D_PI * t_v);
	float K_w = fSigmaW * sqrtf(TWO_D_PI * t_w);

	vUsWindDot[0] = 1.0f / t_u * (K_u * vEta[0] - vUsWind[0]);
	vVsWindDot[0] = vVsWind[1];
	vVsWindDot[1] = 1.0f / (4.0f * powf(t_v, 2.0f)) * (K_v * vEta[1] - vVsWind[0]) - 1.0f / t_v * vVsWind[1];
	vWsWindDot[0] = vWsWind[1];
	vWsWindDot[1] = 1.0f / (4.0f * powf(t_w, 2.0f)) * (K_w * vEta[2] - vWsWind[0]) - 1.0f / t_w * vWsWind[1];

	float cturb = cosf(fTurbulenceAzimuth);
	float sturb = sinf(fTurbulenceAzimuth);

	vTurbulenceVelocity[0] = cturb * vUsWind[0] - sturb * vVsWind[0];
	vTurbulenceVelocity[1] = sturb * vUsWind[0] + cturb * vVsWind[0];
	vTurbulenceVelocity[2] = vWsWind[0];
	if (TRIMMING == false)
		vWind = vWindMeanNED + vTurbulenceVelocity;
	else
		vWind = vWindMeanNED;
}

void HelicopterDynamics::calculateMainRotorForceAndMoment()
{
	// Calculate required parameters;
	
	// one sixth the product(lock# and rotor ang.rate)
	float GAM_OM16 = fAirDensity * MR_GAM_OM16_DRO;
	
	// flapping aero coupling (flapping coupling factor)
	float KC = (0.75f * MR_OMEGA * MR_E / MR_R / GAM_OM16) + MR_K1;

	// flapping x-cpl coef.
	float ITB2_OM = MR_OMEGA / (1.0f + powf(MR_OMEGA / GAM_OM16, 2.0f));

	// flapping primary resp(inverse TPP lag)[rad / s]
	float ITB = ITB2_OM * MR_OMEGA / GAM_OM16;

	// cross(off - axis)flapping stiffness[rad / sec2]
	float DL_DA1 = fAirDensity * MR_DL_DA1_DRO;

	// MR Force, Moments & Inflow Dynamics
	float v_adv_2 = vUVWAir[0] * vUVWAir[0] + vUVWAir[1] * vUVWAir[1];
	float wr = vUVWAir[2] + (vBetas[0] - MR_IS) * vUVWAir[0] - vBetas[1] * vUVWAir[1]; //  z-axis vel re rotor plane
	float wb = wr + 0.66667f * MR_V_TIP * (vSwashDeflection(0) + 0.75f * MR_TWST) + v_adv_2 / MR_V_TIP * (vSwashDeflection(0) + 0.5f * MR_TWST); // z - axis vel re blade(equivalent)

	float thrust_mr = (wb - vMainRotorVi[0]) * fAirDensity * MR_COEF_TH;
	vMainRotorViDot[0] = 0.75f * PI / MR_R * (thrust_mr / (2.0f * PI * fAirDensity * powf(MR_R, 2.0f)) - vMainRotorVi[0] * sqrtf(v_adv_2 + powf((wr - vMainRotorVi[0]), 2.0f)));

	// MR induced flow power consumption
	float induced_power = thrust_mr * (vMainRotorVi[0] - wr);
	
	//MR profile drag power consumption
	float profile_power = 0.5f * fAirDensity * (MR_FR / 4.0f) * MR_V_TIP * (powf(MR_V_TIP, 2.0f) + 3.0f * v_adv_2);
	fMainRotorPower = induced_power + profile_power;
	float torque_mr = fMainRotorPower / MR_OMEGA;

	// thrust coeff.
	float CT = thrust_mr / (fAirDensity * PI * powf(MR_R, 2.0f) * powf(MR_V_TIP, 2.0f));
	CT = fmaxf(CT, 0.0f);
	
	//// Dihedral effect on TPP
	// TPP dihedral effect(late.flap2side vel)
	float DB1DV = 2.0f / MR_V_TIP * (8.0f * CT / MR_A_SIGMA + sqrtf(0.5f * CT));
	float DA1DU = -DB1DV; // TPP pitchup with speed

	// MR TPP Dynamics
	float wake_fn = (fabsf(vUVWAir[0]) > VTRANS) ? 1.0f : 0.0f;
	float a_sum = vBetas[1] - vSwashDeflection(2) + KC * vBetas[0] + DB1DV * vUVWAir[1] * (1.0f + wake_fn);
	float b_sum = vBetas[0] + vSwashDeflection(1) - KC * vBetas[1] + DA1DU * vUVWAir[0] * (1.0f + 2.0f * wake_fn);
	vBetasDot[0] = -ITB * b_sum - ITB2_OM * a_sum - vPQR[1];
	vBetasDot[1] = -ITB * a_sum + ITB2_OM * b_sum - vPQR[0];

	// Compute main rotor force and moment components
	vMainRotorForce[0] = -thrust_mr * (vBetas[0] - MR_IS);
	vMainRotorForce[1] = thrust_mr * vBetas[1];
	vMainRotorForce[2] = -thrust_mr;

	vMainRotorMoment[0] = vMainRotorForce[1] * MR_H + MR_DL_DB1 * vBetas[1] + DL_DA1 * (vBetas[0] + vSwashDeflection(1) - MR_K1 * vBetas[1]);
	vMainRotorMoment[1] = vMainRotorForce[2] * MR_D - vMainRotorForce[0] * MR_H + MR_DL_DB1 * vBetas[0] + DL_DA1 * (-vBetas[1] + vSwashDeflection(2) - MR_K1 * vBetas[0]);
	vMainRotorMoment[2] = torque_mr;
}

void HelicopterDynamics::calculateTailRotorForceAndMoment()
{
	// TR Force Moments and inflow dynamics.
	float _v = vUVWAir[2] + vPQR[1] * TR_D;
	float v_adv_2 = powf(_v, 2.0f) + powf(vUVWAir[0], 2.0f);
	float vr = -(vUVWAir[1] - vPQR[2] * TR_D + vPQR[0] * TR_H); // vel re rotor plane
	float vb = vr + 0.66667f * TR_V_TIP * (vSwashDeflection(3) + 0.75f * TR_TWST) + v_adv_2 / TR_V_TIP * (vSwashDeflection(3) + 0.5f * TR_TWST);// vel re blade plane(equivalent)

	float thrust_tr = (vb - vTailRotorVi[0]) * fAirDensity * TR_COEF_TH;
	vTailRotorViDot[0] = 0.75f * PI / TR_R * (thrust_tr / (2 * PI * fAirDensity * powf(TR_R, 2.0f)) - vTailRotorVi[0] * sqrtf(v_adv_2 + powf(vr - vTailRotorVi[0], 2.0f)));
	vTailRotorViDot[0] *= 0.5f; // slow down inflow dynamics due to numerical unstability.
	fTailRotorPower = thrust_tr * (vTailRotorVi[0] - vr);

	// Compute tail rotor force and moment components
	vTailRotorForce[0] = 0.0f;
	vTailRotorForce[1] = thrust_tr;
	vTailRotorForce[2] = 0.0f;

	vTailRotorMoment[0] = vTailRotorForce[1] * TR_H;
	vTailRotorMoment[1] = 0.0f;
	vTailRotorMoment[2] = -vTailRotorForce[1] * TR_D;
}

void HelicopterDynamics::calculateFuselageForceAndMoment()
{
	float wa_fus = vUVWAir[2] - vMainRotorVi[0]; // Include rotor downwash on fuselage
	wa_fus += (wa_fus > 0.0f) * EPS; // Make it nonzero!

	float d_fw = (vUVWAir[0] / (-wa_fus) * (MR_H - FUS_H)) - (FUS_D - MR_D); // Pos of downwash on fuselage
	d_fw *= FUS_COR; // emprical correction

	float rho_half = 0.5f * fAirDensity;

	vFuselageForce[0] = rho_half * FUS_XUU * fabsf(vUVWAir[0]) * vUVWAir[0];
	vFuselageForce[1] = rho_half * FUS_YVV * fabsf(vUVWAir[1]) * vUVWAir[1];
	vFuselageForce[2] = rho_half * FUS_ZWW * fabsf(wa_fus) * wa_fus;

	vFuselageMoment[0] = vFuselageForce[1] * FUS_H;
	vFuselageMoment[1] = vFuselageForce[2] * d_fw - vFuselageForce[0] * FUS_H;
	vFuselageMoment[2] = 0.0f;

	// Fuselage power consumption
	fFuselagePower = -vFuselageForce[0] * vUVWAir[0] - vFuselageForce[1] * vUVWAir[1] - vFuselageForce[2] * wa_fus;
}

void HelicopterDynamics::calculateHorizontalTailForceAndMoment()
{
	// downwash impinges on tail 
	float v_dw = fmaxf(vMainRotorVi[0] - vUVWAir[2], EPS);
	float d_dw = (vUVWAir[0] / v_dw * (MR_H - HT_H)) - (HT_D - MR_D - MR_R);

	float eps_ht = 0.0f;
	eps_ht = ((d_dw > 0) && (d_dw < MR_R)) ? 2.0f * (1.0f - d_dw / MR_R) : 0.0f; // Triangular downwash

	float wa_ht = vUVWAir[2] - eps_ht * vMainRotorVi[0] + HT_D * vPQR[1]; // local z - vel at h.t
	if (fabsf(wa_ht) > 0.3f * fabsf(vUVWAir[0])) // surface stalled 
	{
		float vta_ht = sqrtf(powf(vUVWAir[0], 2.0f) + powf(vUVWAir[1], 2.0f) + powf(wa_ht, 2.0f));
		vHorizontalTailForce[2] = 0.5f * fAirDensity * HT_ZMAX * fabsf(vta_ht) * wa_ht; // circulation
	}
	else vHorizontalTailForce[2] = 0.5f * fAirDensity * (HT_ZUU * fabsf(vUVWAir[0]) * vUVWAir[0] + HT_ZUW * fabsf(vUVWAir[0]) * wa_ht); // circulation

	vHorizontalTailForce[0] = 0.0f;
	vHorizontalTailForce[1] = 0.0f;

	vHorizontalTailMoment[0] = 0.0f;
	vHorizontalTailMoment[1] = vHorizontalTailForce[2] * HT_D; // pitching moment
	vHorizontalTailMoment[2] = 0.0f;
}

void HelicopterDynamics::calculateVerticalTailForceAndMoment()
{
	float va_vt = vUVWAir[1] + vTailRotorVi[0] - VT_D * vPQR[2];
	if (fabsf(va_vt) > 0.3f * fabsf(vUVWAir[0]))
	{
		float vta_vt = sqrtf(powf(vUVWAir[0], 2.0f) + powf(va_vt, 2.0f));
		vVerticalTailForce[1] = 0.5f * fAirDensity * VT_YMAX * fabsf(vta_vt) * va_vt;
	}
	else vVerticalTailForce[1] = 0.5f * fAirDensity * (VT_YUU * fabs(vUVWAir[0]) * vUVWAir[0] + VT_YUV * fabsf(vUVWAir[0]) * va_vt);

	vVerticalTailForce[0] = 0.0f;
	vVerticalTailForce[2] = 0.0f;

	vVerticalTailMoment[0] = vVerticalTailForce[1] * VT_H;
	vVerticalTailMoment[1] = 0.0f;
	vVerticalTailMoment[2] = -vVerticalTailForce[1] * VT_D;
}

void HelicopterDynamics::calculateWingForceAndMoment()
{
	// Wing
	if (WN_ZUW == 0.0f)
	{
		vWingForce[0] = 0.0f;
		vWingForce[2] = 0.0f;
	}
	else
	{
		float wa_wn = vUVWAir[2] - vMainRotorVi[0]; // local z - vel at wing
		float vta_wn = sqrtf(powf(vUVWAir[0], 2.0f) + powf(wa_wn,2.0f));

		if (fabsf(wa_wn) > 0.3f * fabs(vUVWAir[0])) //surface stalled
			vWingForce[2] = 0.5f * fAirDensity * WN_ZMAX * fabsf(vta_wn) * wa_wn;
		else
			vWingForce[2] = 0.5f * fAirDensity * (WN_ZUU * powf(vUVWAir[0], 2.0f) + WN_ZUW * vUVWAir[0] * wa_wn);

		vWingForce[0] = -0.5f * fAirDensity / PI / powf(vta_wn, 2.0f) * powf(WN_ZUU * powf(vUVWAir[0], 2.0f) + WN_ZUW * vUVWAir[0] * wa_wn, 2); // induced drag
	}
	
	vWingForce[1] = 0.0f;

	fWingPower = fabsf(vWingForce[0] * vUVWAir[0]); // wing power

	vWingMoment[0] = 0.0f;
	vWingMoment[1] = 0.0f;
	vWingMoment[2] = 0.0f;
}

void HelicopterDynamics::calculateLandingGearForceMoment()
{
	// Right Landing Gear
	vRightLandingGearEarthPosition = vXYZ + mBody2Earth * vRightLandingGearBodyPosition;
	getGroundHeightNormal(vRightLandingGearEarthPosition, fGroundHeightRightLandingGear, vGroundNormalRightLandingGear);
	float verticalCompressionRight = vRightLandingGearEarthPosition[2] + fGroundHeightRightLandingGear;
	if (verticalCompressionRight > 0.0f && TRIMMING == false)
	{
		float normalVelocityRight = vNEDVel.dot(vGroundNormalRightLandingGear);
		vGroundTangentRightLandingGear = (vNEDVel - normalVelocityRight*vGroundNormalRightLandingGear).normalized();
		float normalCompressionRight = verticalCompressionRight * vGroundNormalRightLandingGear[2];
		float landinGearForceRight = - LG_C * normalVelocityRight - LG_K * normalCompressionRight;
		vRightLandingGearEarthForce = landinGearForceRight * (vGroundNormalRightLandingGear + LG_MU*vGroundTangentRightLandingGear);
	}
	else
	{
		vRightLandingGearEarthForce[0] = 0.0f;
		vRightLandingGearEarthForce[1] = 0.0f;
		vRightLandingGearEarthForce[2] = 0.0f;
	}
	vRightLandingGearBodyForce = mEarth2Body * vRightLandingGearEarthForce;

	// Left Landing Gear
	vLeftLandingGearEarthPosition = vXYZ + mBody2Earth * vLeftLandingGearBodyPosition;
	getGroundHeightNormal(vLeftLandingGearEarthPosition, fGroundHeightLeftLandingGear, vGroundNormalLeftLandingGear);

	float verticalCompressionLeft = vLeftLandingGearEarthPosition[2] + fGroundHeightLeftLandingGear;	
	if (verticalCompressionLeft > 0.0f && TRIMMING == false)
	{
		float normalVelocityLeft = vNEDVel.dot(vGroundNormalLeftLandingGear);
		vGroundTangentLeftLandingGear = (vNEDVel - normalVelocityLeft*vGroundNormalLeftLandingGear).normalized();
		float normalCompressionLeft = verticalCompressionLeft * vGroundNormalLeftLandingGear[2];
		float landinGearForceLeft = - LG_C * normalVelocityLeft - LG_K * normalCompressionLeft;
		vLeftLandingGearEarthForce = landinGearForceLeft * (vGroundNormalLeftLandingGear+LG_MU*vGroundTangentLeftLandingGear);
	}
	else
	{
		vLeftLandingGearEarthForce[0] = 0.0f;
		vLeftLandingGearEarthForce[1] = 0.0f;
		vLeftLandingGearEarthForce[2] = 0.0f;
	}
	vLeftLandingGearBodyForce = mEarth2Body * vLeftLandingGearEarthForce;

	// Front Landing Gear
	vFrontLandingGearEarthPosition = vXYZ + mBody2Earth * vFrontLandingGearBodyPosition;
	getGroundHeightNormal(vFrontLandingGearEarthPosition, fGroundHeightFrontLandingGear, vGroundNormalFrontLandingGear);

	float verticalCompressionFront = vFrontLandingGearEarthPosition[2] + fGroundHeightFrontLandingGear;
	if (verticalCompressionFront > 0.0f && TRIMMING == false)
	{
		float normalVelocityFront = vNEDVel.dot(vGroundNormalFrontLandingGear);
		vGroundTangentFrontLandingGear = (vNEDVel - normalVelocityFront*vGroundNormalFrontLandingGear).normalized();
		float normalCompressionFront = verticalCompressionFront * vGroundNormalFrontLandingGear[2];
		float landinGearForceFront = - FLG_C * normalVelocityFront - FLG_K * normalCompressionFront;
		vFrontLandingGearEarthForce = landinGearForceFront * (vGroundNormalFrontLandingGear+FLG_MU*vGroundTangentFrontLandingGear);
	}
	else
	{
		vFrontLandingGearEarthForce[0] = 0.0f;
		vFrontLandingGearEarthForce[1] = 0.0f;
		vFrontLandingGearEarthForce[2] = 0.0f;
	}
	vFrontLandingGearBodyForce = mEarth2Body * vFrontLandingGearEarthForce;

	vLandingGearForce = vRightLandingGearBodyForce + vLeftLandingGearBodyForce + vFrontLandingGearBodyForce;
	vLandingGearMoment = vRightLandingGearBodyPosition.cross(vRightLandingGearBodyForce) + 
		vLeftLandingGearBodyPosition.cross(vLeftLandingGearBodyForce) + 
		vFrontLandingGearBodyPosition.cross(vFrontLandingGearBodyForce);
}

void HelicopterDynamics::dynamics()
{
	// Wind calculations 
	calculateWindTurbulence(); 

	// Control input calculations 
	float collInputAngle = D2R * (COL_OS + 0.5f * vSwashInput(0) * (COL_H - COL_L) + 0.5f * (COL_H + COL_L));
	vSwashRate(0) = (collInputAngle - vSwashDeflection(0)) * COL_COF;

	float lonInputAngle = D2R * (0.5f * vSwashInput(1) * (LON_H - LON_L) + 0.5f * (LON_H + LON_L));
	vSwashRate(1) = (lonInputAngle - vSwashDeflection(1)) * LON_COF;

	float latInputAngle = D2R * (0.5f * vSwashInput(2) * (LAT_H - LAT_L) +	0.5f * (LAT_H + LAT_L));
	vSwashRate(2) = (latInputAngle - vSwashDeflection(2)) * LAT_COF;

	float pedalInputAngle = D2R * (PED_OS + 0.5f * vSwashInput(3) * (PED_H - PED_L) + 0.5f * (PED_H + PED_L));
	vSwashRate(3) = (pedalInputAngle - vSwashDeflection(3)) * PED_COF;

	// Kinematic calculations
	quaternion2DCM(mEarth2Body, vQuat.normalized());
	mBody2Earth = mEarth2Body.transpose(); // Body to Earth DCM matrix

	pqr2QuaternionDot(vQuatDot, vQuat, vPQR);
		 
	pqr2EulerDot(mPQR2EulerDot, vEulerAngles); // pqr to eulerdot function.
	vEulerAnglesDot = mPQR2EulerDot * vPQR; // calculated eulerdot. Euler updated during trimming. Therefore its needed.
	vNEDVel = mBody2Earth * vUVW; // ned velocity

	// Airspeed calculations
	vUVWAir = vUVW - mEarth2Body * vWind;

	// power observation
	float power_climb = WT * (-vNEDVel[2]); // Climbing power[hp]

	// Calculate ground related things
	getGroundHeightNormal(vXYZ, fGroundHeight, vGroundNormal); 

	// Ground altitude calculation
	vGroundAltitude(0) = - vXYZ[2] - fGroundHeight;

	// Atmosphere calculation
	calculateAirProperties();

	// Calculate Each Components Force, Moments, Power etc.
	calculateMainRotorForceAndMoment();
	calculateTailRotorForceAndMoment();
	calculateFuselageForceAndMoment();
	calculateHorizontalTailForceAndMoment();
	calculateVerticalTailForceAndMoment();
	calculateWingForceAndMoment();
	calculateLandingGearForceMoment(); 

	// Other power consumptions are counted for main rotor torque
	float power_extra_MR = power_climb + fFuselagePower;
	float extra_MR_torque = power_extra_MR / MR_OMEGA;
	vMainRotorMoment[2] += extra_MR_torque;

	fTotalPower = fMainRotorPower + fTailRotorPower + power_extra_MR + fWingPower + 550.0f * HP_LOSS;

	vGravityForce = mEarth2Body * vWeight;

	vTotalForce = vMainRotorForce + vTailRotorForce + vFuselageForce + vHorizontalTailForce + vVerticalTailForce + vWingForce + vLandingGearForce + vGravityForce ;
	vTotalMoment = vMainRotorMoment + vTailRotorMoment + vFuselageMoment + vHorizontalTailMoment + vVerticalTailMoment + vWingMoment + vLandingGearMoment;

	// Acceleration || dot calculations
	vBodyAcceleration = vTotalForce.array() / M;
	vUVWDot = vBodyAcceleration - vPQR.cross(vUVW);

	vPQRDot = IINV * (vTotalMoment - vPQR.cross(I * vPQR));
	vXYZDot = vNEDVel;

	// Transmission dynamics of MR
	vMainRotorPsiDot[0] = MR_OMEGA;

	// Transmission dynamics of TR
	vTailRotorPsiDot[0] = TR_OMEGA;

	// Extra calculations
	vTotalPowerHP(0) = fTotalPower / 550.0f; // [hp] Power consumption in Horse Power
	unitQuaternion2Euler(vEulerAngles, vQuat);
}

void HelicopterDynamics::stepStart()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<float> dis(0.0, 1.0);

	vEta = Eigen::Vector3f::Zero().unaryExpr([&](float dummy) {return dis(gen); });
	vEta *= fEtaNorm;
}

void HelicopterDynamics::stepEnd()
{
	vQuat.normalize();
	pi_bound(vEulerAngles);
	pi_bound(vMainRotorPsi);
	pi_bound(vTailRotorPsi);
	pi_bound(vBetas);
}

int HelicopterDynamics::getNumberOfObservations()
{
	return iNumberOfObservation;
}

void HelicopterDynamics::trim()
{
	TRIMMING = true;
	// First, fix some parameters which are not iterated through trim algorithm.
	// However, these parameters will affect the trim.
	
	vsStatePerturbation.init(vsState);
	vsActionPerturbation.init(vsAction);

	int stateSize = (int)vsState0.vValues.size();
	int actionSize = (int)vsAction0.vValues.size();
	int observationSize = (int)vsObservation.vValues.size();
	int stateActionSize = stateSize + actionSize;
	
	mJacobian.resize(stateActionSize, stateActionSize);
	mA.resize(stateSize, stateSize), mB.resize(stateSize, actionSize), mC.resize(observationSize, stateSize), mD.resize(observationSize, actionSize);
	mI = Eigen::MatrixXf::Identity(stateActionSize, stateActionSize);
	 
	vStateDotForward.resize(stateSize), vStateDotBackward.resize(stateSize);
	vObservationForward.resize(observationSize), vObservationBackward.resize(observationSize);
	int totalSteps = 0;	
	
	vTrimOutputs.resize(stateActionSize), vTrimOutputsNew.resize(stateActionSize),
	vTrimOutputsForward.resize(stateActionSize), vTrimOutputsBackward.resize(stateActionSize);

	getStates(); getActions(); dynamics(); setStateDots(); setObservations(); 
	getTrimTargets(vTrimOutputs); 
	float tolerance = vTrimOutputs.transpose() * vTrimOutputs;
	for (int iter = 0; iter < 1000; iter++)
	{
		if (tolerance < EPS*EPS)
		{
			READY = true;
			break;
		}
		else if (vsState.isDiverged())
		{
			break;
		}
		// std::cout << "Trim cost: " << tolerance << std::endl;
		vsState0 = vsState;
		vsAction0 = vsAction;
		for (int i = 0; i < stateSize; i++)
		{
			vsStatePerturbation.setAllValuesToZero();
			vsStatePerturbation.setNormalizedValue(i, EPS);
			// perturb forward
			vsState = vsState0 + vsStatePerturbation;
			getStates(); getActions(); dynamics(); setStateDots(); setObservations(); 
			vStateDotForward = vsStateDot.vValues; vObservationForward = vsObservation.vValues; 
			getTrimTargets(vTrimOutputsForward); 
			// perturb backward
			vsState = vsState0 - vsStatePerturbation;
			getStates(); getActions(); dynamics(); setStateDots(); setObservations(); 
			vStateDotBackward = vsStateDot.vValues; vObservationBackward = vsObservation.vValues; 
			getTrimTargets(vTrimOutputsBackward); 
			mJacobian.col(i) = (vTrimOutputsForward - vTrimOutputsBackward) / (vsStatePerturbation.getValue(i));
			mA.col(i) = (vStateDotForward - vStateDotBackward) / vsStatePerturbation.getValue(i);
			mC.col(i) = (vObservationForward - vObservationBackward) / vsStatePerturbation.getValue(i);
		}
		vsState = vsState0;
		for (int i = 0; i < actionSize; i++)
		{
			vsActionPerturbation.setAllValuesToZero();
			vsActionPerturbation.setNormalizedValue(i, EPS);
			// perturb forward
			vsAction = vsAction0 + vsActionPerturbation;
			getStates(); getActions(); dynamics(); setStateDots(); setObservations(); 
			vStateDotForward = vsStateDot.vValues; vObservationForward = vsObservation.vValues; 
			getTrimTargets(vTrimOutputsForward); 
			// perturb backward
			vsAction = vsAction0 - vsActionPerturbation;
			getStates(); getActions(); dynamics(); setStateDots(); setObservations(); 
			vStateDotBackward = vsStateDot.vValues; vObservationBackward = vsObservation.vValues; 
			getTrimTargets(vTrimOutputsBackward); 
			mJacobian.col(i+stateSize) = (vTrimOutputsForward - vTrimOutputsBackward) / (vsActionPerturbation.getValue(i));
			mB.col(i) = (vStateDotForward - vStateDotBackward) / vsActionPerturbation.getValue(i);
			mD.col(i) = (vObservationForward - vObservationBackward) / vsActionPerturbation.getValue(i);
		}
		vsAction = vsAction0;
		// vStep = (mJacobian.transpose() * mJacobian + EPS*mI).inverse() * (mJacobian.transpose() * vTrimOutputs); 
		vStep = mJacobian.inverse() * vTrimOutputs;

		bool _nan = is_nan(vStep); // check whether step_dir become nan, trim default cond.

		if ( _nan == true )
		{
			//const Eigen::IOFormat fmt(1, 1, ", ", "\n");
			//std::cout << mJacobian.format(fmt) << "\n" << std::endl;
			//std::cout << vStep << "\n" << std::endl;
			//std::cout << mJacobian.eigenvalues() << "\n" << std::endl;
			//std::cout << vMainRotorPsi << " " << PSI_MR << std::endl;
			break;
			vStep = (mJacobian.transpose() * mJacobian + mI / (float)(iter+1)).inverse() * (mJacobian.transpose() * vTrimOutputs); 
		}

		float stepSize = 1.0f;
		int numberOfBacktracing = 0;
		float toleranceNew = 0.0f;
		// Backtracing loop
		while (numberOfBacktracing < 10)
		{
			vsState.vValues = vsState0.vValues - stepSize * vStep(Eigen::seqN(0, stateSize));
			vsAction.vValues = vsAction0.vValues - stepSize * vStep(Eigen::seqN(stateSize, actionSize));
			getStates(); getActions(); dynamics(); setStateDots(); setObservations();
			getTrimTargets(vTrimOutputsNew);
			toleranceNew = vTrimOutputsNew.transpose() * vTrimOutputsNew;
			stepSize *= 0.5f;
			if (toleranceNew < tolerance) break; // if tolerance decreased, break for new step.
			numberOfBacktracing++;
			totalSteps++;
		}
		// Update trim outputs and tolerance
		vTrimOutputs = vTrimOutputsNew;
		tolerance = toleranceNew;
	}
	TRIMMING = false;

	// Free memory for prevent leaks.
	mJacobian.resize(0, 0), mA.resize(0, 0), mB.resize(0, 0), mC.resize(0, 0), mD.resize(0, 0), mI.resize(0, 0);
	vStep.resize(0), vStateDotForward.resize(0), vStateDotBackward.resize(0), vObservationForward.resize(0), vObservationBackward.resize(0);
	vTrimOutputs.resize(0), vTrimOutputsNew.resize(0), vTrimOutputsForward.resize(0), vTrimOutputsBackward.resize(0);
}

void HelicopterDynamics::getTrimTargets(Eigen::VectorXf& vTrimOutputs)
{
	vTrimOutputs(0) = vMainRotorViDot[0] / (MR_V_TIP*MR_OMEGA) ;
	vTrimOutputs(1) = vTailRotorViDot[0] / (TR_V_TIP*TR_OMEGA); 
	vTrimOutputs(2) = vBetasDot[0] / MR_OMEGA;
	vTrimOutputs(3) = vBetasDot[1] / MR_OMEGA;
	vTrimOutputs(4) = vUVWDot[0] / (MR_V_TIP*MR_OMEGA);
	vTrimOutputs(5) = vUVWDot[1] / (MR_V_TIP*MR_OMEGA);
	vTrimOutputs(6) = vUVWDot[2] / (MR_V_TIP*MR_OMEGA);
	vTrimOutputs(7) = vPQRDot[0] / (MR_OMEGA*MR_OMEGA);
	vTrimOutputs(8) = vPQRDot[1] / (MR_OMEGA*MR_OMEGA);
	vTrimOutputs(9) = vPQRDot[2] / (MR_OMEGA*MR_OMEGA);
	vTrimOutputs(10) = vEulerAnglesDot[0] / MR_OMEGA;
	vTrimOutputs(11) = vEulerAnglesDot[1] / MR_OMEGA;
	vTrimOutputs(12) = (vEulerAnglesDot[2] - YAW_RATE) / MR_OMEGA;
	vTrimOutputs(13) = (vNEDVel[0] - N_VEL) / MR_V_TIP;
	vTrimOutputs(14) = (vNEDVel[1] - E_VEL) / MR_V_TIP;
	vTrimOutputs(15) = (vNEDVel[2] - D_VEL) / MR_V_TIP;
	vTrimOutputs(16) = vSwashRate[0] / MR_OMEGA;
	vTrimOutputs(17) = vSwashRate[1] / MR_OMEGA;
	vTrimOutputs(18) = vSwashRate[2] / MR_OMEGA;
	vTrimOutputs(19) = vSwashRate[3] / MR_OMEGA;
	vTrimOutputs(20) = vMainRotorPsi[0] - PSI_MR;
	vTrimOutputs(21) = vTailRotorPsi[0] - PSI_TR;
	vTrimOutputs(22) = (vXYZ[0] - N_POS) / (200.0f*MR_R);
	vTrimOutputs(23) = (vXYZ[1] - E_POS) / (200.0f*MR_R);
	vTrimOutputs(24) = (vGroundAltitude[0] - GR_ALT) / (200.0f*MR_R);
	vTrimOutputs(25) = vEulerAngles[2] - YAW;
	vTrimOutputs(26) = vQuat.norm() - 1.0f;
	vTrimOutputs(27) = vUsWind[0] / MR_V_TIP;
	vTrimOutputs(28) = vVsWind[0] / MR_V_TIP;
	vTrimOutputs(29) = vVsWind[1] / MR_V_TIP;
	vTrimOutputs(30) = vWsWind[0] / MR_V_TIP;
	vTrimOutputs(31) = vWsWind[1] / MR_V_TIP;
}
