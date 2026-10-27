#include <vector>
#include <array>
#include <cmath>
#include <limits>
#include <algorithm>
#include <numeric>
#include "Dispersal.h"
#include "random.h"
#include "constants.h"
#include <iostream>

/**
 * Dispersal constructor. 
 * @details Constructs the BoundaryStrategy object from the boundary type passed. 
 * @param[in] params 	dispersal parameters
 * @param[in] boundary 	boundary type to use for calculating dispersal distances
 * @param[in] side_x 	size of one side of the simulation area (x-axis)
 * @param[in] side_y	size of one side of the simulation area (y-axis)
 */
Dispersal::Dispersal(DispersalParams* params, BoundaryType boundary, double side_x, double side_y) 
{
	disp_rate = params->disp_rate;
	max_disp = params->max_disp;
	
	if (boundary == Toroid) {
		boundary_strategy = new ToroidalBoundaryStrategy(side_x, side_y);
	}
	else if (boundary == Edge) {
		boundary_strategy = new EdgeBoundaryStrategy(side_x, side_y);
	}
	else {
		boundary_strategy = new ToroidalBoundaryStrategy(side_x, side_y);
	}
}

/**
 * Determines the number of adult males (of each genotype) dispersing out from each patch.
 * @details The number of adult males of a given genotype dispersing from a given patch is determined by a random draw from a binomial distribution with probability of adult dispersal rate.
 * @param[in] sites vector of all Patch objects
 * @return The number of adult males dispersing out, divided by genotype and outgoing patch.
 * @see DispersalParams::disp_rate, Model::get_sites(), random_binomial()
 */
std::vector<std::array<long long int, constants::num_gen>> Dispersal::M_dispersing_out(const std::vector<Patch*> &sites) 
{
	std::vector<std::array<long long int, constants::num_gen>> m_move;	
	std::array<long long int, constants::num_gen> m;
	std::array<long long int, constants::num_gen> m_out;
	for (int pat=0; pat < sites.size(); ++pat) {
		m = sites[pat]->get_M();
		for (int i=0; i < constants::num_gen; ++i) {
			m_out[i] = random_binomial(m[i], disp_rate); // how many adult males will disperse from the given patch
		}
		m_move.push_back(m_out);
	}
	return m_move;
}

/**
 * Determines the number of adult mated females (of each genotype combination) dispersing out from each patch.
 * @details The number of adult females of a given genotype combination dispersing from a given patch is determined by a random draw from a binomial distribution with probability of adult dispersal rate.
 * @param[in] sites vector of all Patch objects
 * @return The number of adult mated females dispersing out, divided by female genotype, male sperm genotype and outgoing patch.
 * @see DispersalParams::disp_rate, Model::get_sites(), random_binomial()
 */
std::vector<std::array<std::array<long long int, constants::num_gen>, constants::num_gen>> Dispersal::F_dispersing_out(const std::vector<Patch*> &sites)
{
	std::vector<std::array<std::array<long long int, constants::num_gen>, constants::num_gen>> f_move; 
	std::array<std::array<long long int, constants::num_gen>, constants::num_gen> f;
	std::array<std::array<long long int, constants::num_gen>, constants::num_gen> f_out;
	for (int pat=0; pat < sites.size(); ++pat) {
		f = sites[pat]->get_F();
		for (int i=0; i < constants::num_gen; ++i) {
			for (int j=0; j < constants::num_gen; ++j) {
				f_out[i][j] = random_binomial(f[i][j], disp_rate); // how many adult females will disperse from the given patch
			}
		}
		f_move.push_back(f_out);
	}
	return f_move;
}

DistanceKernelDispersal::~DistanceKernelDispersal() {
	delete boundary_strategy;
}

/**
 * Sets the inter-patch connectivities for dispersal.
 * @details If the distance between two patches is less than the the maximum dispersal distance ``max_disp``, they are deemed to be connected. The connection weight is determined by the difference between the maximum dispersal distance and the distance between those patches:
 * 
 * weight = max_disp - distance
 * 
 * @note Under this dispersal type, patches are deemed to be connected to themselves, resulting in self-dispersal. This is such that dispersal can take place even in 1-population models. 
 * @param[in] sites vector of all Patch objects
 */
void DistanceKernelDispersal::set_connecs(std::vector<Patch*> &sites) {
	connec_indices.clear();
	connec_weights.clear();
	auto connecs = compute_connecs(sites);
	connec_indices = connecs.first;
	connec_weights = connecs.second;
}

