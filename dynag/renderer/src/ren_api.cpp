#include "ren_api.h"


Window* createWindow(const unsigned int WIDTH,
		const unsigned int HEIGHT,
		const char* title)
{
	return new Window(WIDTH, HEIGHT, title);
}


void close(Window* window)
{
	glfwSetWindowShouldClose(window->wWindow, true);
}


bool isClose(Window* window)
{
	return glfwWindowShouldClose(window->wWindow);
}


void render(Window* window)
{
	window->render();
}


void terminateWindow()
{
	glfwTerminate();
}

Model* createModel(char* model_path, char* vertex_shader_file_path, char* fragment_shader_file_path)
{
	return new Model(model_path, vertex_shader_file_path, fragment_shader_file_path);
}


void addPermanent2Window(Window* window, Model* model)
{
	window->addPermanentDrawables(model);
}


void addInstantaneous2Window(Window* window, Model* model)
{
	window->addInstantaneousDrawables(model);
}


void translateModel(Model* model, float x, float y, float z)
{
	model->translate(glm::vec3(x, y, z));
}


void rotateModel(Model* model, float angle, float x, float y, float z)
{
	model->rotate(angle, glm::vec3(x, y, z));
}


void scaleModel(Model* model, float x, float y, float z)
{
	model->scale(glm::vec3(x, y, z));
}


float getFps(Window* window)
{
	return window->fFPS;
}


void setFps(Window* window, float fps)
{
	window->fFPSLimit = fps;
	std::chrono::nanoseconds dt{static_cast<long int>( 1000000000.0f/window->fFPSLimit)};
	window->secDt = dt;
}


Camera* getCamera(Window* window)
{
	return window->cmCamera;
}


void setCameraPos(Camera* camera, float x, float y, float z)
{
	camera->vPosition = glm::vec3(x, y, z);
	camera->updateCameraVectors();
}


float* getCameraPos(Camera* camera)
{
	return glm::value_ptr(camera->vPosition);
}



bool isVisible(Window* window)
{
	int visible = glfwGetWindowAttrib(window->wWindow, GLFW_VISIBLE);
	return visible;
}


void hideWindow(Window* window)
{
	glfwHideWindow(window->wWindow);
}


void showWindow(Window* window)
{
	glfwShowWindow(window->wWindow);
}

int createGuiTextVector(Window* window, const char* title,
									float pos_x, float pos_y,
									float size_x, float size_y)
{
	ImVec2 pos = ImVec2(pos_x, pos_y);
	ImVec2 size = ImVec2(size_x, size_y);
	return window->createGuiText(title, pos, size);
}

void addGuiTextAllLines(Window* window, int v_guiText_ind, int size, char** _str, float* _val)
{
	for (int i = 0; i < size; i++)
		window->addItem2GuiText(v_guiText_ind, _str[i], &_val[i] );
}

void addGuiTextLine(Window* window, int v_guiText_ind, char* _text, float* _val)
{
	float* v = (float*)malloc(1);
	window->addItem2GuiText(v_guiText_ind, strdup(_text), v);
}

void setGuiTextAllValues(Window* window, int v_guiText_ind, int size, float* _val)
{
	for (int i = 0; i < size; i++)
	{
		window->setGuiTextLineValue(v_guiText_ind, i, &_val[i]);
	}
}

void setGuiTextAllTextLines(Window* window, int v_guiText_ind, int size, char** _text)
{
	for (int i = 0; i < size; i++)
		window->setGuiTextLine(v_guiText_ind, i, _text[i]);
}

void setGuiTextLineValue(Window* window, int v_guiText_ind, int iLineNumber, float* _val)
{
	window->setGuiTextLineValue(v_guiText_ind, iLineNumber, _val);
}

void setGuiTextLine(Window* window, int v_guiText_ind, int iLineNumber, char* _text)
{
	window->setGuiTextLine(v_guiText_ind, iLineNumber, _text);
}


void setBool(Model* model, char* name, bool value)
{
	container<bool>* temp = new container<bool>();
	temp->name = name;
	temp->data = value;
	temp->reset = !value;
	model->vBooleanContainer.push_back(temp);
}

void setInt(Model* model, char* name, int value)
{
	container<int>* temp = new container<int>();
	temp->name = name;
	temp->data = value;
	temp->reset = (int)0;
	model->vIntegerContainer.push_back(temp);
}

void setFloat(Model* model, char* name, float value)
{
	container<float>* temp = new container<float>();
	temp->name = name;
	temp->data = value;
	temp->reset = 0.0f;
	model->vFloatContainer.push_back(temp);
}

void setVec2(Model* model, char* name, float* value)
{
	container<glm::vec2>* temp = new container<glm::vec2>();
	temp->name = name;
	temp->data = glm::make_vec4(value);
	temp->reset = glm::vec2(0.0f);
	model->vVec2Container.push_back(temp);
	delete temp;
}

void setVec3(Model* model, char* name, float* value)
{
	container<glm::vec3>* temp = new container<glm::vec3>();
	temp->name = name;
	temp->data = glm::make_vec3(value);
	temp->reset = glm::vec3(0.0f);
	model->vVec3Container.push_back(temp);
}

void setVec4(Model* model, char* name, float* value)
{
	container<glm::vec4>* temp = new container<glm::vec4>();
	temp->name = name;
	temp->data = glm::make_vec4(value);
	temp->reset = glm::vec4(0.0f);
	model->vVec4Container.push_back(temp);
}

void setMat2(Model* model, char* name, float* value)
{
	container<glm::mat2>* temp = new container<glm::mat2>();
	temp->name = name;
	temp->data = glm::make_mat2(value);
	temp->reset = glm::mat2(0.0f);
	model->vMat2Container.push_back(temp);
}

void setMat3(Model* model, char* name, float* value)
{
	container<glm::mat3>* temp = new container<glm::mat3>();
	temp->name = name;
	temp->data = glm::make_mat3(value);
	temp->reset = glm::mat3(0.0f);
	model->vMat3Container.push_back(temp);
}


void setMat4(Model* model, char* name, float* value)
{
	container<glm::mat4>* temp = new container<glm::mat4>();
	temp->name = name;
	temp->data = glm::make_mat4(value);
	temp->reset = glm::mat4(0.0f);
	model->vMat4Container.push_back(temp);
}