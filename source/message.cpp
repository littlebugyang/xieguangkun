#include "message.h"
#include <cstring>
#include "stdio.h"
#include "stdlib.h"
#include "myLog.h"

const char* direct[] = {"up","down","left","right",""};

ActMsg::ActMsg(int roundId)
{
    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root, "msg_name", cJSON_CreateString("action"));
    cJSON_AddItemToObject(root, "msg_data", msg_data=cJSON_CreateObject());
    cJSON_AddNumberToObject(msg_data,"round_id",roundId);
    cJSON_AddItemToObject(msg_data, "actions", actions=cJSON_CreateArray());
}

void ActMsg::AddSubAction(int team, int player, SubAction& act)
{
    cJSON_AddItemToArray(actions,subAct=cJSON_CreateObject());
    cJSON_AddNumberToObject(subAct,"team",team);
    cJSON_AddNumberToObject(subAct,"player_id",player);
    cJSON_AddItemToObject(subAct, "move", move=cJSON_CreateArray());
    if (act.moveDirect < NO_MOVE)
    {
        cJSON_AddItemToArray(move,cJSON_CreateString(direct[act.moveDirect]));
    }
}

void ActMsg::PackActMsg(char* actMsg, int maxMsgLenth)
{
    char* msg = cJSON_PrintUnformatted(root);
    // printf("\r\nSendActMsg:%s\r\n",msg);

    snprintf(actMsg, maxMsgLenth, "%05d%s", (int)strlen(msg), msg);
	delete[] msg;
    //free(msg);
}



void LegStartMsg::DecodeMessge(int& myTeamId,int myPlayerId[4])
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* width = cJSON_GetObjectItem(map,"width");
    if(NULL == width) return;
	this->width = width->valueint;
    
    cJSON* height = cJSON_GetObjectItem(map,"height");
    if(NULL == height) return;
	this->height = height->valueint;
    
    cJSON* vision = cJSON_GetObjectItem(map,"vision");
    if(NULL == vision) return;
	this->vision = vision->valueint;

     GetMeteor(myTeamId);
     GetWormhole(myTeamId);
     GetTunnel(myTeamId);
	 GetCloud(myTeamId);
     GetMyTeamInfo(myTeamId,myPlayerId);
	
}

void LegStartMsg::GetMyTeamInfo(int& myTeamId,int myPlayerId[4])
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");

    cJSON* teams = cJSON_GetObjectItem(msg_data,"teams");
    if(NULL == teams) return;

    int len = cJSON_GetArraySize(teams);
    if(len > 4)  return;

	// todo: 该处没有严格释放StartTeam下的players的资源
	for (unsigned short i = 0; i < 2; ++i) {
		if (this->teams[i].players != NULL) {
			delete[] this->teams[i].players;
		}
	}
	delete[] this->teams;//释放指针指向的资源
	this->teams = new StartTeam[len];
	this->myId = myTeamId;
	this->teamsCount = len;

    for (int i = 0; i< len; ++i)
    {
        cJSON* teamInfo = cJSON_GetArrayItem(teams,i);
        if (NULL != teamInfo)
        {
            cJSON* id = cJSON_GetObjectItem(teamInfo,"id");
            if (NULL == id) return;
			this->teams[i].id = id->valueint;
			//if (myTeamId != id->valueint) continue;

			cJSON* force = cJSON_GetObjectItem(teamInfo, "force");
			if (NULL == force) return;
			this->teams[i].force = force->valuestring;

            cJSON* players  = cJSON_GetObjectItem(teamInfo,"players");
            if(NULL == players) return;

            int playerCount = cJSON_GetArraySize(players);
			//delete[] this->teams[i].players;//释放指针指向的资源
			this->teams[i].players = new int[playerCount];

            for (int j = 0; j < playerCount; ++j)
            {
                cJSON* playerInfo = cJSON_GetArrayItem(players,j);
                if(NULL == playerInfo) return;
				this->teams[i].players[j] = playerInfo->valueint;
              
				if (myTeamId == id->valueint)
				{
					myPlayerId[j] = playerInfo->valueint;
				}
            }
        }
    }
}

void LegStartMsg::GetMeteor(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* meteor = cJSON_GetObjectItem(map,"meteor");
    if(NULL == meteor) return;

    int len = cJSON_GetArraySize(meteor);
	delete[] this->meteors;//释放指针指向的资源，(出错了！！)
	this->meteorsCount = len;
	if (len<=0)
	{
		return;
	}
	this->meteors = new Meteor[len];
	

    for (int i = 0; i< len; ++i)
    {
        cJSON* sub_meteor = cJSON_GetArrayItem(meteor,i);
        if (NULL != sub_meteor)
        {
            cJSON* x = cJSON_GetObjectItem(sub_meteor,"x");
            if (NULL == x) return;
			this->meteors[i].x = x->valueint;

            cJSON* y  = cJSON_GetObjectItem(sub_meteor,"y");
            if(NULL == y) return;
			this->meteors[i].y = y->valueint;
        }
    }
}

