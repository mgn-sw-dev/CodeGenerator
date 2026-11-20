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

	/** Locates every COMPU-RATIONAL-COEFFS and reports the elements enclosing it.
	 *  Answers why the model holds fewer linear scales than the file has coefficient
	 *  blocks. Temporary, remove once the question is settled. */
	int reportRationalCoeffParents()
	{
		int result = 0;
		std::ifstream file(ArxmlPath, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "file not found" << std::endl;
			result = 1;
		}
		else
		{
			ArxmlTokenReader reader(&file);
			std::vector<std::string> stack;
			std::map<std::string, uint64_t> chains;
			while (reader.readNext())
			{
				if (reader.getKind() == ArxmlNodeKind::ElementBegin)
				{
					if (reader.getName() == "COMPU-RATIONAL-COEFFS")
					{
						std::string chain;
						size_t from = 0;
						if (4 < stack.size())
						{
							from = stack.size() - 4;
						}
						for (size_t i = from; i < stack.size(); i++)
						{
							chain += "/";
							chain += stack[i];
						}
						chains[chain]++;
					}
					stack.push_back(reader.getName());
				}
				else if (reader.getKind() == ArxmlNodeKind::ElementEnd)
				{
					if (!stack.empty())
					{
						stack.pop_back();
					}
				}
			}
			std::cout << "--- COMPU-RATIONAL-COEFFS by enclosing elements" << std::endl;
			for (const std::pair<const std::string, uint64_t> & entry : chains)
			{
				std::cout << "  " << entry.second << "\t" << entry.first << std::endl;
			}
		}
		return result;
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
		std::cout << "--- model" << std::endl;
		std::cout << "  iSignals:       " << database._iSignals.size() << std::endl;
		std::cout << "  systemSignals:  " << database._systemSignals.size() << std::endl;
		std::cout << "  compuMethods:   " << database._compuMethods.size() << std::endl;
		std::cout << "  dataConstrs:    " << database._dataConstrs.size() << std::endl;
		std::cout << "  swBaseTypes:    " << database._swBaseTypes.size() << std::endl;
		std::cout << "  units:          " << database._units.size() << std::endl;
		std::cout << "  iSignalIPdus:   " << database._iSignalIPdus.size() << std::endl;
		std::cout << "  canFrames:      " << database._canFrames.size() << std::endl;
		std::cout << "  canFrameTrigs:  " << database._canFrameTriggerings.size() << std::endl;
		std::cout << "  canClusters:    " << database._canClusters.size() << std::endl;
		size_t mappings = 0;
		size_t mappingsWithSignal = 0;
		size_t mappingsWithGroup = 0;
		size_t mappingsWithStart = 0;
		size_t pdusWithCycleTime = 0;
		for (const Ar::ArISignalIPdu & pdu : database._iSignalIPdus)
		{
			if (pdu._cycleTime_s.has_value())
			{
				pdusWithCycleTime++;
			}
			for (const Ar::ArISignalToIPduMapping & mapping : pdu._mappings)
			{
				mappings++;
				if (!mapping._iSignalRef._path.empty())
				{
					mappingsWithSignal++;
				}
				if (!mapping._iSignalGroupRef._path.empty())
				{
					mappingsWithGroup++;
				}
				if (mapping._startPosition.has_value())
				{
					mappingsWithStart++;
				}
			}
		}
		std::cout << "  signalMappings: " << mappings
			<< " (signal " << mappingsWithSignal << ", group " << mappingsWithGroup
			<< ", withStart " << mappingsWithStart << ")" << std::endl;
		std::cout << "  pdusWithCycle:  " << pdusWithCycleTime << std::endl;
		size_t frameMappings = 0;
		for (const Ar::ArCanFrame & canFrame : database._canFrames)
		{
			frameMappings += canFrame._mappings.size();
		}
		std::cout << "  frameMappings:  " << frameMappings << std::endl;
		size_t extended = 0;
		size_t withIdentifier = 0;
		for (const Ar::ArCanFrameTriggering & triggering : database._canFrameTriggerings)
		{
			if (triggering._identifier.has_value())
			{
				withIdentifier++;
			}
			if (triggering._canAddressingMode == "EXTENDED")
			{
				extended++;
			}
		}
		std::cout << "  triggerings:    withIdentifier " << withIdentifier
			<< ", extended " << extended << std::endl;
		for (const Ar::ArCanCluster & canCluster : database._canClusters)
		{
			std::cout << "  cluster " << canCluster._shortName << ": baudrate ";
			if (canCluster._baudrate_bps.has_value())
			{
				std::cout << *canCluster._baudrate_bps;
			}
			if (canCluster._canFdBaudrate_bps.has_value())
			{
				std::cout << ", fd " << *canCluster._canFdBaudrate_bps;
			}
			std::cout << std::endl;
		}
		size_t scales = 0;
		size_t linearScales = 0;
		size_t textScales = 0;
		std::map<std::string, uint64_t> categories;
		for (const Ar::ArCompuMethod & compuMethod : database._compuMethods)
		{
			categories[compuMethod._category]++;
			for (const Ar::ArCompuScale & scale : compuMethod._scales)
			{
				scales++;
				if (!scale._numerator.empty())
				{
					linearScales++;
				}
				if (scale._textValue.has_value())
				{
					textScales++;
				}
			}
		}
		std::cout << "  compuScales:    " << scales
			<< " (linear " << linearScales << ", text " << textScales << ")" << std::endl;
		std::cout << "--- compuMethod categories" << std::endl;
		for (const std::pair<const std::string, uint64_t> & entry : categories)
		{
			std::cout << "  " << entry.second << "\t" << entry.first << std::endl;
		}
		size_t withoutCompuMethod = 0;
		size_t withoutDataConstr = 0;
		for (const Ar::ArSystemSignal & systemSignal : database._systemSignals)
		{
			if (systemSignal._compuMethodRef._path.empty())
			{
				withoutCompuMethod++;
			}
			if (systemSignal._dataConstrRef._path.empty())
			{
				withoutDataConstr++;
			}
		}
		size_t withoutSystemSignal = 0;
		size_t withoutBaseType = 0;
		for (const Ar::ArISignal & iSignal : database._iSignals)
		{
			if (iSignal._systemSignalRef._path.empty())
			{
				withoutSystemSignal++;
			}
			if (iSignal._baseTypeRef._path.empty())
			{
				withoutBaseType++;
			}
		}
		std::cout << "--- unresolvable references" << std::endl;
		std::cout << "  systemSignal without compuMethod: " << withoutCompuMethod << std::endl;
		std::cout << "  systemSignal without dataConstr:  " << withoutDataConstr << std::endl;
		std::cout << "  iSignal without systemSignal:     " << withoutSystemSignal << std::endl;
		std::cout << "  iSignal without baseType:         " << withoutBaseType << std::endl;
		for (size_t i = 0; i < database._duplicatePaths.size() && i < 10; i++)
		{
			std::cout << "    duplicate: " << database._duplicatePaths[i] << std::endl;
		}
		std::cout << "--- indexed elements by type" << std::endl;
		for (const std::pair<std::string, uint64_t> & entry : sorted)
		{
			if (100 <= entry.second)
			{
				std::cout << "  " << entry.second << "\t" << entry.first << std::endl;
			}
		}
		std::cout << "--- sample signals" << std::endl;
		size_t shown = 0;
		for (const Ar::ArISignal & iSignal : database._iSignals)
		{
			if (shown < 3)
			{
				std::cout << "  " << iSignal._path << ", length " << iSignal._length
					<< ", systemSignal " << iSignal._systemSignalRef._path
					<< ", baseType " << iSignal._baseTypeRef._path << std::endl;
				shown++;
			}
		}
		for (const Ar::ArSwBaseType & swBaseType : database._swBaseTypes)
		{
			std::cout << "  baseType " << swBaseType._shortName
				<< ", size " << swBaseType._baseTypeSize
				<< ", encoding '" << swBaseType._baseTypeEncoding << "'" << std::endl;
		}
		size_t withDescription = 0;
		for (const Ar::ArISignal & iSignal : database._iSignals)
		{
			if (iSignal._description.has_value())
			{
				withDescription++;
			}
		}
		std::cout << "  iSignals with description: " << withDescription << std::endl;
		size_t shownFrames = 0;
		for (const Ar::ArCanFrameTriggering & triggering : database._canFrameTriggerings)
		{
			if (shownFrames < 3 && triggering._identifier.has_value())
			{
				uint32_t dbcId = *triggering._identifier;
				if (triggering._canAddressingMode == "EXTENDED")
				{
					dbcId |= 0x80000000u;
				}
				std::cout << "  " << triggering._shortName
					<< ", id " << *triggering._identifier
					<< ", dbcId " << dbcId
					<< ", frame " << triggering._frameRef._path << std::endl;
				shownFrames++;
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
