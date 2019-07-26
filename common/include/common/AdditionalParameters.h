//
// Created by max on 21/05/19.
//

#ifndef INC_3D_DEMOS_ADDITIONALPARAMETERS_H
#define INC_3D_DEMOS_ADDITIONALPARAMETERS_H

#include <map>
#include <string>
#include <any>

using namespace std;


class AdditionalParameters {

public:

    AdditionalParameters() = default;

    bool contains(const string& key);

    void cleanupLoopParameters();

    void insertProgramLongParameter(const string& key, const std::any& value);

    void insertLoopLongParameter(const string& key, const std::any& value);

    std::any get(const string& key);

private:

    map<string, std::any> programAgedParameters{};
    map<string, std::any> loopAgedParameters{};

};


#endif //INC_3D_DEMOS_ADDITIONALPARAMETERS_H
