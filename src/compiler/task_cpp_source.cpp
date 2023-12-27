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

#include "base_task.hpp"

#include "database/CPPSourceStage.hxx"

#include "mega/common_strings.hpp"

#include "common/file.hpp"

#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

#include <memory>
#include <iostream>

using namespace CPPSourceStage;

namespace mega::compiler
{

struct trace_include_files : public boost::wave::context_policies::default_preprocessing_hooks
{
    trace_include_files( std::ostream& os, std::set< std::string >& files_ )
        : os( os )
        , files( files_ )
        , include_depth( 0 )
    {
    }
    template < typename ContextT >
    bool locate_include_file( ContextT& ctx, std::string& file_path, bool is_system, char const* current_name,
                              std::string& dir_path, std::string& native_name )
    {
        // return false;

        if( !ctx.find_include_file( file_path, dir_path, is_system, current_name ) )
            return false; // could not locate file

        namespace fs = boost::filesystem;

        fs::path native_path( boost::wave::util::create_path( file_path ) );
        if( !fs::exists( native_path ) )
        {
            BOOST_WAVE_THROW_CTX(
                ctx, boost::wave::preprocess_exception, bad_include_file, file_path.c_str(), ctx.get_main_pos() );
            return false;
        }

        // return the unique full file system path of the located file
        native_name = boost::wave::util::native_file_string( native_path );

        return true; // include file has been located successfully
    }

    template < typename ContextT >
    bool found_include_directive( ContextT const& ctx, std::string const& filename, bool include_next )
    {
        os << "#include " << filename << "\n";
        return true; // ok to include this file
    }

    template < typename ContextT >
    void opened_include_file( ContextT const& ctx, std::string const& relname, std::string const& filename,
                              bool is_system_include )
    {
        std::set< std::string >::iterator it = files.find( filename );
        if( it == files.end() )
        {
            // print indented filename
            for( std::size_t i = 0; i < include_depth; ++i )
                std::cout << " ";
            std::cout << filename << std::endl;

            files.insert( filename );
        }
        ++include_depth;
    }

    template < typename ContextT >
    void returning_from_include_file( ContextT const& ctx )
    {
        --include_depth;
    }

    std::ostream&            os;
    std::set< std::string >& files;
    std::size_t              include_depth;
};

class Task_CPP_Source : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPP_Source( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile = m_environment.CPPSourceStage_Source( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath cppHeaderFile = m_environment.CPPSource( m_sourceFilePath );
        const auto                                 srcFilePath   = m_environment.FilePath( m_sourceFilePath );

        start( taskProgress, "Task_CPP_Source", m_sourceFilePath.path(), cppHeaderFile.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash, srcFilePath } );

        if( m_environment.restore( compilationFile, determinant )
            && m_environment.restore( cppHeaderFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.setBuildHashCode( cppHeaderFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pInterfaceComponent = nullptr;
        {
            for( auto pComponent : database.many< Components::Component >( m_environment.project_manifest() ) )
            {
                if( pComponent->get_type() == mega::ComponentType::eInterface )
                {
                    VERIFY_RTE_MSG( !pInterfaceComponent, "Multiple interface components found" );
                    pInterfaceComponent = pComponent;
                }
            }
        }

        std::string strFileContents;
        boost::filesystem::loadAsciiFile( srcFilePath, strFileContents );

        std::ostringstream osSourceFile;
        {
            // current file position is saved for exception handling
            boost::wave::util::file_position_type current_position;

            try
            {
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
                typedef boost::wave::context< std::string::iterator, lex_iterator_type,
                                              boost::wave::iteration_context_policies::load_file_to_string,
                                              trace_include_files >
                    context_type;

                //  The preprocessor iterator shouldn't be constructed directly. It is
                //  generated through a wave::context<> object. This wave:context<> object
                //  is additionally used to initialize and define different parameters of
                //  the actual preprocessing (not done here).
                //
                //  The preprocessing of the input stream is done on the fly behind the
                //  scenes during iteration over the range of context_type::iterator_type
                //  instances.

                std::set< std::string > files;
                trace_include_files     trace( osSourceFile, files );

                context_type ctx( strFileContents.begin(), strFileContents.end(), srcFilePath.string().c_str(), trace );

                for( const auto& includeDir : pInterfaceComponent->get_include_directories() )
                {
                    ctx.add_include_path( includeDir.string().c_str() );
                }

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
                    current_position = ( *first ).get_position();
                    osSourceFile << ( *first ).get_value();
                    ++first;
                }
                //]
            }
            catch( boost::wave::cpp_exception const& e )
            {
                // some preprocessing error
                THROW_RTE( e.file_name() << ":" << e.line_no() << " " << e.description() );
            }
            catch( std::exception const& e )
            {
                // use last recognized token to retrieve the error position
                THROW_RTE( current_position.get_file() << ":" << current_position.get_line() << " "
                                                       << "exception caught: " << e.what() );
            }
            catch( ... )
            {
                // use last recognized token to retrieve the error position
                THROW_RTE( current_position.get_file() << ":" << current_position.get_line() << " "
                                                       << "unexpected exception caught." );
            }
        }

        {
            if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( cppHeaderFile ), osSourceFile.str() ) )
            {
                m_environment.setBuildHashCode( cppHeaderFile );
                m_environment.stash( cppHeaderFile, determinant );
            }
            else
            {
                m_environment.setBuildHashCode( cppHeaderFile );
            }

            const task::FileHash fileHashCode = database.save_Source_to_temp();
            m_environment.setBuildHashCode( compilationFile, fileHashCode );
            m_environment.temp_to_real( compilationFile );
            m_environment.stash( compilationFile, determinant );

            succeeded( taskProgress );
            return;
        }
        failed( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPP_Source( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP_Source >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
