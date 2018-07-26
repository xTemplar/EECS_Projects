#include <iostream> //ostream
#include <vector>
#include <string>
#include <cstddef> 
#include <deque>
#include <getopt.h>
#include <algorithm>
using namespace std;

void insert(string& n, vector<string>& dict, int& itr, int& num);
void reversal(string& n, vector<string>& dict, int& num);
void duble(string& n, vector<string>& dict, int& itr, int& num);
void swap(string& n, vector<string>& dict, int& itr, int& num);
bool close_check(string& n, string& m, bool& l, bool& p, bool& c);
string first_diff(string& n, string& m, int type);
void ProcessArgs(int argc, char** argv, bool& type,
	bool& length, bool& swap, bool& change,
	bool& o, string& start, string& end);
int type_change(string& m, string& n);

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);
	//keeps track of what route to take. Queue is 0, Stack is 1
	//for output, 1 = word, 0 = modification
	bool qs = 0, change = 0, p = 0, length = 0, o = 1;
	string start = "", end = "";
	ProcessArgs(argc, argv, qs, length, p, change, o, start, end);
	//size keeps track of how many non-comment lines
	//first is index of starting word, last is index of last word
	int size, last = -1, first = -1; 
	//counts number of words in dict THEN counts number of words in trackback
	int words_checked = 0;
	char type;
	string word;
	//checks if end and begin are specified
	if (start == "" || end == "") {
		cerr << "Begin or End not specified" << endl;
		exit(1);
	}
	cin >> type;
	cin >> size;
	vector<string> Dict;
	Dict.reserve(size);
	cin.clear();
	if (type == 'S') {
		while (words_checked < size) {
			getline(cin, word);
			if ((word != "") && (word[0] != '/')) {
				if (!length) {
					if (word.length() == start.length()) {
						Dict.push_back(word);
					}
					words_checked++;
				}
				else {
					Dict.push_back(word);
					words_checked++;
				}
			} 
		}
	}
	else if (type == 'C') {
		int i = 0;
		while (i < size) {
			getline(cin, word);
			if ((word != "") && (word[0] != '/')) {
				int found = static_cast<int> (word.find_first_of("![&?"));
				if (found != static_cast<int> (string::npos)) {
					if (word[found] == '[') {
						insert(word, Dict, found, words_checked);
					}
					else if (word[found] == '&') {
						reversal(word, Dict, words_checked);
					}
					else if (word[found] == '?') {
						duble(word, Dict, found, words_checked);
					}
					else if (word[found] == '!') {
						swap(word, Dict, found, words_checked);
					}
				}
				else {
					Dict.push_back(word);
					words_checked++;
				}
				i++;
			}
		}
	}
	bool beg_dict = false, end_dict = false;
	for (int i = 0; i < words_checked; i++) {
		if (Dict[i] == start) {
			beg_dict = true;
			first = i;
		} 
		if (Dict[i] == end) {
			end_dict = true;
			last = i;
		}
	}
	//check if begin and start are in dictionary
	if (!(beg_dict && end_dict)) {
		cerr << "Begin and End words do not belong to Dictionary" << endl;
		exit(1);
	}
	//check if length is enabled for differing lengths end and begin
	if (!length) {
		if (start.length() != end.length()) {
			cerr << "Length not enabled for differing lengths" << endl;
			exit(1);
		}
	}
	//implementation of the route
	deque<int> buffer;
	buffer.push_back(first);
	//keeps track of index of parent word for backtracking
	vector<int> index(Dict.size(), -1); 
	index[first] = last;
	int prev = first; //keeps track of last word in route
	words_checked = 1;
	while (!buffer.empty() && (start.compare(end) != 0)) {
		if (qs == 1) {
			buffer.pop_back();
		}
		else {
			buffer.pop_front();
		}
		for (int i = 0; i < static_cast<int>(Dict.size()); i++) {
			if ((index[i] == -1)
				&& close_check(start, Dict[i], length, p, change) != 0) {
				buffer.push_back(i);
				words_checked++;
				index[i] = prev;
				if (Dict[i] == end) {
					start = Dict[i];
					break;
				}
			}
		}
		if (!buffer.empty()) {
			if (start == end) {
				break;
			}
			if (qs == 1) {
				prev = buffer.back();
				start = Dict[prev];
			}
			else {
				prev = buffer.front();
				start = Dict[prev];
			}
		} 
		else {
			cout << "No solution, " + to_string(words_checked)
				+ " words checked." + '\n';
			return 0;
		}
	}
	int route_length = 0;
	buffer.clear();
	prev = first;
	while ((index[prev]) != first) {
		buffer.push_front(index[prev]);
		prev = index[prev];
		route_length++;
	}
	buffer.push_front(first);
	route_length++;
	if (o) {
		cout << "Words in morph: " + to_string(route_length) + '\n';
		for (int i = 0; i < route_length; i++) {
			cout << Dict[buffer.front()] << '\n';
			buffer.pop_front();
		}
	}
	else {
		cout << "Words in morph: " + to_string(route_length) + '\n';
		string w_1, w_2;
		cout << Dict[first] << '\n';
		for (int i = 0; i < route_length - 1; i++) {
			w_1 = Dict[buffer[i]];
			w_2 = Dict[buffer[i + 1]];
			int mod = type_change(w_1, w_2);
			if (mod == 1) {
				cout << "c," << first_diff(w_1, w_2, 1) << '\n';
			}
			else if (mod == 2) {
				cout << "s," << first_diff(w_1, w_2, 2) << "\n";
			}
			else if (mod == 3) {
				cout << "i," << first_diff(w_1, w_2, 3) << '\n';
			}
			else if (mod == 4) {
				cout << "d," << first_diff(w_1, w_2, 4) << "\n";
			}
		}
	}
	return 0;
}


