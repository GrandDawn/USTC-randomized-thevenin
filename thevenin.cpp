#include <vector>
#include <unordered_set>
#include <map>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm> 
#include <queue>
#include "adjustment.cpp"
namespace thevenin_theorem{
	using namespace std;
	vector<tuple<int, int, char, equiv_R::complex>> v;
	void init(vector<tuple<int, int, char, equiv_R::complex>> v2){
		v = v2;
	}
	pair<equiv_R::complex, equiv_R::complex> calc(int s, int t){
		equiv_R::complex E = (equiv_R::complex){0, 0}, R = (equiv_R::complex){0, 0};
		for(int i = 0; i < v.size(); i++){
			if(get<2>(v[i]) == 'U'){
				//电压源 
				vector<tuple<int, int, equiv_R::complex>> vt;
				for(int j = 0;j < v.size(); j++){
					if(i == j)continue;
					if(get<2>(v[j]) == 'R')
						vt.push_back(make_tuple(get<0>(v[j]), get<1>(v[j]), get<3>(v[j])));
					else if(get<2>(v[j]) == 'U')
						vt.push_back(make_tuple(get<0>(v[j]), get<1>(v[j]), (equiv_R::complex){0, 0}));						
				}
				equiv_R::init(vt);
				equiv_R::calc(get<0>(v[i]), get<1>(v[i]));
				auto Et = equiv_R::voltage(s, t);
				E = E + Et * get<3>(v[i]);
			} 
			if(get<2>(v[i]) == 'I'){
				//电流源 
				vector<tuple<int, int, equiv_R::complex>> vt;
				for(int j = 0;j < v.size(); j++){
					if(i == j)continue;
					if(get<2>(v[j]) == 'R')
						vt.push_back(make_tuple(get<0>(v[j]), get<1>(v[j]), get<3>(v[j])));
					else if(get<2>(v[j]) == 'U')
						vt.push_back(make_tuple(get<0>(v[j]), get<1>(v[j]), (equiv_R::complex){0, 0}));						
				}
				vt.push_back(make_tuple(0, get<0>(v[i]), (equiv_R::complex){0, 0}));
				equiv_R::init(vt);
				equiv_R::calc(0, get<1>(v[i]));
				auto Et = equiv_R::voltage(s, t); //当电流源电压为 (1, 0) 时的结果
				auto I = equiv_R::current[make_pair(0, get<0>(v[i]))];
				E = E + get<3>(v[i]) / I * Et;
			} 
		}
		vector<tuple<int, int, equiv_R::complex>> vt;
		for(int j = 0; j < v.size(); j++){
			if(get<2>(v[j]) == 'R')
				vt.push_back(make_tuple(get<0>(v[j]),get<1>(v[j]),get<3>(v[j])));
			else if(get<2>(v[j]) == 'U')
				vt.push_back(make_tuple(get<0>(v[j]),get<1>(v[j]),(equiv_R::complex){0, 0}));
		}
		equiv_R::init(vt); 
		R = equiv_R::calc(s, t);
		return make_pair(E, R);
	}
}

