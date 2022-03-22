

//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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


#include "compiler/codegen/codegen.hpp"

#include "compiler/eg.hpp"
#include "compiler/input.hpp"
#include "compiler/interface.hpp"
#include "compiler/derivation.hpp"
#include "compiler/translation_unit.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>


namespace eg
{

    struct ExportSourceVisitor
    {
        std::ostream& os;
        const eg::TranslationUnit& m_translationUnit;
        FunctionBodyGenerator& m_functionBodyGenerator;
        std::string strIndent;

        ExportSourceVisitor( std::ostream& os, 
                    const eg::TranslationUnit& translationUnit,
                    FunctionBodyGenerator& functionBodyGenerator  )
            :   os( os ),
                m_translationUnit( translationUnit ),
                m_functionBodyGenerator( functionBodyGenerator )
        {
        }

        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Visibility*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {
            push( (input::Context*) pElement, pNode );
        }
        void push ( const input::Context*    pElement, const interface::Element* pNode )
        {
            const interface::Context* pContext = dynamic_cast< const interface::Context* >( pNode );
            VERIFY_RTE( pContext );

            //calculate the path to the root type
            std::vector< const interface::Element* > path = getPath( pNode );
          
            std::vector< interface::Export* > exports;
            pContext->getExports( exports );
            for( interface::Export* pExport : exports )
            {
                //generate type comment
                {
                    os << "\n//";
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            os << "::";
                        os << pNodeIter->getIdentifier();
                    }
                    os << "::foobar\n";
                }

                //generate the template argument lists
                {
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        //os << strIndent << "template< typename _EG" << iCounter << " >\n";
                        os << strIndent << "template<>\n";
                        ++iCounter;
                    }
                }

                //os << strIndent << "template< typename... Args >\n";

                //just generate an explicit template specialisation
                os << strIndent << "inline " << pExport->getReturnType() << " ";
                {
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        //os << getInterfaceType( pNodeIter->getIdentifier() ) << "< _EG" << iCounter << " >::";
                        os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                        ++iCounter;
                    }

                    {
                        os << pExport->getIdentifier() << "( " << pExport->getParameters() << " ) const\n";
                    }
                }

