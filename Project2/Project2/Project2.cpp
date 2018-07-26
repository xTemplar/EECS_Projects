#include <iostream> //ostream
#include <vector>
#include <string>
#include <queue>
#include <cstddef> 
#include <cstdlib>
#include <deque>
#include <getopt.h>
#include <algorithm>
#include "P2random.h"
#include <iomanip>
#include <functional>

//functor that compares dereferenced pointers, *a less than *b
struct Comp {
	bool operator() (const std::pair<int, std::pair<int, int>> &a,
		const std::pair<int, std::pair<int, int>> &b) const {
		if (a.first != b.first) {
			return (a.first > b.first);
		}
		else {
			if (a.second.second != b.second.second) {
				return a.second.second > b.second.second;
			}
			else {
				return a.second.first > b.second.first;
			}
		}
	}
};

struct mine_details {
public: 
	std::pair<int, std::pair<int, int>> mine;
	bool explosion;
};

struct Comp_Mine_Details {
	bool operator() (const mine_details &a, const mine_details &b) {
		if (a.mine.first != b.mine.first) {
			return (a.mine.first > b.mine.first);
		} 
		else {
			if (a.mine.second.second != b.mine.second.second) {
				return a.mine.second.second > b.mine.second.second;
			}
			else {
				return a.mine.second.first > b.mine.second.first;
			};
		}
	} 
};