void LegStartMsg::GetWormhole(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
if (NULL == map) return;

cJSON* wormhole = cJSON_GetObjectItem(map, "wormhole");
if (NULL == wormhole) return;


int wormhole_len = cJSON_GetArraySize(wormhole);
delete[] this->wormholes;//释放指针指向的资源
this->wormholesCount = wormhole_len;
if (wormhole_len <= 0)
{
	return;
}
this->wormholes = new Wormhole[wormhole_len];


for (int i = 0; i < wormhole_len; ++i)
{
	cJSON* sub_wormhole = cJSON_GetArrayItem(wormhole, i);
	if (NULL != sub_wormhole)
	{
		cJSON* x = cJSON_GetObjectItem(sub_wormhole, "x");
		if (NULL == x) return;
		this->wormholes[i].x = x->valueint;

		cJSON* y = cJSON_GetObjectItem(sub_wormhole, "y");
		if (NULL == y) return;
		this->wormholes[i].y = y->valueint;

		cJSON* name = cJSON_GetObjectItem(sub_wormhole, "name");
		if (NULL == name) return;
		//this->wormholes[i].name = "A";
		//printf("\n 黑洞 数组：%s \n", this->wormholes[i].name);
		this->wormholes[i].name = name->valuestring;
		//printf("\n 黑洞 数组：%s \n", name->valuestring);
		//printf("\n 黑洞 数组：%s \n", this->wormholes[i].name.c_str());
	}
}
}

void LegStartMsg::GetTunnel(int& myTeamId)
{
	cJSON* msg_data = cJSON_GetObjectItem(root, "msg_data");
	if (NULL == msg_data) return;

	cJSON* map = cJSON_GetObjectItem(msg_data, "map");
	if (NULL == map) return;

	cJSON* tunnel = cJSON_GetObjectItem(map, "tunnel");
	if (NULL == tunnel) return;

	int tunnel_len = cJSON_GetArraySize(tunnel);
	delete[] this->tunnels;//释放指针指向的资源
	this->tunnelsCount = tunnel_len;
	if (tunnel_len <= 0)
	{
		return;
	}
	this->tunnels = new Tunnel[tunnel_len];


	for (int i = 0; i < tunnel_len; ++i)
	{
		cJSON* sub_tunnel = cJSON_GetArrayItem(tunnel, i);
		if (NULL != sub_tunnel)
		{
			cJSON* x = cJSON_GetObjectItem(sub_tunnel, "x");
			if (NULL == x) return;
			this->tunnels[i].x = x->valueint;

			cJSON* y = cJSON_GetObjectItem(sub_tunnel, "y");
			if (NULL == y) return;
			this->tunnels[i].y = y->valueint;

			cJSON* direction = cJSON_GetObjectItem(sub_tunnel, "direction");
			if (NULL == direction) return;
			if (strcmp(direction->valuestring, "down") == 0)
			{
				this->tunnels[i].direction = MOVE_DOWN;
			}
			else if (strcmp(direction->valuestring, "up") == 0)
			{
				this->tunnels[i].direction = MOVE_UP;
			}
			else if (strcmp(direction->valuestring, "left") == 0)
			{
				this->tunnels[i].direction = MOVE_LEFT;
			}
			else if (strcmp(direction->valuestring, "right") == 0)
			{
				this->tunnels[i].direction = MOVE_RIGHT;
			}
		}
	}

}

string LegStartMsg::getMyForce(int teamId)
{
	for (int i = 0; i < 2; ++i) {
		if (this->teams[i].id == teamId) {
			return this->teams[i].force;
		}
	}
	return string("beat");
}


void LegStartMsg::GetCloud(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* cloud = cJSON_GetObjectItem(map,"cloud");
    if(NULL == cloud) return;

    int cloud_len = cJSON_GetArraySize(cloud);
    
    for (int i = 0; i< cloud_len; ++i)
    {
        cJSON* sub_cloud = cJSON_GetArrayItem(cloud,i);
        if (NULL != sub_cloud)
        {
            cJSON* x = cJSON_GetObjectItem(sub_cloud,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_cloud,"y");
            if(NULL == y) return;
        }
    }
    
}



