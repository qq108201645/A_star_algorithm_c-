#include <ncurses.h>
#include <locale.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <string.h>
#include <fstream>
using namespace std;

const char *title = "A star algorithm";
const char *Text[] = {
	"Welcome to use a star tools",
	"by 1598058687" 
};

/************************************************************/
/************************************************************/
/**************** A star algorithm   ************************/
/************        by桔皮沙拉    **************************/
/**************      qq1598058687    ************************/
/************************************************************/
/************************************************************/


/*---------------------------控制台后端---------------------------------------*/

/* up,down,left,right */
const char *dir[] = { "上", "下", "左", "右" };
/*	'.' / 0:空白点 NULL tile
*	'A' / 1:起始点 Start
*	'B' / 2:目标点 Finish
*	'#' / 3:墙	Wall
*	'r' / 4:路线	Path  
*/

struct Node
{
public:
	Node() = default;
	Node(char d_tile, int dx, int dy, int dg = 0, int df = 0)
		: tile(d_tile)
		, x(dx)
		, y(dy)
		, g(dg)
		, h(0)
		, f(df)
		, dir(0)
	{
	}
	/* 包含坐标
	*	F=G+H
	*	G表示该点到起始点位所需要的代价
	*	H表示该点到终点的曼哈顿距离。
	*	F就是G和H的总和，而最优路径也就是选择最小的F值，进行下一步移动（后边会做详细介绍）
	*	地图块
	*	方向
	*/
	int x, y, g, h, f, dir;
	char tile;
	Node(const Node &r)
		: x(r.x)
		, y(r.y)
		, g(r.g)
		, h(r.h)
		, f(r.f)
		, dir(0)
		, tile(r.tile) {}
	Node& operator=(const Node &r) {
		if (this != &r)
		{
			x = r.x, y = r.y, g = r.g, h = r.h, f = r.f, dir = 0, tile = r.tile;
		}
		return *this;
	}
	void Assign(int dx, int dy, int dg = 0, int df = 0)
	{
		x = dx, y = dy, g = dg, h = df, dir = 0, tile = '\0';
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
public:
	int mWidth, mHeight, xStart, yStart, xFinish, yFinish;
public:
	string path;
	vector<vector<Node>> nodeMap;
	vector <Node>openlist;   	// open表
	vector < Node>closedlist;   	// closed表
	Astar()
	{
		// Assign(w, h);
	}
	void Assign(int w, int h)
	{
		nodeMap.resize(h);
		mWidth = w, mHeight = h;
		xStart = yStart = xFinish = yFinish = -1;
		for (int i = 0; i < mHeight; ++i)
		{
			for (int j = 0; j < mWidth; ++j)
				nodeMap[i].push_back(Node('.', j, i, 0, 0));
		}
	}
	void free()
	{
		for (size_t i = 0; i < nodeMap.size(); ++i)
		{
			nodeMap[i].clear();
		}
		nodeMap.clear();

		clearList();
	}
	void clearList()
	{
		path.clear();
		openlist.clear();
		closedlist.clear();
	}
	~Astar()
	{
		free();
	}

	int  setWall(int x, int y)	// 设置障碍
	{
		char c = nodeMap[y][x].tile;

		if (c != 'A' && c != 'B')
		{
			nodeMap[y][x].tile = (c == '.' ? '#' : '.');
			return 1;
		}
		return 0;
	}
	int setFinish(int x, int y)	// 设置目标
	{
		char c = nodeMap[y][x].tile;

		if (c != 'A'&& c != '#')
		{
			if ((xFinish != -1 && yFinish != -1) && (xFinish != x || yFinish != y))
				nodeMap[yFinish][xFinish].tile = '.';   //如果前面有设置过则置0

			if(c == '.')
			{
				nodeMap[y][x].tile = 'B'; 
				xFinish = x, yFinish = y;
				return 1;
			}
			else {
				nodeMap[y][x].tile = '.'; 
				xFinish = -1, yFinish = -1;
				return -1;
			}
		}
		return 0;
	}
	int setStart(int x, int y)	// 设置开始
	{
		char c = nodeMap[y][x].tile;

		if (c != 'B'&& c != '#')
		{
			if ((xStart != -1 && yStart != -1) && (xStart != x || yStart != y))
				nodeMap[yStart][xStart].tile = '.';

			if (c == '.')
			{
				nodeMap[y][x].tile = 'A';
				xStart = x, yStart = y;
				return 1;
			}
			else
			{
				nodeMap[y][x].tile = '.';

				xStart = -1, yStart = -1;
				return -1;
			}
		}
		return 0;
	}
	int direction[4][2] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };   	// 用来生成节点周围上下左右的坐标移动值
	void SaveMap()
	{
		fstream os("Astar-map.txt",ios::out);
		os <<mWidth<<" "<<mHeight<<" "<< xStart << " " << yStart << " " << xFinish << " " << yFinish << "\n";
		for (size_t i = 0; i < nodeMap.size(); i++)
		{
			for (int j = 0; j < nodeMap[i].size(); ++j)
				os << nodeMap[i][j].tile << " ";
			os << "\n";
		}
		os.close();
	}
	bool LoadMap()
	{
		fstream in("Astar-map.txt", ios::in);
		if (!in)
		{
			in.close();
			return false;
		}
		free();
		
		in >> mWidth >> mHeight >> xStart >> yStart >> xFinish >> yFinish;
		nodeMap.resize(mHeight);
		for (size_t i = 0; i < mHeight; i++)
		{
			for (int j = 0; j < mWidth; ++j)
			{
				nodeMap[i].push_back(Node('.', j, i, 0, 0));
				in >> nodeMap[i][j].tile;
			}
		}
		in.close();
		return true;
	}
		
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
		nodeMap[yStart][xStart].calculateCost(xFinish, yFinish);
		openlist.push_back(nodeMap[yStart][xStart]);

