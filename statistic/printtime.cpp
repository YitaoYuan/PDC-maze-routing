#include<bits/stdc++.h>
using namespace std;



int main()
{
  string s;
  vector<double>vec;
  while(cin>>s) {
    if(s == "calculation:") {
       cin>>s;
       double t;
       sscanf(s.c_str(), "%lf", &t);
       vec.push_back(t);
    }
  }
  assert(vec.size() == 150);
  
  double vt = 0, st = 0, mt = 0;
  for(int i = 0; i < 150; i++) {
    if(i < 50) vt += vec[i];
    else if(i < 100) st += vec[i];
    else mt += vec[i];
  }
  printf("vt %.2lfs, st %.2lfs, mt %.2lfs\n", vt, st, mt);
  printf("avt %.2lfs, ast %.2lfs, amt %.2lfs\n", vt/50, st/50, mt/50);
  
  double spd_up_v2m = 0, spd_up_s2m = 0;
  for(int i = 0; i < 100; i++) {
    vec[i] = vec[i]/vec[100+i%50];
    //printf("%s %d: %.2lf\n", i<50?"v2m":"s2m", i%50, vec[i]);
    if(i < 50) spd_up_v2m += vec[i];
    else spd_up_s2m += vec[i]; 
  }
  printf("avg_v2m %.2lf\n", spd_up_v2m/50);
  printf("avg_s2m %.2lf\n", spd_up_s2m/50);
  return 0;
}
