#include <libratss/ProjectS2.h>

namespace LIB_RATSS_NAMESPACE {

void ProjectS2::snap(const mpfr::mpreal &flxs, const mpfr::mpreal &flys, const mpfr::mpreal &flzs, mpq_class &xs, mpq_class &ys, mpq_class &zs, int precision) const {
	using ConstRefWrap = ReferenceWrapper<const mpfr::mpreal>;
	using RefWrap = ReferenceWrapper<mpq_class>;
	using ConstRefWrapIt = ReferenceWrapperIterator<ConstRefWrap * >;
	using RefWrapIt = ReferenceWrapperIterator<RefWrap *>;
	
	ConstRefWrap input[3] = { flxs, flys, flzs};
	RefWrap output[3] = {xs, ys, zs};
	ConstRefWrapIt inputBegin(input);
	ConstRefWrapIt inputEnd(input+3);
	RefWrapIt outputBegin(output);
	
	ProjectSN::snap<ConstRefWrapIt, RefWrapIt>(inputBegin, inputEnd, outputBegin, ST_FT | ST_PLANE | ST_NORMALIZE);

	assert(xs*xs + ys*ys + zs*zs == 1);
}

}//end namespace LIB_RATSS_NAMESPACE