		// A* 算法
		while(!openlist.empty())
		{

			auto curit = findLeastNode(openlist);
		

			int cx = curit->x, cy = curit->y;

			Node curNode = *curit;
			//Node curNode = nodeMap[cy][cx];
			closedlist.push_back(*curit);
			openlist.erase(curit);
			

			if (cx == xFinish && cy == yFinish)
			{
				char c;
				while (!(cx == xStart && cy == yStart))
				{
					int j =  nodeMap[cy][cx].dir;
					c = j ^ 1;
					path = c + path;   //往前插入
					cx += direction[j][0];
					cy += direction[j][1];
				}
				return path;
			}

			for (int i = 0; i < sizeof(direction) / sizeof(*direction); i++)
			{
				int dx = cx + direction[i][0], dy = cy + direction[i][1];

				if (!(dx < 0 || dx >= mWidth || dy < 0 || dy >= mHeight || nodeMap[dy][dx].tile == '#' ||
					find_if(closedlist.begin(),
					closedlist.end(),
					[&](Node &a) {return dx == a.x && dy == a.y; }) != closedlist.end()))
				{

					Node tmp('.', dx, dy, curNode.g, curNode.f);
					tmp.calculateCost(xFinish, yFinish);

					auto open_it = find_if(openlist.begin(),
						openlist.end(),
						[&](Node &a) {return dx == a.x && dy == a.y; });

					if (open_it == openlist.end())
					{
						openlist.push_back(tmp);
						nodeMap[dy][dx].dir = i ^ 1;
				
					}
					else if (open_it != openlist.end() && open_it->f > tmp.f)
					{
						*open_it = tmp;
					
						nodeMap[dy][dx].dir = i ^ 1;
					}

				}
			}

		}
		return ""; // no route found
	}
	void ShowPath() {
		if (path.size())
		{
			for (auto &i : path)
			{
				cout << " " << dir[i];
			}
			cout << endl;
		}
	}
	string GetPath()
	{
		return path;
	}
	//	private:
		void  ShowMap()
	{
		
		if (path.size())
		{
			int cx = xStart, cy = yStart;
			for (size_t i = 0; i < path.size() - 1; ++i)
			{
				int j = path[i];
				cx += direction[j][0], cy += direction[j][1];
				nodeMap[cy][cx].tile = 'r';
			}
			// 		nodeMap[yStart][xStart].tile = 'A';
			// 		nodeMap[yFinish][xFinish].tile = 'B';
		}
		for (auto i = 0; i < mHeight; ++i) {
			for (int j = 0; j < mWidth; ++j)
			{
				cout << nodeMap[i][j].tile;
			}
			cout << endl;
		}
	}
};