/**
 * Implements dispersal by adults from and to each patch, depending on the patch connectivities.
 * @note Only adult males and mated females are assumed to disperse from the patches.
 * @details All dispersing individuals are assumed to survive dispersal, and are guaranteed a connected patch to disperse to. The number of males dispersing from a given patch to each of its connected patches is determined by a random draw from a multinomial distribution with probabilities equal to the connection weights. Similarly for the mated females.
 * @param[in, out] sites vector of all Patch objects
 * @see Model::get_sites(), random_multinomial()
 */
void DistanceKernelDispersal::adults_disperse(std::vector<Patch*> &sites) {
	// adults dispersing out from each patch 
	std::vector<std::array<long long int, constants::num_gen>> m_move = M_dispersing_out(sites); // males dispersing from each patch
	for (int pat = 0; pat < sites.size(); ++pat) { // update population numbers
		sites[pat]->M_disperse_out(m_move[pat]);
	}

	std::vector<std::array<std::array<long long int, constants::num_gen>, constants::num_gen>> f_move = F_dispersing_out(sites);
	for (int pat = 0; pat < sites.size(); ++pat) { 
		sites[pat]->F_disperse_out(f_move[pat]);
	}
		
	// adults dispersing into each patch
	std::vector<long long int> m_disp_by_new_pat;
	for (int pat=0; pat < sites.size(); ++pat) {
		for (int i=0; i < constants::num_gen; ++i) {
			// how many males of the given patch and given genotype will disperse to each of its connected patches
			m_disp_by_new_pat = random_multinomial(m_move[pat][i], connec_weights[pat]);
			for (int new_pat=0; new_pat < m_disp_by_new_pat.size(); ++new_pat) { // update population numbers
				sites[connec_indices[pat][new_pat]]->M_disperse_in(i, m_disp_by_new_pat[new_pat]);
			}
		}
	}

	std::vector<long long int> f_disp_by_new_pat;
	for (int pat = 0; pat < sites.size(); ++pat) {
		for (int i = 0; i < constants::num_gen; ++i) {
			for (int j=0; j < constants::num_gen; ++j) {
				f_disp_by_new_pat = random_multinomial(f_move[pat][i][j], connec_weights[pat]);
				for (int new_pat=0; new_pat < f_disp_by_new_pat.size(); ++new_pat) {
					sites[connec_indices[pat][new_pat]]->F_disperse_in(i, j, f_disp_by_new_pat[new_pat]);
				}
			}
		}
	}
}
 
/**
 * Computes the set of connection indices and weights for a group of patches.
 * @details If the distance between two patches is less than the the maximum dispersal distance ``max_disp``, they are deemed to be connected. The connection weight is determined by the difference between the maximum dispersal distance and the distance between those patches:
 * 
 * weight = max_disp - distance
 * 
 * @note Under this dispersal type, patches are deemed to be connected to themselves, resulting in self-dispersal. This is such that dispersal can take place even in 1-population models. 
 * @param[in] sites vector of all Patches objects
 * @return The connections between all patches, divided into connection indices and connection weights. These are then organised in the same order as Model::get_sites(), where the first item represents all connections to the first Patch of the sites vector, etc. 
 */
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> DistanceKernelDispersal::compute_connecs(std::vector<Patch*>
 &sites) 
{
    std::vector<std::vector<int>> connec_indices;
    std::vector<std::vector<double>> connec_weights;
    std::vector<int> connec_indices_pat;
	std::vector<double> connec_weights_pat;
	for (int pat=0; pat < sites.size(); ++pat) {
		connec_indices_pat.clear();
		connec_weights_pat.clear();
		for (int new_pat=0; new_pat < sites.size(); ++new_pat) {
			double dd = boundary_strategy->distance(sites[pat]->get_coords(), sites[new_pat]->get_coords());
			if (dd < max_disp) {
				connec_indices_pat.push_back(new_pat); 
				double weight = max_disp - dd;
				connec_weights_pat.push_back(weight); 
			}
		}
		connec_indices.push_back(connec_indices_pat);
		connec_weights.push_back(connec_weights_pat);
	}

    return {connec_indices, connec_weights};
}

/**
 * RadialDispersal constructor.
 * @param[in] params 	dispersal parameters
 * @param[in] boundary 	boundary type to use for calculating dispersal distances
 * @param[in] side_x	size of one side of the simulation area (x-axis)
 * @param[in] side_y    size of one side of the simulation area (y-axis)
 */
