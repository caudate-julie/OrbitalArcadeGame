#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <chrono>

typedef std::chrono::high_resolution_clock clocks;

class Logger
{
public:
	Logger(void);
	~Logger(void);

	void start(std::string logmessage);
	void stop(std::string logmessage);
	std::string output();

private:
	struct LogItem
	{
		float values[100];
		float mean;
		int i_max;
		int current_index;
		clocks::time_point start;

		LogItem();
		void find_max();
		void stoptimer();
	};

	std::map<std::string, LogItem> measures;
};

