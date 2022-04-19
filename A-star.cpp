/************************************************************/
/************************************************************/
/**************** A star algorithm   ************************/
/************        by桔皮沙拉    **************************/
/**************      qq1598058687    ************************/
/************************************************************/
/************************************************************/
#include<vector>
#include<string>
#include<list>
#include<algorithm>
#include<iostream>
using namespace std;

/* up,down,left,right */
const char *dir[] = { "上","下","左","右" };
/*	0:空白点 NULL Tils
*	1:起始点 Start
*	2:目标点 Finish
*	3:墙	Wall
*	4:路线	Path  
*/
class MatrixMap
{
	string m;
	int col, row, cursor;
public:
	string& GetMap()
	{
		return m;
	}
	int GetH() const
	{
		return row;
	}
	int GetW() const
	{
		return col;
	}
	MatrixMap() = default;
	MatrixMap(int w, int h, char c)
	{
		Assign(w, h, c);
	}
	void Assign(int w, int h, char c)
	{
		row = h, col = w, m.resize(w*h, c);
		cursor = 0;
	}
	~MatrixMap()
	{
		free();
	}
	void free()
	{
		m.clear();
		cursor = row = col = 0;
	}
	char GetV() const
	{
		return m[cursor];
	}
	char GetV(int x, int y) const
	{
		return m[y * col + x];
	}
	MatrixMap & operator>>(char &c)
	{
		c = m[cursor];
		return *this;
	}
	MatrixMap & operator<<(char c)
	{
		m[cursor] = c;
		return *this;
	}
	bool SetPos(int x, int y)
	{
		if ((x >= 0 && x < col) && (y >= 0 && y < row))
		{
			cursor = y * col + x;
			return true;
		}
		return false;
	}
	friend ostream & operator<<(ostream & os, const MatrixMap & r)
	{
		for (int i = 0; i < r.GetH(); i++)
		{
			for (int j = 0; j < r.GetW(); j++)
				os << /*hex<<(int)*/r.m[i * r.GetW() + j] << " ";
			os << "\n";
		}
		return os;
	}
};

struct Node
{
public:
	Node(int dx, int dy, int dg = 0, int df = 0)
		: x(dx), y(dy), g(dg), h(0), f(df)
	{
	}
	int x, y, g, h, f;
	Node(const Node &r) :x(r.x), y(r.y), g(r.g), h(r.h), f(r.f) {}
	Node& operator=(const Node &r) {
		if (this != &r)
		{
			x = r.x, y = r.y, g = r.g, h = r.h, f = r.f;
		}
		return *this;
	}
	void calculateCost(int x2, int y2)
	{
		g += 1;
		h = std::abs(x2 - x) + std::abs(y2 - y);
		f = g + h;
	}
};


class Astar
{
	MatrixMap m;
	int xStart, yStart, xFinish, yFinish, success;
public:
	string path;
	vector <Node>openlist;	// open表
	vector < Node>closedlist;	// closed表
	Astar()
	{
		// Assign(w, h);
	}
	void Assign(int w, int h)
	{
		xStart = yStart = xFinish = yFinish = -1, success = 0;
		m.Assign(w, h, '\0');
	}
	void free()
	{
		m.free();
		path.clear();
		openlist.clear();
		closedlist.clear();
	}
	~Astar()
	{
		free();
	}

	void setWall(int x, int y)	// 设置障碍
	{
		char c;
		m.SetPos(x, y);
		m >> c;
		if (c != '\1' && c != '\2')
		{
			m << (c == '\0' ? '\3' : '\0');
		}
	}
	void setFinish(int x, int y)	// 设置目标
	{
		char c;
		m.SetPos(x, y);
		m >> c;
		if (c != '\1'&& c != '\3')
		{
			if ((xFinish != -1 && yFinish != -1) && (xFinish != x && yFinish != y))
				m << '\0';//如果前面有设置过则置0
			m << (c == '\0' ? '\2' : '\0');
			xFinish = x, yFinish = y;
		}
	}
	void setStart(int x, int y)	// 设置开始
	{
		char c;
		m.SetPos(x, y);
		m >> c;
		if (c != '\2'&& c != '\3')
		{
			if ((xStart != -1 && yStart != -1) && (xStart != y && yStart != x))
				m << '\0';
			m << (c == '\0' ? '\1' : '\0');
			xStart = x, yStart = y;
			openlist.clear();
			openlist.push_back({ xStart, yStart });
		}
	}
	int direction[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };	// 用来生成节点周围上下左右的坐标移动值

