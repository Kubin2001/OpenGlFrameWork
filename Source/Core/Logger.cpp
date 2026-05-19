#include "Logger.h"

#include <chrono>



void Logger::LogLoop(LogOutput outType) {
	std::vector<LogPack> waitingLogs;
	while (working) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// Transfering data from queue to vector
		{
			std::lock_guard<std::mutex> lock(loggerMut);
			while (!LogQueue.empty()) {
				LogPack log = LogQueue.front();
				LogQueue.pop();
				waitingLogs.emplace_back(log);
			}
		}
		for (auto& log : waitingLogs) {
			std::string* logPrefix = nullptr;
			switch (log.type) {
				case LogType::Undefinded:
					logPrefix = &UndefinedPrefix;
					break;
				case LogType::Info:
					logPrefix = &InfoPrefix;
					break;
				case LogType::Warning:
					logPrefix = &WarningPrefix;
					break;
				case LogType::Error:
					logPrefix = &ErrorPrefix;
					break;
				case LogType::Critical:
					logPrefix = &CriticalPrefix;
					break;
				default:
					logPrefix = &UndefinedPrefix;
					break;
			}
			if (outType == LogOutput::Console) {
				std::println("{} {}", *logPrefix, log.message);
			}
			else {
				if (!outputFile.is_open()) { return; }

				{
					std::lock_guard<std::mutex> lock2(fileMut);
					outputFile << std::format("{} {}\n", *logPrefix, log.message);
				}
			}
		}
		waitingLogs.clear();
	}
}


bool Logger::SetUp(const std::string& outFolder, LogOutput debugOutput, LogOutput releaseOutput) {
	if (working) { return false; }
#ifdef _DEBUG
	if (debugOutput == LogOutput::File) {
		outputType = LogOutput::File;
	}
#else
	if (releaseOutput == LogOutput::File) {
		outputType = LogOutput::File;
	}
#endif 

	if (outputType == LogOutput::File) {
		std::string fileName = "Logs_" + std::to_string(static_cast<size_t> (time(0))) + ".txt";
		std::string fullPath = outFolder;
		if (!fullPath.empty() && fullPath.back() != '/' && fullPath.back() != '\\') {
			fullPath += "/";
		}

		outputFile = std::ofstream(fullPath + fileName);
		if (!outputFile.is_open()) {
			return false; 
		}
	}
	working = true;
	worker = std::thread(Logger::LogLoop, outputType);

	return true;
}

bool Logger::Log(const std::string &msg, LogType type) {
	if (!working) { return false; }
	{
		std::lock_guard<std::mutex> lock(loggerMut);
		if (type == LogType::Error || type == LogType::Critical) { // Program is in very bad state push log no matter what
			std::string* logPrefix = nullptr;
			switch (type) {
				case LogType::Error:
					logPrefix = &ErrorPrefix;
					break;
				case LogType::Critical:
					logPrefix = &CriticalPrefix;
					break;
				default:
					logPrefix = &UndefinedPrefix;
					break;
			}
			if (outputType == LogOutput::Console) {
				std::println("{} {}", *logPrefix, msg);
			}
			else {
				if (!outputFile.is_open()) { return false; }
				{
					std::lock_guard<std::mutex> lock2(fileMut);
					outputFile << std::format("{} {}\n", *logPrefix, msg);
				}
			}
			return true;
		}


		LogQueue.push({type,msg});
	}
	return true;
}

void Logger::Close() {
	working = false;
	worker.join();
	outputFile.close();
}