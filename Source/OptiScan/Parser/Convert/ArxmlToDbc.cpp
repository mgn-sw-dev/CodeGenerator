
#include <OptiScan/Parser/Convert/ArxmlToDbc.h>
#include <set>
#include <utility>

using namespace OptiScan::Parser::Arxml;
using namespace OptiScan::Parser::Arxml::Ar;
using namespace OptiScan::Parser::Dbc;
using namespace std;

namespace OptiScan::Parser::Convert
{
	namespace
	{
		/** Encoding of a SW-BASE-TYPE that marks a signed value. */
		const string TwosComplementEncoding = "2C";
		const string MostSignificantByteFirst = "MOST-SIGNIFICANT-BYTE-FIRST";
	}

	ArxmlToDbc::ArxmlToDbc(const ArxmlDatabase & arxmlDatabase)
		: _arxmlDatabase(&arxmlDatabase)
		, _resolver(arxmlDatabase)
	{
	}

	bool ArxmlToDbc::convertCoefficients(const ArCompuMethod & compuMethod,
		double & factor, double & offset, vector<string> & warnings)
	{
		// AUTOSAR orders the coefficients ascending, so the numerator is
		// (offset, factor) and the denominator usually just (1).
		factor = 1.0;
		offset = 0.0;
		bool result = false;
		for (const ArCompuScale & scale : compuMethod._scales)
		{
			if (!result && !scale._numerator.empty())
			{
				double denominator = 1.0;
				if (!scale._denominator.empty())
				{
					denominator = scale._denominator[0];
				}
				if (denominator == 0.0)
				{
					warnings.push_back("COMPU-METHOD " + compuMethod._path + " has a zero denominator");
				}
				else
				{
					offset = scale._numerator[0] / denominator;
					if (1 < scale._numerator.size())
					{
						factor = scale._numerator[1] / denominator;
					}
					result = true;
				}
			}
		}
		return result;
	}

	void ArxmlToDbc::convertLimits(const ArDataConstr * dataConstr, double factor, double offset,
		DbcSignal & signal, ArxmlToDbcReport & report) const
	{
		if (dataConstr == nullptr)
		{
			report._limitsMissing++;
		}
		else if (dataConstr->_physLowerLimit.has_value() && dataConstr->_physUpperLimit.has_value())
		{
			// A DBC carries the physical range, so this one can be taken over as is.
			signal._minimum = *dataConstr->_physLowerLimit;
			signal._maximum = *dataConstr->_physUpperLimit;
			report._limitsFromPhys++;
		}
		else if (dataConstr->_internalLowerLimit.has_value() && dataConstr->_internalUpperLimit.has_value())
		{
			// Only the raw range is given, so it has to be scaled first.
			signal._minimum = *dataConstr->_internalLowerLimit * factor + offset;
			signal._maximum = *dataConstr->_internalUpperLimit * factor + offset;
			report._limitsFromInternal++;
		}
		else
		{
			report._limitsMissing++;
		}
	}

	void ArxmlToDbc::convertValueDescriptions(const ArCompuMethod & compuMethod,
		vector<DbcValueDescription> & descriptions)
	{
		for (const ArCompuScale & scale : compuMethod._scales)
		{
			if (scale._textValue.has_value() && scale._lowerLimit.has_value())
			{
				// A DBC names single values only. Where a scale covers a range, the
				// lower bound is used, which is what the ranges in these files are.
				DbcValueDescription description;
				description._description = *scale._textValue;
				description._value = *scale._lowerLimit;
				descriptions.push_back(std::move(description));
			}
		}
	}

