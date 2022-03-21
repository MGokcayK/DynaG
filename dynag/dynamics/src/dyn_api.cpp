#include "dyn_api.h"

HelicopterDynamics* createHelicopterDynamics(char* yaml_path, float* dt)
{
	return new HelicopterDynamics(yaml_path, dt);
}

bool ready()
{
	return READY;
}

float* getAllAction(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsAction.vValues.size();
	return DS->vsAction.vValues.data();
}

float* getAllNormalizedAction(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsAction.vNormalizedValues.size();
	return DS->vsAction.vNormalizedValues.data();
}

float* getAction(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsAction.getPointerAndSetSize(name, size_s);
}

float* getNormalizedAction(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsAction.getNormalizedPointerAndSetSize(name, size_s);
}

////////////////////////////////////////////////////////////////////////

float* getAllState(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsState.vValues.size();
	return DS->vsState.vValues.data();
}

float* getAllNormalizedState(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsState.vNormalizedValues.size();
	return DS->vsState.vNormalizedValues.data();
}

float* getState(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsState.getPointerAndSetSize(name, size_s);
}

float* getNormalizedState(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsState.getNormalizedPointerAndSetSize(name, size_s);
}

////////////////////////////////////////////////////////////////////////

float* getAllStateDot(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsStateDot.vValues.size();
	return DS->vsStateDot.vValues.data();
}

float* getAllNormalizedStateDot(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsStateDot.vNormalizedValues.size();
	return DS->vsStateDot.vNormalizedValues.data();
}

float* getStateDot(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsStateDot.getPointerAndSetSize(name, size_s);
}

float* getNormalizedStateDot(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsStateDot.getNormalizedPointerAndSetSize(name, size_s);
}

////////////////////////////////////////////////////////////////////////

float* getAllObservation(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsObservation.vValues.size();
	return DS->vsObservation.vValues.data();
}

float* getAllNormalizedObservation(DynamicSystem* DS, int* size_s)
{
	*size_s = (int)DS->vsObservation.vNormalizedValues.size();
	return DS->vsObservation.vNormalizedValues.data();
}

float* getObservation(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsObservation.getPointerAndSetSize(name, size_s);
}

float* getNormalizedObservation(DynamicSystem* DS, char* name, int* size_s)
{
	return DS->vsObservation.getNormalizedPointerAndSetSize(name, size_s);
}

////////////////////////////////////////////////////////////////////////

float getValueFromYamlNode(DynamicSystem* DS, char* node_name, char* var_name)
{
	return DS->getValueFromYamlNode(node_name, var_name);
}

void setValueInYamlNode(DynamicSystem* DS, char* node_name, char* var_name, float* value)
{
	DS->setValueInYamlNode(node_name, var_name, *value);
	DS->preCalculations();
}

void step(DynamicSystem* DS, float* action)
{
	DS->vsAction.setValues(action);

	DS->step();
}

void reset(DynamicSystem* DS)
{
	DS->reset();
}

int getNumberOfObservations(DynamicSystem* DS)
{
	return DS->getNumberOfObservations();
}