RadialDispersal::RadialDispersal(DispersalParams* params, BoundaryType boundary, double side_x, double side_y): Dispersal(params, boundary, side_x, side_y) {
	connec_weights_sum.clear();
}

RadialDispersal::~RadialDispersal() {
	delete boundary_strategy;
}

/**
 * Sets the inter-patch connectivities for radial dispersal.
 *  @details If the distance between two patches is less than the the maximum dispersal distance, they may be connected. The connection weight of a focal patch to its neighbouring patch is determined by the angle of bisecting lines from the centre of the focal patch to the catchment of the receiving patch. More distant villages may also be directly connected but the connectivity will be reduced if there are closer villages along the same flight path. Patches that are further apart than the maximum dispersal distance are not connected.
 * @param[in] sites vector of all Patch objects
 */
void RadialDispersal::set_connecs(std::vector<Patch*> &sites) {
	connec_indices.clear();
	connec_weights.clear();
	auto connecs = compute_connecs(sites);
	connec_indices = connecs.first;
	connec_weights = connecs.second;

	// calculate the sum of connec weights for each patch to later use for dispersal mortality
	connec_weights_sum.clear();
	std::vector<double> ws(connec_weights.size());
	for (int pat=0; pat < ws.size(); ++pat) {
		double sum = std::accumulate(connec_weights[pat].begin(), connec_weights[pat].end(), 0.0);
		ws[pat] = sum;
	}
	connec_weights_sum = ws;
}

/**
 * Implements dispersal by adults from and to each patch, depending on the patch connectivities.
 * @note Only adult males and mated females are assumed to disperse from the patches. There is also dispersal mortality associated with this dispersal type.
 * @details Only those individuals that disperse out of a patch in a connected direction will survive. The number of dispersing males (of a given genotype) that survive dispersal out of their patch is determined by a random draw from a binomial distribution with probability of the total connection weight for all its connected patches. Of those, the number dispersing to each of the connected patches is determined by a random draw from a multinomial distribution with probabilities equal to the connection weights. Similarly for the mated females.
 * @param[in, out] sites vector of all Patch objects
 * @see Model::get_sites(), random_binomial(), random_multinomial()
 */
void RadialDispersal::adults_disperse(std::vector<Patch*> &sites) {
	// adults dispersing out from each patch 
	std::vector<std::array<long long int, constants::num_gen>> m_move = M_dispersing_out(sites); // males dispersing from each patch
	for (int pat = 0; pat < sites.size(); ++pat) { // update population numbers
		sites[pat]->M_disperse_out(m_move[pat]);
	}

	std::vector<std::array<std::array<long long int, constants::num_gen>, constants::num_gen>> f_move = F_dispersing_out(sites);
	for (int pat = 0; pat < sites.size(); ++pat) { 
		sites[pat]->F_disperse_out(f_move[pat]);
	}
		
	// adults dispersing into each patch
	std::vector<long long int> m_disp_by_new_pat;
	for (int pat=0; pat < sites.size(); ++pat) {
		for (int i=0; i < constants::num_gen; ++i) {
			// how many males survive dispersal due to dispersing in the connected intervals of the catchment radius
			// (whilst those dispersing in unconnected directions die)
			long long int surv_m = random_binomial(m_move[pat][i], connec_weights_sum[pat]);

			// how many males of the given patch and given genotype will disperse to each of its connected patches
			//m_disp_by_new_pat = random_multinomial(m_move[pat][i], connec_weights[pat]);
			m_disp_by_new_pat = random_multinomial(surv_m, connec_weights[pat]);
			for (int new_pat=0; new_pat < m_disp_by_new_pat.size(); ++new_pat) { // update population numbers
				sites[connec_indices[pat][new_pat]]->M_disperse_in(i, m_disp_by_new_pat[new_pat]);
			}
		}
	}

	std::vector<long long int> f_disp_by_new_pat;
	for (int pat = 0; pat < sites.size(); ++pat) {
		for (int i = 0; i < constants::num_gen; ++i) {
			for (int j=0; j < constants::num_gen; ++j) {
				long long int surv_f = random_binomial(f_move[pat][i][j], connec_weights_sum[pat]);

				//f_disp_by_new_pat = random_multinomial(f_move[pat][i][j], connec_weights[pat]);
				f_disp_by_new_pat = random_multinomial(surv_f, connec_weights[pat]);
				for (int new_pat=0; new_pat < f_disp_by_new_pat.size(); ++new_pat) {
					sites[connec_indices[pat][new_pat]]->F_disperse_in(i, j, f_disp_by_new_pat[new_pat]);
				}
			}
		}
	}
}

