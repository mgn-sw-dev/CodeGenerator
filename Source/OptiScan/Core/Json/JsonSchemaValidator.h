#pragma once
#include <Extern/Nlohmann/json.hpp>
#include <Extern/Valijson/Schema.hpp>
#include <string>

namespace OptiScan::Core::Json
{
    class JsonSchemaValidator
    {
    public:
        /** */
        explicit JsonSchemaValidator(const nlohmann::json & jsonDocument);
        /** @return true if schema is set */
        bool hasSchema() const;
        /** Search for the '$schema' property in a JSON document.
         * Load path as url or local file
         * @throw InvalideOperationExeption if '$schema' property not found. */
        void loadSchemaFromJson();
        /** Load schema from a local path.
         * @throw InvalideOperationExeption if the path failed to open.
         * @throw InvalidOperationExeption if the schema failed to parse. */
        void loadSchemaLocalPath(const std::string & schemaPath);
        /** Load schema from a url path.
         * @throw InvalideOperationExeption if the schema failed to download.
         * @throw InvalideOperationExeption if the schema is empty.
         * @throw InvalideOperationExeption if the schema is invalid.
         * @throw InvalideOperationExeption if the schema is failed to parse. */
        void loadSchemaFromUrl(const std::string & schemaUrl);
        /** Validate json Document against loaded json schema. Use a loadSchema* method before.
         * @throw InvalidOperationException if no schema is set.
         * @throw InvalidOperationException if validation fails. */
        void validate() const;

    private:
    	/** json document */
        const nlohmann::json & _jsonDocument;
    	/** json schema */
        nlohmann::json _jsonSchema;

    	/** Create schema parser and populateSchema with nlohmann json adapter.
    	 * @throw InvalidOperationException if schema is null. */
        void getSchemaFromNLohmann(valijson::Schema & schema) const;

    };

}
