#include "utils.h"

bool TRIMMING = false;
bool READY = false;

yamlChecker::yamlChecker()
{
	setRequiredYamlNodes();
}

void yamlChecker::setRequiredYamlNodes()
{
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "R"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "T0"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "LAPSE"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "HTS"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "RO_SEA"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "GRAV"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "MAX_GR_ALT"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "NS_MAX"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "EW_MAX"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "WIND_DIR"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "WIND_SPD"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "WIND_DIR_RND"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "TURB_LVL"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "HMAP_PATH"));
    mapYamlRequiredNodes.insert(std::make_pair("ENV", "NMAP_PATH"));

    mapYamlRequiredNodes.insert(std::make_pair("HELI", "HP_LOSS"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "VTRANS"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "FS_CG"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "WL_CG"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "WT"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "IX"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "IY"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "IZ"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "IXZ"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "COL_OS"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "COL_L"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "COL_H"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "LON_L"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "LON_H"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "LAT_L"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "LAT_H"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "PED_OS"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "PED_L"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "PED_H"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "COL_COF"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "LON_COF"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "LAT_COF"));
    mapYamlRequiredNodes.insert(std::make_pair("HELI", "PED_COF"));

    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "YAW"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "YAW_RATE"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "N_VEL"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "E_VEL"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "D_VEL"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "N_POS"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "E_POS"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "GR_ALT"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "PSI_MR"));
    mapYamlRequiredNodes.insert(std::make_pair("TRIM", "PSI_TR"));

    mapYamlRequiredNodes.insert(std::make_pair("MR", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "IS"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "E"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "IB"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "R"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "A"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "RPM"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "CD0"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "B"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "C"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "TWST"));
    mapYamlRequiredNodes.insert(std::make_pair("MR", "K1"));

    mapYamlRequiredNodes.insert(std::make_pair("TR", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "R"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "A"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "C"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "RPM"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "CD0"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "TWST"));
    mapYamlRequiredNodes.insert(std::make_pair("TR", "B"));

    mapYamlRequiredNodes.insert(std::make_pair("FUS", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("FUS", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("FUS", "XUU"));
    mapYamlRequiredNodes.insert(std::make_pair("FUS", "YVV"));
    mapYamlRequiredNodes.insert(std::make_pair("FUS", "ZWW"));
    mapYamlRequiredNodes.insert(std::make_pair("FUS", "COR"));

    mapYamlRequiredNodes.insert(std::make_pair("HT", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("HT", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("HT", "ZUU"));
    mapYamlRequiredNodes.insert(std::make_pair("HT", "ZUW"));
    mapYamlRequiredNodes.insert(std::make_pair("HT", "ZMAX"));

    mapYamlRequiredNodes.insert(std::make_pair("VT", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("VT", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("VT", "YUU"));
    mapYamlRequiredNodes.insert(std::make_pair("VT", "YUV"));
    mapYamlRequiredNodes.insert(std::make_pair("VT", "YMAX"));

    mapYamlRequiredNodes.insert(std::make_pair("WN", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("WN", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("WN", "ZUU"));
    mapYamlRequiredNodes.insert(std::make_pair("WN", "ZUW"));
    mapYamlRequiredNodes.insert(std::make_pair("WN", "ZMAX"));
    mapYamlRequiredNodes.insert(std::make_pair("WN", "B"));

    mapYamlRequiredNodes.insert(std::make_pair("LG", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("LG", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("LG", "B"));
    mapYamlRequiredNodes.insert(std::make_pair("LG", "C"));
    mapYamlRequiredNodes.insert(std::make_pair("LG", "K"));
    mapYamlRequiredNodes.insert(std::make_pair("LG", "MU"));

    mapYamlRequiredNodes.insert(std::make_pair("FLG", "FS"));
    mapYamlRequiredNodes.insert(std::make_pair("FLG", "WL"));
    mapYamlRequiredNodes.insert(std::make_pair("FLG", "C"));
    mapYamlRequiredNodes.insert(std::make_pair("FLG", "K"));
    mapYamlRequiredNodes.insert(std::make_pair("FLG", "MU"));

    mapYamlNodeStructure.insert(std::make_pair("ENV >> ", "Not Connected, Root Node"));
    mapYamlNodeStructure.insert(std::make_pair("HELI >> ", "Not Connected, Root Node"));
    mapYamlNodeStructure.insert(std::make_pair("TRIM >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("MR >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("TR >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("FUS >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("HT >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("VT >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("WN >> ", "Connected to HELI"));
    mapYamlNodeStructure.insert(std::make_pair("LG >> ", "Connected to LG"));
}

void yamlChecker::checkYamlIsProper(YAML::Node& yNode)
{
    for (auto it = yNode.begin(); it != yNode.end(); ++it) 
    {
        YAML::Node key = it->first;
        YAML::Node child = it->second;
        if (child.IsMap()) 
        {
            sParentName = key.as<std::string>();
            checkYamlIsProper(child);            
        }
        else
        {
           look2RequiredNodes(sParentName, key.as<std::string>());
        }
    }
}

void yamlChecker::look2RequiredNodes(const std::string& strParentNodeName, const std::string& strChildNodeName)
{
    for (auto it = mapYamlRequiredNodes.begin(); it != mapYamlRequiredNodes.end(); ++it) 
    {
        if ((it->first == strParentNodeName) && (it->second == strChildNodeName))
        {
            it = mapYamlRequiredNodes.erase(it);
            break;
        }
    }
}

void yamlChecker::printNodeStructure()
{
    for (auto it = mapYamlNodeStructure.begin(); it != mapYamlNodeStructure.end(); ++it)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

void checkYamlFile(YAML::Node& nYamlNode, char* cYamlPathPtr)
{
    if (nYamlNode.IsNull()) std::cout << cYamlPathPtr << " has error!" << std::endl;
    yamlChecker* sYamlCheckerPtr = new yamlChecker();
    sYamlCheckerPtr->checkYamlIsProper(nYamlNode);
    try 
    {
        if (sYamlCheckerPtr->mapYamlRequiredNodes.size() > 0)
        {
            std::cout << "==========================================\n" <<
                "            NODE STRUCTURE                " << std::endl;
            sYamlCheckerPtr->printNodeStructure();
            std::cout << "==========================================" << std::endl;
            for (auto it = sYamlCheckerPtr->mapYamlRequiredNodes.begin(); it != sYamlCheckerPtr->mapYamlRequiredNodes.end(); ++it)
                gThrow(it->second + " in node " + it->first + " is not found in `" + cYamlPathPtr + "`");           
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }    
    delete sYamlCheckerPtr;
}
