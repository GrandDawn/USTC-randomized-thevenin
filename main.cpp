#include<iostream> 
#include<string>
#include<unordered_map>
#include<sstream>
#include "thevenin.cpp" 
int m,s,t;
std::unordered_map<std::string, int> dict; 
int dictsize;
int hash(std::string s){
	if(dict[s] == 0)dict[s] = ++dictsize;
	return dict[s];
}
equiv_R::complex convert(std::string s){
	std::stringstream ss;
	if(s[0]!='('){
		double x;
		ss << s; ss >> x;
		return (equiv_R::complex){x, 0};
	}
	else{
		double x, y; int pos = 0;
		for(int i = 0; i < s.size(); i++)
			if(s[i] == ',') pos = i;
		ss << s.substr(1, pos - 1); ss >> x;
		while(s[pos + 1] == ' ') pos++;
		ss << s.substr(pos + 1, (int)s.size() - pos - 2); ss >> y;
		return (equiv_R::complex){x, y};
	}
}
int main(){
	freopen("grid.txt","r",stdin);
	//freopen(".out","w",stdout);
	std::string tmpx, tmpy;
	std::cin>> m >> tmpx >> tmpy;
	s = hash(tmpx); t = hash(tmpy);
	std::vector<std::tuple<int, int, char, equiv_R::complex>>v;
	for(int i=1;i<=m;i++){
		std::string c, t; double a, b;
		std::cin >> tmpx >> tmpy >> c >> t;
		int x = hash(tmpx), y = hash(tmpy);
		if(c == "R")
			v.push_back(std::make_tuple(x, y, 'R', convert(t)));
		if(c == "L")
			v.push_back(std::make_tuple(x, y, 'R', (equiv_R::complex){0, convert(t).x}));
		if(c == "C")
			v.push_back(std::make_tuple(x, y, 'R', (equiv_R::complex){0, -convert(t).x}));
		if(c == "E" || c == "U"){
			//交流电源 
			v.push_back(std::make_tuple(x, y, 'U', convert(t)));
		}
		if(c == "I"){
			//交流电源 
			v.push_back(std::make_tuple(x, y, 'I', convert(t)));
		}
	}
	thevenin_theorem::init(v);
	auto ans = thevenin_theorem::calc(s, t);
	if(-1e50 < ans.first.x && ans.first.x < 1e-50) ans.first.x = 0;
	if(-1e50 < ans.first.y && ans.first.y < 1e-50) ans.first.y = 0;
	if(-1e50 < ans.second.x && ans.second.x < 1e-50) ans.second.x = 0;
	if(-1e50 < ans.second.y && ans.second.y < 1e-50) ans.second.y = 0;
	std::cout<<"U = ("<<ans.first.x<<","<<ans.first.y<<"), R = ("<<ans.second.x<<","<<ans.second.y<<")"<<std::endl;
	//std::cout<<equiv_R::calc(s,t).x;
	return 0;
}
/*
电压源/电流源 先输入正极再负极 
6 D E
A E U (20,0)
A B R 20
B C L 10
C D L 20
B E R 21
C E C 20

4 B D
A B R 4
B C 

5 B C
A B R (1,0)
B D R (1,0)
A C R 2
C D R 1
A D I (5,0)

12 1 8
1 2 R 1
2 3 R 1
3 4 R 1
4 1 R 1
1 5 R 1
2 6 R 1
3 7 R 1
4 8 R 1
5 6 R 1
6 7 R 1
7 8 R 1
8 5 R 1
*/
