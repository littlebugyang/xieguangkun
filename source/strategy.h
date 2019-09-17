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
	ATTACK_ACTIVE = 0,//������ǰ100�غ�
	ATTACK_NEGATVE = 1,//�����ĺ�50�غ�
	DEFENSE = 2,//���ص�150�غ�
};

enum ATTACKER
{
	FINDER = 0,//�÷���
	PORTRAITER = 1,// ����ѹ����
	TRANSVERSER = 2, // ����ѹ����
	HELPER = 3, // Э����
	PORTRAITER_1 = 4,//����ѹ����
	TRANSVERSER_1 = 5,//����ѹ����
	EATTER = 6 //��ʳ��
};

struct PressurePoint {
	int x = -1;
	int y = -1;
	int ownerId = -1;  // ����õ������
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
	Player* EnemyVis;//��ֻ�ҷ�kun��Ұ�ڵз�kun
	int EnemyVisNum;
	Power* PowerVis;//��ֻ�ҷ�kun��Ұ��������
	int PowerVisNum;
	TURN turn; // ��ǰս�����
	int myTeamId; // �ҷ�id
	string myForce; // �ҷ�����
	BLOCK kunblock[4];
	bool* dispersed;
	int BaitId;
	int* Attacker; // �洢ÿ������Ľ�ɫ(�������������
	Player targetKun;
	int* attackerDirections; // �ݴ�����Ľ�������
	int attackRoundId;
	Player GuessEnemyKun[4];
	int diffuseThreshold;
	//int minPressureThreshold; // ��ȡѹ�ȵ�ʱ����ɢ��Χ
	//PressurePoint pressurePoints[4]; // �洢ѹ�ȵ�
	bool surroundMode;

	int patrolRoundId; // ���ڱ�֤Ѳ�ߵ��ȶ���
	Point patrolLeaderAreaPoint; // ��ͷ���Ŀ������
	Point patrolTargetAreaPoints[4]; // �������Ŀ������

	bool isHarass;//�Ƿ�����ɧ�ŵз�
	bool isPursuing;//�Ƿ�����Χ��
	int remainingSteps;//Χ��ʣ�ಽ��
	vector<Player> harassingEnm;//����ɧ�ŵĵз���λ
	vector<Player> pursuingEnm;//����Χ���ĵз���λ
	vector<Point> lastHarPos;//�ϻغ�ɧ�ŵĵз�����λ��
	vector<Point> lastPurPos;//�ϻغ�Χ���ĵз�����λ��
	vector<vector<Player>> pursuingUnit;//����Χ��������
	vector<vector<DIRECT>> pursuingDecision;//���ֶ�������
	vector<Player> harassingUnit;//����ɧ�ŶԷ�������
	vector<DIRECT> harassingDecision;//ɧ�����ֶ�������

public:
	void updateAttacker();
	void initEnemyKun();
	void updateEnemyKun();

	void updateLeg(LegStartMsg* t);
	void updateRound(RoundMsg* t);

	Map* map;
	Player* OurKun;
	int ourkunnum;

	int getMyTeamId();//��ȡ����id
	void initKunBlock(int pid_1, int pid_2, int pid_3, int pid_4);
	string getMyForce();// void setMyForce(string); һ���趨�㲻��ʹ��, ɾ��set����
	void setSituation(int, bool); // ���µ�ǰս�����
	TURN GameSituation() { return turn; };//���ص�ǰ��ս��״��
	void DistinguishPlayers(int pid_1, int pid_2, int pid_3, int pid_4);//����˫��kun

	void AddMapFrequency(int pid);//Ϊ��ͼ���Ƶ����
	void GetEnemyVis(int pid);//��ȡ�ҷ�ָ��kun��Ұ�ڵĵз�kun
	int GetEnemyVisNum() { return EnemyVisNum; }; //��ȡ�ҷ�ָ��kun��Ұ�ڵĵз�kun����
	void GetPowerVis(int pid);//��ȡ�ҷ�ָ��kun��Ұ�ڵ�����
	int GetPowerVisNum() { return PowerVisNum; };//��ȡ�ҷ�ָ��kun��Ұ�ڵ���������
	int ChangePid(int pid);//����pidֵ���ض�Ӧ��ourkun�����±�
	int ChangeBlockPid(int pid);//����pidֵ���ض�Ӧ��hasBlock��BlockTar�����±�
	Power FindNearestPow(Player thiskun);//�����kun���������
	bool CheckPow(int x, int y);
	void sethasBlock(int i) { kunblock[i].isBlock = false; };//�ж�kun�Ƿ����谭״̬
	int gethasBlock(int i) { return kunblock[i].isBlock == true ? 1 : 0; };

	DIRECT Normal(Player thisplayer);//ͨ������
	DIRECT FindClosestEdge(int x, int y);//�ҵ�����kun����ı߽�
	DIRECT CheckOnRoute(int x, int y, int radius, Clock wise);//����ָ���뾶�켣�˶�
	Point NowTo(Player thiskun);//������Ѱ���߾���

	DIRECT Run(Player thisplayer);//���ܾ���
	void UpdateDispersed();
	DIRECT CarefreeRun(Player thisplayer);//
	DIRECT UrgentRun(Player thisplayer);//���ܳ����������
	DIRECT CrazyRun(Player thisplayer);
	int IsReachForRun(int x, int y, DIRECT dir, int xx, int yy);//���ܷ�������Լ���
	bool dangerousRun();
	void UpdateBait();

	// Ѳ���㷨
	Point FollowPatrolRoutine(Player); // ����һ��ָ��, ����ÿ������Ӧ��ȥ��Area
	Point GoToArea(Player, Point); // ����һ��Area������ �� ����, ������������ȥ�ľ���ĵ�(��map)��
	Point FindArea(Point); // ���뵱ǰ���������, ������һ��Ѳ�����������

	DIRECT Attack(Player thisplayer);//��������
	void CheckSituation();//����Ƿ����γɿ�׷������
	void UpdateLastPos();
	vector<MapItem> Subtract(vector<MapItem> left, vector<MapItem> right);

	DIRECT FinalDecision(int pid);//���վ���

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
		patrolRoundId = -2; // �����-1�Ļ�, ��ôround_id=0ʱ����, �ͻ�����Ϊ֮ǰ�Ѿ���ʼ��Ѳ��ģʽ
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