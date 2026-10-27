#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <limits>
#include <chrono>
#include <vector>
#include "Simulation.h"
#include "Params.h"
#include "inputval.h"
#include "InputParams.h"
#include "sets.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

int main()
{	
	std::vector<InputParams> sets = {set1, set2, set3, set4, set5, set6, set7, set8, set9, set10, set11, set12, set13, set14, set15, set16, set17, set18};
	
	// input parameters
	// progression parameters
	int num_runs;
	int max_t; 

	// area parameters
	int num_pat;  
	
	// model parameters
	double mu_j;
	double mu_a;
	double beta;
	double theta;
	double comp_power;
	int min_dev;

	// gene drive inheritance parameters
	double gamma;
	double xi;
	double e;

	// gene drive release parameters
	int driver_start;
	int num_driver_M;
	int num_driver_sites; 

	// dispersal parameters 
	double disp_rate; 
	double max_disp; 

	// aestivation parameters
	double psi;
	double mu_aes;
	int t_hide1;
	int t_hide2;
	int t_wake1; 
	int t_wake2;

	// seasonality parameters
	double alpha0_mean;
	double alpha0_variance;
	double alpha1;
	double amp;
	double resp;

	// data-recording parameters
	int rec_start; 
	int rec_end;
	int rec_interval_global;
	int rec_interval_local;
	int rec_sites_freq; 
	int set_label; 

	bool continue_program = true;

	while (continue_program) {
		std::cout << "Welcome to GDSiMS: the Gene Drive Simulator of Mosquito Spread. \n";
		std::cout << "Set 1  - default \n";
		std::cout << "Set 2  - 1 population \n";
		std::cout << "Set 3  - no dispersal \n";
		std::cout << "Set 4  - full mixing \n";
		std::cout << "Set 5  - 1 day \n";
		std::cout << "Set 6  - high fitness cost \n";
		std::cout << "Set 7  - low aestivation \n";
		std::cout << "Set 8  - high aestivation \n";
		std::cout << "Set 9  - no gene drive \n";
		std::cout << "Set 10 - high gene drive \n";
		std::cout << "Set 11 - 1 population, full dispersal \n";
		std::cout << "Set 12 - low seasonality, default rainfall \n";
		std::cout << "Set 13 - high seasonality, default rainfall \n";
		std::cout << "Set 14 - low seasonality, pre-defined rainfall \n";
		std::cout << "Set 15 - high seasonality, pre-defined rainfall \n";
		std::cout << "Set 16 - alpha0 variance \n";
		std::cout << "Set 17 - multiple release times \n";
		std::cout << "Set 18 - high juvenile survival probability \n";
		std::cout << "Set 100  - custom \n";
		std::cout << "Default parameter sets include gene drive and dispersal but don't include aestivation unless otherwise stated. \n \n";
		std::cout << "Select which parameter set (1-" << std::to_string(sets.size()) << ", 100) you'd like to preview, \n";
		std::cout << "or enter 0 to exit the program:" << std::endl;

		int option1;
		char option2;
		std::cin.clear();
		std::cin >> option1;

		if (std::cin.fail() || std::cin.peek() != '\n' || option1 < 0 || (option1 > sets.size() && option1 != 100)) {
			do {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid option. Please enter a number between 0 and " << std::to_string(sets.size()) << " (or 100):" << std::endl;
				std::cin >> option1;
			} while (std::cin.fail() || std::cin.peek() != '\n' || option1 < 0 || (option1 > sets.size() && option1 != 100));
		}
		if (option1 == 0) {
			continue_program = false;
		}
		else if (option1 == 100) {
			bool invalid_input = true;
			while (invalid_input) {
				std::cout << "\n" << "Custom set: \n";
				std::cout << "num_runs             " << "\n"; 
				std::cout << "max_t                " << "\n"; 
				std::cout << "num_pat              " << "\n";  
				std::cout << "mu_j                 " << "\n"; 
				std::cout << "mu_a                 " << "\n"; 
				std::cout << "beta                 " << "\n"; 
				std::cout << "theta                " << "\n";  
				std::cout << "comp_power           " << "\n"; 
				std::cout << "min_dev              " << "\n"; 
				std::cout << "gamma                " << "\n"; 
				std::cout << "xi                   " << "\n"; 
				std::cout << "e                    " << "\n"; 
				std::cout << "driver_start         " << "\n"; 
				std::cout << "num_driver_M         " << "\n"; 
				std::cout << "num_driver_sites     " << "\n"; 
				std::cout << "disp_rate            " << "\n"; 
				std::cout << "max_disp             " << "\n"; 
				std::cout << "psi                  " << "\n"; 
				std::cout << "mu_aes               " << "\n"; 
				std::cout << "t_hide1              " << "\n"; 
				std::cout << "t_hide2              " << "\n"; 
				std::cout << "t_wake1              " << "\n"; 
				std::cout << "t_wake2              " << "\n";
				std::cout << "alpha0_mean          " << "\n";
				std::cout << "alpha0_variance      " << "\n";
				std::cout << "alpha1               " << "\n";
				std::cout << "amp                  " << "\n";
				std::cout << "resp                 " << "\n"; 
				std::cout << "rec_start            " << "\n"; 
				std::cout << "rec_end              " << "\n"; 
				std::cout << "rec_interval_global  " << "\n"; 
				std::cout << "rec_interval_local   " << "\n"; 
				std::cout << "rec_sites_freq       " << "\n"; 
				std::cout << "set_label            " << "\n"; 

				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				
				std::string params_filename;
				std::cout << "\n" << "Enter the filename or filepath of the parameters file (e.g. 'params.txt'). ";
				std::cout << "This must be a .txt file with parameter values in the above order and format: " << std::endl;
				std::getline(std::cin, params_filename);

				auto params_filepath = std::filesystem::path(params_filename);
				if (!std::filesystem::exists(params_filepath) || !std::filesystem::is_regular_file(params_filepath)) {
					do {
					std::cout << "Invalid filename. Enter either a filename from your build directory or the absolute or relative filepath:" << std::endl;
					std::getline(std::cin, params_filename);
					params_filepath = std::filesystem::path(params_filename);
					} while (!std::filesystem::exists(params_filepath) || !std::filesystem::is_regular_file(params_filepath));
				}

				std::ifstream file(params_filepath);
				if (file.is_open()) {
					if (!file_read_and_validate_type(file, num_runs, "num_runs", "int")) continue;
					if (!file_read_and_validate_type(file, max_t, "max_t", "int")) continue;
					if (!file_read_and_validate_type(file, num_pat, "num_pat", "int")) continue;
					if (!file_read_and_validate_type(file, mu_j, "mu_j", "double")) continue;
					if (!file_read_and_validate_type(file, mu_a, "mu_a", "double")) continue;
					if (!file_read_and_validate_type(file, beta, "beta", "double")) continue;
					if (!file_read_and_validate_type(file, theta, "theta", "double")) continue;
					if (!file_read_and_validate_type(file, comp_power, "comp_power", "double")) continue;
					if (!file_read_and_validate_type(file, min_dev, "min_dev", "int")) continue;
					if (!file_read_and_validate_type(file, gamma, "gamma", "double")) continue;
					if (!file_read_and_validate_type(file, xi, "xi", "double")) continue;
					if (!file_read_and_validate_type(file, e, "e", "double")) continue;
					if (!file_read_and_validate_type(file, driver_start, "driver_start", "int")) continue;
					if (!file_read_and_validate_type(file, num_driver_M, "num_driver_M", "int")) continue;
					if (!file_read_and_validate_type(file, num_driver_sites, "num_driver_sites", "int")) continue;
					if (!file_read_and_validate_type(file, disp_rate, "disp_rate", "double")) continue;
					if (!file_read_and_validate_type(file, max_disp, "max_disp", "double")) continue;
					if (!file_read_and_validate_type(file, psi, "psi", "double")) continue;
					if (!file_read_and_validate_type(file, mu_aes, "mu_aes", "double")) continue;
					if (!file_read_and_validate_type(file, t_hide1, "t_hide1", "int")) continue;
					if (!file_read_and_validate_type(file, t_hide2, "t_hide2", "int")) continue;
					if (!file_read_and_validate_type(file, t_wake1, "t_wake1", "int")) continue;
					if (!file_read_and_validate_type(file, t_wake2, "t_wake2", "int")) continue;
					if (!file_read_and_validate_type(file, alpha0_mean, "alpha0_mean", "double")) continue;
					if (!file_read_and_validate_type(file, alpha0_variance, "alpha0_variance", "double")) continue;
					if (!file_read_and_validate_type(file, alpha1, "alpha1", "double")) continue;
					if (!file_read_and_validate_type(file, amp, "amp", "double")) continue;
					if (!file_read_and_validate_type(file, resp, "resp", "double")) continue;
					if (!file_read_and_validate_type(file, rec_start, "rec_start", "int")) continue;
					if (!file_read_and_validate_type(file, rec_end, "rec_end", "int")) continue;
					if (!file_read_and_validate_type(file, rec_interval_global, "rec_interval_global", "int")) continue;
					if (!file_read_and_validate_type(file, rec_interval_local, "rec_interval_local", "int")) continue;
					if (!file_read_and_validate_type(file, rec_sites_freq, "rec_sites_freq", "int")) continue;
					if (!file_read_and_validate_type(file, set_label, "set_label", "int")) continue;
				}
				file.close();

				int bound_errors = 0; // so only run interval checks if bounds on variables are valid 
				// (and can display all bound errors at once)
				if (!check_bounds("num_runs", num_runs, 0, false)) bound_errors++;
				if (!check_bounds("max_t", max_t, 0, false)) bound_errors++;
				if (!check_bounds("num_pat", num_pat, 0, false)) bound_errors++;
				if (!check_bounds("mu_j", mu_j, 0.0, true, 1.0, false)) bound_errors++;
				if (!check_bounds("mu_a", mu_a, 0.0, false, 1.0, false)) bound_errors++;
				if (!check_bounds("beta", beta, 0.0, false)) bound_errors++;
				if (!check_bounds("theta", theta, 0.0, false)) bound_errors++;
				if (!check_bounds("comp_power", comp_power, 0.0, false)) bound_errors++;
				if (!check_bounds("min_dev", min_dev, 0, false)) bound_errors++;
				if (!check_bounds("gamma", gamma, 0.0, true, 1.0, true)) bound_errors++;
				if (!check_bounds("xi", xi, 0.0, true, 1.0, true)) bound_errors++;
				if (!check_bounds("e", e, 0.0, true, 1.0, true)) bound_errors++;
				if (!check_bounds("driver_start", driver_start, 1)) bound_errors++;
				if (!check_bounds("num_driver_M", num_driver_M, 0)) bound_errors++;
				if (!check_bounds("num_driver_sites", num_driver_sites, 0)) bound_errors++;
				if (!check_bounds("disp_rate", disp_rate, 0.0, true, 1.0, true)) bound_errors++;
				if (!check_bounds("max_disp", max_disp, 0.0, false)) bound_errors++;
				if (!check_bounds("psi", psi, 0.0, true, 1.0, true)) bound_errors++;
				if (!check_bounds("mu_aes", mu_aes, 0.0, true, 1.0, true)) bound_errors++;
				if (psi > 0) {
					if (!check_bounds("t_hide1", t_hide1, 1, true, 365, true)) bound_errors++;
					if (!check_bounds("t_hide2", t_hide2, 1, true, 365, true)) bound_errors++;
					if (!check_bounds("t_wake1", t_wake1, 1, true, 365, true)) bound_errors++;
					if (!check_bounds("t_wake2", t_wake2, 1, true, 365, true)) bound_errors++;
				}
				if (!check_bounds("alpha0_mean", alpha0_mean, 0.0, false)) bound_errors++;
				if (!check_bounds("alpha0_variance", alpha0_variance, 0.0, true)) bound_errors++;
				if (!check_bounds("alpha1", alpha1, 0.0, true)) bound_errors++;
				if (!check_bounds("amp", amp, 0.0, true, 1.0, true)) bound_errors++;
				if (!check_bounds("resp", resp, 0.0, true)) bound_errors++;
				if (!check_bounds("rec_start", rec_start, 0)) bound_errors++;
				if (!check_bounds("rec_end", rec_end, 0)) bound_errors++;
				if (!check_bounds("rec_interval_global", rec_interval_global, 1)) bound_errors++;
				if (!check_bounds("rec_interval_local", rec_interval_local, 1)) bound_errors++;
				if (!check_bounds("rec_sites_freq", rec_sites_freq, 1)) bound_errors++;
				if (!check_bounds("set_label", set_label, 0)) bound_errors++;

				if (bound_errors == 0) {
					int interval_errors = 0; // so can display all errors at once before continuing while loop
					// (only valid because variables don't overlap between different errors)
					if (psi > 0) {
						if (!check_interval("t_hide1", "t_hide2", t_hide1, t_hide2)) interval_errors++;
						if (!check_interval("t_wake1", "t_wake2", t_wake1, t_wake2)) interval_errors++;
					}
					if (!check_interval("rec_start", "rec_end", rec_start, rec_end)) interval_errors++;

					if (interval_errors != 0) continue;
				}
				else continue;

				invalid_input = false;
			}

			std::cout << "\n" << "Custom set: \n";
			std::cout << "num_runs             " << num_runs << "\n"; 
			std::cout << "max_t                " << max_t << "\n"; 
			std::cout << "num_pat              " << num_pat << "\n";  
			std::cout << "mu_j                 " << mu_j << "\n"; 
			std::cout << "mu_a                 " << mu_a << "\n"; 
			std::cout << "beta                 " << beta << "\n"; 
			std::cout << "theta                " << theta << "\n"; 
			std::cout << "comp_power           " << comp_power << "\n"; 
			std::cout << "min_dev              " << min_dev << "\n"; 
			std::cout << "gamma                " << gamma << "\n"; 
			std::cout << "xi                   " << xi << "\n"; 
			std::cout << "e                    " << e << "\n"; 
			std::cout << "driver_start         " << driver_start << "\n"; 
			std::cout << "num_driver_M         " << num_driver_M << "\n"; 
			std::cout << "num_driver_sites     " << num_driver_sites << "\n"; 
			std::cout << "disp_rate            " << disp_rate << "\n"; 
			std::cout << "max_disp             " << max_disp << "\n"; 
			std::cout << "psi                  " << psi << "\n"; 
			std::cout << "mu_aes               " << mu_aes << "\n"; 
			std::cout << "t_hide1              " << t_hide1 << "\n"; 
			std::cout << "t_hide2              " << t_hide2 << "\n"; 
			std::cout << "t_wake1              " << t_wake1 << "\n"; 
			std::cout << "t_wake2              " << t_wake2 << "\n"; 
			std::cout << "alpha0_mean          " << alpha0_mean << "\n"; 
			std::cout << "alpha0_variance      " << alpha0_variance << "\n";
			std::cout << "alpha1               " << alpha1 << "\n";
			std::cout << "amp                  " << amp << "\n";
			std::cout << "resp                 " << resp << "\n";
			std::cout << "rec_start            " << rec_start << "\n"; 
			std::cout << "rec_end              " << rec_end << "\n"; 
			std::cout << "rec_interval_global  " << rec_interval_global << "\n"; 
			std::cout << "rec_interval_local   " << rec_interval_local << "\n"; 
			std::cout << "rec_sites_freq       " << rec_sites_freq << "\n"; 
			std::cout << "set_label            " << set_label << "\n"; 

			if (num_driver_sites > num_pat) {
				std::cout << "Warning: num_driver_sites > num_pat. ";
				std::cout << "This simulation will have reduced gene drive release sites num_driver_sites = num_pat." << std::endl;
			}
			if (driver_start > max_t || num_driver_sites == 0 || num_driver_M == 0) {
				std::cout << "Warning: num_driver_sites or num_driver_M = 0, or driver_start > max_t. ";
				std::cout << "This simulation will not include gene drive." << std::endl;
				std::cout << "Inheritance parameters xi, e and gamma will have no effect." << std::endl;
			} 
			if (disp_rate == 0 || max_disp == 0) {
				std::cout << "Warning: disp_rate or max_disp = 0. This simulation will not include dispersal." << std::endl;
			} 
			if (t_hide1 > max_t || t_hide2 > max_t || t_wake1 > max_t || t_wake2 > max_t) {
				std::cout << "Warning: the aestivation interval times are larger than max_t. ";
				std::cout << "This simulation will only run partly through the aestivation period." << std::endl;
			}
			if (psi == 0) {
				std::cout << "Warning: psi = 0. This simulation will not include aestivation." << std::endl;
			} 

			if (rec_start > max_t) {
				std::cout << "Warning: rec_start > max_t. This simulation will not include local recording." << std::endl;
			}
			if (max_t - rec_interval_local - rec_start < 0) {
				std::cout << "Warning: the interval between rec_start and max_t is larger than rec_interval_local. ";
				std::cout << "This simulation will only record local data for day 0." << std::endl;
			}

			std::cout << "\n" << "Would you like to run the simulation with this set of parameters? (y/n)" << std::endl;

			bool invalid_option_custom = true;
			while (invalid_option_custom) {
				option2 = 'n';
				std::cin >> option2;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				switch (option2) {
					case 'y':
					{
						InputParams custom_input;
						custom_input.num_runs = num_runs;
						custom_input.max_t = max_t;
						custom_input.num_pat = num_pat;
						custom_input.mu_j = mu_j;
						custom_input.mu_a = mu_a;
						custom_input.beta = beta;
						custom_input.theta = theta;
						custom_input.comp_power = comp_power;
						custom_input.min_dev = min_dev;
						custom_input.gamma = gamma;
						custom_input.xi = xi;
						custom_input.e = e;
						custom_input.driver_start = driver_start;
						custom_input.num_driver_M = num_driver_M;
						custom_input.num_driver_sites = num_driver_sites;
						custom_input.disp_rate = disp_rate;
						custom_input.max_disp = max_disp;
						custom_input.psi = psi;
						custom_input.mu_aes = mu_aes;
						custom_input.t_hide1 = t_hide1;
						custom_input.t_hide2 = t_hide2;
						custom_input.t_wake1 = t_wake1;
						custom_input.t_wake2 = t_wake2;
						custom_input.alpha0_mean = alpha0_mean;
						custom_input.alpha0_variance = alpha0_variance;
						custom_input.alpha1 = alpha1;
						custom_input.amp = amp;
						custom_input.resp = resp;
						custom_input.rec_start = rec_start;
						custom_input.rec_end = rec_end;
						custom_input.rec_interval_global = rec_interval_global;
						custom_input.rec_interval_local = rec_interval_local;
						custom_input.rec_sites_freq = rec_sites_freq;
						custom_input.set_label = set_label;

						InheritanceParams inher;
						inher.gamma = gamma;
						inher.xi = xi;
						inher.e = e;

						// set up simulation
						Simulation simulation_1(custom_input);
						simulation_1.set_inheritance(inher);

						// advanced option setting
						char option3;
						bool invalid_option_adv_enter = true;
						std::cout << "Would you like to set any advanced options before running? (y/n)" << std::endl;
						while (invalid_option_adv_enter) {
							option3 = 'n';
							std::cin >> option3;
							std::cin.clear();
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							bool invalid_option_adv_num = true;
							switch (option3) {
								case 'y':
									int option4;
									while (invalid_option_adv_num) {
										std::cout << "\nAdvanced options: \n";
										std::cout << "1 - Boundary type \n";
										std::cout << "2 - Dispersal type \n";
										std::cout << "3 - Custom rainfall \n";
										std::cout << "4 - Custom patch coordinates \n";
										std::cout << "5 - Multiple gene drive release times \n";
										std::cout << "0 - Exit advanced options and run the program" << std::endl;
										std::cin >> option4;
										if (std::cin.fail() || std::cin.peek() != '\n' || option4 < 0 || option4 > 5) {
											do {
												std::cin.clear();
												std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
												std::cout << "Invalid option. Please enter a number between 0 and 5:" << std::endl;
												std::cin >> option4;
											} while (std::cin.fail() || std::cin.peek() != '\n' || option4 < 0 || option4 > 5);
										}
										std::cin.clear();
										std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
										if (option4 == 0) {
											invalid_option_adv_enter = false;
											invalid_option_adv_num = false;
										}
										else if (option4 == 1) {
											char boundary_option;
											bool invalid_boundary_option = true;
											BoundaryType boundary_type;
											std::vector<std::string> boundaries = {"Toroid", "Edge"};
											std::cout << "Boundary type options: " << boundaries[0] << " / " << boundaries[1];
											std::cout << " (t/e)" << std::endl;
											while (invalid_boundary_option) {
												std::cin >> boundary_option;
												std::cin.clear();
												std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
												switch (boundary_option) {
													case 't':
														boundary_type = Toroid;
														invalid_boundary_option = false;
														break;
													case 'e':
														boundary_type = Edge;
														invalid_boundary_option = false;
														break;
													default:
														std::cout << "\n" << "Invalid option. Please select again:" << std::endl;
														break;
												}
											}
											simulation_1.set_boundary_type(boundary_type);
											std::cout << "Boundary type set to " << boundaries[boundary_type] << "." << std::endl;
										}
										else if (option4 == 2) {
											char disp_option;
											bool invalid_disp_option = true;
											DispersalType disp_type;
											std::vector<std::string> dispersal_types = {"Distance Kernel", "Radial"};
											std::cout << "Dispersal type options: " << dispersal_types[0] << " / " << dispersal_types[1];
											std::cout << " (d/r)" << std::endl;
											while (invalid_disp_option) {
												std::cin >> disp_option;
												std::cin.clear();
												std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
												switch (disp_option) {
													case 'd':
														disp_type = DistanceKernel;
														invalid_disp_option = false;
														break;
													case 'r':
														disp_type = Radial;
														invalid_disp_option = false;
														break;
													default:
														std::cout << "\n" << "Invalid option. Please select again:" << std::endl;
														break;
												}
											}
											simulation_1.set_dispersal_type(disp_type);
											std::cout << "Dispersal type set to " << dispersal_types[disp_type] << "." << std::endl;
										}
										else if (option4 == 3) {
											std::cout << "Note: the resp value used for custom rainfall will be the one ";
											std::cout << "previously entered as part of the simulation parameters." << std::endl;
											std::string rainfall_filename;
											std::cout << "Enter the filename or filepath of the rainfall file (e.g. 'rainfall.txt'). ";
											std::cout << "This must be a .txt file with values delimited by \\n: " << std::endl;
											std::getline(std::cin, rainfall_filename);

											auto rainfall_filepath = std::filesystem::path(rainfall_filename);
											if (!std::filesystem::exists(rainfall_filepath) ||
											 !std::filesystem::is_regular_file(rainfall_filepath)) {
												do {
													std::cout << "Invalid filename. Enter either a filename from your build directory or the absolute or relative filepath:" << std::endl;
													std::getline(std::cin, rainfall_filename);
													rainfall_filepath = std::filesystem::path(rainfall_filename);
												} while (!std::filesystem::exists(rainfall_filepath) ||!std::filesystem::is_regular_file(rainfall_filepath));
											}
											simulation_1.set_rainfall(rainfall_filepath);
											std::cout << "Custom rainfall values set." << std::endl;
										}
										else if (option4 == 4) {
											std::string coords_filename;
											std::cout << "\n" << "Enter the filename or filepath of the patch coordinates file ";
											std::cout << "(e.g. 'coords.txt'). ";
											std::cout << "This must be a .txt file in x y char\\n x y char table format:" << std::endl;
											std::getline(std::cin, coords_filename);

											auto coords_filepath = std::filesystem::path(coords_filename);
											if (!std::filesystem::exists(coords_filepath) ||
											 !std::filesystem::is_regular_file(coords_filepath)) {
												do {
													std::cout << "Invalid filename. Enter either a filename from your build directory or the absolute or relative filepath:" << std::endl;
													std::getline(std::cin, coords_filename);
													coords_filepath = std::filesystem::path(coords_filename);
												} while (!std::filesystem::exists(coords_filepath) ||
												 !std::filesystem::is_regular_file(coords_filepath));
											}
											simulation_1.set_coords(coords_filepath);
											std::cout << "Custom coordinates set." << std::endl;
										}
										else if (option4 == 5) {
											std::string times_filename;
											std::cout << "\n" << "Enter the filename or filepath of the gene drive release times";
											std::cout << "(e.g. 'rel_times.txt'). ";
											std::cout << "This must be a .txt file, with values delimited by \\n:" << std::endl;
											std::getline(std::cin, times_filename);

											auto times_filepath = std::filesystem::path(times_filename);
											if (!std::filesystem::exists(times_filepath) ||
											 !std::filesystem::is_regular_file(times_filepath)) {
												do {
													std::cout << "Invalid filename. Enter either a filename from your build directory or the absolute or relative filepath:" << std::endl;
													std::getline(std::cin, times_filename);
													times_filepath = std::filesystem::path(times_filename);
												} while (!std::filesystem::exists(times_filepath) ||
												 !std::filesystem::is_regular_file(times_filepath));
											}
											simulation_1.set_release_times(times_filepath);
											std::cout << "Multiple release times set." << std::endl;
										}
									}
									break;
								case 'n':
									std::cout << std::endl;
									invalid_option_adv_enter = false;
									break;
								default:
									std::cout << "\n" << "Invalid option. Please select again:" << std::endl;
									break;
							}
						}

						// run the simulation;
						auto start_1 = std::chrono::steady_clock::now();
						simulation_1.run_reps();

						auto finish_1 = std::chrono::steady_clock::now();
						double elapsed_seconds_1 = std::chrono::duration_cast<std::chrono::duration<double>>(finish_1 - start_1).count();
						std::cout << "Program run time: " << std::endl;
						printf("%.10f\n", elapsed_seconds_1);

						invalid_option_custom = false;
						continue_program = false;
						break;
					}
					case 'n':
						std::cout << std::endl;
						invalid_option_custom = false;
						break;
					default:
						std::cout << "\n" << "Invalid option. Please select again:" << std::endl;
						break;
				}
			}
		}

		else {
			std::cout << "\n" << "Set " << option1 << ": \n";
			std::cout << "num_runs             " << sets[option1 - 1].num_runs << "\n"; 
			std::cout << "max_t                " << sets[option1 - 1].max_t << "\n"; 
			std::cout << "num_pat              " << sets[option1 - 1].num_pat << "\n";  
			std::cout << "mu_j                 " << sets[option1 - 1].mu_j << "\n"; 
			std::cout << "mu_a                 " << sets[option1 - 1].mu_a << "\n"; 
			std::cout << "beta                 " << sets[option1 - 1].beta << "\n"; 
			std::cout << "theta                " << sets[option1 - 1].theta << "\n";  
			std::cout << "comp_power           " << sets[option1 - 1].comp_power << "\n"; 
			std::cout << "min_dev              " << sets[option1 - 1].min_dev << "\n"; 
			std::cout << "gamma                " << sets[option1 - 1].gamma << "\n"; 
			std::cout << "xi                   " << sets[option1 - 1].xi << "\n"; 
			std::cout << "e                    " << sets[option1 - 1].e << "\n"; 
			std::cout << "driver_start         " << sets[option1 - 1].driver_start << "\n"; 
			std::cout << "num_driver_M         " << sets[option1 - 1].num_driver_M << "\n"; 
			std::cout << "num_driver_sites     " << sets[option1 - 1].num_driver_sites << "\n"; 
			std::cout << "disp_rate            " << sets[option1 - 1].disp_rate << "\n"; 
			std::cout << "max_disp             " << sets[option1 - 1].max_disp << "\n"; 
			std::cout << "psi                  " << sets[option1 - 1].psi << "\n"; 
			std::cout << "mu_aes               " << sets[option1 - 1].mu_aes << "\n"; 
			std::cout << "t_hide1              " << sets[option1 - 1].t_hide1 << "\n"; 
			std::cout << "t_hide2              " << sets[option1 - 1].t_hide2 << "\n"; 
			std::cout << "t_wake1              " << sets[option1 - 1].t_wake1 << "\n"; 
			std::cout << "t_wake2              " << sets[option1 - 1].t_wake2 << "\n";
			std::cout << "alpha0_mean          " << sets[option1 - 1].alpha0_mean << "\n";
			std::cout << "alpha0_variance      " << sets[option1 - 1].alpha0_variance << "\n"; 
			std::cout << "alpha1               " << sets[option1 - 1].alpha1 << "\n";
			std::cout << "amp                  " << sets[option1 - 1].amp << "\n";
			std::cout << "resp                 " << sets[option1 - 1].resp << "\n";
			std::cout << "rec_start            " << sets[option1 - 1].rec_start << "\n"; 
			std::cout << "rec_end              " << sets[option1 - 1].rec_end << "\n"; 
			std::cout << "rec_interval_global  " << sets[option1 - 1].rec_interval_global << "\n"; 
			std::cout << "rec_interval_local   " << sets[option1 - 1].rec_interval_local << "\n"; 
			std::cout << "rec_sites_freq       " << sets[option1 - 1].rec_sites_freq << "\n"; 
			std::cout << "set_label            " << sets[option1 - 1].set_label << "\n"; 

			std::cout << "\n" << "Would you like to run the simulation with this set of parameters? (y/n)" << std::endl;

			bool invalid_option_default = true;
			while (invalid_option_default) {
				option2 = 'n';
				std::cin >> option2;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				switch (option2) {
					case 'y':
					{
						InputParams sim_params = sets[option1 - 1];

						InheritanceParams inher;
						inher.gamma = sim_params.gamma;
						inher.xi = sim_params.xi;
						inher.e = sim_params.e;
						
						auto start = std::chrono::steady_clock::now();

						// run simulation
						Simulation simulation(sim_params);
						if (option1 == 14 || option1 == 15) {simulation.set_rainfall("../includes/rainfall.txt");}
						if (option1 == 17) {simulation.set_release_times("../includes/rel_times.txt");}
						simulation.set_inheritance(inher);
						simulation.run_reps();

						auto finish = std::chrono::steady_clock::now();
						double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
						std::cout << "Program run time: " << std::endl;
						printf("%.10f\n", elapsed_seconds);

						invalid_option_default = false;
						continue_program = false;
						break;
					}
					case 'n':
						std::cout << std::endl;
						invalid_option_default = false;
						break;
					default:
						std::cout << "\n" << "Invalid option. Please select again:" << std::endl;
						break;
				}
			}
		}
	}
	
	return 0;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */