#include "vectorspace.h"

std::unordered_map<std::string, numberedInfos>::const_iterator VectorSpace::findIndex(const std::string& sVectorName)
{
	std::unordered_map<std::string, numberedInfos>::const_iterator ind = mapInfo.find(sVectorName);
	if (ind == mapInfo.end())
	{
		gThrow("Not Found : Variable `" + sVectorName + "` not found in " + sName + "!");
	}
	else
	{
		return ind;
	}
}

void VectorSpace::checkOperation(const VectorSpace& vsOther, const std::string& operatorName)
{
	if (vValues.size() != vsOther.vValues.size())
	{
		gThrow("Size Error : Size of two VectorSpace are not equal for " + operatorName + "! (" + sName + " & " + vsOther.sName + ")");
	}
}

VectorSpace::VectorSpace(const std::string& sName)
{
	this->sName = sName;
}

void VectorSpace::init(const VectorSpace& vsOther)
{
	mapInfo = vsOther.mapInfo;
	vValues = vsOther.vValues;
	vNormalizedValues = vsOther.vNormalizedValues;
	vNormalizer = vsOther.vNormalizer;
}

void VectorSpace::reset()
{
	mapInfo = {};
	vValues = {};
	vNormalizedValues = {};
	vNormalizer = {};
}

void VectorSpace::registerVector(const std::string& sVectorName, const Eigen::VectorXf& vValue, const float& fNormalizer)
{
	numberedInfos temp;
	temp.index = vValues.size();
	temp.size = vValue.size();
	mapInfo.insert(std::make_pair(sVectorName, temp));
	Eigen::VectorXf vec_joined(vValues.size() + vValue.size());
	Eigen::VectorXf vecNorm_joined(vNormalizedValues.size() + vValue.size());
	Eigen::VectorXf vecNormalizer_joined(vNormalizer.size() + vValue.size());
	vec_joined << vValues, vValue;
	vValues = vec_joined;
	vDefault = vValues;
	vecNorm_joined << vNormalizedValues, vValue.array() / (fNormalizer + EPS);
	vNormalizedValues = vecNorm_joined;
	vecNormalizer_joined << vNormalizer, Eigen::VectorXf::Constant(vValue.size(), fNormalizer);
	vNormalizer = vecNormalizer_joined;
}