	bool ArxmlToDbc::convertSignal(const ArISignalToIPduMapping & mapping, uint32_t pduStartPosition,
		DbcSignal & signal, vector<DbcValueDescription> & descriptions, ArxmlToDbcReport & report) const
	{
		bool result = false;
		if (!mapping._iSignalGroupRef._path.empty())
		{
			report._skippedGroupMappings++;
		}
		else
		{
			const ArISignal * const iSignal = this->_resolver.findISignal(mapping._iSignalRef);
			if (iSignal == nullptr)
			{
				report._warnings.push_back("mapping " + mapping._path + " has no I-SIGNAL");
			}
			else
			{
				signal._name = iSignal->_shortName;
				signal._size_bit = iSignal->_length;
				signal._isBigEndian = mapping._packingByteOrder == MostSignificantByteFirst;
				if (mapping._startPosition.has_value())
				{
					signal._startBit = *mapping._startPosition + pduStartPosition;
				}
				const ArSwBaseType * const baseType = this->_resolver.findSwBaseType(iSignal->_baseTypeRef);
				if (baseType != nullptr)
				{
					signal._isSigned = baseType->_baseTypeEncoding == TwosComplementEncoding;
				}

				double factor = 1.0;
				double offset = 0.0;
				const ArDataConstr * dataConstr = nullptr;
				const ArSystemSignal * const systemSignal =
					this->_resolver.findSystemSignal(iSignal->_systemSignalRef);
				if (systemSignal != nullptr)
				{
					dataConstr = this->_resolver.findDataConstr(systemSignal->_dataConstrRef);
					const ArCompuMethod * const compuMethod =
						this->_resolver.findCompuMethod(systemSignal->_compuMethodRef);
					if (compuMethod == nullptr)
					{
						report._signalsWithoutCompuMethod++;
					}
					else
					{
						ArxmlToDbc::convertCoefficients(*compuMethod, factor, offset, report._warnings);
						ArxmlToDbc::convertValueDescriptions(*compuMethod, descriptions);
						if (!descriptions.empty())
						{
							report._valueTableCount++;
						}
						const ArUnit * const unit = this->_resolver.findUnit(compuMethod->_unitRef);
						if (unit != nullptr)
						{
							signal._unit = unit->_displayName;
						}
					}
				}
				signal._factor = factor;
				signal._offset = offset;
				this->convertLimits(dataConstr, factor, offset, signal, report);
				result = true;
			}
		}
		return result;
	}

	void ArxmlToDbc::convert(DbcDatabase & dbcDatabase, ArxmlToDbcReport & report) const
	{
		dbcDatabase = DbcDatabase();
		report = ArxmlToDbcReport();
		if (!this->_arxmlDatabase->_canClusters.empty()
			&& this->_arxmlDatabase->_canClusters[0]._baudrate_bps.has_value())
		{
			dbcDatabase._bitTiming._baudrate = *this->_arxmlDatabase->_canClusters[0]._baudrate_bps;
		}
		set<string> usedPdus;

		for (const ArCanFrameTriggering & triggering : this->_arxmlDatabase->_canFrameTriggerings)
		{
			const ArCanFrame * const canFrame = this->_resolver.findCanFrame(triggering._frameRef);
			if (canFrame == nullptr)
			{
				report._warnings.push_back("triggering " + triggering._path + " has no CAN-FRAME");
			}
			else
			{
				DbcMessage message;
				message._name = canFrame->_shortName;
				message._size_byte = canFrame->_frameLength;
				if (triggering._identifier.has_value())
				{
					// A DBC marks an extended identifier by setting bit 31.
					if (triggering._canAddressingMode == "EXTENDED")
					{
						message.setExtendedCanId(*triggering._identifier);
					}
					else
					{
						message._id = *triggering._identifier;
					}
				}

				for (const ArPduToFrameMapping & pduMapping : canFrame->_mappings)
				{
					const ArISignalIPdu * const pdu = this->_resolver.findISignalIPdu(pduMapping._pduRef);
					if (pdu == nullptr)
					{
						// For example a DCM-I-PDU, a CONTAINER-I-PDU or an NM-PDU.
						// None of them carries application signals the way a DBC
						// message does.
						report._skippedPdus++;
						report._skippedPduTypes[pduMapping._pduRef._dest]++;
					}
					else
					{
						uint32_t pduStartPosition = 0;
						if (pduMapping._startPosition.has_value())
						{
							pduStartPosition = *pduMapping._startPosition;
						}
						for (const ArISignalToIPduMapping & signalMapping : pdu->_mappings)
						{
							DbcSignal signal;
							vector<DbcValueDescription> descriptions;
							if (this->convertSignal(signalMapping, pduStartPosition, signal,
								descriptions, report))
							{
								if (!descriptions.empty())
								{
									DbcSignalValueDescriptions valueDescriptions;
									valueDescriptions._descriptions = std::move(descriptions);
									valueDescriptions._messageId = message._id;
									valueDescriptions._signalName = signal._name;
									dbcDatabase._signalValueDescriptions.push_back(std::move(valueDescriptions));
								}
								message._signals.push_back(std::move(signal));
								report._signalCount++;
							}
						}
						usedPdus.insert(pdu->_path);
					}
				}
				report._messageCount++;
				dbcDatabase._messages.push_back(std::move(message));
			}
		}
		for (const ArISignalIPdu & pdu : this->_arxmlDatabase->_iSignalIPdus)
		{
			if (usedPdus.find(pdu._path) == usedPdus.end())
			{
				report._unreferencedPdus++;
			}
		}
	}

}
