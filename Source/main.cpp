#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/Parser/Arxml/ArxmlParser.h>
#include <OptiScan/Parser/Arxml/ArxmlResolver.h>
#include <OptiScan/Parser/Convert/ArxmlToDbc.h>
#include <OptiScan/Parser/Convert/DbcComparison.h>
#include <OptiScan/Parser/Dbc/DbcParser.h>
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
	const std::string DbcPath = "C:/CodeGenerator/Test/BACKBONE_EXT_CAN_2025_31a.dbc";
	const std::string ConfigPath = "C:/CodeGenerator/Test/ConfigTest.json";

	/** Prints the raw ARXML values behind a signal, so a limit difference against the
	 *  DBC can be attributed. Temporary. */
	void dumpSignalLimits(const ArxmlDatabase & database, const ArxmlResolver & resolver,
		const std::string & signalName)
	{
		for (const Ar::ArISignal & iSignal : database._iSignals)
		{
			if (iSignal._shortName == signalName)
			{
				std::cout << "    " << signalName << ":";
				const Ar::ArSystemSignal * const systemSignal =
					resolver.findSystemSignal(iSignal._systemSignalRef);
				if (systemSignal == nullptr)
				{
					std::cout << " no system signal" << std::endl;
				}
				else
				{
					const Ar::ArDataConstr * const dataConstr =
						resolver.findDataConstr(systemSignal->_dataConstrRef);
					if (dataConstr == nullptr)
					{
						std::cout << " no data constraint";
					}
					else
					{
						std::cout << " " << dataConstr->_path << " internal [";
						if (dataConstr->_internalLowerLimit.has_value())
						{
							std::cout << *dataConstr->_internalLowerLimit;
						}
						std::cout << "|";
						if (dataConstr->_internalUpperLimit.has_value())
						{
							std::cout << *dataConstr->_internalUpperLimit;
						}
						std::cout << "] phys [";
						if (dataConstr->_physLowerLimit.has_value())
						{
							std::cout << *dataConstr->_physLowerLimit;
						}
						std::cout << "|";
						if (dataConstr->_physUpperLimit.has_value())
						{
							std::cout << *dataConstr->_physUpperLimit;
						}
						std::cout << "]";
					}
					const Ar::ArCompuMethod * const compuMethod =
						resolver.findCompuMethod(systemSignal->_compuMethodRef);
					if (compuMethod != nullptr)
					{
						std::cout << ", " << compuMethod->_category
							<< ", scales " << compuMethod->_scales.size();
					}
					std::cout << std::endl;
				}
			}
		}
	}

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
		// Hold the result against the DBC that another tool generated from the same
		// network. Neither side is the truth: the DBC is itself the output of a
		// conversion, so a difference says the two disagree, not who is right.
		std::ifstream dbcFile(DbcPath, std::ios::binary);
		if (!dbcFile.is_open())
		{
			std::cerr << "dbc not found" << std::endl;
			return 1;
		}
		OptiScan::Parser::Dbc::DbcParser dbcParser(&dbcFile);
		OptiScan::Parser::Dbc::DbcDatabase referenceDatabase;
		try
		{
			dbcParser.parse(referenceDatabase);
		}
		catch (const std::exception & error)
		{
			std::cerr << "FAILED to read the dbc: " << error.what() << std::endl;
			return 1;
		}

		DbcComparison comparison;
		DbcComparisonReport comparisonReport;
		comparison.compare(referenceDatabase, dbcDatabase, comparisonReport);

		std::cout << "--- compared against " << DbcPath << std::endl;
		std::cout << "  dbc messages " << referenceDatabase._messages.size()
			<< ", converted " << dbcDatabase._messages.size() << std::endl;
		std::cout << "  messages matched " << comparisonReport._messagesMatched
			<< ", only in dbc " << comparisonReport._messagesOnlyInReference
			<< ", only in arxml " << comparisonReport._messagesOnlyInCandidate << std::endl;
		std::cout << "  signals matched  " << comparisonReport._signalsMatched
			<< ", only in dbc " << comparisonReport._signalsOnlyInReference
			<< ", only in arxml " << comparisonReport._signalsOnlyInCandidate << std::endl;
		std::cout << "  converted messages without signals: "
			<< comparisonReport._candidateMessagesWithoutSignals << std::endl;
		std::cout << "  of the dbc only signals, in such a message: "
			<< comparisonReport._signalsOnlyInReferenceInEmptyMessages << std::endl;
		std::cout << "  units equal after encoding: "
			<< comparisonReport._unitsEqualAfterEncoding << std::endl;
		std::cout << "  attribute differences:" << std::endl;
		for (const std::pair<const std::string, uint64_t> & entry : comparisonReport._differencesByAttribute)
		{
			std::cout << "    " << entry.second << "\t" << entry.first << std::endl;
		}
		std::cout << "  examples:" << std::endl;
		size_t shownDifferences = 0;
		for (const DbcDifference & difference : comparisonReport._differences)
		{
			if (shownDifferences < 12 && !difference._attribute.empty())
			{
				std::cout << "    " << difference._messageName << "." << difference._signalName
					<< " " << difference._attribute
					<< ": dbc '" << difference._expected
					<< "', arxml '" << difference._found << "'" << std::endl;
				shownDifferences++;
			}
		}
		std::cout << "  raw arxml values behind the first limit differences:" << std::endl;
		size_t shownLimits = 0;
		for (const DbcDifference & difference : comparisonReport._differences)
		{
			bool const isLimit = difference._attribute == "minimum"
				|| difference._attribute == "maximum";
			if (shownLimits < 6 && isLimit)
			{
				dumpSignalLimits(database, resolver, difference._signalName);
				shownLimits++;
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
