#pragma once
#ifndef _MAP_H_
#define _MAP_H_

#include "cJSON.h"
#include "message.h"
#include <string>
#include <cmath>
#include <vector>

//扩散阈值
#define NEAREST_THRESHOLD 150

using namespace std;

enum Clock
{
	CLOCK_WISE = 0,
	ANTI_CLOCK_WISE = 1,
};
struct Point
{
	int x = -1;
	int y = -1;
};

struct MapItem
{
	int x;
	int y;
	int type;//0代表空地，1代表陨石，2代表虫洞，3代表滑道，4代表哨点
	DIRECT dir;//滑道的方向（如果有的话）
	string name;//虫洞的名称（如果有的话）
	int towerType;//哨塔类型，0代表初始建立，1代表传送带起点，2代表路
	int frequency;//频度（光顾度）
};

struct Area {
	int x = -1;
	int y = -1;
	int freq = 0; // 访问次数
	//bool beingLeft = false; // 自己的鲲将要离开这片区域
	bool occupied = false; // 被自家鲲占有
	bool dangerous = false; // 有敌鲲
};

class Map
{
public:
	Map()
	{
		MapItem** map = NULL;
		int width = 0;
		int height = 0;
	};
	~Map() {
		if (this->map != NULL)
		{
			for (int i = 0; i < this->width; i++)
			{
				delete[] map[i];
			}
			delete[] map;
			this->map = NULL;
		}
	};
	void UpdateMap(LegStartMsg* ls);
	void AddFrequency(int x, int y);//
	void printfAreaFre();
	bool EatSafety(int x, int y);
	bool IsReach(int x, int y);
	bool DirDelivery(int x, int y, DIRECT dir);
	DIRECT ContractDir(DIRECT dir);
	MapItem GetMap(int i, int j) { return map[i][j]; };
	DIRECT pathFind(int ourx, int oury, int dstx, int dsty, DIRECT dir = MOVE_UP);
	vector<DIRECT> fullPathFind(int ourx, int oury, int dstx, int dsty, DIRECT dir = MOVE_UP);
	Point getAreaByCoord(int, int);
	DIRECT getMinDirection(int, int);
	MapItem FindTransEnd(MapItem t, bool excHole = false);
	MapItem FindMoveEnd(int ourx, int oury, DIRECT move);

	int getWidth() { return this->width; }
	int getHeight() { return this->height; }
	int FindRealDistance(int ourx, int oury, int dstx, int dsty, int threshold = NEAREST_THRESHOLD);
	vector<Point> FindBatchMinDistance(int ourx, int oury, vector<MapItem> neibor, int threshold = NEAREST_THRESHOLD);
	vector<MapItem> getPeripheral(int ourx, int oury, int step);
	vector<MapItem> getScope(int ourx, int oury, int step);
	void DisplayScope(int ourx, int oury, int step);
	vector<MapItem> getPressurePoint(int ourx, int oury, int step);
	vector<MapItem> RotatingDiffusion(vector<Player> ours, Player ene, int threshold = NEAREST_THRESHOLD);
	vector<MapItem> SortScope(int ourx, int oury, DIRECT dir, int threshold, bool incHole = true);

	// 与区域相关
	void setAreaDangerous(int, int);
	void setAreaSafe(int, int);
	void occupyArea(int, int);
	void deoccupyArea(int, int);
	void setArea(int, int, Area);
	Area getArea(int, int);
	void resetArea();

private:
	MapItem** map = NULL;
	int width = 0;
	int height = 0;
	Area areas[3][3];
	// int Area[3][3] = { { 0 } };
	MapItem FindAnotherHole(MapItem t);
	MapItem FindNearestTraffic(int x, int y);
	DIRECT FindBestRoute(int ourx, int oury, int dstx, int dsty);
	DIRECT GoNearestTraffic(int ourx, int oury, int dstx, int dsty);
	DIRECT GoDestination(int ourx, int oury, int dstx, int dsty, vector<DIRECT> seq);
	vector<DIRECT> FullGoDestination(int ourx, int oury, int dstx, int dsty, vector<DIRECT> seq);
	vector<DIRECT> FindPossibleBranch(int ourx, int oury, DIRECT except, bool isFar);
	bool HasTraffic(int& ourx, int& oury, int& dstx, int& dsty);
	void FirstPass();
	void SecondPass();
	void ThirdPass();
	void Display();
};

#endif