//unravels reversal operation
void reversal(string& n, vector<string>& dict, int& num) {
	int size = static_cast<int> (n.size());
	dict.push_back(n.substr(0, size - 1));
	num++;
	for (int i = 0; i < (size / 2); i++) {
		swap(n[i], n[size - i - 2]);
	}
	dict.push_back(n.substr(0, size - 1));
	num++;
}	

//unravels insert-each operation
void insert(string& n, vector<string>& dict, int& init_pos, int& num) {
	//find end of bracket
	int end_pos = static_cast<int> (n.find_first_of("]"));  
	//keeps track of characters in bracket
	string str = n.substr(init_pos + 1, end_pos - init_pos - 1); 
	string prefix;
	if (init_pos != 0) {
		prefix = n.substr(0, init_pos);
	}
	string postfix = "";
	if (end_pos != static_cast<int> (n.size())) {
		postfix = n.substr(end_pos + 1, n.size() - end_pos - 1);
	}
	if (!str.empty()) {
		for (int i = 0; i < (end_pos - init_pos - 1); i++) {
			dict.push_back(prefix + str[i] + postfix);
			num++;
		}
	}
}

//unravels swap operation
void swap(string& n, vector<string>& dict, int& pos, int& num) {
	string str = n.substr(pos - 2, 2);
	string prefix = n.substr(0, pos - 2);
	string postfix = n.substr(pos + 1, n.size() - pos - 1);
	dict.push_back(prefix + str + postfix);
	dict.push_back(prefix + str[1] + str[0] + postfix);
	num = num + 2;
}

//unravels double operation
void duble(string& n, vector<string>& dict, int& pos, int& num) {
	string repeat = n.substr(pos - 1, 1);
	int size = static_cast<int> (n.size());
	dict.push_back(n.substr(0, pos - 1) + repeat
		+ n.substr(pos + 1, size - pos - 1));
	dict.push_back(n.substr(0, pos - 1) + repeat + repeat
		+ n.substr(pos + 1, size - pos - 1));
	num = num + 2;
}

//checks if two words are close to one another
bool close_check(string& n, string& m, bool& length, 
						   bool& p, bool& change) {
	int len_n = static_cast<int> (n.length());
	int len_m = static_cast<int> (m.length());
	//if same length then it is swap or or change
	if ((len_n - len_m == 0) && (p || change)){
		int diff_count = 0;
		for (int i = 0; i < len_n; i++) {
			if (n[i] != m[i]) {
				diff_count++;
			}
		}
		//if difference is exactly 1 then it is a change 
		if ((diff_count == 1) && change){
			return true;
		}
		//if difference is exactly 2 check if the sum of char vals is same
		else if ((diff_count == 2) && p) {
			for (int i = 0; i < len_n - 1; i++) {
				if (n[i] != m[i]) {
					//checks if letters are adjacent
					if ((n[i] == m[i + 1]) && (m[i] == n[i + 1])) {
						return true;
					}
					else {
						return false;
					}
				}
			}
			return false;
		}
		//if there are more than 2 different they're not close
		else {
			return false;
		}
	} 
	else if (abs(len_n - len_m) == 1 && length) {
		//checks if it is either insert or delete
		int i = 0;
		while (i < min(len_n, len_m) && n[i] == m[i]) {
			i++;
		}
		if (len_n < len_m) {
			while (i < len_n && n[i] == m[i + 1]) {
				i++;
			}
		}
		else {
			while (i < len_m && n[i + 1] == m[i]) {
				i++;
			}
		}
		if (i == min(len_n, len_m)) {
			return true;
		}
	}
	return false;
}

