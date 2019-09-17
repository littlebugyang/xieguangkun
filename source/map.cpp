#include "map.h"
#include "stdlib.h"
#include "stdio.h"
#include "vector"
#include "myLog.h"
#include <algorithm>

#define MAX_LEN_PLUS 6
#define MAX_BRANCH_COUNT 1000
#define DISTANCE_THRESHOLD 6
// self-defined functions
string strupr_xgk(string param)
{
	string copy = param;
	transform(copy.begin(), copy.end(), copy.begin(), ::toupper);
	return copy;
}

string strlwr_xgk(string param)
{
	string copy = param;
	transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
	return copy;
}

void Map::Display()
{
	printf("\n");
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			/*for (int k = 0; k < sco.size(); k++)
			{
			for (int m = 0; m < sco[k].size(); m++)
			{
			if (sco[k][m].x==)
			{

			}
			}
			}*/
			switch (this->map[i][j].type)
			{
			case 0:
				printf("·");
				break;
			case 1:
				printf("×");
				break;
			case 2:
				printf("θ");
				break;
			case 3:
				switch (map[i][j].dir)
				{
				case MOVE_LEFT:
					printf("←");
					break;
				case MOVE_RIGHT:
					printf("→");
					break;
				case MOVE_UP:
					printf("↑");
					break;
				case MOVE_DOWN:
					printf("↓");
					break;
				default:
					printf("滑");
					break;
				}
				break;
			case 4:
				switch (map[i][j].towerType)
				{
				case 0:
					printf("○");
					break;
				case 1:
					printf("○");
					break;
				case 2:
					printf("┼ ");
					break;
				default:
					printf("·");
					break;
				}
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
}

void Map::UpdateMap(LegStartMsg* ls)
{
	if (this->map != NULL)
	{
		for (int i = 0; i < ls->getWidth(); i++)
		{
			delete[] map[i];
		}
		delete[] map;
		this->map = NULL;
	}
	this->width = ls->getWidth();
	this->height = ls->getHeight();
	this->map = new MapItem * [ls->getWidth()];
	for (int i = 0; i < ls->getWidth(); i++)
	{
		map[i] = new MapItem[ls->getHeight()];
		for (int j = 0; j < ls->getHeight(); j++)
		{
			map[i][j].type = 0;
			map[i][j].x = i;
			map[i][j].y = j;
			map[i][j].dir = NO_MOVE;
			map[i][j].name = "";
			map[i][j].towerType = 0;
			map[i][j].frequency = 0;
		}
	}
	for (int i = 0; i < ls->getMeteorsCount(); i++)
	{
		map[ls->getMeteors()[i].x][ls->getMeteors()[i].y].type = 1;
	}
	for (int i = 0; i < ls->getTunnelsCount(); i++)
	{
		map[ls->getTunnels()[i].x][ls->getTunnels()[i].y].type = 3;
		map[ls->getTunnels()[i].x][ls->getTunnels()[i].y].dir = ls->getTunnels()[i].direction;
	}
	for (int i = 0; i < ls->getWormholesCount(); i++)
	{
		map[ls->getWormholes()[i].x][ls->getWormholes()[i].y].type = 2;
		map[ls->getWormholes()[i].x][ls->getWormholes()[i].y].name = ls->getWormholes()[i].name;
		//printf("\n 黑洞 数组：%s \n", map[ls->getWormholes()[i].x][ls->getWormholes()[i].y].name.c_str());
	}
	//for (int i = 0; i < ls->getMeteorsCount(); i++)
	//{
	//	if ((ls->getMeteors()[i].x + 1) < ls->getWidth() && (ls->getMeteors()[i].y + 1) < ls->getHeight() && map[ls->getMeteors()[i].x + 1][ls->getMeteors()[i].y + 1].type == 0)
	//	{
	//		map[ls->getMeteors()[i].x + 1][ls->getMeteors()[i].y + 1].type = 4;
	//	}
	//	if ((ls->getMeteors()[i].x - 1) >= 0 && (ls->getMeteors()[i].y + 1) < ls->getHeight() && map[ls->getMeteors()[i].x - 1][ls->getMeteors()[i].y + 1].type == 0)
	//	{
	//		map[ls->getMeteors()[i].x - 1][ls->getMeteors()[i].y + 1].type = 4;
	//	}
	//	if ((ls->getMeteors()[i].x + 1) < ls->getWidth() && (ls->getMeteors()[i].y - 1) >= 0 && map[ls->getMeteors()[i].x + 1][ls->getMeteors()[i].y - 1].type == 0)
	//	{
	//		map[ls->getMeteors()[i].x + 1][ls->getMeteors()[i].y - 1].type = 4;
	//	}
	//	if ((ls->getMeteors()[i].x - 1) >= 0 && (ls->getMeteors()[i].y - 1) >= 0 && map[ls->getMeteors()[i].x - 1][ls->getMeteors()[i].y - 1].type == 0)
	//	{
	//		map[ls->getMeteors()[i].x - 1][ls->getMeteors()[i].y - 1].type = 4;
	//	}
	//}
	//for (int i = 0; i < ls->getWormholesCount(); i++)
	//{
	//	if (ls->getWormholes()[i].x + 1 < ls->getWidth() && ls->getWormholes()[i].y + 1 < ls->getHeight() && map[ls->getWormholes()[i].x + 1][ls->getWormholes()[i].y + 1].type == 0)
	//	{
	//		map[ls->getWormholes()[i].x + 1][ls->getWormholes()[i].y + 1].type = 4;
	//	}
	//	if (ls->getWormholes()[i].x - 1 >= 0 && ls->getWormholes()[i].y + 1 < ls->getHeight() && map[ls->getWormholes()[i].x - 1][ls->getWormholes()[i].y + 1].type == 0)
	//	{
	//		map[ls->getWormholes()[i].x - 1][ls->getWormholes()[i].y + 1].type = 4;
	//	}
	//	if (ls->getWormholes()[i].x + 1 < ls->getWidth() && ls->getWormholes()[i].y - 1 >= 0 && map[ls->getWormholes()[i].x + 1][ls->getWormholes()[i].y - 1].type == 0)
	//	{
	//		map[ls->getWormholes()[i].x + 1][ls->getWormholes()[i].y - 1].type = 4;
	//	}
	//	if (ls->getWormholes()[i].x - 1 >= 0 && ls->getWormholes()[i].y - 1 >= 0 && map[ls->getWormholes()[i].x - 1][ls->getWormholes()[i].y - 1].type == 0)
	//	{
	//		map[ls->getWormholes()[i].x - 1][ls->getWormholes()[i].y - 1].type = 4;
	//	}
	//}
	this->Display();
	//this->FirstPass();
	//this->Display();
	//this->SecondPass();
	//this->Display();
	//this->ThirdPass();
	//this->Display();
}

void Map::AddFrequency(int x, int y)
{
	//将地图划为3*3的九个区域
	//四角的区域固定为正方形(square)
	//传入坐标x，y
	//为这个坐标所属区域的area数组增加一点访问度
	Point areaIndex = this->getAreaByCoord(x, y);
	++this->areas[areaIndex.x][areaIndex.y].freq;
}

Point Map::getAreaByCoord(int x, int y)
{
	// todo: 只考虑方形地图
	Point areaIndex;
	int squareWidth = 8 - (25 - this->width) / 3;
	int indices[2] = { 0 };
	indices[0] = x;
	indices[1] = y;
	int areaIndices[2] = { 0 };
	for (int i = 0; i < 2; ++i) {
		if (indices[i] < squareWidth) {
			areaIndices[i] = 0;
		}
		else if (indices[i] > this->width - 1 - squareWidth) {
			areaIndices[i] = 2;
		}
		else {
			areaIndices[i] = 1;
		}
	}
	areaIndex.x = areaIndices[0];
	areaIndex.y = areaIndices[1];
	return areaIndex;
}

void Map::printfAreaFre()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			printf("%d     ", areas[j][i].freq);
		printf("\n");
	}
}

DIRECT Map::getMinDirection(int x, int y)
{
	// 传入当前点的坐标，返回距离该点最近的、拥有访问频度最低、非己区域的方向
	// 也就是说返回一个方向，这个方向指向周边访问频度最低的区域

	Point presentAreaPoint = this->getAreaByCoord(x, y);

	int directFreqs[4] = { 0 };

	// 顺时针方向计算；由上方向开始
	DIRECT minDirect = NO_MOVE; // 默认方向
	int minFreq = 0; // 默认频度

	if (presentAreaPoint.x - 1 >= 0) { // 上边还有area
		directFreqs[0] = this->areas[presentAreaPoint.x - 1][presentAreaPoint.y].freq;
		if (directFreqs[0] > minFreq) {
			minFreq = directFreqs[0];
			minDirect = MOVE_UP;
		}
	}
	if (presentAreaPoint.y + 1 <= 2) { // 右边还有area
		directFreqs[1] = this->areas[presentAreaPoint.x][presentAreaPoint.y + 1].freq;
		if (directFreqs[1] > minFreq) {
			minFreq = directFreqs[1];
			minDirect = MOVE_RIGHT;
		}
	}
	if (presentAreaPoint.x + 1 <= 2) { // 下边还有area
		directFreqs[2] = this->areas[presentAreaPoint.x + 1][presentAreaPoint.y].freq;
		if (directFreqs[2] > minFreq) {
			minFreq = directFreqs[2];
			minDirect = MOVE_DOWN;
		}
	}
	if (presentAreaPoint.y - 1 >= 0) { // 左边还有area
		directFreqs[3] = this->areas[presentAreaPoint.x][presentAreaPoint.y - 1].freq;
		if (directFreqs[3] > minFreq) {
			minFreq = directFreqs[3];
			minDirect = MOVE_LEFT;
		}
	}
	if (directFreqs[0] == directFreqs[1] && directFreqs[2] == directFreqs[3] && directFreqs[1] == directFreqs[2]) {
		// 不存在最优值，全为0，或者全部频度相同
		return NO_MOVE;
	}
	return minDirect;
}

