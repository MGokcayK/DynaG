#include "error_handler.h"

#if PLATFORM_ID == 0
    gStackWalkPtr gWalker = &GetStackWalkWIN;
#elif PLATFORM_ID == 1
    gStackWalkPtr gWalker = &GetStackWalkLINUX;
#endif

ErrorHandler::ErrorHandler(const std::string& message, const char* file, unsigned int line) :
    std::runtime_error(message)
{
    _message = "Error thrown from " + std::string(file) + ":" + std::to_string(line) + "\n\t " + message;
}

const char* ErrorHandler::what() const throw()
{
    gWalker();
    return _message.c_str();
}





#if _WIN32
void GetStackWalkWIN()
{
    #if _WIN64
        DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    #else
        DWORD machine = IMAGE_FILE_MACHINE_I386;
    #endif
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    if (SymInitialize(process, NULL, TRUE) == FALSE)
    {
        gThrow(__FUNCTION__ ": Failed to call SymInitialize.");
    }

    CONTEXT    context = {};
    context.ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(&context);

    #if _WIN64
        STACKFRAME frame = {};
        frame.AddrPC.Offset = context.Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Rsp;
        frame.AddrStack.Mode = AddrModeFlat;
    #else
        STACKFRAME frame = {};
        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
    #endif

    std::vector<StackFrame> vStackFrame;
    for (int i = 0; i < 11; i++)
    {
        StackWalk(machine, process, thread, &frame, &context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL);

        StackFrame f = {};
        f.address = frame.AddrPC.Offset;

        #if _WIN64
                DWORD64 moduleBase = 0;
                DWORD64 offset = 0;
                DWORD offset_ln = 0;
                IMAGEHLP_LINE64 line;
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        #else
                DWORD moduleBase = 0;
                DWORD offset = 0;
                DWORD offset_ln = 0;
                IMAGEHLP_LINE line;
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        #endif

        moduleBase = SymGetModuleBase(process, frame.AddrPC.Offset);

        char moduleBuff[MAX_PATH];
        if (moduleBase && GetModuleFileNameA((HINSTANCE)moduleBase, moduleBuff, MAX_PATH))
        {
            f.module = basename(moduleBuff);
        }
        else
        {
            f.module = "Unknown Module";
        }

        char                name[(MAX_PATH * sizeof(TCHAR))];
        char                symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + (sizeof(name))];

        IMAGEHLP_SYMBOL64* symbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
        symbol->SizeOfStruct = sizeof(symbolBuffer);
        symbol->MaxNameLength = sizeof(name);

        if (SymGetSymFromAddr(process, (DWORD64)frame.AddrPC.Offset, &offset, symbol))
        {
            UnDecorateSymbolName(symbol->Name, (PSTR)name, sizeof(name), UNDNAME_COMPLETE);
            f.name = symbol->Name;
        }
        else
        {
            DWORD error = GetLastError();
            f.name = "Unknown Function, Failed to resolve address. Error";
        }

        if (SymGetLineFromAddr(process, frame.AddrPC.Offset, &offset_ln, &line))
        {
            f.file = line.FileName;
            f.line = (int)line.LineNumber;
        }
        else
        {
            DWORD error = GetLastError();
            f.line = error;
            f.file = "File Not Found, Failed to resolve adress. Error";
        }
        vStackFrame.push_back(f);
    }

    std::cout << "Stack Tracing.." << std::endl;
    for (int i = (int)vStackFrame.size() - 1; i > 6; i--)
    {
        std::cout << "Error Adress : 0x" << std::hex << vStackFrame[i].address << std::endl
                  << "\tin module : " << vStackFrame[i].module << std::endl
                  << "\tfrom `" << vStackFrame[i].name << "` at `" 
                  << vStackFrame[i].file << ":" << std::dec << vStackFrame[i].line << "`\n"<< std::endl;
    }
    
    SymCleanup(process);
}

std::string basename(const std::string& file)
{
    size_t i = file.find_last_of("\\/");
    if (i == std::string::npos)
    {
        return file;
    }
    else
    {
        return file.substr(i + 1);
    }
}
#endif

#if linux
void GetStackWalkLINUX()
{
    std::vector <std::string> vErrorMessage;
    std::string sTemp = {};

    char cCommandOutput[1024];
    FILE *filePtr;
    
    std::string strSymbol = "";
    int firstAdd, endAdd, firstFunc, endFunc;
    
    void *stack[8];
    char **symbols = (char **)NULL;
    
    int stackSize = backtrace(stack, sizeof(stack)/sizeof(stack[0]));
    symbols = backtrace_symbols(stack, stackSize);
    
    for (int i = 0; i < stackSize; i++)
    {
        sTemp = "";
        strSymbol = std::string(symbols[i]);        
        
        firstAdd = strSymbol.find_last_of("[");
        endAdd = strSymbol.find_last_of("]");
        firstFunc = strSymbol.find_last_of("(");
        endFunc = strSymbol.find_last_of(")");

        sTemp += "Error Adress : " + strSymbol.substr(firstAdd+1, endAdd-firstAdd-1) + 
            "\n\tin module " + strSymbol.substr(0, strSymbol.find_first_of("(")) + "\n\tfrom ";
        char location[1024];
        Dl_info info;
        if(dladdr(stack[i],&info))
        {
            char command[256];
            size_t VMA_addr=ConvertToVMA((size_t)stack[i]);
            VMA_addr-=1;
            snprintf(command, sizeof(command),"addr2line -f -e %s -p -Ci %zx",info.dli_fname,VMA_addr);
           
            filePtr = popen(command, "r");

            char* a = fgets(cCommandOutput, 1024, filePtr);
            sTemp += std::string(cCommandOutput);
            vErrorMessage.push_back(sTemp);
        }
    }

    for (int i = vErrorMessage.size()-1; i > 2; i--) std::cout << vErrorMessage[i] << std::endl;

}


// converts a function's address in memory to its VMA address in the executable file. VMA is what addr2line expects
size_t ConvertToVMA(size_t addr)
{
  Dl_info info;
  link_map* link_map;
  dladdr1((void*)addr,&info,(void**)&link_map,RTLD_DL_LINKMAP);
  return addr-link_map->l_addr;
}
#endif