/**
 * Computes the set of connection indices and weights for a group of patches.
 * @details If the distance between two patches is less than the the maximum dispersal distance, they may be connected. The connection weight of a focal patch to its neighbouring patch is determined by the angle of bisecting lines from the centre of the focal patch to the catchment of the receiving patch. More distant villages may also be directly connected but the connectivity will be reduced if there are closer villages along the same flight path. Patches that are further apart than the maximum dispersal distance are not connected.
 * @note Under this dispersal type, patches are NOT connected to themselves. 
 * @param[in] sites vector of all Patches objects
 * @return The connections between all patches, divided into connection indices and connection weights. These are then organised in the same order as Model::get_sites(), where the first item represents all connections to the first patch in Model::get_sites(), etc. 
 */
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> RadialDispersal::compute_connecs(std::vector<Patch*> &sites) {
	int num_sites = sites.size();
	std::vector<std::vector<double>> connec_weights(num_sites);
	std::vector<std::vector<int>> connec_indices(num_sites);
	std::vector<double> radii;
	std::vector<std::pair<double, double>> intervals; // vector to store intervals
	std::pair<double, double> qq; // temporary interval
	double alpha, theta, smallest_dist;
	Point loc1, loc2;
	for(int pat=0;pat<num_sites;pat++) {
		auto result = compute_distances_site(pat,sites);
		auto distances =result.first;
		auto local_indices =result.second;
		smallest_dist = std::numeric_limits<double>::infinity();
		for (double dist : distances) {
			if (dist > 0 && dist < smallest_dist) {smallest_dist = dist;}
		}
		radii.push_back(0.5*smallest_dist);
	}
	// compute inter-point distances
	for (int i=0; i < num_sites; i++) {
		loc1 = sites[i]->get_coords();
		intervals.clear();
		auto result = compute_distances_site(i,sites);
		auto distances = result.first;
		auto local_indices =result.second;
		std::vector<int> order = get_sorted_positions(distances);
		for (int j=0; j < order.size(); j++) 
		{
			int loc_index = order[j]; // index among locally connected sites
			int glob_index = local_indices[loc_index]; // index among all sites
			loc2 = sites[glob_index]->get_coords();
			alpha = std::atan(radii[glob_index] / distances[loc_index]); 
			loc2 = boundary_strategy->relative_pos(loc1, loc2);
			double length = 0;
			if (loc2.y > loc1.y) 
			{	
				if (loc2.x > loc1.x) {
					theta = std::atan((loc2.y - loc1.y) / (loc2.x - loc1.x));
				}
				else if (loc2.x == loc1.x) {
					theta = constants::pi/2;
				}
				else {
					theta = constants::pi/2 + std::atan((loc1.x-loc2.x) / (loc2.y-loc1.y)); 
				}
			};
			if (loc2.y == loc1.y) {
				if (loc2.x >= loc1.x) {
					theta = 0;
				}
				else {
					theta = constants::pi;
				} 
			}
			if (loc2.y < loc1.y) {
				if (loc1.x > loc2.x) {
					theta = constants::pi + std::atan((loc1.y - loc2.y) / (loc1.x - loc2.x));
				}
				else if (loc2.x==loc1.x) {
					theta = 3 * constants::pi / 2;
				}
				else {
					theta = 3 * constants::pi / 2 + std::atan((loc2.x - loc1.x) / (loc1.y - loc2.y)); 
				}
			}
			double t_min = wrap_around((theta - alpha) / (2*(constants::pi)), 1);
			double t_plus = wrap_around((theta + alpha) / (2*(constants::pi)), 1);
			if (t_min > t_plus) {
				qq = {t_min, 1};
				auto result = compute_interval_union(qq, intervals);
				intervals = result.first;
				length += result.second;
				qq = {0, t_plus};
				result = compute_interval_union(qq, intervals);
				intervals = result.first;
				length += result.second;
			}
			else { 
				qq = {t_min, t_plus};
				auto result = compute_interval_union(qq, intervals);
				intervals = result.first;
				length = result.second;
			}
			if (length > 0) {
				connec_weights[i].push_back(length);
				connec_indices[i].push_back(glob_index);
			}
		}
	}
	return {connec_indices, connec_weights};
}