bool Map::EatSafety(int x, int y)
{
	int tonglu = 4;
	//上
	if (y == 0)
		tonglu--;
	else if (this->map[x][y - 1].type == 1)
		tonglu--;
	else if (this->map[x][y - 1].type == 3 && this->map[x][y - 1].dir == MOVE_DOWN)
		tonglu--;
	//下
	if (y == height - 1)
		tonglu--;
	else if (this->map[x][y + 1].type == 1)
		tonglu--;
	else if (this->map[x][y + 1].type == 3 && this->map[x][y + 1].dir == MOVE_DOWN)
		tonglu--;
	//左
	if (x == 0)
		tonglu--;
	else if (this->map[x - 1][y].type == 1)
		tonglu--;
	else if (this->map[x - 1][y].type == 3 && this->map[x - 1][y].dir == MOVE_DOWN)
		tonglu--;
	//右
	if (x == width - 1)
		tonglu--;
	else if (this->map[x + 1][y].type == 1)
		tonglu--;
	else if (this->map[x + 1][y].type == 3 && this->map[x + 1][y].dir == MOVE_DOWN)
		tonglu--;

	LOGE("this power has %d path", tonglu);
	if (tonglu >= 3)
		return true;
	else
		return false;
}

bool Map::IsReach(int x, int y)
{
	if (this->map[x][y].type == 1)
		return false;
	if (this->map[x][y].type == 3)
		return false;
	//printf("%d,%d,%d可以到达\n", this->map[x][y].type,this->map[x][y].dir, ContractDir(dir));
	return true;
}

bool Map::DirDelivery(int x, int y, DIRECT dir)
{
	if (dir == MOVE_UP)
	{
		printf("  find dir is %d\n", dir);
		for (int i = y; i >= 0; i--)
		{
			if (this->map[x][y].type == 2)
			{
				printf("  find wormhole\n");
				return true;
			}
		}
		return false;
	}
	else if (dir == MOVE_DOWN)
	{
		printf("  find dir is %d\n", dir);
		for (int i = y; i <= height - 1; i++)
		{
			if (this->map[x][y].type == 2)
			{
				printf("  find wormhole\n");
				return true;
			}
		}
		return false;
	}
	else if (dir == MOVE_LEFT)
	{
		printf("  find dir is %d\n", dir);
		for (int i = x; i >= 0; i--)
		{
			if (this->map[x][y].type == 2)
			{
				printf("  find wormhole\n");
				return true;
			}
		}
		return false;
	}
	else
	{
		printf("  find dir is %d\n", dir);
		for (int i = x; i <= width - 1; i++)
		{
			if (this->map[x][y].type == 2)
			{
				printf("  find wormhole\n");
				return true;
			}
		}
		return false;
	}
}

