#include "myLog.h"

std::mutex mux;
static FILE * fp = NULL;
int MaxLenChar = 1024 * 1024 * 5; // 5 M Êý¾Ý

int LOGINIT(char *path)
{
	fp = fopen(path, "w+");
	if (fp == NULL)
	{
		return -1;
	}
	return 0;
}
int LOGSetMaxSpace(int mSize)
{
	MaxLenChar = mSize;
	return 0;
}

int logWrite(char * s)
{
	if (fp)
	{
		int len = ftell(fp);
		if (len > MaxLenChar)
		{
			LOGEnd();
			return 0;
		}
		mux.lock();
		fprintf(fp, "%s", s);
		fflush(fp);
		mux.unlock();
	}

	return 0;
}

int LOGEnd()
{
	if (fp)
	{
		fflush(fp);
		fclose(fp);
		fp = 0;
	}
	return 0;
}

std::string getCurrentSystemTime()
{
	auto time_now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(time_now);
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
	auto duration_in_s = std::chrono::duration_cast<std::chrono::seconds>(time_now.time_since_epoch());
	int theMs = duration_in_ms.count() - duration_in_s.count() * 1000;
	struct tm* ptm = localtime(&tt);


	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d-%02d.%02d.%02d.%03d%",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
		(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec, theMs);
	return std::string(date);
}
long long getCurrentMs()
{
	auto time_now = std::chrono::system_clock::now();
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());

	return duration_in_ms.count();
}
std::string GetName(char * fileName)
{
	char temp[256] = { 0 };
	memcpy(temp, fileName, strlen(fileName) + 1);

	std::string sName = temp;

#ifdef WIN32
	int startP = sName.find_last_of("\\");
	if (startP == -1)
	{
		startP = 0;
	}
	std::string theLastName = sName.substr(startP + 1, sName.size() - startP);

#else
	int startP = sName.find_last_of("/");
	if (startP == -1)
	{
		startP = 0;
	}
	std::string theLastName = sName.substr(startP + 1, sName.size() - startP);

#endif
	return theLastName;
}