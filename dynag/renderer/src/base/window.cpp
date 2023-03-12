#include "window.h"

void WindowBase::preciseSleep(double seconds)
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
        m2 += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // Spin lock handling.
    auto start = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - start).count() / 1e9 < seconds);
}


void WindowBase::mouseCallback(GLFWwindow* window, double xpos, double ypos)
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


    // If left mouse button pressed, change the camera angles.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        cmCamera->processMouseMovement(-fXOffSet, -fYOffSet);
    }

}


void WindowBase::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    cmCamera->processMouseScroll((float)yoffset);
}


void WindowBase::staticFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    WindowBase* temp_window = static_cast<WindowBase*>(glfwGetWindowUserPointer(window));
    temp_window->frameBufferSizeCallback(window, width, height);
}


void WindowBase::staticMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    WindowBase* temp_window = static_cast<WindowBase*>(glfwGetWindowUserPointer(window));
    temp_window->mouseCallback(window, xpos, ypos);
}


void WindowBase::staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    WindowBase* temp_window = static_cast<WindowBase*>(glfwGetWindowUserPointer(window));
    temp_window->scrollCallback(window, xoffset, yoffset);
}

void WindowBase::addPermanentDrawables(ModelBase* drawable)
{
    vPermanentDrawables.push_back(drawable);
}

void WindowBase::addInstantaneousDrawables(ModelBase* drawable)
{
    vInstantaneousDrawables.push_back(drawable);
}


int WindowBase::createGuiText(const char* title, ImVec2 position, ImVec2 size)
{
    guiTextSection* temp = new guiTextSection();
    temp->sTitle = title;
    temp->vText = new std::vector<guiText>();
    temp->vPosition = position;
    temp->vSize = size;
    vGuiTextSection.push_back(temp);

    return (int)vGuiTextSection.size() - 1;
}


void WindowBase::addItem2GuiText(int iGuiTextInd, const char* str, float* fValue)
{
    guiText* temp = new guiText((char*)str, fValue);
    vGuiTextSection[iGuiTextInd]->vText->push_back(*temp);
    delete temp;
}

void WindowBase::setGuiTextLineValue(int iGuiTextInd, int iLineNumber, float* fValue)
{
    *vGuiTextSection[iGuiTextInd]->vText->at(iLineNumber).val = *fValue;
}

void WindowBase::setGuiTextLine(int iGuiTextInd, int iLineNumber, char* cText)
{
    vGuiTextSection[iGuiTextInd]->vText->at(iLineNumber).str = strdup(cText);
}

