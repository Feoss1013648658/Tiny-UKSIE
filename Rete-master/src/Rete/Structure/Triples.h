#pragma once

#include <vector>
#include <string>

#include "Field.h"
#include "../../StringOp.h"

class Triples
{
protected:
	std::vector<std::string> fields;
	size_t getIndex(Field::Name fieldName) const;
public:
	template<typename ID, typename Attr, typename Value,typename Probability>
	Triples(const ID& id, const Attr& attr, const Value& value,const Probability& px);
	void set(Field::Name fieldName, const std::string& v);
	const std::string& get(Field::Name fieldName) const;
	void print(int level) const;
};

template<typename ID, typename Attr, typename Value,typename Probability>
inline Triples::Triples(const ID & id, const Attr & attr, const Value & value,const Probability & px)
	: fields({ CStringOp::ToString(id), CStringOp::ToString(attr), CStringOp::ToString(value),CStringOp::ToString(px) }) {
}