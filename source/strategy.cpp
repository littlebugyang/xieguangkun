#include "strategy.h"
#include "map.h"
#include "stdlib.h"
#include "stdio.h"
#include "myLog.h"
#include "vector"
#include<algorithm>

#include <algorithm>
#include "vector"
#include <ctime>
#include <cmath>

//安全距离
#define SAFE_THRESHOLD 5
//围捕距离阈值
//#define ATTACK_THRESHOLD 
//安全距离
#define TEST_THRESHOLD 3

void OurStrategy::updateAttacker()
{
	if (this->ourkunnum < 3) { // 咱不玩了
		for (int i = 0; i < this->ourkunnum; ++i) {
			this->Attacker[i] = FINDER;
		}
	}

	// 临时维护的变量
	this->surroundMode = false;
	if (this->attackerDirections != NULL) {
		this->ResetAttackerDirections();
	}

	if (enemykunnum == 0) {
		// 视野内不存在敌人
		LOGE("There is no enemy in sight. ");
		if (this->Attacker != NULL) {
			delete[] this->Attacker;
		}
		if (this->attackerDirections != NULL) {
			delete[] this->attackerDirections;
		}
		this->Attacker = new int[this->ourkunnum];
		this->attackerDirections = new int[this->ourkunnum];
		for (int i = 0; i < this->ourkunnum; ++i) {
			Attacker[i] = FINDER;
			attackerDirections[i] = NO_MOVE;
		}
		if (this->attackRoundId == this->localRound->getRoundId() - 1) {
			// 刚刚退出进攻状态
			LOGE("刚刚还看到鲲鲲呢");
			this->map->resetArea();
			LOGE("清空了Area");
		}
		// todo: 刷新threshold
	}
	else if (this->attackRoundId + 1 == this->localRound->getRoundId()) {
		LOGE("Enemy in sight. Continue attacking. ");
		this->attackRoundId = this->localRound->getRoundId();
		// todo: 如果吃完敌人之后, 视野中还有敌人, 也要刷新threshold

		// 视野内存在敌人 且 攻击回合数与当前回合数差1, 证明上个回合已经在追击敌人
		// 更新一下攻击回合id即可
		int finderIndex = -1;
		Player originEnemy;
		for (int i = 0; i < this->ourkunnum; ++i) {
			bool found = false;
			if (this->Attacker[i] == EATTER) {
				this->GetEnemyVis(this->OurKun[i].id);
				Player nearestEnemy;
				for (int j = 0; j < this->EnemyVisNum; ++j) {
					if (this->EnemyVis[j].id == this->targetKun.id) {
						nearestEnemy = this->EnemyVis[j];
						finderIndex = i;
						originEnemy = this->EnemyVis[j];
						found = true;
						break;
					}
				}
			}
			if (found) {
				break;
			}
		}

		if (finderIndex != -1) {
			// 找到了, 这只鲲就是你之前找的那只鲲
			this->harassingUnit.clear();
			this->harassingUnit.push_back(this->OurKun[finderIndex]);
			this->harassingEnm.clear();
			this->harassingEnm.push_back(originEnemy);
			this->CheckSituation();
			LOGE("视野内存在敌人, 上个回合就是这个敌人");
			this->targetKun = originEnemy;
		}

		else {
			// 是的, 视野内确实有鲲, 但是这已经不是你的鲲鲲了. 转换目标吧
			LOGE("Enemy in sight. But it is not the original one. ");
			// 1. 确定主要追捕者.
			int nearestKunIndex = -1;
			Player nearestKun;
			Player nearestEnemy;
			int nearestDistance = 1000;
			for (int i = 0; i < this->ourkunnum; ++i) {
				this->GetEnemyVis(this->OurKun[i].id);
				int myX = this->OurKun[i].x;
				int myY = this->OurKun[i].y;
				for (int j = 0; j < this->EnemyVisNum; ++j) {
					int targetX = this->EnemyVis[j].x;
					int targetY = this->EnemyVis[j].y;
					// 简单地通过曼哈顿距离来衡量是否要设置为追击目标
					int manhattanDistance = abs(myX - targetX) + abs(myY - targetY);
					if (manhattanDistance < nearestDistance) {
						nearestDistance = manhattanDistance;
						nearestKun = this->OurKun[i];
						nearestEnemy = this->EnemyVis[j];
						nearestKunIndex = i;
					}
				}
			}

			// 2. 确定横向压迫者、纵向压迫者、协助者(堵截黑洞)
			Attacker[nearestKunIndex] = EATTER;
			int role = 0;
			for (int i = 0; i < this->ourkunnum; ++i) {
				if (i == nearestKunIndex) {
					continue;
				}
				else {
					Attacker[i] = (ATTACKER)(1 + role++);
					LOGE("鲲鲲%d号被分为%d", this->OurKun[i].id, Attacker[i]);
				}
			}

			// 3. 更新骚扰算法需要的变量、更新攻击回合ID和存储被围捕的敌方鲲
			this->harassingUnit.clear();
			this->harassingUnit.push_back(nearestKun);
			this->harassingEnm.clear();
			this->harassingEnm.push_back(nearestEnemy);
			this->CheckSituation();

			LOGE("我们失去了那个鲲. 但是我们找到了新的鲲. 其id为%d", nearestEnemy.id);
			this->targetKun = nearestEnemy;
		}

		// 查看EATTER, PORTRAITER, TRANSVERSER是否能包围敌人
		LOGE("计算我们的势力范围是否能将敌人围住");
		// 找到其他鲲
		Player eatter;
		Player portraiter;
		Player transverser;
		int portraiterIndex = -1;
		int transverserIndex = -1;
		int eatterIndex = -1;
		for (int i = 0; i < this->ourkunnum; ++i) {
			if (this->Attacker[i] == PORTRAITER) {
				portraiter = this->OurKun[i];
				portraiterIndex = i;
			}
			else if (this->Attacker[i] == TRANSVERSER) {
				transverser = this->OurKun[i];
				transverserIndex = i;
			}
			else if (this->Attacker[i] == EATTER) {
				eatter = this->OurKun[i];
				eatterIndex = i;
			}
		}
		vector<Player> helpers;
		helpers.push_back(eatter);
		helpers.push_back(portraiter);
		helpers.push_back(transverser);
		vector<MapItem> result = this->map->RotatingDiffusion(helpers, this->targetKun, this->localLeg->getVision());

		if (result.size() != 0) {
			// 我们可以把他包围住
			this->surroundMode = true;
			return;
		}


		// 查看EATTER, PORTRAITER, TRANSVERSER的视野是否能包括敌人在视野步长内能到达的所有点
		/*
		int vision = this->localLeg->getVision();
		vector<MapItem> enemyPossiblePoints = this->map->getPeripheral(this->targetKun.x, this->targetKun.y, vision - 1);
		Player eatter;
		Player portraiter;
		int portraiterIndex = -1;
		Player transverser;
		int transverserIndex = -1;
		int eatterIndex = -1;
		for (int i = 0; i < this->ourkunnum; ++i) {
			if (this->Attacker[i] == PORTRAITER) {
				portraiter = this->OurKun[i];
				portraiterIndex = i;
			}
			else if (this->Attacker[i] == TRANSVERSER) {
				transverser = this->OurKun[i];
				transverserIndex = i;
			}
			else if (this->Attacker[i] == EATTER) {
				eatter = this->OurKun[i];
				eatterIndex = i;
			}
		}

		bool seen = true;
		for (vector<MapItem>::iterator it = enemyPossiblePoints.begin(); it != enemyPossiblePoints.end(); ++it) {
			if ((abs((*it).x - eatter.x) > vision || abs((*it).y - eatter.y) > vision) &&
				(abs((*it).x - portraiter.x) > vision || abs((*it).y - portraiter.y) > vision) &&
				(abs((*it).x - transverser.x) > vision || abs((*it).y - transverser.y) > vision)
				) {
				LOGE("对EATTER、PORTRAITER和TRANSVERSER来说, 敌方两步能到达的点, 不全都在我眼皮底下, 我不可以承担另外一个人的角色");
				seen = false;
				break;
			}
		}

		if (seen) {
			LOGE("条件成熟了, 可以调换EATTER和压迫者的角色");

			// 查看另外两个压迫者视野是否能够包含敌人
			// todo: 查看是否能在被吃光的情况下正常工作
			bool portraiterNotSeen = false;
			bool transverserNotSeen = false;

			if (abs(this->targetKun.x - portraiter.x) > vision || abs(this->targetKun.y - portraiter.y) > vision) {
				portraiterNotSeen = true;
			}

			if (abs(this->targetKun.x - transverser.x) > vision || abs(this->targetKun.y - transverser.y) > vision) {
				transverserNotSeen = true;
			}

			if (portraiterNotSeen && transverserNotSeen) { // 两者都没看到, EATTER不与任何人交换身份
				LOGE("两者都没看到, EATTER不与任何人交换");
			}
			else if (portraiterNotSeen) {
				// 仅有portraiter没看见敌人, 如果可以的话, EATTER与portraiter交换一下角色
				LOGE("此时EATTER和纵向压迫者交换角色了");
				this->Attacker[eatterIndex] = PORTRAITER;
				this->Attacker[portraiterIndex] = EATTER;
			}
			else if (transverserNotSeen) {
				// 仅有transverser没看见敌人, 如果可以的话, EATTER与transverser交换一下角色
				LOGE("此时EATTER和横向压迫者交换角色了");
				this->Attacker[eatterIndex] = TRANSVERSER;
				this->Attacker[transverserIndex] = EATTER;
			}
			else {
				// 大家都看到了, 不用多此一举
				LOGE("大家都看到了, EATTER不用多此一举");
			}
		}
		*/


		// 查看EATTER的视野范围内是否能包含敌人在视野步长内能到达的所有点
		/*
		int vision = this->localLeg->getVision();
		vector<MapItem> enemyPossiblePoints = this->map->getPeripheral(this->targetKun.x, this->targetKun.y, vision - 1);
		Player eatter;
		int eatterIndex = -1;
		for (int i = 0; i < this->ourkunnum; ++i) {
			if (this->Attacker[i] == EATTER) {
				eatter = this->OurKun[i];
				eatterIndex = i;
				break;
			}
		}

		bool seen = true;
		for (vector<MapItem>::iterator it = enemyPossiblePoints.begin(); it != enemyPossiblePoints.end(); ++it) {
			if (abs((*it).x - eatter.x) > vision || abs((*it).y - eatter.y) > vision) {
				LOGE("我是Eatter, 敌方两步能到达的点, 不全都在我眼皮底下, 我不可以承担另外一个人的角色");
				seen = false;
				break;
			}
		}

		if (seen) {
			LOGE("我是Eatter, 条件成熟了, 派我去吧");

			// 查看另外两个压迫者视野是否能够包含敌人
			// todo: 查看是否能在被吃光的情况下正常工作
			bool portraiterNotSeen = false;
			bool transverserNotSeen = false;
			Player portraiter;
			int portraiterIndex = -1;
			Player transverser;
			int transverserIndex = -1;
			for (int i = 0; i < this->ourkunnum; ++i) {
				if (this->Attacker[i] == PORTRAITER) {
					portraiter = this->OurKun[i];
					portraiterIndex = i;
				}
				else if (this->Attacker[i] == TRANSVERSER) {
					transverser = this->OurKun[i];
					transverserIndex = i;
				}
			}

			if (abs(this->targetKun.x - portraiter.x) > vision || abs(this->targetKun.y - portraiter.y) > vision) {
				portraiterNotSeen = true;
			}

			if (abs(this->targetKun.x - transverser.x) > vision || abs(this->targetKun.y - transverser.y) > vision) {
				transverserNotSeen = true;
			}

			if (portraiterNotSeen && transverserNotSeen) { // 两者都没看到, EATTER不与任何人交换身份
				LOGE("两者都没看到, EATTER不与任何人交换");
			}
			else if (portraiterNotSeen) {
				// 仅有portraiter没看见敌人, 如果可以的话, EATTER与portraiter交换一下角色
				LOGE("此时EATTER和纵向压迫者交换角色了");
				this->Attacker[eatterIndex] = PORTRAITER;
				this->Attacker[portraiterIndex] = EATTER;
			}
			else if (transverserNotSeen) {
				// 仅有transverser没看见敌人, 如果可以的话, EATTER与transverser交换一下角色
				LOGE("此时EATTER和横向压迫者交换角色了");
				this->Attacker[eatterIndex] = TRANSVERSER;
				this->Attacker[transverserIndex] = EATTER;
			}
			else {
				// 大家都看到了, 不用多此一举
				LOGE("大家都看到了, EATTER不用多此一举");
			}
		}*/

		// 安排一下压迫点
		/*
		LOGE("获取压迫点的阈值设置为%d", this->minPressureThreshold);
		vector<MapItem> pressureMapItem = this->map->getPressurePoint(this->targetKun.x, this->targetKun.y, this->minPressureThreshold);

		int pressurePointsCount = 0;
		for (vector<MapItem>::iterator it = pressureMapItem.begin(); it != pressureMapItem.end(); ++it) {
			this->pressurePoints[pressurePointsCount].x = (*it).x;
			this->pressurePoints[pressurePointsCount++].y = (*it).y;
		}

		for (int i = 0; i < this->ourkunnum; ++i) {
			if (this->Attacker[i] == EATTER) { continue; }
			MapItem minMapItem;
			int minDistance = 999;
			int pressurePointsIndex = -1;
			pressurePointsCount = 0;
			for (vector<MapItem>::iterator it = pressureMapItem.begin(); it != pressureMapItem.end(); ++it, ++pressurePointsCount) {
				if (this->pressurePoints[pressurePointsCount].ownerId != -1) { continue; }
				int tempDistance = this->map->FindRealDistance(this->OurKun[i].x, this->OurKun[i].y, (*it).x, (*it).y);
				if (tempDistance < minDistance) {
					minDistance = tempDistance;
					minMapItem = *it;
					pressurePointsIndex = pressurePointsCount;
				}
			}
			if (minDistance != 999) {
				LOGE("返回的压迫点中的第%d个已经分配给了鲲鲲%d号", pressurePointsIndex, i);
				this->pressurePoints[pressurePointsIndex].x = minMapItem.x;
				this->pressurePoints[pressurePointsIndex].y = minMapItem.y;
				this->pressurePoints[pressurePointsIndex].ownerId = i;
			}
		}*/
	}
	else {
		// 作死reset一下区域
		this->map->resetArea();

		this->attackRoundId = this->localRound->getRoundId();
		// 视野内存在敌人, 且 攻击回合数与当前回合数相差不止为1, 则证明已经有一些回合没有追击敌人了. 也就是要初始化追捕团队

		// 也许一开场就遇到了敌人[滑稽]
		if (this->Attacker == NULL) {
			this->Attacker = new int[this->ourkunnum];
			this->attackerDirections = new int[this->ourkunnum];
			for (int i = 0; i < this->ourkunnum; ++i) {
				Attacker[i] = FINDER;
				attackerDirections[i] = NO_MOVE;
			}
		}

		LOGE("Enemy discovered. Attack. ");

		// this->minPressureThreshold = 3;

		// 1. 确定主要追捕者.
		int nearestKunIndex = -1;
		Player nearestKun;
		Player nearestEnemy;
		int nearestDistance = 1000;
		for (int i = 0; i < this->ourkunnum; ++i) {
			this->GetEnemyVis(this->OurKun[i].id);
			int myX = this->OurKun[i].x;
			int myY = this->OurKun[i].y;
			for (int j = 0; j < this->EnemyVisNum; ++j) {
				int targetX = this->EnemyVis[j].x;
				int targetY = this->EnemyVis[j].y;
				// 简单地通过曼哈顿距离来衡量是否要设置为追击目标
				int manhattanDistance = abs(myX - targetX) + abs(myY - targetY);
				if (manhattanDistance < nearestDistance) {
					nearestDistance = manhattanDistance;
					nearestKun = this->OurKun[i];
					nearestEnemy = this->EnemyVis[j];
					nearestKunIndex = i;
				}
			}
		}

		// 2. 确定横向压迫者、纵向压迫者、协助者(堵截黑洞)
		Attacker[nearestKunIndex] = EATTER;
		int role = 0;
		for (int i = 0; i < this->ourkunnum; ++i) {
			if (i == nearestKunIndex) {
				continue;
			}
			else {
				Attacker[i] = (ATTACKER)(1 + role++);
				LOGE("鲲鲲%d号被分为%d", this->OurKun[i].id, Attacker[i]);
			}
		}

		// 2.1 避免他们占了Area的位置
		for (int i = 0; i < this->ourkunnum; ++i) {
			if (Attacker[i] == HELPER) { continue; }
			if (this->OurKun[i].patrolRoundId + 1 == this->localRound->getRoundId()) {
				Point targetPoint;
				targetPoint.x = this->OurKun[i].patrolPointIndex % this->map->getWidth();
				targetPoint.y = this->OurKun[i].patrolPointIndex / this->map->getHeight();
				Point areaPoint = this->map->getAreaByCoord(targetPoint.x, targetPoint.y);
				this->map->deoccupyArea(areaPoint.x, areaPoint.y);
			}
		}

		// 3. 更新骚扰算法需要的变量、更新攻击回合ID和存储被围捕的敌方鲲
		this->harassingUnit.clear();
		this->harassingUnit.push_back(nearestKun);
		this->harassingEnm.clear();
		this->harassingEnm.push_back(nearestEnemy);
		this->CheckSituation();

		LOGE("初始化追捕团队的时候设定了this->targetKun");
		this->targetKun = nearestEnemy;
	}
}

