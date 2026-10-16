
#include <OptiScan/Parser/A2l/A2lKeyword.h>
#include <OptiScan/Parser/A2l/A2lParser.h>
#include <OptiScan/Parser/FileException.h>
#include <OptiScan/Parser/TokenReaderUtils.h>
#include <unordered_set>
#include <utility>

using namespace OptiScan::Parser::A2l::Mcd;
using namespace std;

namespace OptiScan::Parser::A2l
{
	A2lParser::A2lParser(std::istream * input)
		: _options()
		, _reader(input)
	{
	}

	void A2lParser::getOptions(A2lOptions & options) const
	{
		options = this->_options;
	}

	void A2lParser::parse(A2lDatabase & a2lDatabase)
	{
		a2lDatabase = A2lDatabase();
		this->_reader.setCompatStyles(this->_options._compatStyles);
		this->_reader.readNextToken();
		this->parseAsap2Version(a2lDatabase._asap2Version);
		this->parseA2mlVersion(a2lDatabase._a2mlVersion);
		this->parseProject(a2lDatabase._project);
		this->_reader.matchToken(A2lTokenKind::None);
	}

	void A2lParser::parseA2mlVersion(optional<McdVersion> & version)
	{
		if (this->_reader.tryMatchKeyword(A2lKeyword::A2mlVersion))
		{
			this->_reader.readNextToken();
			version.emplace();
			this->parseVersion(*version);
		}
	}

	void A2lParser::parseAsap2Version(McdVersion & version)
	{
		this->_reader.matchKeyword(A2lKeyword::Asap2Version);
		this->_reader.readNextToken();
		this->parseVersion(version);
	}