	vector<Node>::iterator findLeastNode(vector<Node>& v)
	{
		auto result = v.begin();
		for (auto i = v.begin(); i != v.end(); i++) {
			if (result->f > i->f) {
				result = i;
			}
		}
		return result;
	}
	string pathFind()
	{
		Node n0(xStart, yStart);
		n0.calculateCost(xFinish, yFinish);
		openlist.push_back(n0);

		// A* 算法
		while (!openlist.empty())
		{

			auto it = findLeastNode(openlist);
			n0 = *it;

			int x = it->x, y = it->y;

			closedlist.push_back(*it);
			openlist.erase(it);

			if (x == xFinish && y == yFinish)
			{
				char c;
				while (!(x == xStart && y == yStart))
				{
					m.SetPos(x, y);
					m >> c;
					/* j指向上级方向 */
					int j = c >> 4;
					c = (j ^ 1) + '0';
					path = c + path;//往前插入
					x += direction[j][0];
					y += direction[j][1];
				}
				for (auto &i : m.GetMap())
					i &= 0xf;
				return path;
			}

			for (int i = 0; i < sizeof(direction) / sizeof(*direction); i++)
			{
				int cx = x + direction[i][0], cy = y + direction[i][1];

				m.SetPos(cx, cy);

				if (!(cx < 0 || cx >= m.GetW() || cy < 0 || cy >= m.GetH() || m.GetV() == '\3' ||
					find_if(closedlist.begin(), closedlist.end(),
						[&](Node &a) {return cx == a.x && cy == a.y; }) != closedlist.end()))
				{

					Node m0(cx, cy, n0.g,
						n0.f);
					m0.calculateCost(xFinish, yFinish);

					auto open_it = find_if(openlist.begin(), openlist.end(),
						[&](Node &a) {return cx == a.x && cy == a.y; });

					if (open_it == openlist.end())
					{
						openlist.push_back(m0);

						m.SetPos(cx, cy);
						m << (m.GetV() + ((i ^ 1) << 4));
				
					}
					else if (open_it != openlist.end() && open_it->f > m0.f)
					{
						m.SetPos(cx, cy);
						*open_it = m0; 
					
						m.SetPos(cx, cy);
						m << (m.GetV() + ((i ^ 1) << 4));
					}

				}
			}

		}
		return ""; // no route found
	}
	void DisplayPath()
	{
		for (auto &u : path)
			cout << dir[u^'0'] << " ";
		cout << endl;
	}
	void ConvertMap()
	{
		if (path.size())
		{
			m.SetPos(xStart, yStart);
			m << '\1';

			int x = xStart, y = yStart;
			for (auto &u : path)
			{
				int c = (u - '0') & 0xff;
				x += direction[c][0];
				y += direction[c][1];
				m.SetPos(x, y);
				m << '\4';
			}
			m.SetPos(xFinish, yFinish);
			m << '\2';
			ShowMap();
		}
		else
			cout << "没有内容" << endl;
	}
//	private:
	void  ShowMap()
	{
		for (auto i = 0; i < m.GetH(); ++i) {
			for (int j = 0; j < m.GetW(); ++j)
			{
				m.SetPos(j, i);
				if (m.GetV() == '\0')
					cout << "." ;
				else if (m.GetV() == '\1')
					cout << "a" ;
				else if (m.GetV() == '\2')
					cout << "b" ;
				else if (m.GetV() == '\3')
					cout << "#" ;
				else if (m.GetV() == '\4')
					cout << "r" ;
			}
			cout << endl;
		}
			
	
	}
};

int main()
{
	Astar astar;
	//初始化地图大小
	astar.Assign(10, 10);

	/* 设置墙 */
	for (int i = 2; i < 8; i++)
	{
		astar.setWall(i, 5);
		astar.setWall(5, i);
	}
	astar.setWall(5, 1);
	astar.setWall(5, 9);
	//astar.ShowMap();
	/* 设置起点与终点 */
	astar.setStart(4, 4);
	astar.setFinish(6, 6);
	
	cout << "总共步数:"<<astar.pathFind().size() << endl;
	astar.DisplayPath();

	astar.ConvertMap();
	//astar.ShowMap();
	return 0;
}