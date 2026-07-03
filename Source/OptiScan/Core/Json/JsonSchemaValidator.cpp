#include <Extern/Valijson/Adapters/NlohmannJsonAdapter.hpp>
#include <Extern/Valijson/Schema.hpp>
#include <Extern/Valijson/SchemaParser.hpp>
#include <Extern/Valijson/Validator.hpp>

#include <OptiScan/Core/Json/JsonSchemaValidator.h>
#include <fstream>
#include <stdexcept>

using namespace nlohmann;
using namespace std;
using namespace valijson;

namespace OptiScan::Core::Json
{

	JsonSchemaValidator::JsonSchemaValidator(const json & jsonDocument)
		: _jsonDocument(jsonDocument)
		, _jsonSchema()
	{
	}

	void JsonSchemaValidator::getSchemaFromNLohmann(Schema & schema) const
	{
		if (this->_jsonSchema.is_null())
		{
			throw runtime_error("Schema is null");
		}
		SchemaParser schemaParser(SchemaParser::kDraft7);
		adapters::NlohmannJsonAdapter schemaAdapter(this->_jsonSchema);
		schemaParser.populateSchema(schemaAdapter, schema);
	}

	bool JsonSchemaValidator::hasSchema() const
	{
		return !this->_jsonSchema.is_null();
	}

	void JsonSchemaValidator::loadSchemaFromJson()
	{
		if (!this->_jsonDocument.contains("$schema"))
		{
			throw runtime_error("Config JSON does not contain '$schema' property");
		}
		const string schemaPath = this->_jsonDocument.at("$schema");
		if (schemaPath.rfind("http://", 0) == 0 || schemaPath.rfind("https://", 0) == 0)
		{
			this->loadSchemaFromUrl(schemaPath);
		}
		else
		{
			this->loadSchemaLocalPath(schemaPath);
		}
	}

	void JsonSchemaValidator::loadSchemaLocalPath(const string & schemaPath)
	{
		ifstream inputFile(schemaPath);
		if (!inputFile.is_open())
		{
			throw runtime_error("Failed to open schema file from path: " + schemaPath);
		}
		try
		{
			inputFile >> this->_jsonSchema;
		}
		catch (const json::parse_error & error)
		{
			throw runtime_error("Failed to parse schema file '" + schemaPath + "': " + error.what());
		}
		if (this->_jsonSchema.is_null())
		{
			throw runtime_error("Schema file '" + schemaPath + "' is empty or invalid");
		}
	}

	void JsonSchemaValidator::loadSchemaFromUrl(const std::string & schemaUrl)
	{
		throw runtime_error("Loading JSON schema from URL is not supported without an HTTP backend: " + schemaUrl);
	}

	void JsonSchemaValidator::validate() const
	{
		if (!this->hasSchema())
		{
			throw runtime_error("No schema has been set. Call setSchema* method first.");
		}
		Schema schema;
		this->getSchemaFromNLohmann(schema);

		Validator validator;
		adapters::NlohmannJsonAdapter configAdapter(this->_jsonDocument);
		ValidationResults validationResults;

		const Subschema * rootSchema = schema.root();
		if (!validator.validate(*rootSchema, configAdapter, &validationResults))
		{
			ValidationResults::Error error;
			string errors;
			while (validationResults.popError(error))
			{
				string context;
				for (const auto & part: error.context)
				{
					context += part + " ";
				}
				errors += "- " + context + ": " + error.description + "\n";
			}
			throw runtime_error("JSON Validation Error:\n" + errors);
		}
	}

}
