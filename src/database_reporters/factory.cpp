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

#include "database_reporters/factory.hpp"

#include "reporters.hpp"

namespace mega::reporters
{
bool isCompilationReportType( const mega::reports::URL& url )
{
    static std::map< char, std::string > reportedIDMap;
    if( reportedIDMap.empty() )
    {
        // clang-format off
        VERIFY_RTE_MSG( reportedIDMap.insert( { SymbolsReporter::ID.front(), SymbolsReporter::ID } ).second, "Invalid reported first char" );
        VERIFY_RTE_MSG( reportedIDMap.insert( { InterfaceTypeIDReporter::ID.front(), InterfaceTypeIDReporter::ID } ).second, "Invalid reported first char" );
        VERIFY_RTE_MSG( reportedIDMap.insert( { ConcreteTypeIDReporter::ID.front(), ConcreteTypeIDReporter::ID } ).second, "Invalid reported first char" );
        VERIFY_RTE_MSG( reportedIDMap.insert( { InterfaceReporter::ID.front(), InterfaceReporter::ID } ).second, "Invalid reported first char" );
        VERIFY_RTE_MSG( reportedIDMap.insert( { InheritanceReporter::ID.front(), InheritanceReporter::ID } ).second, "Invalid reported first char" );

        // clang-format on
    }

    auto reportType = mega::reports::getReportType( url );

    if( reportType.has_value() )
    {
        auto iFind = reportedIDMap.find( reportType.value().front() );
        if( iFind != reportedIDMap.end() )
        {
            return true;
        }
    }
    return false;
}

mega::reports::Container generateCompilationReport( const mega::reports::URL& url, CompilationReportArgs args )
{
    if( mega::reporters::isCompilationReportType( url ) )
    {
        auto reportType = mega::reports::getReportType( url ).value();
        if( reportType == SymbolsReporter::ID )
        {
            mega::reporters::SymbolsReporter reporter( args.environment, args.database );
            return reporter.generate( url );
        }
        else if( reportType == InterfaceTypeIDReporter::ID )
        {
            mega::reporters::InterfaceTypeIDReporter reporter( args.environment, args.database );
            return reporter.generate( url );
        }
        else if( reportType == ConcreteTypeIDReporter::ID )
        {
            mega::reporters::ConcreteTypeIDReporter reporter( args.environment, args.database );
            return reporter.generate( url );
        }
        else if( reportType == InterfaceReporter::ID )
        {
            mega::reporters::InterfaceReporter reporter( args.manifest, args.environment, args.database );
            return reporter.generate( url );
        }
        else if( reportType == InheritanceReporter::ID )
        {
            mega::reporters::InheritanceReporter reporter( args.manifest, args.environment, args.database );
            return reporter.generate( url );
        }
    }
    return {};
}

} // namespace mega::reporters