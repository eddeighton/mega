
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

#include "invocation/invocation.hpp"

#include "database/OperationsStage.hxx"

#include "compiler/derivation.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/compilation/operation_id.hpp"
#include "mega/values/compilation/hyper_graph.hpp"

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "common/unreachable.hpp"

#include <optional>
#include <unordered_set>

namespace OperationsStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/common_ancestor.hpp"
#include "compiler/invocation_policy.hpp"
namespace Derivation
{
#include "compiler/derivation_printer.hpp"
#include "compiler/disambiguate.hpp"
} // namespace Derivation
} // namespace OperationsStage

using namespace OperationsStage;

namespace mega
{
namespace
{
struct InvocationPolicy : public InvocationPolicyBase
{
    OrPtrVector expandLink( OrPtr pOr ) const
    {
        OrPtrVector result;
        result.push_back( pOr );
        return result;
    }

    GraphVertex* commonRootDerivation( GraphVertex* pSource, GraphVertex* pTarget, GraphEdgeVector& edges ) const
    {
        return CommonAncestor::commonRootDerivation( pSource, pTarget, edges, true );
    }

    InvocationPolicy( Database& database )
        : InvocationPolicyBase( database )
    {
    }
};

} // namespace
} // namespace mega

namespace mega::invocation
{

void compileTransitions( OperationsStage::Database& database, const mega::io::megaFilePath& sourceFile )
{
    using namespace OperationsStage;

    auto                         allStates = database.many< Concrete::State >( sourceFile );
    std::set< Concrete::State* > remainingStates( allStates.begin(), allStates.end() );

    auto                            allInterupts = database.many< Concrete::Interupt >( sourceFile );
    std::set< Concrete::Interupt* > remainingInterupts( allInterupts.begin(), allInterupts.end() );

    for( auto pTransition : database.many< Interface::TransitionTypeTrait >( sourceFile ) )
    {
        for( auto pContextVert : pTransition->get_parent_invocation_context()->get_concrete() )
        {
            std::vector< InvocationPolicy::RootPtr > derivations;

            for( auto pSymbolVariantSequence : pTransition->get_symbol_variant_sequences() )
            {
                InvocationPolicy::Spec derivationSpec( pContextVert, pSymbolVariantSequence );

                // solve the context free derivation
                InvocationPolicy              policy( database );
                InvocationPolicy::OrPtrVector finalFrontier;
                InvocationPolicy::RootPtr     pRoot
                    = DerivationSolver::solveContextFree( derivationSpec, policy, finalFrontier );

                Derivation::precedence( pRoot );

                try
                {
                    const Derivation::Disambiguation result = Derivation::disambiguate( pRoot, finalFrontier );
                    if( result != Derivation::eSuccess )
                    {
                        std::ostringstream os;
                        using ::           operator<<;
                        if( result == Derivation::eAmbiguous )
                            os << "Derivation disambiguation was ambiguous for: "
                                << Concrete::printContextFullType( pContextVert ) << "\n";
                        else if( result == Derivation::eFailure )
                            os << "Derivation disambiguation failed for: "
                                << Concrete::printContextFullType( pContextVert ) << "\n";
                        else
                            THROW_RTE( "Unknown derivation failure type" );
                        THROW_RTE( os.str() );
                    }
                    else
                    {
                        derivations.push_back( pRoot );
                    }
                }
                catch( std::exception& ex )
                {
                    std::ostringstream os;
                    os << "Exception while compiling successor for: "
                        << Concrete::printContextFullType( pContextVert ) << "\n";
                    printDerivationStep( pRoot, true, os );
                    os << "\nError: " << ex.what();
                    THROW_RTE( os.str() );
                }
            }

            if( auto pState = db_cast< Concrete::State >( pContextVert ) )
            {
                auto iFind = remainingStates.find( pState );
                VERIFY_RTE( iFind != remainingStates.end() );
                remainingStates.erase( iFind );
                database.construct< Concrete::State >( Concrete::State::Args{ pState, derivations } );
            }
            else if( auto pInterupt = db_cast< Concrete::Interupt >( pContextVert ) )
            {
                auto iFind = remainingInterupts.find( pInterupt );
                VERIFY_RTE( iFind != remainingInterupts.end() );
                remainingInterupts.erase( iFind );
                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, derivations, {} } );
            }
            else
            {
                THROW_RTE( "Unknown successor type" );
            }
        }
    }

    // ensure ALL optional transitions are set
    for( auto pState : remainingStates )
    {
        database.construct< Concrete::State >( Concrete::State::Args{ pState, {} } );
    }
    for( auto pInterupt : remainingInterupts )
    {
        database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, {}, {} } );
    }
}

} // namespace mega::invocation