DIRECT Map::ContractDir(DIRECT dir)
{
	switch (dir)
	{
	case MOVE_UP:
	{
		return MOVE_DOWN;
		break;
	}
	case MOVE_DOWN:
	{
		return  MOVE_UP;
		break;
	}
	case MOVE_LEFT:
	{
		return MOVE_RIGHT;
		break;
	}
	case MOVE_RIGHT:
	{
		return MOVE_LEFT;
		break;
	}
	case NO_MOVE:
	{
		return NO_MOVE;
		break;
	}
	default:
	{
		return NO_MOVE;
		break;
	}
	}
}
//最优路径决策，默认参数为优先扩散方向
DIRECT Map::pathFind(int ourx, int oury, int dstx, int dsty, DIRECT dir)
{

	/*if (ourx == 10 && oury == 2)
	{
		printf("\n aaaaa \n");
	}*/
	int tdx = dstx, tdy = dsty;

	//计算方向顺序
	vector<DIRECT> seq;
	int base = ((int)dir / 2) * 2;
	int baseRev = (base + 2) % 4;
	seq.push_back(dir);
	seq.push_back((DIRECT)(baseRev + dir % 2));
	seq.push_back((DIRECT)(baseRev + (dir + 1) % 2));
	seq.push_back((DIRECT)(base + (dir + 1) % 2));


	if (!this->HasTraffic(ourx, oury, tdx, tdy))
	{

		/*if (ourx == 9 && oury == 5 && dstx == 8 && dsty == 3)
		{
			printf("\n aaaaaaaaaaaaaaaaaa \n");
		}*/
		/*if (ourx != dstx)
		{
			return ourx > dstx ? MOVE_LEFT : MOVE_RIGHT;
		}
		else
		{
			return oury > dsty ? MOVE_UP : MOVE_DOWN;
		}*/

		/*DIRECT tmpres = NO_MOVE;
		DIRECT tmprev = NO_MOVE;*/
		//MapItem tmpTest = this->map[ourx][oury];
		if (ourx == dstx && oury == dsty)
		{
			return NO_MOVE;
		}
		for (int i = 0; i < seq.size(); i++)
		{
			switch (seq[i])
			{
			case MOVE_UP:
				if (oury > dsty)
				{
					return MOVE_UP;
				}
				break;
			case MOVE_DOWN:
				if (oury < dsty)
				{
					return MOVE_DOWN;
				}
				break;
			case MOVE_LEFT:
				if (ourx > dstx)
				{
					return MOVE_LEFT;
				}
				break;
			case MOVE_RIGHT:
				if (ourx < dstx)
				{
					return MOVE_RIGHT;
				}
				break;
			default:
				break;
			}
		}
		/*if (ourx != dstx)
		{
			return ourx > dstx ? MOVE_LEFT : MOVE_RIGHT;
		}
		else
		{
			return oury > dsty ? MOVE_UP : MOVE_DOWN;
		}*/
		//上路的过程中规避反向滑道
		/*switch (tmpres)
		{
		case MOVE_LEFT:
			tmpTest = this->map[tmpTest.x - 1][tmpTest.y];
			tmprev = MOVE_RIGHT;
			break;
		case MOVE_UP:
			tmpTest = this->map[tmpTest.x][tmpTest.y - 1];
			tmprev = MOVE_DOWN;
			break;
		case MOVE_RIGHT:
			tmpTest = this->map[tmpTest.x + 1][tmpTest.y];
			tmprev = MOVE_LEFT;
			break;
		case MOVE_DOWN:
			tmpTest = this->map[tmpTest.x][tmpTest.y + 1];
			tmprev = MOVE_UP;
			break;
		default:
			printf("\n 傻瓜函数出错p1 \n");
			break;
		}
		if (tmpTest.type == 3 && tmpTest.dir == tmprev)
		{
			return (DIRECT)(((int)tmpres + rand() % 3 + 1) % 4);
		}
		else
		{
			return tmpres;
		}*/
	}
	else
	{
		return this->GoDestination(ourx, oury, dstx, dsty, seq);//测试全图扩散
		//dstx = tdx;
		//dsty = tdy;
		//此时矩形区域并非全是可直接到达单位，
		/*if (this->map[ourx][oury].type == 0 || this->map[ourx][oury].type == 2)
		{
			return this->GoNearestTraffic(ourx, oury, dstx, dsty);
		}
		if (this->map[dstx][dsty].type == 0 || this->map[dstx][dsty].type == 2)
		{
			//printf("\n aaaaaaaaaaa \n");
			int tmpx = this->FindNearestTraffic(dstx, dsty).x;
			int tmpy = this->FindNearestTraffic(dstx, dsty).y;
			dstx = tmpx;
			dsty = tmpy;
		}
		if (ourx == 9 && oury == 5 && dstx == 8 && dsty == 3)
		{
			printf("\n cccccccccccccccccccc \n");
		}
		if (dstx==ourx&&dsty==oury)
		{
			return this->GoDestination(ourx, oury, tdx, tdy);
		}
		return this->FindBestRoute(ourx, oury, dstx, dsty);*/
	}



	return NO_MOVE;
}
//对应的全路径计算
vector<DIRECT> Map::fullPathFind(int ourx, int oury, int dstx, int dsty, DIRECT dir)
{
	int tdx = dstx, tdy = dsty;

	//计算方向顺序
	vector<DIRECT> seq;
	int base = ((int)dir / 2) * 2;
	int baseRev = (base + 2) % 4;
	seq.push_back(dir);
	seq.push_back((DIRECT)(baseRev + dir % 2));
	seq.push_back((DIRECT)(baseRev + (dir + 1) % 2));
	seq.push_back((DIRECT)(base + (dir + 1) % 2));

	if (!this->HasTraffic(ourx, oury, tdx, tdy))
	{
		if (ourx == dstx && oury == dsty)
		{
			return vector<DIRECT>(1, NO_MOVE);
		}
		vector<DIRECT> res;
		for (int i = 0; i < seq.size(); i++)
		{
			switch (seq[i])
			{
			case MOVE_UP:
				if (oury > dsty)
				{
					for (int j = 0; j < oury - dsty; j++)
					{
						res.push_back(MOVE_UP);
					}
					//return MOVE_UP;
				}
				break;
			case MOVE_DOWN:
				if (oury < dsty)
				{
					for (int j = 0; j < dsty - oury; j++)
					{
						res.push_back(MOVE_DOWN);
					}
					//return MOVE_DOWN;
				}
				break;
			case MOVE_LEFT:
				if (ourx > dstx)
				{
					for (int j = 0; j < ourx - dstx; j++)
					{
						res.push_back(MOVE_LEFT);
					}
					//return MOVE_LEFT;
				}
				break;
			case MOVE_RIGHT:
				if (ourx < dstx)
				{
					for (int j = 0; j < dstx - ourx; j++)
					{
						res.push_back(MOVE_RIGHT);
					}
					//return MOVE_RIGHT;
				}
				break;
			default:
				break;
			}
		}
		if (res.empty())
		{
			return vector<DIRECT>(1, NO_MOVE);
		}
		return res;
	}
	else
	{
		return this->FullGoDestination(ourx, oury, dstx, dsty, seq);
	}



	return vector<DIRECT>(1, NO_MOVE);
}
//找到对应的虫洞
MapItem Map::FindAnotherHole(MapItem t)
{
	//printf("\n aaaaaa \n");
	string pair = strupr_xgk(t.name);
	if (pair == t.name)
	{
		pair = strlwr_xgk(t.name);
	}
	//printf("\n aaaaaa \n");
	//printf("\n %d,%s \n", t.type, t.name.c_str());
	for (int i = 0; i < this->width; i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			if (this->map[i][j].name == pair)
			{
				return this->map[i][j];
			}
		}
	}
	return t;
}
//找到最近的交通线/点
MapItem Map::FindNearestTraffic(int x, int y)
{
	for (int i = 1; i < (this->width > this->height ? this->width : this->height); i++)
	{
		for (int j = 0; j <= i; j++)
		{
			if (x + j < this->width && y + i - j < this->height && this->map[x + j][y + i - j].type == 4)
			{
				return this->map[x + j][y + i - j];
			}
			if (x - j >= 0 && y + i - j < this->height && this->map[x - j][y + i - j].type == 4)
			{
				return this->map[x - j][y + i - j];
			}
			if (x + j < this->width && y - i + j > 0 && this->map[x + j][y - i + j].type == 4)
			{
				return this->map[x + j][y - i + j];
			}
			if (x - j >= 0 && y - i + j >= 0 && this->map[x - j][y - i + j].type == 4)
			{
				return this->map[x - j][y - i + j];
			}
		}
	}
	return this->map[x][y];
}
//找到这一点最终被传送到的点，默认参数为是否考虑滑道终点的虫洞
MapItem Map::FindTransEnd(MapItem t, bool excHole)
{
	if (excHole)
	{
		if (t.type == 2)
		{
			t = this->FindAnotherHole(t);
		}
	}
	if (t.type == 3)
	{
		while (t.type == 3)
		{
			switch (t.dir)
			{
			case MOVE_LEFT:
				t = this->map[t.x - 1][t.y];
				break;
			case MOVE_UP:
				t = this->map[t.x][t.y - 1];
				break;
			case MOVE_RIGHT:
				t = this->map[t.x + 1][t.y];
				break;
			case MOVE_DOWN:
				t = this->map[t.x][t.y + 1];
				break;
			default:
				printf("寻找最佳路径函数出错p4");
				break;
			}
		}
	}
	//滑道出口也可能是虫洞，所以写在后面
	if (!excHole)
	{
		if (t.type == 2)
		{
			t = this->FindAnotherHole(t);
		}
	}
	return t;
}
//计算沿特定方向走一格后到达的点
MapItem Map::FindMoveEnd(int ourx, int oury, DIRECT move)
{
	switch (move)
	{
	case MOVE_UP:
		if (oury - 1 >= 0 && map[ourx][oury - 1].type != 1)
		{
			return FindTransEnd(map[ourx][oury - 1]);
		}
		break;
	case MOVE_DOWN:
		if (oury + 1 < height && map[ourx][oury + 1].type != 1)
		{
			return FindTransEnd(map[ourx][oury + 1]);
		}
		break;
	case MOVE_LEFT:
		if (ourx - 1 >= 0 && map[ourx - 1][oury].type != 1)
		{
			return FindTransEnd(map[ourx - 1][oury]);
		}
		break;
	case MOVE_RIGHT:
		if (ourx + 1 < width && map[ourx + 1][oury].type != 1)
		{
			return FindTransEnd(map[ourx + 1][oury]);
		}
		break;
	case NO_MOVE:
		break;
	default:
		break;
	}
	return map[ourx][oury];
}
//找到同样在连通交通线上的两点的最佳路线
DIRECT Map::FindBestRoute(int ourx, int oury, int dstx, int dsty)
{
	bool find = false;
	vector<vector<MapItem>> pathRecord;//路径档案
	vector<vector<DIRECT>> dirRecord;//行进方向档案
	vector<vector<DIRECT>> branchRecord;//每个档案最后一个路口的可能分支
	//vector<DIRECT> curBranchRecord;//目前所有路口的的分支选择状态
	vector<MapItem> tmpPath;//当前路径
	vector<DIRECT> tmpDir;//当前行进方向
	vector<DIRECT> tmpBranch;//当前所有可能分支
	vector<int> lens;//所有成功到达终点的路径长度
	vector<vector<DIRECT>> sp;//所有成功到达终点的行进方向
	MapItem cur = this->map[ourx][oury];//当前探索点
	//MapItem lastCur = cur;//当前探索点传送起点
	DIRECT lastDir;//来时的方向
	tmpBranch = this->FindPossibleBranch(ourx, oury, NO_MOVE, abs(dstx - ourx) > DISTANCE_THRESHOLD || abs(dsty - oury) > DISTANCE_THRESHOLD);
	int branchCount = tmpBranch.size();
	if (tmpBranch.empty())
	{
		return NO_MOVE;
	}
	/*for (int i = 0; i < tmpBranch.size(); i++)
	{
		printf("\n %d \n", (int)tmpBranch[i]);
	}*/

	tmpPath.push_back(cur);
	/*else
	{
		//curBranchRecord.push_back(tmpBranch.at(0));
		branchRecord.push_back(tmpBranch);
		tmpPath.push_back(cur);
		pathRecord.push_back(tmpPath);
	}*/
	do
	{

		//printf("\n %d,%d \n", cur.x, cur.y);
		if (lens.size() > 0)
		{
			int min = lens[0];
			for (int i = 0; i < lens.size(); i++)
			{
				if (lens[i] < min)
				{
					min = lens[i];
				}
			}
			if (tmpDir.size() > min)
			{
				//回档
				while (!branchRecord.empty() && branchRecord.back().empty())
				{
					branchRecord.pop_back();
					dirRecord.pop_back();
					pathRecord.pop_back();
				}
				if (branchRecord.empty())
				{
					break;
				}
				tmpBranch.clear();
				tmpBranch.push_back(branchRecord.back()[0]);
				//删除第一个元素
				branchRecord.back().erase(branchRecord.back().begin());
				tmpPath = pathRecord.back();
				tmpDir = dirRecord.back();
				cur = tmpPath.back();
				//lastCur = tmpPath.back();
				//printf(" 超过已有长度回档！！\n");
			}
		}
		else
		{
			if (tmpDir.size() > MAX_LEN_PLUS + (abs(dstx - ourx) + abs(dsty - oury)))
			{
				//回档
				while (!branchRecord.empty() && branchRecord.back().empty())
				{
					branchRecord.pop_back();
					dirRecord.pop_back();
					pathRecord.pop_back();
				}
				if (branchRecord.empty())
				{
					break;
				}
				tmpBranch.clear();
				tmpBranch.push_back(branchRecord.back()[0]);
				//删除第一个元素
				branchRecord.back().erase(branchRecord.back().begin());
				tmpPath = pathRecord.back();
				tmpDir = dirRecord.back();
				cur = tmpPath.back();
				//lastCur = tmpPath.back();
				//printf(" 超过可容忍最大长度回档！！\n");
			}
		}


		//printf("\n 成功路径条数：%d \n", lens.size());
		//printf("\n 探索到新地区 \n");
		//如果到达目标点
		if (cur.x == dstx && cur.y == dsty)
		{
			lens.push_back(tmpDir.size());
			sp.push_back(tmpDir);
			//回档
			while (!branchRecord.empty() && branchRecord.back().empty())
			{
				branchRecord.pop_back();
				dirRecord.pop_back();
				pathRecord.pop_back();
			}
			if (branchRecord.empty())
			{
				break;
			}
			tmpBranch.clear();
			tmpBranch.push_back(branchRecord.back()[0]);
			//删除第一个元素
			branchRecord.back().erase(branchRecord.back().begin());
			tmpPath = pathRecord.back();
			tmpDir = dirRecord.back();
			cur = tmpPath.back();
			//lastCur = tmpPath.back();
			//printf("目标点回档！！\n");
		}

		//如果无路可走
		if (tmpBranch.empty())
		{
			//回档
			while (!branchRecord.empty() && branchRecord.back().empty())
			{
				branchRecord.pop_back();
				dirRecord.pop_back();
				pathRecord.pop_back();
			}
			if (branchRecord.empty())
			{
				break;
			}
			tmpBranch.clear();
			tmpBranch.push_back(branchRecord.back()[0]);
			//删除第一个元素
			branchRecord.back().erase(branchRecord.back().begin());
			tmpPath = pathRecord.back();
			tmpDir = dirRecord.back();
			cur = tmpPath.back();
			//lastCur = tmpPath.back();
			//printf("死路回档！！\n");
		}

		bool isBack = false;
		//走到走过的地方时回档
		for (int i = 0; i < tmpPath.size() - 1; i++)
		{
			if (tmpPath[i].x == cur.x && tmpPath[i].y == cur.y)
			{
				//printf("\n aaaaaa \n");
				//回档
				while (!branchRecord.empty() && branchRecord.back().empty())
				{
					branchRecord.pop_back();
					dirRecord.pop_back();
					pathRecord.pop_back();
				}
				if (branchRecord.empty())
				{
					isBack = true;
					break;
				}
				tmpBranch.clear();
				tmpBranch.push_back(branchRecord.back()[0]);
				//删除第一个元素
				branchRecord.back().erase(branchRecord.back().begin());
				tmpPath = pathRecord.back();
				tmpDir = dirRecord.back();
				cur = tmpPath.back();
				//lastCur = tmpPath.back();
				//printf("循环回档！！\n");
				break;
			}
		}
		if (isBack && branchRecord.empty())
		{
			break;
		}

		if (tmpBranch.size() > 1)//可走分支超过一条
		{
			switch (tmpBranch[0])
			{
			case MOVE_LEFT:
				cur = this->map[cur.x - 1][cur.y];
				break;
			case MOVE_UP:
				cur = this->map[cur.x][cur.y - 1];
				break;
			case MOVE_RIGHT:
				cur = this->map[cur.x + 1][cur.y];
				break;
			case MOVE_DOWN:
				cur = this->map[cur.x][cur.y + 1];
				break;
			default:
				printf("寻找最佳路径函数出错p1");
				break;
			}
			vector<DIRECT> t = tmpBranch;
			pathRecord.push_back(tmpPath);
			dirRecord.push_back(tmpDir);
			branchRecord.push_back(t);
			//删除第一个元素
			branchRecord.back().erase(branchRecord.back().begin());
			//printf("建档！！\n");
		}
		else if (tmpBranch.size() == 1)
		{
			switch (tmpBranch[0])
			{
			case MOVE_LEFT:
				cur = this->map[cur.x - 1][cur.y];
				break;
			case MOVE_UP:
				cur = this->map[cur.x][cur.y - 1];
				break;
			case MOVE_RIGHT:
				cur = this->map[cur.x + 1][cur.y];
				break;
			case MOVE_DOWN:
				cur = this->map[cur.x][cur.y + 1];
				break;
			default:
				printf("寻找最佳路径函数出错p2");
				break;
			}
		}
		else
		{
			printf("寻找最佳路径函数出错p3");
		}


		//lastDir = tmpBranch[0];
		//来时的相反方向
		switch (tmpBranch[0])
		{
		case MOVE_LEFT:
			lastDir = MOVE_RIGHT;
			break;
		case MOVE_UP:
			lastDir = MOVE_DOWN;
			break;
		case MOVE_RIGHT:
			lastDir = MOVE_LEFT;
			break;
		case MOVE_DOWN:
			lastDir = MOVE_UP;
			break;
		default:
			printf("寻找最佳路径函数出错p5");
			break;
		}
		//tmpBranch = FindPossibleBranch(cur.x, cur.y, lastDir);
		tmpDir.push_back(tmpBranch[0]);//存入当前方向

		//lastCur = cur;

		if (cur.type == 3)
		{
			lastDir = NO_MOVE;
			while (cur.type == 3)
			{
				switch (cur.dir)
				{
				case MOVE_LEFT:
					cur = this->map[cur.x - 1][cur.y];
					break;
				case MOVE_UP:
					cur = this->map[cur.x][cur.y - 1];
					break;
				case MOVE_RIGHT:
					cur = this->map[cur.x + 1][cur.y];
					break;
				case MOVE_DOWN:
					cur = this->map[cur.x][cur.y + 1];
					break;
				default:
					printf("寻找最佳路径函数出错p4");
					break;
				}
			}
		}
		//滑道出口也可能是虫洞，所以写在后面
		if (cur.type == 2)
		{
			lastDir = NO_MOVE;
			cur = this->FindAnotherHole(cur);
		}
		tmpPath.push_back(cur);
		tmpBranch = FindPossibleBranch(cur.x, cur.y, lastDir, abs(dstx - ourx) > DISTANCE_THRESHOLD || abs(dsty - oury) > DISTANCE_THRESHOLD);
		branchCount += tmpBranch.size();
	} while (branchCount < MAX_BRANCH_COUNT);

	//根据最佳路径决定方向
	if (lens.empty())
	{
		//printf("\n %d,%d \n", cur.x, cur.y);
		return NO_MOVE;
	}
	else
	{
		int minidx = 0;
		int min = lens[0];
		for (int i = 0; i < lens.size(); i++)
		{
			if (lens[i] < min)
			{
				minidx = i;
				min = lens[i];
			}
		}
		/*if (sp[minidx][1].x!=ourx)
		{
			if (sp[minidx][1].x > ourx)
			{
				return MOVE_RIGHT;
			}
			else
			{
				return MOVE_LEFT;
			}
		}
		else
		{
			if (sp[minidx][1].y > oury)
			{
				return MOVE_DOWN;
			}
			else
			{
				return MOVE_UP;
			}
		}*/
		return sp[minidx][0];
	}
	return NO_MOVE;
}
//去最近的交通单位（扩散算法）
DIRECT Map::GoNearestTraffic(int ourx, int oury, int dstx, int dsty)
{
	//printf("\n aaaa \n");
	vector<MapItem> save;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	/*for (int i = 1; i <= 6; i++)
	{
		tmpSave.clear();
		tmpSave.push_back(this->map[ourx][oury]);
		ttmpSave.clear();
		ttmpSave.push_back(this->map[ourx][oury]);
		ttmpDir.clear();
		ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));*/
	for (int j = 0; j < NEAREST_THRESHOLD; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				//printf("\n %d,%d,%d \n", tmp.x, tmp.y, tmpSave.size());
				if (!isRep)
				{


					if (tmp.type == 4 || (tmp.x == dstx && tmp.y == dsty))
					{
						vector<DIRECT> t = ttmpDir[k];
						t.push_back((DIRECT)l);
						return t[1];
					}
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					tttmpDir.push_back(t);

					//printf("\n %d,%d,%d \n", tmp.x, tmp.y,l);
				}
				isRep = false;
				/*for (int m = 0; m < save.size(); m++)
				{
					if (save[m].x == tmp.x || save[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				if (!isRep)
				{

					save.push_back(tmp);
				}*/
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
	}
	//}
	return NO_MOVE;
}
//直接探寻到达目标点的路（扩散算法），阈值是NEAREST_THRESHOLD
DIRECT Map::GoDestination(int ourx, int oury, int dstx, int dsty, vector<DIRECT> seq)
{
	if (ourx == dstx && oury == dsty)
	{
		return NO_MOVE;
	}
	//printf("\n aaaa \n");
	vector<MapItem> save;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	/*for (int i = 1; i <= 6; i++)
	{
	tmpSave.clear();
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.clear();
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.clear();
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));*/
	//LOGE("\n  %d,%d,%d,%d \n", ourx, oury, dstx, dsty);
	for (int j = 0; j < NEAREST_THRESHOLD; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < seq.size(); l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (seq[l])
				{
				case MOVE_UP:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case MOVE_DOWN:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case MOVE_LEFT:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case MOVE_RIGHT:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}



				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				//printf("\n %d,%d,%d \n", tmp.x, tmp.y, tmpSave.size());
				if (!isRep)
				{

					//LOGE("\n  %d,%d \n", tmp.x, tmp.y);
					if (tmp.x == dstx && tmp.y == dsty)
					{
						vector<DIRECT> t = ttmpDir[k];
						t.push_back(seq[l]);
						return t[1];
					}
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back(seq[l]);
					tttmpDir.push_back(t);

					//printf("\n %d,%d,%d \n", tmp.x, tmp.y,l);
				}
				isRep = false;
				/*for (int m = 0; m < save.size(); m++)
				{
				if (save[m].x == tmp.x || save[m].y == tmp.y)
				{
				isRep = true;
				}
				}
				if (!isRep)
				{

				save.push_back(tmp);
				}*/
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
	}
	//}
	return NO_MOVE;
}
//对应的全路径计算
vector<DIRECT> Map::FullGoDestination(int ourx, int oury, int dstx, int dsty, vector<DIRECT> seq)
{
	if (ourx == dstx && oury == dsty)
	{
		return vector<DIRECT>(1, NO_MOVE);
	}
	//printf("\n aaaa \n");
	vector<MapItem> save;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	/*for (int i = 1; i <= 6; i++)
	{
	tmpSave.clear();
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.clear();
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.clear();
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));*/
	//LOGE("\n  %d,%d,%d,%d \n", ourx, oury, dstx, dsty);
	for (int j = 0; j < NEAREST_THRESHOLD; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < seq.size(); l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (seq[l])
				{
				case MOVE_UP:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case MOVE_DOWN:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case MOVE_LEFT:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case MOVE_RIGHT:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}



				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				//printf("\n %d,%d,%d \n", tmp.x, tmp.y, tmpSave.size());
				if (!isRep)
				{

					//LOGE("\n  %d,%d \n", tmp.x, tmp.y);
					if (tmp.x == dstx && tmp.y == dsty)
					{
						vector<DIRECT> t = ttmpDir[k];
						t.push_back(seq[l]);
						t.erase(t.begin());
						return t;
					}
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back(seq[l]);
					tttmpDir.push_back(t);

					//printf("\n %d,%d,%d \n", tmp.x, tmp.y,l);
				}
				isRep = false;
				/*for (int m = 0; m < save.size(); m++)
				{
				if (save[m].x == tmp.x || save[m].y == tmp.y)
				{
				isRep = true;
				}
				}
				if (!isRep)
				{

				save.push_back(tmp);
				}*/
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
	}
	//}
	return vector<DIRECT>(1, NO_MOVE);
}
//计算两点之间真实距离
int Map::FindRealDistance(int ourx, int oury, int dstx, int dsty, int threshold)
{
	if (ourx == dstx && oury == dsty)
	{
		return 0;
	}
	//printf("\n aaaa \n");
	vector<MapItem> save;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	/*for (int i = 1; i <= 6; i++)
	{
	tmpSave.clear();
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.clear();
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.clear();
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));*/
	for (int j = 0; j < threshold; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				//printf("\n %d,%d,%d \n", tmp.x, tmp.y, tmpSave.size());
				if (!isRep)
				{


					if (tmp.x == dstx && tmp.y == dsty)
					{
						vector<DIRECT> t = ttmpDir[k];
						t.push_back((DIRECT)l);
						return (t.size() - 1);
					}
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					tttmpDir.push_back(t);

					//printf("\n %d,%d,%d \n", tmp.x, tmp.y,l);
				}
				isRep = false;
				/*for (int m = 0; m < save.size(); m++)
				{
				if (save[m].x == tmp.x || save[m].y == tmp.y)
				{
				isRep = true;
				}
				}
				if (!isRep)
				{

				save.push_back(tmp);
				}*/
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
	}
	//}
	return threshold + 1;
}
//返回地图向量中距离最近的一/多个点的编号·距离点对
vector<Point> Map::FindBatchMinDistance(int ourx, int oury, vector<MapItem> neibor, int threshold)
{
	/*if (ourx == dstx&&oury == dsty)
	{
		return 0;
	}*/
	for (int i = 0; i < neibor.size(); i++)
	{
		if (ourx == neibor[i].x && oury == neibor[i].y)
		{
			Point t = { i,0 };
			return vector<Point>(1, t);
		}
	}
	vector<Point> res;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	for (int j = 0; j < threshold; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		bool isReach = false;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				if (!isRep)
				{
					for (int m = 0; m < neibor.size(); m++)
					{
						if (tmp.x == neibor[m].x && tmp.y == neibor[m].y)
						{
							isReach = true;
							vector<DIRECT> t = ttmpDir[k];
							t.push_back((DIRECT)l);
							Point tp = { m,t.size() - 1 };
							res.push_back(tp);
						}
					}

					/*if (tmp.x == dstx&&tmp.y == dsty)
					{
						vector<DIRECT> t = ttmpDir[k];
						t.push_back((DIRECT)l);
						return (t.size() - 1);
					}*/
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					tttmpDir.push_back(t);
				}
				isRep = false;
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
		if (isReach)
		{
			break;
		}
	}
	return res;
}
//根据步数计算活动范围的外围一圈
vector<MapItem> Map::getPeripheral(int ourx, int oury, int step)
{
	//printf("\n aaaa \n");
	vector<MapItem> save;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	/*for (int i = 1; i <= 6; i++)
	{
	tmpSave.clear();
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.clear();
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.clear();
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));*/
	for (int j = 0; j < step; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				//printf("\n %d,%d,%d \n", tmp.x, tmp.y, tmpSave.size());
				if (!isRep)
				{


					/*if (tmp.x == dstx&&tmp.y == dsty)
					{
						vector<DIRECT> t = ttmpDir[k];
						t.push_back((DIRECT)l);
						return t[1];
					}*/
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					tttmpDir.push_back(t);

					//printf("\n %d,%d,%d \n", tmp.x, tmp.y,l);
				}
				isRep = false;
				/*for (int m = 0; m < save.size(); m++)
				{
				if (save[m].x == tmp.x || save[m].y == tmp.y)
				{
				isRep = true;
				}
				}
				if (!isRep)
				{

				save.push_back(tmp);
				}*/
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
	}
	//}
	return ttmpSave;
}
//根据步数计算全部活动范围
vector<MapItem> Map::getScope(int ourx, int oury, int step)
{
	//printf("\n aaaa \n");
	vector<MapItem> save;
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	//save.push_back(this->map[ourx][oury]);
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	/*for (int i = 1; i <= 6; i++)
	{
	tmpSave.clear();
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.clear();
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.clear();
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));*/
	for (int j = 0; j < step; j++)
	{


		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				//printf("\n %d,%d,%d \n", tmp.x, tmp.y, tmpSave.size());
				if (!isRep)
				{


					/*if (tmp.x == dstx&&tmp.y == dsty)
					{
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					return t[1];
					}*/
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					tttmpDir.push_back(t);

					//printf("\n %d,%d,%d \n", tmp.x, tmp.y,l);
				}
				isRep = false;
				/*for (int m = 0; m < save.size(); m++)
				{
				if (save[m].x == tmp.x || save[m].y == tmp.y)
				{
				isRep = true;
				}
				}
				if (!isRep)
				{

				save.push_back(tmp);
				}*/
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
	}
	//}
	return tmpSave;
}
//展示扩散范围
void Map::DisplayScope(int ourx, int oury, int step)
{
	vector<vector<MapItem>> sco;
	for (int i = 1; i <= step; i++)
	{
		sco.push_back(getScope(ourx, oury, i));
	}
	printf("\n");
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			bool isSco = false;
			for (int k = 0; k < sco.size(); k++)
			{
				for (int m = 0; m < sco[k].size(); m++)
				{
					if (sco[k][m].x == i && sco[k][m].y == j)
					{
						printf("%d", k + 1);
						isSco = true;
						break;
					}
				}
				if (isSco)break;
			}
			if (isSco)break;
			switch (this->map[i][j].type)
			{
			case 0:
				printf("·");
				break;
			case 1:
				printf("×");
				break;
			case 2:
				printf("θ");
				break;
			case 3:
				switch (map[i][j].dir)
				{
				case MOVE_LEFT:
					printf("←");
					break;
				case MOVE_RIGHT:
					printf("→");
					break;
				case MOVE_UP:
					printf("↑");
					break;
				case MOVE_DOWN:
					printf("↓");
					break;
				default:
					printf("滑");
					break;
				}
				break;
			case 4:
				switch (map[i][j].towerType)
				{
				case 0:
					printf("○");
					break;
				case 1:
					printf("○");
					break;
				case 2:
					printf("┼ ");
					break;
				default:
					printf("·");
					break;
				}
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
}
//按照上下左右顺序输出四个压迫点(可能相同)
vector<MapItem> Map::getPressurePoint(int ourx, int oury, int step)
{
	vector<MapItem> per = getPeripheral(ourx, oury, step);
	if (per.empty())
	{
		return vector<MapItem>();
	}
	int minx = width - 1, miny = height - 1, maxx = 0, maxy = 0;
	vector<MapItem> minxi, minyi, maxxi, maxyi;
	for (int i = 0; i < per.size(); i++)
	{
		if (minx >= per[i].x)
		{
			if (minx > per[i].x)
			{
				minxi.clear();
			}
			minxi.push_back(per[i]);
		}
		if (miny >= per[i].y)
		{
			if (miny > per[i].y)
			{
				minyi.clear();
			}
			minyi.push_back(per[i]);
		}
		if (maxx <= per[i].x)
		{
			if (maxx < per[i].x)
			{
				maxxi.clear();
			}
			maxxi.push_back(per[i]);
		}
		if (maxy <= per[i].y)
		{
			if (maxy < per[i].y)
			{
				maxyi.clear();
			}
			maxyi.push_back(per[i]);
		}
		minx = minx > per[i].x ? per[i].x : minx;
		miny = miny > per[i].y ? per[i].y : miny;
		maxx = maxx < per[i].x ? per[i].x : maxx;
		maxy = maxy < per[i].y ? per[i].y : maxy;
	}
	vector<MapItem> res;
	res.push_back(minyi[0]);
	res.push_back(maxyi[0]);
	res.push_back(minxi[0]);
	res.push_back(maxxi[0]);
	return res;
}
//轮流扩散围捕敌人
vector<MapItem> Map::RotatingDiffusion(vector<Player> ours, Player ene, int threshold)
{
	//vector<MapItem> save;
	//我方占领区域
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	//敌方占领区域
	vector<MapItem> tenSave;
	vector<MapItem> ttenSave;

	for (int i = 0; i < ours.size(); i++)
	{
		tmpSave.push_back(this->map[ours[i].x][ours[i].y]);
		ttmpSave.push_back(this->map[ours[i].x][ours[i].y]);
	}
	tenSave.push_back(this->map[ene.x][ene.y]);
	ttenSave.push_back(this->map[ene.x][ene.y]);


	vector<MapItem> tttenSave;
	MapItem tmp;

	//敌方扩散
	for (int k = 0; k < ttenSave.size(); k++)
	{

		for (int l = 0; l < 4; l++)
		{
			tmp = ttenSave[k];
			bool touch = false;
			switch (l)
			{
			case 0:
				if (tmp.y - 1 < 0)
				{
					touch = true;
				}
				tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
				break;
			case 1:
				if (tmp.y + 1 >= this->height)
				{
					touch = true;
				}
				tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
				break;
			case 2:
				if (tmp.x - 1 < 0)
				{
					touch = true;
				}
				tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
				break;
			case 3:
				if (tmp.x + 1 >= this->width)
				{
					touch = true;
				}
				tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
				break;
			default:
				break;
			}
			if (!touch)//防止触边的时候依旧计算虫洞传送
			{
				switch (tmp.type)
				{
				case 1:
					tmp = ttenSave[k];
					break;
				case 2:
					tmp = this->FindAnotherHole(tmp);
					break;
				case 3:
					tmp = this->FindTransEnd(tmp, true);
					if (!(tmp.x == ttenSave[k].x && tmp.y == ttenSave[k].y))//防止传送回原点的时候依旧计算虫洞
					{
						tmp = this->FindTransEnd(tmp);
					}
					break;
				default:
					break;
				}
			}
			bool isRep = false;
			for (int m = 0; m < tmpSave.size(); m++)
			{
				if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
				{
					isRep = true;
				}
			}
			for (int m = 0; m < tenSave.size(); m++)
			{
				if (tenSave[m].x == tmp.x && tenSave[m].y == tmp.y)
				{
					isRep = true;
				}
			}
			if (!isRep)
			{
				tenSave.push_back(tmp);
				tttenSave.push_back(tmp);
			}
			isRep = false;
		}

	}
	ttenSave.clear();
	ttenSave = tttenSave;


	for (int j = 0; j < threshold; j++)
	{
		vector<MapItem> tttmpSave;
		vector<MapItem> tttenSave;
		MapItem tmp;

		//我方扩散
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				for (int m = 0; m < tenSave.size(); m++)
				{
					if (tenSave[m].x == tmp.x && tenSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				if (!isRep)
				{
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
				}
				isRep = false;
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;

		//敌方扩散
		for (int k = 0; k < ttenSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttenSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttenSave[k];
						break;
					case 2:
						tmp = this->FindAnotherHole(tmp);
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttenSave[k].x && tmp.y == ttenSave[k].y))//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				for (int m = 0; m < tenSave.size(); m++)
				{
					if (tenSave[m].x == tmp.x && tenSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				if (!isRep)
				{
					tenSave.push_back(tmp);
					tttenSave.push_back(tmp);
				}
				isRep = false;
			}

		}
		ttenSave.clear();
		ttenSave = tttenSave;
		if (j == threshold - 1 && ttenSave.empty())
		{
			LOGE("我们将其包围了, 敌方占领区域是: ");
			for (vector<MapItem>::iterator it = tenSave.begin(); it != tenSave.end(); ++it) {
				LOGE("占领区域: (%d, %d)", (*it).x, (*it).y);
			}
			return tenSave;
		}
	}
	LOGE("我们还不能将其包围");
	return vector<MapItem>();
}
//取得按照一定方向排序后的范围，结构体中的towertype代表此点与原点的真实距离
vector<MapItem> Map::SortScope(int ourx, int oury, DIRECT dir, int threshold, bool incHole)
{
	if (ourx < 0 || ourx >19 || oury < 0 || oury>19) {
		LOGE("卧槽!!!(%d, %d)", ourx, oury);
	}
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	vector<vector<DIRECT>> ttmpDir;
	vector<vector<MapItem>> allPhe;
	tmpSave.push_back(this->map[ourx][oury]);
	ttmpSave.push_back(this->map[ourx][oury]);
	ttmpDir.push_back(vector<DIRECT>(1, NO_MOVE));
	allPhe.push_back(ttmpSave);
	int minx = tmpSave[0].x;
	int miny = tmpSave[0].y;
	int maxx = tmpSave[0].x;
	int maxy = tmpSave[0].y;
	for (int j = 0; j < threshold; j++)
	{
		vector<MapItem> tttmpSave;
		vector<vector<DIRECT>> tttmpDir;
		MapItem tmp;
		for (int k = 0; k < ttmpSave.size(); k++)
		{

			for (int l = 0; l < 4; l++)
			{
				tmp = ttmpSave[k];
				bool touch = false;
				switch (l)
				{
				case 0:
					if (tmp.y - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y - 1 >= 0 ? (tmp.y - 1) : 0];
					break;
				case 1:
					if (tmp.y + 1 >= this->height)
					{
						touch = true;
					}
					tmp = this->map[tmp.x][tmp.y + 1 < this->height ? (tmp.y + 1) : (this->height - 1)];
					break;
				case 2:
					if (tmp.x - 1 < 0)
					{
						touch = true;
					}
					tmp = this->map[tmp.x - 1 >= 0 ? (tmp.x - 1) : 0][tmp.y];
					break;
				case 3:
					if (tmp.x + 1 >= this->width)
					{
						touch = true;
					}
					tmp = this->map[tmp.x + 1 < this->width ? (tmp.x + 1) : (this->width - 1)][tmp.y];
					break;
				default:
					break;
				}
				if (!touch)//防止触边的时候依旧计算虫洞传送
				{
					switch (tmp.type)
					{
					case 1:
						tmp = ttmpSave[k];
						break;
					case 2:
						if (incHole)
						{
							tmp = this->FindAnotherHole(tmp);
						}
						else
						{
							tmp = ttmpSave[k];
						}
						break;
					case 3:
						tmp = this->FindTransEnd(tmp, true);
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y) && incHole)//防止传送回原点的时候依旧计算虫洞
						{
							tmp = this->FindTransEnd(tmp);
						}
						break;
					default:
						break;
					}
				}
				bool isRep = false;
				for (int m = 0; m < tmpSave.size(); m++)
				{
					if (tmpSave[m].x == tmp.x && tmpSave[m].y == tmp.y)
					{
						isRep = true;
					}
				}
				if (!isRep)
				{
					minx = minx > tmp.x ? tmp.x : minx;
					miny = miny > tmp.y ? tmp.y : miny;
					maxx = maxx < tmp.x ? tmp.x : maxx;
					maxy = maxy < tmp.y ? tmp.y : maxy;
					tmp.towerType = j + 1;
					tmpSave.push_back(tmp);
					tttmpSave.push_back(tmp);
					vector<DIRECT> t = ttmpDir[k];
					t.push_back((DIRECT)l);
					tttmpDir.push_back(t);

				}
				isRep = false;
			}

		}
		ttmpSave.clear();
		ttmpSave = tttmpSave;
		ttmpDir.clear();
		ttmpDir = tttmpDir;
		allPhe.push_back(ttmpSave);
	}

	vector<MapItem> res;

	//排序: 降序
	switch (dir)
	{
	case MOVE_UP:
		for (int i = maxy; i >= miny; i--)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].y == i)
					{
						res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case MOVE_DOWN:
		for (int i = miny; i <= maxy; i++)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].y == i)
					{
						res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case MOVE_LEFT:
		for (int i = maxx; i >= minx; i--)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].x == i)
					{
						res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case MOVE_RIGHT:
		for (int i = minx; i <= maxx; i++)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].x == i)
					{
						res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case NO_MOVE:
		break;
	default:
		break;
	}

	// 排序: 升序
	switch (dir)
	{
	case MOVE_UP:
		for (int i = maxy; i >= miny; i--)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].y == i)
					{
						res.insert(res.begin(), allPhe[j][k]);
						//res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case MOVE_DOWN:
		for (int i = miny; i <= maxy; i++)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].y == i)
					{
						res.insert(res.begin(), allPhe[j][k]);
						//res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case MOVE_LEFT:
		for (int i = maxx; i >= minx; i--)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].x == i)
					{
						res.insert(res.begin(), allPhe[j][k]);
						//res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case MOVE_RIGHT:
		for (int i = minx; i <= maxx; i++)
		{
			for (int j = allPhe.size() - 1; j >= 0; j--)
			{
				for (int k = 0; k < allPhe[j].size(); k++)
				{
					if (allPhe[j][k].x == i)
					{
						res.insert(res.begin(), allPhe[j][k]);
						//res.push_back(allPhe[j][k]);
					}
				}
			}
		}
		break;
	case NO_MOVE:
		break;
	default:
		break;
	}
	return res;
}

