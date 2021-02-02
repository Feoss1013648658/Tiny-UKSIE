#include "Condition.h"

#include "../../StringOp.h"


std::string Condition::arbitraryString = "*"; 


bool Condition::isConstTest(Field::Name fieldName) const {
	return !Field::isParam(this->get(fieldName));
}

Condition Condition::getIndex() const {
	Condition ret = *this;
	if (!ret.isConstTest(Field::id))
		ret.fields.at(0) = Condition::arbitraryString;
	if (!ret.isConstTest(Field::attr))
		ret.fields.at(1) = Condition::arbitraryString;
	if (!ret.isConstTest(Field::value))
		ret.fields.at(2) = Condition::arbitraryString;
	if (!ret.isConstTest(Field::px))
		ret.fields.at(3) = Condition::arbitraryString;
	return ret;
}

const Condition::Type & Condition::getType() const {
	return type;
}

void Condition::print(int level) const {

	std::string prefix = CStringOp::repeat("\t", level);
	std::cout << prefix << fields.at(0) << ","
		<< (type == negetive ? "~" : "")
		<< fields.at(1)
		<< "," << fields.at(2) <<","<< fields.at(3)<< std::endl;   //LC 
}

bool Condition::operator==(const Condition & rhs) const {
	return (fields.at(0) == rhs.fields.at(0)
		&& fields.at(1) == rhs.fields.at(1)
		&& fields.at(2) == rhs.fields.at(2)
//		&& fields.at(3) == rhs.fields.at(3)            //LC
		&& type == rhs.type);
}

/*size_t std::hash<Condition>::operator()(const Condition& c) const {
	size_t ret = 2166136261;
	ret = (ret * 16777619) ^ hash<string>()(c.get(Field::id));
	ret = (ret * 16777619) ^ hash<string>()(c.get(Field::attr));
	ret = (ret * 16777619) ^ hash<string>()(c.get(Field::value));
	//      ret = (ret * 16777619) ^ hash<string>()(c.get(Field::px));        //LC
	ret = (ret * 16777619) ^ hash<Condition::Type>()(c.getType());
	return ret;
}*/

