#include "dynamics.h"

std::unordered_map<std::string, YAML::Node>::const_iterator DynamicSystem::findIndex(const std::string& sNodeName, const std::string& sVariableName)
{
	std::unordered_map<std::string, YAML::Node>::const_iterator ind = node_map.find(sNodeName);
	if (ind != node_map.end())
	{
		return ind;
	}
	else
	{
		gThrow("Not Found : Node named as '" + sNodeName + "' or variable named as '" + sVariableName + "'not found in " + typeid(*this).name() + "!");
	}
}

DynamicSystem::DynamicSystem(const float& fDt)
{
	this->fDt = fDt;
}

void DynamicSystem::setDt(float* fDt)
{
	this->fDt = *fDt;
}

void DynamicSystem::setActionSize(int iNumberOfAct)
{
	this->iNumberOfAct = iNumberOfAct;
}

void DynamicSystem::registerState(const std::string& sName, const Eigen::VectorXf& vValue, const float& fNormalizer)
{
	vsState.registerVector(sName, vValue, fNormalizer);
	vsStateDot.registerVector(sName, vValue, fNormalizer);
	vsState0.registerVector(sName, vValue, fNormalizer);
	vsStateDot0.registerVector(sName, vValue, fNormalizer);
	vsStateDot1.registerVector(sName, vValue, fNormalizer);
	vsStateDot2.registerVector(sName, vValue, fNormalizer);
	vsStateDot3.registerVector(sName, vValue, fNormalizer);
}

void DynamicSystem::registerObservation(const std::string& sName, const Eigen::VectorXf& vValue, const float& fNormalizer)
{
	vsObservation.registerVector(sName, vValue, fNormalizer);
}

void DynamicSystem::registerAction(const std::string& sName, const Eigen::VectorXf& vValue, const float& fNormalizer)
{
	vsAction.registerVector(sName, vValue, fNormalizer);
	vsAction0.registerVector(sName, vValue, fNormalizer);
}

void DynamicSystem::step()
{
	getActions(); 
	stepStart();
	setActions();
	
	vsState0 = vsState;

	// No need to set vsState at first step..
	getStates();
	dynamics(); // Call with x0
	setStateDots();
	vsStateDot0 = vsStateDot; // evaluate k0
	if (READY == false)	return;

	vsState = vsState0 + vsStateDot0 * (0.5f * fDt); // find x1, and set it
	getStates();
	dynamics();  // call with x1
	setStateDots();
	vsStateDot1 = vsStateDot; // evaluate k1
	if (READY == false)	return;

	vsState = vsState0 + vsStateDot1 * (0.5f * fDt); // find x2, and set it
	getStates();
	dynamics(); // call with x2
	setStateDots();
	vsStateDot2 = vsStateDot; // evaluate k2
	if (READY == false)	return;

	vsState = vsState0 + vsStateDot2 * fDt; // find x3, and set it
	getStates();
	dynamics();  // call with x3
	setStateDots();
	vsStateDot3 = vsStateDot; // evaluate k3 
	if (READY == false)	return;

	vsStateDot0 = (vsStateDot0 + vsStateDot1 * 2.0f + vsStateDot2 * 2.0f + vsStateDot3) * 0.16666666666666666f;
	
	vsState = vsState0 + vsStateDot0 * fDt;
	vsStateDot = (vsStateDot0 * (-2.0f) + vsStateDot1 * 2.0f + vsStateDot2 * 2.0f + vsStateDot3) * 0.333333333333333f;

	getStates();
	stepEnd();
	setStates();
	if (READY == false)	return;
	
	setObservations();
}

float DynamicSystem::getValueFromYamlNode(const std::string& sNodeName, const std::string& sVariableName)
{
	try
	{
		std::unordered_map<std::string, YAML::Node>::const_iterator ind = findIndex(sNodeName, sVariableName);
		return ind->second[sVariableName].as<float>();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void DynamicSystem::setValueInYamlNode(const std::string& sNodeName, const std::string& sVariableName, const float& fValue)
{
	try
	{
		std::unordered_map<std::string, YAML::Node>::const_iterator ind = findIndex(sNodeName, sVariableName);
		YAML::Node tempNode = ind->second;
		tempNode[sVariableName] = fValue;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}
