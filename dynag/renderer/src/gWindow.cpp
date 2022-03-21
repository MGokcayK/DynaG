#include "gWindow.h"

Window::Window(const unsigned int SCR_WIDTH,
                        const unsigned int SCR_HEIGHT,
                        const char* title)
{
    // Initialize GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Handle for Apple.
    #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create Window.
    fScrWidth = (float)SCR_WIDTH;
    fScrHeight = (float)SCR_HEIGHT;
    wWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
    if (wWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    // Set the context.
    glfwMakeContextCurrent(wWindow);

    // To close off the v-sync, set swap interwal of glfw.
    glfwSwapInterval(0);

    // Initialize GLAD.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Enable Global OpenGL properties.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);  
    glEnable(GL_MULTISAMPLE);

    // Create camera.
    cmCamera = new Camera(glm::vec3(5.415f, 0.2f, 30.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    fLastX = SCR_WIDTH / 2.0f;
    fLastY = SCR_HEIGHT / 2.0f;

    // Set Input Mode for mouse buttons.
    glfwSetInputMode(wWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    // Set callbacks functions to the window and window pointer to OpenGL.
    glfwSetWindowUserPointer(wWindow, this);
    glfwSetFramebufferSizeCallback(wWindow, Window::staticFrameBufferSizeCallback);
    glfwSetCursorPosCallback(wWindow, Window::staticMouseCallback);
    glfwSetScrollCallback(wWindow, Window::staticScrollCallback);

    // Create gui.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(wWindow, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // Create Uniform Buffer Object to reduce need memory in GPU

    // for UBObject block
    glGenBuffers(1, &iUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, iUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // define the range of the buffer that links to a uniform binding point which is 0 for UBObjects block
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, iUbo, 0, sizeof(glm::mat4) + sizeof(glm::vec3)); 

    // for LightBlock block
    glGenBuffers(1, &iLight);
    glBindBuffer(GL_UNIFORM_BUFFER, iLight);
    glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // define the range of the buffer that links to a uniform binding point which is 1 for LightBlock block
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, iLight, 0, 4 * sizeof(glm::vec3));

    // use vec4 for handling std140 format for uniform buffer objects
    vLightPosition = glm::vec4(0.0f, 7500.0f, 0.0f, 0.0f);
    glm::vec4 light_ambient = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    glm::vec4 light_diffuse = glm::vec4(255.0 / 255.0f, 241.0 / 255.0f, 242.0 / 255.0f, 0.0f);
    glm::vec4 light_specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, iLight);
    glBufferSubData(GL_UNIFORM_BUFFER,                     0, sizeof(glm::vec4), glm::value_ptr(vLightPosition));
    glBufferSubData(GL_UNIFORM_BUFFER,     sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(light_ambient));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(light_diffuse));
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(light_specular));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // for FogBlock block
    glGenBuffers(1, &iFog);
    glBindBuffer(GL_UNIFORM_BUFFER, iFog);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // define the range of the buffer that links to a uniform binding point which is 2 for FogBlock block
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, iFog, 0, 2 * sizeof(glm::vec4));

    glm::vec4 fog_color = glm::vec4(0.74f, 0.35f, 0.51f, 0.4f);
    glm::vec4 density_grad = glm::vec4(0.002f, 5.0f, 0.0, 0.0); // density & grad in first 2 elements, other for padding
   
    glBindBuffer(GL_UNIFORM_BUFFER, iFog);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(fog_color));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(density_grad));
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Window::render()
{
    if (!glfwWindowShouldClose(wWindow))
    {
        // Clear window for new frame.
        glClearColor(61.0f/255.0f, 89.0f/255.0f, 129.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                
       
        // Get procection matrix.
        glm::mat4 projection = glm::perspective(glm::radians(cmCamera->fZoom), fScrWidth / fScrHeight, 0.1f, 10000.0f);

        // Camera/view transformation.
        glm::mat4 view = cmCamera->getViewMatrix();
        mProjectionView = projection * view;

        // Set project_view as Uniform Buffer Objects 
        glBindBuffer(GL_UNIFORM_BUFFER, iUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mProjectionView));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Set camera position as Uniform Buffer Objects
        glBindBuffer(GL_UNIFORM_BUFFER, iUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(cmCamera->vPosition));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Draw objects.
        this->draw();     

        // Render gui.
        this->renderGUI();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap buffer to render the context
        glfwSwapBuffers(wWindow);

        // Poll OpenGL events.
        glfwPollEvents();        

        // If OpenGL FPS is higher than dynamics' FPS (which calculated in Python side)
        // wait some times to sync them.

        // Calculate passed time from last rendered frame.
        durDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::steady_clock::now() - tpLastFrame);

        // Calculate difference of passed time and dt which is calculated by FPS.
        auto diff =  (secDt - durDeltaTime);

        // If there is difference, wait that much time.
        if (diff.count() > 0)
        {
            this->preciseSleep(diff.count()/1e9);
        } 

        // Calculate FPS from last rendered frame.
        auto dt1 =  std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - tpLastFrame);
        fFPS = 1.0f / (dt1.count() / 1e9f);

        // Set current time as last frame time.
        tpLastFrame = std::chrono::steady_clock::now();
    }
    else
    {
        // If window should close. Close the Dear ImGui.
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}