void OurStrategy::initEnemyKun()
{
	GuessEnemyKun[0].x = 0;
	GuessEnemyKun[0].y = 0;
	GuessEnemyKun[0].id = -1;

	GuessEnemyKun[1].x = 0;
	GuessEnemyKun[1].y = this->localLeg->getHeight() - 1;
	GuessEnemyKun[1].id = -1;

	GuessEnemyKun[2].x = this->localLeg->getWidth() - 1;
	GuessEnemyKun[2].y = 0;
	GuessEnemyKun[2].id = -1;

	GuessEnemyKun[3].x = this->localLeg->getWidth() - 1;
	GuessEnemyKun[3].y = this->localLeg->getHeight() - 1;
	GuessEnemyKun[3].id = -1;

}

void OurStrategy::updateEnemyKun()
{
	if (enemykunnum == 0)
		return;
	else
	{
		for (int i = 0; i < enemykunnum; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (GuessEnemyKun[j].id == -1 || EnemyKun[i].id == GuessEnemyKun[j].id)
				{
					GuessEnemyKun[j] = EnemyKun[i];
					break;
				}
			}
		}
		for (int i = 0; i < 4; i++)
		{
			LOGE("enemykun update %d:(%d,%d)", GuessEnemyKun[i].id, GuessEnemyKun[i].x, GuessEnemyKun[i].y);
		}
	}
}

void OurStrategy::updateLeg(LegStartMsg* t)
{
	//delete[] localLeg;
	this->localLeg = t;
	if (this->map == NULL)
	{
		map = new Map();
	}
	map->UpdateMap(t);
	// 设定我方势力
	this->myForce = t->getMyForce(this->myTeamId);
	this->map->resetArea();

	for (int i = 0; i < ourkunnum; ++i) {
		this->OurKun[i].patrolPointIndex = -1;
		this->OurKun[i].patrolRoundId = -2;
	}

	//this->diffuseThreshold = 2 * t->getVision();
	this->diffuseThreshold = 150;

	isHarass = false;
	isPursuing = false;
	remainingSteps = -1;
	harassingEnm.clear();
	pursuingEnm.clear();
	lastHarPos.clear();
	lastPurPos.clear();
	pursuingUnit.clear();
	pursuingDecision.clear();
	harassingUnit.clear();
	harassingDecision.clear();
}

void OurStrategy::updateRound(RoundMsg* t)
{
	//delete[] localRound;
	this->localRound = t;
	this->setSituation(t->round_id, t->getMode() == this->getMyForce());
}


int OurStrategy::getMyTeamId()
{
	return this->myTeamId;
}

void OurStrategy::initKunBlock(int pid_1, int pid_2, int pid_3, int pid_4)
{
	kunblock[0].pid = pid_1;
	kunblock[1].pid = pid_2;
	kunblock[2].pid = pid_3;
	kunblock[3].pid = pid_4;
	for (int i = 0; i < 4; i++)
	{
		kunblock[i].isBlock = false;
		kunblock[i].BlockTar.x = 0;
		kunblock[i].BlockTar.y = 0;
	}
}

string OurStrategy::getMyForce()
{
	return this->myForce;
}

// 更新当前战局情况
void OurStrategy::setSituation(int roundId, bool benificial)
{
	if (!benificial) {
		// 劣势
		LOGE("防守！！！！\r\n");
		this->turn = DEFENSE;
	}
	else {
		// 优势
		if (roundId >= 100 && roundId < 150 || roundId >= 250 && this->getMyForce() == "think") {
			// 优势局的后半段, 但是要略去一部分, 该部分是我方为火鲲时出现的情况
			this->turn = ATTACK_NEGATVE;
		}
		else {
			this->turn = ATTACK_ACTIVE;
		}
	}
}

//区分双方kun
void OurStrategy::DistinguishPlayers(int pid_1, int pid_2, int pid_3, int pid_4)
{
	// copy CURRENT players
	Player* copiedPlayers = new Player[this->ourkunnum];
	for (unsigned short i = 0; i < this->ourkunnum; ++i) {
		copiedPlayers[i] = this->OurKun[i];
	}
	int formerKunNum = this->ourkunnum;

	Player* tmp = this->localRound->getPlayers();//获取原players数组
	ourkunnum = 0;
	enemykunnum = 0;
	for (int i = 0; i < this->localRound->getPlayersCount(); i++)//第一次遍历记录下敌我kun的数量
	{
		if (tmp[i].id == pid_1 || tmp[i].id == pid_2 || tmp[i].id == pid_3 || tmp[i].id == pid_4)
			ourkunnum++;
		else
			enemykunnum++;
	}
	//LOGE("%d，%d，%d，%d\n", pid_1, pid_2, pid_3, pid_4);
	//LOGE("（%d，%d）\n", ourkunnum,enemykunnum);

	//创建敌我kun数组并重置num值
	if (this->OurKun)
	{
		delete[] this->OurKun;//释放指针指向的资源
		this->OurKun = NULL;
	}
	if (ourkunnum != 0)
		this->OurKun = new Player[ourkunnum];

	if (this->EnemyKun)
	{
		delete[] this->EnemyKun;//释放指针指向的资源
		this->EnemyKun = NULL;
	}

	if (enemykunnum != 0) {
		this->EnemyKun = new Player[enemykunnum];
	}
	ourkunnum = 0;
	enemykunnum = 0;

	for (int i = 0; i < this->localRound->getPlayersCount(); i++)//第二次遍历为敌我kun数组赋值
	{
		if (tmp[i].id == pid_1 || tmp[i].id == pid_2 || tmp[i].id == pid_3 || tmp[i].id == pid_4)
		{
			for (unsigned short j = 0; j < formerKunNum; ++j) {
				if (copiedPlayers[j].id == tmp[i].id) {
					tmp[i].patrolPointIndex = copiedPlayers[j].patrolPointIndex;
					tmp[i].patrolRoundId = copiedPlayers[j].patrolRoundId;
					break;
				}
			}
			OurKun[ourkunnum] = tmp[i];
			ourkunnum++;
		}
		else
		{
			EnemyKun[enemykunnum] = tmp[i];
			enemykunnum++;
		}
	}
	for (int i = 0; i < enemykunnum; i++)
	{
		LOGE("敌方%d号棋子（%d，%d）", EnemyKun[i].id, EnemyKun[i].x, EnemyKun[i].y);
	}
	for (int i = 0; i < ourkunnum; i++)
	{
		LOGE("我方%d号棋子（%d，%d）", OurKun[i].id, OurKun[i].x, OurKun[i].y);
	}
	Power* allpow = this->localRound->getPowers();
	int pownum = this->localRound->getPowersCount();
	for (int i = 0; i < pownum; i++)
	{
		LOGE("%d号能量点；（%d,%d）；%d 分", i, allpow->x, allpow->y, allpow->point);
	}

	delete[] copiedPlayers;
}

//为地图添加频繁度
void  OurStrategy::AddMapFrequency(int pid)
{
	Player thisplayer;
	int i = 0;
	for (; i < this->ourkunnum; i++)//找到对应编号的kun
	{
		if (OurKun[i].id == pid)
		{
			thisplayer = OurKun[i];
			break;
		}
	}
	if (i != ourkunnum)//如果这个鲲没扑街，为所在区域增加一点访问度
	{
		//LOGE("-----(%d,%d)-----\d", thisplayer.x, thisplayer.y);
		map->AddFrequency(thisplayer.x, thisplayer.y);
	}
}

//获取我方指定kun视野内的敌方kun
void OurStrategy::GetEnemyVis(int pid)
{
	EnemyVisNum = 0;
	int vis = this->localLeg->getVision();
	for (int i = 0; i < enemykunnum; i++)//计算范围内可见敌方kun数量
	{
		Player thiskun = OurKun[ChangePid(pid)];
		int absx = abs(thiskun.x - EnemyKun[i].x);
		int absy = abs(thiskun.y - EnemyKun[i].y);
		if (absx <= vis && absy <= vis)
			EnemyVisNum++;
	}
	if (this->EnemyVis)
	{
		delete[] this->EnemyVis;//释放指针指向的资源
		this->EnemyVis = NULL;
	}
	if (EnemyVisNum != 0)//建立可见范围内敌方kun数组并赋值
	{
		this->EnemyVis = new Player[EnemyVisNum];
		int k = 0;
		for (int i = 0; i < enemykunnum; i++)
		{
			Player thiskun = OurKun[ChangePid(pid)];
			int absx = abs(thiskun.x - EnemyKun[i].x);
			int absy = abs(thiskun.y - EnemyKun[i].y);
			if (absx <= vis && absy <= vis)
			{
				EnemyVis[k] = EnemyKun[i];
				k++;
			}
		}
	}
}

//获取我方指定kun视野内的能量点
void OurStrategy::GetPowerVis(int pid)
{
	PowerVisNum = 0;
	int vis = this->localLeg->getVision();
	Power* allpow = this->localRound->getPowers();
	int pownum = this->localRound->getPowersCount();
	for (int i = 0; i < pownum; i++)//计算范围内可见能量数量
	{
		Player thiskun = OurKun[ChangePid(pid)];
		Power apow = allpow[i];
		//LOGE("powers:%d,%d", allpow->x, allpow->y);
		int absx = abs(thiskun.x - apow.x);
		int absy = abs(thiskun.y - apow.y);
		if (absx <= vis && absy <= vis)
			PowerVisNum++;
	}
	if (this->PowerVis)
	{
		delete[] this->PowerVis;//释放指针指向的资源
		this->PowerVis = NULL;
	}
	if (PowerVisNum != 0)//建立可见范围内能量数组并赋值
	{
		this->PowerVis = new Power[PowerVisNum];
		int k = 0;
		for (int i = 0; i < pownum; i++)
		{
			Player thiskun = OurKun[ChangePid(pid)];
			Power apow = allpow[i];
			int absx = abs(thiskun.x - apow.x);
			int absy = abs(thiskun.y - apow.y);
			if (absx <= vis && absy <= vis)
			{
				PowerVis[k] = allpow[i];
				k++;
			}
		}
	}
}

//根据pid值返回对应的kun数组下标
int OurStrategy::ChangePid(int pid)
{
	int i = 0;
	for (; i < this->ourkunnum; i++)//找到对应编号的kun
	{
		if (OurKun[i].id == pid)
		{
			break;
		}
	}
	if (i == ourkunnum)
	{
		return -1;//如果有kun被吃了
	}
	else
		return i;
}

int OurStrategy::ChangeBlockPid(int pid)
{
	for (int i = 0; i < 4; i++)
	{
		if (kunblock[i].pid == pid)
		{
			return i;
		}
	}
	return -1;
}

//根据视野内能量点数组寻找最近能量点
Power OurStrategy::FindNearestPow(Player thiskun)
{
	Power* tmp = PowerVis;//获取能量点
	int* close = new int[PowerVisNum];//建立能量点距离数组
	for (int i = 0; i < PowerVisNum; i++)
	{
		close[i] = abs(thiskun.x - tmp[i].x) + abs(thiskun.y - tmp[i].y);
	}
	int least = 0;
	for (int i = 0; i < PowerVisNum - 1; i++)//找到最近的能量点
	{
		if (close[i + 1] < close[least])
			least = i + 1;
	}
	delete[] close;
	return tmp[least];//返回最近的能量点
}

bool OurStrategy::CheckPow(int x, int y)
{
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x > this->localLeg->getWidth() - 1)
		x = this->localLeg->getWidth() - 1;
	if (y > this->localLeg->getHeight() - 1)
		y = this->localLeg->getHeight() - 1;

	for (int i = 0; i < PowerVisNum; i++)
	{
		if (PowerVis[i].x == x && PowerVis[i].y == y)
			return true;
	}
	return false;
}

//通常决策
DIRECT OurStrategy::Normal(Player thisplayer)
{
	//新增代码, 未经测试
	LOGE("<<<Enter Patrol Rountine>>>");
	Point targetPoint = this->FollowPatrolRoutine(thisplayer);
	// 遍历players, 准确找到该player, 更改其patrol属性.
	for (unsigned short i = 0; i < this->ourkunnum; ++i) {
		if (this->OurKun[i].id == thisplayer.id) {
			int patrolPointIndex = this->map->getHeight() * targetPoint.y + targetPoint.x;
			if (this->OurKun[i].patrolPointIndex != patrolPointIndex) {
				this->OurKun[i].patrolPointIndex = patrolPointIndex;
			}
			this->OurKun[i].patrolRoundId = this->localRound->round_id; // patrolRoundId无论如何都是要更新的, 但是patroPointIndex不一样
			break;
		}
	}

	//// todo: 现在是全图搜索, 没有可以规避危险区域和重叠区域
	LOGE("寻路的参数为-thisplayer的位置:(%d, %d), targetPoint的位置:(%d, %d)", thisplayer.x, thisplayer.y, targetPoint.x, targetPoint.y);
	return map->pathFind(thisplayer.x, thisplayer.y, targetPoint.x, targetPoint.y);

	// 下面是历史代码
	//正常流程,调试时请注释掉
	if (PowerVisNum == 0)//视野内无能量点则执行漫步搜寻策略
	{
		LOGE("<<<WALK>>>");
		Point tarpoi = NowTo(thisplayer);
		return map->pathFind(thisplayer.x, thisplayer.y, tarpoi.x, tarpoi.y);
	}
	else//视野内有能量则去吃能量
	{
		LOGE("thisplayer.id=%d", thisplayer.id);
		int tmp = ChangeBlockPid(thisplayer.id);
		LOGE("tmp=%d", tmp);
		kunblock[tmp].isBlock = false;
		Power nearestPow = FindNearestPow(thisplayer);
		LOGE("<<<WLAK>>>try to eat(%d,%d)", nearestPow.x, nearestPow.y);
		return map->pathFind(thisplayer.x, thisplayer.y, nearestPow.x, nearestPow.y);
	}
	//正常流程
}

//找到距离kun最近的边界
DIRECT OurStrategy::FindClosestEdge(int x, int y)
{
	//巡游模式：
	//找到最接近的一条边
	int width = this->localLeg->getWidth();
	int height = this->localLeg->getHeight();
	if (x < width / 2)
	{
		if (y < height / 2)
			return (x >= y ? MOVE_UP : MOVE_LEFT);
		else
			return (x > (height - y - 1) ? MOVE_DOWN : MOVE_LEFT);
	}
	else
	{
		if (y < height / 2)
			return ((width - x - 1) > y ? MOVE_UP : MOVE_RIGHT);
		else
			return ((width - x - 1) >= (height - y - 1) ? MOVE_DOWN : MOVE_RIGHT);
	}
}

