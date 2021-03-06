#include "Field.h"

#include "../../PublicDefine.h"

std::string Field::paramPrefix = "?";   //定义前缀
char Field::attrPrefix = ':';   //定义前缀

std::string Field::getParamString(const std::string & prefix, size_t paramCount) {
	return paramPrefix + prefix + CStringOp::ToString(paramCount);
}

std::string Field::getParamName(const std::string & str) {
	myAssert(isParam(str));
	return str.substr(paramPrefix.length());
}

bool Field::isParam(const std::string & str) {
	return CStringOp::startsWith(str, paramPrefix);
}
