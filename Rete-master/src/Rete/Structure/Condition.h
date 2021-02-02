#pragma once

#include <iostream>
#include <fstream>
#include "Triples.h"
class Condition : public Triples {
public:
	enum Type {
		positive,
		negetive
	};
	
	static std::string arbitraryString;
	template<typename ID, typename Attr, typename Value,typename Probability>
	Condition(const ID& id, const Attr& attr, const Value& value, const Probability& px, 
		Type type = positive);
	bool isConstTest(Field::Name fieldName) const;
	Condition getIndex() const;
	const Condition::Type& getType() const;
	void print(int level) const;
	bool operator==(const Condition& rhs) const;
protected:
	Type type;
};

using ConditionVector = std::vector<Condition>;

namespace std {
	
	template <>
	struct hash<Condition::Type>{
			size_t operator()( Condition::Type c) const {
			//return static_cast<size_t>(c);
			size_t ret = 2166136261;
			ret = (ret * 16777619) ^ hash<size_t>()(static_cast<size_t>(c));
			return ret;
		}
	};
	
	template <>
	struct hash<Condition> {
		size_t operator()(const Condition& c) const
		{
			size_t ret = 2166136261;
			ret = (ret * 16777619) ^ hash<string>()(c.get(Field::id));
			ret = (ret * 16777619) ^ hash<string>()(c.get(Field::attr));
			ret = (ret * 16777619) ^ hash<string>()(c.get(Field::value));
//			ret = (ret * 16777619) ^ hash<string>()(c.get(Field::px));        //LC
			ret = (ret * 16777619) ^ hash<Condition::Type>()(c.getType());
			return ret;
		}
	};
}

template<typename ID, typename Attr, typename Value, typename Probability>
inline Condition::Condition(const ID & id, const Attr & attr, const Value & value, const Probability & px, Type type)   //LC
	: Triples(id, attr, value, px), type(type) {
}

