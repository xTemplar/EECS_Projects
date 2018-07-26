#include <iostream> //ostream
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <cstddef> 
#include <cstdlib>
#include <cmath>
#include <deque>
#include <getopt.h>
#include <float.h>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <unordered_set>

struct adj_list {
public:
	//keeps track of whether vertex was visited
	//bool visited;
	//keeps track of distance of edge
	double dist;
	//keeps track which vertex is adjacent to vertex
	int nbhr;
};

void ProcessArgs(int argc, char** argv, std::string &mode) {
	const char* const short_opts = "m:h";
	struct option long_opts[] = {
		{ "mode", required_argument, nullptr, 'm' }, 
		{ "help", optional_argument, nullptr, 'h'}
	};
	int opt = 0;
	int option_index = 0;
	while ((opt = getopt_long(argc, argv, short_opts,
		long_opts, &option_index)) != -1) {
		switch (opt) {
		case 'm':
			mode = optarg;
			if (mode == "") {
				std::cerr << "The usual stuff" << std::endl;
				exit(1);
			}
			break;
		case 'h':
			std::cerr << "Send Help or something" << std::endl;
			exit(0);
		}
	}
}

void InputData(int &size, std::vector<std::pair<int, int>> &data) {
	std::cin >> size;
	data.reserve(size);
	std::pair<int, int> temp;
	for (int i = 0; i < size; ++i) {
		std::cin >> temp.first;
		std::cin >> temp.second;
		data.push_back(temp);
	}
}

double Euc_Dist(std::pair<int, int> loc_1, std::pair<int, int> loc_2) {
	long long dist_1 = (loc_1.first - loc_2.first);
	dist_1 *= dist_1;
	long long dist_2 = (loc_1.second - loc_2.second);
	dist_2 *= dist_2;
	return sqrt(static_cast<double> (dist_1 + dist_2));
}

double get_weight(std::vector<adj_list> &min_tree, int size) {
	double sum = 0;
	for (int i = 0; i < size; ++i) {
		sum += min_tree[i].dist;
	}
	return sum;
}

double get_weight(std::vector<std::pair<int, int>> &data,
	std::vector<int> &path) {
	double sum = 0;
	for (int i = 0; i < static_cast<int>(data.size()) - 1; ++i) {
		sum += Euc_Dist(data[path[i]], data[path[i + 1]]);
	}
	sum += Euc_Dist(data[path[0]], data[path[data.size() - 1]]);
	return sum;
}

bool valid_path(std::pair<int, int> &loc_1, std::pair<int, int> &loc_2) {
	if (!((loc_1.first < 0 && loc_1.second < 0) ||
		(loc_2.first < 0 && loc_2.second < 0)) ||
		(loc_1.first <= 0 && loc_1.second <= 0 &&
			loc_2.first <= 0 && loc_2.second <= 0)) {
		return true;
	}
	return false;
}

void MST(std::vector<adj_list> &min_tree,
	std::vector<std::pair<int, int>> &data, int size, 
	bool print, bool not_cage) {
	std::unordered_set<int> unvisited;
	for (int i = 0; i < size; ++i) {
		unvisited.insert(i);
	}
	for (int i = 1; i < size; ++i) {
		//min_tree[i].visited = false;
		min_tree[i].dist = DBL_MAX;
	}
	min_tree[0].dist = 0;
	min_tree[0].nbhr = -1;
	while (!unvisited.empty()) {
		double min = DBL_MAX;
		int min_index = -1;
		for (int i : unvisited) {
			if (min_tree[i].dist < min) {
				min_index = i;
				min = min_tree[i].dist;
			}
		}
		unvisited.erase(min_index);
		//min_tree[min_index].dist = min;
		for (int j : unvisited) {
			if (not_cage || valid_path(data[min_index], data[j])) {
				if (min_tree[j].dist > Euc_Dist(data[min_index], data[j])) {
					min_tree[j].dist = Euc_Dist(data[min_index], data[j]);
					min_tree[j].nbhr = min_index;
				}
			}
		}
	}
	if (print) {
		std::cout << get_weight(min_tree, size) << '\n';
		for (int i = 1; i < size; ++i) {
			if (i < min_tree[i].nbhr) {
				std::cout << i << " " << min_tree[i].nbhr << '\n';
			}
			else {
				std::cout << min_tree[i].nbhr << " " << i << '\n';
			}
		}
	}
}