//Returns type of change: Change (1), Swap (2), Insert (3), Delete (4)
string first_diff(string& n, string& m, int type) {
	int n_len = static_cast<int> (n.size());
	if (type == 1) {
		for (int i = 0; i < n_len; i++) {
			if (n[i] != m[i]) {
				return to_string(i) + "," + m[i];
			}
		}
	}
	else if (type == 2) {
		for (int i = 0; i < n_len; i++) {
			if (n[i] != m[i]) {
				return "" + to_string(i);
			}
		}
	}
	else if (type == 3) {
		for (int i = 0; i < n_len; i++) {
			if (n[i] != m[i]) {
				return to_string(i) + "," + m[i];
			} 
		}
		return to_string(n_len) + "," + m[n_len];
	}
	else if (type == 4) {
		for (int i = 0; i < n_len - 1; i++) {
			if (n[i] != m[i]) {
				return "" + to_string(i);
			}
		}
		return "" + to_string(n_len - 1);
	}
	return "";
}

void ProcessArgs(int argc, char** argv, bool& qs,
	bool& length, bool& swap, bool& change,
	bool& o, string& start, string& end) {
	const char* const short_opts = "sqcplo:b:e:h";
	struct option long_opts[] = {
		{ "stack", no_argument, nullptr, 's' },
		{ "queue", no_argument, nullptr, 'q' },
		{ "change", no_argument, nullptr, 'c' },
		{ "swap", no_argument, nullptr, 'p' },
		{ "length", no_argument, nullptr, 'l' },
		{ "output", required_argument, nullptr, 'o' },
		{ "begin", required_argument, nullptr, 'b' },
		{ "end", required_argument, nullptr, 'e' },
		{ "help", no_argument, nullptr, 'h' },
		{ nullptr, 0, nullptr, '\0' }
	};
	int opt = 0;
	string output;
	int option_index = 0;
	int num_qs = 0;
	while ((opt = getopt_long(argc, argv, short_opts,
		long_opts, &option_index)) != -1) {
		switch (opt)
		{
		case 's':
			qs = true;
			num_qs++;
			break;
		case 'q':
			qs = false;
			num_qs++;
			break;
		case 'c':
			change = true;
			break;
		case 'p':
			swap = true;
			break;
		case 'l':
			length = true;
			break;
		case 'o':
			output = optarg;
			if (output == "W" || output == "w") {
				o = true;
			}
			else if (output == "M" || output == "m") {
				o = false;
			}
			else {
				cerr << "-o flag is followed by an invalid character" << endl;
				exit(1);
			}
			break;
		case 'b':
			output = optarg;
			if (output != "") {
				start = output;
			}
			else {
				cerr << "invalid input" << endl;
				exit(1);
			}
			break;
		case 'e':
			output = optarg;
			if (output != "") {
				end = output;
			}
			else {
				cerr << "invalid input" << endl;
				exit(1);
			}
			break;
		case 'h':
			cerr << "send help plz" << endl;
			exit(0);
		}
	}
	if (num_qs != 1) {
		cerr << "More or less than one -s or -q on commandline" << endl;
		exit(1);
	}
	if (!(change || swap || length)) {
		cerr << "-c, -l, -p flag not specified on commandline" << endl;
		exit(1);
	}
}

//Returns type of change: Change (1), Swap (2), Insert (3), Delete (4)
int type_change(string& m, string& n) {
	int m_len = static_cast<int>(m.length());
	int n_len = static_cast<int>(n.length());
	if (m_len == n_len) {
		for (int i = 0; i < m_len; i++) {
			if (m[i] != n[i]) {
				if (i != m_len - 1) {
					if (m[i] == n[i + 1] && m[i + 1] == n[i]) {
						return 2;
					}
					return 1;
				} 
				else {
					return 1;
				}
			}
		}
		return -1;
	}
	else {
		if (m_len > n_len) {
			return 4;
		}
		return 3;
	}
}