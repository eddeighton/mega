
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

#include "preprocessor/preprocessor.hpp"

#include "common/assert_verify.hpp"

///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <boost/wave.hpp>

///////////////////////////////////////////////////////////////////////////////
// Include the lexer stuff
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

namespace mega::preprocessor
{

void preprocess( const Preprocess& settings, std::ostream& output )
{
    // current file position is saved for exception handling
    boost::wave::util::file_position_type current_position;

    try
    {
        //  The following preprocesses the input file given by argv[1].
        //  Open and read in the specified input file.
        std::string instring;
        {
            std::ifstream instream( settings.inputFile.string().c_str() );
            VERIFY_RTE_MSG( instream.is_open(), "Failed to locate input file: " << settings.inputFile.string() );
            instream.unsetf( std::ios::skipws );
            instring = std::string(
                std::istreambuf_iterator< char >( instream.rdbuf() ), std::istreambuf_iterator< char >() );
        }

        //  This token type is one of the central types used throughout the library.
        //  It is a template parameter to some of the public classes and instances
        //  of this type are returned from the iterators.
        typedef boost::wave::cpplexer::lex_token<> token_type;

        //  The template boost::wave::cpplexer::lex_iterator<> is the lexer type to
        //  to use as the token source for the preprocessing engine. It is
        //  parametrized with the token type.
        typedef boost::wave::cpplexer::lex_iterator< token_type > lex_iterator_type;

        //  This is the resulting context type. The first template parameter should
        //  match the iterator type used during construction of the context
        //  instance (see below). It is the type of the underlying input stream.
        typedef boost::wave::context< std::string::iterator, lex_iterator_type > context_type;

        //  The preprocessor iterator shouldn't be constructed directly. It is
        //  generated through a wave::context<> object. This wave:context<> object
        //  is additionally used to initialize and define different parameters of
        //  the actual preprocessing (not done here).
        //
        //  The preprocessing of the input stream is done on the fly behind the
        //  scenes during iteration over the range of context_type::iterator_type
        //  instances.
        context_type ctx( instring.begin(), instring.end(), settings.inputFile.string().c_str() );

        //  Get the preprocessor iterators and use them to generate the token
        //  sequence.
        context_type::iterator_type first = ctx.begin();
        context_type::iterator_type last  = ctx.end();

        //  The input stream is preprocessed for you while iterating over the range
        //  [first, last). The dereferenced iterator returns tokens holding
        //  information about the preprocessed input stream, such as token type,
        //  token value, and position.
        while( first != last )
        {
            const auto& tok  = *first;
            current_position = tok.get_position();
            output << tok.get_value();
            ++first;
        }
    }
    catch( boost::wave::cpp_exception const& e )
    {
        // some preprocessing error
        THROW_RTE( e.file_name() << "(" << e.line_no() << "): " << e.description() );
    }
    catch( std::exception const& e )
    {
        // use last recognized token to retrieve the error position
        THROW_RTE( current_position.get_file() << "(" << current_position.get_line() << "): "
                                               << "exception caught: " << e.what() );
    }
}

} // namespace mega::preprocessor
