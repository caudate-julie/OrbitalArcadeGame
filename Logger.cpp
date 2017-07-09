#include "Logger.h"
#include <sstream>
#include <iomanip>

Logger::Logger(void) {}

Logger::~Logger(void)
{
	std::fstream logfile;
	logfile.open("timelog.log");
	//for (std::map<std::string, LogItem> iterator = measures.begin(); 
	logfile.close();
}

void Logger::start(std::string item)
{
	measures[item].start = clocks::now();
}

void Logger::stop(std::string item)
{
	auto it = measures.find(item);
	if (it == measures.end()) { return; }   // measures[item] does not exist
	measures[item].stoptimer();
}

std::string Logger::output()
{
	std::stringstream ss;
	for (auto it = measures.begin(); it != measures.end(); it++)
	{
		ss << std::setprecision(2) << std::showpoint << std::fixed << std::left
		   << std::setw(20) << it->first << " : "
		   << std::setw(8) << it->second.mean  << " : "
		   << std::setw(8) << it->second.values[it->second.i_max] << "\n\n";
	}
	return ss.str();
}

Logger::LogItem::LogItem()
{
	for (int i = 0; i < FRAMES; i++) values[i] = 0;
	mean = 0;
	i_max = FRAMES - 1;
	current_index = 0;
}

void Logger::LogItem::find_max()
{
	i_max = 0;
	for (int i = 0; i < FRAMES; i++) 
	{
		if (values[i] > values[i_max]) i_max = i; 
	}
}

void Logger::LogItem::stoptimer()
{
	std::chrono::duration<double, std::milli> duration_span = clocks::now() - start;
	float duration = duration_span.count();
	mean += (duration - values[current_index]) / (float)FRAMES;
	if (duration > values[i_max]) { i_max = current_index; }
	else if (i_max == current_index) { find_max(); }

	values[current_index] = duration;
	current_index = (current_index == FRAMES - 1) ? 0 : current_index + 1;
}

Logger* logger;