void Window::preciseSleep(double seconds) 
{
    // Create temporary variables.
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    // Wait upto seconds.
    while (seconds > estimate) {
        auto start = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        auto end = std::chrono::steady_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // Spin lock handling.
    auto start = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - start).count() / 1e9 < seconds);
}

void Window::renderGUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (int s = 0; s < vGuiTextSection.size(); s++)
    {
        // Set window position and size.
        ImGui::SetNextWindowPos(vGuiTextSection[s]->vPosition);
        ImGui::SetNextWindowSize(vGuiTextSection[s]->vSize);

        // Render texts.
        vGuiText = *vGuiTextSection[s]->vText;
        
        ImGui::Begin(vGuiTextSection[s]->sTitle.c_str());
        
        for (int i = 0; i < vGuiText.size(); i++)
        {
            ImGui::Text(vGuiText[i].str, *vGuiText[i].val);
        }
        
        ImGui::End();
    }

    // Render the gui.
    ImGui::Render();
}


void Window::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // If first frame, set the lastX & lastY as default parameters.
    if (bFirstMouse)
    {
        fLastX = (float)xpos;
        fLastY = (float)ypos;
        bFirstMouse = false;
    }

    // Set offsets.
    fXOffSet = (float)xpos - fLastX;
    fYOffSet = fLastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    // Set lastX & lastY.
    fLastX = (float)xpos;
    fLastY = (float)ypos;

    /*
    // If left mouse button pressed, change the camera angles.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        cmCamera->processMouseMovement(-fXOffSet, -fYOffSet);
    } 
    */
}


void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    //cmCamera->processMouseScroll((float)yoffset);
}


void Window::staticFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* temp_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    temp_window->frameBufferSizeCallback(window, width, height);
}


void Window::staticMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    Window* temp_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    temp_window->mouseCallback(window, xpos, ypos);
}


void Window::staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Window* temp_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    temp_window->scrollCallback(window, xoffset, yoffset);
}


void Window::addPermanentDrawables(Model* drawable)
{
	vPermanentDrawables.push_back(drawable);
}

void Window::addInstantaneousDrawables(Model* drawable)
{
	vInstantaneousDrawables.push_back(drawable);
}


void Window::draw()
{
	for (int i = 0; i < vPermanentDrawables.size(); i++)
	{
		vPermanentDrawables[i]->draw();
	}

	for (int i = 0; i < vInstantaneousDrawables.size(); i++)
	{
		vInstantaneousDrawables[i]->draw();
	}
	vInstantaneousDrawables.clear();
}


int Window::createGuiText(const char* title, ImVec2 position, ImVec2 size)
{
    guiTextSection* temp = new guiTextSection();
    temp->sTitle = title;
    temp->vText = new std::vector<guiText>();
    temp->vPosition = position;
    temp->vSize = size;
    vGuiTextSection.push_back(temp);
    
    return (int)vGuiTextSection.size() - 1;
} 


void Window::addItem2GuiText(int iGuiTextInd, const char* str, float* fValue)
{
    guiText* temp = new guiText((char*) str, fValue);
    vGuiTextSection[iGuiTextInd]->vText->push_back(*temp);
    delete temp;
}

void Window::setGuiTextLineValue(int iGuiTextInd, int iLineNumber, float* fValue)
{
    *vGuiTextSection[iGuiTextInd]->vText->at(iLineNumber).val = *fValue;
}

void Window::setGuiTextLine(int iGuiTextInd, int iLineNumber, char* cText)
{
    vGuiTextSection[iGuiTextInd]->vText->at(iLineNumber).str = strdup(cText);
}

