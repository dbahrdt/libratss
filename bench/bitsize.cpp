#include <libratss/ProjectS2.h>
#include "../common/generators.h"
#include "../common/stats.h"

void help() {
	std::cout << "prg [-i <file with one point per line> [-r <number of random points>] [-p <precisio>] [-e <manual eps>]" << std::endl;
}

using LIB_RATSS_NAMESPACE::BitCount;

int main(int argc, char ** argv) {
	std::size_t num_rand_points = 0;
	std::string inFile;
	int initialPrecision = 32;
	int manualEps = -1;
	mpq_class manEpsVal(1);
	
	for(int i(0); i < argc; ++i) {
		std::string token(argv[i]);
		if (token == "-r" && i+1 < argc) {
			num_rand_points = ::atoi(argv[i+1]);
			++i;
		}
		else if (token == "-i" && i+1 < argc) {
			inFile = std::string(argv[i+1]);
			++i;
		}
		else if (token == "-p" && i+1 < argc) {
			initialPrecision = ::atoi(argv[i+1]);
			++i;
		}
		else if (token == "-e" && i+1 < argc) {
			manualEps = ::atoi(argv[i+1]);
			if (manualEps <= 0) {
				std::cerr << "Epsilon needs to be larger than 0" << std::endl;
				return -1;
			}
			manEpsVal >>= manualEps;
			++i;
		}
		else if (token == "-h" || token == "--help") {
			help();
			return 0;
		}
	}
	if (num_rand_points == 0 && inFile.empty()) {
		help();
		return -1;
	}
	
	std::vector<LIB_RATSS_NAMESPACE::SphericalCoord> points;
	
	if (inFile.size()) {
		struct MyIt {
			std::vector<LIB_RATSS_NAMESPACE::SphericalCoord> & points;
			MyIt(std::vector<LIB_RATSS_NAMESPACE::SphericalCoord> & points) : points(points) {}
			MyIt(const MyIt & other) : points(other.points) {}
			MyIt & operator*() { return *this; }
			MyIt & operator++() { return *this; }
			MyIt & operator=(const LIB_RATSS_NAMESPACE::GeoCoord & c) {
				points.emplace_back(c);
			}
		};
		LIB_RATSS_NAMESPACE::readPoints(inFile, MyIt(points));
		std::cout << "Read " << points.size() << " points from " << inFile << std::endl;
	}
	
	if (num_rand_points) {
		ratss::getRandomPolarPoints(num_rand_points, std::back_inserter(points));
	}
	
	BitCount snapFrac, snapCast, snapManEps;
	BitCount snapFracProj, snapCastProj, snapManEpsProj;
	
	std::cout << "Points: " << points.size() << std::endl;
	std::cout << "Precision: " << initialPrecision << std::endl;
	if (manualEps > 0) {
		std::cout << "Manual eps: " << manualEps << std::endl;
	}
	
	ratss::ProjectS2 p;
	ratss::GeoCalc calc;
	mpfr::mpreal xd, yd, zd, xpd, ypd, zpd;
	mpq_class xs, ys, zs, xps, yps, zps;
	for(std::size_t i(0), s(points.size()); i < s; ++i) {
		const LIB_RATSS_NAMESPACE::SphericalCoord & c = points[i];
		mpfr::mpreal theta(c.theta, initialPrecision);
		mpfr::mpreal phi(c.phi, initialPrecision);
		calc.cartesianFromSpherical(theta, phi, xd, yd, zd);
		
		//we want the bitsize for the snapping in the plane
		auto pos = p.sphere2Plane(xd, yd, zd, xpd, ypd, zpd);
		
		assert(xpd*xpd + ypd*ypd + zpd*zpd <= 1);
		
		//snap with cast
		xps = LIB_RATSS_NAMESPACE::Conversion<mpfr::mpreal>::toMpq(xpd);
		yps = LIB_RATSS_NAMESPACE::Conversion<mpfr::mpreal>::toMpq(ypd);
		zps = LIB_RATSS_NAMESPACE::Conversion<mpfr::mpreal>::toMpq(zpd);
		p.plane2Sphere(xps, yps, zps, pos, xs, ys, zs);
		snapCast.update(xps);
		snapCast.update(yps);
		snapCast.update(zps);
		snapCastProj.update(xs);
		snapCastProj.update(ys);
		snapCastProj.update(zs);
		
		//snap with continous fraction
		xps = calc.snap(xpd, LIB_RATSS_NAMESPACE::Calc::ST_CF);
		yps = calc.snap(ypd, LIB_RATSS_NAMESPACE::Calc::ST_CF);
		zps = calc.snap(zpd, LIB_RATSS_NAMESPACE::Calc::ST_CF);
		p.plane2Sphere(xps, yps, zps, pos, xs, ys, zs);
		snapFrac.update(xps);
		snapFrac.update(yps);
		snapFrac.update(zps);
		snapFracProj.update(xs);
		snapFracProj.update(ys);
		snapFracProj.update(zs);
		
		//snap with continous fraction with manual eps
		if (manualEps > 0) {
			xps = LIB_RATSS_NAMESPACE::Conversion<mpfr::mpreal>::toMpq(xpd);
			yps = LIB_RATSS_NAMESPACE::Conversion<mpfr::mpreal>::toMpq(ypd);
			zps = LIB_RATSS_NAMESPACE::Conversion<mpfr::mpreal>::toMpq(zpd);
			xps = calc.within(xps-manEpsVal, xps+manEpsVal);
			yps = calc.within(yps-manEpsVal, yps+manEpsVal);
			zps = calc.within(zps-manEpsVal, zps+manEpsVal);
			p.plane2Sphere(xps, yps, zps, pos, xs, ys, zs);
			snapManEps.update(xps);
			snapManEps.update(yps);
			snapManEps.update(zps);
			snapManEpsProj.update(xs);
			snapManEpsProj.update(ys);
			snapManEpsProj.update(zs);
		}
		if (i % 1000 == 0) {
			std::cout << '\xd' << i/1000 << "k" << std::flush;
		}
	}
	std::cout << std::endl;
	
	std::cout << "Bit sizes with snapping by cast:\n" << snapCast << '\n';
	std::cout << "Bit sizes with snapping by continous fraction:\n" << snapFrac << '\n';
	if (manualEps > 0) {
		std::cout << "Bit sizes with snapping by continous fraction with given eps:\n" << snapManEps << '\n';
	}
	std::cout << "Bit sizes with projection and snapping by cast:\n" << snapCastProj << '\n';
	std::cout << "Bit sizes with projection and snapping by continous fraction::\n" << snapFracProj << '\n';
	if (manualEps > 0) {
		std::cout << "Bit sizes with projection and snapping by continous fraction with given eps:\n" << snapManEpsProj << '\n';
	}
	std::cout << std::flush;
	return 0;
}