void Map::setAreaDangerous(int x, int y)
{
	Area temp = this->areas[x][y];
	temp.dangerous = true;
	this->setArea(x, y, temp);
}

void Map::setAreaSafe(int x, int y)
{
	Area temp = this->areas[x][y];
	temp.dangerous = false;
	this->setArea(x, y, temp);
}

void Map::occupyArea(int x, int y)
{
	Area temp = this->areas[x][y];
	temp.occupied = true;
	this->setArea(x, y, temp);
}

void Map::deoccupyArea(int x, int y)
{
	Area temp = this->areas[x][y];
	temp.occupied = false;
	this->setArea(x, y, temp);
}

void Map::setArea(int x, int y, Area area)
{
	if (x < 0 || x >2 || y < 0 || y>2) {
		LOGE("卧槽!!!(%d, %d)", x, y);
	}
	this->areas[x][y] = area;
}

Area Map::getArea(int x, int y)
{
	if (x < 0 || x >2 || y < 0 || y>2) {
		LOGE("卧槽!!!(%d, %d)", x, y);
	}
	return this->areas[x][y];
}

void Map::resetArea()
{
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			Area area;
			area.x = i;
			area.y = j;
			area.dangerous = false;
			area.occupied = false;
			this->setArea(i, j, area);
		}
	}
}

