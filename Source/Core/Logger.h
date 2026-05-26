#pragma once

#include <string>
#include <print>
#include <format>
#include <mutex>
#include <fstream>
#include <atomic>
#include <queue>

enum class LogType{
	Undefinded,
	Info,
	Warning,
	Error,
	Critical
};

enum class LogOutput {
	Console,
	File
};


class Logger {
	private:

	inline static std::atomic<bool> working = false;
	inline static LogOutput outputType = LogOutput::Console;
	inline static std::mutex loggerMut{};
	inline static std::mutex fileMut{};

	inline static std::string UndefinedPrefix = "";
	inline static std::string InfoPrefix = "[INFO]";
	inline static std::string WarningPrefix = "[WARNING]";
	inline static std::string ErrorPrefix = "[ERROR]";
	inline static std::string CriticalPrefix = "[CRITICAL]";

	inline static std::ofstream outputFile{};
	inline static std::queue<std::string> LogQueue{};
	inline static std::thread worker{};
 
	static void LogLoop(LogOutput outType);

	public:
	static bool SetUp(const std::string &outFolder = "", LogOutput debugOutput = LogOutput::Console, LogOutput releaseOutput = LogOutput::File);

	static bool Log(const std::string &msg, LogType type = LogType::Undefinded);

	static void SetPrefix(LogType type, const std::string& prefix);

	static void  Close();

};
