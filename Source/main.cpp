#include <OptiScan/Parser/A2l/A2lParser.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	/** Temporary check path for the A2L parser (plan 11-m1-arbeitspakete.md, AP 1.10).
	 *  To be replaced by the parser test project once P7 is in place. */
	bool parseCheck(const std::filesystem::path & path)
	{
		using namespace OptiScan::Parser::A2l;

		std::cout << "=== " << path.filename().string() << std::endl;
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "  file not found" << std::endl;
			return false;
		}

		A2lParser parser(&file);
		A2lDatabase database;
		const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		try
		{
			parser.parse(database);
		}
		catch (const std::exception & error)
		{
			std::cout << "  FAILED: " << error.what() << std::endl;
			return false;
		}
		const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		const int64_t duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		std::cout << "  asap2Version:  " << database._asap2Version._versionNo
			<< "." << database._asap2Version._upgradeNo << std::endl;
		std::cout << "  a2mlVersion:   "
			<< (database._a2mlVersion.has_value() ? "yes" : "no") << std::endl;
		std::cout << "  project:       " << database._project._name.toString() << std::endl;
		std::cout << "  header:        "
			<< (database._project._header.has_value() ? "yes" : "no") << std::endl;
		std::cout << "  modules:       " << database._project._modules.size() << std::endl;
		for (const Mcd::McdModule & module : database._project._modules)
		{
			std::cout << "    - " << module._name.toString() << std::endl;
			std::cout << "      modCommon comment:  \"" << module._common._comment << "\"" << std::endl;
			std::cout << "      modPar epk:         \"" << module._parameters._epk << "\"" << std::endl;
			std::cout << "      modPar addrEpks:    " << module._parameters._addrEpks.size() << std::endl;
			std::cout << "      modPar sysConsts:   " << module._parameters._systemConstants.size() << std::endl;
			std::cout << "      compuMethods:       " << module._compuMethods.size() << std::endl;
			std::cout << "      compuTabs:          " << module._compuTabs.size() << std::endl;
			std::cout << "      compuVTabs:         " << module._compuVTabs.size() << std::endl;
			std::cout << "      compuVTabRanges:    " << module._compuVTabRanges.size() << std::endl;
			std::cout << "      measurements:       " << module._measurements.size() << std::endl;
			size_t withAddress = 0;
			size_t withAddressExtension = 0;
			size_t withMatrixDim = 0;
			size_t writeable = 0;
			size_t withByteOrder = 0;
			size_t withSymbolLink = 0;
			size_t withMaxRefresh = 0;
			size_t withModelLink = 0;
			for (const Mcd::McdMeasurement & measurement : module._measurements)
			{
				withAddress += measurement._ecuAddress.has_value() ? 1 : 0;
				withAddressExtension += measurement._ecuAddressExtension.has_value() ? 1 : 0;
				withMatrixDim += measurement._matrixDim.empty() ? 0 : 1;
				writeable += measurement._isWriteable ? 1 : 0;
				withByteOrder += measurement._byteOrder == Mcd::McdByteOrder::None ? 0 : 1;
				withSymbolLink += measurement._symbolLink.has_value() ? 1 : 0;
				withMaxRefresh += measurement._maxRefresh.has_value() ? 1 : 0;
				withModelLink += measurement._modelLink.has_value() ? 1 : 0;
			}
			std::cout << "        ecuAddress:       " << withAddress << std::endl;
			std::cout << "        ecuAddressExt:    " << withAddressExtension << std::endl;
			std::cout << "        byteOrder:        " << withByteOrder << std::endl;
			std::cout << "        matrixDim:        " << withMatrixDim << std::endl;
			std::cout << "        readWrite:        " << writeable << std::endl;
			std::cout << "        symbolLink:       " << withSymbolLink << std::endl;
			std::cout << "        maxRefresh:       " << withMaxRefresh << std::endl;
			std::cout << "        modelLink:        " << withModelLink << std::endl;
			if (!module._measurements.empty())
			{
				const Mcd::McdMeasurement & first = module._measurements.front();
				std::cout << "        first:            " << first._name.toString()
					<< ", dataType " << static_cast<int>(first._dataType)
					<< ", conversion " << first._conversion.toString();
				if (first._ecuAddress.has_value())
				{
					std::cout << std::hex << std::showbase << ", ecuAddress " << *first._ecuAddress
						<< std::dec << std::noshowbase;
				}
				std::cout << std::endl;
			}
		}
		std::cout << "  duration:      " << duration_ms << " ms" << std::endl;
		return true;
	}
}

int main(int argc, char * argv[])
{
	std::vector<std::filesystem::path> paths;
	for (int i = 1; i < argc; i++)
	{
		paths.emplace_back(argv[i]);
	}
	if (paths.empty())
	{
		paths.emplace_back("C:/CodeGenerator/Test/ACM_FS_DTA30_0d1_S10_52_01_000.a2l");
		paths.emplace_back("C:/CodeGenerator/Test/MCM2_M19.12.0.3_CLASSIC_EURO_ROM_xcp_measurementsystem.a2l");
	}

	bool succeeded = true;
	for (const std::filesystem::path & path : paths)
	{
		succeeded = parseCheck(path) && succeeded;
	}
	std::cout << (succeeded ? "all files parsed" : "at least one file failed") << std::endl;
	return succeeded ? 0 : 1;
}