//按照指定半径轨迹运动
DIRECT OurStrategy::CheckOnRoute(int x, int y, int radius, Clock wise)
{
	int width = this->localLeg->getWidth();
	int height = this->localLeg->getHeight();
	DIRECT m_u, m_d, m_l, m_r;
	if (wise == CLOCK_WISE)//顺时针
	{
		m_u = MOVE_RIGHT;
		m_d = MOVE_LEFT;
		m_l = MOVE_UP;
		m_r = MOVE_DOWN;
	}
	else
	{
		m_u = MOVE_LEFT;
		m_d = MOVE_RIGHT;
		m_l = MOVE_DOWN;
		m_r = MOVE_UP;
	}
	//int vision = this->localLeg->getVision()-3;
	 // LOGE("------y：%d  vision：%d direct：%d \n", y, vision,FindClosestEdge(x, y));
	if (FindClosestEdge(x, y) == MOVE_UP)
	{
		if (y > radius)
			return MOVE_UP;
		if (y < radius)
			return MOVE_DOWN;
		else
			return m_u;
	}
	if (FindClosestEdge(x, y) == MOVE_DOWN)
	{
		if (height - y - 1 > radius)
			return MOVE_DOWN;
		if (height - y - 1 < radius)
			return MOVE_UP;
		else
			return m_d;
	}
	if (FindClosestEdge(x, y) == MOVE_LEFT)
	{
		if (x > radius)
			return MOVE_LEFT;
		if (x < radius)
			return MOVE_RIGHT;
		else
			return m_l;
	}
	if (FindClosestEdge(x, y) == MOVE_RIGHT)
	{
		if (width - x - 1 > radius)
			return MOVE_RIGHT;
		if (width - x - 1 < radius)
			return MOVE_LEFT;
		else
			return m_r;
	}
	return NO_MOVE;
}

//漫步搜寻决策
Point OurStrategy::NowTo(Player thiskun)
{
	Point poi = kunblock[ChangeBlockPid(thiskun.id)].BlockTar;
	int xr = thiskun.x;
	int yr = thiskun.y;
	if ((thiskun.x == poi.x) && (thiskun.y == poi.y))//如果已经到达目标点，则取消阻碍标志
	{
		LOGE("arrive!(%d,%d),(%d,%d)", thiskun.x, thiskun.y, poi.x, poi.y);
		kunblock[ChangeBlockPid(thiskun.id)].isBlock = false;
	}
	if (kunblock[ChangeBlockPid(thiskun.id)].isBlock == true)//如果已经有阻碍标志，则继续前往目标点
		return  kunblock[ChangeBlockPid(thiskun.id)].BlockTar;

	DIRECT basdir = CheckOnRoute(xr, yr, this->localLeg->getVision(), CLOCK_WISE);//找到前往下一个目标点所需的方向
	if (basdir == MOVE_UP)
	{
		yr = yr - 1;
	}
	else if (basdir == MOVE_DOWN)
	{
		yr = yr + 1;
	}
	else if (basdir == MOVE_LEFT)
	{
		xr = xr - 1;
	}
	else
	{
		xr = xr + 1;
	}//找到下一个目标点的位置
	if (map->IsReach(xr, yr))//如果下一个目标点可以到达
	{
		poi.x = xr;
		poi.y = yr;
		return poi;
	}
	while (!map->IsReach(xr, yr))//寻找下一个可以到达的点
	{
		basdir = CheckOnRoute(xr, yr, this->localLeg->getVision(), CLOCK_WISE);
		if (basdir == MOVE_UP)
		{
			yr = yr - 1;
		}
		else if (basdir == MOVE_DOWN)
		{
			yr = yr + 1;
		}
		else if (basdir == MOVE_LEFT)
		{
			xr = xr - 1;
		}
		else
		{
			xr = xr + 1;
		}
		//LOGE("process:(%d,%d)\n", xr, yr);
	}
	kunblock[ChangeBlockPid(thiskun.id)].isBlock = true;
	kunblock[ChangeBlockPid(thiskun.id)].BlockTar.x = xr;
	kunblock[ChangeBlockPid(thiskun.id)].BlockTar.y = yr;
	LOGE("block!(%d,%d)try to(%d,%d)", thiskun.x, thiskun.y, xr, yr);
	poi.x = xr;
	poi.y = yr;
	return poi;
}

//逃跑总决策
DIRECT  OurStrategy::Run(Player thisplayer)
{
	LOGE("<<< RUN >>>");
	if (dangerousRun())
	{
		return CrazyRun(thisplayer);
	}
	else
	{
		if (PowerVisNum != 0)//&& EnemyVisNum == 1)
			return CarefreeRun(thisplayer);
		else
			return UrgentRun(thisplayer);
	}
}

//逃跑驱散更新
void OurStrategy::UpdateDispersed()
{
	//数组初始化
	if (this->dispersed)
	{
		delete[] this->dispersed;//释放指针指向的资源
		this->dispersed = NULL;
	}
	if (ourkunnum == 0)//没有kun就提前结束
		return;
	this->dispersed = new bool[ourkunnum];
	for (int i = 0; i < ourkunnum; i++)
		this->dispersed[i] = false;


	if (ourkunnum == 1)//一只kun就算了
		return;
	for (int i = 0; i < ourkunnum - 1; i++)//为后面与自己一样坐标的kun标记
	{
		for (int j = i; j < ourkunnum - 1; j++)
		{
			if (OurKun[i].x == OurKun[j + 1].x && OurKun[i].y == OurKun[j + 1].y)
			{
				dispersed[j + 1] = true;
				break;
			}
		}
	}
	for (int i = 0; i < ourkunnum; i++)//打印查看效果
	{
		if (dispersed[i] == true)
		{
			LOGE("true");
		}
		else
		{
			LOGE("false");
		}
	}
}

//悠闲逃跑决策
DIRECT  OurStrategy::CarefreeRun(Player thisplayer)
{
	LOGE("<<< RUN-CarefreeRun >>>");
	Power nearestPow = FindNearestPow(thisplayer);
	LOGE("test pow(%d,%d) is safe or not", nearestPow.x, nearestPow.y);
	if (!map->EatSafety(nearestPow.x, nearestPow.y))
	{
		LOGE("not safe");
		return UrgentRun(thisplayer);
	}
	LOGE("safe");
	LOGE("now test if can go", nearestPow.x, nearestPow.y);
	int x = thisplayer.x;
	int y = thisplayer.y;
	int mydistant = map->FindRealDistance(x, y, nearestPow.x, nearestPow.y, TEST_THRESHOLD);
	//LOGE("your pos is (%d,%d),the distance is%d", x, y, mydistant);
	for (int i = 0; i < this->enemykunnum; i++)
	{
		int enemydistant = map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, nearestPow.x, nearestPow.y, TEST_THRESHOLD);
		//LOGE("enemy pos is (%d,%d),the distance is%d",EnemyKun[i].x, EnemyKun[i].y, enemydistant);
		if (enemydistant <= mydistant)
		{
			LOGE("no");
			LOGE("your pos is (%d,%d),the distance is%d", x, y, mydistant);
			LOGE("enemy pos is (%d,%d),the distance is%d", EnemyKun[i].x, EnemyKun[i].y, enemydistant);
			return UrgentRun(thisplayer);
		}

	}

	LOGE("yes");
	return map->pathFind(thisplayer.x, thisplayer.y, nearestPow.x, nearestPow.y);

}

//紧急逃跑决策
DIRECT  OurStrategy::UrgentRun(Player thisplayer)
{
	LOGE("<<< RUN-UrgentRun >>>");
	int x = thisplayer.x;
	int y = thisplayer.y;

	//设置方向数组
	DIRECT fourDir[4] = { MOVE_UP,MOVE_DOWN,MOVE_LEFT,MOVE_RIGHT };
	//设置化距离权重
	int fourDistant[4];
	{
		fourDistant[0] = y;
		fourDistant[1] = this->localLeg->getHeight() - y - 1;
		fourDistant[2] = x;
		fourDistant[3] = this->localLeg->getWidth() - x - 1;
	}
	//设置威胁方向权重
	int fourFeq[4] = { 0,0,0,0 };
	for (int i = 0; i < 4; i++)
	{
		int enemy_x = GuessEnemyKun[i].x;
		int enemy_y = GuessEnemyKun[i].y;
		if (x > enemy_x)
		{
			if (y > enemy_y)
			{
				if ((y - enemy_y) > (x - enemy_x))
				{
					fourFeq[0] += 4;
					fourFeq[2] += 2;
				}
				else if ((y - enemy_y) < (x - enemy_x))
				{
					fourFeq[0] += 4;
					fourFeq[2] += 2;
				}
				else
				{
					fourFeq[0] += 3;
					fourFeq[2] += 3;
				}
			}
			else if (y < enemy_y)
			{
				if ((enemy_y - y) > (x - enemy_x))
				{
					fourFeq[1] += 4;
					fourFeq[2] += 2;
				}
				else if ((enemy_y - y) < (x - enemy_x))
				{
					fourFeq[1] += 4;
					fourFeq[2] += 2;
				}
				else
				{
					fourFeq[1] += 3;
					fourFeq[2] += 3;
				}
			}
			else
			{
				fourFeq[0] += 1;
				fourFeq[1] += 1;
				fourFeq[2] += 5;
			}
		}
		else if (x < enemy_x)
		{
			if (y > enemy_y)
			{
				if ((y - enemy_y) > (enemy_x - x))
				{
					fourFeq[0] += 4;
					fourFeq[3] += 2;
				}
				else if ((y - enemy_y) < (enemy_x - x))
				{
					fourFeq[0] += 4;
					fourFeq[3] += 2;
				}
				else
				{
					fourFeq[0] += 3;
					fourFeq[3] += 3;
				}
			}
			else if (y < enemy_y)
			{
				if ((enemy_y - y) > (enemy_x - x))
				{
					fourFeq[1] += 4;
					fourFeq[3] += 2;
				}
				else if ((enemy_y - y) < (enemy_x - x))
				{
					fourFeq[1] += 4;
					fourFeq[3] += 2;
				}
				else
				{
					fourFeq[1] += 3;
					fourFeq[3] += 3;
				}
			}
			else
			{
				fourFeq[0] += 1;
				fourFeq[1] += 1;
				fourFeq[3] += 5;
			}
		}
		else
		{
			if (y > enemy_y)
			{
				fourFeq[0] += 5;
				fourFeq[2] += 1;
				fourFeq[3] += 1;
			}
			else if (y < enemy_y)
			{
				fourFeq[1] += 5;
				fourFeq[2] += 1;
				fourFeq[3] += 1;
			}
			else
			{
				return NO_MOVE;//你已经死了
			}
		}
	}
	//设置能量点权重
	int fourPow[4] = { 0,0,0,0 };
	{
		if (CheckPow(x, y - 1))
			fourPow[0] = 1;
		if (CheckPow(x, y + 1))
			fourPow[1] = 1;
		if (CheckPow(x - 1, y))
			fourPow[2] = 1;
		if (CheckPow(x + 1, y))
			fourPow[3] = 1;
	}
	//设置优先检查权重
	int fourSeq[4] = { 0,0,0,0 };
	{
		if (y == 0) { ; }
		else if (map->GetMap(x, y - 1).type == 3)
			fourSeq[0] += 1;
		else if (map->GetMap(x, y - 1).type == 2)
			fourSeq[0] += 2;

		if (y == this->localLeg->getHeight() - 1) { ; }
		else if (map->GetMap(x, y + 1).type == 3)
			fourSeq[1] += 1;
		else if (map->GetMap(x, y + 1).type == 2)
			fourSeq[1] += 2;

		if (x == 0) { ; }
		else if (map->GetMap(x - 1, y).type == 3)
			fourSeq[2] += 1;
		else if (map->GetMap(x - 1, y).type == 2)
			fourSeq[2] += 2;

		if (x == this->localLeg->getWidth() - 1) { ; }
		else if (map->GetMap(x + 1, y).type == 3)
			fourSeq[3] += 1;
		else if (map->GetMap(x + 1, y).type == 2)
			fourSeq[3] += 2;
	}

	LOGE("Dir:\n  dir:(%d,%d,%d,%d)", fourDir[0], fourDir[1], fourDir[2], fourDir[3]);
	LOGE("  distant:(%d,%d,%d,%d)", fourDistant[0], fourDistant[1], fourDistant[2], fourDistant[3]);
	LOGE("  fequency:(%d,%d,%d,%d)", fourFeq[0], fourFeq[1], fourFeq[2], fourFeq[3]);
	LOGE("  power:(%d,%d,%d,%d)", fourPow[0], fourPow[1], fourPow[2], fourPow[3]);
	LOGE("  sequency:(%d,%d,%d,%d)", fourSeq[0], fourSeq[1], fourSeq[2], fourSeq[3]);
	//根据map距离重棑数组
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3 - i; j++)
		{
			if (fourDistant[j + 1] > fourDistant[j])
			{
				int temp = fourDistant[j + 1];
				fourDistant[j + 1] = fourDistant[j];
				fourDistant[j] = temp;

				DIRECT temp_1 = fourDir[j + 1];
				fourDir[j + 1] = fourDir[j];
				fourDir[j] = temp_1;

				int temp_2 = fourFeq[j + 1];
				fourFeq[j + 1] = fourFeq[j];
				fourFeq[j] = temp_2;

				int temp_3 = fourSeq[j + 1];
				fourSeq[j + 1] = fourSeq[j];
				fourSeq[j] = temp_3;

				int temp_4 = fourPow[j + 1];
				fourPow[j + 1] = fourPow[j];
				fourPow[j] = temp_4;
			}
		}
	}
	LOGE("Distant:\n  dir:(%d,%d,%d,%d)", fourDir[0], fourDir[1], fourDir[2], fourDir[3]);
	LOGE("  distant:(%d,%d,%d,%d)", fourDistant[0], fourDistant[1], fourDistant[2], fourDistant[3]);
	LOGE("  fequency:(%d,%d,%d,%d)", fourFeq[0], fourFeq[1], fourFeq[2], fourFeq[3]);
	LOGE("  power:(%d,%d,%d,%d)", fourPow[0], fourPow[1], fourPow[2], fourPow[3]);
	LOGE("  sequency:(%d,%d,%d,%d)", fourSeq[0], fourSeq[1], fourSeq[2], fourSeq[3]);
	//根据威胁大小重排数组
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3 - i; j++)
		{
			if (fourFeq[j + 1] < fourFeq[j])
			{
				int temp = fourDistant[j + 1];
				fourDistant[j + 1] = fourDistant[j];
				fourDistant[j] = temp;

				DIRECT temp_1 = fourDir[j + 1];
				fourDir[j + 1] = fourDir[j];
				fourDir[j] = temp_1;

				int temp_2 = fourFeq[j + 1];
				fourFeq[j + 1] = fourFeq[j];
				fourFeq[j] = temp_2;

				int temp_3 = fourSeq[j + 1];
				fourSeq[j + 1] = fourSeq[j];
				fourSeq[j] = temp_3;

				int temp_4 = fourPow[j + 1];
				fourPow[j + 1] = fourPow[j];
				fourPow[j] = temp_4;
			}
		}
	}
	LOGE("Fequency:\n  dir:(%d,%d,%d,%d)", fourDir[0], fourDir[1], fourDir[2], fourDir[3]);
	LOGE("  distant:(%d,%d,%d,%d)", fourDistant[0], fourDistant[1], fourDistant[2], fourDistant[3]);
	LOGE("  fequency:(%d,%d,%d,%d)", fourFeq[0], fourFeq[1], fourFeq[2], fourFeq[3]);
	LOGE("  power:(%d,%d,%d,%d)", fourPow[0], fourPow[1], fourPow[2], fourPow[3]);
	LOGE("  sequency:(%d,%d,%d,%d)", fourSeq[0], fourSeq[1], fourSeq[2], fourSeq[3]);
	//根据能量点重排数组
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3 - i; j++)
		{
			if (fourPow[j + 1] > fourPow[j])
			{
				int temp = fourDistant[j + 1];
				fourDistant[j + 1] = fourDistant[j];
				fourDistant[j] = temp;

				DIRECT temp_1 = fourDir[j + 1];
				fourDir[j + 1] = fourDir[j];
				fourDir[j] = temp_1;

				int temp_2 = fourFeq[j + 1];
				fourFeq[j + 1] = fourFeq[j];
				fourFeq[j] = temp_2;

				int temp_3 = fourSeq[j + 1];
				fourSeq[j + 1] = fourSeq[j];
				fourSeq[j] = temp_3;

				int temp_4 = fourPow[j + 1];
				fourPow[j + 1] = fourPow[j];
				fourPow[j] = temp_4;
			}
		}
	}
	LOGE("Power:\n  dir:(%d,%d,%d,%d)", fourDir[0], fourDir[1], fourDir[2], fourDir[3]);
	LOGE("  distant:(%d,%d,%d,%d)", fourDistant[0], fourDistant[1], fourDistant[2], fourDistant[3]);
	LOGE("  fequency:(%d,%d,%d,%d)", fourFeq[0], fourFeq[1], fourFeq[2], fourFeq[3]);
	LOGE("  power:(%d,%d,%d,%d)", fourPow[0], fourPow[1], fourPow[2], fourPow[3]);
	LOGE("  sequency:(%d,%d,%d,%d)", fourSeq[0], fourSeq[1], fourSeq[2], fourSeq[3]);
	//根据优先检查重排数组
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3 - i; j++)
		{
			if (fourSeq[j + 1] > fourSeq[j])
			{
				int temp = fourDistant[j + 1];
				fourDistant[j + 1] = fourDistant[j];
				fourDistant[j] = temp;

				DIRECT temp_1 = fourDir[j + 1];
				fourDir[j + 1] = fourDir[j];
				fourDir[j] = temp_1;

				int temp_2 = fourFeq[j + 1];
				fourFeq[j + 1] = fourFeq[j];
				fourFeq[j] = temp_2;

				int temp_3 = fourSeq[j + 1];
				fourSeq[j + 1] = fourSeq[j];
				fourSeq[j] = temp_3;

				int temp_4 = fourPow[j + 1];
				fourPow[j + 1] = fourPow[j];
				fourPow[j] = temp_4;
			}
		}
	}
	LOGE("Sequency:\n  dir:(%d,%d,%d,%d)", fourDir[0], fourDir[1], fourDir[2], fourDir[3]);
	LOGE("  distant:(%d,%d,%d,%d)", fourDistant[0], fourDistant[1], fourDistant[2], fourDistant[3]);
	LOGE("  fequency:(%d,%d,%d,%d)", fourFeq[0], fourFeq[1], fourFeq[2], fourFeq[3]);
	LOGE("  power:(%d,%d,%d,%d)", fourPow[0], fourPow[1], fourPow[2], fourPow[3]);
	LOGE("  sequency:(%d,%d,%d,%d)", fourSeq[0], fourSeq[1], fourSeq[2], fourSeq[3]);

	//设置最终选择数组
	int finalDir[4] = { 0,0,0,0 };
	for (int i = 0; i < 4; i++)
	{
		if (fourDir[i] == MOVE_UP)
		{
			finalDir[i] = IsReachForRun(x, y - 1, MOVE_UP, x, y);
		}
		else if (fourDir[i] == MOVE_DOWN)
		{
			finalDir[i] = IsReachForRun(x, y + 1, MOVE_DOWN, x, y);
		}
		else if (fourDir[i] == MOVE_LEFT)
		{
			finalDir[i] = IsReachForRun(x - 1, y, MOVE_LEFT, x, y);
		}
		else
		{
			finalDir[i] = IsReachForRun(x + 1, y, MOVE_RIGHT, x, y);
		}
	}
	//排序最终选择数组
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3 - i; j++)
		{
			if (finalDir[j + 1] < finalDir[j])
			{
				int temp = fourDistant[j + 1];
				fourDistant[j + 1] = fourDistant[j];
				fourDistant[j] = temp;

				DIRECT temp_1 = fourDir[j + 1];
				fourDir[j + 1] = fourDir[j];
				fourDir[j] = temp_1;

				int temp_2 = fourFeq[j + 1];
				fourFeq[j + 1] = fourFeq[j];
				fourFeq[j] = temp_2;

				int temp_3 = fourSeq[j + 1];
				fourSeq[j + 1] = fourSeq[j];
				fourSeq[j] = temp_3;

				int temp_4 = fourPow[j + 1];
				fourPow[j + 1] = fourPow[j];
				fourPow[j] = temp_4;

				int temp_5 = finalDir[j + 1];
				finalDir[j + 1] = finalDir[j];
				finalDir[j] = temp_5;
			}
		}
	}
	LOGE("FINAL:\n  dir:(%d,%d,%d,%d)", fourDir[0], fourDir[1], fourDir[2], fourDir[3]);
	LOGE("  final:(%d,%d,%d,%d)", finalDir[0], finalDir[1], finalDir[2], finalDir[3]);
	if (dispersed[ChangePid(thisplayer.id)] == true)
	{
		LOGE("this kun need dispersed")
			if (finalDir[1] <= 5)
				return fourDir[1];
			else
				return fourDir[0];
	}
	else
	{
		if (finalDir[0] > 5)
			return NO_MOVE;
		else
			return fourDir[0];
	}


}

