#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/Parser/Arxml/ArxmlParser.h>
#include <OptiScan/Parser/Arxml/ArxmlTokenReader.h>
#include <OptiScan/View/Console/ConsoleLogHandler.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace OptiScan::Core::Database;
using namespace OptiScan::Parser::Arxml;
using namespace OptiScan::View::Console;

namespace
{
	// ----------------------------------------------------------------------------
	// Temporary wiring while the parsers are being built. Paths are hard coded so
	// the program runs straight from the IDE without arguments. To be replaced by a
	// proper command line and by the test project in P7.
	// ----------------------------------------------------------------------------

	/** Set to false to run the CodeGenerator itself instead of the current check. */
	const bool RunArxmlCheck = true;

	const std::string ArxmlPath = "C:/CodeGenerator/Test/BACKBONE_EXT_CAN_2025_31a.arxml";
	const std::string ConfigPath = "C:/CodeGenerator/Test/ConfigTest.json";

	/** Check path for the ARXML reader (plan 20-parser-arxml.md, N1).
	 *  Counts the opening tags per element name so the result can be held against an
	 *  independently produced count. */
	int runArxmlCheck()
	{
		std::cout << "=== " << ArxmlPath << std::endl;
		std::ifstream file(ArxmlPath, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "file not found" << std::endl;
			return 1;
		}

		ArxmlTokenReader reader(&file);
		std::map<std::string, uint64_t> counts;
		uint64_t nodes = 0;
		uint64_t texts = 0;
		uint64_t maxDepth = 0;
		const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		try
		{
			while (reader.readNext())
			{
				nodes++;
				if (reader.getKind() == ArxmlNodeKind::ElementBegin)
				{
					counts[reader.getName()]++;
					maxDepth = std::max(maxDepth, reader.getDepth());
				}
				else if (reader.getKind() == ArxmlNodeKind::Text)
				{
					texts++;
				}
			}
		}
		catch (const std::exception & error)
		{
			std::cerr << "FAILED: " << error.what() << std::endl;
			return 1;
		}
		const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::vector<std::pair<std::string, uint64_t>> sorted(counts.begin(), counts.end());
		std::sort(sorted.begin(), sorted.end(),
			[](const std::pair<std::string, uint64_t> & left, const std::pair<std::string, uint64_t> & right)
			{
				return left.second != right.second ? right.second < left.second : left.first < right.first;
			});

		std::cout << "  nodes:          " << nodes << std::endl;
		std::cout << "  text nodes:     " << texts << std::endl;
		std::cout << "  distinct names: " << counts.size() << std::endl;
		std::cout << "  max depth:      " << maxDepth << std::endl;
		std::cout << "  duration:       "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< " ms" << std::endl;
		std::cout << "--- elements by count" << std::endl;
		for (const std::pair<std::string, uint64_t> & entry : sorted)
		{
			std::cout << "  " << entry.second << "\t" << entry.first << std::endl;
		}
		return 0;
	}

	/** Check path for the path index (plan 20-parser-arxml.md, N2). */
	int runArxmlIndexCheck()
	{
		std::cout << "=== " << ArxmlPath << std::endl;
		std::ifstream file(ArxmlPath, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "file not found" << std::endl;
			return 1;
		}

		ArxmlParser parser(&file);
		ArxmlDatabase database;
		const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		try
		{
			parser.parse(database);
		}
		catch (const std::exception & error)
		{
			std::cerr << "FAILED: " << error.what() << std::endl;
			return 1;
		}
		const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::map<std::string, uint64_t> counts;
		for (const std::pair<const std::string, ArxmlIdentifiable> & entry : database._pathIndex)
		{
			counts[entry.second._elementType]++;
		}
		std::vector<std::pair<std::string, uint64_t>> sorted(counts.begin(), counts.end());
		std::sort(sorted.begin(), sorted.end(),
			[](const std::pair<std::string, uint64_t> & left, const std::pair<std::string, uint64_t> & right)
			{
				return left.second != right.second ? right.second < left.second : left.first < right.first;
			});

		std::cout << "  indexed paths:  " << database._pathIndex.size() << std::endl;
		std::cout << "  duplicates:     " << database._duplicatePaths.size() << std::endl;
		std::cout << "  element types:  " << counts.size() << std::endl;
		std::cout << "  duration:       "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< " ms" << std::endl;
		for (size_t i = 0; i < database._duplicatePaths.size() && i < 10; i++)
		{
			std::cout << "    duplicate: " << database._duplicatePaths[i] << std::endl;
		}
		std::cout << "--- indexed elements by type" << std::endl;
		for (const std::pair<std::string, uint64_t> & entry : sorted)
		{
			std::cout << "  " << entry.second << "\t" << entry.first << std::endl;
		}
		std::cout << "--- sample paths" << std::endl;
		size_t shown = 0;
		for (const std::pair<const std::string, ArxmlIdentifiable> & entry : database._pathIndex)
		{
			if (shown < 5 && (entry.second._elementType == "CAN-FRAME" || entry.second._elementType == "I-SIGNAL"))
			{
				std::cout << "  " << entry.second._elementType << "\t" << entry.first << std::endl;
				shown++;
			}
		}
		return 0;
	}

	/** */
	int runCodeGenerator()
	{
		ConsoleLogHandler logHandler;
		SystemDatabase systemDatabase;
		systemDatabase.setLogHandler(&logHandler);
		try
		{
			systemDatabase.loadFromConfigFile(ConfigPath);
			systemDatabase.loadSelectionTables();
			systemDatabase.loadVmms();
			systemDatabase.loadMapping();
		}
		catch (const std::exception & error)
		{
			logHandler.logError(std::string("CodeGenerator: ") + error.what());
			return 1;
		}
		return 0;
	}
}

int main()
{
	int result = 0;
	if (RunArxmlCheck)
	{
		result = runArxmlIndexCheck();
	}
	else
	{
		result = runCodeGenerator();
	}
	return result;
}
