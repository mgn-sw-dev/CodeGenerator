
#include <OptiScan/Parser/Arxml/ArxmlKeyword.h>
#include <OptiScan/Parser/Arxml/ArxmlParser.h>
#include <OptiScan/Parser/FileException.h>
#include <limits>
#include <utility>

using namespace OptiScan::Parser::Arxml::Ar;
using namespace std;

namespace OptiScan::Parser::Arxml
{
	ArxmlParser::ArxmlParser(istream * input)
		: _reader(input)
	{
	}

	void ArxmlParser::parse(ArxmlDatabase & arxmlDatabase)
	{
		arxmlDatabase = ArxmlDatabase();
		if (!this->_reader.readNext())
		{
			this->_reader.throwFormatError("ArxmlParser::parse", "The document is empty");
		}
		this->_reader.matchElementBegin(ArxmlKeyword::Autosar);
		this->parseElement(arxmlDatabase, string());
		if (this->_reader.hasNode())
		{
			this->_reader.throwFormatError("ArxmlParser::parse",
				"Unexpected content after </" + ArxmlKeyword::Autosar + ">");
		}
	}

	void ArxmlParser::parseChildren(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & elementType)
	{
		while (!this->_reader.tryMatchElementEnd(elementType))
		{
			if (!this->_reader.hasNode())
			{
				this->_reader.throwFormatError("ArxmlParser::parseChildren",
					"Unexpected end of document inside <" + elementType + ">");
			}
			if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->parseElement(arxmlDatabase, path);
			}
			else
			{
				this->_reader.readNext();
			}
		}
	}

	void ArxmlParser::parseCoefficients(const string & elementType, vector<double> & values)
	{
		this->_reader.parseElementBegin(elementType);
		while (!this->_reader.tryMatchElementEnd(elementType))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::V))
			{
				double value = 0.0;
				this->_reader.parseElementDouble(ArxmlKeyword::V, value);
				values.push_back(value);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		this->_reader.parseElementEnd(elementType);
	}

	void ArxmlParser::parseCompuMethod(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & shortName, size_t & elementIndex)
	{
		ArCompuMethod compuMethod;
		compuMethod._path = path;
		compuMethod._shortName = shortName;
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::CompuMethod))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Category))
			{
				this->_reader.parseElementText(ArxmlKeyword::Category, compuMethod._category);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::UnitRef))
			{
				this->parseRef(ArxmlKeyword::UnitRef, compuMethod._unitRef);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuInternalToPhys))
			{
				this->_reader.parseElementBegin(ArxmlKeyword::CompuInternalToPhys);
				while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::CompuInternalToPhys))
				{
					if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuScales))
					{
						this->_reader.parseElementBegin(ArxmlKeyword::CompuScales);
						while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::CompuScales))
						{
							if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuScale))
							{
								this->parseCompuScale(compuMethod);
							}
							else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
							{
								this->_reader.skipElement();
							}
							else
							{
								this->_reader.readNext();
							}
						}
						this->_reader.parseElementEnd(ArxmlKeyword::CompuScales);
					}
					else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
					{
						this->_reader.skipElement();
					}
					else
					{
						this->_reader.readNext();
					}
				}
				this->_reader.parseElementEnd(ArxmlKeyword::CompuInternalToPhys);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		elementIndex = arxmlDatabase._compuMethods.size();
		arxmlDatabase._compuMethods.push_back(std::move(compuMethod));
	}

	void ArxmlParser::parseCompuScale(ArCompuMethod & compuMethod)
	{
		ArCompuScale scale;
		this->_reader.parseElementBegin(ArxmlKeyword::CompuScale);
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::CompuScale))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::ShortLabel))
			{
				this->_reader.parseElementText(ArxmlKeyword::ShortLabel, scale._shortLabel);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Symbol))
			{
				this->_reader.parseElementText(ArxmlKeyword::Symbol, scale._symbol);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::LowerLimit))
			{
				this->parseLimit(ArxmlKeyword::LowerLimit, scale._lowerLimit);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::UpperLimit))
			{
				this->parseLimit(ArxmlKeyword::UpperLimit, scale._upperLimit);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuConst))
			{
				this->_reader.parseElementBegin(ArxmlKeyword::CompuConst);
				while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::CompuConst))
				{
					if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Vt))
					{
						string text;
						this->_reader.parseElementText(ArxmlKeyword::Vt, text);
						scale._textValue = std::move(text);
					}
					else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
					{
						this->_reader.skipElement();
					}
					else
					{
						this->_reader.readNext();
					}
				}
				this->_reader.parseElementEnd(ArxmlKeyword::CompuConst);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuRationalCoeffs))
			{
				this->_reader.parseElementBegin(ArxmlKeyword::CompuRationalCoeffs);
				while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::CompuRationalCoeffs))
				{
					if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuNumerator))
					{
						this->parseCoefficients(ArxmlKeyword::CompuNumerator, scale._numerator);
					}
					else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuDenominator))
					{
						this->parseCoefficients(ArxmlKeyword::CompuDenominator, scale._denominator);
					}
					else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
					{
						this->_reader.skipElement();
					}
					else
					{
						this->_reader.readNext();
					}
				}
				this->_reader.parseElementEnd(ArxmlKeyword::CompuRationalCoeffs);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		this->_reader.parseElementEnd(ArxmlKeyword::CompuScale);
		compuMethod._scales.push_back(std::move(scale));
	}

	void ArxmlParser::parseDataConstr(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & shortName, size_t & elementIndex)
	{
		ArDataConstr dataConstr;
		dataConstr._path = path;
		dataConstr._shortName = shortName;
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::DataConstr))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::DataConstrRules))
			{
				this->_reader.parseElementBegin(ArxmlKeyword::DataConstrRules);
				while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::DataConstrRules))
				{
					if (this->_reader.tryMatchElementBegin(ArxmlKeyword::DataConstrRule))
					{
						this->_reader.parseElementBegin(ArxmlKeyword::DataConstrRule);
						while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::DataConstrRule))
						{
							bool const isInternal =
								this->_reader.tryMatchElementBegin(ArxmlKeyword::InternalConstrs);
							bool const isPhysical =
								this->_reader.tryMatchElementBegin(ArxmlKeyword::PhysConstrs);
							if (isInternal || isPhysical)
							{
								string const wrapper = this->_reader.getName();
								this->_reader.parseElementBegin(wrapper);
								while (!this->_reader.tryMatchElementEnd(wrapper))
								{
									if (this->_reader.tryMatchElementBegin(ArxmlKeyword::LowerLimit))
									{
										if (isInternal)
										{
											this->parseLimit(ArxmlKeyword::LowerLimit,
												dataConstr._internalLowerLimit);
										}
										else
										{
											this->parseLimit(ArxmlKeyword::LowerLimit,
												dataConstr._physLowerLimit);
										}
									}
									else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::UpperLimit))
									{
										if (isInternal)
										{
											this->parseLimit(ArxmlKeyword::UpperLimit,
												dataConstr._internalUpperLimit);
										}
										else
										{
											this->parseLimit(ArxmlKeyword::UpperLimit,
												dataConstr._physUpperLimit);
										}
									}
									else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
									{
										this->_reader.skipElement();
									}
									else
									{
										this->_reader.readNext();
									}
								}
								this->_reader.parseElementEnd(wrapper);
							}
							else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
							{
								this->_reader.skipElement();
							}
							else
							{
								this->_reader.readNext();
							}
						}
						this->_reader.parseElementEnd(ArxmlKeyword::DataConstrRule);
					}
					else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
					{
						this->_reader.skipElement();
					}
					else
					{
						this->_reader.readNext();
					}
				}
				this->_reader.parseElementEnd(ArxmlKeyword::DataConstrRules);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		elementIndex = arxmlDatabase._dataConstrs.size();
		arxmlDatabase._dataConstrs.push_back(std::move(dataConstr));
	}

	void ArxmlParser::parseDescription(optional<ArDescription> & description)
	{
		ArDescription parsed;
		this->_reader.parseElementBegin(ArxmlKeyword::Desc);
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::Desc))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::L2))
			{
				// The language attribute belongs to the opening node, so it has to be
				// read before parseElementText moves on.
				string const language = this->_reader.attributeOrEmpty(ArxmlKeyword::L);
				string text;
				this->_reader.parseElementText(ArxmlKeyword::L2, text);
				if (language == "DE")
				{
					parsed._german = std::move(text);
				}
				else if (language == "EN")
				{
					parsed._english = std::move(text);
				}
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		this->_reader.parseElementEnd(ArxmlKeyword::Desc);
		if (!parsed._english.empty() || !parsed._german.empty())
		{
			description = std::move(parsed);
		}
	}

	void ArxmlParser::parseElement(ArxmlDatabase & arxmlDatabase, const string & parentPath)
	{
		this->_reader.matchKind(ArxmlNodeKind::ElementBegin);
		string const elementType = this->_reader.getName();
		uint64_t const lineNumber = this->_reader.getLineNumber();
		if (elementType == ArxmlKeyword::AdminData)
		{
			// Pure metadata: SDG, SD and the captions below them. Their SHORT-NAMEs are
			// not referenceable, so indexing them would pollute the path index.
			this->_reader.skipElement();
		}
		else
		{
			this->_reader.readNext();
			string path = parentPath;
			string shortName;
			bool const isIdentifiable = this->_reader.tryMatchElementBegin(ArxmlKeyword::ShortName);
			if (isIdentifiable)
			{
				// In AUTOSAR the SHORT-NAME is the first child of an identifiable element.
				this->_reader.parseElementText(ArxmlKeyword::ShortName, shortName);
				path = parentPath + "/" + shortName;
			}
			size_t elementIndex = ArxmlIdentifiable::NoIndex;
			if (elementType == ArxmlKeyword::CompuMethod)
			{
				this->parseCompuMethod(arxmlDatabase, path, shortName, elementIndex);
			}
			else if (elementType == ArxmlKeyword::DataConstr)
			{
				this->parseDataConstr(arxmlDatabase, path, shortName, elementIndex);
			}
			else if (elementType == ArxmlKeyword::ISignal)
			{
				this->parseISignal(arxmlDatabase, path, shortName, elementIndex);
			}
			else if (elementType == ArxmlKeyword::SwBaseType)
			{
				this->parseSwBaseType(arxmlDatabase, path, shortName, elementIndex);
			}
			else if (elementType == ArxmlKeyword::SystemSignal)
			{
				this->parseSystemSignal(arxmlDatabase, path, shortName, elementIndex);
			}
			else if (elementType == ArxmlKeyword::Unit)
			{
				this->parseUnit(arxmlDatabase, path, shortName, elementIndex);
			}
			else
			{
				this->parseChildren(arxmlDatabase, path, elementType);
			}
			if (isIdentifiable)
			{
				this->registerPath(arxmlDatabase, path, elementType, lineNumber, elementIndex);
			}
			this->_reader.parseElementEnd(elementType);
		}
	}

	void ArxmlParser::parseISignal(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & shortName, size_t & elementIndex)
	{
		ArISignal iSignal;
		iSignal._path = path;
		iSignal._shortName = shortName;
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::ISignal))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Length))
			{
				this->_reader.parseElementUInt32(ArxmlKeyword::Length, iSignal._length);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::SystemSignalRef))
			{
				this->parseRef(ArxmlKeyword::SystemSignalRef, iSignal._systemSignalRef);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::InitValue))
			{
				this->parseNumericalValue(ArxmlKeyword::InitValue, iSignal._initValue);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::NetworkRepresentationProps))
			{
				this->parseISignalProps(iSignal);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Desc))
			{
				this->parseDescription(iSignal._description);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		elementIndex = arxmlDatabase._iSignals.size();
		arxmlDatabase._iSignals.push_back(std::move(iSignal));
	}

	void ArxmlParser::parseISignalProps(ArISignal & iSignal)
	{
		this->_reader.parseElementBegin(ArxmlKeyword::NetworkRepresentationProps);
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::NetworkRepresentationProps))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::BaseTypeRef))
			{
				this->parseRef(ArxmlKeyword::BaseTypeRef, iSignal._baseTypeRef);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::InvalidValue))
			{
				this->parseNumericalValue(ArxmlKeyword::InvalidValue, iSignal._invalidValue);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::SwDataDefPropsVariants)
				|| this->_reader.tryMatchElementBegin(ArxmlKeyword::SwDataDefPropsConditional))
			{
				// Both are pure wrappers; step into them instead of skipping.
				this->_reader.readNext();
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		this->_reader.parseElementEnd(ArxmlKeyword::NetworkRepresentationProps);
	}

	void ArxmlParser::parseLimit(const string & elementType, optional<double> & value)
	{
		string text;
		this->_reader.parseElementText(elementType, text);
		if (text == "INF")
		{
			value = numeric_limits<double>::infinity();
		}
		else if (text == "-INF")
		{
			value = -numeric_limits<double>::infinity();
		}
		else if (!text.empty())
		{
			const char * const begin = text.c_str();
			char * end = nullptr;
			double const parsed = strtod(begin, &end);
			if (end == begin || *end != '\0')
			{
				this->_reader.throwFormatError("ArxmlParser::parseLimit",
					"<" + elementType + "> is not a number: '" + text + "'");
			}
			value = parsed;
		}
	}

	void ArxmlParser::parseNumericalValue(const string & elementType, optional<double> & value)
	{
		this->_reader.parseElementBegin(elementType);
		while (!this->_reader.tryMatchElementEnd(elementType))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::NumericalValueSpecification))
			{
				this->_reader.parseElementBegin(ArxmlKeyword::NumericalValueSpecification);
				while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::NumericalValueSpecification))
				{
					if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Value))
					{
						double parsed = 0.0;
						this->_reader.parseElementDouble(ArxmlKeyword::Value, parsed);
						value = parsed;
					}
					else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
					{
						this->_reader.skipElement();
					}
					else
					{
						this->_reader.readNext();
					}
				}
				this->_reader.parseElementEnd(ArxmlKeyword::NumericalValueSpecification);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				// For example an APPLICATION-VALUE-SPECIFICATION, which is not modelled.
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		this->_reader.parseElementEnd(elementType);
	}

	void ArxmlParser::parseRef(const string & elementType, ArRef & reference)
	{
		// The DEST attribute belongs to the opening node, so it has to be read before
		// parseElementText moves on.
		reference._dest = this->_reader.attributeOrEmpty(ArxmlKeyword::Dest);
		this->_reader.parseElementText(elementType, reference._path);
	}

	void ArxmlParser::parseSwBaseType(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & shortName, size_t & elementIndex)
	{
		ArSwBaseType swBaseType;
		swBaseType._path = path;
		swBaseType._shortName = shortName;
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::SwBaseType))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::BaseTypeEncoding))
			{
				this->_reader.parseElementText(ArxmlKeyword::BaseTypeEncoding, swBaseType._baseTypeEncoding);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::BaseTypeSize))
			{
				this->_reader.parseElementUInt32(ArxmlKeyword::BaseTypeSize, swBaseType._baseTypeSize);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		elementIndex = arxmlDatabase._swBaseTypes.size();
		arxmlDatabase._swBaseTypes.push_back(std::move(swBaseType));
	}

	void ArxmlParser::parseSystemSignal(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & shortName, size_t & elementIndex)
	{
		ArSystemSignal systemSignal;
		systemSignal._path = path;
		systemSignal._shortName = shortName;
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::SystemSignal))
		{
			string name = this->_reader.getName();
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::DynamicLength))
			{
				string text;
				this->_reader.parseElementText(ArxmlKeyword::DynamicLength, text);
				systemSignal._isDynamicLength = text == "true";
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::PhysicalProps))
			{
				this->parseSystemSignalProps(systemSignal);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::Desc))
			{
				this->parseDescription(systemSignal._description);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		elementIndex = arxmlDatabase._systemSignals.size();
		arxmlDatabase._systemSignals.push_back(std::move(systemSignal));
	}

	void ArxmlParser::parseSystemSignalProps(ArSystemSignal & systemSignal)
	{
		this->_reader.parseElementBegin(ArxmlKeyword::PhysicalProps);
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::PhysicalProps))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::CompuMethodRef))
			{
				this->parseRef(ArxmlKeyword::CompuMethodRef, systemSignal._compuMethodRef);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::DataConstrRef))
			{
				this->parseRef(ArxmlKeyword::DataConstrRef, systemSignal._dataConstrRef);
			}
			else if (this->_reader.tryMatchElementBegin(ArxmlKeyword::SwDataDefPropsVariants)
				|| this->_reader.tryMatchElementBegin(ArxmlKeyword::SwDataDefPropsConditional))
			{
				// Both are pure wrappers; step into them instead of skipping.
				this->_reader.readNext();
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		this->_reader.parseElementEnd(ArxmlKeyword::PhysicalProps);
	}

	void ArxmlParser::parseUnit(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & shortName, size_t & elementIndex)
	{
		ArUnit unit;
		unit._path = path;
		unit._shortName = shortName;
		while (!this->_reader.tryMatchElementEnd(ArxmlKeyword::Unit))
		{
			if (this->_reader.tryMatchElementBegin(ArxmlKeyword::DisplayName))
			{
				this->_reader.parseElementText(ArxmlKeyword::DisplayName, unit._displayName);
			}
			else if (this->_reader.getKind() == ArxmlNodeKind::ElementBegin)
			{
				this->_reader.skipElement();
			}
			else
			{
				this->_reader.readNext();
			}
		}
		elementIndex = arxmlDatabase._units.size();
		arxmlDatabase._units.push_back(std::move(unit));
	}

	void ArxmlParser::registerPath(ArxmlDatabase & arxmlDatabase, const string & path,
		const string & elementType, uint64_t lineNumber, size_t elementIndex)
	{
		ArxmlIdentifiable entry;
		entry._elementIndex = elementIndex;
		entry._elementType = elementType;
		entry._lineNumber = lineNumber;
		pair<map<string, ArxmlIdentifiable>::iterator, bool> const inserted =
			arxmlDatabase._pathIndex.insert(make_pair(path, std::move(entry)));
		if (!inserted.second)
		{
			// AUTOSAR requires paths to be unique. The first occurrence is kept so a
			// broken file still yields a usable index.
			arxmlDatabase._duplicatePaths.push_back(path);
		}
	}

}