//夺命逃跑决策
DIRECT OurStrategy::CrazyRun(Player thisplayer)
{
	vector<MapItem> farpoi = map->getPeripheral(thisplayer.x, thisplayer.y, 4);
	int size = farpoi.size();
	int* distant = new int[size];
	int absx, absy;
	for (int i = 0; i < size; i++)
	{
		LOGE("点%d：(%d,%d)", i, farpoi[i].x, farpoi[i].y)
			absx = abs(thisplayer.x - farpoi[i].x);
		absy = abs(thisplayer.y - farpoi[i].y);
		distant[i] = absx + absy;
	}
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size - i - 1; j++)
		{
			if (distant[j] < distant[j + 1])
			{
				int temp = distant[j];
				distant[j] = distant[j + 1];
				distant[j + 1] = temp;

				MapItem temp1 = farpoi[j];
				farpoi[j] = farpoi[j + 1];
				farpoi[j + 1] = temp1;
			}
		}
	}
	for (int i = 0; i < size; i++)
	{
		int enemycost = 5;
		for (int j = 0; j < enemykunnum; j++)
		{
			int thisenemycost = map->FindRealDistance(EnemyKun[j].x, EnemyKun[j].y, farpoi[i].x, farpoi[i].y, 4);
			if (thisenemycost < enemycost)
				enemycost = thisenemycost;
			LOGE("敌人%d(%d,%d)到点(%d,%d)的距离：%d", j, EnemyKun[j].x, EnemyKun[j].y, farpoi[i].x, farpoi[i].y, thisenemycost);
		}
		if (enemycost > 4)
			return map->pathFind(thisplayer.x, thisplayer.y, farpoi[i].x, farpoi[i].y);
	}
	delete[] distant;
	return UrgentRun(thisplayer);
}

bool OurStrategy::dangerousRun()
{
	int nowlife;
	Team team1 = this->localRound->getTeams()[0];
	Team team2 = this->localRound->getTeams()[1];
	if (team1.id == this->myTeamId)
		nowlife = team1.remain_life;
	else
		nowlife = team2.remain_life;
	int round = this->localRound->getRoundId();
	if (round >= 150)
		round -= 150;
	int guessround = (4 - nowlife) * 30;

	LOGE("nowlife:%d, round:%d, gessround:%d", nowlife, round, guessround);
	if (nowlife == 0)
	{
		return true;
	}
	if (round >= guessround)
		return false;
	else
		return true;

	//回合-----剩余生命
	//0-------------4
	//30------------3
	//60------------2
	//90------------1
	//120-----------0
}

int OurStrategy::IsReachForRun(int x, int y, DIRECT dir, int xx, int yy)
{
	//威胁等级：1（低）->6（高）
	//7：必死：        步行点有敌人，自杀式行为
	//                 传送点有敌人，自杀式行为
	//                 步行点是不能到达的，与自杀没有什么区别
	//6：几乎必死：    步行点敌人可以一步走到，生还几率渺茫
	//5：非常可能必死：传送点敌人可以一步走到，由于敌人有可能丢失视野，有些许生还几率
	//4：不安全：      步行，敌人的距离更近了
	//3：不太安全：    快要接近边界而且直线上没有传送装置
	//                 传送，敌人的距离更近了
	//2：安全：        传送，与敌人的距离不会发生改变
	//1：非常安全：    步行，与敌人的距离不会发生改变
	//0：绝对安全：    传送，与敌人的距离变得更长了
	//   
	LOGE("Go (%d)，test (%d,%d) feasibility:", dir, x, y);
	if (x < 0)
	{
		LOGE("can not move left!");
		return 7;
	}
	if (y < 0)
	{
		LOGE("can not move up!");
		return 7;
	}
	if (x > map->getWidth() - 1)
	{
		LOGE("can not move right!");
		return 7;
	}
	if (y > map->getHeight() - 1)
	{
		LOGE("can not move down!");
		return 7;
	}

	//步行点是不能到达的，与自杀没有什么区别。必死
	if (map->GetMap(x, y).type == 1)
	{
		LOGE("no move meteor");
		return 7;
	}
	if (map->GetMap(x, y).type == 3 && map->GetMap(x, y).dir == map->ContractDir(dir))
	{
		LOGE("no move tunnel");
		return 7;
	}

	//空地
	if (map->GetMap(x, y).type == 0)
	{
		LOGE("my(%d,%d)", x, y);
		//步行点有敌人，自杀式行为。必死
		for (int i = 0; i < EnemyVisNum; i++)
		{
			if (EnemyVis[i].x == x && EnemyVis[i].y == y)
			{
				LOGE("enemy is here");
				return 7;
			}
		}
		//步行点敌人可以一步走到，生还几率渺茫。几乎必死
		for (int i = 0; i < enemykunnum; i++)
		{
			LOGE("enemy(%d,%d) distance：%d", EnemyKun[i].x, EnemyKun[i].y, map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, x, y, TEST_THRESHOLD));
			if (map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, x, y, TEST_THRESHOLD) == 1)
			{
				LOGE("enemy can reach here");
				return 6;
			}
		}
		//如果是空地，但是太接近边界，直线上又没有可以传送的点
		{
			int distant;
			if (dir == MOVE_UP)
				distant = y + 1;
			else if (dir == MOVE_DOWN)
				distant = map->getHeight() - y;
			else if (dir == MOVE_LEFT)
				distant = x + 1;
			else
				distant = map->getWidth() - x;
			if (distant <= 3 && !map->DirDelivery(x, y, dir))
			{
				LOGE("  too close and can't find tunnnel or wormhole");
				return 3;
			}
		}
		//确定这一步的安全性
		{
			bool level_2 = false;
			for (int i = 0; i < this->enemykunnum; i++)
			{
				int transdistant = map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, x, y, TEST_THRESHOLD);
				LOGE("if you go, your pos id(%d,%d),enemy pos is (%d,%d),the distance is%d", x, y, EnemyKun[i].x, EnemyKun[i].y, transdistant);
				int non_transdistant = map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, xx, yy, TEST_THRESHOLD);
				LOGE("if you not, your pos id(%d,%d),enemy pos is (%d,%d),the distance is%d", xx, yy, EnemyKun[i].x, EnemyKun[i].y, non_transdistant);
				if (non_transdistant > transdistant)
				{
					level_2 = true;
				}
			}
			if (level_2 == true)
			{
				LOGE("walk unsafe");
				return 4;
			}
			else
			{
				LOGE("walk safe");
				return 1;
			}
		}
	}

	//传送带或者黑洞
	if (map->GetMap(x, y).type == 2 || map->GetMap(x, y).type == 3)
	{
		MapItem maptem = map->FindTransEnd(map->GetMap(x, y));
		Point transpos;
		bool level[4] = { false,false,false,false };
		transpos.x = maptem.x;
		transpos.y = maptem.y;
		for (int i = 0; i < this->enemykunnum; i++)
		{
			if (EnemyKun[i].x == transpos.x && EnemyKun[i].y == transpos.y)
			{
				LOGE("enemy is here(trans)"); //传送点有敌人，自杀式行为。必死
				level[0] = true;
			}
			int transdistant = map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, transpos.x, transpos.y, TEST_THRESHOLD);
			LOGE("if you trans, your pos id(%d,%d),enemy pos is (%d,%d),the distance is%d", transpos.x, transpos.y, EnemyKun[i].x, EnemyKun[i].y, transdistant);
			if (transdistant == 1)
			{
				LOGE("enemy can reach here(trans)");//传送点敌人可以一步走到，由于敌人有可能丢失视野，有些许生还几率。非常可能必死
				level[1] = true;
			}
			int non_transdistant = map->FindRealDistance(EnemyKun[i].x, EnemyKun[i].y, xx, yy, TEST_THRESHOLD);
			LOGE("if you not, your pos id(%d,%d),enemy pos is (%d,%d),the distance is%d", xx, yy, EnemyKun[i].x, EnemyKun[i].y, non_transdistant);
			if (non_transdistant == transdistant && transdistant != TEST_THRESHOLD + 1)
			{
				LOGE("unsafe trans");	//传送，敌人的距离更近了
				level[2] = true;
			}
			if (non_transdistant > transdistant)
			{
				LOGE("unsafe trans"); //传送，敌人的距离更近了
				level[2] = true;
			}
			if (non_transdistant + 1 == transdistant)
			{
				LOGE("safe trans"); //传送，与敌人的距离不会发生改变。安全
				level[3] = true;
			}
		}
		if (level[0] == true)
			return 7;
		else if (level[1] == true)
			return 5;
		else if (level[2] == true)
			return 3;
		else if (level[3] == true)
			return 2;
		else
		{
			LOGE("very safe trans");
			return 0;//传送，与敌人的距离变得更长了。绝对安全
		}
	}

	return 10;
}

void OurStrategy::UpdateBait() {

	if (ourkunnum == 1 || ourkunnum == 0) {//如果场上我方只有一直鲲就算了
		BaitId = -1;
		return;
	}
	int least = 0;
	//LOGE("kun_1_score：%d,kun_2_score：%d,kun_3_score：%d,kun_4_score：%d", OurKun[0].score, OurKun[1].score, OurKun[2].score, OurKun[3].score);
	for (int i = 0; i < ourkunnum; i++)
	{
		if (OurKun[least].score > OurKun[i].score)
			least = i;
		LOGE("least：%d", least);
	}
	LOGE("final,least：%d", least);
	BaitId = OurKun[least].id;
	LOGE("（%d)", BaitId);
}

