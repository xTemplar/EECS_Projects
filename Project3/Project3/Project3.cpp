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
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <unordered_map>

struct Logs {
public:
	//long long to keep track of time
	uint64_t time;
	//String to keep track of category
	std::string category = "";
	//String to keep track of messages
	std::string message = "";
	//Int to keep track of entryID
	int entryID;
};

struct Order {
	bool operator() (const Logs &a, const Logs &b) {
		if (a.time != b.time) {
			return a.time < b.time;
		}
		else {
			std::string std_a = "", std_b = "";
			for (char x : a.category) {
				std_a += static_cast<char> (std::tolower(x));
			}
			for (char x : b.category) {
				std_b += static_cast<char> (std::tolower(x));
			}
			if (std_a.compare(std_b) != 0) {
				return std_a < std_b;
			}
			else {
				return a.entryID < b.entryID;
			}
		}
	}
};

void InputData(std::vector<Logs> &master_log,
	std::unordered_map<std::string, std::vector<int>>& category,
	std::unordered_map<std::string, std::vector<int>>& keyword,
	std::vector<uint64_t>& time,
	std::vector<int> &order, std::string filename) {
	uint64_t time_var = 0;
	std::string category_var;
	std::string message_var;
	int entryID = 0;
	time.reserve(1);
	std::ifstream ifs;
	ifs.open(filename);
	//Attempt 1
	std::string line;
	std::string delimiter = "|";
	int num;
	while (std::getline(ifs, line)) {
		Logs temp;
		time_var = 0;
		std::string time_str = line.substr(0, line.find(delimiter));
		category_var = line.substr(line.find(delimiter) + 1, line.size() - 1);
		message_var = category_var.substr(category_var.find(delimiter) + 1, 
			category_var.size() - category_var.find(delimiter) - 1);
		category_var = category_var.substr(0, category_var.find(delimiter));
		int i = 0;
		//inputs timestamp log
		while (i < 13) {
			time_var = 100 * time_var 
				+ 10 * (time_str[i] - '0')
				+ time_str[i + 1] - '0';
			i = i + 3;
		}
		temp.time = time_var;
		//inputs category logs
		temp.category = category_var;
		//inputs message log
		temp.message = message_var;
		//inputs keyword logs
		temp.entryID = entryID;
		entryID++;
		master_log.resize(entryID);
		master_log.back() = temp;
	}
	order.resize(entryID);
	time.resize(entryID);
	std::sort(master_log.begin(), master_log.end(), Order());
	int master_size = static_cast<int> (master_log.size());
	category.reserve(master_size);
	for (int j = 0; j < master_size; ++j) {
		auto logs = master_log[j];
		//keeps track order of entryID
		order[logs.entryID] = j;
		//inputs time log
		time[j] = logs.time;
		//inputs category logs
		std::vector<std::string> listed;
		for (char &x : logs.category) {
			x = static_cast<char>(std::tolower(x));
		}
		category[logs.category].push_back(j);
		//inputs message log
		for (char &x : logs.message) {
			x = static_cast<char>(std::tolower(x));
		}
		int category_size = static_cast<int>(logs.category.size());
		for (int i = 0; i < category_size; ++i) {
			if (isalnum(logs.category[i])) {
				num = i;
				while (isalnum(logs.category[i])) {
					i++;
				}
				std::string word = logs.category.substr(num, i - num);
				if (find(listed.begin(), listed.end(), word) == listed.end()) {
					keyword[word].push_back(j);
					listed.push_back(word);
				}
			}
		}
		int message_size = static_cast<int>(logs.message.size());
		for (int i = 0; i < message_size; ++i) {
			if (isalnum(logs.message[i])) {
				num = i;
				while (isalnum(logs.message[i])) {
					i++;
				}
				std::string word = logs.message.substr(num, i - num);
				if (find(listed.begin(), listed.end(), word) == listed.end()) {
					keyword[word].push_back(j);
					listed.push_back(word);
				}
			}
		}
		listed.clear();
	}
	//std::sort(master_log.begin(), master_log.end(), intOrder());
	std::cout << entryID << " entries read" << '\n';
	ifs.close();
}

