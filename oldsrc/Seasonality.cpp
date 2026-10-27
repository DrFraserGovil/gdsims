#include <cmath>
#include <vector>
#include "Seasonality.h"
#include "constants.h"

/**
 * SineRainfall constructor.
 * @param[in] params seasonality parameters for sine rainfall-type seasonality
 */
SineRainfall::SineRainfall(SineRainfallParams *params): Seasonality(params->alpha1) 
{
    amp = params->amp;
}

/**
 * Computes the carrying-capacity alpha value for the given day and alpha0. Models rainfall contribution as a sine wave.
 * @details
 * \f[
 *      \alpha = \alpha_0 + \alpha_1 \left(1 + \textrm{A}\sin\left(\frac{2 \pi d}{365}\right)\right),
 * \f]
 * where \f$ \alpha \f$ is the carrying capacity, \f$ \alpha_0 \f$ is the baseline contribution, \f$ \alpha_1 \f$ is the factor accounting for rainfall contribution, \f$ \textrm{A} \f$ is the amplitude of rainfall fluctuations and \f$ d \f$ is the given simulation day.
 * @param[in] day       simulation day
 * @param[in] alpha0    baseline contribution to the carrying capacity
 * @return The carrying-capacity.
 */
double SineRainfall::alpha(int day, double alpha0)
{
    double alpha = alpha0 + alpha1*(1 + amp*std::sin(2 * constants::pi * day/365));
	return alpha;
}

/**
 * InputRainfall constructor.
 * @param[in] params seasonality parameters for input rainfall-type seasonality
 */
InputRainfall::InputRainfall(InputRainfallParams *params): Seasonality(params->alpha1) 
{
    resp = params->resp;

    if (!((params->rainfall).empty())) {
        rainfall = params->rainfall;
    }
    else {
        for (int i = 0; i < 365; ++i) {
            rainfall.push_back(0);
        }
    }
}

/**
 * Computes the carrying-capacity alpha value for the given day and alpha0. Models rainfall contribution as an exponential of daily rainfall.
 * @details
 * \f[
 *      \alpha = \alpha_0 + \alpha_1 (1 - e^{-\textrm{R} r_d}),
 * \f]
 * where \f$ \alpha \f$ is the carrying capacity, \f$ \alpha_0 \f$ is the baseline contribution, \f$ \alpha_1 \f$ is the factor accounting for rainfall contribution, \f$ \textrm{R} \f$ is the responsiveness to the rainfall contribution and \f$ r_d \f$ is the rainfall for the given day.
 * @param[in] day       simulation day
 * @param[in] alpha0    baseline contribution to the carrying capacity
 * @return The carrying-capacity.
 */
double InputRainfall::alpha(int day, double alpha0)
{
    double r_d;
    if (day == 0) {r_d = rainfall.at(0);} // use day1 rainfall value for day0 as well, to set up the environmental conditions
    else {
        if (rainfall.size() == 365) {
            r_d = day <= 365 ? rainfall.at(day - 1) : rainfall.at(((day - 1)%365)); // simulation days start from 1
        }
        else {
            r_d = rainfall.at(day - 1); // simulation days start from 1
        }
    }
    double alpha = alpha0 + alpha1*(1 - std::exp(-resp * r_d));
    return alpha;
}