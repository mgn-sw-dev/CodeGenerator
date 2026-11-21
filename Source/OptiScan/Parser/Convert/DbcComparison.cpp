
#include <OptiScan/Parser/Convert/DbcComparison.h>
#include <cmath>
#include <sstream>
#include <utility>

using namespace OptiScan::Parser::Dbc;
using namespace std;

namespace OptiScan::Parser::Convert
{
	namespace
	{
		/** Numbers are compared relatively, with an absolute floor for values near zero. */
		const double RelativeTolerance = 1e-9;
		const double AbsoluteTolerance = 1e-12;

		string toText(double value)
		{
			ostringstream stream;
			stream << value;
			return stream.str();
		}

		string toText(uint32_t value)
		{
			ostringstream stream;
			stream << value;
			return stream.str();
		}

		string toText(bool value)
		{
			string result = "false";
			if (value)
			{
				result = "true";
			}
			return result;
		}
	}

	void DbcComparison::addDifference(DbcDifference && difference, DbcComparisonReport & report)
	{
		if (!difference._attribute.empty())
		{
			report._differencesByAttribute[difference._attribute]++;
		}
		report._differences.push_back(std::move(difference));
	}

	void DbcComparison::compare(const DbcDatabase & reference, const DbcDatabase & candidate,
		DbcComparisonReport & report) const
	{
		report = DbcComparisonReport();
		map<uint32_t, const DbcMessage *> candidateMessages;
		for (const DbcMessage & message : candidate._messages)
		{
			candidateMessages[message._id] = &message;
		}
		map<uint32_t, const DbcMessage *> referenceMessages;
		for (const DbcMessage & message : reference._messages)
		{
			referenceMessages[message._id] = &message;
		}

		for (const DbcMessage & message : reference._messages)
		{
			map<uint32_t, const DbcMessage *>::const_iterator const entry =
				candidateMessages.find(message._id);
			if (entry == candidateMessages.end())
			{
				report._messagesOnlyInReference++;
				DbcDifference difference;
				difference._kind = DbcDifferenceKind::MessageMissing;
				difference._messageName = message._name;
				difference._expected = toText(message._id);
				DbcComparison::addDifference(std::move(difference), report);
			}
			else
			{
				report._messagesMatched++;
				this->compareMessage(message, *entry->second, report);
			}
		}
		for (const DbcMessage & message : candidate._messages)
		{
			if (referenceMessages.find(message._id) == referenceMessages.end())
			{
				report._messagesOnlyInCandidate++;
				DbcDifference difference;
				difference._kind = DbcDifferenceKind::MessageAdded;
				difference._messageName = message._name;
				difference._found = toText(message._id);
				DbcComparison::addDifference(std::move(difference), report);
			}
		}
	}

	void DbcComparison::compareMessage(const DbcMessage & reference, const DbcMessage & candidate,
		DbcComparisonReport & report) const
	{
		if (reference._name != candidate._name)
		{
			DbcDifference difference;
			difference._attribute = "message name";
			difference._expected = reference._name;
			difference._found = candidate._name;
			difference._kind = DbcDifferenceKind::MessageAttribute;
			difference._messageName = reference._name;
			DbcComparison::addDifference(std::move(difference), report);
		}
		if (reference._size_byte != candidate._size_byte)
		{
			DbcDifference difference;
			difference._attribute = "dlc";
			difference._expected = toText(reference._size_byte);
			difference._found = toText(candidate._size_byte);
			difference._kind = DbcDifferenceKind::MessageAttribute;
			difference._messageName = reference._name;
			DbcComparison::addDifference(std::move(difference), report);
		}

		if (candidate._signals.empty())
		{
			report._candidateMessagesWithoutSignals++;
		}

		map<string, const DbcSignal *> candidateSignals;
		for (const DbcSignal & signal : candidate._signals)
		{
			candidateSignals[signal._name] = &signal;
		}
		map<string, const DbcSignal *> referenceSignals;
		for (const DbcSignal & signal : reference._signals)
		{
			referenceSignals[signal._name] = &signal;
		}

		for (const DbcSignal & signal : reference._signals)
		{
			map<string, const DbcSignal *>::const_iterator const entry =
				candidateSignals.find(signal._name);
			if (entry == candidateSignals.end())
			{
				report._signalsOnlyInReference++;
				if (candidate._signals.empty())
				{
					// The whole message came out empty, so its PDU was skipped rather
					// than a single signal having gone missing.
					report._signalsOnlyInReferenceInEmptyMessages++;
				}
				DbcDifference difference;
				difference._kind = DbcDifferenceKind::SignalMissing;
				difference._messageName = reference._name;
				difference._signalName = signal._name;
				DbcComparison::addDifference(std::move(difference), report);
			}
			else
			{
				report._signalsMatched++;
				this->compareSignal(reference._name, signal, *entry->second, report);
			}
		}
		for (const DbcSignal & signal : candidate._signals)
		{
			if (referenceSignals.find(signal._name) == referenceSignals.end())
			{
				report._signalsOnlyInCandidate++;
				DbcDifference difference;
				difference._kind = DbcDifferenceKind::SignalAdded;
				difference._messageName = reference._name;
				difference._signalName = signal._name;
				DbcComparison::addDifference(std::move(difference), report);
			}
		}
	}

