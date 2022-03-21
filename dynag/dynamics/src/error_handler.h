/*
* DynaG Dynamics C++ / ErrorHandler
*
* ErrorHandler basic error handling class.
*
* Author : @MGokcayK
*
* C.Date : 19/12/2021
* Update : 19/12/2021 - Class Initialization // @MGokcayK
* Update : 20/12/2021 - Adding macro and platform dependence. // @MGokcayK
* Update : 21/12/2021 - Adding StackWalking Mechanism. // @MGokcayK
*/

#pragma once
#include "utils.h"

// Define basic Throw macro for ErrorHandler.
#define gThrow(message) throw ErrorHandler(message, __FILE__, __LINE__);

// Function Pointer for StackWalking w.r.t PLATFORM_ID.
typedef void (*gStackWalkPtr)();


#ifdef _WIN32
    #include <Windows.h>
    #include <DbgHelp.h>
    #define PLATFORM_ID 0
    
    // StackFrame Structure to print proper info.
    struct StackFrame
    {
        DWORD64 address;
        std::string name;
        std::string module;
        unsigned int line;
        std::string file;
    };

    // Windows OS Stack Walking.
    // Modified from https://www.rioki.org/2017/01/09/windows_stacktrace.html.
    void GetStackWalkWIN();

    // Extract Basename
    std::string basename(const std::string& file);
#elif linux
    #include <execinfo.h>
    #include <link.h>
    #include <stdlib.h>
    #include <stdio.h>
    #define PLATFORM_ID 1
    
    // Linux StackWalking
    // Modified from https://stackoverflow.com/questions/56046062/linux-addr2line-command-returns-0
    void GetStackWalkLINUX();
    
    // Convert Address to VMA
    size_t ConvertToVMA(size_t addr);
#endif



// Basic Error handling class for runtime. 
class ErrorHandler : public std::runtime_error
{
private:
    std::string _message;

public:
    ErrorHandler(const std::string& message, const char* file, unsigned int line);

    ~ErrorHandler() throw() {}

    const char* what() const throw();
};