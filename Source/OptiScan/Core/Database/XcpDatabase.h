#pragma once

#include <OptiScan/Parser/A2l/A2lDatabase.h>
#include <filesystem>
#include <string>

namespace OptiScan::Core::Database
{
	class XcpDatabase
	{
	public:
		Parser::A2l::A2lDatabase _a2lDatabase;
		std::string _name;

		XcpDatabase();
		/** */
		void clear();
		/** @throw runtime_error if a2lPath does not exist.
		 *  @throw runtime_error if a2lPath could not be opened. */
		void loadA2lDatabase(const std::filesystem::path & a2lPath);
	};
}
