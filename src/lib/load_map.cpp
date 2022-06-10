#include "src/lib/maze_routing.h"
#include<sys/stat.h>
#include<sys/types.h>
#include<string>
#include<dirent.h>
#include<algorithm>
namespace load_map
{
    FILE *fin;

    char mygetchar()
    {
        static const int BUFSIZE = 4096;
        static char buf[BUFSIZE], *ptr = buf + BUFSIZE;
        static int len = BUFSIZE;
        if(ptr == buf + len) {
            len = fread(buf, 1, BUFSIZE, fin);
            ptr = buf;
            if(len == 0) return -1;
        }
        return *(ptr++);
    }

    char mygetint()
    {
        char ret = 0;
        bool flag = 0;
        char c;
        do c = mygetchar(); while(c != 'S' && c != 'D' && c != '-' && (c < '0' || c > '9') );
        if(c == 'S') return -2;
        if(c == 'D') return -3;
        if(c == '-') flag = 1, c = mygetchar();
        do {
            ret = ret*10 + c-'0';
            c = mygetchar();
        }while('0' <= c && c <= '9');
        return flag ? -ret : ret;
    }

    void load_map(const char *file_name)
    {
        fin = fopen(file_name, "r");
        if(fin == NULL) {
            printf("can not open file %s\n", file_name);
            exit(0);
        }
        for(index_t i = 1; i <= Row; i++)
            for(index_t j = 1; j <= Col; j++) {
                char val = mygetint();
                if(val == -1) val = MAX_VAL+1;
                if(val == -2) S = {i, j}, val = 0;
                if(val == -3) D = {i, j}, val = 0;
                *(uc*)(map[i]+j) = (uc)val; // clear and set
            }
        for(index_t i = 1; i <= Row; i++) map[i][0].val = map[i][Col+1].val = MAX_VAL+1;
        for(index_t j = 1; j <= Col; j++) map[0][j].val = map[Row+1][j].val = MAX_VAL+1;
        fclose(fin);
    }
    bool cmp(const string &s1, const string &s2)
    {
        return s1.size() == s2.size() ? s1 < s2 : s1.size() < s2.size();
    }
    
    vector<string> get_file_list(const char *path)
    {
        vector<string>files;
        struct stat s_buf;
        my_assert(stat(path, &s_buf)==0, "can not open file");

        if(S_ISDIR(s_buf.st_mode)) {
            struct dirent *filename;
            DIR *dp = opendir(path);
            my_assert(dp != NULL, "can not open directory");

            while((filename = readdir(dp)))
            {
                string file_path = string(path) + "/" + filename->d_name;
                
                my_assert(stat(file_path.c_str(), &s_buf)==0, "can not open file in directory");
    
                if(S_ISREG(s_buf.st_mode))
                    files.push_back(file_path);
            }
            sort(files.begin(), files.end(), cmp);
        }
        else if(S_ISREG(s_buf.st_mode)) {
            files.push_back(path);
        }
        return files;
    }
}