Point OurStrategy::FollowPatrolRoutine(Player player)
{
	// 返回目的地的点坐标
	/*
	//// 使用完之后记得要delete
	//// todo: 目前仅仅在九宫格最外围行走, 没有绕入中心点

	//// 开始重新规划路线
	//Point nextPoint[3][3] = { {{1,0}, {0,0}, {0, 1}}, {{2,0}, {-1, -1}, {0, 2}}, {{2,1}, {2,2}, {1,2}} };
	//// 上面这个数组记录了每一个周边Area的下一个是谁
	//Point prevPoint[3][3] = { {{0,1}, {0,2}, {1,2}}, {{0,1}, {-1,-1}, {2,2}}, {{1,0}, {2,0}, {2,1}} };
	//// 上面这个数组记录了每一个周边Area的上一个是谁

	//// 将存活的鲲中的第一个当做领头鲲.
	//Player leader = {};
	//if (this->ourkunnum > 0) {
	//	leader = this->OurKun[0];
	//}
	//else {
	//	return; // 一个鲲都没有还搞什么?
	//}

	//Point leaderAreaPoint = this->map->getAreaByCoord(leader.x, leader.y);  // 领头鲲当前处在的area

	//// 判断是重新规划路线, 还是继续按照之前的路走
	//if (this->localRound->round_id - 1 != this->patrolRoundId) {
	//	// 需要重新规划路线
	//	// 找到离领头鲲除现Area之外的最近的外围Area.

	//	int squareWidth = 8 - (25 - this->map->getWidth()) / 3; // area中方块的长度, 有用的
	//	// 当前鲲在中间区域
	//	if (leaderAreaPoint.x == 1 && leaderAreaPoint.y == 1) {
	//		if (leader.y < this->map->getHeight() / 2) { // 领头鲲在地图偏上
	//			if (leader.x < this->map->getWidth() / 2) { // 领头鲲在地图偏左
	//				// 此时要么去area[1][0], 要么去area[0][1]
	//				if (leader.x < leader.y) { // 更靠近area[0][1]
	//					leaderAreaPoint = { 0, 1 };
	//				}
	//				else { // 更靠近area[1][0]
	//					leaderAreaPoint = { 1 ,0 };
	//				}
	//			}
	//			else {// 领头鲲在地图偏右
	//				if (this->map->getWidth() - leader.x < leader.y) {
	//					leaderAreaPoint = { 2, 1 };
	//				}
	//				else {
	//					leaderAreaPoint = { 1, 0 };
	//				}
	//			}
	//		}
	//		else { // 领头鲲在地图偏下
	//			if (leader.x < this->map->getWidth() / 2) { // 领头鲲在地图偏左
	//				if (leader.x < this->map->getHeight() - leader.y) {
	//					leaderAreaPoint = { 0 , 1 };
	//				}
	//				else {
	//					leaderAreaPoint = { 1 , 2 };
	//				}
	//			}
	//			else { // 领头鲲在地图偏右
	//				if (this->map->getWidth() - leader.x < this->map->getHeight() - leader.y) {
	//					leaderAreaPoint = { 2, 1 };
	//				}
	//				else {
	//					leaderAreaPoint = { 1, 2 };
	//				}
	//			}
	//		}
	//	}

	//	// 当前鲲不在中间区域
	//	else {
	//		leaderAreaPoint = nextPoint[leaderAreaPoint.x][leaderAreaPoint.y];
	//	}
	//}
	//else {
	//	// 一直处在巡逻模式

	//	// 领头鲲已经到达指定地点
	//	if (this->patrolLeaderAreaPoint.x == leaderAreaPoint.x && this->patrolLeaderAreaPoint.y == leaderAreaPoint.y) {
	//		leaderAreaPoint = nextPoint[this->patrolLeaderAreaPoint.x][this->patrolLeaderAreaPoint.y];
	//	}

	//	// 领头鲲还没到达指定地点, 所有人按照计划继续前进
	//	// todo: 极大地依赖于领头鲲
	//	else {
	//		leaderAreaPoint = this->patrolLeaderAreaPoint;
	//		for (unsigned int i = 0; i < this->ourkunnum; ++i) {
	//			points[i] = this->patrolTargetAreaPoints[i];
	//		}
	//		return;
	//	}
	//}

	//// 确定领头鲲的目标区域
	//int leaderIndex = 0;
	//for (int i = 0; i < this->ourkunnum; ++i) {
	//	if (leader.id == this->OurKun[i].id) {
	//		leaderIndex = i;
	//		points[i] = leaderAreaPoint;
	//		this->patrolTargetAreaPoints[i] = leaderAreaPoint;
	//		break;
	//	}
	//}
	//this->patrolLeaderAreaPoint = leaderAreaPoint;

	//// 对于剩下来的鲲, 按次序安排到与前一个鲲的area距离为1个area的area里
	//Point ptrPrev = prevPoint[leaderAreaPoint.x][leaderAreaPoint.y]; // 当前Area的前一个Area, 此时ptr为leader

	//for (int i = leaderIndex; i < this->ourkunnum; ++i) {
	//	this->patrolTargetAreaPoints[i] = prevPoint[ptrPrev.x][ptrPrev.y];
	//	points[i] = prevPoint[ptrPrev.x][ptrPrev.y];
	//	ptrPrev = prevPoint[points[i].x][points[i].y];
	//}

	//// 在round_id这一回合进入了巡逻模式
	//this->patrolRoundId = this->localRound->round_id;
	*/

	// 新代码
	Point nextPoint[3][3] = { {{1,0}, {0,0}, {0, 1}}, {{2,0}, {-1, -1}, {0, 2}}, {{2,1}, {2,2}, {1,2}} };
	// 上面这个数组记录了每一个周边Area的下一个是谁
	Point prevPoint[3][3] = { {{0,1}, {0,2}, {1,2}}, {{0,1}, {-1,-1}, {2,2}}, {{1,0}, {2,0}, {2,1}} };
	// 上面这个数组记录了每一个周边Area的上一个是谁

	/*
	这个鲲被叫去执行巡逻任务, 只有以下几种情况:
	(1) 该鲲是新生鲲, 什么都不知道, 周边也没有危险, 然后去巡逻;
		(1)(1) 按照规定, 应该顺时针走, 看顺时针路上是否可行(前方没被占领且不危险), 如果可行, 直走
		(1)(2) 如果不可行, 则往逆时针看, 看逆时针路上是否可行(前方没被占领且不危险), 如果可行, 直走
		(1)(3) 如果不可行, 则往中心区域看, 看看中心区域是否可行(自己不处于中心区域且中心区域不被占领且不危险), 如果可行, 直走
		(1)(4) 如果不可行, 则前往其他未被占领且不危险的区域, 一定能找到的, 如果找不到就到中间去
	(2) 该鲲是老鲲, 刚刚已经在巡逻任务了, 现在督促他巡逻;
		(2)(1) 还没到达目的地, 继续前往目的地;
		(2)(2) 如果已经到达了目的地, 寻找下一个目的地
	(3) 该鲲是老鲲, 刚刚躲过追杀/取消追杀它鲲, 现在改为巡逻
		(3)(1) 与新生鲲一样
	*/

	// 该鲲现处的区域的坐标
	Point current = this->map->getAreaByCoord(player.x, player.y);

	// 该鲲刚刚还在巡逻
	if (player.patrolRoundId + 1 == this->localRound->round_id) {

		Point targetPoint;
		targetPoint.x = player.patrolPointIndex % this->map->getWidth();
		targetPoint.y = player.patrolPointIndex / this->map->getHeight();
		if (player.x == targetPoint.x && player.y == targetPoint.y) {
			// 鲲鲲已经到达了目的地, 寻找下一个目的地
			// 设置该区域为未被占领
			this->map->deoccupyArea(current.x, current.y);

			Point targetCoord = this->FindArea(current);
			// 给目标区域上"锁"
			this->map->occupyArea(targetCoord.x, targetCoord.y);
			Point finalPoint = this->GoToArea(player, targetCoord);
			LOGE("鲲鲲已经到达目的地了, 现在他要去这个区域: (%d, %d) 的 这个点: (%d, %d)", targetCoord.x, targetCoord.y, finalPoint.x, finalPoint.y);
			return finalPoint;
		}
		else {
			// todo: 可能会出现 明知山有虎偏向虎山行 的情况. 如果目标区域此时变成了危险区域, 应该改变目标
			LOGE("鲲鲲不忘使命, 去这个点 (%d, %d)", targetPoint.x, targetPoint.y);
			return targetPoint;
		}
	}
	// 巡逻任务刚刚下达
	else {
		Point targetCoord = this->FindArea(current);
		// 给目标区域上"锁"
		this->map->occupyArea(targetCoord.x, targetCoord.y);
		Point finalPoint = this->GoToArea(player, targetCoord);
		LOGE("鲲鲲一脸懵逼, 现在他被上级要去这个区域: (%d, %d) 的 这个点: (%d, %d)", targetCoord.x, targetCoord.y, finalPoint.x, finalPoint.y);
		return finalPoint;
	}
}

Point OurStrategy::GoToArea(Player player, Point point)
{
	// todo: 优先选择存在能量的点. 因为从目前表现来看, 能量点生成的地方都是唯一的
	int areaX = point.x;
	int areaY = point.y;

	int squareWidth = 8 - (25 - this->map->getWidth()) / 3;

	// 找到目标Area的横始终坐标和纵始终坐标
	int xStart = 0;
	switch (areaX) {
	case 0:
		xStart = 0; break;
	case 1:
		xStart = squareWidth; break;
	case 2:
		xStart = this->map->getWidth() - squareWidth;
	}
	int xEnd = xStart == squareWidth ? this->map->getWidth() - squareWidth : xStart + squareWidth;

	int yStart = 0;
	switch (areaY) {
	case 0:
		yStart = 0; break;
	case 1:
		yStart = squareWidth; break;
	case 2:
		yStart = this->map->getHeight() - squareWidth;
	}
	int yEnd = yStart == squareWidth ? this->map->getHeight() - squareWidth : yStart + squareWidth;

	Point* possiblePoints = new Point[(xEnd - xStart) * (yEnd - yStart)];

	// 遍历Area的每一个MapItem
	int numPossiblePoints = 0;
	for (int i = xStart; i < xEnd; ++i) {
		for (int j = yStart; j < yEnd; ++j) {
			if (this->map->GetMap(i, j).type == 0) {
				possiblePoints[numPossiblePoints] = { i, j };
				++numPossiblePoints;
			}
		}
	}

	// 随机在possiblePoints中找一个点, 看看是否可达
	srand((unsigned)time(NULL));
	Point targetPoint = possiblePoints[rand() % numPossiblePoints];
	while (!this->map->EatSafety(targetPoint.x, targetPoint.y)) {
		targetPoint = possiblePoints[rand() % numPossiblePoints];
	}

	delete[] possiblePoints;
	return targetPoint;
}
Point OurStrategy::FindArea(Point current)
{
	Point nextPoint[3][3] = { {{1,0}, {0,0}, {0, 1}}, {{2,0}, {-1, -1}, {0, 2}}, {{2,1}, {2,2}, {1,2}} };
	// 上面这个数组记录了每一个周边Area的下一个是谁
	Point prevPoint[3][3] = { {{0,1}, {0,2}, {1,2}}, {{0,1}, {-1,-1}, {2,2}}, {{1,0}, {2,0}, {2,1}} };
	// 上面这个数组记录了每一个周边Area的上一个是谁

	// 顺时针寻找区域
	Point nextCoord = nextPoint[current.x][current.y];
	Area nextArea = this->map->getArea(nextCoord.x, nextCoord.y);
	if (!nextArea.dangerous && !nextArea.occupied) {
		LOGE("鲲鲲在(%d, %d)区域寻找到了下一个目标区域: (%d, %d)", current.x, current.y, nextCoord.x, nextCoord.y);
		return nextCoord;
	}
	LOGE("此时的鲲鲲所在区域的下一个区域是危险的/被占领的");

	// 逆时针寻找区域
	Point prevCoord = prevPoint[current.x][current.y];
	Area prevArea = this->map->getArea(prevCoord.x, prevCoord.y);
	if (!prevArea.dangerous && !prevArea.occupied) {
		LOGE("鲲鲲在(%d, %d)区域寻找到了下一个目标区域: (%d, %d)", current.x, current.y, prevCoord.x, prevCoord.y);
		return prevCoord;
	}
	LOGE("此时的鲲鲲所在区域的上一个区域是危险的/被占领的");

	// 寻找中心区域 ( 有可能是现在的位置 )
	Point centerCoord;
	centerCoord.x = 1;
	centerCoord.y = 1;
	Area centerArea = this->map->getArea(1, 1);
	if (!centerArea.dangerous && !centerArea.occupied && current.x != 1 && current.y != 1) {
		LOGE("鲲鲲在(%d, %d)区域寻找到了下一个目标区域: (1, 1)", current.x, current.y);
		return centerCoord;
	}
	LOGE("此时的鲲鲲发现中间区域也不可到达");

	// 遍历其他区域
	unsigned short x = 0;
	unsigned short y = 0;
	Point finalCoord;
	finalCoord.x = x;
	finalCoord.y = y;

	prevCoord = prevPoint[current.x][current.y];
	nextCoord = nextPoint[current.x][current.y];
	for (x = 0; x < 3; ++x) {
		for (y = 0; y < 3; ++y) {
			if (x != prevCoord.x && y != prevCoord.y &&
				x != nextCoord.x && y != nextCoord.y &&
				x != 1 && y != 1 &&
				!this->map->getArea(x, y).dangerous &&
				!this->map->getArea(x, y).occupied) {
				finalCoord.x = x;
				finalCoord.y = y;
				LOGE("鲲鲲碰壁了, 在(%d, %d)区域随便寻找到了下一个目标区域: (%d, %d)", current.x, current.y, nextCoord.x, nextCoord.y);
				break;
			}
			LOGE("鲲鲲尝试了区域(%d, %d), 但是其不满足条件. 危险: %d, 被占领: %d", x, y, this->map->getArea(x, y).dangerous, this->map->getArea(x, y).occupied);
		}
	}
	return finalCoord;
}

