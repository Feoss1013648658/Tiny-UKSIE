#pragma once

#include "Triples.h"
#include "../../PublicDefine.h"

class WME : public Triples {
public:
	template<typename ID, typename Attr, typename Value,typename Probability>
	WME(const ID& id, const Attr& attr, const Value& value,const Probability& px);   //LC
};

class WMEVector : public std::vector<WME> {
};

template<typename ID, typename Attr, typename Value,typename Probability>
inline WME::WME(const ID & id, const Attr & attr, const Value & value,const Probability& px)
	: Triples({ id, attr, value,px }) {
	myAssert(!Field::isParam(fields.at(0)));
	myAssert(!Field::isParam(fields.at(1)));
	myAssert(!Field::isParam(fields.at(2)));
	myAssert(!Field::isParam(fields.at(3)));
}
