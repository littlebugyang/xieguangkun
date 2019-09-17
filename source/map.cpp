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
				printf("��");
				break;
			case 1:
				printf("��");
				break;
			case 2:
				printf("��");
				break;
			case 3:
				switch (map[i][j].dir)
				{
				case MOVE_LEFT:
					printf("��");
					break;
				case MOVE_RIGHT:
					printf("��");
					break;
				case MOVE_UP:
					printf("��");
					break;
				case MOVE_DOWN:
					printf("��");
					break;
				default:
					printf("��");
					break;
				}
				break;
			case 4:
				switch (map[i][j].towerType)
				{
				case 0:
					printf("��");
					break;
				case 1:
					printf("��");
					break;
				case 2:
					printf("�� ");
					break;
				default:
					printf("��");
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
		//printf("\n �ڶ� ���飺%s \n", map[ls->getWormholes()[i].x][ls->getWormholes()[i].y].name.c_str());
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
	//����ͼ��Ϊ3*3�ľŸ�����
	//�Ľǵ�����̶�Ϊ������(square)
	//��������x��y
	//Ϊ����������������area��������һ����ʶ�
	Point areaIndex = this->getAreaByCoord(x, y);
	++this->areas[areaIndex.x][areaIndex.y].freq;
}

Point Map::getAreaByCoord(int x, int y)
{
	// todo: ֻ���Ƿ��ε�ͼ
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
	// ���뵱ǰ������꣬���ؾ���õ�����ġ�ӵ�з���Ƶ����͡��Ǽ�����ķ���
	// Ҳ����˵����һ�������������ָ���ܱ߷���Ƶ����͵�����

	Point presentAreaPoint = this->getAreaByCoord(x, y);

	int directFreqs[4] = { 0 };

	// ˳ʱ�뷽����㣻���Ϸ���ʼ
	DIRECT minDirect = NO_MOVE; // Ĭ�Ϸ���
	int minFreq = 0; // Ĭ��Ƶ��

	if (presentAreaPoint.x - 1 >= 0) { // �ϱ߻���area
		directFreqs[0] = this->areas[presentAreaPoint.x - 1][presentAreaPoint.y].freq;
		if (directFreqs[0] > minFreq) {
			minFreq = directFreqs[0];
			minDirect = MOVE_UP;
		}
	}
	if (presentAreaPoint.y + 1 <= 2) { // �ұ߻���area
		directFreqs[1] = this->areas[presentAreaPoint.x][presentAreaPoint.y + 1].freq;
		if (directFreqs[1] > minFreq) {
			minFreq = directFreqs[1];
			minDirect = MOVE_RIGHT;
		}
	}
	if (presentAreaPoint.x + 1 <= 2) { // �±߻���area
		directFreqs[2] = this->areas[presentAreaPoint.x + 1][presentAreaPoint.y].freq;
		if (directFreqs[2] > minFreq) {
			minFreq = directFreqs[2];
			minDirect = MOVE_DOWN;
		}
	}
	if (presentAreaPoint.y - 1 >= 0) { // ��߻���area
		directFreqs[3] = this->areas[presentAreaPoint.x][presentAreaPoint.y - 1].freq;
		if (directFreqs[3] > minFreq) {
			minFreq = directFreqs[3];
			minDirect = MOVE_LEFT;
		}
	}
	if (directFreqs[0] == directFreqs[1] && directFreqs[2] == directFreqs[3] && directFreqs[1] == directFreqs[2]) {
		// ����������ֵ��ȫΪ0������ȫ��Ƶ����ͬ
		return NO_MOVE;
	}
	return minDirect;
}

