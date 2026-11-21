
#include <OptiScan/Parser/Arxml/ArxmlKeyword.h>
#include <OptiScan/Parser/Arxml/ArxmlResolver.h>

using namespace OptiScan::Parser::Arxml::Ar;
using namespace std;

namespace OptiScan::Parser::Arxml
{
	ArxmlResolver::ArxmlResolver(const ArxmlDatabase & arxmlDatabase)
		: _arxmlDatabase(&arxmlDatabase)
	{
	}

	void ArxmlResolver::check(const ArRef & reference, const string & source,
		ArxmlResolveReport & report) const
	{
		// An absent reference is not an issue; many of them are optional.
		if (!reference._path.empty())
		{
			report._referenceCount++;
			map<string, ArxmlIdentifiable>::const_iterator const entry =
				this->_arxmlDatabase->_pathIndex.find(reference._path);
			if (entry == this->_arxmlDatabase->_pathIndex.end())
			{
				ArxmlResolveIssue issue;
				issue._dest = reference._dest;
				issue._path = reference._path;
				issue._source = source;
				report._unresolved.push_back(std::move(issue));
			}
			else if (!reference._dest.empty() && entry->second._elementType != reference._dest)
			{
				ArxmlResolveIssue issue;
				issue._dest = reference._dest;
				issue._elementType = entry->second._elementType;
				issue._path = reference._path;
				issue._source = source;
				report._typeMismatches.push_back(std::move(issue));
			}
			else
			{
				report._resolvedCount++;
			}
		}
	}

	const ArxmlIdentifiable * ArxmlResolver::find(const ArRef & reference,
		const string & elementType) const
	{
		const ArxmlIdentifiable * result = nullptr;
		map<string, ArxmlIdentifiable>::const_iterator const entry =
			this->_arxmlDatabase->_pathIndex.find(reference._path);
		if (entry != this->_arxmlDatabase->_pathIndex.end()
			&& entry->second._elementType == elementType)
		{
			result = &entry->second;
		}
		return result;
	}

	const ArCanFrame * ArxmlResolver::findCanFrame(const ArRef & reference) const
	{
		const ArCanFrame * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::CanFrame);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_canFrames.size())
		{
			result = &this->_arxmlDatabase->_canFrames[entry->_elementIndex];
		}
		return result;
	}

	const ArCompuMethod * ArxmlResolver::findCompuMethod(const ArRef & reference) const
	{
		const ArCompuMethod * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::CompuMethod);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_compuMethods.size())
		{
			result = &this->_arxmlDatabase->_compuMethods[entry->_elementIndex];
		}
		return result;
	}

	const ArDataConstr * ArxmlResolver::findDataConstr(const ArRef & reference) const
	{
		const ArDataConstr * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::DataConstr);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_dataConstrs.size())
		{
			result = &this->_arxmlDatabase->_dataConstrs[entry->_elementIndex];
		}
		return result;
	}

	const ArISignal * ArxmlResolver::findISignal(const ArRef & reference) const
	{
		const ArISignal * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::ISignal);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_iSignals.size())
		{
			result = &this->_arxmlDatabase->_iSignals[entry->_elementIndex];
		}
		return result;
	}

	const ArISignalIPdu * ArxmlResolver::findISignalIPdu(const ArRef & reference) const
	{
		const ArISignalIPdu * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::ISignalIPdu);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_iSignalIPdus.size())
		{
			result = &this->_arxmlDatabase->_iSignalIPdus[entry->_elementIndex];
		}
		return result;
	}

	const ArSwBaseType * ArxmlResolver::findSwBaseType(const ArRef & reference) const
	{
		const ArSwBaseType * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::SwBaseType);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_swBaseTypes.size())
		{
			result = &this->_arxmlDatabase->_swBaseTypes[entry->_elementIndex];
		}
		return result;
	}

	const ArSystemSignal * ArxmlResolver::findSystemSignal(const ArRef & reference) const
	{
		const ArSystemSignal * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::SystemSignal);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_systemSignals.size())
		{
			result = &this->_arxmlDatabase->_systemSignals[entry->_elementIndex];
		}
		return result;
	}

	const ArUnit * ArxmlResolver::findUnit(const ArRef & reference) const
	{
		const ArUnit * result = nullptr;
		const ArxmlIdentifiable * const entry = this->find(reference, ArxmlKeyword::Unit);
		if (entry != nullptr && entry->_elementIndex < this->_arxmlDatabase->_units.size())
		{
			result = &this->_arxmlDatabase->_units[entry->_elementIndex];
		}
		return result;
	}

	void ArxmlResolver::validate(ArxmlResolveReport & report) const
	{
		report = ArxmlResolveReport();
		for (const ArCanFrameTriggering & triggering : this->_arxmlDatabase->_canFrameTriggerings)
		{
			this->check(triggering._frameRef, triggering._path, report);
		}
		for (const ArCanFrame & canFrame : this->_arxmlDatabase->_canFrames)
		{
			for (const ArPduToFrameMapping & mapping : canFrame._mappings)
			{
				this->check(mapping._pduRef, mapping._path, report);
			}
		}
		for (const ArCompuMethod & compuMethod : this->_arxmlDatabase->_compuMethods)
		{
			this->check(compuMethod._unitRef, compuMethod._path, report);
		}
		for (const ArISignal & iSignal : this->_arxmlDatabase->_iSignals)
		{
			this->check(iSignal._baseTypeRef, iSignal._path, report);
			this->check(iSignal._systemSignalRef, iSignal._path, report);
		}
		for (const ArISignalIPdu & iSignalIPdu : this->_arxmlDatabase->_iSignalIPdus)
		{
			for (const ArISignalToIPduMapping & mapping : iSignalIPdu._mappings)
			{
				this->check(mapping._iSignalGroupRef, mapping._path, report);
				this->check(mapping._iSignalRef, mapping._path, report);
			}
		}
		for (const ArSystemSignal & systemSignal : this->_arxmlDatabase->_systemSignals)
		{
			this->check(systemSignal._compuMethodRef, systemSignal._path, report);
			this->check(systemSignal._dataConstrRef, systemSignal._path, report);
		}
	}

}