vector<DIRECT> Map::FindPossibleBranch(int ourx, int oury, DIRECT except, bool isFar)
{
	vector<DIRECT> tmpBranch;//当前（路口或者路径上最近经过的路口）所有可能分支
	DIRECT seq[4] = { NO_MOVE,NO_MOVE, NO_MOVE, NO_MOVE };
	for (int i = 0; i < 4; i++)
	{
		do
		{
			bool rep = false;
			int ran = rand() % 4;
			for (int j = 0; j < i; j++)
			{
				if (ran == (int)seq[j])
				{
					rep = true;
				}
			}
			if (!rep)
			{
				seq[i] = (DIRECT)ran;
			}
			else
			{
				seq[i] = NO_MOVE;
			}
		} while (seq[i] == NO_MOVE);
	}
	for (int i = 0; i < 4; i++)
	{
		switch (seq[i])
		{
		case MOVE_LEFT:
			if (except != MOVE_LEFT && ourx - 1 >= 0 && (this->map[ourx - 1][oury].type == 4 || this->map[ourx - 1][oury].type == 2 || (this->map[ourx - 1][oury].type == 3 && this->map[ourx - 1][oury].dir != MOVE_RIGHT)))
			{
				bool left = false;
				bool right = false;
				MapItem tmp = this->map[ourx - 1][oury];
				while (tmp.x - 1 >= 0 && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x - 1][tmp.y];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					left = true;
				}
				tmp = this->map[ourx - 1][oury];
				while (tmp.x + 1 < this->width && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x + 1][tmp.y];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					right = true;
				}
				if (!isFar || (left && right) || !(this->map[ourx - 1][oury].type == 4 && this->map[ourx - 1][oury].towerType == 2))
				{
					tmpBranch.push_back(MOVE_LEFT);
				}

			}
			break;
		case MOVE_UP:
			if (except != MOVE_UP && oury - 1 >= 0 && (this->map[ourx][oury - 1].type == 4 || this->map[ourx][oury - 1].type == 2 || (this->map[ourx][oury - 1].type == 3 && this->map[ourx][oury - 1].dir != MOVE_DOWN)))
			{
				bool up = false;
				bool down = false;
				MapItem tmp = this->map[ourx][oury - 1];
				while (tmp.y - 1 >= 0 && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x][tmp.y - 1];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					up = true;
				}
				tmp = this->map[ourx][oury - 1];
				while (tmp.y + 1 < this->height && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x][tmp.y + 1];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					down = true;
				}
				if (!isFar || (up && down) || !(this->map[ourx][oury - 1].type == 4 && this->map[ourx][oury - 1].towerType == 2))
				{
					tmpBranch.push_back(MOVE_UP);
				}
			}
			break;
		case MOVE_RIGHT:
			if (except != MOVE_RIGHT && ourx + 1 < this->width && (this->map[ourx + 1][oury].type == 4 || this->map[ourx + 1][oury].type == 2 || (this->map[ourx + 1][oury].type == 3 && this->map[ourx + 1][oury].dir != MOVE_LEFT)))
			{
				bool left = false;
				bool right = false;
				MapItem tmp = this->map[ourx + 1][oury];
				while (tmp.x - 1 >= 0 && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x - 1][tmp.y];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					left = true;
				}
				tmp = this->map[ourx + 1][oury];
				while (tmp.x + 1 < this->width && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x + 1][tmp.y];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					right = true;
				}
				if (!isFar || (left && right) || !(this->map[ourx + 1][oury].type == 4 && this->map[ourx + 1][oury].towerType == 2))
				{
					tmpBranch.push_back(MOVE_RIGHT);
				}

			}
			break;
		case MOVE_DOWN:
			if (except != MOVE_DOWN && oury + 1 <= this->height && (this->map[ourx][oury + 1].type == 4 || this->map[ourx][oury + 1].type == 2 || (this->map[ourx][oury + 1].type == 3 && this->map[ourx][oury + 1].dir != MOVE_UP)))
			{
				bool up = false;
				bool down = false;
				MapItem tmp = this->map[ourx][oury + 1];
				while (tmp.y - 1 >= 0 && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x][tmp.y - 1];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					up = true;
				}
				tmp = this->map[ourx][oury + 1];
				while (tmp.y + 1 < this->height && tmp.type == 4 && tmp.towerType == 2)
				{
					tmp = this->map[tmp.x][tmp.y + 1];
				}
				if (tmp.type == 4 && tmp.towerType < 2 || tmp.type == 2)
				{
					down = true;
				}
				if (!isFar || (up && down) || !(this->map[ourx][oury + 1].type == 4 && this->map[ourx][oury + 1].towerType == 2))
				{
					tmpBranch.push_back(MOVE_DOWN);
				}

			}
			break;
		default:
			break;
		}
	}
	/*for (int i = 0; i < tmpBranch.size(); i++)
	{
		printf("\n %d",(int)tmpBranch[i]);
	}*/
	return tmpBranch;
}
//找出除自身及目标点以外自身与目标点连线为对角线的方形区域内是否全为可直达点(相当于原空地)
bool Map::HasTraffic(int& ourx, int& oury, int& dstx, int& dsty)
{

	bool findNearest = false;
	bool isAllVoid = true;
	int curDis = abs(dstx - ourx) + abs(dsty - oury);
	if (dstx >= ourx)
	{
		for (int i = ourx; i <= dstx; i++)
		{
			if (dsty >= oury)
			{
				for (int j = oury; j <= dsty; j++)
				{
					if (i == ourx && j == oury)continue;
					if (i == dstx && j == dsty)continue;
					/*if (this->map[i][j].type == 1)
					{
						findNearest = true;
						break;
					}*/
					/*if (this->map[i][j].type == 4 || (this->map[i][j].type == 2 && (abs(FindAnotherHole(this->map[i][j]).x - dstx) + abs(FindAnotherHole(this->map[i][j]).y - dsty)) < (abs(this->map[i][j].x - dstx) + abs(this->map[i][j].y - dsty))))
					{
						if (!findNearest)
						{
							dstx = this->map[i][j].x;
							dsty = this->map[i][j].y;
							findNearest = true;
						}
						break;
					}*/
					if (this->map[i][j].type != 4 && this->map[i][j].type != 0)
					{
						isAllVoid = false;
					}
				}
			}
			else
			{
				for (int j = oury; j >= dsty; j--)
				{
					if (i == ourx && j == oury)continue;
					if (i == dstx && j == dsty)continue;
					/*if (this->map[i][j].type == 1)
					{
						findNearest = true;
						break;
					}*/
					/*if (this->map[i][j].type == 4 || (this->map[i][j].type == 2 && (abs(FindAnotherHole(this->map[i][j]).x - dstx) + abs(FindAnotherHole(this->map[i][j]).y - dsty)) < (abs(this->map[i][j].x - dstx) + abs(this->map[i][j].y - dsty))))
					{
						if (!findNearest)
						{
							dstx = this->map[i][j].x;
							dsty = this->map[i][j].y;
							findNearest = true;
						}
						break;
					}*/
					if (this->map[i][j].type != 4 && this->map[i][j].type != 0)
					{
						isAllVoid = false;
					}
				}
			}
			if (!isAllVoid)
			{
				break;
			}
		}
	}
	else
	{
		for (int i = ourx; i >= dstx; i--)
		{
			if (dsty >= oury)
			{
				for (int j = oury; j <= dsty; j++)
				{
					if (i == ourx && j == oury)continue;
					if (i == dstx && j == dsty)continue;
					/*if (this->map[i][j].type == 1)
					{
						findNearest = true;
						break;
					}*/
					/*if (this->map[i][j].type == 4 || (this->map[i][j].type == 2 && (abs(FindAnotherHole(this->map[i][j]).x - dstx) + abs(FindAnotherHole(this->map[i][j]).y - dsty)) < (abs(this->map[i][j].x - dstx) + abs(this->map[i][j].y - dsty))))
					{
						if (!findNearest)
						{
							dstx = this->map[i][j].x;
							dsty = this->map[i][j].y;
							findNearest = true;
						}
						break;
					}*/
					if (this->map[i][j].type != 4 && this->map[i][j].type != 0)
					{
						isAllVoid = false;
					}
				}
			}
			else
			{
				for (int j = oury; j >= dsty; j--)
				{
					if (i == ourx && j == oury)continue;
					if (i == dstx && j == dsty)continue;
					/*if (this->map[i][j].type == 1)
					{
						findNearest = true;
						break;
					}*/
					/*if (this->map[i][j].type == 4 || (this->map[i][j].type == 2 && (abs(FindAnotherHole(this->map[i][j]).x - dstx) + abs(FindAnotherHole(this->map[i][j]).y - dsty)) < (abs(this->map[i][j].x - dstx) + abs(this->map[i][j].y - dsty))))
					{
						//printf("\n aaaaaa \n");
						if (!findNearest)
						{
							dstx = this->map[i][j].x;
							dsty = this->map[i][j].y;
							findNearest = true;
						}
						//printf("\n %d,%d,%d,%d,%d,%d,%d \n", dstx, dsty, this->map[i][j].x, this->map[i][j].y, i, j, this->map[i][j].type);
						break;
					}*/
					if (this->map[i][j].type != 4 && this->map[i][j].type != 0)
					{
						isAllVoid = false;
					}
				}
			}
			if (!isAllVoid)
			{
				break;
			}
		}
	}
	if (isAllVoid)
	{
		return false;
	}
	return true;
}

