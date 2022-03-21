/*
* DynaG Dynamics C++ / Utils
*
* Utility functions for DynaG Dynamics.
*
* Author : @MGokcayK
*
* C.Date : 15/09/2021
* Update : 15/09/2021 - Initialization & Adding `extractSubVectorByIndexes` method. // @MGokcayK
* Update : 17/09/2021 - Adding `pi_bound` method. // @MGokcayK
* Update : 29/11/2021 - Adding yamlCheckter structure. // @MGokcayK
*/

#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>
#include <Eigen/Eigen/Dense>
#include <Eigen/Eigen/Core>
#include <math.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <functional>
#include "yaml-cpp/yaml.h"
#include "error_handler.h"

extern bool TRIMMING;
extern bool READY;

constexpr float PI = 3.141592653589793f; // pi
constexpr float TWOPI = 6.283185307179586f; // 2 * pi
constexpr float FTS2KNOT = 0.5924838f; // ft / s to knots conversion;
constexpr float EPS = 1e-6f; // small value for divison by zero
constexpr float R2D = 180.0f / PI; // Rad to deg
constexpr float D2R = 1.0f / R2D;  // Deg to rad
constexpr float FT2MTR = 0.3048f; // ft to meter
constexpr float SQRT_3 = 1.7320508075688772f; // sqrt(3)
constexpr float TWO_D_PI = 0.6366197723675814f; // 2 / pi

struct yamlChecker
{
    std::unordered_multimap<std::string, std::string> mapYamlRequiredNodes;
    std::unordered_multimap<std::string, std::string> mapYamlNodeStructure;
    std::string sParentName = "Root";

    yamlChecker();

    void setRequiredYamlNodes();    
    
    void checkYamlIsProper(YAML::Node& yNode);

    void look2RequiredNodes(const std::string& strParentNodeName, const std::string& strChildNodeName);

    void printNodeStructure();
};

// Checking yaml file is proper or not.
void checkYamlFile(YAML::Node& nYamlNode, char* cYamlPathPtr);

// Extract sub-vector of vector w.r.t indexes.
template <typename T, typename T2>
T extractSubVectorByIndexes(const T& full, const T2& ind)
{
    int num_indices = ind.innerSize();
    T target(num_indices);
    for (int i = 0; i < num_indices; i++)
    {
        target[i] = full[ind[i]];
    }
    return target;
}

// Module of 2pi operator. This implementation is different than
// `fmodf` method. Therefore, it should be reimplemented.
struct mod2PI {
    float operator()(float v) const { return v - floor(v / TWOPI) * TWOPI; }
};

// Bounding Eigen objects w.r.t pi.
template<typename T>
void pi_bound(Eigen::MatrixBase<T>& x)
{
    x = (x.array() + PI);
    x = x.unaryExpr(mod2PI());
    x = x.array() - PI;
}

// Checking wheter derived Eigen object is nan.
template<typename Derived>
inline bool is_nan(const Eigen::MatrixBase<Derived>& x)
{
    return !(((x.array() == x.array())).all());
}