	void A2lParser::parseCompuMethod(McdModule & module)
	{
		McdCompuMethod method;
		this->_reader.parseBlockBegin(A2lKeyword::CompuMethod);
		this->parseIdent(method._name);
		this->_reader.parseString(method._longIdentifier);
		this->_reader.parseEnumValue(McdEnumText::ConversionType, "conversion type", method._conversionType);
		this->_reader.parseString(method._format);
		this->_reader.parseString(method._physicalUnit);
		unordered_set<string> oneTimeKeywords;
		while (!this->_reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			if (this->_reader.tryMatchToken(A2lTokenKind::Identifier))
			{
				string const keyword = this->_reader.token()._text;
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->_reader.readNextToken();
				if (keyword == A2lKeyword::Coeffs)
				{
					McdCoeffs tmp;
					this->_reader.parseFloat64(tmp._a);
					this->_reader.parseFloat64(tmp._b);
					this->_reader.parseFloat64(tmp._c);
					this->_reader.parseFloat64(tmp._d);
					this->_reader.parseFloat64(tmp._e);
					this->_reader.parseFloat64(tmp._f);
					method._coeffs = tmp;
				}
				else if (keyword == A2lKeyword::CoeffsLinear)
				{
					McdCoeffsLinear tmp;
					this->_reader.parseFloat64(tmp._a);
					this->_reader.parseFloat64(tmp._b);
					method._coeffsLinear = tmp;
				}
				else if (keyword == A2lKeyword::CompuTabRef)
				{
					this->parseIdent(method._compuTabRef);
				}
				else if (keyword == A2lKeyword::RefUnit)
				{
					this->parseIdent(method._refUnit);
				}
				else if (keyword == A2lKeyword::StatusStringRef)
				{
					this->parseIdent(method._statusStringRef);
				}
				else
				{
					this->_reader.throwFormatError("A2lParser::parseCompuMethod", "Unknown keyword '" + keyword + "'");
				}
			}
			else
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, A2lKeyword::Formula);
				this->parseFormula(method._formula);
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::CompuMethod);
		module._compuMethods.push_back(std::move(method));
	}

	void A2lParser::parseCompuTab(McdModule & module)
	{
		McdCompuTab tab;
		this->_reader.parseBlockBegin(A2lKeyword::CompuTab);
		this->parseIdent(tab._name);
		this->_reader.parseString(tab._longIdentifier);
		this->_reader.parseEnumValue(McdEnumText::ConversionType, "conversion type", tab._conversionType);
		uint16_t count = 0;
		this->_reader.parseUInt16(count);
		tab._valuePairs.reserve(count);
		for (uint16_t i = 0; i < count; i++)
		{
			tuple<double, double> pair;
			this->_reader.parseFloat64(get<0>(pair));
			this->_reader.parseFloat64(get<1>(pair));
			tab._valuePairs.push_back(pair);
		}
		if (this->_reader.tryMatchKeyword(A2lKeyword::DefaultValue))
		{
			this->_reader.readNextToken();
			string tmp;
			this->_reader.parseString(tmp);
			tab._defaultValue = tmp;
		}
		else if (this->_reader.tryMatchKeyword(A2lKeyword::DefaultValueNumeric))
		{
			this->_reader.readNextToken();
			double tmp = 0.0;
			this->_reader.parseFloat64(tmp);
			tab._defaultValue = tmp;
		}
		this->_reader.parseBlockEnd(A2lKeyword::CompuTab);
		module._compuTabs.push_back(std::move(tab));
	}

	void A2lParser::parseCompuVTab(McdModule & module)
	{
		McdCompuVTab tab;
		this->_reader.parseBlockBegin(A2lKeyword::CompuVTab);
		this->parseIdent(tab._name);
		this->_reader.parseString(tab._longIdentifier);
		this->_reader.parseEnumValue(McdEnumText::ConversionType, "conversion type", tab._conversionType);
		uint16_t count = 0;
		this->_reader.parseUInt16(count);
		tab._valuePairs.reserve(count);
		for (uint16_t i = 0; i < count; i++)
		{
			tuple<double, string> pair;
			this->_reader.parseFloat64(get<0>(pair));
			this->_reader.parseString(get<1>(pair));
			tab._valuePairs.push_back(pair);
		}
		if (this->_reader.tryMatchKeyword(A2lKeyword::DefaultValue))
		{
			this->_reader.readNextToken();
			this->_reader.parseString(tab._defaultValue);
		}
		this->_reader.parseBlockEnd(A2lKeyword::CompuVTab);
		module._compuVTabs.push_back(std::move(tab));
	}

	void A2lParser::parseCompuVTabRange(McdModule & module)
	{
		McdCompuVTabRange tab;
		this->_reader.parseBlockBegin(A2lKeyword::CompuVTabRange);
		this->parseIdent(tab._name);
		this->_reader.parseString(tab._longIdentifier);
		uint16_t count = 0;
		this->_reader.parseUInt16(count);
		tab._valueTriples.reserve(count);
		for (uint16_t i = 0; i < count; i++)
		{
			tuple<double, double, string> triple;
			this->_reader.parseFloat64(get<0>(triple));
			this->_reader.parseFloat64(get<1>(triple));
			this->_reader.parseString(get<2>(triple));
			tab._valueTriples.push_back(triple);
		}
		if (this->_reader.tryMatchKeyword(A2lKeyword::DefaultValue))
		{
			this->_reader.readNextToken();
			this->_reader.parseString(tab._defaultValue);
		}
		this->_reader.parseBlockEnd(A2lKeyword::CompuVTabRange);
		module._compuVTabRanges.push_back(std::move(tab));
	}

	void A2lParser::parseFormula(optional<McdFormula> & formula)
	{
		McdFormula tmp;
		this->_reader.parseBlockBegin(A2lKeyword::Formula);
		this->_reader.parseString(tmp._f);
		if (this->_reader.tryMatchToken(A2lTokenKind::StringLiteral))
		{
			this->_reader.parseString(tmp._g);
		}
		this->_reader.parseBlockEnd(A2lKeyword::Formula);
		formula = std::move(tmp);
	}

	void A2lParser::parseHeader(McdHeader & header)
	{
		this->_reader.parseBlockBegin(A2lKeyword::Header);
		this->_reader.parseString(header._comment);
		unordered_set<string> oneTimeKeywords;
		while (!this->_reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			this->_reader.matchToken(A2lTokenKind::Identifier);
			string const keyword = this->_reader.token()._text;
			A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
			this->_reader.readNextToken();
			if (keyword == A2lKeyword::ProjectNo)
			{
				this->parseIdent(header._projectNo);
			}
			else if (keyword == A2lKeyword::Version)
			{
				this->_reader.parseString(header._version);
			}
			else
			{
				this->_reader.throwFormatError("A2lParser::parseHeader", "Unknown keyword '" + keyword + "'");
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::Header);
	}

	void A2lParser::parseIdent(McdIdent & ident)
	{
		bool checkNext = true;
		while (checkNext)
		{
			McdIdentPart part;
			this->parseIdentPart(part);
			ident._parts.push_back(part);
			checkNext = this->_reader.tryMatchToken(A2lTokenKind::Dot);
			if (checkNext)
			{
				this->_reader.readNextToken();
			}
		}
	}

	void A2lParser::parseIdentPart(McdIdentPart & part)
	{
		this->_reader.matchToken(A2lTokenKind::Identifier);
		part._name = this->_reader.token()._text;
		this->_reader.readNextToken();
		while (this->_reader.tryMatchToken(A2lTokenKind::LeftSquareBracket))
		{
			this->_reader.readNextToken();
			if (this->_reader.tryMatchToken(A2lTokenKind::Identifier))
			{
				part._arrayIndexList.push_back(this->_reader.token()._text);
			}
			else if (this->_reader.tryMatchToken(A2lTokenKind::IntegerLiteral))
			{
				part._arrayIndexList.push_back(TokenReaderUtils::literalIntegerTokenTextToInt64(this->_reader.token()._text));
			}
			else
			{
				this->_reader.throwFormatError("A2lParser::parseIdentPart", "Expected identifier or integer literal");
			}
			this->_reader.readNextToken();
			this->_reader.matchToken(A2lTokenKind::RightSquareBracket);
			this->_reader.readNextToken();
		}
	}

	void A2lParser::parseMeasurement(McdModule & module)
	{
		McdMeasurement measurement;
		this->_reader.parseBlockBegin(A2lKeyword::Measurement);
		this->parseIdent(measurement._name);
		this->_reader.parseString(measurement._longIdentifier);
		this->_reader.parseEnumValue(McdEnumText::DataType, "data type", measurement._dataType);
		this->parseIdent(measurement._conversion);
		this->_reader.parseUInt16(measurement._resolution);
		this->_reader.parseFloat64(measurement._accuracy);
		this->_reader.parseFloat64(measurement._lowerLimit);
		this->_reader.parseFloat64(measurement._upperLimit);
		unordered_set<string> oneTimeKeywords;
		while (!this->_reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			if (this->_reader.tryMatchToken(A2lTokenKind::Identifier))
			{
				string const keyword = this->_reader.token()._text;
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->_reader.readNextToken();
				if (keyword == A2lKeyword::AddressType)
				{
					this->_reader.parseEnumValue(McdEnumText::AddressType, "address type", measurement._addressType);
				}
				else if (keyword == A2lKeyword::ArraySize)
				{
					uint16_t tmp = 0;
					this->_reader.parseUInt16(tmp);
					measurement._arraySize = tmp;
				}
				else if (keyword == A2lKeyword::BitMask)
				{
					this->_reader.parseUInt64(measurement._bitMask);
				}
				else if (keyword == A2lKeyword::ByteOrder)
				{
					this->_reader.parseEnumValue(McdEnumText::ByteOrder, "byte order", measurement._byteOrder);
				}
				else if (keyword == A2lKeyword::Discrete)
				{
					measurement._isDiscrete = true;
				}
				else if (keyword == A2lKeyword::DisplayIdentifier)
				{
					this->parseIdent(measurement._displayIdentifier);
				}
				else if (keyword == A2lKeyword::EcuAddress)
				{
					uint32_t tmp = 0;
					this->_reader.parseUInt32(tmp);
					measurement._ecuAddress = tmp;
				}
				else if (keyword == A2lKeyword::EcuAddressExtension)
				{
					int16_t tmp = 0;
					this->_reader.parseInt16(tmp);
					measurement._ecuAddressExtension = tmp;
				}
				else if (keyword == A2lKeyword::ErrorMask)
				{
					this->_reader.parseUInt64(measurement._errorMask);
				}
				else if (keyword == A2lKeyword::Format)
				{
					this->_reader.parseString(measurement._format);
				}
				else if (keyword == A2lKeyword::Layout)
				{
					this->_reader.parseEnumValue(McdEnumText::IndexMode, "index mode", measurement._indexMode);
				}
				else if (keyword == A2lKeyword::MatrixDim)
				{
					// At least one dimension, further ones until the next keyword starts.
					uint16_t tmp = 0;
					this->_reader.parseUInt16(tmp);
					measurement._matrixDim.push_back(tmp);
					exception_ptr error;
					while (this->_reader.tryParseUInt16(tmp, error))
					{
						measurement._matrixDim.push_back(tmp);
					}
				}
				else if (keyword == A2lKeyword::MaxRefresh)
				{
					McdMaxRefresh tmp;
					this->_reader.parseUInt16(tmp._scalingUnit);
					this->_reader.parseUInt32(tmp._rate);
					measurement._maxRefresh = tmp;
				}
				else if (keyword == A2lKeyword::ModelLink)
				{
					string tmp;
					this->_reader.parseString(tmp);
					measurement._modelLink = std::move(tmp);
				}
				else if (keyword == A2lKeyword::PhysUnit)
				{
					this->_reader.parseString(measurement._physicalUnit);
				}
				else if (keyword == A2lKeyword::ReadWrite)
				{
					measurement._isWriteable = true;
				}
				else if (keyword == A2lKeyword::SymbolLink)
				{
					McdSymbolLink tmp;
					this->_reader.parseString(tmp._symbolName);
					this->_reader.parseInt32(tmp._offset);
					measurement._symbolLink = std::move(tmp);
				}
				else
				{
					this->_reader.throwFormatError("A2lParser::parseMeasurement", "Unknown keyword '" + keyword + "'");
				}
			}
			else
			{
				// M5: IF_DATA. ANNOTATION, BIT_OPERATION, FUNCTION_LIST and VIRTUAL stay
				// out of scope, as does anything else.
				this->_reader.parseUnknownBlock();
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::Measurement);
		module._measurements.push_back(std::move(measurement));
	}

	void A2lParser::parseModCommon(McdModuleCommon & common)
	{
		this->_reader.parseBlockBegin(A2lKeyword::ModCommon);
		this->_reader.parseString(common._comment);
		unordered_set<string> oneTimeKeywords;
		while (this->_reader.tryMatchToken(A2lTokenKind::Identifier))
		{
			string const keyword = this->_reader.token()._text;
			A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
			this->_reader.readNextToken();
			if (keyword == A2lKeyword::AlignmentByte)
			{
				this->_reader.parseUInt16(common._alignmentByte);
			}
			else if (keyword == A2lKeyword::AlignmentFloat16Ieee)
			{
				this->_reader.parseUInt16(common._alignmentFloat16Ieee);
			}
			else if (keyword == A2lKeyword::AlignmentFloat32Ieee)
			{
				this->_reader.parseUInt16(common._alignmentFloat32Ieee);
			}
			else if (keyword == A2lKeyword::AlignmentFloat64Ieee)
			{
				this->_reader.parseUInt16(common._alignmentFloat64Ieee);
			}
			else if (keyword == A2lKeyword::AlignmentInt64)
			{
				this->_reader.parseUInt16(common._alignmentInt64);
			}
			else if (keyword == A2lKeyword::AlignmentLong)
			{
				this->_reader.parseUInt16(common._alignmentLong);
			}
			else if (keyword == A2lKeyword::AlignmentWord)
			{
				this->_reader.parseUInt16(common._alignmentWord);
			}
			else if (keyword == A2lKeyword::ByteOrder)
			{
				this->_reader.parseEnumValue(McdEnumText::ByteOrder, "byte order", common._byteOrder);
			}
			else if (keyword == A2lKeyword::DataSize)
			{
				this->_reader.parseUInt16(common._dataSize);
			}
			else if (keyword == A2lKeyword::Deposit)
			{
				this->_reader.parseEnumValue(McdEnumText::Deposit, "deposit", common._deposit);
			}
			else
			{
				this->_reader.throwFormatError("A2lParser::parseModCommon", "Unknown keyword '" + keyword + "'");
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::ModCommon);
	}

	void A2lParser::parseModPar(McdModuleParameters & parameters)
	{
		this->_reader.parseBlockBegin(A2lKeyword::ModPar);
		this->_reader.parseString(parameters._comment);
		unordered_set<string> oneTimeKeywords;
		while (!this->_reader.tryMatchToken(A2lTokenKind::EscapeEnd))
		{
			if (this->_reader.tryMatchToken(A2lTokenKind::Identifier))
			{
				string const keyword = this->_reader.token()._text;
				this->_reader.readNextToken();
				// ADDR_EPK and SYSTEM_CONSTANT may occur more than once.
				if (keyword == A2lKeyword::AddrEpk)
				{
					uint32_t tmp = 0;
					this->_reader.parseUInt32(tmp);
					parameters._addrEpks.push_back(tmp);
				}
				else if (keyword == A2lKeyword::SystemConstant)
				{
					McdSystemConstant tmp;
					this->_reader.parseString(tmp._name);
					this->_reader.parseString(tmp._value);
					parameters._systemConstants.push_back(std::move(tmp));
				}
				else if (keyword == A2lKeyword::CpuType)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._cpuType);
				}
				else if (keyword == A2lKeyword::Customer)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._customer);
				}
				else if (keyword == A2lKeyword::CustomerNo)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._customerNumber);
				}
				else if (keyword == A2lKeyword::Ecu)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._ecu);
				}
				else if (keyword == A2lKeyword::EcuCalibrationOffset)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseInt32(parameters._ecuCalibrationOffset);
				}
				else if (keyword == A2lKeyword::Epk)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._epk);
				}
				else if (keyword == A2lKeyword::NoOfInterfaces)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseUInt16(parameters._numberOfInterfaces);
				}
				else if (keyword == A2lKeyword::PhoneNo)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._phoneNumber);
				}
				else if (keyword == A2lKeyword::Supplier)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._supplier);
				}
				else if (keyword == A2lKeyword::User)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._user);
				}
				else if (keyword == A2lKeyword::Version)
				{
					A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
					this->_reader.parseString(parameters._version);
				}
				else
				{
					this->_reader.throwFormatError("A2lParser::parseModPar", "Unknown keyword '" + keyword + "'");
				}
			}
			else
			{
				// CALIBRATION_METHOD, MEMORY_LAYOUT, MEMORY_SEGMENT and anything else
				// are not part of the parsed scope.
				this->_reader.parseUnknownBlock();
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::ModPar);
	}

	void A2lParser::parseModule(McdProject & project)
	{
		McdModule module;
		this->_reader.parseBlockBegin(A2lKeyword::Module);
		this->parseIdent(module._name);
		this->_reader.parseString(module._longIdentifier);
		unordered_set<string> oneTimeKeywords;
		string keyword;
		while (this->_reader.tryTokenStackBlockBeginAny(keyword))
		{
			if (keyword == A2lKeyword::A2ml)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->_reader.parseUnknownBlock();
			}
			else if (keyword == A2lKeyword::CompuMethod)
			{
				this->parseCompuMethod(module);
			}
			else if (keyword == A2lKeyword::CompuTab)
			{
				this->parseCompuTab(module);
			}
			else if (keyword == A2lKeyword::CompuVTab)
			{
				this->parseCompuVTab(module);
			}
			else if (keyword == A2lKeyword::CompuVTabRange)
			{
				this->parseCompuVTabRange(module);
			}
			else if (keyword == A2lKeyword::Measurement)
			{
				this->parseMeasurement(module);
			}
			else if (keyword == A2lKeyword::ModCommon)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->parseModCommon(module._common);
			}
			else if (keyword == A2lKeyword::ModPar)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->parseModPar(module._parameters);
			}
			else if (keyword == A2lKeyword::VariantCoding)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				this->_reader.parseUnknownBlock();
			}
			else
			{
				// M5: IF_DATA. Everything else stays out of scope
				// (CHARACTERISTIC, RECORD_LAYOUT, AXIS_PTS, FUNCTION, GROUP, ...).
				this->_reader.parseUnknownBlock();
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::Module);
		project._modules.push_back(std::move(module));
	}

	void A2lParser::parseProject(McdProject & project)
	{
		this->_reader.parseBlockBegin(A2lKeyword::Project);
		this->parseIdent(project._name);
		this->_reader.parseString(project._longIdentifier);
		string keyword;
		unordered_set<string> oneTimeKeywords;
		// HEADER is optional and may be absent entirely.
		while (this->_reader.tryTokenStackBlockBeginAny(keyword))
		{
			if (keyword == A2lKeyword::Header)
			{
				A2lTokenReader::trackOneTimeKeyword(oneTimeKeywords, keyword);
				project._header.emplace();
				this->parseHeader(*project._header);
			}
			else if (keyword == A2lKeyword::Module)
			{
				this->parseModule(project);
			}
			else
			{
				this->_reader.throwFormatError("A2lParser::parseProject", "Unexpected block '" + keyword + "'");
			}
		}
		this->_reader.parseBlockEnd(A2lKeyword::Project);
	}

	void A2lParser::parseVersion(McdVersion & version)
	{
		this->_reader.parseUInt16(version._versionNo);
		this->_reader.parseUInt16(version._upgradeNo);
	}

	void A2lParser::setOptions(const A2lOptions & options)
	{
		this->_options = options;
	}
}
