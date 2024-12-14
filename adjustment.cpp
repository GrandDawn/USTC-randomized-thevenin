namespace equiv_R{
	using namespace std;
	const int CPS=1000000;
	time_t inline get_current_time() {
		return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
	}
	mt19937 Rand(chrono::steady_clock::now().time_since_epoch().count()); // 定义一个随机数生成
	struct complex{
		double x,y;
		//complex(double x, double y) : x(x), y(y) {}
		complex operator+ (const complex& a) const {
			return (complex){x + a.x,y + a.y};
		}
		complex& operator+= (const complex& a) {
			x += a.x; y += a.y;
			return *this;
		}
		complex operator- (const complex& a) const {
			return (complex){x - a.x,y - a.y};
		}
		complex& operator-= (const complex& a) {
			x -= a.x; y -= a.y;
			return *this;
		}
		complex operator* (const complex& a) const {
			return (complex){x * a.x - y * a.y,x * a.y + y*a.x};
		}
		complex operator/ (const double& a) const {
			return (complex){x / a, y / a};
		}
		complex operator/ (const complex& a) const {
			return (complex){x * a.x + y * a.y, y * a.x - x * a.y}
				/(a.x * a.x + a.y * a.y);
		}
		complex& operator= (const double& a) {
			x = a;
			return *this;
		}
	}; 
	vector< pair<int, complex> > to[100009];
	map<pair<int, int>, complex> current;
	int mxid = 0;
	void clear(){
		current.clear();
		for(int i = 0;i <= mxid; i++)to[i].clear();
		mxid = 0;
	}
	void init(vector<tuple<int, int, complex>> v){
		/*
		cout << "START:" << endl;
		for(auto i: v){
			cout << get<0>(i) << " " << get<1>(i) << " " << get<2>(i).x << " " << get<2>(i).y << endl;
		}*/
		clear();
		for(auto i: v){
			to[get<0>(i)].push_back(make_pair(get<1>(i), get<2>(i)));
			to[get<1>(i)].push_back(make_pair(get<0>(i), get<2>(i)));
			mxid = max(mxid, get<0>(i));
			mxid = max(mxid, get<1>(i));
		}
	}
	complex calc(int s, int t, int steps_limit = 10 * mxid,int simu_timeout = 3,int num_threads = 256){
		auto get_route = [&] (vector<pair<int, complex> >* path) {
			int x = s;
			unordered_set<int> visited; visited.insert(x);
			int steps = 0;
			while (x != t) {
				if( to[x].size() == 0)break;// 无出边则开路 
				auto edge = to[x][Rand() % to[x].size()]; // 随机访问一个相邻点
				auto y = edge.first;
				if (visited.find(t) != visited.end()) {
					// 如果出现环，则删除掉这个环
					while (path->size() > 0 && path->back().first != y) {
						visited.erase(path->back().first);
						path->pop_back();
					}
				}
				else {
					// 将这条边加入路径中
					path->emplace_back(edge);
					visited.insert(y);
				}
				if (++steps > steps_limit) break;
				x = y;
			}
		};
		time_t start = get_current_time(), last_flush_t = 0;
		complex result = (complex){0, 0};
		int simu_times = 0;
		while (get_current_time() - start < simu_timeout * CPS) {
			/*
			if (progress_bar && get_current_time() - last_flush_t > 0.1 * CPS) {
				// 输出一个进度条
				printf("\rSimu Times: %d (%.2lf / %.2lf s)", simu_times, 1. * (get_current_time() - begin_t) / CPS, simu_timeout);
				fflush(stdout);
				last_flush_t = get_current_time();
			}*/ 
			simu_times += num_threads;
			vector<pair<int, complex> > route[num_threads];
			for (int i = 0; i < num_threads; i++) route[i].reserve(steps_limit);
			thread threads[num_threads];
			for (int i = 0; i < num_threads; i++) {
				threads[i] = thread(get_route, route + i);
			}
			for (int i = 0; i < num_threads; i++) threads[i].join();
			for (int i = 0; i < num_threads; i++) {
				if (route[i].back().first != t) continue;
				complex r_sum = (complex){0, 0}, v_sum = (complex){0, 0};
				int x = s;
				for (auto edge : route[i]) {
					int y = edge.first;
					complex r = edge.second;
					r_sum += r;
					if(x < y)
						v_sum = v_sum + current[make_pair(x, y)] * r;
					else 
						v_sum = v_sum - current[make_pair(y, x)] * r;
					x = y;
				}
				x = s;
				if (r_sum.x == 0 && r_sum.y == 0) continue; 
				complex new_current = ((complex){1, 0} - v_sum) / r_sum; // 计算增加电流 I' = (U - U_0) / R_0
				//cout << r_sum.x << " " << r_sum.y << " " << new_current.x << " " << new_current.y << endl;
				result += new_current;
				for (auto edge : route[i]) {
					int y = edge.first;
					complex r = edge.second;
					if(x < y)
						current[make_pair(x, y)] += new_current;
					else 
						current[make_pair(y, x)] -= new_current;
					// 更新路径上的电流值
					x = y;
				}
			}
		}
		//if (progress_bar) printf("\rSimu Times: %d (%.2lf / %.2lf s)\n", simu_times, (1. * get_current_time() - begin_t) / CPS, simu_timeout);
		return complex{1, 0} / result;
	}
	complex voltage(int s, int t){
		map<int, complex> V;
		V[s] = 0;
		queue<int> q; q.push(s);
		// BFS 计算电压差 
		while(!q.empty()){
			int x = q.front(); q.pop();
			for (auto edge : to[x]) {
				int y = edge.first;
				complex r = edge.second;
				if(V.find(y) == V.end()){
					if (x < y)
						V[y] = V[x] + current[make_pair(x, y)] * r;
					else
						V[y] = V[x] - current[make_pair(y, x)] * r;
					q.push(y);
					if(y == t)return V[t];
				}
			}
		}
		return (complex){1, 0};// 开路 
	} 
}
