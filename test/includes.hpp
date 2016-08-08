#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <cassert>
#include <climits>
#include <csetjmp>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <tuple>
#include <unordered_set>

using namespace std;

constexpr int Width = 60;
#define DBG(x) { cout << left << setw(Width) << #x << boolalpha << (x) << endl; }
#define DBG_CONT(x) { cout << left << setw(Width) << #x; for(auto const& _name : (x)) \
	cout << boolalpha << _name << " "; cout << endl; }

#define BARK cout << __PRETTY_FUNCTION__ << endl;

#endif // INCLUDES_HPP