//进攻决策
DIRECT OurStrategy::Attack(Player thisplayer)
{
	int attackerIndex = this->ChangePid(thisplayer.id);

	// 如果为寻能量者
	if (this->Attacker[attackerIndex] == FINDER || this->Attacker[attackerIndex] == HELPER) {
		// todo: 这里的FINDER只会巡逻
		return Normal(thisplayer);
		/*Point targetPoint = this->FollowPatrolRoutine(thisplayer);
		LOGE("我是得分手, 吃分顶呱呱, 去(%d, %d)", targetPoint.x, targetPoint.y);
		return this->map->pathFind(thisplayer.x, thisplayer.y, targetPoint.x, targetPoint.y);*/
	}

	if (this->Attacker[attackerIndex] != HELPER && this->Attacker[attackerIndex] != EATTER && this->surroundMode) {
		int minSize = 999;
		int minDirection = NO_MOVE;
		// 遍历thisplayer所有的方向
		for (int i = 0; i < 4; ++i) {
			Player eatter;
			Player portraiter;
			Player transverser;
			int portraiterIndex = -1;
			int transverserIndex = -1;
			int eatterIndex = -1;
			for (int i = 0; i < this->ourkunnum; ++i) {
				if (this->Attacker[i] == PORTRAITER) {
					portraiter = this->OurKun[i];
					portraiterIndex = i;
				}
				else if (this->Attacker[i] == TRANSVERSER) {
					transverser = this->OurKun[i];
					transverserIndex = i;
				}
				else if (this->Attacker[i] == EATTER) {
					eatter = this->OurKun[i];
					eatterIndex = i;
				}
			}

			MapItem nextEatterMapItem = this->map->FindMoveEnd(eatter.x, eatter.y, DIRECT(this->attackerDirections[eatterIndex]));
			MapItem nextPortraiterMapItem = this->map->FindMoveEnd(portraiter.x, portraiter.y, DIRECT(this->attackerDirections[portraiterIndex]));
			MapItem nextTransverserMapItem = this->map->FindMoveEnd(transverser.x, transverser.y, DIRECT(this->attackerDirections[transverserIndex]));
			eatter.x = nextEatterMapItem.x;
			eatter.y = nextEatterMapItem.y;
			portraiter.x = nextPortraiterMapItem.x;
			portraiter.y = nextPortraiterMapItem.y;
			transverser.x = nextTransverserMapItem.x;
			transverser.y = nextTransverserMapItem.y;

			vector<Player> helpers;
			if (this->Attacker[attackerIndex] != EATTER) { helpers.push_back(eatter); }
			if (this->Attacker[attackerIndex] != PORTRAITER) { helpers.push_back(portraiter); }
			if (this->Attacker[attackerIndex] != TRANSVERSER) { helpers.push_back(transverser); }

			Player tmp = thisplayer;
			MapItem nextMapItem = this->map->FindMoveEnd(thisplayer.x, thisplayer.y, DIRECT(i));
			tmp.x = nextMapItem.x;
			tmp.y = nextMapItem.y;
			helpers.push_back(tmp);
			//int diffusionThreshold = this->map->FindRealDistance(tmp.x, tmp.y, this->targetKun.x, this->targetKun.y);
			LOGE("传入给RotatingDiffusion的阈值为%d", this->localLeg->getVision());
			vector<MapItem> result = this->map->RotatingDiffusion(helpers, this->targetKun, this->localLeg->getVision());
			if (result.size() != 0 && result.size() < minSize) {
				minDirection = i;
				LOGE("我发现我等下走向%d的话, 我和兄弟们能够更加包围敌人", minDirection);
				minSize = result.size();
			}
		}
		this->attackerDirections[attackerIndex] = minDirection;
		return (DIRECT)minDirection;
	}


	// 如果为主要追捕者, 则持续追踪对方kun
	if (this->Attacker[attackerIndex] == EATTER) {
		LOGE("我是主要追捕者, 我要追捕的敌人在(%d, %d)", this->targetKun.x, this->targetKun.y);
		this->attackerDirections[attackerIndex] = harassingDecision[0];
		return harassingDecision[0];
	}


	if (this->Attacker[attackerIndex] == HELPER) {
		LOGE("我没有工作, 我不动");
		return NO_MOVE;
	}

	int distanceToEnemy = this->map->FindRealDistance(thisplayer.x, thisplayer.y, this->targetKun.x, this->targetKun.y);
	// todo: 获取在一定范围内的MapItem包含其距离

	//// 请问你在人家哪里? 能不能靠近点说话? 最多给你两倍视距的容错
	//if (distanceToEnemy > this->localLeg->getVision() * 2) {
	//	LOGE("我距离敌人还有很长一段距离, 我现在去敌人位置(%d, %d)", this->targetKun.x, this->targetKun.y);
	//	return this->map->pathFind(thisplayer.x, thisplayer.y, this->targetKun.x, this->targetKun.y);
	//}

	if (this->Attacker[attackerIndex] == TRANSVERSER || this->Attacker[attackerIndex] == PORTRAITER) {
		LOGE("我是辅助追捕者");

		vector<MapItem> sortedScope;
		int sortedScopeThreshold = this->localLeg->getVision() + 1;

		// 横向压迫者在横截面压制敌人
		if (this->Attacker[attackerIndex] == TRANSVERSER) {
			LOGE("我负责横向压迫");
			if (thisplayer.y < this->targetKun.y) {
				sortedScope = this->map->SortScope(this->targetKun.x, this->targetKun.y, MOVE_UP, sortedScopeThreshold, false);
			}
			else {
				sortedScope = this->map->SortScope(this->targetKun.x, this->targetKun.y, MOVE_DOWN, sortedScopeThreshold, false);
			}
			// todo: 还不知道相等的时候应该怎么解决
		}

		// 纵向压迫者在纵切面压制敌人
		if (this->Attacker[attackerIndex] == PORTRAITER) {
			LOGE("我负责纵向压迫");
			if (thisplayer.x < this->targetKun.x) {
				sortedScope = this->map->SortScope(this->targetKun.x, this->targetKun.y, MOVE_LEFT, sortedScopeThreshold, false);
			}
			else {
				sortedScope = this->map->SortScope(this->targetKun.x, this->targetKun.y, MOVE_RIGHT, sortedScopeThreshold, false);
			}
			// todo: 还不知道相等的时候应该怎么解决
		}

		Point destination;
		int tempEqualValue = 999;
		Point tempEqualDestination;
		LOGE("敌人%d步能到达的点, 分别为", sortedScopeThreshold);
		for (vector<MapItem>::iterator it = sortedScope.begin(); it != sortedScope.end(); ++it) {
			LOGE("点: (%d, %d), 敌人到达此处的步数为%d", (*it).x, (*it).y, (*it).towerType);
			int distanceToPoint = this->map->FindRealDistance(thisplayer.x, thisplayer.y, (*it).x, (*it).y);
			LOGE("到达上面这个点, 我们到此的距离为%d", distanceToPoint);
			if ((*it).towerType < distanceToPoint) {
				if (destination.x == -1 && destination.y == -1) {
					LOGE("我好难, 我一上来就遇到了敌方到达此点距离要比我到达此点距离小的点");
					return this->map->pathFind(thisplayer.x, thisplayer.y, (*it).x, (*it).y);
				}
				else if (tempEqualValue == 999) {
					LOGE("本来是一直大于大得好好的, 一下子给我来了个小于号");
					return NO_MOVE;
				}
				else {
					LOGE("搞了一阵子的大于号, 之前也出现过等于号, 现在取出现过距离最小的等于号");
					return this->map->pathFind(thisplayer.x, thisplayer.y, tempEqualDestination.x, tempEqualDestination.y);
				}
				break;
			}
			else if ((*it).towerType == distanceToPoint) {
				if (distanceToPoint < tempEqualValue) {
					tempEqualValue = distanceToPoint;
					destination.x = (*it).x;
					destination.y = (*it).y;
					tempEqualDestination.x = (*it).x;
					tempEqualDestination.y = (*it).y;
				}
			}
			else {
				destination.x = (*it).x;
				destination.y = (*it).y;
			}
		}
		// 建立在不可能不出现小于号的基础上, 可以把所有的return放在if ((*it).towerType < distanceToPoint) 中

		/*if (tempEqualValue == 999) {
			LOGE("我最终设定我自己的目标为点(%d, %d)", destination.x, destination.y);
			return this->map->pathFind(thisplayer.x, thisplayer.y, destination.x, destination.y);
		}
		else {
			LOGE("我最终设定我自己的目标为点(%d, %d)", tempEqualDestination.x, tempEqualDestination.y);
			return this->map->pathFind(thisplayer.x, thisplayer.y, tempEqualDestination.x, tempEqualDestination.y);
		}*/
	}

	LOGE("谁知道这是什么鲲");
	return NO_MOVE;


	// 协助者前往易丢失视野的点进行堵塞
	// todo: 谁来完成呢?


	// 如果为辅助追捕者, 则持续协助行为
	LOGE("我是辅助追捕者");
	/*int measureThreshold = 3;*/
	Point directions[5] = { {0,-1}, {0, 1}, {-1, 0} , {1,0} , {0, 0} };
	int thisDistance = this->map->FindRealDistance(thisplayer.x, thisplayer.y, this->targetKun.x, this->targetKun.y, this->diffuseThreshold);

	if (thisDistance <= this->diffuseThreshold) { // 如果有协助者与目标的距离不超过阈值, 那么应该缩短扩散范围, 并且按照缩小对方生存空间的方向去

		LOGE("我到敌人的距离不超过阈值, 所以我要开始计算我们的势力范围是否能将敌人围住");
		// 找到所有其他也符合收缩条件的鲲
		vector<Player> helpers;
		for (int i = 0; i < this->ourkunnum; ++i) {
			if (i == attackerIndex) { continue; }
			MapItem kunAtNextRound = this->map->FindMoveEnd(this->OurKun[i].x, this->OurKun[i].y, (DIRECT)this->attackerDirections[i]);
			int othersDistance = this->map->FindRealDistance(kunAtNextRound.x, kunAtNextRound.y, this->targetKun.x, this->targetKun.y, this->diffuseThreshold);
			if (othersDistance <= this->diffuseThreshold) {
				LOGE("我发现鲲鲲%d号走到下一步(或者不动)后, 它与敌人的距离也不超过阈值", this->OurKun[i].id);
				helpers.push_back(this->OurKun[i]);
			}
		}

		int minSize = 999;
		int minDirection = NO_MOVE;
		// 遍历thisplayer所有的方向
		for (int i = 0; i < 4; ++i) {
			Player tmp = thisplayer;
			MapItem nextMapItem = this->map->FindMoveEnd(thisplayer.x, thisplayer.y, DIRECT(i));
			tmp.x = nextMapItem.x;
			tmp.y = nextMapItem.y;
			vector<Player> newHelpers = helpers;
			newHelpers.push_back(tmp);
			//int diffusionThreshold = this->map->FindRealDistance(tmp.x, tmp.y, this->targetKun.x, this->targetKun.y);
			LOGE("传入给RotatingDiffusion的阈值为%d", this->diffuseThreshold - 1);
			vector<MapItem> result = this->map->RotatingDiffusion(newHelpers, this->targetKun, this->diffuseThreshold - 1);
			if (result.size() != 0 && result.size() < minSize) {
				minDirection = i;
				LOGE("我发现我等下走向%d的话, 我和兄弟们能够更加包围敌人", minDirection);
				minSize = result.size();
			}
		}

		--this->diffuseThreshold; // 如果不减小阈值, 只能放着人家跑
		this->attackerDirections[attackerIndex] = minDirection;
		if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
		if (minDirection != NO_MOVE) {
			return (DIRECT)minDirection;
		}
	}


	// 距离还是很远, 我们还是要继续向目标前进
	DIRECT nextMove = this->map->pathFind(thisplayer.x, thisplayer.y, this->targetKun.x, this->targetKun.y);

	// 避免鲲鲲们走到同一个位置
	for (int i = 0; i < attackerIndex; ++i) {
		MapItem partnerNextMapItem = this->map->FindMoveEnd(this->OurKun[i].x, this->OurKun[i].y, (DIRECT)this->attackerDirections[i]);
		MapItem myNextMapItem = this->map->FindMoveEnd(thisplayer.x, thisplayer.y, nextMove);

		if (partnerNextMapItem.x == myNextMapItem.x && partnerNextMapItem.y == myNextMapItem.y) {
			LOGE("我等下如果往%d走的话, 会跟别人重合");
			nextMove = NO_MOVE;
			break;
		}
	}

	this->attackerDirections[attackerIndex] = nextMove;
	if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
	return nextMove;

	// 2019年9月2日14:29:31 现在开始, 不考虑压迫点

	// 即使是执行了缩小地方生存空间, 也无法找到合适的方位, 退而求其次, 先去上级安排的点

	/*LOGE("开始考虑压迫点");
	for (int i = 0; i < 4; ++i) {
		if (this->pressurePoints[i].ownerId == thisplayer.id) {
			return this->map->pathFind(thisplayer.x, thisplayer.y, this->pressurePoints[i].x, this->pressurePoints[i].y);
		}
	}

	LOGE("天底下没有容得下本鲲的地方");*/

	//vector<MapItem> tmp;
	//if (thisDistance <= this->minPressureThreshold) { // 如果有协助者非常接近该点了, 那么应该缩短扩散范围
	//	--this->minPressureThreshold;
	//}
	//int getPressureThreshold = this->minPressureThreshold;
	//LOGE("获取压迫点的阈值设置为%d", getPressureThreshold);
	//tmp = map->getPressurePoint(harassingEnm[0].x, harassingEnm[0].y, getPressureThreshold);
	//for (vector<MapItem>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
	//	LOGE("协助者的目的点: (%d, %d)", (*it).x, (*it).y);
	//}

	//if (Attacker[attackerIndex] == TRANSVERSER_1 || Attacker[attackerIndex] == TRANSVERSER_2) {
	//	LOGE("TRANSVERSER_1:(%d,%d)", thisplayer.x, thisplayer.y);
	//	if (thisplayer.y < targetKun.y)
	//	{
	//		LOGE("ID为 %d 的我方鲲鲲在敌人上方, 现在to(%d,%d)", thisplayer.id, tmp[0].x, tmp[0].y);
	//		if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
	//		return map->pathFind(thisplayer.x, thisplayer.y, tmp[0].x, tmp[0].y);
	//	}
	//	else
	//	{
	//		LOGE("ID为 %d 的我方鲲鲲在敌人下方(或同高度), 现在to(%d,%d)", thisplayer.id, tmp[1].x, tmp[1].y);
	//		if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
	//		return map->pathFind(thisplayer.x, thisplayer.y, tmp[1].x, tmp[1].y);
	//	}
	//}
	//else if (Attacker[attackerIndex] == PORTRAITER_1 || Attacker[attackerIndex] == PORTRAITER_2)
	//{
	//	LOGE("PORTRAITER_1:(%d,%d)", thisplayer.x, thisplayer.y);
	//	if (thisplayer.x < targetKun.x)
	//	{
	//		LOGE("ID为 %d 的我方鲲鲲在敌人左方, 现在to(%d,%d)", thisplayer.id, tmp[2].x, tmp[2].y);
	//		if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
	//		return map->pathFind(thisplayer.x, thisplayer.y, tmp[2].x, tmp[2].y);
	//	}
	//	else
	//	{
	//		LOGE("ID为 %d 的我方鲲鲲在敌人右方(或同宽度), 现在to(%d,%d)", thisplayer.id, tmp[3].x, tmp[3].y);
	//		if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
	//		return map->pathFind(thisplayer.x, thisplayer.y, tmp[3].x, tmp[3].y);
	//	}
	//}
	//else {
	//	LOGE("本鲲什么职位都没有, 我不动");
	//	if (attackerIndex == this->ourkunnum - 1) { this->ResetAttackerDirections(); }
	//	return NO_MOVE;
	//}



	//添加：任何时候若能直接击杀敌人，则立即执行
	/*for (int i = 0; i < enemykunnum; i++)
	{
		if (map->FindRealDistance(thisplayer.x, thisplayer.y, EnemyKun[i].x, EnemyKun[i].y,1)<=1)
		{
			return map->pathFind(thisplayer.x, thisplayer.y, EnemyKun[i].x, EnemyKun[i].y);
		}
	}*/
	//优先执行围捕决策
	//printf("\n aaaaaaaaaaa \n");
	if (isPursuing)
	{
		for (int i = 0; i < pursuingUnit[0].size(); i++)
		{
			if (thisplayer.id == pursuingUnit[0][i].id)
			{
				return pursuingDecision[0][i];
			}
		}

	}
	//printf("\n aaaaaaaaaaa \n");
	if (isHarass)
	{
		for (int i = 0; i < harassingUnit.size(); i++)
		{
			if (thisplayer.id == harassingUnit[i].id)
			{
				return harassingDecision[i];
			}
		}
	}
	return Normal(thisplayer);
}

