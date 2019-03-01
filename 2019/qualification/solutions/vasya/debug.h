#define CONCATENATE_DETAIL(x,y) x##y
#define CONCATENATE(x,y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __COUNTER__)

#define show(...)                \
if (!SHUT_DEBUG) {               \
	SHUT_DEBUG = 1;              \
	PRINT_LINE_NUMBER(__LINE__); \
	PRINT_DEBUG_PREFIX();        \
	FORMAT(__VA_ARGS__);         \
	NEWLINE();                   \
	SHUT_DEBUG = 0;              \
}

#define debug(...)               \
if (!SHUT_DEBUG) {               \
	SHUT_DEBUG = 1;              \
	PRINT_LINE_NUMBER(__LINE__); \
	PRINT_DEBUG_PREFIX();        \
	PRINT_ARGS(#__VA_ARGS__);    \
	OUTPUT(__VA_ARGS__);         \
	NEWLINE();                   \
	SHUT_DEBUG = 0;              \
}

#define fbegin(...) auto __PRINT_RES = __VA_ARGS__; auto __RESULT = [&] () {
#define fend }(); __PRINT_RES(__RESULT); return __RESULT;

#define pbegin(...) auto __PRINT_RES = __VA_ARGS__; [&] () {
#define pend }(); __PRINT_RES();

#define deepen(...)                                            \
unique_ptr<RECURSION_LEVEL> MAKE_UNIQUE(THIS_RECURSION_LEVEL)( \
	SHUT_DEBUG ? 0 : new RECURSION_LEVEL(__VA_ARGS__)          \
)

#define timer(...)                                     \
unique_ptr<TIMESTAMPER> MAKE_UNIQUE(THIS_TIMESTAMPER)( \
	SHUT_DEBUG ? 0 : new TIMESTAMPER(__LINE__)         \
)

#define problemname EXTRACT_NAME(__FILE__)

#ifndef MAX_PREFIX_SIZE
#define MAX_PREFIX_SIZE 50
#endif

#ifndef LINE_DIGITS_CNT
#define LINE_DIGITS_CNT 4
#endif

struct outputable {};
template<typename T> struct outputer;

template<typename It> struct _INTERVAL {
	It __begin, __end;
	_INTERVAL (It __begin_, It __end_) : __begin(__begin_), __end(__end_) {}
};

template<typename It> _INTERVAL<It> INTERVAL (It __begin, It __end) {
	return _INTERVAL<It>(__begin, __end);
}

bool SHUT_DEBUG = 0;
string DEBUG_PREFIX;

template<typename T> string TO_STRING (const T &__a) {
	stringstream __out;
	__out << __a;
	return __out.str();
}

void PRINT (const string &__s) {
	cerr << __s;
}
void PRINT (char __c) {
	cerr << __c;
}

void PRINT_LINE_NUMBER (int __line) {
	string __s = TO_STRING(__line);
	PRINT(string(LINE_DIGITS_CNT - __s.size(), '0') + __s + ": ");
}

void PRINT_DEBUG_PREFIX () {
	if (DEBUG_PREFIX.size() <= size_t(MAX_PREFIX_SIZE)) {
		PRINT(DEBUG_PREFIX);
		return;
	}
	string __num = TO_STRING(DEBUG_PREFIX.size());
	PRINT("(" + __num + ")" + DEBUG_PREFIX.substr(
		DEBUG_PREFIX.size() - MAX_PREFIX_SIZE + __num.size() + 2,
		MAX_PREFIX_SIZE - __num.size() - 2
	));
}

void PRINT_ARGS (string __args) {
	PRINT(__args + ": ");
}

void NEWLINE () {
	cerr << endl;
}

void OUTPUT () {}

template<typename T, bool = is_base_of<outputable, T>::value> struct OUTPUT_TYPE;

template<typename T> struct OUTPUT_TYPE<T, 0> {
	typedef T __type;
};
template<typename T> struct OUTPUT_TYPE<T, 1> {
	typedef outputable __type;
};

template<typename T> void OUTPUT (const T &__a) {
	outputer<typename OUTPUT_TYPE<T>::__type>()(__a);
}

template<typename It> void OUTPUT (It begin, It end) {
	PRINT("[");
	while (begin != end) {
		OUTPUT(*begin);
		if (++begin != end) {
			PRINT(", ");
		}
	}
	PRINT("]");
}

void FORMAT (const char *__s) {
	for (; *__s; ++__s) {
		if (*__s == '%') {
			assert(0);
		}
		PRINT(*__s);
	}
}
template<typename T, typename... Args> void FORMAT (const char *__s, const T &__a, const Args &...__args) {
	for (; *__s; ++__s) {
		if (*__s == '%') {
			OUTPUT(__a);
			FORMAT(++__s, __args...);
			return;
		}
		PRINT(*__s);
	}
	assert(0);
}

template<typename T> struct outputer {
	void operator () (const T &__a) {
		cerr << __a;
	}
};
template<> struct outputer<outputable> {
	template<typename T> void operator () (const T &__a) {
		__a.output();
	}
};
template<typename It> struct outputer<_INTERVAL<It> > {
	void operator () (const _INTERVAL<It> &__a) {
		OUTPUT(__a.__begin, __a.__end);
	}
};
template<> struct outputer<long double> {
	void operator () (long double __a) {
		OUTPUT(double(__a));
	}
};
template<> struct outputer<char> {
	void operator () (char __a) {
		if (isprint(__a)) {
			cerr << __a;
		} else {
			PRINT("${");
			cerr << int(__a);
			PRINT("}");
		}
	}
};
template<> struct outputer<const char*> {
	void operator () (const char *__a) {
		PRINT("\"");
		for (; *__a; ++__a) {
			OUTPUT(*__a);
		}
		PRINT("\"");
	}
};
template<> struct outputer<string> {
	void operator () (const string &__a) {
		OUTPUT(__a.c_str());
	}
};
template<typename T> struct outputer<deque<T> > {
	void operator () (const deque<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<list<T> > {
	void operator () (const list<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename K, typename T, typename C> struct outputer<map<K, T, C> > {
	void operator () (const map<K, T, C> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename K, typename T, typename C> struct outputer<multimap<K, T, C> > {
	void operator () (const multimap<K, T, C> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<queue<T> > {
	void operator () (const queue<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T, typename C> struct outputer<set<T, C> > {
	void operator () (const set<T, C> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T, typename C> struct outputer<multiset<T, C> > {
	void operator () (const multiset<T, C> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<stack<T> > {
	void operator () (const stack<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<valarray<T> > {
	void operator () (const valarray<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<vector<T> > {
	void operator () (const vector<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T1, typename T2> struct outputer<pair<T1, T2> > {
	void operator () (const pair<T1, T2> &__a) {
		PRINT("(");
		OUTPUT(__a.first);
		PRINT(", ");
		OUTPUT(__a.second);
		PRINT(")");
	}
};
template<typename T, size_t N> struct outputer<array<T, N> > {
	void operator () (const array<T, N> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<forward_list<T> > {
	void operator () (const forward_list<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T> struct outputer<initializer_list<T> > {
	void operator () (const initializer_list<T> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename K, typename T, typename H, typename P> struct outputer<unordered_map<K, T, H, P> > {
	void operator () (const unordered_map<K, T, H, P> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename K, typename T, typename H, typename P> struct outputer<unordered_multimap<K, T, H, P> > {
	template<typename A> void operator () (const unordered_multimap<K, T, H, P> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T, typename H, typename P> struct outputer<unordered_set<T, H, P> > {
	template<typename A> void operator () (const unordered_set<T, H, P> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};
template<typename T, typename H, typename P> struct outputer<unordered_multiset<T, H, P> > {
	template<typename A> void operator () (const unordered_multiset<T, H, P> &__a) {
		OUTPUT(__a.begin(), __a.end());
	}
};

template<typename Tuple, size_t Size = tuple_size<Tuple>::value> struct tuple_outputer;

template<typename Tuple> struct tuple_outputer<Tuple, 0> {
	template<typename A> void operator () (const A&) {}
};

template<typename Tuple, size_t Size> struct tuple_outputer : tuple_outputer<Tuple, Size - 1> {
	template<typename A> void operator () (const A &__a) {
		tuple_outputer<Tuple, Size - 1>::operator()(__a);
		if (Size > 1) {
			PRINT(", ");
		}
		OUTPUT(get<Size - 1>(__a));
	}
};

template<typename... Args> struct outputer<tuple<Args...> > {
	void operator () (const tuple<Args...> &__a) {
		OUTPUT("(");
		tuple_outputer<tuple<Args...> >()(__a);
		OUTPUT(")");
	}
};

struct RECURSION_LEVEL {
	size_t __shift_size;
	
	RECURSION_LEVEL (bool __b = 1, const string &__s = " ") : __shift_size(__s.size()) {
		SHUT_DEBUG = !__b;
		DEBUG_PREFIX += __s;
	}
	
	~RECURSION_LEVEL () {
		DEBUG_PREFIX.resize(DEBUG_PREFIX.size() - __shift_size);
		SHUT_DEBUG = 0;
	}
};

struct TIMESTAMPER {
	clock_t __start;
	int __line;
	
	TIMESTAMPER (int __l) : __start(clock()), __line(__l) {}
	
	~TIMESTAMPER () {
		stringstream __s;
		__s.setf(ios::showpoint | ios::fixed);
		__s.precision(3);
		__s << "Time=" << double(clock() - __start) / CLOCKS_PER_SEC << "s";
		PRINT_LINE_NUMBER(__line);
		PRINT_DEBUG_PREFIX();
		PRINT(__s.str());
		NEWLINE();
	}
};

string EXTRACT_NAME (string __s) {
	size_t __length = min(
		find(__s.rbegin(), __s.rend(), '/') - __s.rbegin(),
		find(__s.rbegin(), __s.rend(), '\\') - __s.rbegin()
	);
	__s = __s.substr(__s.size() - __length, __length);
	return __s.substr(0, find(__s.begin(), __s.end(), '.') - __s.begin());
}