/*---------------------------控制台前端---------------------------------------*/
/* ACS_LARROW:←
 ACS_RARROW : →    ACS_DARROW : ↓
ACS_UARROW : ↑
*/

		
const char *Menubar[] = { "Start", "Finish", "Wall", "save","load","Run" };
int wMax = 0, hMax = 0, mwMax = 0, mhMax = 0, dtypew = 0, dtypeh = 0;

WINDOW* DrawWin(int &wMax, int &hMax, int &mwMax, int &mhMax)
{
	getmaxyx(stdscr, hMax, wMax);
	
	WINDOW *win = newwin(22 - 4, wMax - 4, 2, 2);
	getmaxyx(win, mhMax, mwMax);
	
	box(win, 0, 0);
	
	
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwhline(win, 2, 1, ACS_HLINE, mwMax);
	mvwaddch(win, 2, mwMax - 1, ACS_RTEE);
	
	
	mvwaddch(win, 2, mwMax - 9, ACS_TTEE);
	mvwvline(win, 3, mwMax - 9, ACS_VLINE, mhMax - 3 - 1);
	mvwaddch(win, mhMax - 1, mwMax - 9, ACS_BTEE);
	
	//wattron(win, A_BOLD);
	mvwprintw(win, 1, (mwMax - strlen(title)) / 2, title);
	for (int i = 0; i < sizeof(Menubar) / sizeof(*Menubar); ++i)
		mvwprintw(win, i * 2 + 3, mwMax - 7, Menubar[i]);
	//wattroff(win, A_BOLD);
	//mvwchgat(win, 1, 1, mwMax-2, ACS_NORMAR, 1, NULL);
	wrefresh(win);
	//wgetch(win);
	return win;
}