                //generate the function body
                os << strIndent << "{\n";
                m_functionBodyGenerator.printExportBody( 
                    dynamic_cast< const input::Export* >( pExport->getInputElement() ), os );
                os << strIndent << "}\n";
            }
        }
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Using*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Visibility*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Context*    pElement, const interface::Element* pNode )
        {
        }
    };

    struct OperationsSourceVisitor
    {
        std::ostream& os;
        const eg::TranslationUnit& m_translationUnit;
        FunctionBodyGenerator& m_functionBodyGenerator;
        std::string strIndent;

        OperationsSourceVisitor( std::ostream& os, 
                    const eg::TranslationUnit& translationUnit, 
                    FunctionBodyGenerator& functionBodyGenerator )
            :   os( os ),
                m_translationUnit( translationUnit ),
                m_functionBodyGenerator( functionBodyGenerator )
        {
        }

        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Visibility*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {
            push( (input::Context*) pElement, pNode );
        }
        void push ( const input::Context*    pElement, const interface::Element* pNode )
        {
            const interface::Context* pContext = dynamic_cast< const interface::Context* >( pNode );
            VERIFY_RTE( pContext );

            bool bVisibilityGuard = false;
            
            if( ( m_translationUnit.isAction( pContext ) ) && pContext->getDefinitionFile() )
            {
                //calculate the path to the root type
                std::vector< const interface::Element* > path = getPath( pNode );

                //acquire the object via the starter / allocator
                if( const interface::Abstract* pContext = dynamic_cast< const interface::Abstract* >( pNode ) )
                {
                }
                else if( const interface::Event* pContext = dynamic_cast< const interface::Event* >( pNode ) )
                {
                }
                else if( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pNode ) )
                {
                    if( pContext->getVisibility() == eg::eVisPrivate )
                    {
                        const std::string strDefine = 
                            m_translationUnit.getCoordinatorHostnameDefinitionFile().getHostDefine();
                        if( !strDefine.empty() )
                        {
                            os << "#ifdef " << strDefine << "\n";
                            bVisibilityGuard = true;
                        }
                    }
                    
                    //generate type comment
                    {
                        os << "\n//";
                        for( const interface::Element* pNodeIter : path )
                        {
                            if( pNodeIter != *path.begin())
                                os << "::";
                            os << pNodeIter->getIdentifier();
                        }
                        os << " " << pContext->getDefinitionFile().value() << "\n";
                    }

                    //generate the template argument lists
                    {
                        int iCounter = 1;
                        for( const interface::Element* pNodeIter : path )
                        {
                            os << strIndent << "template<>\n";
                            ++iCounter;
                        }
                    }
                    
                    os << strIndent << pContext->getReturnType() << " ";
                    {
                        for( const interface::Element* pNodeIter : path )
                        {
                            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                        }
                    }
                    if( pElement->getParams() )
                        os << "operator()( " << pElement->getParams()->getStr() << " ) const\n";
                    else
                        os << "operator()() const\n";
                    
                    //generate the function body
                    os << strIndent << "{\n";
                    m_functionBodyGenerator.printFunctionBody( pElement, os );
                    os << strIndent << "}\n";
                }
                else if( const interface::Action* pContext = dynamic_cast< const interface::Action* >( pNode ) )
                {
                    {
                        const std::string strDefine = 
                            m_translationUnit.getCoordinatorHostnameDefinitionFile().getHostDefine();
                        if( !strDefine.empty() )
                        {
                            os << "#ifdef " << strDefine << "\n";
                            bVisibilityGuard = true;
                        }
                    }
                    
                    //generate type comment
                    {
                        os << "\n//";
                        for( const interface::Element* pNodeIter : path )
                        {
                            if( pNodeIter != *path.begin())
                                os << "::";
                            os << pNodeIter->getIdentifier();
                        }
                        os << " " << pContext->getDefinitionFile().value() << "\n";
                    }

                    //generate the template argument lists
                    {
                        int iCounter = 1;
                        for( const interface::Element* pNodeIter : path )
                        {
                            os << strIndent << "template<>\n";
                            ++iCounter;
                        }
                    }
                    
                    os << strIndent << EG_RETURN_REASON_TYPE << " ";
                    {
                        for( const interface::Element* pNodeIter : path )
                        {
                            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                        }
                    }
                    os << "operator()() const\n";
                    
                    //generate the function body
                    os << strIndent << "{\n";
                    m_functionBodyGenerator.printFunctionBody( pElement, os );
                    os << strIndent << "\n  co_return eg::done();\n";
                    os << strIndent << "}\n";
                }
                else if( const interface::Object* pContext = dynamic_cast< const interface::Object* >( pNode ) )
                {
                    {
                        const std::string strDefine = 
                            m_translationUnit.getCoordinatorHostnameDefinitionFile().getHostDefine();
                        if( !strDefine.empty() )
                        {
                            os << "#ifdef " << strDefine << "\n";
                            bVisibilityGuard = true;
                        }
                    }
                    
                    //generate type comment
                    {
                        os << "\n//";
                        for( const interface::Element* pNodeIter : path )
                        {
                            if( pNodeIter != *path.begin())
                                os << "::";
                            os << pNodeIter->getIdentifier();
                        }
                        os << " " << pContext->getDefinitionFile().value() << "\n";
                    }

                    //generate the template argument lists
                    {
                        int iCounter = 1;
                        for( const interface::Element* pNodeIter : path )
                        {
                            os << strIndent << "template<>\n";
                            ++iCounter;
                        }
                    }
                    
                    os << strIndent << EG_RETURN_REASON_TYPE << " ";
                    {
                        for( const interface::Element* pNodeIter : path )
                        {
                            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                        }
                    }
                    os << "operator()() const\n";
                    
                    //generate the function body
                    os << strIndent << "{\n";
                    m_functionBodyGenerator.printFunctionBody( pElement, os );
                    os << strIndent << "\n  co_return eg::done();\n";
                    os << strIndent << "}\n";
                }
                else if( const interface::Link* pContext = dynamic_cast< const interface::Link* >( pNode ) )
                {
                }
                else
                {
                    THROW_RTE( "Unknown abstract type" );
                }
            }
            
            if( bVisibilityGuard )
            {
                os << "#endif\n";
            }
        }
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Using*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Visibility*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Context*    pElement, const interface::Element* pNode )
        {
        }
    };

    void generateOperationSource( std::ostream& os, 
        const interface::Root* pRoot, 
        const eg::TranslationUnit& translationUnit,
        FunctionBodyGenerator& functionBodyGenerator  )
    {
        //generate exports first
        {
            ExportSourceVisitor visitor( os, translationUnit, functionBodyGenerator );
            pRoot->pushpop( visitor );
        }

        //generate operations
        {
            OperationsSourceVisitor visitor( os, translationUnit, functionBodyGenerator );
            pRoot->pushpop( visitor );
        }
    }


}

