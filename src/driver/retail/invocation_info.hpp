
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

#ifndef GUARD_2023_July_29_invocation_info
#define GUARD_2023_July_29_invocation_info

namespace driver::retail
{

struct InvocationInfo
{
    using InvocationIDMap   = std::map< mega::InvocationID, const FinalStage::Operations::Invocation* >;
    using IContextMap       = std::map< const FinalStage::Interface::IContext*, InvocationIDMap >;
    using VariantMap        = std::map< std::set< const FinalStage::Interface::IContext* >, InvocationIDMap >;
    using InvocationUUIDMap = std::map< mega::InvocationID, int >;

    mutable IContextMap contextInvocations;
    mutable VariantMap  variantInvocations;
    InvocationUUIDMap   invocationUUIDs;

    std::string generateResultTypeID( const FinalStage::Operations::Invocation* pInvocation ) const
    {
        auto iFind = invocationUUIDs.find( pInvocation->get_id() );
        VERIFY_RTE( iFind != invocationUUIDs.end() );
        std::ostringstream os;
        os << iFind->second;
        return os.str();
    }

    std::string generateInvocationName( const FinalStage::Operations::Invocation* pInvocation ) const
    {
        std::ostringstream os;
        os << '_' << generateResultTypeID( pInvocation );
        return os.str();
    }

    std::string generateInvocationUse( const FinalStage::Operations::Invocation* pInvocation ) const
    {
        auto               id = generateResultTypeID( pInvocation );
        std::ostringstream os;
        os << '_' << id << '<' << id << '>';
        return os.str();
    }

    InvocationInfo( const mega::io::Manifest& manifest, FinalStage::Database& database )
    {
        using namespace FinalStage;

        mega::U64 uuid = 1;
        for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
        {
            Operations::Invocations* pInvocations = database.one< Operations::Invocations >( megaSrcFile );

            for( const auto& [ id, pInvocation ] : pInvocations->get_invocations() )
            {
                const auto context = pInvocation->get_context();

                std::set< const Interface::IContext* >       uniqueInterfaceContexts;
                std::set< const Interface::DimensionTrait* > uniqueDimensionContexts;

                for( const auto vec : context->get_vectors() )
                {
                    for( const auto element : vec->get_elements() )
                    {
                        auto interfaceContext = element->get_interface();
                        if( interfaceContext->get_context().has_value() )
                        {
                            uniqueInterfaceContexts.insert( interfaceContext->get_context().value() );
                        }
                        else
                        {
                            uniqueDimensionContexts.insert( interfaceContext->get_dimension().value() );
                        }
                    }
                }

                VERIFY_RTE( uniqueDimensionContexts.empty() );
                VERIFY_RTE( !uniqueInterfaceContexts.empty() );

                if( uniqueInterfaceContexts.size() == 1 )
                {
                    auto pContext = *uniqueInterfaceContexts.begin();
                    if( contextInvocations[ pContext ].insert( { id, pInvocation } ).second )
                    {
                        invocationUUIDs[ id ] = uuid++;
                    }
                }
                else
                {
                    // variant
                    if( variantInvocations[ uniqueInterfaceContexts ].insert( { id, pInvocation } ).second )
                    {
                        invocationUUIDs[ id ] = uuid++;
                    }
                }
            }
        }
    }
};

} // namespace driver::retail

#endif // GUARD_2023_July_29_invocation_info