void Map::FirstPass()//第一遍筛选，去除非加强点的对向均有哨点的哨点
{
	MapItem** tmpMap = new MapItem * [this->width];
	for (int i = 0; i < this->width; i++)
	{
		tmpMap[i] = new MapItem[this->height];
		for (int j = 0; j < this->height; j++)
		{
			tmpMap[i][j].type = this->map[i][j].type;
			tmpMap[i][j].x = this->map[i][j].x;
			tmpMap[i][j].y = this->map[i][j].y;
			tmpMap[i][j].dir = this->map[i][j].dir;
			tmpMap[i][j].name = this->map[i][j].name;
			tmpMap[i][j].towerType = this->map[i][j].towerType;
			tmpMap[i][j].frequency = this->map[i][j].frequency;
		}
	}
	for (int i = 0; i < this->width; i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			if (this->map[i][j].type == 4)
			{
				if ((i - 1 >= 0 && i + 1 < this->width && tmpMap[i - 1][j].type == 4 && tmpMap[i + 1][j].type == 4) || (j - 1 >= 0 && j + 1 < this->height && tmpMap[i][j - 1].type == 4 && tmpMap[i][j + 1].type == 4))
				{//判断是否是加强点，是的话就不能删掉
					int neiborCount = 0, routeCount = 0;
					if (i + 1 < this->width && j + 1 < this->height && tmpMap[i + 1][j + 1].type != 0 && tmpMap[i + 1][j + 1].type != 4)
					{
						neiborCount++;
					}
					if (i - 1 >= 0 && j + 1 < this->height && tmpMap[i - 1][j + 1].type != 0 && tmpMap[i - 1][j + 1].type != 4)
					{
						neiborCount++;
					}
					if (i + 1 < this->width && j - 1 >= 0 && tmpMap[i + 1][j - 1].type != 0 && tmpMap[i + 1][j - 1].type != 4)
					{
						neiborCount++;
					}
					if (i - 1 >= 0 && j - 1 >= 0 && tmpMap[i - 1][j - 1].type != 0 && tmpMap[i - 1][j - 1].type != 4)
					{
						neiborCount++;
					}

					if (i + 1 < this->width && (tmpMap[i + 1][j].type != 1 && !(tmpMap[i + 1][j].type == 3 && tmpMap[i + 1][j].dir == MOVE_LEFT)))
					{
						routeCount++;
					}
					if (i - 1 >= 0 && (tmpMap[i - 1][j].type != 1 && !(tmpMap[i - 1][j].type == 3 && tmpMap[i - 1][j].dir == MOVE_RIGHT)))
					{
						routeCount++;
					}
					if (j + 1 < this->height && (tmpMap[i][j + 1].type != 1 && !(tmpMap[i][j + 1].type == 3 && tmpMap[i][j + 1].dir == MOVE_UP)))
					{
						routeCount++;
					}
					if (j - 1 >= 0 && (tmpMap[i][j - 1].type != 1 && !(tmpMap[i][j - 1].type == 3 && tmpMap[i][j - 1].dir == MOVE_DOWN)))
					{
						routeCount++;
					}

					if (neiborCount < 3 || routeCount < 3)
					{
						this->map[i][j].type = 0;
					}
				}
			}
		}
	}
	for (int i = 0; i < this->width; i++)
	{
		delete[] tmpMap[i];
	}
	delete[] tmpMap;
}