void RoundMsg::DecodePower(cJSON *power)
{
	//char *string = cJSON_Print(power);
	//printf("value =%s\n", string);

    int len = cJSON_GetArraySize(power);
	delete[] this->powers;//释放指针指向的资源
	this->powersCount = len;
	if (len<=0)
	{
		return;
	}
	this->powers = new Power[len];
	
    for (int i = 0; i < len; ++i)
    {
        cJSON* powerInfo = cJSON_GetArrayItem(power,i);
        if (NULL != powerInfo)
        {
            cJSON* x     = cJSON_GetObjectItem(powerInfo,"x");
            cJSON* y     = cJSON_GetObjectItem(powerInfo,"y");
            cJSON* point = cJSON_GetObjectItem(powerInfo,"point");
            if((NULL == x)||(NULL == y)||(NULL == powerInfo)) return;

			this->powers[i].x = x->valueint;//为数据成员逐个赋值
			this->powers[i].y = y->valueint;
			this->powers[i].point = point->valueint;

            //使用字段(x->valueint,y->valueint,point->valueint)
        }
    }
}

void RoundMsg::DecodePlayers(cJSON *players)
{
    int len = cJSON_GetArraySize(players);
	if (this->players)
	{
		delete[] this->players;//释放指针指向的资源
		this->players = NULL;
	}
	this->playersCount = len;
	if (len<=0)
	{
		return;
	}
	this->players = new Player[len];
	
    for (int i = 0; i < len; ++i)
    {
        cJSON* playerInfo = cJSON_GetArrayItem(players,i);
        if (NULL != playerInfo)
        {
            cJSON* id= cJSON_GetObjectItem(playerInfo,"id");
            cJSON* score = cJSON_GetObjectItem(playerInfo,"score");
            cJSON* sleep= cJSON_GetObjectItem(playerInfo,"sleep");
            cJSON* team = cJSON_GetObjectItem(playerInfo,"team");
            cJSON* x = cJSON_GetObjectItem(playerInfo,"x");
            cJSON* y = cJSON_GetObjectItem(playerInfo,"y");
            if((NULL == id)||(NULL == score)) return;

			this->players[i].id = id->valueint;//为数据成员逐个赋值
			this->players[i].score = score->valueint;
			this->players[i].sleep = sleep->valueint;
			this->players[i].team = team->valueint;
			this->players[i].x = x->valueint;
			this->players[i].y = y->valueint;

			//printf("value =%s\n", "------------------------");
			//char *string = cJSON_Print(playerInfo);
			//printf("value =%s\n", string);
			////printf("value =%s\n", score->valuestring);
			////printf("value =%s\n", sleep->valuestring);
			//printf("value =%s\n", "------------------------");
   //         //使用字段(x->valueint,y->valueint,0)
        }
    }
}

void RoundMsg::DecodeTeams(cJSON *teams)
{
    int len = cJSON_GetArraySize(teams);
	delete[] this->teams;//释放指针指向的资源
	this->teamsCount = len;
	if (len<=0)
	{
		return;
	}
	this->teams = new Team[len];
	
    for (int i = 0; i < len; ++i)
    {
        cJSON* teamInfo = cJSON_GetArrayItem(teams,i);
        if (NULL != teamInfo)
        {
            cJSON* id    = cJSON_GetObjectItem(teamInfo,"id");
            cJSON* point = cJSON_GetObjectItem(teamInfo,"point");
			cJSON* remain_life = cJSON_GetObjectItem(teamInfo,"remain_life");
            if((NULL == id)||(NULL == point)) return;

			this->teams[i].id = id->valueint;//为数据成员逐个赋值
			this->teams[i].point = point->valueint;
			this->teams[i].remain_life = remain_life->valueint;

            //使用字段(id->valueint,point->valueint,remain_life->valueint)
        }
    }
}

void RoundMsg::DecodeMessge()
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* round_id = cJSON_GetObjectItem(msg_data,"round_id");
    if(NULL == round_id) return;
	this->round_id = round_id->valueint;

	cJSON*  mode = cJSON_GetObjectItem(msg_data, "mode");
	if (NULL == mode) return;
	this->mode = mode->valuestring;

	LOGE("\r\n ============ roundId: %d ============ \r\n ", round_id->valueint);

    cJSON* players = cJSON_GetObjectItem(msg_data,"players");
	if (NULL == players)
	{
		playersCount = 0;
		return;
	}
    DecodePlayers(players);

    cJSON* teams = cJSON_GetObjectItem(msg_data,"teams");
    if(NULL == teams) return;
    DecodeTeams(teams);  


    cJSON* power = cJSON_GetObjectItem(msg_data,"power");
	if (NULL == power)
	{
		powersCount = 0;
		return;
	}
    DecodePower(power);
}

