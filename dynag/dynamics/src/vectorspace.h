/*
* DynaG Dynamics C++ / VectorSpace
* 
* VectorSpace is an struct for store the vector.
* 
* Author : @MGokcayK 
* Author : @ugurcanozalp (uurcann94@gmail.com)
*
* C.Date : 15/09/2021
* Update : 15/09/2021 - Class Initialization & Implementation // @MGokcayK
* Update : 17/09/2021 - Adding `init` & `assign operator` // @MGokcayK
* Update : 23/09/2021 - Adding other `get` methods and modify `set` method // @MGokcayK
* Update : 25/11/2021 - Modify the struct for normalizer property. // @MGokcayK
* Update : 18/12/2021 - Modify the struct and add name variable for log. // @MGokcayK
* Update : 19/12/2021 - Adding registerVector vector istead of registering in set method and `reset` method.// @MGokcayK
* Update : 29/12/2021 - Adding getter and setter for `vValues` and `vNormalizedValues`. // @MGokcayK
*/

#pragma once
#define NORM_LIMIT 20
#include "utils.h"

struct numberedInfos
{
	Eigen::Index index;
	Eigen::Index size;
};

// Struct a general VectorSpace which used for storing vectors and updates in numerical methods.
struct VectorSpace
{
private:
	// Finding index of vector whether if it is exist or not. If not, throw error.
	std::unordered_map<std::string, numberedInfos>::const_iterator findIndex(const std::string& sVectorName);
	
	// Check mathematical operation of VectorSpace is proper or not. If not, throw error.
	void checkOperation(const VectorSpace& vsOther, const std::string& operatorName);

public:
	// Default Constructor.
	VectorSpace() {};

	// Named Constructor which can set the name of VectorSpace during Construction.
	VectorSpace(const std::string& sName);

	// Basic storage knowlagde of vector w.r.t its name, start index and size.
	std::unordered_map<std::string, numberedInfos> mapInfo = {};
	// Basic storage of vector values.
	Eigen::VectorXf vDefault;
	Eigen::VectorXf vValues;
	Eigen::VectorXf vNormalizedValues;
	Eigen::VectorXf vNormalizer;
	Eigen::VectorXf vTemp;

	std::string sName = "vectorspace";

	// Copying the VectorSpace into another VectorSpace without any checking.
	void init(const VectorSpace& vsOther);

	// Reset VectorSpace. 
	void reset();

	// Registering vector into VectorSpace w.r.t name.
	void registerVector(const std::string& sVectorName, const Eigen::VectorXf& vValue, const float& fNormalizer = 1.0f);

	// Setting vector into VectorSpace w.r.t name. If vector is exist, set new value.
	void set(const std::string& sVectorName, const Eigen::VectorXf& vValue);

	// Setting values of vector space by pointer of new value vector.
	void setValues(float* vValue);

	// Get vector from VectorSpace w.r.t name.
	Eigen::VectorXf get(const std::string& sVectorName);

	// Get vector value from VectorSpace w.r.t its name and index. It set the target value to only one 
	// value. 
	void get(float& fTarget, const std::string& sVectorName, const int& iIndex);

	// Get vector from VectorSpace to destination Eigen based object w.r.t its name.
	template<typename T>
	void get(Eigen::MatrixBase<T>& mDestination, const std::string& sVectorName)
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = mapInfo.find(sVectorName);
		if (ind == mapInfo.end()) std::cout << "vector `" << sVectorName << "` not found in " << sName << "!" << std::endl;
		mDestination = Eigen::Map<Eigen::VectorXf> (vValues.data() + ind->second.index, ind->second.size);
	}

	// Get vector from [] operator w.r.t its name.
	Eigen::VectorXf operator[](const std::string& sVectorName);

	// Get normalized vector from VectorSpace w.r.t name.
	Eigen::VectorXf getNormalized(const std::string& sVectorName);

	// Get normalized vector value from VectorSpace w.r.t its name and index. It set the target value to only one 
	// value. 
	void getNormalized(float& fTarget, const std::string& sVectorName, const int& iIndex);

	// Get pointer and size of vector from VectorSpace. It is written for Python side.
	// It return the pointer of vector.
	float* getPointerAndSetSize(const std::string& sVectorName, int* iSizePtr);

	// Get pointer and size of normalized vector from VectorSpace. It is written for Python side.
	// It return the pointer of vector.
	float* getNormalizedPointerAndSetSize(const std::string& sVectorName, int* iSizePtr);

	// Get value of Vectorspace at `iIndex`.
	const float& getValue(int iIndex);

	// Get vector of Vectorspace form `iIndex` with size `iSize`.
	Eigen::VectorXf getValue(int iIndex, int iSize);

	// Set value of Vectorspace at `iIndex`.
	void setValue(int iIndex, float fNewValue);

	// Set vector of Vectorspace form `iIndex` with size `iSize`.
	void setValue(int iIndex, Eigen::VectorXf vNewValue);

	// Get normalized value of Vectorspace at `iIndex`.
	const float& getNormalizedValue(int iIndex);

	// Get normalized vector of Vectorspace form `iIndex` with size `iSize`.
	Eigen::VectorXf getNormalizedValue(int iIndex, int iSize);

	// Set normalized value of Vectorspace at `iIndex`.
	void setNormalizedValue(int iIndex, float fNewValue);

	// Set normalized vector of Vectorspace form `iIndex` with size `iSize`.
	void setNormalizedValue(int iIndex, Eigen::VectorXf vNewValue);

	// Set all values to zero
	void setAllValuesToZero();

	// Divergence flag.
	bool isDiverged();

	// Set all values and normalized values to default state which equals to registered values.
	void toDefault();

	// Addition operator of two VectorSpace. It checks wheter two VectorSpace has same size or not.
	VectorSpace operator+(const VectorSpace& vsOther);

	// Addition operator of VectorSpace and float. It adds the float to all elements in VectorSpace.
	VectorSpace operator+(const float& fOther);

	// Subtraction operator of two VectorSpace. It checks wheter two VectorSpace has same size or not.
	VectorSpace operator-(const VectorSpace& vsOther);

	// Subtraction operator of VectorSpace and float. It subtract the float to all elements in VectorSpace.
	VectorSpace operator-(const float& fOther);
	
	// Multiplication operator of two VectorSpace. It checks wheter two VectorSpace has same size or not.
	// It multiply same indexes into each other.
	VectorSpace operator*(const VectorSpace& vsOther);

	// Multiplication operator of VectorSpace and float. It multiply the float to all elements in VectorSpace.
	VectorSpace operator*(const float& fOther);

	// Division operator of two VectorSpace. It checks wheter two VectorSpace has same size or not.
	// It divide same indexes into each other.
	VectorSpace operator/(const VectorSpace& vsOther);

	// Division operator of VectorSpace and float. It divide the float to all elements in VectorSpace.
	VectorSpace operator/(const float& fOther);

	// Assign operator of two VectorSpace. It checks wheter two VectorSpace has same size or not.
	void operator=(const VectorSpace& vsOther);
};