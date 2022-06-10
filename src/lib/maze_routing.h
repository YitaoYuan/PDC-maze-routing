#include<cstdio> 
#include<cstdlib> 
#include<vector>
#include<assert.h>
#include<deque>
#include<string>
#include<cstring>
#include<omp.h>

using namespace std;

typedef unsigned char uc;
typedef unsigned long long ull;
typedef short index_t;

const index_t Row = 10000, Col = 10000;
const uc MAX_VAL = 5;

struct block{
    uc val : 3;
    uc in_q : 1;
    uc out_q : 1;
    uc set : 1;
    uc from : 2;
    char dis;
};

typedef short block_t;
static_assert(sizeof(block)==sizeof(block_t));

struct pos{
    index_t r, c;
    bool operator == (const pos x) const{
        return r == x.r && c == x.c;
    }
};

namespace load_map 
{
    vector<string> get_file_list(const char *);
    void load_map(const char *);
}

extern block map[Row+2][Col+2];

extern pos S, D;

inline void my_assert(bool val, const char* s)
{
    if(!val) {
        puts(s);
        exit(0);
    }
}

inline ull get_time_ns()
{
	struct timespec tm;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tm);
	return tm.tv_sec * 1000000000ull + tm.tv_nsec;// wrapped around every 585 years after starting up
}
