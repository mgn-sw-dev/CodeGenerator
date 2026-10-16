#pragma once

#include <OptiScan/Parser/A2l/A2lDatabase.h>
#include <OptiScan/Parser/A2l/A2lOptions.h>
#include <OptiScan/Parser/A2l/A2lTokenReader.h>
#include <fstream>
#include <optional>

namespace OptiScan::Parser::A2l
{

    class A2lParser 
    {
    public:
        A2lParser(std::istream * input);
        /** */
        void getOptions(A2lOptions & options) const;
        /** @throws FormatException. */
        void parse(A2lDatabase & a2lDatabase);
        /** Call before @ref parse. */
        void setOptions(const A2lOptions & options);

    private:
        A2lOptions _options;
        A2lTokenReader _reader;

        /** @throws FormatException. */
        void parseAsap2Version(Mcd::McdVersion & version);
        /** @throws FormatException. */
        void parseA2mlVersion(std::optional<Mcd::McdVersion> & version);
        /** @throws FormatException. */
        void parseCompuMethod(Mcd::McdModule & module);
        /** @throws FormatException. */
        void parseCompuTab(Mcd::McdModule & module);
        /** @throws FormatException. */
        void parseCompuVTab(Mcd::McdModule & module);
        /** @throws FormatException. */
        void parseCompuVTabRange(Mcd::McdModule & module);
        /** FORMULA block inside a COMPU_METHOD. @throws FormatException. */
        void parseFormula(std::optional<Mcd::McdFormula> & formula);
        /** @throws FormatException. */
        void parseHeader(Mcd::McdHeader & header);
        /** @throws FormatException. */
        void parseIdent(Mcd::McdIdent & ident);
        /** @throws FormatException. */
        void parseIdentPart(Mcd::McdIdentPart & part);
        /** @throws FormatException. */
        void parseMeasurement(Mcd::McdModule & module);
        /** @throws FormatException. */
        void parseModCommon(Mcd::McdModuleCommon & common);
        /** @throws FormatException. */
        void parseModPar(Mcd::McdModuleParameters & parameters);
        /** Parse a MODULE block and append it to @p project.
         *  @throws FormatException. */
        void parseModule(Mcd::McdProject & project);
        /** @throws FormatException. */
        void parseProject(Mcd::McdProject & project);
        /** @throws FormatException. */
        void parseVersion(Mcd::McdVersion & version);
    
    };

}
