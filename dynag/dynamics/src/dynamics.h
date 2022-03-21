/*
* DynaG Dynamics C++ / DynamicSystem
* 
* Dynamic System is a system which can be used for defining a dynamic system. 
* It has own solver which is Runge-Kutta-4th order.
* 
* Author : @MGokcayK
* 
* C.Date : 15/09/2021
* Update : 15/09/2021 - Class Initialization // @MGokcayK
* Update : 17/09/2021 - Class Implementation // @MGokcayK
* Update : 06/11/2021 - Adding map for yaml nodes. // @MGokcayK
* Update : 29/11/2021 - Adding Observation, Action register methods and checking yaml file. // @MGokcayK
* Update : 18/12/2021 - Adding setValueInYamlNode method. // @MGokcayK
*/

#pragma once

#include "vectorspace.h"
#include "kinematics.h"

class DynamicSystem
{
private:
	// Finding index of map whether if it is exist or not. If not, throw error.
	std::unordered_map<std::string, YAML::Node>::const_iterator findIndex(const std::string& sNodeName, const std::string& sVariableName);

public :
	int iNumberOfAct = 1; // number of action

	// Variables of dynamics system for 4th order Runge-Kutta implementation.
	VectorSpace vsStateDot0 = { std::string{"StateDot0"} }, 
				vsStateDot1 = { std::string{"StateDot1"} }, 
				vsStateDot2 = { std::string{"StateDot2"} }, 
				vsStateDot3 = { std::string{"StateDot3"} }; // RK4 parameters
	VectorSpace vsState0 = { std::string{"States0"} }; // Used for copying state at the beginning of solving a time step.
	VectorSpace vsAction0 = { std::string{"Action0"} }; // Used for copying state at the beginning of solving a time step.
	VectorSpace vsState = { std::string{"State"} };
	VectorSpace vsStateDot = { std::string{"StatesDot"} };
	float fDt = 0.01f;
	VectorSpace vsObservation = { std::string{"Observation"} };
	VectorSpace vsAction = { std::string{"Action"} };
	VectorSpace vsTrimVariables = { std::string{"TrimVariables"} };
	VectorSpace vsTrimOutputs = { std::string{"TrimOutputs"} };
	// VectorSpace vsTrimCondition = { std::string{"Trim Condition"} };
	VectorSpace vsTrimTargets = { std::string{"TrimTargets"} }; // kind of old trim condition
	Eigen::VectorXf vReset;


	// Yaml Nodes Map
	std::unordered_map<std::string, YAML::Node> node_map = {};

	// Constructor of Dynamics System.
	DynamicSystem(const float& fDt = 0.01f);

	// Destuctor of Dynamics System.
	virtual ~DynamicSystem() {};

	// Setting step size of RK4.
	void setDt(float* fDt);

	// Setting action size of dynamics system.
	void setActionSize(int iNumberOfAct);

	// Base preCalculations method.
	virtual void preCalculations() {};

	// Base calculation of dynamics of Dynamics System.
	virtual void dynamics() = 0;

	// Base reset function of Dynamics System.
	virtual void reset(const VectorSpace& vsTrimCondition = {}) = 0;

	// Base getting actions method.
	virtual void getActions() {};

	// Base getting states method.
	virtual void getStates() {};

	// Base setting actions method.
	virtual void setActions() {};

	// Base setting states method.
	virtual void setStates() {};

	// Base setting statesdot method.
	virtual void setStateDots() {};

	// Base setting observations method.
	virtual void setObservations() {};

	// Base getting number of obsevation of Dynamics System.
	virtual int getNumberOfObservations() = 0;

	// Registering all the state with normalizer into VectorSpace which also named as state.
	virtual void registerStates() {};

	// Registering all the observations as VectorSpace.
	virtual void registerObservations() {};

	// Registering all the actions as VectorSpace.
	virtual void registerActions() {};

	// Registering the state with normalizer into VectorSpace which also named as state.
	void registerState(const std::string& sName, const Eigen::VectorXf& vValue, const float& fNormalizer = 1.0f);

	// Registering the observations as VectorSpace.
	void registerObservation(const std::string& sName, const Eigen::VectorXf& vValue, const float& fNormalizer = 1.0f);

	// Registering the actions as VectorSpace.
	void registerAction(const std::string& sName, const Eigen::VectorXf& vValue, const float& fNormalizer = 1.0f);

	// Step RK4.
	void step();

	// Base start method for step method. Sometimes some methods should be called before
	// the RK4 execution such as generating random variables. To handle this, step_start method called.
	virtual void stepStart() {};

	// Base end method for step method. Sometimes some methods should be called after
	// the RK4 execution such as bounding. To handle this, step_end method called.
	virtual void stepEnd() {};

	// Getting value from yaml node w.r.t node's name and variable name.
	float getValueFromYamlNode(const std::string& sNodeName, const std::string& sVariableName);

	// Setting value of yaml node from Python side without changing `.yaml` file.
	void setValueInYamlNode(const std::string& sNodeName, const std::string& sVariableName, const float& fValue);
};