bool Map::EatSafety(int x, int y)
{
	int tonglu = 4;
	//��
	if (y == 0)
		tonglu--;
	else if (this->map[x][y - 1].type == 1)
		tonglu--;
	else if (this->map[x][y - 1].type == 3 && this->map[x][y - 1].dir == MOVE_DOWN)
		tonglu--;
	//��
	if (y == height - 1)
		tonglu--;
	else if (this->map[x][y + 1].type == 1)
		tonglu--;
	else if (this->map[x][y + 1].type == 3 && this->map[x][y + 1].dir == MOVE_DOWN)
		tonglu--;
	//��
	if (x == 0)
		tonglu--;
	else if (this->map[x - 1][y].type == 1)
		tonglu--;
	else if (this->map[x - 1][y].type == 3 && this->map[x - 1][y].dir == MOVE_DOWN)
		tonglu--;
	//��
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
	//printf("%d,%d,%d���Ե���\n", this->map[x][y].type,this->map[x][y].dir, ContractDir(dir));
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
//����·�����ߣ�Ĭ�ϲ���Ϊ������ɢ����
DIRECT Map::pathFind(int ourx, int oury, int dstx, int dsty, DIRECT dir)
{

	/*if (ourx == 10 && oury == 2)
	{
		printf("\n aaaaa \n");
	}*/
	int tdx = dstx, tdy = dsty;

	//���㷽��˳��
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
		//��·�Ĺ����й�ܷ��򻬵�
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
			printf("\n ɵ�Ϻ�������p1 \n");
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
		return this->GoDestination(ourx, oury, dstx, dsty, seq);//����ȫͼ��ɢ
		//dstx = tdx;
		//dsty = tdy;
		//��ʱ�������򲢷�ȫ�ǿ�ֱ�ӵ��ﵥλ��
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
//��Ӧ��ȫ·������
vector<DIRECT> Map::fullPathFind(int ourx, int oury, int dstx, int dsty, DIRECT dir)
{
	int tdx = dstx, tdy = dsty;

	//���㷽��˳��
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
//�ҵ���Ӧ�ĳ涴
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
//�ҵ�����Ľ�ͨ��/��
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
//�ҵ���һ�����ձ����͵��ĵ㣬Ĭ�ϲ���Ϊ�Ƿ��ǻ����յ�ĳ涴
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
				printf("Ѱ�����·����������p4");
				break;
			}
		}
	}
	//��������Ҳ�����ǳ涴������д�ں���
	if (!excHole)
	{
		if (t.type == 2)
		{
			t = this->FindAnotherHole(t);
		}
	}
	return t;
}
//�������ض�������һ��󵽴�ĵ�
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
//�ҵ�ͬ������ͨ��ͨ���ϵ���������·��
DIRECT Map::FindBestRoute(int ourx, int oury, int dstx, int dsty)
{
	bool find = false;
	vector<vector<MapItem>> pathRecord;//·������
	vector<vector<DIRECT>> dirRecord;//�н����򵵰�
	vector<vector<DIRECT>> branchRecord;//ÿ���������һ��·�ڵĿ��ܷ�֧
	//vector<DIRECT> curBranchRecord;//Ŀǰ����·�ڵĵķ�֧ѡ��״̬
	vector<MapItem> tmpPath;//��ǰ·��
	vector<DIRECT> tmpDir;//��ǰ�н�����
	vector<DIRECT> tmpBranch;//��ǰ���п��ܷ�֧
	vector<int> lens;//���гɹ������յ��·������
	vector<vector<DIRECT>> sp;//���гɹ������յ���н�����
	MapItem cur = this->map[ourx][oury];//��ǰ̽����
	//MapItem lastCur = cur;//��ǰ̽���㴫�����
	DIRECT lastDir;//��ʱ�ķ���
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
				//�ص�
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
				//ɾ����һ��Ԫ��
				branchRecord.back().erase(branchRecord.back().begin());
				tmpPath = pathRecord.back();
				tmpDir = dirRecord.back();
				cur = tmpPath.back();
				//lastCur = tmpPath.back();
				//printf(" �������г��Ȼص�����\n");
			}
		}
		else
		{
			if (tmpDir.size() > MAX_LEN_PLUS + (abs(dstx - ourx) + abs(dsty - oury)))
			{
				//�ص�
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
				//ɾ����һ��Ԫ��
				branchRecord.back().erase(branchRecord.back().begin());
				tmpPath = pathRecord.back();
				tmpDir = dirRecord.back();
				cur = tmpPath.back();
				//lastCur = tmpPath.back();
				//printf(" ������������󳤶Ȼص�����\n");
			}
		}


		//printf("\n �ɹ�·��������%d \n", lens.size());
		//printf("\n ̽�����µ��� \n");
		//�������Ŀ���
		if (cur.x == dstx && cur.y == dsty)
		{
			lens.push_back(tmpDir.size());
			sp.push_back(tmpDir);
			//�ص�
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
			//ɾ����һ��Ԫ��
			branchRecord.back().erase(branchRecord.back().begin());
			tmpPath = pathRecord.back();
			tmpDir = dirRecord.back();
			cur = tmpPath.back();
			//lastCur = tmpPath.back();
			//printf("Ŀ���ص�����\n");
		}

		//�����·����
		if (tmpBranch.empty())
		{
			//�ص�
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
			//ɾ����һ��Ԫ��
			branchRecord.back().erase(branchRecord.back().begin());
			tmpPath = pathRecord.back();
			tmpDir = dirRecord.back();
			cur = tmpPath.back();
			//lastCur = tmpPath.back();
			//printf("��·�ص�����\n");
		}

		bool isBack = false;
		//�ߵ��߹��ĵط�ʱ�ص�
		for (int i = 0; i < tmpPath.size() - 1; i++)
		{
			if (tmpPath[i].x == cur.x && tmpPath[i].y == cur.y)
			{
				//printf("\n aaaaaa \n");
				//�ص�
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
				//ɾ����һ��Ԫ��
				branchRecord.back().erase(branchRecord.back().begin());
				tmpPath = pathRecord.back();
				tmpDir = dirRecord.back();
				cur = tmpPath.back();
				//lastCur = tmpPath.back();
				//printf("ѭ���ص�����\n");
				break;
			}
		}
		if (isBack && branchRecord.empty())
		{
			break;
		}

		if (tmpBranch.size() > 1)//���߷�֧����һ��
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
				printf("Ѱ�����·����������p1");
				break;
			}
			vector<DIRECT> t = tmpBranch;
			pathRecord.push_back(tmpPath);
			dirRecord.push_back(tmpDir);
			branchRecord.push_back(t);
			//ɾ����һ��Ԫ��
			branchRecord.back().erase(branchRecord.back().begin());
			//printf("��������\n");
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
				printf("Ѱ�����·����������p2");
				break;
			}
		}
		else
		{
			printf("Ѱ�����·����������p3");
		}


		//lastDir = tmpBranch[0];
		//��ʱ���෴����
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
			printf("Ѱ�����·����������p5");
			break;
		}
		//tmpBranch = FindPossibleBranch(cur.x, cur.y, lastDir);
		tmpDir.push_back(tmpBranch[0]);//���뵱ǰ����

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
					printf("Ѱ�����·����������p4");
					break;
				}
			}
		}
		//��������Ҳ�����ǳ涴������д�ں���
		if (cur.type == 2)
		{
			lastDir = NO_MOVE;
			cur = this->FindAnotherHole(cur);
		}
		tmpPath.push_back(cur);
		tmpBranch = FindPossibleBranch(cur.x, cur.y, lastDir, abs(dstx - ourx) > DISTANCE_THRESHOLD || abs(dsty - oury) > DISTANCE_THRESHOLD);
		branchCount += tmpBranch.size();
	} while (branchCount < MAX_BRANCH_COUNT);

	//�������·����������
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
//ȥ����Ľ�ͨ��λ����ɢ�㷨��
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//ֱ��̽Ѱ����Ŀ����·����ɢ�㷨������ֵ��NEAREST_THRESHOLD
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//��Ӧ��ȫ·������
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//��������֮����ʵ����
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//���ص�ͼ�����о��������һ/�����ı�š�������
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//���ݲ���������Χ����ΧһȦ
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//���ݲ�������ȫ�����Χ
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
//չʾ��ɢ��Χ
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
				printf("��");
				break;
			case 1:
				printf("��");
				break;
			case 2:
				printf("��");
				break;
			case 3:
				switch (map[i][j].dir)
				{
				case MOVE_LEFT:
					printf("��");
					break;
				case MOVE_RIGHT:
					printf("��");
					break;
				case MOVE_UP:
					printf("��");
					break;
				case MOVE_DOWN:
					printf("��");
					break;
				default:
					printf("��");
					break;
				}
				break;
			case 4:
				switch (map[i][j].towerType)
				{
				case 0:
					printf("��");
					break;
				case 1:
					printf("��");
					break;
				case 2:
					printf("�� ");
					break;
				default:
					printf("��");
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
//������������˳������ĸ�ѹ�ȵ�(������ͬ)
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
//������ɢΧ������
vector<MapItem> Map::RotatingDiffusion(vector<Player> ours, Player ene, int threshold)
{
	//vector<MapItem> save;
	//�ҷ�ռ������
	vector<MapItem> tmpSave;
	vector<MapItem> ttmpSave;
	//�з�ռ������
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

	//�з���ɢ
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
			if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
					if (!(tmp.x == ttenSave[k].x && tmp.y == ttenSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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

		//�ҷ���ɢ
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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

		//�з���ɢ
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttenSave[k].x && tmp.y == ttenSave[k].y))//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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
			LOGE("���ǽ����Χ��, �з�ռ��������: ");
			for (vector<MapItem>::iterator it = tenSave.begin(); it != tenSave.end(); ++it) {
				LOGE("ռ������: (%d, %d)", (*it).x, (*it).y);
			}
			return tenSave;
		}
	}
	LOGE("���ǻ����ܽ����Χ");
	return vector<MapItem>();
}
//ȡ�ð���һ�����������ķ�Χ���ṹ���е�towertype����˵���ԭ�����ʵ����
vector<MapItem> Map::SortScope(int ourx, int oury, DIRECT dir, int threshold, bool incHole)
{
	if (ourx < 0 || ourx >19 || oury < 0 || oury>19) {
		LOGE("�Բ�!!!(%d, %d)", ourx, oury);
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
				if (!touch)//��ֹ���ߵ�ʱ�����ɼ���涴����
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
						if (!(tmp.x == ttmpSave[k].x && tmp.y == ttmpSave[k].y) && incHole)//��ֹ���ͻ�ԭ���ʱ�����ɼ���涴
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

	//����: ����
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

	// ����: ����
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
		LOGE("�Բ�!!!(%d, %d)", x, y);
	}
	this->areas[x][y] = area;
}

