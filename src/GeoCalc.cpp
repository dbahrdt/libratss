#include <libratss/GeoCalc.h>

namespace LIB_RATSS_NAMESPACE {


bool GeoCalc::isOnSphere(const mpfr::mpreal & mpdx, const mpfr::mpreal & mpdy, const mpfr::mpreal & mpdz) const {
	return (add(add(sq(mpdx), sq(mpdy)), sq(mpdz)) == 1);
}

void GeoCalc::geo(const mpfr::mpreal &theta, const mpfr::mpreal &phi, mpfr::mpreal &lat, mpfr::mpreal &lon) const
{
	int inputPrecision = std::max<int>(theta.getPrecision(), phi.getPrecision());
	int outputPrecision = std::max<int>(lat.getPrecision(), lon.getPrecision());
	int calcPrecision = std::max<int>(inputPrecision, outputPrecision);
	
	auto pi = mpfr::const_pi(calcPrecision);

	//lat = (pi/2 - theta)/pi*2*90;
	//lon = (phi <= pi ? phi/pi*180 : (phi-2*pi)/pi*180);

	lat = sub(90, mult(div(theta, pi), 180));
	lon = mult( div(phi, pi), 180);
	if (phi > pi) {
		lon = sub(lon, 90);
	}
}

void GeoCalc::spherical(const mpfr::mpreal &lat, const mpfr::mpreal &lon, mpfr::mpreal &theta, mpfr::mpreal &phi) const
{
	int outputPrecision = std::max<int>(theta.getPrecision(), phi.getPrecision());
	int inputPrecision = std::max<int>(lat.getPrecision(), lon.getPrecision());
	int calcPrecision = std::max<int>(inputPrecision, outputPrecision);
	
	auto pi = mpfr::const_pi(calcPrecision);
	
// 	auto theta = pi/180.0*(90.0-lat); 
// 	auto phi = (lon >= 0 ? lon/180*pi : (lon + 360)/180*pi);
	
	theta = mult(div(pi, 180.0), sub(90.0, lat));
	phi = (lon >= 0 ? div(mult(lon, pi), 180) : div(mult(add(lon, 360), pi), 180) );
}

void GeoCalc::cartesian(const mpfr::mpreal & lat, const mpfr::mpreal & lon, mpfr::mpreal & x, mpfr::mpreal & y, mpfr::mpreal & z) const {
// 	mpfr::mpreal theta, phi;
// 	spherical(lat, lon, theta, phi);
// 	cartesianFromSpherical(theta, phi, x, y, z);

	int outputPrecision = std::max<int>(std::max<int>(x.getPrecision(), y.getPrecision()), z.getPrecision());
	int inputPrecision = std::max<int>(lat.getPrecision(), lon.getPrecision());
	int calcPrecision = std::max<int>(inputPrecision, outputPrecision);
	
	auto pi = mpfr::const_pi(calcPrecision);
	
	mpfr::mpreal lat_rad = lat/180 * pi;
	mpfr::mpreal lon_rad = lon/180 * pi;
	
	x = mult(cos(lon_rad), cos(lat_rad));
	y = mult(sin(lon_rad), cos(lat_rad));
	z = sin(lat_rad);
}

void GeoCalc::cartesianFromSpherical(const mpfr::mpreal & theta, const mpfr::mpreal & phi, mpfr::mpreal & x, mpfr::mpreal & y, mpfr::mpreal & z) const {
	mpfr::mpreal sinTheta = sin(theta);
	x = mult(sinTheta, cos (phi));
	y = mult(sinTheta, sin (phi));
	z = cos(theta);
}

void GeoCalc::spherical(const mpfr::mpreal& x, const mpfr::mpreal& y, const mpfr::mpreal& z, mpfr::mpreal& theta, mpfr::mpreal& phi) const {
	theta = acos(z);
	phi = atan(div(y, x));
	if (x < 0) {
		if (y < 0) {
			phi -= mpfr::const_pi(phi.get_prec());
		}
		else {
			phi += mpfr::const_pi(phi.get_prec());
		}
	}
}

void GeoCalc::geo(const mpfr::mpreal& x, const mpfr::mpreal& y, const mpfr::mpreal& z, mpfr::mpreal& lat, mpfr::mpreal& lon) const {
	mpfr::mpreal theta, phi;
	spherical(x, y, z, theta, phi);
	geo(theta, phi, lat, lon);
}


}//end namespace LIB_RATSS_NAMESPACE