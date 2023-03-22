#include "ren_api.h"


ApplicationBase* createApplication(const unsigned int _width,
		const unsigned int _height,
		const char* _title,
		const bool _vulkan)
{
	if (_vulkan) return new Vulkan::Application(_width, _height, _title);
	else return new OpenGL::Application(_width, _height, _title);
}


void close(ApplicationBase* app)
{
	glfwSetWindowShouldClose(app->getWindow()->wWindow, true);
}


bool isClose(ApplicationBase* app)
{
	return glfwWindowShouldClose(app->getWindow()->wWindow);
}


void render(ApplicationBase* app)
{
	app->getWindow()->render();
}


void terminateApplication()
{
	glfwTerminate();
}

ModelBase* createModel(ApplicationBase* app, char* model_path, char* vertex_shader_file_path, char* fragment_shader_file_path)
{
	if (app->api == ApplicationAPI::kVulkan)
	{
		return app->createModel(model_path, vertex_shader_file_path, fragment_shader_file_path);
	}
	else if (app->api == ApplicationAPI::kOpenGL)
	{
		return app->createModel(model_path, vertex_shader_file_path, fragment_shader_file_path);
	}
	else
	{
		std::cout << "Unknown Application!" << std::endl;
		return nullptr;
	}
}


void addPermanent2App(ApplicationBase* app, ModelBase* model)
{
	app->getWindow()->addPermanentDrawables(model);
}


void addInstantaneous2App(ApplicationBase* app, ModelBase* model)
{
	app->getWindow()->addInstantaneousDrawables(model);
}


void translateModel(ModelBase* model, float x, float y, float z)
{
	model->translate(glm::vec3(x, y, z));
}


void rotateModel(ModelBase* model, float angle, float x, float y, float z)
{
	model->rotate(angle, glm::vec3(x, y, z));
}


void scaleModel(ModelBase* model, float x, float y, float z)
{
	model->scale(glm::vec3(x, y, z));
}


float getFps(ApplicationBase* app)
{
	return app->getWindow()->fFPS;
}


void setFps(ApplicationBase* app, float fps)
{
	const auto& window = app->getWindow();
	window->fFPSLimit = fps;
	std::chrono::nanoseconds dt{static_cast<long int>( 1000000000.0f/window->fFPSLimit)};
	window->secDt = dt;
}


Camera* getCamera(ApplicationBase* app)
{
	return app->getWindow()->cmCamera;
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



bool isVisible(ApplicationBase* app)
{
	int visible = glfwGetWindowAttrib(app->getWindow()->wWindow, GLFW_VISIBLE);
	return visible;
}


void hideWindow(ApplicationBase* app)
{
	glfwHideWindow(app->getWindow()->wWindow);
}


void showWindow(ApplicationBase* app)
{
	glfwShowWindow(app->getWindow()->wWindow);
}

int createGuiTextVector(ApplicationBase* app, const char* title,
									float pos_x, float pos_y,
									float size_x, float size_y)
{
	ImVec2 pos = ImVec2(pos_x, pos_y);
	ImVec2 size = ImVec2(size_x, size_y);
	return app->getWindow()->createGuiText(title, pos, size);
}

void addGuiTextAllLines(ApplicationBase* app, int v_guiText_ind, int size, char** _str, float* _val)
{
	const auto& window = app->getWindow();
	for (int i = 0; i < size; i++)
		window->addItem2GuiText(v_guiText_ind, _str[i], &_val[i] );
}

void addGuiTextLine(ApplicationBase* app, int v_guiText_ind, char* _text, float* _val)
{
	float* v = (float*)malloc(1);
	app->getWindow()->addItem2GuiText(v_guiText_ind, strdup(_text), v);
}

void setGuiTextAllValues(ApplicationBase* app, int v_guiText_ind, int size, float* _val)
{
	const auto& window = app->getWindow();
	for (int i = 0; i < size; i++)
	{
		window->setGuiTextLineValue(v_guiText_ind, i, &_val[i]);
	}
}

void setGuiTextAllTextLines(ApplicationBase* app, int v_guiText_ind, int size, char** _text)
{
	const auto& window = app->getWindow();
	for (int i = 0; i < size; i++)
		window->setGuiTextLine(v_guiText_ind, i, _text[i]);
}

void setGuiTextLineValue(ApplicationBase* app, int v_guiText_ind, int iLineNumber, float* _val)
{
	app->getWindow()->setGuiTextLineValue(v_guiText_ind, iLineNumber, _val);
}

void setGuiTextLine(ApplicationBase* app, int v_guiText_ind, int iLineNumber, char* _text)
{
	app->getWindow()->setGuiTextLine(v_guiText_ind, iLineNumber, _text);
}


void setBool(OpenGL::Model* model, char* name, bool value)
{
	OpenGL::container<bool>* temp = new OpenGL::container<bool>();
	temp->name = name;
	temp->data = value;
	temp->reset = !value;
	model->vBooleanContainer.push_back(temp);
}

void setInt(OpenGL::Model* model, char* name, int value)
{
	OpenGL::container<int>* temp = new OpenGL::container<int>();
	temp->name = name;
	temp->data = value;
	temp->reset = (int)0;
	model->vIntegerContainer.push_back(temp);
}

void setFloat(OpenGL::Model* model, char* name, float value)
{
	OpenGL::container<float>* temp = new OpenGL::container<float>();
	temp->name = name;
	temp->data = value;
	temp->reset = 0.0f;
	model->vFloatContainer.push_back(temp);
}

void setVec2(OpenGL::Model* model, char* name, float* value)
{
	OpenGL::container<glm::vec2>* temp = new OpenGL::container<glm::vec2>();
	temp->name = name;
	temp->data = glm::make_vec4(value);
	temp->reset = glm::vec2(0.0f);
	model->vVec2Container.push_back(temp);
	delete temp;
}

void setVec3(OpenGL::Model* model, char* name, float* value)
{
	OpenGL::container<glm::vec3>* temp = new OpenGL::container<glm::vec3>();
	temp->name = name;
	temp->data = glm::make_vec3(value);
	temp->reset = glm::vec3(0.0f);
	model->vVec3Container.push_back(temp);
}

void setVec4(OpenGL::Model* model, char* name, float* value)
{
	OpenGL::container<glm::vec4>* temp = new OpenGL::container<glm::vec4>();
	temp->name = name;
	temp->data = glm::make_vec4(value);
	temp->reset = glm::vec4(0.0f);
	model->vVec4Container.push_back(temp);
}

void setMat2(OpenGL::Model* model, char* name, float* value)
{
	OpenGL::container<glm::mat2>* temp = new OpenGL::container<glm::mat2>();
	temp->name = name;
	temp->data = glm::make_mat2(value);
	temp->reset = glm::mat2(0.0f);
	model->vMat2Container.push_back(temp);
}

void setMat3(OpenGL::Model* model, char* name, float* value)
{
	OpenGL::container<glm::mat3>* temp = new OpenGL::container<glm::mat3>();
	temp->name = name;
	temp->data = glm::make_mat3(value);
	temp->reset = glm::mat3(0.0f);
	model->vMat3Container.push_back(temp);
}


void setMat4(OpenGL::Model* model, char* name, float* value)
{
	OpenGL::container<glm::mat4>* temp = new OpenGL::container<glm::mat4>();
	temp->name = name;
	temp->data = glm::make_mat4(value);
	temp->reset = glm::mat4(0.0f);
	model->vMat4Container.push_back(temp);
}