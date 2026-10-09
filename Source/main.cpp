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
