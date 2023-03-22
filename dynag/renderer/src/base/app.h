#ifndef BASE_APPLICATION
#define BASE_APPLICATION

#include "window.h"

#include <string>
#include <unordered_map>

enum ApplicationAPI
{
	kOpenGL,
	kVulkan,
	kUnknown
};

static std::unordered_map<ApplicationAPI, std::string> appApiNames =
{
	{ApplicationAPI::kOpenGL, "OpenGL"},
	{ApplicationAPI::kVulkan, "Vulkan"},
	{ApplicationAPI::kUnknown, "Unknown"}
};

class ApplicationBase
{
public:

	ApplicationBase();
	~ApplicationBase();

	ApplicationBase(const ApplicationBase&) = delete;
	ApplicationBase& operator=(const ApplicationBase&) = delete;

	WindowBase* getWindow() { return m_window.get(); }

	std::string getApiName() { return appApiNames.find(api)->second; }

	virtual ModelBase* createModel(const std::string& path,
		std::string vertex_shader_file_path,
		std::string fragment_shader_file_path) = 0;
	
	ApplicationAPI api = ApplicationAPI::kUnknown;
protected:
	std::unique_ptr<WindowBase> m_window = nullptr;
};
#endif