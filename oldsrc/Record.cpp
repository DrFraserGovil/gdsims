#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "Record.h"

/**
 * Record constructor.
 * Creates LocalData, Totals and CoordinateList output .txt files.
 * @details Creates a subdirectory for output files in the current directory.
 * @param[in] rec_params 	recording parameters
 * @param[in] rep 			initial repetition label for the given set of runs
 */
Record::Record(RecordParams* rec_params, int rep) 
{
	rec_start = rec_params->rec_start;
	rec_end = rec_params->rec_end;
	rec_interval_global = rec_params->rec_interval_global;
	rec_interval_local = rec_params->rec_interval_local;
	rec_sites_freq = rec_params->rec_sites_freq;
	set_label = rec_params->set_label;
	rep_label = rep;

	next_local_day = rec_params->rec_start;
	next_global_day = 0;

	// create folder for destination of output files 
	if (!std::filesystem::exists("./output_files")) {
		std::filesystem::create_directory("output_files");
	}
	std::filesystem::current_path("./output_files");
	
	os1 << "LocalData" << set_label << "run" << rep_label << ".txt"; 
	local_data.open(os1.str());
	os2 << "Totals" << set_label << "run" << rep_label << ".txt";
	global_data.open(os2.str());
	os3 << "CoordinateList" << set_label << "run" << rep_label << ".txt";
	coord_list.open(os3.str());

	local_data << "Adult female populations of each genotype at each site\n";
	local_data << "Day" << "\t" << "Site" << "\t" << "WW" << "\t" << "WD" << "\t" << "DD" << "\t" << "WR" << "\t" << "RR" << "\t" << "DR" << std::endl;

	global_data << "Total adult females of each genotype\n";
	global_data << "Day" << "\t" << "WW" << "\t" << "WD" << "\t" << "DD" << "\t" << "WR" << "\t" << "RR" << "\t" << "DR" << std::endl;

	coord_list << "Coordinate list of the sites\n";
	coord_list << "Site" << "\t" << "x" << "\t" << "y" << std::endl;
}

/**
 * Record destructor.
 * Resets the current filepath so the output_files directory can be found in the next set of runs. 
 */
Record::~Record()
{
	std::filesystem::current_path("..");
}

/**
 * @brief Records the coordinates of the population sites. 
 * @details Relevant parameters include the fraction of sites to collect data for.
 * @param[in] sites vector of all Patch objects
 * @see InputParams::rec_sites_freq
 */
void Record::record_coords(const std::vector<Patch*> &sites) 
{
	const auto default_precision{std::cout.precision()};
	constexpr auto max_precision{std::numeric_limits<double>::digits10 + 1};
	for (int pat=0; pat < sites.size(); pat += rec_sites_freq) {
		auto coords = sites[pat]->get_coords();
		coord_list << pat+1 << "\t" << std::setprecision(max_precision) << coords.x << "\t" << coords.y << std::endl;
	}
	coord_list << std::setprecision(default_precision);
}

/**
 * @brief Records the total numbers of adult mated female mosquitoes for the given day, divided by female genotype. 
 * @details The totals are assumed to be across all sites. 
 * @param[in] day 		simulation day
 * @param[in] tot_fem_gen total number of adult mated females divided by female genotype
 * @see Model::calculate_tot_F_gen(), Patch::get_F()
 */
void Record::record_global(int day, const std::array<long long int, constants::num_gen> &tot_F_gen)
{
	global_data << day;
	for (const auto& f_gen : tot_F_gen) {
		global_data << "\t" << f_gen;
	}
	global_data << std::endl;
	next_global_day += rec_interval_global;
}

/**
 * @brief Outputs the total numbers of juvenile (J), adult male (M), adult virgin female (V) and adult mated female (F) mosquitoes for the given day.
 * @details The totals are assumed to be across all sites, and over all genotypes and age groups. 
 * @param[in] day 	simulation day
 * @param[in] tot_J	total number of juveniles
 * @param[in] tot_M	total number of adult males
 * @param[in] tot_V	total number of adult virgin (unmated) females
 * @param[in] tot_F	total number of adult mated females
 * @see Patch
 */
void Record::output_totals(int day, long long int tot_J, long long int tot_M, long long int tot_V, long long int tot_F)
{
	if (day == 0) {
		std::cout << "\n" << "rep " << rep_label << "\n";
		std::cout << "day" << "   " << "total J" << "   " << "total M" << "   " << "total V" << "   " << "total F" << "\n";
	}
	std::cout << day << "     " << tot_J << "   " << tot_M << "   " << tot_V << "   " << tot_F << std::endl;
}

/**
 * @brief Records the number of adult mated females at each site for the given day.
 * @details The number of adult mated females at each site is divided by female genotype. Relevant parameters include the fraction of sites to collect data for.
 * @param[in] day 	simulation day
 * @param[in] sites vector of all Patch objects
 * @see InputParams::rec_sites_freq
 */
void Record::record_local(int day, const std::vector<Patch*> &sites) 
{
	for (int pat=0; pat < sites.size(); pat += rec_sites_freq) {
		local_data << day << "\t" << pat+1;
		for (const auto& fem_gen : sites[pat]->get_F_fem_gen()) {
			local_data << "\t" << fem_gen;
		}
		local_data << std::endl;
	}
	next_local_day += rec_interval_local;
}

/**
 * @brief Determines if it is time to record global data.
 * @details The number of adult males at each site is divided by genotype.  
 * @param[in] day 	simulation day
 * @return As you would expect.
 * @see Record::record_global()
 */
bool Record::is_rec_global_time(int day)
{
	return day == next_global_day;
}

/**
 * @brief Determines if it is time to record local data.
 * @note The initialisation day (day 0) will always be recorded, and the recording window will be inclusive of the start and end times. 
 * @details Other relevant parameters include the local recording interval. 
 * @param[in] day 	simulation day
 * @return As you would expect.
 * @see Record::record_local(), InputParams::rec_start, InputParams::rec_end, InputParams::rec_interval_local
 */
bool Record::is_rec_local_time(int day) 
{
	return day >= rec_start && day <= rec_end && day == next_local_day;
}