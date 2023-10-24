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

#ifndef REPORTERS_24_OCT_2023
#define REPORTERS_24_OCT_2023

#include "database/FinalStage.hxx"
#include "environment/environment_archive.hpp"
#include "reports/report.hpp"
#include "reports/reporter.hpp"
#include "mega/values/service/url.hpp"

namespace mega::reporters
{

class SymbolsReporter : public mega::reports::Reporter
{
    mega::io::Environment& m_environment;
    FinalStage::Database&  m_database;

public:
    SymbolsReporter( mega::io::Environment& environment, FinalStage::Database& database );
    mega::reports::ReporterID getID() override { return "symbols"; }
    mega::reports::Container  generate( const mega::reports::URL& url ) override;
};

class InterfaceReporter : public mega::reports::Reporter
{
    mega::io::Manifest&    m_manifest;
    mega::io::Environment& m_environment;
    FinalStage::Database&  m_database;

    void addInheritance( std::optional< FinalStage::Interface::InheritanceTrait* > inheritance,
                         mega::reports::Branch&                                    branch );
    void addProperties( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch,
                        const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions );

    void recurse( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch,
                  FinalStage::Interface::IContext* pContext );

public:
    InterfaceReporter( mega::io::Manifest&    manifest,
                       mega::io::Environment& environment,
                       FinalStage::Database&  database );

    mega::reports::ReporterID getID() override { return "interface"; }
    mega::reports::Container  generate( const mega::reports::URL& url ) override;
};

} // namespace mega::reporters

#endif // REPORTERS_24_OCT_2023