class Miner {
public:
	Miner(bool f, int s) {
		rub_clear = 0;
		tile_clear = 0;
		start_x = 0; 
		start_y = 0;
		median = 1000;
		format = f;
		size = s;
		arr.resize(s);
		seed = 0;
		max_rubble = 0;
		num_tnt = 0;
		s_num = 0;
		for (int i = 0; i < size; i++) {
			arr[i].resize(size);
		}
	}
	void set_s_num(int n) {
		s_num = n;
	}
	//adds to total rubble cleared
	void add_r_clear(int r_clear) {
		rub_clear += r_clear;
	}
	//adds one to tiles cleared
	void add_t_clear() {
		tile_clear += 1;
	}
	int get_size() {
		return size;
	}
	bool get_form() {
		return format;
	}
	void set_start() {
		std::string fill;
		std::cin >> fill >> start_x >> start_y;
		if (start_x >= size || start_x < 0) {
			std::cerr << "error start position" << std::endl;
			exit(1);
		}
		if (start_y >= size || start_y < 0) {
			std::cerr << "error start position" << std::endl;
			exit(1);
		}
	}
	int get_start_x() {
		return start_x;
	}
	int get_start_y() {
		return start_y;
	}
	void set_seed() {
		std::string fill;
		std::cin >> fill >> seed;
	}
	int get_seed() {
		return seed;
	}
	bool is_tnt_empty() {
		return tnt.empty();
	}
	void set_Max_Rubble() {
		std::string fill;
		std::cin >> fill >> max_rubble;
	}
	int get_rubble() {
		return max_rubble;
	}
	void set_TNT() {
		std::string fill;
		std::cin >> fill >> num_tnt;
	}
	int get_TNT() {
		return num_tnt;
	}
	//print summary of escape
	void print_escape(bool& m, bool& s, bool&v) {
		if (m || v) {
			//keeps track of parity of num of rubble:
			//false is even, true is odd
			bool odd = false;
			bool first_explo = true;
			std::priority_queue<int, std::vector<int>, std::less<int>> l;
			std::priority_queue<int, std::vector<int>, std::greater<int>> g;
			while (!spots.empty()) {
				if (v) {
					print_verbose(first_explo);
				}
 				buffer.push_front(spots.front());
				if (spots.front().mine.first != -1) {
					if (m) {
						print_median(odd, l, g, median);
					}
				}
				spots.pop_front();
			}
			//restocks the spots deque
			while (!buffer.empty()) {
				spots.push_front(buffer.front());
				buffer.pop_front();
			}
		}
		std::cout << "Cleared " << tile_clear << " tiles containing "
			<< rub_clear << " rubble and escaped." << '\n';
		if (s) {
			std::cout << "First tiles cleared:" << '\n';
			s_num = std::min(static_cast<int> (spots.size()), s_num);
			int n = s_num;
			std::priority_queue<mine_details,
				std::vector<mine_details>, Comp_Mine_Details> tiles;
			while (!spots.empty()) {
				if (n > 0) {
					mine_details spot = spots.front();
					if (spot.mine.first == -1) {
						std::cout << "TNT at [" << spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					else {
						std::cout << spot.mine.first << " at ["
							<< spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					n--;
				}
				tiles.push(spots.front());
				buffer.push_front(spots.front());
				spots.pop_front();
			}
			std::cout << "Last tiles cleared:" << '\n';
			n = s_num;
			while (!buffer.empty()) {
				if (n > 0) {
					mine_details spot = buffer.front();
					if (spot.mine.first == -1) {
						std::cout << "TNT at [" << spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					else {
						std::cout << spot.mine.first << " at ["
							<< spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					n--;
				}
				buffer.pop_front();
			}
			std::cout << "Easiest tiles cleared:" << '\n';
			n = s_num;
			while (!tiles.empty()) {
				if (n > 0) {
					mine_details spot = tiles.top();
					if (spot.mine.first == -1) {
						std::cout << "TNT at [" << spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					else {
						std::cout << spot.mine.first << " at ["
							<< spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					n--;
				}
				buffer.push_back(tiles.top());
				tiles.pop();
			}
			std::cout << "Hardest tiles cleared:" << '\n';
			n = s_num;
			while (!buffer.empty()) {
				if (n > 0) {
					mine_details spot = buffer.back();
					if (spot.mine.first == -1) {
						std::cout << "TNT at [" << spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					else {
						std::cout << spot.mine.first << " at ["
							<< spot.mine.second.first
							<< "," << spot.mine.second.second << "]" << '\n';
					}
					n--;
				}
				buffer.pop_back();
			}
		}
	}
	void print_verbose(bool& first_explo) {
		mine_details spot = spots.front();
		if (spot.explosion) {
			if (spot.mine.first == -1) {
				if (first_explo) {
					std::cout << "TNT explosion started at ["
						<< spot.mine.second.first << ","
						<< spot.mine.second.second << "]!" << '\n';
					first_explo = false;
				}
			}
			else {
				std::cout << "Cleared by TNT: " << spot.mine.first << " at ["
					<< spot.mine.second.first << ","
					<< spot.mine.second.second << "]" << '\n';
				first_explo = true;
			}
		}
		else {
			first_explo = true;
			std::cout << "Cleared: " << spot.mine.first << " at ["
				<< spot.mine.second.first << ","
				<< spot.mine.second.second << "]" << '\n';
		}
	}
	void print_median(bool& o, 
		std::priority_queue<int, std::vector<int>, std::less<int>> &l,
		std::priority_queue<int, std::vector<int>, std::greater<int>> &g, 
		double& median) {
		int x_1 = spots.front().mine.first;
		if (x_1 < median) {
			l.push(x_1);
		}
		else {
			g.push(x_1);			
		}
		//if we start odd, then we become even so g and l equal in size
		if (o) {
			if (g.size() > l.size()) {
				l.push(g.top());
				g.pop();
			}
			else if (l.size() > g.size()) {
				g.push(l.top());
				l.pop();
			}
			median = static_cast<double> (g.top() + l.top()) / 2.0;
		} //if we start even, we have +1 in g or l. This excess is median
		else {
			if (g.size() == l.size() + 1) {
				median = g.top();
			}
			else if (l.size() == g.size() + 1) {
				median = l.top();
			}
			else {
				std::cerr << "what the hell happened" << std::endl;
				exit(1);
			}
		}
		o = !o;
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Median difficulty of clearing rubble is: "
			<< median << '\n';
	}
	//updates spots kept track of 
	void add_spots(std::pair<int, std::pair<int, int>> temp) {
		mine_details spot;
		spot.explosion = false;
		spot.mine = temp;
		spots.push_back(spot);
	}
	//adds tnt to spots
	void add_tnt(std::pair<int, std::pair<int, int>> temp) {
		mine_details spot;
		spot.explosion = true;
		spot.mine = temp;
		spots.push_back(spot);
	}
	//changes tile value in mine at spot in array
	void update_mine(int x, int y, int val) {
		arr[x][y] = val;
	}
	//creates custom mine depending on input
	void fill_mine() {
		int val;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				std::cin >> val;
				arr[i][j] = val;
			}
		}
	}
	//creates randomized mine depending on input
	void fill_mine(std::stringstream& ss) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				ss >> arr[i][j];
			}
		}
	}
	//returns value of rubble in mine
	int get_mine_data(int x, int y) {
		return arr[x][y];
	}
	//checks if we have exited mine
	bool check_exit(int x, int y) {
		if (x == (size - 1) || x == 0) {
			if ((arr[x][y] == 0) || (arr[x][y] == 1000)) {
				return true;
			}
			return false;
		}
		if (y == (size - 1) || y == 0) {
			if ((arr[x][y] == 0) || (arr[x][y] == 1000)) {
				return true;
			}
			return false;
		}
		return false;
	}
	//detonates TNT at location
	void detonate_tnt(int x, int y) {
		std::pair<int, std::pair<int, int>> temp;
		arr[x][y] = 0;
		if ((y - 1) >= 0) {
			temp.second.first = x;
			temp.second.second = y - 1;
			if (arr[x][y - 1] == -1) {
				temp.first = -1;
				tnt.push(temp);
				arr[x][y - 1] = -2;
			}
			else if (arr[x][y - 1] > 0 && arr[x][y - 1] < 1000) {
				temp.first = arr[x][y - 1];
				arr[x][y - 1] = 0;
				tnt.push(temp);
			}
			//attempt
			else if (arr[x][y - 1] == 0){
				pqMax.push(temp);
			}
		}
		if ((x - 1) >= 0) {
			temp.second.first = x - 1;
			temp.second.second = y;
			if (arr[x - 1][y] == -1) {
				temp.first = -1;
				tnt.push(temp);
				arr[x - 1][y] = -2;
			}
			else if (arr[x - 1][y] > 0 && arr[x - 1][y] < 1000) {
				temp.first = arr[x - 1][y];
				arr[x - 1][y] = 0;
				tnt.push(temp);
			}
			//attempt
			else if (arr[x - 1][y] == 0) {
				pqMax.push(temp);
			}
		}
		if ((x + 1) < size) {
			temp.second.first = x + 1;
			temp.second.second = y;
			if (arr[x + 1][y] == -1) {
				temp.first = -1;
				tnt.push(temp);
				arr[x + 1][y] = -2;
			}
			else if (arr[x + 1][y] > 0 && arr[x + 1][y] < 1000) {
				temp.first = arr[x + 1][y];
				tnt.push(temp);
				arr[x + 1][y] = 0;
			}
			//attempt
			else if (arr[x + 1][y] == 0) {
				pqMax.push(temp);
			}
		}
		if ((y + 1) < size) {
			temp.second.first = x;
			temp.second.second = y + 1;
			if (arr[x][y + 1] == -1) {
				temp.first = -1;
				tnt.push(temp);
				arr[x][y + 1] = -2;
			}
			else if (arr[x][y + 1] > 0 && arr[x][y + 1] < 1000) {
				temp.first = arr[x][y + 1];
				tnt.push(temp);
				arr[x][y + 1] = 0;
			}
			//attempt
			else if (arr[x][y + 1] == 0) {
				pqMax.push(temp);
			}
		}
	}
	void add_pqMax(std::pair<int, std::pair<int, int>> temp) {
		pqMax.push(temp);
	}

	//adds to priority queue visible tiles
	void add_seen_tiles(int x, int y) {
		std::pair<int, std::pair<int, int>> temp;
		arr[x][y] = 1000;
		if (y - 1 >= 0) {
			if (arr[x][y - 1] >= -1 && arr[x][y - 1] < 1000) {
			temp.first = arr[x][y - 1];
			temp.second.first = x;
			temp.second.second = y - 1;
			pqMax.push(temp);
			}
		}
		if (x - 1 >= 0) {
			if (arr[x - 1][y] >= -1 && arr[x - 1][y] < 1000) {
				temp.first = arr[x - 1][y];
				temp.second.first = x - 1;
				temp.second.second = y;
				pqMax.push(temp);
			}
		}
		if (x + 1 < size) {
			if (arr[x + 1][y] >= -1 && arr[x + 1][y] < 1000) {
				temp.first = arr[x + 1][y];
				temp.second.first = x + 1;
				temp.second.second = y;
				pqMax.push(temp);
			}
		}
		if (y + 1 < size) {
			if (arr[x][y + 1] >= -1 && arr[x][y + 1] < 1000) {
				temp.first = arr[x][y + 1];
				temp.second.first = x;
				temp.second.second = y + 1;
				pqMax.push(temp);
			}
		}
	}
	//retrieves highest priority tile
	std::pair<int, std::pair<int, int>> get_maxpq() {	
		std::pair<int, std::pair<int, int>> next = pqMax.top();
		pqMax.pop();
		return next;
	}
	std::pair<int, std::pair<int, int>> get_nexttnt() {
		std::pair<int, std::pair<int, int>> next = tnt.top();
		tnt.pop();
		return next;
	}
private:
	//keeps track of map format: 1 for 'M', 0 for 'R'
	bool format;
	//keeps track of size of mine
	//keeps track of start location
	int start_x, start_y;
	//keeps track of random seed
	int seed;
	//keeps track of max rubble
	int max_rubble;
	//keeps track of num of tnt
	int num_tnt;
	int size;
	//keeps track rubbled cleared (TNT is 0 rubble)
	int rub_clear;
	//keeps track of tiles clear (TNT is 0 tiles)
	int tile_clear;
	//keeps track N for Stats output
	int s_num;
	//keeps track of current median
	double median;
	//keeps track of mine grid
	std::vector<std::vector<int>> arr;
	//priority queue for escape
	std::priority_queue<std::pair<int, std::pair<int, int>>,
		std::vector<std::pair<int, std::pair<int, int>>>, Comp> pqMax;
	//priority queue for tnt
	std::priority_queue<std::pair<int, std::pair<int, int>>,
		std::vector<std::pair<int, std::pair<int, int>>>, Comp> tnt;
	//keeps track of location of where rubble is removed and how much
	//in the order
	//location is saved as pair (x,y) 
	std::deque<mine_details> spots;
	//a buffer deque for printing stats to restock spots
	std::deque<mine_details> buffer;
};

void iterate_mine(int& x, int& y, Miner& me);

void ProcessArgs(int argc, char** argv,	bool& s, 
	bool& m, bool& v, int& s_num) {
	const char* const short_opts = "hs:mv";
	struct option long_opts[] = {
		{ "help", optional_argument, nullptr, 'h' },
		{ "stats", required_argument, nullptr, 's' },
		{ "median", optional_argument, nullptr, 'm' },
		{ "verbose", optional_argument, nullptr, 'v' }
	};
	int opt = 0;
	int option_index = 0;
	while ((opt = getopt_long(argc, argv, short_opts,
		long_opts, &option_index)) != -1) {
		switch (opt)
		{
		case 'h':
			std::cout << "SEND HELP PLZ" << std::endl;
			exit(0);
		case 's':
			s = true;
			s_num = atoi(optarg);
			break;
		case 'm':
			m = true;
			break;
		case 'v':
			v = true;
			break;
		}
	}
}

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);
	//keeps track of flags for output
	bool s = false, m = false, v = false;
	//keeps track of N tiles for S output
	int s_num;
	ProcessArgs(argc, argv, s, m, v, s_num);
	//keeps track of format;
	bool format;
	char f;
	std::cin >> f;
	if (f == 'M') {
		format = 1;
	}
	else if (f == 'R') {
		format = 0;
	}
	else {
		std::cerr << "error format" << std::endl;
		exit(1);
	}
	//keeps track of size;
	int size;
	std::string fill;
	std::cin >> fill >> size;
	Miner me(format, size);
	me.set_start();
	me.set_s_num(s_num);
	if (!me.get_form()) {
		std::stringstream ss;
		me.set_seed();
		me.set_Max_Rubble();
		me.set_TNT();
		P2random::PR_init(ss, me.get_size(), me.get_seed(),
			me.get_rubble(), me.get_TNT());
		me.fill_mine(ss);
	}
	else {
		me.fill_mine();
	}
	int x = me.get_start_x();
	int y = me.get_start_y();
	while (!(me.check_exit(x,y))) { //checks if we have exited
		iterate_mine(x, y, me);
		std::pair<int, std::pair<int, int>> temp; 
		//chain reaction loop
		while (!(me.is_tnt_empty())) { //checks if tnt pq empty
			temp = me.get_nexttnt();  //detonates max priority tnt 
			if (temp.first == -1) {
				me.detonate_tnt(temp.second.first, temp.second.second);
				me.add_tnt(temp);
			}
			else {
				me.add_tnt(temp);
				me.add_r_clear(temp.first);
				me.add_t_clear();
				me.update_mine(temp.second.first, temp.second.second, 0);
				//adds zeros to the priority queue to replace exploded spots
				temp.first = 0;
				me.add_pqMax(temp);
			}
		}
	}
	me.print_escape(m, s, v);
	return 0;
}

void iterate_mine(int& x, int& y, Miner& me) {
	std::pair<int, std::pair<int, int>> temp;
	int val = me.get_mine_data(x,y);
	if (val > 0 && val <= 999) {
		temp.first = val;
		temp.second.first = x;
		temp.second.second = y;
		me.add_spots(temp);
		me.add_r_clear(val);
		me.add_t_clear();
		//clears tile we're on - we set this to 1000 to show we've been here
		me.update_mine(x, y, 1000);
		me.add_seen_tiles(x, y);
	}
	else if (val == -1) {
		temp.first = -1;
		temp.second.first = x;
		temp.second.second = y;
		me.add_tnt(temp);
		me.detonate_tnt(x, y);
	}
	else {
		if (val == 0) {
			me.add_seen_tiles(x, y);
			//clears tile we're on - we set this to 1000 to show we've been here
			me.update_mine(x, y, 1000);
		}
		if (val == -2) {
			me.update_mine(x, y, 1000);
		}
		temp = me.get_maxpq();
		//moves to the max priority location
		x = temp.second.first;
		y = temp.second.second;
	}
}