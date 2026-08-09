#pragma once

#include <OptiScan/Parser/A2l/A2lScanner.h>
#include <istream>
#include <vector>

namespace OptiScan::Parser::A2l
{

    class A2lTokenReader 
    {
    public:
        A2lTokenReader(std::istream * input);
        /** */
        bool hasToken() const;
        /** */
        const A2lScanner & scanner() const;
        /** */
        const A2lToken & token() const;
        /** */
        const std::vector<A2lToken> & tokenStack() const;
        /** @throws InvalidOperationException. */
        void tokenStackBegin();
        /** @throws InvalidOperationException. */
        void tokenStackCommit();
        /** */
        int64_t tokenStackCount() const;
        /** @throws InvalidOperationException. */
        void tokenStackRollback();
    private:
        A2lScanner _scanner;
        std::vector<A2lToken> _tokenStack;
        int64_t _tokenStackCount;
    };

}
