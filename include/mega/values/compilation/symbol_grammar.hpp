
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef GUARD_2023_November_14_symbol_grammar
#define GUARD_2023_November_14_symbol_grammar

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

namespace mega
{

struct SymbolParseException : public std::runtime_error
{
    SymbolParseException( const std::string& str )
        : std::runtime_error{ str }
    {
    }
};

struct Symbol
{
    static Symbol parse( const std::string& str );
    std::string   str() const;

    inline bool operator==( const Symbol& ) const = default;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_identifier;
    }

    std::string m_identifier;
};

struct SymbolVariant
{
    static SymbolVariant parse( const std::string& str );
    std::string          str() const;

    inline bool operator==( const SymbolVariant& ) const = default;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_symbols;
    }
    std::vector< Symbol > m_symbols;
};

struct SymbolVariantPath
{
    static SymbolVariantPath parse( const std::string& str );
    std::string              str() const;

    inline bool operator==( const SymbolVariantPath& ) const = default;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_symbolVariants;
    }
    std::vector< SymbolVariant > m_symbolVariants;
};

struct SymbolVariantPathSequence
{
    static SymbolVariantPathSequence parse( const std::string& str );
    std::string                      str() const;

    inline bool operator==( const SymbolVariantPathSequence& ) const = default;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_symbolVariantPaths;
    }
    std::vector< SymbolVariantPath > m_symbolVariantPaths;
};

struct NamedSymbolVariantPath
{
    static NamedSymbolVariantPath parse( const std::string& str );
    std::string                   str() const;

    inline bool operator==( const NamedSymbolVariantPath& ) const = default;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_symbolVariantPath;
        archive& m_name;
    }
    SymbolVariantPath       m_symbolVariantPath;
    std::optional< Symbol > m_name;
};

struct NamedSymbolVariantPathSequence
{
    static NamedSymbolVariantPathSequence parse( const std::string& str );
    static std::optional< NamedSymbolVariantPathSequence > parseOptional( const std::string& str );
    std::string                           str() const;

    inline bool operator==( const NamedSymbolVariantPathSequence& ) const = default;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_symbolVariantPaths;
    }
    std::vector< NamedSymbolVariantPath > m_symbolVariantPaths;
};

} // namespace mega

std::ostream& operator<<( std::ostream& os, const mega::Symbol& value );
std::ostream& operator<<( std::ostream& os, const mega::SymbolVariant& value );
std::ostream& operator<<( std::ostream& os, const mega::SymbolVariantPath& value );
std::ostream& operator<<( std::ostream& os, const mega::SymbolVariantPathSequence& value );
std::ostream& operator<<( std::ostream& os, const mega::NamedSymbolVariantPath& value );
std::ostream& operator<<( std::ostream& os, const mega::NamedSymbolVariantPathSequence& value );

#endif // GUARD_2023_November_14_symbol_grammar
