#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/Parser/Arxml/ArxmlParser.h>
#include <OptiScan/Parser/Arxml/ArxmlResolver.h>
#include <OptiScan/Parser/Convert/ArxmlToDbc.h>
#include <OptiScan/View/Console/ConsoleLogHandler.h>
#include <chrono>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

using namespace OptiScan::Core::Database;
using namespace OptiScan::Parser::Arxml;
using namespace OptiScan::Parser::Convert;
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

	/** Check path for the ARXML parser (plan 20-parser-arxml.md, N5b).
	 *  Parses the file, resolves its references and translates it into the DBC world. */
	int runArxmlCheck()
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

		std::cout << "--- model" << std::endl;
		std::cout << "  indexed paths:  " << database._pathIndex.size()
			<< ", duplicates " << database._duplicatePaths.size() << std::endl;
		std::cout << "  iSignals:       " << database._iSignals.size() << std::endl;
		std::cout << "  systemSignals:  " << database._systemSignals.size() << std::endl;
		std::cout << "  compuMethods:   " << database._compuMethods.size() << std::endl;
		std::cout << "  dataConstrs:    " << database._dataConstrs.size() << std::endl;
		std::cout << "  units:          " << database._units.size() << std::endl;
		std::cout << "  iSignalIPdus:   " << database._iSignalIPdus.size() << std::endl;
		std::cout << "  canFrames:      " << database._canFrames.size() << std::endl;
		std::cout << "  canFrameTrigs:  " << database._canFrameTriggerings.size() << std::endl;
		std::cout << "  duration:       "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< " ms" << std::endl;

		ArxmlResolver resolver(database);
		ArxmlResolveReport resolveReport;
		resolver.validate(resolveReport);
		std::cout << "--- references" << std::endl;
		std::cout << "  total " << resolveReport._referenceCount
			<< ", resolved " << resolveReport._resolvedCount
			<< ", unresolved " << resolveReport._unresolved.size()
			<< ", type mismatch " << resolveReport._typeMismatches.size() << std::endl;

		ArxmlToDbc converter(database);
		OptiScan::Parser::Dbc::DbcDatabase dbcDatabase;
		ArxmlToDbcReport convertReport;
		converter.convert(dbcDatabase, convertReport);

		std::cout << "--- converted to the DBC world" << std::endl;
		std::cout << "  messages:       " << convertReport._messageCount << std::endl;
		std::cout << "  signals:        " << convertReport._signalCount << std::endl;
		std::cout << "  valueTables:    " << convertReport._valueTableCount << std::endl;
		std::cout << "  baudrate:       " << dbcDatabase._bitTiming._baudrate << std::endl;
		std::cout << "  skipped pdus:   " << convertReport._skippedPdus << std::endl;
		for (const std::pair<const std::string, uint64_t> & entry : convertReport._skippedPduTypes)
		{
			std::cout << "    " << entry.second << "\t" << entry.first << std::endl;
		}
		std::cout << "  pdus no frame:  " << convertReport._unreferencedPdus << std::endl;
		std::cout << "  skipped groups: " << convertReport._skippedGroupMappings << std::endl;
		std::cout << "  no compuMethod: " << convertReport._signalsWithoutCompuMethod << std::endl;
		std::cout << "  limits phys " << convertReport._limitsFromPhys
			<< ", internal " << convertReport._limitsFromInternal
			<< ", missing " << convertReport._limitsMissing << std::endl;
		std::cout << "  warnings:       " << convertReport._warnings.size() << std::endl;
		for (size_t i = 0; i < convertReport._warnings.size() && i < 5; i++)
		{
			std::cout << "    " << convertReport._warnings[i] << std::endl;
		}

		std::cout << "--- sample message" << std::endl;
		for (const OptiScan::Parser::Dbc::DbcMessage & message : dbcDatabase._messages)
		{
			if (message._name == "EBS21_TRLR_EBS")
			{
				std::cout << "  BO_ " << message._id << " " << message._name
					<< ": " << message._size_byte << std::endl;
				for (const OptiScan::Parser::Dbc::DbcSignal & signal : message._signals)
				{
					std::cout << "   SG_ " << signal._name << " : " << signal._startBit
						<< "|" << signal._size_bit;
					if (signal._isBigEndian)
					{
						std::cout << "@0";
					}
					else
					{
						std::cout << "@1";
					}
					if (signal._isSigned)
					{
						std::cout << "-";
					}
					else
					{
						std::cout << "+";
					}
					std::cout << " (" << signal._factor << "," << signal._offset << ")"
						<< " [" << signal._minimum << "|" << signal._maximum << "]"
						<< " \"" << signal._unit << "\"" << std::endl;
				}
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
		result = runArxmlCheck();
	}
	else
	{
		result = runCodeGenerator();
	}
	return result;
}