void Map::SecondPass()//第二遍筛选，添加滑道起始点
{
	LOGE("正在进行地图的第二次筛选");
	for (int i = 0; i < this->width; i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			if (this->map[i][j].type != 3)continue;
			MapItem tmpItem = this->map[i][j];
			do
			{
				switch (tmpItem.dir)
				{
				case MOVE_LEFT:
					if (tmpItem.x + 1 < this->width)
					{
						tmpItem = map[tmpItem.x + 1][tmpItem.y];
					}
					else
					{
						tmpItem.type = -1;
					}
					break;
				case MOVE_RIGHT:
					if (tmpItem.x - 1 >= 0)
					{
						tmpItem = map[tmpItem.x - 1][tmpItem.y];
					}
					else
					{
						tmpItem.type = -1;
					}
					break;
				case MOVE_UP:
					if (tmpItem.y + 1 < this->height)
					{
						tmpItem = map[tmpItem.x][tmpItem.y + 1];
					}
					else
					{
						tmpItem.type = -1;
					}
					break;
				case MOVE_DOWN:
					if (tmpItem.y - 1 >= 0)
					{
						tmpItem = map[tmpItem.x][tmpItem.y - 1];
					}
					else
					{
						tmpItem.type = -1;
					}
					break;
				default:
					tmpItem.type = -1;
					break;
				}
			} while (tmpItem.type == 3);
			if (tmpItem.type == 0 || tmpItem.type == 4)
			{
				this->map[tmpItem.x][tmpItem.y].type = 4;
				this->map[tmpItem.x][tmpItem.y].towerType = 1;
			}
		}
	}
	LOGE("筛选完毕");
}