void VectorSpace::set(const std::string& sVectorName, const Eigen::VectorXf& vValue)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		if (ind != mapInfo.end() && ind->second.size == vValue.size())
		{
			vValues.segment(ind->second.index, ind->second.size) = vValue;
			vNormalizedValues.segment(ind->second.index, ind->second.size) = vValue.array() / (vNormalizer.segment(ind->second.index, ind->second.size).array() + EPS);
			if (TRIMMING == false && sName == "State")
			{
				bool d = isDiverged();
				if (d == true) READY = false;
			}
		}
		else if (ind != mapInfo.end())
		{			
			gThrow("Not Set : Please set different name for `" + sVectorName + "` in " + sName + "! \n\t\tBecause `" + sVectorName + "` has already defined with different size!");		
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

bool VectorSpace::isDiverged()
{
	if ((vNormalizedValues.array().abs() > NORM_LIMIT).any())
	{
		return true;				
	}
	else
	{
		return false;
	}
}

void VectorSpace::toDefault()
{
	vValues = vDefault;
	vNormalizedValues = vValues.array() / (vNormalizer.array() + EPS);
}

void VectorSpace::setValues(float* vValue)
{
	for (int i = 0; i < vValues.size(); i++)
	{
		vValues(i) = vValue[i];
		vNormalizedValues(i) = vValue[i] / (vNormalizer[i] + EPS);
	}
}

Eigen::VectorXf VectorSpace::get(const std::string& sVectorName)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		return vValues(Eigen::seqN(ind->second.index, ind->second.size));
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void VectorSpace::get(float& fTarget, const std::string& sVectorName, const int& iIndex)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		fTarget = vValues(ind->second.index + iIndex);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

Eigen::VectorXf VectorSpace::operator[](const std::string& sVectorName)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		return vValues(Eigen::seqN(ind->second.index, ind->second.size));
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

Eigen::VectorXf VectorSpace::getNormalized(const std::string& sVectorName)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		return vNormalizedValues(Eigen::seqN(ind->second.index, ind->second.size));
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void VectorSpace::getNormalized(float& fTarget, const std::string& sVectorName, const int& iIndex)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		fTarget = vNormalizedValues(ind->second.index + iIndex);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

float* VectorSpace::getPointerAndSetSize(const std::string& sVectorName, int* iSizePtr)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		*iSizePtr = (int)ind->second.size;
		return vValues.data() + ind->second.index;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

float* VectorSpace::getNormalizedPointerAndSetSize(const std::string& sVectorName, int* iSizePtr)
{
	try
	{
		std::unordered_map<std::string, numberedInfos>::const_iterator ind = findIndex(sVectorName);
		*iSizePtr = (int)ind->second.size;
		return vNormalizedValues.data() + ind->second.index;
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

const float& VectorSpace::getValue(int iIndex)
{
	return vValues(iIndex);
}

Eigen::VectorXf VectorSpace::getValue(int iIndex, int iSize)
{
	return vValues(Eigen::seqN(iIndex, iSize));
}

void VectorSpace::setValue(int iIndex, float fNewValue)
{
	vValues(iIndex) = fNewValue;
	vNormalizedValues(iIndex) = fNewValue / (vNormalizer(iIndex) + EPS);
}

void VectorSpace::setValue(int iIndex, Eigen::VectorXf vNewValue)
{
	vValues.segment(iIndex, vNewValue.size()) = vNewValue;
	vNormalizedValues.segment(iIndex, vNewValue.size()) = vNewValue.array() / (vNormalizer(Eigen::seqN(iIndex, vNewValue.size())).array() + EPS);
}

const float& VectorSpace::getNormalizedValue(int iIndex)
{
	return vNormalizedValues(iIndex);
}

Eigen::VectorXf VectorSpace::getNormalizedValue(int iIndex, int iSize)
{
	return vNormalizedValues(Eigen::seqN(iIndex, iSize));
}

void VectorSpace::setNormalizedValue(int iIndex, float fNewValue)
{
	vNormalizedValues(iIndex) = fNewValue;
	vValues(iIndex) = fNewValue * vNormalizer(iIndex);
}

void VectorSpace::setNormalizedValue(int iIndex, Eigen::VectorXf vNewValue)
{
	vNormalizedValues.segment(iIndex, vNewValue.size()) = vNewValue;
	vValues.segment(iIndex, vNewValue.size()) = vNewValue * vNormalizer(iIndex);
}

void VectorSpace::setAllValuesToZero()
{
	vValues.setConstant(0.0f);
	vNormalizedValues = vValues.array() / (vNormalizer.array() + EPS);
}

VectorSpace VectorSpace::operator+(const VectorSpace& vsOther)
{
	try
	{
		checkOperation(vsOther, "summation");
		VectorSpace out;
		out.vValues = vValues + vsOther.vValues;
		out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
		out.mapInfo = mapInfo;
		return out;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

VectorSpace VectorSpace::operator+(const float& fOther)
{
	VectorSpace out;
	out.vValues = vValues.array() + fOther;
	out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
	out.mapInfo = mapInfo;
	return out;
}

VectorSpace VectorSpace::operator-(const VectorSpace& vsOther)
{
	try
	{
		checkOperation(vsOther, "subtraction");
		VectorSpace out;
		out.vValues = vValues - vsOther.vValues;
		out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
		out.mapInfo = mapInfo;
		return out;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

VectorSpace VectorSpace::operator-(const float& fOther)
{
	VectorSpace out;
	out.vValues = vValues.array() - fOther;
	out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
	out.mapInfo = mapInfo;
	return out;
}

VectorSpace VectorSpace::operator*(const VectorSpace& vsOther)
{
	try
	{
		checkOperation(vsOther, "multiplication");
		VectorSpace out;
		out.vValues = vValues * vsOther.vValues;
		out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
		out.mapInfo = mapInfo;
		return out;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

VectorSpace VectorSpace::operator*(const float& fOther)
{
	VectorSpace out;
	out.vValues = vValues.array() * fOther;
	out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
	out.mapInfo = mapInfo;
	return out;
}

VectorSpace VectorSpace::operator/(const VectorSpace& vsOther)
{
	try
	{
		checkOperation(vsOther, "division");
		VectorSpace out;
		out.vValues = vValues.array() / (vsOther.vValues.array() + EPS);
		out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
		out.mapInfo = mapInfo;
		return out;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

VectorSpace VectorSpace::operator/(const float& fOther)
{
	VectorSpace out;
	out.vValues = vValues.array() / (fOther + EPS);
	out.vNormalizedValues = out.vValues.array() / (vNormalizer.array() + EPS);
	out.mapInfo = mapInfo;
	return out;
}

void VectorSpace::operator=(const VectorSpace& vsOther)
{
	try
	{
		checkOperation(vsOther, "assigning");
		VectorSpace out;
		vValues = vsOther.vValues.array();
		vNormalizedValues = vValues.array() / (vNormalizer.array() + EPS);
		mapInfo = vsOther.mapInfo;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}
