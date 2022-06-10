#include "src/lib/maze_routing.h"

block map[Row+2][Col+2];

pos S, D;

namespace maze_routing
{
    FILE *fout;
    pos SMID, DMID;
    const index_t dr[] = {-1, 0, 1, 0}, dc[] = {0, -1, 0, 1};

    #define Map(Pos) map[Pos.r][Pos.c]
    
    pos pre_pos(pos p, int way)
    {
        return {index_t(p.r - dr[way]), index_t(p.c - dc[way])};
    }

    int solve(const char *case_name)
    {
        int nxt_q_id[11] = {0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4};
        vector<pos> q[MAX_VAL + 1];
        q[0].push_back(S);
        //q[0].push_back(D);

        Map(S).in_q = 1;
        Map(S).set = 0;
        Map(D).in_q = 1;
        Map(D).set = 1;
        Map(D).out_q = 1;
        
	int n_empty_q = 0;

        for(int dis = 0, q_id = 0; ; dis++, q_id = q_id==MAX_VAL ? 0 : q_id+1)
        {
            if(q[q_id].empty()) n_empty_q++;
            else n_empty_q=0;
            if(n_empty_q == MAX_VAL) break;
            
            for(auto p : q[q_id])
            {
                Map(p).out_q = 1;
                char myset = Map(p).set;
                for(int way = 0; way < (int)(sizeof(dr)/sizeof(dr[0])); way ++)
                {
                    pos subp = {index_t(p.r + dr[way]), index_t(p.c + dc[way])};     
                    
                    if(Map(subp).val > MAX_VAL) continue;
                    
                    if(Map(subp).out_q == 1)
                    {
                        if(Map(subp).set == myset) continue; // out queue before in this set
                            
                        // found
                        if(myset == 0) SMID = p, DMID = subp;
                        else SMID = subp, DMID = p;
                        goto found;
                    }
                    else if(Map(subp).in_q == 1) continue;
                    // it "in queue" earlier in another set, so "out queue" earlier too.
                    // or, it has "in queue"ed in this set 
                    Map(subp).set = myset;
                    Map(subp).from = way;
                    Map(subp).in_q = 1;
                    q[nxt_q_id[q_id + Map(subp).val]].push_back(subp);
                }
            }
            q[q_id].clear();
        }
        //not_found:
        fprintf(fout, "%s -1\n", case_name);
        return -1;
        
        found:
        int sum = 0;
        pos p;
        p = SMID;
        deque<pos>route;
        while(1) {
            sum += Map(p).val;
            route.push_front(p);
            if(p == S) break;
            p = pre_pos(p, Map(p).from);
        }
        p = DMID;
        while(1) {
            sum += Map(p).val;
            route.push_back(p);
            if(p == D) break;
            p = pre_pos(p, Map(p).from);
        }
        fprintf(fout, "%s ", case_name);
        fprintf(fout, "%d", sum);
        for(auto pt : route)
            fprintf(fout, " (%d, %d)", pt.r-1, pt.c-1);
        fprintf(fout, "\n");
        return sum;
    }
}



int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("use: ./maze_routing file_name/dir_name result_file_name\n");
        return 0;
    }

    vector<string>files = load_map::get_file_list(argv[1]);
    maze_routing::fout = fopen(argv[2], "w");
    my_assert(maze_routing::fout!=NULL, "can not open result file");

    for(auto &s : files) {
        ull t1 = get_time_ns(), t2;
        printf("%s:\n", s.c_str());
        
        load_map::load_map(s.c_str());
        t2 = get_time_ns();
        printf("    Run time of loading map file: %.2lfs\n", (t2-t1)*1e-9);
        t1 = t2;
        
        size_t file_name_st = 0, file_name_ed = s.size();
        for(size_t i = 0; i < s.size(); i++) 
            if(s[i] == '/') file_name_st = i+1;
            else if(s[i] == '.') file_name_ed = i;
        my_assert(file_name_st != s.size(), "not a file");
        int res = maze_routing::solve(s.substr(file_name_st, file_name_ed - file_name_st).c_str());
        printf("    Answer in brief: %d\n", res);
        t2 = get_time_ns();
        printf("    Run time of calculation: %.2lfs\n", (t2-t1)*1e-9);
    }
    return 0;
}