//检查是否已形成可追捕局面，并作出攻击决策
void OurStrategy::CheckSituation()
{
	//非进攻回合直接返回
	if (GameSituation() == DEFENSE)return;

	if (remainingSteps < 0)
	{//结束追捕状态
		if (isPursuing)
		{
			isPursuing = false;
			remainingSteps = -1;
			pursuingEnm.clear();
			lastPurPos.clear();
			pursuingUnit.clear();
			pursuingDecision.clear();

		}

	}

	//int pursueThreshold = this->localLeg->getVision() > 5 ? 5 : this->localLeg->getVision();//步骤阈值（暂定视野范围）
	//检测是否形成可围捕局面
	if (!isHarass)
	{
		if (enemykunnum == 0)
		{
			return;
		}
		else
		{

			for (int k = 0; k < ourkunnum; k++)
			{
				GetEnemyVis(OurKun[k].id);
				if (EnemyVisNum > 0)
				{
					isHarass = true;
					harassingEnm.clear();
					harassingEnm.push_back(EnemyVis[0]);
					lastHarPos.clear();
					Point t;
					t.x = EnemyVis[0].x;
					t.y = EnemyVis[0].y;
					lastHarPos = vector<Point>(1, t);
					harassingUnit.clear();
					harassingUnit.push_back(OurKun[k]);
					break;
				}
			}
		}
	}
	//if (isHarass)
	//{
	//	/*if (!isPursuing)
	//	{
	//
	//		vector<Player> possiblePursuer;
	//		for (int i = 0; i < enemykunnum; i++)
	//		{
	//			vector<MapItem> tmpPeripheral = map->getScope(EnemyKun[i].x, EnemyKun[i].y, pursueThreshold);
	//			for (int k = 0; k < ourkunnum; k++)
	//			{
	//				vector<MapItem> tmpScope = map->getScope(OurKun[k].x, OurKun[k].y, pursueThreshold+1);
	//				for (int i = 0; i < tmpScope.size(); i++)
	//				{
	//					for (int j = 0; j < tmpPeripheral.size(); j++)
	//					{
	//						if (tmpScope[i].x == tmpPeripheral[j].x&&tmpScope[i].y == tmpPeripheral[j].y)
	//						{
	//							bool hasAddPursuer = false;
	//							for (int m = 0; m < possiblePursuer.size(); m++)
	//							{
	//								if (possiblePursuer[m].id == OurKun[k].id)
	//								{
	//									hasAddPursuer = true;
	//								}
	//							}
	//							if (!hasAddPursuer)
	//							{
	//								possiblePursuer.push_back(OurKun[k]);
	//							}
	//							tmpPeripheral.erase(tmpPeripheral.begin() + j);
	//							j--;
	//						}
	//					}
	//				}
	//			}
	//			if (tmpPeripheral.empty())
	//			{
	//				isPursuing = true;
	//				remainingSteps = pursueThreshold;
	//				pursuingEnm.clear();
	//				pursuingEnm.push_back(EnemyKun[i]);
	//
	//				lastPurPos.clear();
	//				Point t;
	//				t.x = EnemyKun[i].x;
	//				t.y = EnemyKun[i].y;
	//				lastPurPos = vector<Point>(1, t);
	//
	//				for (int i = 0; i < pursuingUnit.size(); i++)
	//				{
	//					pursuingUnit[i].clear();
	//				}
	//				pursuingUnit.clear();
	//				pursuingUnit.push_back(possiblePursuer);
	//				break;
	//			}
	//			else
	//			{
	//				possiblePursuer.clear();
	//			}
	//		}
	//	}*/
	//	/*for (int i = 0; i < pursuingEnm.size(); i++)
	//	{
	//		bool isRep = false;
	//		for (int j = 0; j < enemykunnum; j++)
	//		{
	//			if (EnemyKun[j].id == pursuingEnm[i].id)
	//			{
	//				isRep = true;
	//				break;
	//			}
	//		}
	//		if (!isRep)
	//		{
	//			ene.push_back(pursuingEnm[i]);
	//		}
	//	}*/
	//	vector<Player> ene;//待确认的追捕对象
	//	vector<int> scrpstp;//各个敌人的最优平均步分
	//	vector<int> eneIndex;//最优步分对应追捕对象编号
	//	vector<vector<Point>> sln;//对于每个敌人，每只鲲的行动方案
	//	vector<vector<int>> slnIndex;//对于每个敌人，执行追捕行动的每只鲲的编号
	//	for (int i = 0; i < enemykunnum; i++)
	//	{
	//		ene.push_back(EnemyKun[i]);
	//	}
	//
	//
	//	vector<vector<MapItem>> neibors;
	//
	//	//printf("\n aaaaaaaaaaa \n");
	//	for (int i = 0; i < ene.size(); i++)
	//	{
	//		vector<MapItem> neibor = map->getPeripheral(ene[i].x, ene[i].y, 1);
	//		neibors.push_back(neibor);
	//		vector<vector<Point>> pc;
	//		vector<int> kunIndex;
	//
	//		for (int j = 0; j < ourkunnum; j++)
	//		{
	//			vector<int> neiborIndex;
	//			vector<int> distance;
	//			int min = -1;
	//			vector<Point> re = map->FindBatchMinDistance(OurKun[j].x, OurKun[j].y, neibor, 10);
	//			if (!re.empty())
	//			{
	//				for (int k = 0; k < re.size(); k++)
	//				{
	//					neiborIndex.push_back(re[k].x);
	//					distance.push_back(re[k].y);
	//
	//				}
	//			}
	//			/*for (int k = 0; k < neibor.size(); k++)
	//			{
	//				int t = map->FindRealDistance(OurKun[j].x, OurKun[j].y, neibor[k].x, neibor[k].y, 10);//暂定十步之内必须能到周围点
	//				if (t<=10)
	//				{
	//					distance.push_back(t);
	//					neiborIndex.push_back(k);
	//				}
	//
	//			}*/
	//			if (!distance.empty())
	//			{
	//				min = *min_element(distance.begin(), distance.end());
	//				for (int k = 0; k < distance.size(); k++)
	//				{
	//					if (distance[k] != min)
	//					{
	//						distance.erase(distance.begin() + k);
	//						neiborIndex.erase(neiborIndex.begin() + k);
	//						k--;
	//					}
	//				}
	//				vector<Point> tmp;
	//				for (int k = 0; k < distance.size(); k++)
	//				{
	//					Point t;
	//					t.x = neiborIndex[k];
	//					t.y = distance[k];
	//					tmp.push_back(t);
	//				}
	//				Point t = { -1,0 };
	//				tmp.push_back(t);
	//				if (!tmp.empty())
	//				{
	//					pc.push_back(tmp);
	//					kunIndex.push_back(j);
	//				}
	//			}
	//		}
	//		//printf("\n %d \n",pc.size()); 检查逻辑错误
	//		if (!pc.empty())
	//		{
	//			vector<int> branch = vector<int>(pc.size(), 0);
	//			vector<vector<Point>> successc;//最后添加最优平均的时候记得筛掉占领数不及最大占领数的项目
	//			vector<vector<int>> successIndex;
	//			vector<int> successScr;
	//			int maxTakeUp = -1;
	//			//遍历所有方案可能
	//			while (true)
	//			{
	//				vector<Point> curc;
	//				vector<int> curk;
	//				//int curTakeUp = 0;
	//				int exitCount = 0;
	//				bool isFail = false;
	//				for (int j = 0; j < pc.size(); j++)
	//				{
	//					//注意我方鲲可以选择不追捕,此时x=-1
	//					curc.push_back(pc[j][branch[j]]);
	//					curk.push_back(kunIndex[j]);
	//					if (pc[j][branch[j]].x == (-1))
	//					{
	//						exitCount++;
	//					}
	//					bool isRep = false;
	//					for (int k = 0; k < j; k++)
	//					{
	//						if (pc[k][branch[k]].x == pc[j][branch[j]].x && pc[k][branch[k]].x != (-1))
	//						{
	//							isRep = true;
	//							break;
	//						}
	//					}
	//					if (isRep)
	//					{
	//						isFail = true;
	//						break;
	//					}
	//				}
	//
	//				if (!isFail)
	//				{
	//					if (curc.size() - exitCount >= neibor.size() - 1)
	//					{
	//						//printf("\n 总个数：%d \n", neibor.size());
	//						if (maxTakeUp < ((int)curc.size() - exitCount))
	//						{
	//							maxTakeUp = ((int)curc.size() - exitCount);
	//						}
	//						//printf("\n 目前占领个数：%d \n", (int)curc.size() - exitCount);
	//						//printf("\n 目前最大占领个数：%d \n", maxTakeUp);
	//						successc.push_back(curc);
	//						successIndex.push_back(curk);
	//						int steps = 0;
	//						for (int j = 0; j < curc.size(); j++)
	//						{
	//							steps += curc[j].y;
	//						}
	//						successScr.push_back(ene[i].score / (steps + 1));
	//					}
	//				}
	//
	//				//继续进行下一个分支的判定
	//				int index = branch.size();
	//				while (index > 0)
	//				{
	//					index--;
	//					branch[index] = (branch[index] + 1) % pc[index].size();
	//					if (branch[index] > 0)
	//					{
	//						break;
	//					}
	//				}
	//				if (index <= 0)
	//				{
	//					break;
	//				}
	//			}
	//			//筛选该敌人最佳追捕分配方案
	//			int maxScr = 0;
	//			//*max_element(successScr.begin(), successScr.end());
	//			for (int j = 0; j < successScr.size(); j++)
	//			{
	//				int tak = 0;
	//				for (int k = 0; k < successc[j].size(); k++)
	//				{
	//					if (successc[j][k].x != -1)
	//					{
	//						tak++;
	//					}
	//				}
	//				if (maxScr < successScr[j] && tak >= maxTakeUp)
	//				{
	//					maxScr = successScr[j];
	//				}
	//			}
	//			for (int j = 0; j < successc.size(); j++)
	//			{
	//				for (int k = 0; k < successc[j].size(); k++)
	//				{
	//					if (successc[j][k].x == -1)
	//					{
	//						successc[j].erase(successc[j].begin() + k);
	//						successIndex[j].erase(successIndex[j].begin() + k);
	//						k--;
	//					};
	//				}
	//				//如果觉得步分更加重要的话可以把这个if去掉，同时更改上文最大分值计算操作
	//				if (successc[j].size() < maxTakeUp)
	//				{
	//					continue;
	//				}
	//				if (successScr[j] < maxScr)
	//				{
	//					continue;
	//				}
	//				sln.push_back(successc[j]);
	//				slnIndex.push_back(successIndex[j]);
	//				scrpstp.push_back(successScr[j]);
	//				eneIndex.push_back(i);
	//				break;
	//			}
	//		}
	//
	//	}
	//
	//	//printf("\n aaaaaaaaaaa \n");
	//
	//	if (scrpstp.empty())
	//	{
	//		//printf("\n aaaaaaaaaaa \n");
	//		isPursuing = false;
	//		remainingSteps = -1;
	//		pursuingEnm.clear();
	//		lastPurPos.clear();
	//		pursuingUnit.clear();
	//		pursuingDecision.clear();
	//	}
	//	else
	//	{
	//		int bestScr = *max_element(scrpstp.begin(), scrpstp.end());
	//		int bestIdx = max_element(scrpstp.begin(), scrpstp.end()) - scrpstp.begin();
	//
	//		isPursuing = true;
	//		remainingSteps = 50;//暂定
	//		pursuingEnm.clear();
	//		pursuingEnm.push_back(ene[eneIndex[bestIdx]]);
	//
	//		lastPurPos.clear();
	//		Point t = { ene[eneIndex[bestIdx]].x ,ene[eneIndex[bestIdx]].y };
	//		lastPurPos = vector<Point>(1, t);
	//
	//		for (int i = 0; i < pursuingUnit.size(); i++)
	//		{
	//			pursuingUnit[i].clear();
	//		}
	//		pursuingUnit.clear();
	//		vector<Player> unit;
	//		for (int i = 0; i < slnIndex[bestIdx].size(); i++)
	//		{
	//			unit.push_back(OurKun[slnIndex[bestIdx][i]]);
	//		}
	//		pursuingUnit.push_back(unit);
	//
	//		pursuingDecision.clear();
	//		vector<DIRECT> decision;
	//		for (int i = 0; i < slnIndex[bestIdx].size(); i++)
	//		{
	//			DIRECT advDir = map->pathFind(ene[eneIndex[bestIdx]].x, ene[eneIndex[bestIdx]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y);
	//			bool isRipe = true;//时机是否成熟？
	//			for (int j = 0; j < sln[bestIdx].size(); j++)
	//			{
	//				if (sln[bestIdx][j].y > 0)
	//				{
	//					isRipe = false;
	//				}
	//			}
	//			if (isRipe && slnIndex[bestIdx].size() == neibors[eneIndex[bestIdx]].size())
	//			{
	//				LOGE("\n 击杀决策：我方坤（%d,%d），前往（%d,%d）击杀 \n", OurKun[slnIndex[bestIdx][i]].x, OurKun[slnIndex[bestIdx][i]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y);
	//				decision.push_back(map->pathFind(OurKun[slnIndex[bestIdx][i]].x, OurKun[slnIndex[bestIdx][i]].y, ene[eneIndex[bestIdx]].x, ene[eneIndex[bestIdx]].y));
	//			}
	//			else if (isRipe)
	//			{
	//				bool hasAdd = false;
	//				for (int j = 0; j < neibors[eneIndex[bestIdx]].size(); j++)
	//				{
	//					bool isc = true;
	//					for (int k = 0; k < sln[bestIdx].size(); k++)
	//					{
	//						if (sln[bestIdx][k].x == j)
	//						{
	//							isc = false;
	//						}
	//					}
	//					if (isc)
	//					{
	//						LOGE("\n 压迫决策：我方坤（%d,%d），前往（%d,%d）准备压迫 \n", OurKun[slnIndex[bestIdx][i]].x, OurKun[slnIndex[bestIdx][i]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y);
	//						decision.push_back(map->pathFind(ene[eneIndex[bestIdx]].x, ene[eneIndex[bestIdx]].y, neibors[eneIndex[bestIdx]][j].x, neibors[eneIndex[bestIdx]][j].y));
	//						hasAdd = true;
	//						break;
	//					}
	//				}
	//				if (!hasAdd)
	//				{
	//					printf("\n 预判轨迹出错！！ \n");
	//					decision.push_back(map->pathFind(OurKun[slnIndex[bestIdx][i]].x, OurKun[slnIndex[bestIdx][i]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y, advDir));
	//				}
	//			}
	//			else
	//			{
	//				LOGE("\n 围捕决策：我方坤（%d,%d），前往（%d,%d）准备围捕，优先路径方向%d \n", OurKun[slnIndex[bestIdx][i]].x, OurKun[slnIndex[bestIdx][i]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y, (int)advDir);
	//				//DIRECT advDir = map->pathFind(ene[eneIndex[bestIdx]].x, ene[eneIndex[bestIdx]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y);
	//				decision.push_back(map->pathFind(OurKun[slnIndex[bestIdx][i]].x, OurKun[slnIndex[bestIdx][i]].y, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].x, neibors[eneIndex[bestIdx]][sln[bestIdx][i].x].y, advDir));
	//			}
	//		}
	//		pursuingDecision.push_back(decision);
	//
	//
	//	}
	//	//printf("\n %d,%d \n", pursuingUnit.size(), pursuingDecision.size());
	//
	//}


	//追捕/骚扰动作决策，追捕/骚扰结束的代码，

	//if (isPursuing)
	//{
	//	if (pursuingEnm.empty() || pursuingUnit.empty())
	//	{
	//		LOGE("追捕决策错误！！");
	//		remainingSteps = -1;
	//		return;
	//	}
	//	//避免我方坤移动到同一格
	//	vector<MapItem> already;
	//	if (!pursuingDecision.empty())
	//	{
	//		for (int i = 0; i < pursuingDecision[0].size(); i++)
	//		{
	//			MapItem t = map->FindMoveEnd(pursuingUnit[0][i].x, pursuingUnit[0][i].y, pursuingDecision[0][i]);
	//			bool hasMan = false;
	//			for (int j = 0; j < already.size(); j++)
	//			{
	//				if (already[j].x == t.x && already[j].y == t.y)
	//				{
	//					pursuingDecision[0][i] = NO_MOVE;
	//					hasMan = true;
	//					break;
	//				}
	//			}
	//			if (!hasMan)
	//			{
	//				already.push_back(t);
	//			}
	//		}
	//
	//	}
	//
	//	//更新追捕单位
	//	/*pursuingDecision.clear();
	//	for (int i = 0; i < pursuingUnit[0].size(); i++)
	//	{
	//		pursuingUnit[0][i] = OurKun[ChangePid(pursuingUnit[0][i].id)];
	//	}
	//	pursuingDecision.push_back(vector<DIRECT>(pursuingUnit[0].size(), NO_MOVE));
	//	//更新被追捕敌人
	//	bool isInView = false;
	//	for (int i = 0; i < enemykunnum; i++)
	//	{
	//		if (EnemyKun[i].id == pursuingEnm[0].id)
	//		{
	//			isInView = true;
	//			pursuingEnm[0] = EnemyKun[i];
	//		}
	//	}
	//
	//	printf("\n aaaaaaaa \n");*/
	//	//敌人传送出视野时的更新方案
	//	/*if (!isInView)
	//	{
	//		vector<MapItem> ends;
	//		if (lastPurPos.empty())
	//		{
	//			LOGE("追捕决策错误p2！！");
	//			remainingSteps = -1;
	//			return;
	//		}
	//		if (lastPurPos[0].y - 1 >= 0 && (map->GetMap(lastPurPos[0].x, lastPurPos[0].y - 1).type == 2 || (map->GetMap(lastPurPos[0].x, lastPurPos[0].y - 1).type == 3&& map->GetMap(lastPurPos[0].x, lastPurPos[0].y - 1).dir != MOVE_DOWN)))
	//		{
	//			vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastPurPos[0].x, lastPurPos[0].y - 1)));
	//			if (!Subtract(t,ends).empty())
	//			{
	//				ends.push_back(t[0]);
	//			}
	//		}
	//		if (lastPurPos[0].y + 1 < this->localLeg->getHeight() && (map->GetMap(lastPurPos[0].x, lastPurPos[0].y + 1).type == 2 || (map->GetMap(lastPurPos[0].x, lastPurPos[0].y + 1).type == 3 && map->GetMap(lastPurPos[0].x, lastPurPos[0].y + 1).dir != MOVE_UP)))
	//		{
	//			vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastPurPos[0].x, lastPurPos[0].y + 1)));
	//			if (!Subtract(t, ends).empty())
	//			{
	//				ends.push_back(t[0]);
	//			}
	//		}
	//		if (lastPurPos[0].x - 1 >= 0 && (map->GetMap(lastPurPos[0].x - 1, lastPurPos[0].y).type == 2 || (map->GetMap(lastPurPos[0].x - 1, lastPurPos[0].y).type == 3 && map->GetMap(lastPurPos[0].x - 1, lastPurPos[0].y).dir != MOVE_RIGHT)))
	//		{
	//			vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastPurPos[0].x-1, lastPurPos[0].y)));
	//			if (!Subtract(t, ends).empty())
	//			{
	//				ends.push_back(t[0]);
	//			}
	//		}
	//		if (lastPurPos[0].x + 1 < this->localLeg->getWidth() && (map->GetMap(lastPurPos[0].x + 1, lastPurPos[0].y).type == 2 || (map->GetMap(lastPurPos[0].x + 1, lastPurPos[0].y).type == 3 && map->GetMap(lastPurPos[0].x + 1, lastPurPos[0].y).dir != MOVE_LEFT)))
	//		{
	//			vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastPurPos[0].x + 1, lastPurPos[0].y)));
	//			if (!Subtract(t, ends).empty())
	//			{
	//				ends.push_back(t[0]);
	//			}
	//		}
	//		for (int i = 0; i < ends.size(); i++)
	//		{
	//			for (int j = 0; j < pursuingUnit[0].size(); j++)
	//			{
	//				if (map->FindRealDistance(pursuingUnit[0][j].x, pursuingUnit[0][j].y, ends[i].x, ends[i].y, 1) <= 1)
	//				{
	//					if (pursuingDecision[0][j]==NO_MOVE)
	//					{
	//						pursuingDecision[0][j] = map->pathFind(pursuingUnit[0][j].x, pursuingUnit[0][j].y, ends[i].x, ends[i].y);
	//						break;
	//					}
	//
	//				}
	//			}
	//		}
	//		remainingSteps = -1;
	//		printf("\n aaaaaaaa \n");
	//	}
	//	else
	//	{
	//
	//		if (remainingSteps >= 0)
	//		{
	//			//穷举出最佳决策，（注意考虑对角挟持情况）
	//			bool hasSln = false;
	//			for (int i = 0; i < pow(4, pursuingUnit[0].size()); i++)
	//			{
	//				vector<MapItem> sub = map->getScope(pursuingEnm[0].x, pursuingEnm[0].y, remainingSteps);
	//				for (int j = 0; j < pursuingUnit[0].size(); j++)
	//				{
	//					int t = 1;
	//					for (int k = 0; k < j; k++)
	//					{
	//						t *= 4;
	//					}
	//					switch (i / t % 4)
	//					{
	//					case 0:
	//						if (pursuingUnit[0][j].y - 1 >= 0 && map->GetMap(pursuingUnit[0][j].x, pursuingUnit[0][j].y - 1).type != 1)
	//						{
	//							MapItem center = map->FindTransEnd(map->GetMap(pursuingUnit[0][j].x, pursuingUnit[0][j].y - 1));
	//							sub = Subtract(sub, map->getScope(center.x, center.y, remainingSteps));
	//						}
	//						break;
	//					case 1:
	//						if (pursuingUnit[0][j].y + 1 < localLeg->getHeight() && map->GetMap(pursuingUnit[0][j].x, pursuingUnit[0][j].y + 1).type != 1)
	//						{
	//							MapItem center = map->FindTransEnd(map->GetMap(pursuingUnit[0][j].x, pursuingUnit[0][j].y + 1));
	//							sub = Subtract(sub, map->getScope(center.x, center.y, remainingSteps));
	//						}
	//						break;
	//					case 2:
	//						if (pursuingUnit[0][j].x - 1 >= 0 && map->GetMap(pursuingUnit[0][j].x - 1, pursuingUnit[0][j].y).type != 1)
	//						{
	//							MapItem center = map->FindTransEnd(map->GetMap(pursuingUnit[0][j].x - 1, pursuingUnit[0][j].y));
	//							sub = Subtract(sub, map->getScope(center.x, center.y, remainingSteps));
	//						}
	//						break;
	//					case 3:
	//						if (pursuingUnit[0][j].x + 1 < localLeg->getWidth() && map->GetMap(pursuingUnit[0][j].x + 1, pursuingUnit[0][j].y).type != 1)
	//						{
	//							MapItem center = map->FindTransEnd(map->GetMap(pursuingUnit[0][j].x + 1, pursuingUnit[0][j].y));
	//							sub = Subtract(sub, map->getScope(center.x, center.y, remainingSteps));
	//						}
	//						break;
	//					default:
	//						break;
	//					}
	//				}
	//				if (sub.empty())
	//				{
	//					//存储决策并break
	//					for (int m = 0; m < pursuingDecision[0].size(); m++)
	//					{
	//						int t = 1;
	//						for (int k = 0; k < m; k++)
	//						{
	//							t *= 4;
	//						}
	//						pursuingDecision[0][m] = (DIRECT)(i / t % 4);
	//					}
	//					hasSln = true;
	//					break;
	//				}
	//
	//			}
	//			//若无覆盖方案，则退出围捕模式（对角挟持除外）
	//			if (!hasSln)
	//			{
	//				isPursuing = false;
	//				remainingSteps = -1;
	//				pursuingEnm.clear();
	//				lastPurPos.clear();
	//				pursuingUnit.clear();
	//				pursuingDecision.clear();
	//			}
	//
	//		}
	//
	//	}*/
	//	remainingSteps--;
	//}
	if (isHarass)
	{
		if (harassingEnm.empty() || harassingUnit.empty())
		{
			LOGE("骚扰决策错误！！");
			//退出骚扰模式
			isHarass = false;
			harassingEnm.clear();
			lastHarPos.clear();
			harassingUnit.clear();
			harassingDecision.clear();
			return;
		}



		//更新追捕单位
		harassingDecision.clear();
		for (int i = 0; i < harassingUnit.size(); i++)
		{
			harassingUnit[i] = OurKun[ChangePid(harassingUnit[i].id)];
		}
		harassingDecision = vector<DIRECT>(harassingUnit.size(), NO_MOVE);
		//更新被追捕敌人
		bool isInView = false;
		for (int i = 0; i < enemykunnum; i++)
		{
			if (EnemyKun[i].id == harassingEnm[0].id)
			{
				isInView = true;
				harassingEnm[0] = EnemyKun[i];
			}
		}
		/*if (!isInView && harassingUnit[0].x == lastHarPos[0].x && harassingUnit[0].y == lastHarPos[0].y )
		{
			//退出骚扰模式
		}
		else
		{*/
		if (!isInView)
		{
			vector<MapItem> ends;
			if (lastHarPos.empty())
			{
				LOGE("骚扰决策错误p2！！");
				//退出骚扰模式

				isHarass = false;
				harassingEnm.clear();
				lastHarPos.clear();
				harassingUnit.clear();
				harassingDecision.clear();
				return;
			}
			if (lastHarPos[0].y - 1 >= 0 && (map->GetMap(lastHarPos[0].x, lastHarPos[0].y - 1).type == 2 || (map->GetMap(lastHarPos[0].x, lastHarPos[0].y - 1).type == 3 && map->GetMap(lastHarPos[0].x, lastHarPos[0].y - 1).dir != MOVE_DOWN)))
			{
				vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastHarPos[0].x, lastHarPos[0].y - 1)));
				if (!Subtract(t, ends).empty())
				{
					ends.push_back(t[0]);
				}
			}
			if (lastHarPos[0].y + 1 < this->localLeg->getHeight() && (map->GetMap(lastHarPos[0].x, lastHarPos[0].y + 1).type == 2 || (map->GetMap(lastHarPos[0].x, lastHarPos[0].y + 1).type == 3 && map->GetMap(lastHarPos[0].x, lastHarPos[0].y + 1).dir != MOVE_UP)))
			{
				vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastHarPos[0].x, lastHarPos[0].y + 1)));
				if (!Subtract(t, ends).empty())
				{
					ends.push_back(t[0]);
				}
			}
			if (lastHarPos[0].x - 1 >= 0 && (map->GetMap(lastHarPos[0].x - 1, lastHarPos[0].y).type == 2 || (map->GetMap(lastHarPos[0].x - 1, lastHarPos[0].y).type == 3 && map->GetMap(lastHarPos[0].x - 1, lastHarPos[0].y).dir != MOVE_RIGHT)))
			{
				vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastHarPos[0].x - 1, lastHarPos[0].y)));
				if (!Subtract(t, ends).empty())
				{
					ends.push_back(t[0]);
				}
			}
			if (lastHarPos[0].x + 1 < this->localLeg->getWidth() && (map->GetMap(lastHarPos[0].x + 1, lastHarPos[0].y).type == 2 || (map->GetMap(lastHarPos[0].x + 1, lastHarPos[0].y).type == 3 && map->GetMap(lastHarPos[0].x + 1, lastHarPos[0].y).dir != MOVE_LEFT)))
			{
				vector<MapItem> t = vector<MapItem>(1, map->FindTransEnd(map->GetMap(lastHarPos[0].x + 1, lastHarPos[0].y)));
				if (!Subtract(t, ends).empty())
				{
					ends.push_back(t[0]);
				}
			}
			if (ends.empty())
			{
				//退出骚扰模式
				LOGE("退出骚扰模式. Line 2505");
				isHarass = false;
				harassingEnm.clear();
				lastHarPos.clear();
				harassingUnit.clear();
				harassingDecision.clear();
				for (int i = 0; i < this->ourkunnum; ++i) {
					this->Attacker[i] = FINDER;
				}
				return;
			}
			else
			{
				//lastHarPos[0].x = ends[0].x;
				//lastHarPos[0].y = ends[0].y;
				if (map->FindRealDistance(harassingUnit[0].x, harassingUnit[0].y, ends[0].x, ends[0].y) == 0)
				{
					//退出骚扰模式
					LOGE("退出骚扰模式. Line 2520");
					isHarass = false;
					harassingEnm.clear();
					lastHarPos.clear();
					harassingUnit.clear();
					harassingDecision.clear();
					for (int i = 0; i < this->ourkunnum; ++i) {
						this->Attacker[i] = FINDER;
					}
					return;
				}
				harassingDecision[0] = map->pathFind(harassingUnit[0].x, harassingUnit[0].y, ends[0].x, ends[0].y);
				harassingEnm[0].x = ends[0].x;
				harassingEnm[0].y = ends[0].y;
			}
		}
		else
		{
			int diffX = harassingEnm[0].x - harassingUnit[0].x;
			int diffY = harassingEnm[0].y - harassingUnit[0].y;

			if (abs(diffY) < abs(diffX)) {
				if (diffX < 0) {
					harassingDecision[0] = map->pathFind(harassingUnit[0].x, harassingUnit[0].y, harassingEnm[0].x, harassingEnm[0].y, MOVE_LEFT);
				}
				else {
					harassingDecision[0] = map->pathFind(harassingUnit[0].x, harassingUnit[0].y, harassingEnm[0].x, harassingEnm[0].y, MOVE_RIGHT);
				}
			}
			else {
				if (diffY < 0) {
					harassingDecision[0] = map->pathFind(harassingUnit[0].x, harassingUnit[0].y, harassingEnm[0].x, harassingEnm[0].y, MOVE_UP);
				}
				else {
					harassingDecision[0] = map->pathFind(harassingUnit[0].x, harassingUnit[0].y, harassingEnm[0].x, harassingEnm[0].y, MOVE_DOWN);
				}
			}
		}
		/*}*/

	}

	//在attack中围捕决策优先级高于骚扰决策
}

