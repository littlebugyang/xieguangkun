#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "cJSON.h"
#include <string>

using namespace std;

enum DIRECT
{
	MOVE_UP = 0,
	MOVE_DOWN = 1,
	MOVE_LEFT = 2,
	MOVE_RIGHT = 3,
	NO_MOVE = 4
};

struct SubAction
{
	DIRECT moveDirect;
};


struct Power
{
	int x;
	int y;
	int point;
};
struct Player
{
	int id = -1;
	int score;
	int sleep;
	int team;
	int x;
	int y;

	int patrolPointIndex = -1; // 不用Point, 怕会翻车. 获取时使用 patrolPointIndex % width 和 patrolPointIndex / height 来获取x, y坐标
	int patrolRoundId = -2; // 进入巡逻时的RoundId, 用来标识巡逻是否连续; 初始化不能为-1
};
struct Team
{
	int id;
	int point;
	int remain_life;
};
struct StartTeam
{
	int id;
	int* players = NULL;
	string force;
};
struct Meteor
{
	int x;
	int y;
};
struct Tunnel
{
	int x;
	int y;
	DIRECT direction;
};
struct Wormhole
{
	string name;
	int x;
	int y;
};

class LegStartMsg
{
public:
	LegStartMsg(cJSON* msg) {
		root = msg;
		meteors = new Meteor[1]; // why 1 ??
		tunnels = new Tunnel[1]; // why 1 ??
		wormholes = new Wormhole[1]; // why 1 ??
		teams = new StartTeam[2];
	};
	~LegStartMsg() {
		cJSON_Delete(root);
		delete[] this->meteors;
		delete[] this->tunnels;
		delete[] this->wormholes;

		for (unsigned short i = 0; i < 2; ++i) {
			if (this->teams[i].players != NULL) {
				delete[] this->teams[i].players;
			}
		}
		delete[] this->teams;
	};

	void DecodeMessge(int& myTeamId, int myPlayerId[4]);
	void GetMyTeamInfo(int& myTeamId, int myPlayerId[4]);
	void GetMeteor(int& myTeamId);
	void GetWormhole(int& myTeamId);
	void GetTunnel(int& myTeamId);
	void GetTeams(int& myTeamId);
	void GetCloud(int& myTeamId);

	int getWidth() { return width; };
	int getHeight() { return height; };
	int getVision() { return vision; };
	int getMyId() { return myId; };
	Meteor* getMeteors() { return meteors; };
	Tunnel* getTunnels() { return tunnels; };
	Wormhole* getWormholes() { return wormholes; };
	StartTeam* getTeams() { return teams; };
	int getMeteorsCount() { return meteorsCount; };
	int getTunnelsCount() { return tunnelsCount; };
	int getWormholesCount() { return wormholesCount; };
	int getTeamsCount() { return teamsCount; };

	string getMyForce(int);
private:
	int width;
	int height;
	int vision;
	int myId;
	Meteor* meteors;
	int meteorsCount;
	Tunnel* tunnels;
	int tunnelsCount;
	Wormhole* wormholes;
	int wormholesCount;
	StartTeam* teams;
	int teamsCount;
public:
	cJSON* root;
};

class ActMsg
{
public:
	ActMsg(int roundId);
	~ActMsg() { cJSON_Delete(root); };
	void AddSubAction(int team, int player, SubAction& act);

	void PackActMsg(char* actMsg, int maxMsgLenth);
public:
	cJSON* root;
	cJSON* msg_data;
	cJSON* actions;
	cJSON* subAct;
	cJSON* move;
};

class RoundMsg
{
public:
	RoundMsg(cJSON* msg) {
		root = msg;
		powers = new Power[1];
		players = new Player[1];
		teams = new Team[1];
	};
	~RoundMsg() {
		cJSON_Delete(root);
		delete[] this->powers;
		delete[] this->players;
		delete[] this->teams;
	};
	void DecodeMessge();
	int getRoundId() { return round_id; };
	Power* getPowers() { return powers; };
	Player* getPlayers() { return players; };
	Team* getTeams() { return teams; };
	string getMode() { return mode; };
	int getPowersCount() { return powersCount; };
	int getPlayersCount() { return playersCount; };
	int getTeamsCount() { return teamsCount; };
private:
	void DecodePlayers(cJSON* players);
	void DecodeTeams(cJSON* teams);
	void DecodePower(cJSON* coins);
	Power* powers;
	int powersCount;
	Player* players;
	int playersCount;
	Team* teams;
	int teamsCount;
	string mode;

public:
	cJSON* root;
	int round_id;
};

class LegEndMsg
{
public:
	LegEndMsg(cJSON* msg) { root = msg; };
	~LegEndMsg() { cJSON_Delete(root); };
	void DecodeMessge() {};
public:
	cJSON* root;
};

class GameEndMsg
{
public:
	GameEndMsg(cJSON* msg) { root = msg; };
	~GameEndMsg() { cJSON_Delete(root); };
	void DecodeMessge() {};
public:
	cJSON* root;
};

#endif