	void DbcComparison::compareSignal(const string & messageName, const DbcSignal & reference,
		const DbcSignal & candidate, DbcComparisonReport & report) const
	{
		DbcDifference difference;
		difference._kind = DbcDifferenceKind::SignalAttribute;
		difference._messageName = messageName;
		difference._signalName = reference._name;

		if (reference._startBit != candidate._startBit)
		{
			difference._attribute = "startBit";
			difference._expected = toText(reference._startBit);
			difference._found = toText(candidate._startBit);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (reference._size_bit != candidate._size_bit)
		{
			difference._attribute = "length";
			difference._expected = toText(reference._size_bit);
			difference._found = toText(candidate._size_bit);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (reference._isBigEndian != candidate._isBigEndian)
		{
			difference._attribute = "byteOrder";
			difference._expected = toText(reference._isBigEndian);
			difference._found = toText(candidate._isBigEndian);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (reference._isSigned != candidate._isSigned)
		{
			difference._attribute = "signed";
			difference._expected = toText(reference._isSigned);
			difference._found = toText(candidate._isSigned);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (!DbcComparison::equalNumbers(reference._factor, candidate._factor))
		{
			difference._attribute = "factor";
			difference._expected = toText(reference._factor);
			difference._found = toText(candidate._factor);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (!DbcComparison::equalNumbers(reference._offset, candidate._offset))
		{
			difference._attribute = "offset";
			difference._expected = toText(reference._offset);
			difference._found = toText(candidate._offset);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (!DbcComparison::equalNumbers(reference._minimum, candidate._minimum))
		{
			difference._attribute = "minimum";
			difference._expected = toText(reference._minimum);
			difference._found = toText(candidate._minimum);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (!DbcComparison::equalNumbers(reference._maximum, candidate._maximum))
		{
			difference._attribute = "maximum";
			difference._expected = toText(reference._maximum);
			difference._found = toText(candidate._maximum);
			DbcComparison::addDifference(DbcDifference(difference), report);
		}
		if (reference._unit != candidate._unit)
		{
			if (DbcComparison::latin1ToUtf8(reference._unit) == candidate._unit)
			{
				// The same text, only written in another encoding.
				report._unitsEqualAfterEncoding++;
			}
			else
			{
				difference._attribute = "unit";
				difference._expected = reference._unit;
				difference._found = candidate._unit;
				DbcComparison::addDifference(DbcDifference(difference), report);
			}
		}
	}

	bool DbcComparison::equalNumbers(double left, double right)
	{
		double const difference = fabs(left - right);
		double scale = fabs(left);
		if (scale < fabs(right))
		{
			scale = fabs(right);
		}
		return difference <= AbsoluteTolerance || difference <= scale * RelativeTolerance;
	}

	string DbcComparison::latin1ToUtf8(const string & value)
	{
		string result;
		result.reserve(value.size());
		for (size_t i = 0; i < value.size(); i++)
		{
			unsigned char const character = static_cast<unsigned char>(value[i]);
			if (character < 0x80)
			{
				result += static_cast<char>(character);
			}
			else
			{
				result += static_cast<char>(0xC0 | (character >> 6));
				result += static_cast<char>(0x80 | (character & 0x3F));
			}
		}
		return result;
	}

}
