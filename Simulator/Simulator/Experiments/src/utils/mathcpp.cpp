#include <random>
#include <chrono>



double get_gamma_sample_cpp(double alpha, double betta) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::gamma_distribution<double> d(alpha, betta);

	return d(gen);
}

extern "C" double get_gamma_sample(double alpha, double betta) {
	return get_gamma_sample_cpp(alpha, betta);
}


double get_gaussian_sample_cpp(double mean, double std) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::normal_distribution<double> d(mean, std);

	return d(gen);
}

extern "C" double get_gaussian_sample(double mean, double std) {
	return get_gaussian_sample_cpp(mean, std);
}