void ProcessArgs(int argc, char** argv, char &MODE) {
	const char* const short_opts = "hl:";
	struct option long_opts[] = {
		{ "help", optional_argument, nullptr, 'h' }
	};
	int opt = 0;
	int option_index = 0;
	while ((opt = getopt_long(argc, argv, short_opts, 
		long_opts, &option_index)) != -1) {
		switch (opt) {
		case 'm':
			if (optarg == "MST") {
				MODE = 'm';
			}
			else if (optarg == "OPTTSP") {
				MODE = 'o';
			}
			else if (optarg == "FASTTSP") {
				MODE = 'f';
			}
			else {
				std::cerr << "Bad Input" << std::endl;
				exit(1);
			}
			break;
		case 'h':
			std::cerr << "Send Help or something" << std::endl;
			exit(0);
		}
	}
}

//Finds all logs within two timestamp bounds
void time_search(std::vector<int> &temp_list,
	std::vector<uint64_t> &time, uint64_t time_1, uint64_t time_2) {
	temp_list.clear();
	int count = 0;
	auto start = time.begin();
	auto first = std::lower_bound(time.begin(), time.end(), time_1);
	auto last = std::upper_bound(time.begin(), time.end(), time_2);
	temp_list.reserve(temp_list.size() + first - start + 1);
	for (; first != last; first++) {
		temp_list.push_back(static_cast<int>(first - start));
		count++;
	}
	std::cout << "Timestamps search: " << count << " entries found" << '\n';	
}

//Finds all logs that match the timestamp bound
void time_search(std::vector<int> &temp_list,
	std::vector<uint64_t> &time, uint64_t time_1) {
	temp_list.clear();
	int count = 0;
	auto start = time.begin();
	auto first = std::lower_bound(time.begin(), time.end(), time_1);
	auto last = std::upper_bound(time.begin(), time.end(), time_1);
	temp_list.reserve(temp_list.size() + first - start + 1);
	for (; first != last; first++) {
		temp_list.push_back(static_cast<int>(first - start));
		count++;
	}
	std::cout << "Timestamp search: " << count << " entries found" << '\n';
}

//Finds all logs by category
void category_search(std::vector<int> &temp_list,
	std::unordered_map<std::string, std::vector<int>> &category,
	std::string category_var) {
	temp_list.clear();
	for (char &x : category_var) {
		x = static_cast<char>(std::tolower(x));
	}
	int count = 0;
	auto it = category.find(category_var);
	if (it != category.end()) {
		temp_list.reserve(temp_list.size() + category[category_var].size());
		for (int index : category[category_var]) {
			count++;
			temp_list.push_back(index);
		}
	}
	std::cout << "Category search: " << count << " entries found" << '\n';
}

//Finds all logs by keyword
void keyword_search(std::vector<int> &temp_list,
	std::unordered_map<std::string, std::vector<int>> &keyword,
	std::vector<std::string> &keywords, int& count) {
	temp_list.clear();
	//keeps track if value has already been counted
	//std::unordered_map<int, char> counted;
	for (std::string &word : keywords) {
		for (char &x : word) {
			x = static_cast<char>(std::tolower(x));
		}
	}
	std::vector<std::vector<int>> itr_list(2);
	auto itr = keyword.find(keywords[0]);
	if (itr != keyword.end()) {
		for (int val : keyword[keywords[0]]) {
			itr_list[0].push_back(val);
		}
	}
	//std::sort(itr_list[0].begin(), itr_list[0].end());
	int keyword_size = static_cast<int>(keywords.size());
	for (int i = 1; i < keyword_size; ++i) {
		itr_list[1].clear();
		auto itr = keyword.find(keywords[i]);
		if (itr != keyword.end()) {
			for (int val : keyword[keywords[i]]) {
				itr_list[1].push_back(val);
			}
		}
		//std::sort(itr_list[1].begin(), itr_list[1].end());
		auto it2 = std::set_intersection(itr_list[0].begin(), itr_list[0].end(),
			itr_list[1].begin(), itr_list[1].end(), itr_list[0].begin());
		itr_list[0].resize((unsigned int)(it2 - itr_list[0].begin()));
	}
	count = static_cast<int> (itr_list[0].size());
	for (int index : itr_list[0]) {
		temp_list.push_back(index);
	}
}

std::string ltotime(uint64_t time) {
	std::string temp = "";
	for (int i = 0; i < 4; ++i) {
		temp.insert(0, std::to_string(time % 10));
		time /= 10;
		temp.insert(0, std::to_string(time % 10));
		time /= 10;
		temp.insert(0, ":");
	}
	temp.insert(0, std::to_string(time % 10));
	time /= 10;
	temp.insert(0, std::to_string(time % 10));
	return temp;
}