class SelectBar {
	vector<pair<int, int>> choice;
public:
	int cursor,  runflag,dispdir, oldStartx, oldStarty, oldFinishx, oldFinishy;
public:
	SelectBar()
		: cursor(-1)
		, runflag(0)
		, dispdir(0)
		, oldStartx(-1)
		, oldStarty(-1)
		, oldFinishx(-1)
		, oldFinishy(-1) {}
	void add(int x, int y) {
		choice.push_back({ x, y });
	}
	void clear(WINDOW *win) {
		for (auto &j : choice)
		{
			mvwprintw(win, j.second + 1, j.first, "     ");
			mvwchgat(win, j.second, j.first, 7, A_NORMAL, 0, NULL);
			
			mvwchgat(win, j.second + 1, j.first, 7, A_NORMAL, 0, NULL);
		}
	}
	int Handle(WINDOW* win, int x, int y, Astar &astar)
	{
		for (int i = 0; i < choice.size(); ++i)
			if (!(x<choice[i].first || x>choice[i].first + 6 || y<choice[i].second || y>choice[i].second + 3))
			{
				clear(win);
				cursor = i;
				return cursor;
			}
		for (size_t i = 0; i < astar.nodeMap.size(); i++)
		{
			auto &Nmap = astar.nodeMap;
			for (size_t j = 0; j < Nmap[i].size(); j++)
			{
				int curOffsetx = x - 3, curOffsety = y - 5;
	
				if (!(curOffsetx<j || curOffsetx>j || curOffsety<i || curOffsety>i))
				{
					if (cursor == 0 || cursor == 1 || cursor == 2)
					{
						int state = 0;
						if (cursor == 0)
						{
							state = astar.setStart(j, i);
							
							if (state == 1)
							{
								if ((oldStartx != -1&&oldStarty != -1)&&(oldStartx != x - 2 || oldStarty != y - 2))
								{
									
									mvwaddch(win, oldStarty, oldStartx, ACS_BOARD);
									//	mvwchgat(win, oldStarty, oldStartx, 1, A_NORMAL, 0, 0);
								}
								oldStartx = x - 2;
								oldStarty = y - 2;
							}
							else if (state == -1)
							{
								oldStartx = -1;
								oldStarty = -1;
							}
						}
					
						else if (cursor == 1)
						{
							state = astar.setFinish(j, i);
							if (state == 1)
							{
								if ((oldFinishx != -1&&oldFinishy != -1)&&(oldFinishx != x - 2 || oldFinishy != y - 2))
								{
									mvwaddch(win, oldFinishy, oldFinishx, ACS_BOARD);
									//mvwchgat(win, oldFinishy, oldFinishx, 1, A_NORMAL, 0, 0);
								}
								oldFinishx = x - 2;
								oldFinishy = y - 2;
							}
							else if (state == -1)
							{
								oldFinishx = -1;
								oldFinishy = -1;
							}
								
						}
						else if (cursor == 2)
							state = astar.setWall(j, i);
						
						if (Nmap[i][j].tile == '.')
							mvwaddch(win, y - 2, x - 2, ACS_BOARD);
						else if (state != 0)
							mvwaddch(win, y - 2, x - 2, Nmap[i][j].tile);
						//上色
							if(cursor == 0 &&state == 1)
								mvwchgat(win, y - 2, x - 2, 1, A_NORMAL, 2, 0);
						
						else if(cursor == 1 &&state == 1)
								mvwchgat(win, y - 2, x - 2, 1, A_NORMAL, 3, 0);
						
						
					}
				}
			}
		}
		
		return cursor;
	}
	int GetSel(){return cursor;}
	void display(WINDOW *win) {
		if (runflag)
		{
			cursor = -1, runflag = 0;
			clear(win);
		}
		if (cursor != -1)
		{
			mvwprintw(win, choice[cursor].second + 1, choice[cursor].first, "  ok ");
			mvwchgat(win, choice[cursor].second, choice[cursor].first, 7, A_STANDOUT, 1, NULL);
			mvwchgat(win, choice[cursor].second + 1, choice[cursor].first, 7, A_STANDOUT, 1, NULL);
			if (cursor >= 3)
				runflag = 1;
		}
		
	}
};
void CalcOffset(WINDOW *win) {
	dtypew = mwMax - 9;
	dtypeh = mhMax - 1;
	//	for (int i = 3; i < dtypeh; i++)
	//	{
	//		for (int j = 1; j < dtypew; ++j)
	//			mvwaddch(win, i, j, ACS_BOARD);
	//	
	//	}
		dtypew -= 1;
	dtypeh -= 3;
}

void displayMap(Astar &astar, WINDOW *win)
{
	for (size_t i = 0; i < astar.nodeMap.size(); i++)
	{
		for (size_t j = 0; j < astar.nodeMap[i].size(); j++)
			if (astar.nodeMap[i][j].tile == '.')
				mvwaddch(win, i + 3, j + 1, ACS_BOARD);
			else
				mvwaddch(win, i + 3, j + 1, astar.nodeMap[i][j].tile);
	}
	
}
int kbhit(WINDOW *win)
{
	nodelay(win, TRUE);
	
	int ch = wgetch(win);
	if (ch != ERR)
	{
		ungetch(ch);
		nodelay(win, false);
		return 1;
	}
	else
		return 0;
}

