#include <libratss/CGAL/ExtendedInt64Pq.h>
#include <stdexcept>

namespace CGAL {

ExtendedInt64Pq::ExtendedInt64Pq() :
m_isExtended(false)
{
	set(base_type(0), base_type(1));
}

ExtendedInt64Pq::ExtendedInt64Pq(const ExtendedInt64Pq & other) :
m_isExtended(false)
{
	if (other.isExtended()) {
		set(other.getExtended());
	}
	else {
		set(other.get());
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(ExtendedInt64Pq && other) :
m_isExtended(false)
{
	if (other.isExtended()) {
		m_isExtended = true;
		m_v.ptr = other.m_v.ptr;
		
		other.m_isExtended = false;
		other.m_v.ptr = 0;
	}
	else {
		set(other.get());
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(const CGAL::Gmpq & q) :
m_isExtended(false)
{
	set(q);
}

ExtendedInt64Pq::ExtendedInt64Pq(int32_t n) :
ExtendedInt64Pq(base_type(n))
{}

ExtendedInt64Pq::ExtendedInt64Pq(uint32_t n) :
ExtendedInt64Pq(base_type(n))
{}

ExtendedInt64Pq::ExtendedInt64Pq(base_type n) :
m_isExtended(false)
{
	set(n, base_type(1));
}

ExtendedInt64Pq::ExtendedInt64Pq(uint64_t n) :
m_isExtended(false)
{
	if (n < (uint64_t) btmax) {
		set(base_type(n), base_type(1));
	}
	else {
		set(extension_type(n));
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(const Gmpz& n) :
ExtendedInt64Pq(extension_type(n))
{}

ExtendedInt64Pq::ExtendedInt64Pq(const ExtendedInt64z & n) :
m_isExtended(false)
{
	if (n.isExtended()) {
		set( extension_type(n.getExtended()) );
	}
	else {
		set(n.get(), base_type(1));
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(const Gmpfr &f) :
ExtendedInt64Pq(extension_type(f))
{}

ExtendedInt64Pq::ExtendedInt64Pq(int32_t n, int32_t d) :
ExtendedInt64Pq(base_type(n), base_type(d))
{}

ExtendedInt64Pq::ExtendedInt64Pq(base_type n, base_type d) :
m_isExtended(false)
{
	set(n, d);
}

ExtendedInt64Pq::ExtendedInt64Pq(base_type n, uint64_t d) :
m_isExtended(false)
{
	if (d < uint64_t(btmax) ) {
		set(base_type(n), base_type(d));
	}
	else {
		set(extension_type(n, d));
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(uint64_t n, uint64_t d) :
m_isExtended(false)
{
	if (n < uint64_t(btmax) && d < uint64_t(btmax) ) {
		set(base_type(n), base_type(d));
	}
	else {
		set(extension_type(n, d));
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(const ExtendedInt64z& n, const ExtendedInt64z& d) :
m_isExtended(false)
{
	if (n.isExtended() && d.isExtended()) {
		set( extension_type( n.getExtended(), d.getExtended()) );
	}
	else if (n.isExtended()) {
		set( extension_type( n.getExtended(), d.asExtended()) );
	}
	else if (d.isExtended()) {
		set( extension_type( n.asExtended(), d.getExtended()) );
	}
	else {
		set(n.get(), d.get());
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(const Gmpz& n, const Gmpz& d) :
ExtendedInt64Pq(extension_type(n, d))
{}

ExtendedInt64Pq::ExtendedInt64Pq(double d) :
m_isExtended(false)
{
	if (double(base_type(d)) == d) {
		set(base_type(d), base_type(1));
	}
	else {
		set(extension_type(d));
	}
}

ExtendedInt64Pq::ExtendedInt64Pq(const std::string& str, int base) :
ExtendedInt64Pq(extension_type(str, base))
{}

ExtendedInt64Pq::~ExtendedInt64Pq() {
	if (isExtended()) {
		delete m_v.ptr;
		m_v.ptr = 0;
		m_isExtended = false;
	}
}


ExtendedInt64Pq& ExtendedInt64Pq::operator=(const ExtendedInt64Pq & other) {
	if (other.isExtended()) {
		set(other.getExtended());
	}
	else {
		set(other.get());
	}
	return *this;
}

ExtendedInt64Pq& ExtendedInt64Pq::operator=(ExtendedInt64Pq && other) {
	if (other.isExtended()) {
		if (isExtended()) {
			using std::swap;
			swap(m_v.ptr, other.m_v.ptr);
		}
		else {
			m_isExtended = true;
			m_v.ptr = other.m_v.ptr;
			
			other.m_isExtended = false;
			m_v.ptr = 0;
		}
	}
	else {
		set(other.get());
	}
	return *this;
}

std::size_t ExtendedInt64Pq::size() const {
	return sizeof(m_v) + sizeof(m_isExtended) + (isExtended() ? getExtended().size() : 0);
}

void ExtendedInt64Pq::canonicalize() {
	if (isExtended()) {
		::mpq_canonicalize(getExtended().mpq());
	}
	else {
		auto tmp = asExtended();
		::mpq_canonicalize(tmp.mpq());
		set(tmp);
	}
}

ExtendedInt64z ExtendedInt64Pq::numerator() const {
	if (isExtended()) {
		return ExtendedInt64z( getExtended().numerator() );
	}
	else {
		return ExtendedInt64z( get().num );
	}
}

ExtendedInt64z ExtendedInt64Pq::denominator() const {
	if (isExtended()) {
		return ExtendedInt64z( getExtended().denominator() );
	}
	else {
		return ExtendedInt64z( get().den );
	}
}

ExtendedInt64Pq ExtendedInt64Pq::operator+() const {
	return *this;
}

ExtendedInt64Pq ExtendedInt64Pq::operator-() const {
	if (isExtended()) {
		return ExtendedInt64Pq( -getExtended() );
	}
	else {
		return ExtendedInt64Pq( -get().num, get().den );
	}
}

ExtendedInt64Pq& ExtendedInt64Pq::operator+=(const ExtendedInt64Pq &q) {
	if (isExtended() && q.isExtended()) {
		getExtended() += q.getExtended();
	}
	else if (isExtended()) {
		getExtended() += q.asExtended();
	}
	else if (q.isExtended()) {
		set( asExtended() + q.getExtended() );
	}
	else {
		set( asExtended() + q.asExtended() );
	}
	return *this;
}

ExtendedInt64Pq& ExtendedInt64Pq::operator-=(const ExtendedInt64Pq &q) {
	if (isExtended() && q.isExtended()) {
		getExtended() -= q.getExtended();
	}
	else if (isExtended()) {
		getExtended() -= q.asExtended();
	}
	else if (q.isExtended()) {
		set( asExtended() - q.getExtended() );
	}
	else {
		set( asExtended() - q.asExtended() );
	}
	return *this;
}

ExtendedInt64Pq& ExtendedInt64Pq::operator*=(const ExtendedInt64Pq &q) {
	if (isExtended() && q.isExtended()) {
		getExtended() *= q.getExtended();
	}
	else if (isExtended()) {
		getExtended() *= q.asExtended();
	}
	else if (q.isExtended()) {
		set( asExtended() * q.getExtended() );
	}
	else {
		set( asExtended() * q.asExtended() );
	}
	return *this;
}

ExtendedInt64Pq& ExtendedInt64Pq::operator/=(const ExtendedInt64Pq &q) {
	if (isExtended() && q.isExtended()) {
		getExtended() /= q.getExtended();
	}
	else if (isExtended()) {
		getExtended() /= q.asExtended();
	}
	else if (q.isExtended()) {
		set( asExtended() / q.getExtended() );
	}
	else {
		set( asExtended() / q.asExtended() );
	}
	return *this;
}

ExtendedInt64Pq ExtendedInt64Pq::operator+(const ExtendedInt64Pq & q) const {
	ExtendedInt64Pq tmp(*this);
	tmp += q;
	return tmp;
}

ExtendedInt64Pq ExtendedInt64Pq::operator-(const ExtendedInt64Pq & q) const {
	ExtendedInt64Pq tmp(*this);
	tmp -= q;
	return tmp;
}

ExtendedInt64Pq ExtendedInt64Pq::operator*(const ExtendedInt64Pq & q) const {
	ExtendedInt64Pq tmp(*this);
	tmp *= q;
	return tmp;
}

ExtendedInt64Pq ExtendedInt64Pq::operator/(const ExtendedInt64Pq & q) const {
	ExtendedInt64Pq tmp(*this);
	tmp /= q;
	return tmp;
}

bool ExtendedInt64Pq::operator==(const ExtendedInt64Pq &q) const {
	return asExtended() == q.asExtended();
}

bool ExtendedInt64Pq::operator< (const ExtendedInt64Pq &q) const {
	return asExtended() < q.asExtended();
}

double ExtendedInt64Pq::to_double() const {
	return asExtended().to_double();
}

Sign ExtendedInt64Pq::sign() const {
	if (isExtended()) {
		return getExtended().sign();
	}
	else {
		return CGAL::sign(get().num);
	}
}

const ExtendedInt64Pq::extension_type & ExtendedInt64Pq::getExtended() const {
	assert(isExtended());
	return *m_v.ptr;
}

ExtendedInt64Pq::extension_type & ExtendedInt64Pq::getExtended() {
	assert(isExtended());
	return *m_v.ptr;
}

ExtendedInt64Pq::PQ& ExtendedInt64Pq::get() {
	assert(!isExtended());
	return m_v.pq;
}

const ExtendedInt64Pq::PQ& ExtendedInt64Pq::get() const {
	assert(!isExtended());
	return m_v.pq;
}

ExtendedInt64Pq::extension_type ExtendedInt64Pq::asExtended() const {
	if (isExtended()) {
		return getExtended();
	}
	else {
		return extension_type(get().num, get().den);
	}
}

bool ExtendedInt64Pq::isExtended() const {
	return m_isExtended;
}

void ExtendedInt64Pq::set(const ExtendedInt64Pq::PQ& pq) {
	set(pq.num, pq.den);
}

void ExtendedInt64Pq::set(base_type num, base_type den) {
	if (isExtended()) {
		delete m_v.ptr;
		m_isExtended = false;
	}
	if (den < 0) {
		den = -den;
		num = -num;
	}
	if (!den) {
		throw std::domain_error("Denominator is not allowed to be zero");
	}
	m_v.pq.num = num;
	m_v.pq.den = den;
}

void ExtendedInt64Pq::set(const extension_type & v) {
	
	auto num = v.numerator();
	auto den = v.denominator();
	
	if (::mpz_fits_slong_p(num.mpz()) && ::mpz_fits_slong_p(den.mpz())) {
		set(::mpz_get_si(num.mpz()), ::mpz_get_si(den.mpz()));
	}
	else {
		if (isExtended()) {
			getExtended() = v;
		}
		else {
			m_v.ptr = new extension_type(v);
			m_isExtended = true;
		}
	}
}

}//end namespace CGAL