void execute(std::vector<Logs> &master_log,
	std::vector<int> &temp_list,
	std::vector<int> &excerpt_list, char input,
	std::unordered_map<std::string, std::vector<int>> &category,
	std::unordered_map<std::string, std::vector<int>> &keyword,
	std::vector<uint64_t> &time, std::vector<int>& order,
	bool &quit, bool& checked) {
	std::vector<std::string> keywords;
	Logs temp;
	uint64_t time_1 = 0, time_2 = 0;
	std::string string_var = "";
	int string_size, order_size, excerpt_size;
	int count = 0;
	int num = 0;
	char delimiter = '|';
	int i = 0;
	switch (input) {
	case 't':
		std::getline(std::cin, string_var);
		string_var = string_var.substr(1, string_var.size() - 1);
		if (string_var.length() != 29) {
			std::cerr << "Error Time Input" << std::endl;
			break;
		}
		checked = true;
		while (i < 13) {
			time_1 = 100 * time_1
				+ 10 * (string_var[i] - '0')
				+ string_var[i + 1] - '0';
			i = i + 3;
		}
		string_var = string_var.substr(string_var.find(delimiter) + 1, 14);
		i = 0;
		while (i < 13) {
			time_2 = 100 * time_2
				+ 10 * (string_var[i] - '0')
				+ string_var[i + 1] - '0';
			i = i + 3;
		}
		if (time_1 <= time_2) {
			time_search(temp_list, time, time_1, time_2);
		}
		else {
			std::cerr << "Error Time Values" << std::endl;
		}
		break;
	case 'm':
		std::getline(std::cin, string_var);
		string_var = string_var.substr(1, string_var.size() - 1);
		checked = true;
		if (string_var.length() != 14) {
			std::cerr << "Error Time Input" << std::endl;
			std::cout << "Timestamp search: " << 0 << " entries found" << '\n';
			break;
		}
		while (i < 13) {
			time_1 = 100 * time_1
				+ 10 * (string_var[i] - '0')
				+ string_var[i + 1] - '0';
			i = i + 3;
		}
		time_search(temp_list, time, time_1);
		break;
	case 'c':
		std::getline(std::cin, string_var);
		string_var = string_var.substr(1, string_var.size() - 1);
		category_search(temp_list, category, string_var);
		checked = true;
		break;
	case 'k':
		count = 0;
		std::getline(std::cin, string_var);
		string_var = string_var.substr(1, string_var.size() - 1);
		string_size = static_cast<int>(string_var.size());
		for (int i = 0; i < string_size; ++i) {
			if (isalnum(string_var[i])) {
				num = i;
				while (isalnum(string_var[i])) {
					i++;
				}
				keywords.push_back(string_var.substr(num, i - num));
			}
		}
		keyword_search(temp_list, keyword, keywords, count);
		checked = true;
		std::cout << "Keyword search: " << count << " entries found" << '\n';
		break;
	case 'a':
		std::cin >> num;
		order_size = static_cast<int>(order.size());
		if (num < order_size && num >= 0) {
			excerpt_list.reserve(excerpt_list.size() + 1);
			excerpt_list.push_back(order[num]);
			std::cout << "log entry " << num << " appended" << '\n';
		}
		else {
			std::cerr << "Invalid Position" << std::endl;
		}
		break;
	case 'r':
		if (checked) {
			excerpt_list.reserve(excerpt_list.size() + temp_list.size());
			//std::sort(temp_list.begin(), temp_list.end(), Order());
			for (auto itr = temp_list.begin(); itr != temp_list.end(); ++itr) {
				excerpt_list.push_back(*itr);
			}
			std::cout << temp_list.size() << " log entries appended" << '\n';
		} 
		else {
			std::cerr << "No Previous Search" << std::endl;
		}
		break;
	case 'd':
		std::cin >> num;
		excerpt_size = static_cast<int>(excerpt_list.size());
		if (num < excerpt_size && num >= 0) {
			for (int i = num; i < (excerpt_size - 1); ++i) {
				excerpt_list[i] = excerpt_list[i + 1];
			}
			excerpt_list.pop_back();
			std::cout << "Deleted excerpt list entry " << num << '\n';
		} 
		else {
			std::cerr << "Invalid Position" << std::endl;
		}
		break;
	case 'b':
		std::cin >> num;
		excerpt_size = static_cast<int>(excerpt_list.size());
		if (num < excerpt_size && num >= 0) {
			for (int i = num; i > 0; --i) {
				std::swap(excerpt_list[i - 1], excerpt_list[i]);
			}
			std::cout << "Moved excerpt list entry " << num << '\n';
		}
		else {
			std::cerr << "Invalid Position" << std::endl;
		}
		break;
	case 'e':
		std::cin >> num;
		excerpt_size = static_cast<int>(excerpt_list.size());
		if (num < excerpt_size && num >= 0) {
			for (int i = num; i < (excerpt_size - 1); ++i) {
				std::swap(excerpt_list[i], excerpt_list[i + 1]);
			}
			std::cout << "Moved excerpt list entry " << num << '\n';
		}
		else {
			std::cerr << "Invalid Position" << std::endl;
		}
		break;
	case 's':
		if (!excerpt_list.empty()) {
			std::cout << "excerpt list sorted" << '\n';
			std::cout << "previous ordering:" << '\n';
			excerpt_size = static_cast<int> (excerpt_list.size());
			Logs temp = master_log[excerpt_list[0]];
			std::cout << 0 << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
			std::cout << "..." << '\n';
			temp = master_log[excerpt_list[excerpt_size - 1]];
			std::cout << excerpt_size - 1 << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
			std::sort(excerpt_list.begin(), excerpt_list.end());
			std::cout << "new ordering:" << '\n';
			temp = master_log[excerpt_list[0]];
			std::cout << 0 << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
			std::cout << "..." << '\n'; 
			temp = master_log[excerpt_list[excerpt_size - 1]];
			std::cout << excerpt_size - 1 << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
		}
		else {
			std::cout << "excerpt list sorted" << '\n'
				<< "(previously empty)" << '\n';
		}
		break;
	case 'l':
		if (!excerpt_list.empty()) {
			excerpt_size = static_cast<int> (excerpt_list.size());
			std::cout << "excerpt list cleared" << '\n'
				<< "previous contents:" << '\n';
			Logs temp = master_log[excerpt_list[0]];
			std::cout << 0 << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
			std::cout << "..." << '\n';
			temp = master_log[excerpt_list[excerpt_size - 1]];
			std::cout << excerpt_size - 1 << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
			excerpt_list.clear();
		}
		else {
			std::cout << "excerpt list cleared" << '\n'
				<< "(previously empty)" << '\n';
		}
		break;
	case 'g':
		if (!temp_list.empty()) {
			std::sort(temp_list.begin(), temp_list.end());
			for (int i = 0; i < static_cast<int>(temp_list.size()); ++i) {
				Logs temp = master_log[temp_list[i]];
				std::cout << temp.entryID << "|"
					<< ltotime(temp.time) << "|"
					<< temp.category << "|"
					<< temp.message << '\n';
			}
		}
		else {
			std::cerr << "No Previous Search" << std::endl;
		}
		break;
	case 'p':
		excerpt_size = static_cast<int>(excerpt_list.size());
		for (int i = 0; i < excerpt_size; ++i) {
			Logs temp = master_log[excerpt_list[i]];
			std::cout << i << "|" << temp.entryID << "|"
				<< ltotime(temp.time) << "|"
				<< temp.category << "|"
				<< temp.message << '\n';
		}
		break;
	case 'q':
		quit = true;
		break;
	case '#':
		std::getline(std::cin, string_var);
		break;
	default:
		std::getline(std::cin, string_var);
		std::cerr << "Error Command" << std::endl;
		break;
	}
}

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);
	std::vector<Logs> master_log;
	std::unordered_map<std::string, std::vector<int>> category;
	std::unordered_map<std::string, std::vector<int>> keyword;
	std::vector<uint64_t> time;
	std::vector<int> temp_list;
	std::vector<int> excerpt_list;
	std::vector<int> order;
	std::string filename = "";
	bool quit = false;
	bool checked = false;
	ProcessArgs(argc, argv);
	filename = argv[1];
	InputData(master_log, category, keyword, time, order, filename);
	char command = '\0';
	while (command != 'q') {
		std::cout << "% ";
		std::cin >> command;
		execute(master_log, temp_list, excerpt_list, command, category,
			keyword, time, order, quit, checked);
		if (quit) {
			return 0;
		}
	}
	return 0;
}