Area Map::getArea(int x, int y)
{
	if (x < 0 || x >2 || y < 0 || y>2) {
		LOGE("�Բ�!!!(%d, %d)", x, y);
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
	vector<DIRECT> tmpBranch;//��ǰ��·�ڻ���·�������������·�ڣ����п��ܷ�֧
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
//�ҳ�������Ŀ�������������Ŀ�������Ϊ�Խ��ߵķ����������Ƿ�ȫΪ��ֱ���(�൱��ԭ�յ�)
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

void Map::FirstPass()//��һ��ɸѡ��ȥ���Ǽ�ǿ��Ķ�������ڵ���ڵ�
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
				{//�ж��Ƿ��Ǽ�ǿ�㣬�ǵĻ��Ͳ���ɾ��
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

void Map::SecondPass()//�ڶ���ɸѡ����ӻ�����ʼ��
{
	LOGE("���ڽ��е�ͼ�ĵڶ���ɸѡ");
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
	LOGE("ɸѡ���");
}

void Map::ThirdPass()//������ɸѡ��������ͨ��
{
	//����ȫͼ��Ŧ��
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
			if ((this->map[i][j].type == 4 && this->map[i][j].towerType != 2) || this->map[i][j].type == 2)//����õ��Ƿǽ�ͨ�������߳涴
			{
				MapItem tmpItem = this->map[i][j];
				while (tmpItem.x + 1 < this->width && this->map[tmpItem.x + 1][tmpItem.y].type != 1 && this->map[tmpItem.x + 1][tmpItem.y].type != 3)//�ұߵĵ㲻���߻�����ʯ���߻����Ļ�����ѭ��
				{
					tmpItem = this->map[tmpItem.x + 1][tmpItem.y];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//����������������߳涴�򹹽���ͨ�߲�����ѭ��
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
				while (tmpItem.x - 1 >= 0 && this->map[tmpItem.x - 1][tmpItem.y].type != 1 && this->map[tmpItem.x - 1][tmpItem.y].type != 3)//��ߵĵ㲻���߻�����ʯ���߻����Ļ�����ѭ��
				{
					tmpItem = this->map[tmpItem.x - 1][tmpItem.y];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//����������������߳涴�򹹽���ͨ�߲�����ѭ��
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
				while (tmpItem.y + 1 < this->height && this->map[tmpItem.x][tmpItem.y + 1].type != 1 && this->map[tmpItem.x][tmpItem.y + 1].type != 3)//�±ߵĵ㲻���߻�����ʯ���߻����Ļ�����ѭ��
				{
					tmpItem = this->map[tmpItem.x][tmpItem.y + 1];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//����������������߳涴�򹹽���ͨ�߲�����ѭ��
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
				while (tmpItem.y - 1 >= 0 && this->map[tmpItem.x][tmpItem.y - 1].type != 1 && this->map[tmpItem.x][tmpItem.y - 1].type != 3)//�ϱߵĵ㲻���߻�����ʯ���߻����Ļ�����ѭ��
				{
					tmpItem = this->map[tmpItem.x][tmpItem.y - 1];
					if ((tmpItem.type == 4 && tmpItem.towerType != 2) || tmpItem.type == 2)//����������������߳涴�򹹽���ͨ�߲�����ѭ��
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
