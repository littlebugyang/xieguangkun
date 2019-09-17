#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include "cJSON.h"
#include "message.h"
#include "map.h"
#include <string>
#include <cmath>
#include "vector"

using namespace std;

enum TURN
{
	ATTACK_ACTIVE = 0,//进攻的前100回合
	ATTACK_NEGATVE = 1,//进攻的后50回合
	DEFENSE = 2,//防守的150回合
};

enum ATTACKER
{
	FINDER = 0,//得分手
	PORTRAITER = 1,// 纵向压迫者
	TRANSVERSER = 2, // 横向压迫者
	HELPER = 3, // 协助者
	PORTRAITER_1 = 4,//纵向压迫者
	TRANSVERSER_1 = 5,//纵向压迫者
	EATTER = 6 //进食者
};

struct PressurePoint {
	int x = -1;
	int y = -1;
	int ownerId = -1;  // 负责该点的鲲鲲
};

struct BLOCK
{
	int pid;
	bool isBlock;
	Point BlockTar;
};


class OurStrategy
{
private:
	LegStartMsg* localLeg;
	RoundMsg* localRound;
	//Player* OurKun;
	Player* EnemyKun;
	//Map* map;
	//int ourkunnum;
	int enemykunnum;
	Player* EnemyVis;//单只我方kun视野内敌方kun
	int EnemyVisNum;
	Power* PowerVis;//单只我方kun视野内能量点
	int PowerVisNum;
	TURN turn; // 当前战局情况
	int myTeamId; // 我方id
	string myForce; // 我方势力
	BLOCK kunblock[4];
	bool* dispersed;
	int BaitId;
	int* Attacker; // 存储每个鲲鲲的角色(这命名我真的醉
	Player targetKun;
	int* attackerDirections; // 暂存鲲鲲的进攻方向
	int attackRoundId;
	Player GuessEnemyKun[4];
	int diffuseThreshold;
	//int minPressureThreshold; // 获取压迫点时的扩散范围
	//PressurePoint pressurePoints[4]; // 存储压迫点
	bool surroundMode;

	int patrolRoundId; // 用于保证巡逻的稳定性
	Point patrolLeaderAreaPoint; // 领头鲲的目标区域
	Point patrolTargetAreaPoints[4]; // 各个鲲的目标区域

	bool isHarass;//是否正在骚扰敌方
	bool isPursuing;//是否正在围捕
	int remainingSteps;//围捕剩余步数
	vector<Player> harassingEnm;//正在骚扰的敌方单位
	vector<Player> pursuingEnm;//正在围捕的敌方单位
	vector<Point> lastHarPos;//上回合骚扰的敌方所在位置
	vector<Point> lastPurPos;//上回合围捕的敌方所在位置
	vector<vector<Player>> pursuingUnit;//正在围捕的人手
	vector<vector<DIRECT>> pursuingDecision;//捕手动作决策
	vector<Player> harassingUnit;//正在骚扰对方的人手
	vector<DIRECT> harassingDecision;//骚扰人手动作决策

public:
	void updateAttacker();
	void initEnemyKun();
	void updateEnemyKun();

	void updateLeg(LegStartMsg* t);
	void updateRound(RoundMsg* t);

	Map* map;
	Player* OurKun;
	int ourkunnum;

	int getMyTeamId();//获取队伍id
	void initKunBlock(int pid_1, int pid_2, int pid_3, int pid_4);
	string getMyForce();// void setMyForce(string); 一旦设定便不再使用, 删除set方法
	void setSituation(int, bool); // 更新当前战局情况
	TURN GameSituation() { return turn; };//返回当前的战局状况
	void DistinguishPlayers(int pid_1, int pid_2, int pid_3, int pid_4);//区分双方kun

	void AddMapFrequency(int pid);//为地图添加频繁度
	void GetEnemyVis(int pid);//获取我方指定kun视野内的敌方kun
	int GetEnemyVisNum() { return EnemyVisNum; }; //获取我方指定kun视野内的敌方kun数量
	void GetPowerVis(int pid);//获取我方指定kun视野内的能量
	int GetPowerVisNum() { return PowerVisNum; };//获取我方指定kun视野内的能量数量
	int ChangePid(int pid);//根据pid值返回对应的ourkun数组下标
	int ChangeBlockPid(int pid);//根据pid值返回对应的hasBlock和BlockTar数组下标
	Power FindNearestPow(Player thiskun);//检查我kun最近能量点
	bool CheckPow(int x, int y);
	void sethasBlock(int i) { kunblock[i].isBlock = false; };//判断kun是否处于阻碍状态
	int gethasBlock(int i) { return kunblock[i].isBlock == true ? 1 : 0; };

	DIRECT Normal(Player thisplayer);//通常决策
	DIRECT FindClosestEdge(int x, int y);//找到距离kun最近的边界
	DIRECT CheckOnRoute(int x, int y, int radius, Clock wise);//按照指定半径轨迹运动
	Point NowTo(Player thiskun);//漫步搜寻决策决策

	DIRECT Run(Player thisplayer);//逃跑决策
	void UpdateDispersed();
	DIRECT CarefreeRun(Player thisplayer);//
	DIRECT UrgentRun(Player thisplayer);//逃跑初步方向决策
	DIRECT CrazyRun(Player thisplayer);
	int IsReachForRun(int x, int y, DIRECT dir, int xx, int yy);//逃跑方向可行性计算
	bool dangerousRun();
	void UpdateBait();

	// 巡逻算法
	Point FollowPatrolRoutine(Player); // 传入一个指针, 返回每个鲲鲲应该去的Area
	Point GoToArea(Player, Point); // 传入一个Area的坐标 和 鲲鲲, 返回这个鲲鲲该去的具体的点(在map)上
	Point FindArea(Point); // 传入当前区域的坐标, 返回下一个巡逻区域的坐标

	DIRECT Attack(Player thisplayer);//进攻决策
	void CheckSituation();//检查是否已形成可追捕局面
	void UpdateLastPos();
	vector<MapItem> Subtract(vector<MapItem> left, vector<MapItem> right);

	DIRECT FinalDecision(int pid);//最终决策

	void ResetAttackerDirections();
	OurStrategy(int teamId)
	{
		localLeg = NULL;
		localRound = NULL;
		OurKun = NULL;
		EnemyKun = NULL;
		map = NULL;
		ourkunnum = 0;
		enemykunnum = 0;
		EnemyVis = NULL;
		EnemyVisNum = 0;
		PowerVis = NULL;
		PowerVisNum = 0;
		myTeamId = teamId;
		dispersed = NULL;
		attackRoundId = -2;
		Attacker = NULL;
		attackerDirections = NULL;
		diffuseThreshold = 99;
		surroundMode = false;

		isHarass = false;
		isPursuing = false;
		remainingSteps = -1;
		patrolRoundId = -2; // 如果是-1的话, 那么round_id=0时调用, 就会误认为之前已经开始了巡逻模式
	}

	~OurStrategy() {
		delete[] this->Attacker;
		delete[] this->attackerDirections;
		delete[] this->OurKun;
		delete[] this->EnemyKun;
		delete[] this->EnemyVis;
		delete[] this->PowerVis;
		delete[] this->dispersed;
	}
};

#endif