void Map::ThirdPass()//第三遍筛选，构建交通线
{
	//测试全图枢纽点
	/*for (int i = 0; i < this->width; i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			if (this->map[i][j].type == 0)
			{
				this->map[i][j].type = 4;
			}
		}
	}*/

	for (int i = 0; i < this->width; i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			if ((this->map[i][j].type == 4 && this->map[i][j].towerType != 2) || this->map[i][j].type == 2)//如果该点是非交通线塔或者虫洞
			{
				MapItem tmpItem = this->map[i][j];
				while (tmpItem.x + 1 < this->width && this->map[tmpItem.x + 1][tmpItem.y].type != 1 && this->map[tmpItem.x + 1][tmpItem.y].type != 3)//右边的点不触边或者陨石或者滑道的话继续循环
				{
					tmpItem = this->map[tmpItem.x + 1][tmpItem.y];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//如果遇到非线塔或者虫洞则构建交通线并跳出循环
					{
						while (tmpItem.x - 1 > i)
						{
							tmpItem = this->map[tmpItem.x - 1][tmpItem.y];
							this->map[tmpItem.x][tmpItem.y].type = 4;
							this->map[tmpItem.x][tmpItem.y].towerType = 2;
						}
						break;
					}
				}
				tmpItem = this->map[i][j];
				while (tmpItem.x - 1 >= 0 && this->map[tmpItem.x - 1][tmpItem.y].type != 1 && this->map[tmpItem.x - 1][tmpItem.y].type != 3)//左边的点不触边或者陨石或者滑道的话继续循环
				{
					tmpItem = this->map[tmpItem.x - 1][tmpItem.y];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//如果遇到非线塔或者虫洞则构建交通线并跳出循环
					{
						while (tmpItem.x + 1 < i)
						{
							tmpItem = this->map[tmpItem.x + 1][tmpItem.y];
							this->map[tmpItem.x][tmpItem.y].type = 4;
							this->map[tmpItem.x][tmpItem.y].towerType = 2;
						}
						break;
					}
				}
				tmpItem = this->map[i][j];
				while (tmpItem.y + 1 < this->height && this->map[tmpItem.x][tmpItem.y + 1].type != 1 && this->map[tmpItem.x][tmpItem.y + 1].type != 3)//下边的点不触边或者陨石或者滑道的话继续循环
				{
					tmpItem = this->map[tmpItem.x][tmpItem.y + 1];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//如果遇到非线塔或者虫洞则构建交通线并跳出循环
					{
						while (tmpItem.y - 1 > j)
						{
							tmpItem = this->map[tmpItem.x][tmpItem.y - 1];
							this->map[tmpItem.x][tmpItem.y].type = 4;
							this->map[tmpItem.x][tmpItem.y].towerType = 2;
						}
						break;
					}
				}
				tmpItem = this->map[i][j];
				while (tmpItem.y - 1 >= 0 && this->map[tmpItem.x][tmpItem.y - 1].type != 1 && this->map[tmpItem.x][tmpItem.y - 1].type != 3)//上边的点不触边或者陨石或者滑道的话继续循环
				{
					tmpItem = this->map[tmpItem.x][tmpItem.y - 1];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//如果遇到非线塔或者虫洞则构建交通线并跳出循环
					{
						while (tmpItem.y + 1 < j)
						{
							tmpItem = this->map[tmpItem.x][tmpItem.y + 1];
							this->map[tmpItem.x][tmpItem.y].type = 4;
							this->map[tmpItem.x][tmpItem.y].towerType = 2;
						}
						break;
					}
				}
			}
		}
	}
}
