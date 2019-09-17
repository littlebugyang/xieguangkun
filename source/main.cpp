/*
 * @Date: 2019-08-12 11:31:51
 * @LastEditors: lby
 */
 //Defines the entry point for the console application.


/*

注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!
注意! 本程序可能有内存泄漏!

*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <chrono>   
using namespace std;
using namespace chrono;

//user defined
#include "message.h"
#include "strategy.h"
#include "socketClient.h"
#include "myLog.h"

int main(int argc, char* argv[])
{
	char logPath[] = "/var/log/battle.log";

	if (argc < 4)
	{
		LOGE("Usage: %s [player_id] [serverIp] [serverPort]\n", argv[0]);
		return -1;
	}

	/* 提取命令行参数 */

	unsigned long serverIp = inet_addr(argv[2]);
	unsigned short serverPort = atoi(argv[3]);

#if defined(_MSC_VER)//win
#pragma comment(lib,"ws2_32.lib")

	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsAdata;

	if (0 != WSAStartup(wVersionRequested, &wsAdata))
	{
		return -1;
	}

	if ((LOBYTE(wsAdata.wVersion) != 1) || (HIBYTE(wsAdata.wVersion) != 1))
	{
		WSACleanup();
		return -1;
	}

	// windows下存放日志文件
	strcpy(logPath, "./battle.log");