int main()
{
	static char line[512];
	size_t max_size = sizeof(line);

	setlocale(LC_ALL, "");
	
	initscr();
	noecho();
	curs_set(0);
	start_color();
	init_pair(0, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	
	WINDOW *win = DrawWin(wMax, hMax, mwMax, mhMax);
	keypad(win, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	
	SelectBar sbar;
	sbar.add(mwMax - 8, 3);
	sbar.add(mwMax - 8, 5);
	sbar.add(mwMax - 8, 7);
	sbar.add(mwMax - 8, 9); 
	sbar.add(mwMax - 8, 11); 
	sbar.add(mwMax - 8, 13); 
	;
	
	for (size_t i = 0; i < sizeof(Text) / sizeof(*Text); i++)
		mvwprintw(win, (mhMax - 2) / 2 + i, (mwMax - strlen(title) - 12) / 2, Text[i]);
	
	mvwchgat(win, (mhMax - 2) / 2 + 1, (mwMax - strlen(title) - 11) / 2, 15, COLOR_BLACK, 1, NULL);
	wgetch(win);
	DrawWin(wMax, hMax, mwMax, mhMax);
	CalcOffset(win);
	
	Astar astar;
	astar.Assign(dtypew, dtypeh);
	displayMap(astar, win);
	
	string path;
	int cnt = 0;
	int cx = 0, cy = 0,j=0;
	while (1)
	{
		if (kbhit(win))
		{
			int c = wgetch(win);
			if (c == ERR)
			{
				snprintf(line, max_size, "Nothing happened.");
			}
			else if (c == KEY_MOUSE) {
				MEVENT event;
				if (getmouse(&event) == OK && (event.bstate&BUTTON1_CLICKED))
				{
					int selectItem = sbar.Handle(win, event.x, event.y, astar);
					sbar.display(win);
					wrefresh(win);
					switch (selectItem)
					{
					case 3:
						astar.SaveMap();
						
						break;
						
					case 4:
						if (astar.LoadMap()) {
							cx = astar.xStart + 1, cy = astar.yStart + 3;
							if (path.size())
								for (int i = 0; i < path.size() - 1; ++i)
								{
									j = path[i];
									cx += astar.direction[j][0], cy += astar.direction[j][1];
									mvwaddch(win, cy, cx, ACS_BOARD);
									//mvwchgat(win, cy, cx, 1, A_NORMAL, 0, NULL);
								}
							path.clear();
							sbar.oldStartx = astar.xStart+1;
							sbar.oldStarty = astar.yStart + 3;
							sbar.oldFinishx = astar.xFinish + 1;
							sbar.oldFinishy = astar.yFinish + 3;
							displayMap(astar, win);
						}
						break;
					case 5:
						if ((sbar.oldStartx != -1&&sbar.oldStarty != -1)&&(sbar.oldFinishx != -1&&sbar.oldFinishy != -1))
						{
							/* 清空它 */
							cx = astar.xStart + 1, cy = astar.yStart + 3;
							if(path.size())
							for (int i = 0; i < path.size()-1; ++i)
							{
								j = path[i];
								cx += astar.direction[j][0], cy += astar.direction[j][1];
								mvwaddch(win, cy, cx, ACS_BOARD);
								//mvwchgat(win, cy, cx, 1, A_NORMAL, 0, NULL);
							}
							path.clear();
							
							sbar.dispdir = 0;
							astar.clearList();
							astar.pathFind();
							path = astar.GetPath();
							cx = astar.xStart + 1, cy = astar.yStart + 3;
							
							j = path[sbar.dispdir++];
							
							
						}
						break;
						
					}
					//延迟按键显示
					napms(500);
				}
				
			}
		}
		if (sbar.dispdir)
		{
			if (path.size())
			{
				//j = path[sbar.dispdir++];
				cx += astar.direction[j][0], cy += astar.direction[j][1];
				wattron(win, COLOR_PAIR(COLOR_GREEN));
				switch (path[sbar.dispdir])
				{/* ACS_LARROW:←
 ACS_RARROW : →    ACS_DARROW : ↓
ACS_UARROW : ↑
*/
				case 0:
					mvwaddch(win, cy, cx, ACS_UARROW);
					break;
				case 1:
					mvwaddch(win, cy, cx, ACS_DARROW);
					break;
				case 2:
					mvwaddch(win, cy, cx, ACS_LARROW);
					break;
				case 3:
					mvwaddch(win, cy, cx, ACS_RARROW);
					break;
				default:
					break;
				}
				wattroff(win, COLOR_PAIR(COLOR_GREEN));
				j = path[sbar.dispdir++];
				
				// 		nodeMap[yStart][xStart].tile = 'A';
				// 		nodeMap[yFinish][xFinish].tile = 'B';
			}
			if (sbar.dispdir == path.size() )
			sbar.dispdir = 0;
		}
		sbar.display(win);
		wrefresh(win);
	}
	
	endwin();
	return 0;
}