void FASTTSP(std::vector<std::pair<int, int>> &data,
	std::vector<int> &partial_tour,
	std::vector<int> &unvisited,
	int size, bool print) {
	for (int &i : partial_tour) {
		i = -1;
	}
	int temp = unvisited.front(), temp_nbhr = 0;
	int start = temp;
	int node_1, node_2, node_3;
	double min = DBL_MAX;
	//initialization
	for (int i = 1; i < size; ++i) {
		double distance = Euc_Dist(data[i], data[0]);
		if (distance < min) {
			min = distance;
			node_1 = i;
		}
	}
	unvisited.erase(unvisited.begin() + node_1);
	unvisited.erase(unvisited.begin());
	//form tour of beginning node with closest node
	temp_nbhr = node_1;
	partial_tour[temp] = temp_nbhr;
	std::swap(temp, temp_nbhr);
	partial_tour[temp] = temp_nbhr;
	//selection
	while (!unvisited.empty()) {
		min = DBL_MAX;
		int i = unvisited.front();
		start = 0;
		int j = start;
		int k = partial_tour[j];
		node_3 = k;
		node_2 = j;
		while (k != start) {
			double dist_1 = Euc_Dist(data[i], data[k]);
			double dist_2 = Euc_Dist(data[i], data[j]);
			double dist_3 = Euc_Dist(data[j], data[k]);
			if (dist_1 + dist_2 - dist_3 < min) {
				node_1 = i;
				node_2 = j;
				node_3 = k;
				min = dist_1 + dist_2 - dist_3;
			}
			j = k;
			k = partial_tour[j];
		}
		temp = i;
		temp_nbhr = node_3;
		partial_tour[temp] = temp_nbhr;
		partial_tour[node_2] = i;
		unvisited.erase(unvisited.begin());
	}
	if (print) {
		double sum = 0;
		for (auto j : partial_tour) {
			sum += Euc_Dist(data[j], data[partial_tour[j]]);
		}
		std::cout << sum << '\n';
		std::cout << "0" << " ";
		int current = partial_tour[0];
		while (current != 0) {
			std::cout << current << " ";
			current = partial_tour[current];
		}
	}
}

double lowerbound(std::vector<std::pair<int, int>> &data,
	std::vector<int> &path, size_t permLength) {
	std::vector<std::pair<int, int>> new_data;
	double sum = 0;
	double min_1 = DBL_MAX;
	double min_2 = DBL_MAX;
	//weight of fixed path
	for (int i = 0; i < static_cast<int> (permLength) - 1; ++i) {
		sum += Euc_Dist(data[path[i]], data[path[i + 1]]);
	}
	//trimming data to only unfixed path
	for (size_t i = permLength;	i < data.size(); ++i) {
		new_data.push_back(data[path[i]]);
	}
	std::vector<adj_list> min_tree(new_data.size());
	//constructs MST out of unfixed vertices
	MST(min_tree, new_data, static_cast<int>(new_data.size()), false, true);
	//connect end of fixed path to closest unfixed vertex in MST
	for (size_t i = permLength; i < data.size(); ++i) {
		double dist_1 = Euc_Dist(data[path[i]], data[0]);
		double dist_2 = Euc_Dist(data[path[i]], data[path[permLength - 1]]);
		if (dist_1 < min_1) {
			min_1 = dist_1;
		}
		if (dist_2 < min_2) {
			min_2 = dist_2;
		}
	}
	sum += (min_1 + min_2);
	return (sum + get_weight(min_tree, static_cast<int>(new_data.size())));
}

bool promising(std::vector<std::pair<int, int>> &data,
	std::vector<int> &path, size_t permLength, double &currBest) {
	double l_bound = lowerbound(data, path, permLength);
	if (l_bound < currBest) {
		return true;
	}
	else {
		return false;
	}
}

void genPerms(std::vector<std::pair<int, int>> &data, 
	std::vector<int> &path, size_t permLength, std::vector<int> &solution,
	double &currBest) {
	if (path.size() == permLength) {
		double weight = get_weight(data, path);
		if (weight < currBest) {
			solution = path;
			currBest = weight;
		}
		return;
	}
	if (!promising(data, path, permLength, currBest)) {
		return;
	}
	for (size_t i = permLength; i < path.size(); ++i) {
		std::swap(path[permLength], path[i]);
		genPerms(data, path, permLength + 1, solution, currBest);
		std::swap(path[permLength], path[i]);
	}
}


void OPTTSP(std::vector<std::pair<int, int>> &data, int size) {
	std::vector<int> solution(size);
	std::vector<int> path(size);
	size_t permLength = 1;
	std::vector<int> unvisited(size);
	for (int i = 0; i < size; ++i) {
		unvisited[i] = i;
	}
	std::vector<int> partial_tour;
	partial_tour.resize(size);
	FASTTSP(data, partial_tour, unvisited, size, false);
	int j = 0;
	for (int i = 0; i < size; ++i) {
		path[i] = j;
		j = partial_tour[j];
	}
	double currBest = get_weight(data, path);
	solution = path;
	genPerms(data, path, permLength, solution, currBest);
	double sum = get_weight(data, solution);
	std::cout << sum << '\n'; 
	for (int vertex : solution) {
		std::cout << vertex << " ";
	}
}

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);
	std::cout << std::setprecision(2);//Always show 2 decimal places
	std::cout << std::fixed; //Disable scientific notation for large numbers
	std::vector<std::pair<int, int>> data;
	std::string mode;
	int size = 0;
	ProcessArgs(argc, argv, mode);
	InputData(size, data);
	if (mode == "MST") {
		std::vector<adj_list> min_tree(size);
		MST(min_tree, data, size, true, false);
	}
	else if (mode == "FASTTSP") {
		std::vector<int> unvisited(size);
		for (int i = 0; i < size; ++i) {
			unvisited[i] = i;
		}
		std::vector<int> partial_tour;
		partial_tour.resize(size);
		FASTTSP(data, partial_tour, unvisited, size, true);
	}
	else if (mode == "OPTTSP") {
		OPTTSP(data, size);
	}
	else {
		std::cerr << "Error Mode" << std::endl;
		exit(1);
	}
 	return 0;
}