#endif

	// 创建日志文件
	LOGINIT(logPath);

	time_t now = time(0);
	char* timeStr = ctime(&now);
	LOGE("Current time is: %s", timeStr);

	/* 创建socket */
	SocketClient client(serverIp, serverPort);
	//    sockClient = socket(AF_INET,SOCK_STREAM,0);

	//    /* 连接server */
	//    struct sockaddr_in addrSrv;
	//    bzero( &addrSrv, sizeof(addrSrv) );
	//    addrSrv.sin_addr.s_addr = serverIp;
	//    addrSrv.sin_family = AF_INET;
	//    addrSrv.sin_port = htons(serverPort);

	LOGE("try to connect server(%s:%u)", inet_ntoa(client.addrSrv_.sin_addr), ntohs(client.addrSrv_.sin_port));

	while (0 != client.Connect())
	{
		sleep(10);
	};

	LOGE("connect server success", inet_ntoa(client.addrSrv_.sin_addr), ntohs(client.addrSrv_.sin_port));

	int myTeamId = atoi(argv[1]);
	int myPlayerId[4] = { 0 };

	/* 向server注册 */
	char regMsg[200] = { '\0' };
	snprintf(regMsg, sizeof(regMsg), "{\"msg_name\":\"registration\",\"msg_data\":{\"team_id\":%d,\"team_name\":\"xgk\"}}", myTeamId);
	char regMsgWithLength[200] = { '\0' };
	snprintf(regMsgWithLength, sizeof(regMsgWithLength), "%05d%s", (int)strlen(regMsg), regMsg);
	client.Send(regMsgWithLength, (int)strlen(regMsgWithLength), 0);
	//    send(sockClient, regMsgWithLength, (int)strlen(regMsgWithLength), 0);

	LOGE("register my info to server success\n");
	OurStrategy strategy = OurStrategy(myTeamId); // 设置我方teamId,  该参数一直不变

	RoundMsg* globalRoundMsg = NULL;
	LegStartMsg* globalLegMsg = NULL;

	/* 进入游戏 */
	int temptime = 0;
	while (1)
	{
		char buffer[99999] = { '\0' };
		//        int size = recv(sockClient, buffer, sizeof(buffer)-1, 0);
		int size = client.Recv(buffer, sizeof(buffer) - 1, 0);

		if (size > 0)
		{
			LOGE("\r Round Server Msg: %s\r", buffer);

			cJSON* msgBuf = cJSON_Parse(buffer + 5);
			if (NULL == msgBuf) continue;

			LOGE("\r OnMssageRecv: %d\r ", clock());
			cJSON* msgNamePtr = cJSON_GetObjectItem(msgBuf, "msg_name");
			if (NULL == msgNamePtr) continue;

			char* msgName = msgNamePtr->valuestring;

			if (0 == strcmp(msgName, "round"))
			{
				/* ------------- 计时 ----------------- */
				auto start = system_clock::now();

				//printf("the round is start, myPlayerId are:%d,%d,%d,%d\n", myPlayerId[0], myPlayerId[1], myPlayerId[2], myPlayerId[3]);
				//RoundMsg roundMsg(msgBuf);
				//roundMsg.DecodeMessge();
				if (globalRoundMsg)
				{
					delete globalRoundMsg;
					//printf("round1\n");
					globalRoundMsg = NULL;
				}
				globalRoundMsg = new RoundMsg(msgBuf);
				globalRoundMsg->DecodeMessge();
				strategy.updateRound(globalRoundMsg);
				//printf("%d\n", sizeof(globalRoundMsg->getPlayers()));

				//根据策略和寻路决定下一步的动作，向服务器发送action消息
				ActMsg actMsg(globalRoundMsg->getRoundId());
				LOGE("********************round %d is start********************", globalRoundMsg->getRoundId());
				strategy.DistinguishPlayers(myPlayerId[0], myPlayerId[1], myPlayerId[2], myPlayerId[3]);//将敌我kun
				strategy.UpdateDispersed();
				if (strategy.GameSituation() != DEFENSE)
					strategy.updateAttacker();
				else
					strategy.updateEnemyKun();
				//strategy.UpdateBait();//选择诱饵kun
				//strategy.CheckSituation();

				for (int index = 0; index < 4; ++index)
				{
					LOGE("----------------------------------------------");
					strategy.AddMapFrequency(myPlayerId[index]);
					strategy.GetEnemyVis(myPlayerId[index]);
					strategy.GetPowerVis(myPlayerId[index]);
					LOGE("mykun(id: %d)\ncan see %d enemys and %d powers", myPlayerId[index], strategy.GetEnemyVisNum(), strategy.GetPowerVisNum());
					SubAction action;
					action.moveDirect = strategy.FinalDecision(myPlayerId[index]);//我方kun做出移动决定
					if (action.moveDirect == NO_MOVE)
					{
						LOGE("DEAD!reset it's block state");
						strategy.sethasBlock(index);
					}
					LOGE("decides it's direct:%d", action.moveDirect);
					LOGE("----------------------------------------------");
					actMsg.AddSubAction(myTeamId, myPlayerId[index], action);//发送决定
				}

				LOGE("%d", strategy.gethasBlock(0));
				LOGE("%d", strategy.gethasBlock(1));
				LOGE("%d", strategy.gethasBlock(2));
				LOGE("%d", strategy.gethasBlock(3));

				strategy.map->printfAreaFre();
				strategy.UpdateLastPos();

				const int maxActMsgLenth = 9999;
				char msgToSend[maxActMsgLenth] = { 0 };
				actMsg.PackActMsg(msgToSend, maxActMsgLenth);
				LOGE("\nsend message:\n%s", msgToSend);

				auto end = system_clock::now();
				auto duration = duration_cast<microseconds>(end - start);
				LOGE("这一回合在计算上花费的时间是: %f秒", double(duration.count()) * microseconds::period::num / microseconds::period::den);

				client.Send(msgToSend, (int)strlen(msgToSend), 0);
			}
			else if (0 == strcmp(msgName, "leg_start"))
			{
				//LegStartMsg legMsg(msgBuf);
				//legMsg.DecodeMessge(myTeamId,myPlayerId);
				if (globalLegMsg)
				{
					delete globalLegMsg;
					globalLegMsg = NULL;
				}
				globalLegMsg = new LegStartMsg(msgBuf);
				globalLegMsg->DecodeMessge(myTeamId, myPlayerId);
				strategy.initKunBlock(myPlayerId[0], myPlayerId[1], myPlayerId[2], myPlayerId[3]);
				LOGE("game start\n");
				strategy.updateLeg(globalLegMsg);
				strategy.initEnemyKun();
				//DIRECT test = strategy.map->pathFind(18, 14, 13, 7);
				//printf("TeST");
			}
			else if (0 == strcmp(msgName, "leg_end"))
			{
				LegEndMsg legMsg(msgBuf);
				legMsg.DecodeMessge();
			}
			else if (0 == strcmp(msgName, "game_over"))
			{
				break;
			}
		}
		/* 如果收到game_over消息, 请跳出循环，进入释放资源程序退出阶段 */
	}

	delete globalRoundMsg;
	delete globalLegMsg;

	//close(sockClient);
	client.Close();
	LOGEnd();

	return 0;
}
