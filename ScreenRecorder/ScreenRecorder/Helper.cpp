#include "Helper.h"

bool StringInVector(const string& str, const vector<string>& vec)
{
    for (const auto& elem : vec) {
        if (elem == str) {
            return true;
        }
    }
    return false;
}
