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