/**
 * Determines the points within a distance 'max_dis' of focal patch object from the set of all patch objects
 *  @param[in] i the index of the focal point
 *  @param[in] sites vector of all Patch objects
 * @return A list of the distances of these points, and a list of the indices within the set of all points
 */
std::pair<std::vector<double>,std::vector<int>> RadialDispersal::compute_distances_site(int i, std::vector<Patch*> &sites)
{
        std::vector<double> distances;
        std::vector<int> indices;
        double dd;
        for (int j=0; j < sites.size(); ++j) {
			dd = boundary_strategy->distance(sites[i]->get_coords(), sites[j]->get_coords());
            if(dd < max_disp && i != j) {
				distances.push_back(dd);
				indices.push_back(j);
            }
        }
        return {distances,indices};
}

/**
 * Function to 'wrap' a real-valued number into the interval from zero to a maximum specified by the parameter 'range': if the input value is outside the interval it is wrapped around so that the output is within the interval.
 * @param[in] value the real-valued number to be wrapped around
 * @param[in] range the maximum value of the interval to be wrapped into
 * @return the wrapped number within the interval
 */
double RadialDispersal::wrap_around(double value, double range)
{
	return std::fmod(std::fmod(value, range) + range, range);
}

/**
 * @brief Computes the union of overlapping intervals and calculates the difference in the sum of lengths between the merged intervals and the original intervals.
 * @details This function takes a single interval `qq` and a vector of intervals `input`. It merges
 * any overlapping intervals, includes the non-overlapping intervals as they are, and 
 * calculates the total sum of lengths of the resulting intervals. It also computes the
 * difference between the total sum of lengths of the merged intervals and the original 
 * interval. This is required to compute the extent of flight path between a focal patch and a recieving patch, while accounting for the shadowing effect of nearer patches
 * @param[in] qq A pair of doubles representing the interval to be merged with the input intervals.
 *           The first value is the start of the interval, and the second value is the end of the interval.
 * @param[in] input A vector of pairs of doubles, where each pair represents an interval. Each interval
 *              has a start value (first) and an end value (second).
 * @return A pair consisting of:
 *         - A vector of pairs of doubles representing the union of the merged and non-overlapping intervals,
 *           sorted by the start of each interval.
 *         - A double representing the difference between the sum of lengths of the merged intervals and the 
 *           original intervals.
 * @note The function assumes that each interval in the input is well-formed, meaning that for each interval
 *       `std::pair<double, double>`, the first value (start) is less than or equal to the second value (end).
 */
std::pair<std::vector<std::pair<double, double>>, double> RadialDispersal::compute_interval_union(const std::pair<double, double>& qq,
 const std::vector<std::pair<double, double>>& input)
{
	// Create a vector to store the union of intervals
	std::vector<std::pair<double, double>> output;

	// Merge overlapping intervals in the output vector
	std::pair<double, double> merged_interval = qq;
	for (const auto& interval : input) {
		if (interval.second < merged_interval.first || interval.first > merged_interval.second) {
			output.push_back(interval);
		}
		else {
			merged_interval.first = std::min(merged_interval.first, interval.first);
			merged_interval.second = std::max(merged_interval.second, interval.second);
		}
	}

	// Add the merged interval
	output.push_back(merged_interval);

	// Calculate the difference in the sum of lengths
	double sum_lengths = 0.0;
	for (const auto& interval : output) {
		sum_lengths += interval.second - interval.first;
	}
	double input_sum_lengths = 0.0;
	for (const auto& interval : input) {
		input_sum_lengths += interval.second - interval.first;
	}
	double diff = sum_lengths - input_sum_lengths;

	std::sort(output.begin(), output.end());
	return {output, diff};
}


/**
 * Sorts the indices of the vector elements based on the numeric value of the corresponding element (in ascending order).
 * @param[in] numbers vector to sort
 * @return The sorted indices vector.
 */
std::vector<int> RadialDispersal::get_sorted_positions(const std::vector<double>& numbers) 
{
	// Create a vector of indices (0 to N-1)
	std::vector<int> indices(numbers.size());
	std::iota(indices.begin(), indices.end(), 0);

	// Sort the indices based on the corresponding values in the vector
	std::sort(indices.begin(), indices.end(), [&numbers](int a, int b) {return numbers[a] < numbers[b];});

	return indices;
}
