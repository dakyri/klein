#pragma once

#include <unordered_map>
#include <string>

#include "KLFunValue.h"

using namespace std;

class KLFObject {
public:
	KLFObject() {}
protected:
	unordered_map<string, KLFValue> map;
};