void OurStrategy::UpdateLastPos()
{
	if (isPursuing)
	{
		bool find = false;
		if (pursuingEnm.empty())
		{
			LOGE("\n 更新上一个被围捕者位置出错！！！ \n");
			return;
		}
		for (int i = 0; i < enemykunnum; i++)
		{
			if (EnemyKun[i].id == pursuingEnm[0].id)
			{
				lastPurPos.clear();
				Point t;
				t.x = EnemyKun[i].x;
				t.y = EnemyKun[i].y;
				lastPurPos = vector<Point>(1, t);
				find = true;
			}
		}
	}
	if (isHarass)
	{
		if (harassingEnm.empty())
		{
			LOGE("\n 更新上一个被骚扰者位置出错！！！ \n");
			return;
		}
		for (int i = 0; i < enemykunnum; i++)
		{
			if (EnemyKun[i].id == harassingEnm[0].id)
			{
				lastHarPos.clear();
				Point t;
				t.x = EnemyKun[i].x;
				t.y = EnemyKun[i].y;
				lastHarPos = vector<Point>(1, t);
			}
		}
	}
}

vector<MapItem> OurStrategy::Subtract(vector<MapItem> left, vector<MapItem> right)
{
	//printf("\n bbbbbbbbbbb \n");
	vector<MapItem> res = left;
	for (int i = 0; i < res.size(); i++)
	{
		for (int j = 0; j < right.size(); j++)
		{
			if (res[i].x == right[j].x && res[i].y == right[j].y)
			{
				res.erase(res.begin() + i);
				i--;
				break;
			}
		}
	}
	//printf("\n bbbbbbbbbbb \n");
	return res;
}




//最终决策
DIRECT OurStrategy::FinalDecision(int pid)
{
	Player thisplayer;
	int i = 0;
	for (; i < this->ourkunnum; i++)//找到对应编号的kun
	{
		if (OurKun[i].id == pid)
		{
			thisplayer = OurKun[i];
			break;
		}
	}
	if (i == ourkunnum)//如果有kun被吃了，直接返回原地停止的指令
	{
		//LOGE("%d号鲲被吃了\n", pid);
		return DIRECT(4);
	}
	LOGE("now in(%d,%d)", thisplayer.x, thisplayer.y);


	// return Normal(thisplayer);


	if (GameSituation() == DEFENSE)//如果是我方的防守回合
	{
		if (EnemyVisNum == 0)//视野范围内无敌人
			return Normal(thisplayer);
		else {
			// 如果你要跑路, 先把你之前的patrolRoundIndex的area解除了再跑路
			if (thisplayer.patrolRoundId + 1 == this->localRound->getRoundId()) {
				Point targetPoint;
				targetPoint.x = this->OurKun[i].patrolPointIndex % this->map->getWidth();
				targetPoint.y = this->OurKun[i].patrolPointIndex / this->map->getHeight();
				Point areaPoint = this->map->getAreaByCoord(targetPoint.x, targetPoint.y);
				this->map->deoccupyArea(areaPoint.x, areaPoint.y);
			}
			return Run(thisplayer);
		}
		//return BasicDecision(thisplayer);测试
		//if (thisplayer.id == BaitId)//诱饵kun做出决定
		//{
		//	if (EnemyVisNum == 0)//视野范围内无敌人
		//		return Normal(thisplayer);
		//	else
		//		return Run(thisplayer);
		//}
		//else
		//{
		//	if (EnemyVisNum == 0)//视野范围内无敌人
		//		return Normal(thisplayer);
		//	else
		//		return Run(thisplayer);
		//}
	}
	else
	{
		return this->Attack(thisplayer);

		// 下面的代码成为历史

		if (isHarass || isPursuing)//如果有人正在骚扰/围捕敌人
		{
			for (int i = 0; i < harassingUnit.size(); i++)
			{
				if (thisplayer.id == harassingUnit[i].id)
				{
					return Attack(thisplayer);
				}
			}
			for (int i = 0; i < pursuingUnit.size(); i++)
			{
				for (int j = 0; j < pursuingUnit[i].size(); j++)
				{
					if (thisplayer.id == pursuingUnit[i][j].id)
					{
						return Attack(thisplayer);
					}
				}
			}
			return Normal(thisplayer);
		}
		else
		{
			return Normal(thisplayer);
		}
		/*if (EnemyVisNum == 0)//视野范围内无敌人
			return Normal(thisplayer);
		else
			return Normal(thisplayer);*/
	}

}

void OurStrategy::ResetAttackerDirections()
{
	for (int i = 0; i < this->ourkunnum; ++i) {
		this->attackerDirections[i] = NO_MOVE;
	}
}
