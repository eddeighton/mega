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


#ifndef CODE_GEN_18_04_2019
#define CODE_GEN_18_04_2019

#include "mega/eg_common_strings.hpp"

#include "compiler/codegen/dataAccessPrinter.hpp"

#include "compiler/interface.hpp"
#include "compiler/stages/interface_stage.hpp"
#include "compiler/stages/implementation_stage.hpp"
#include "compiler/instruction.hpp"

#include <string>
#include <cstddef>
#include <vector>
#include <ostream>

namespace eg
{
    static const char* EG_BASE_PREFIX = "Base_";
    static const char* EG_INTERFACE_PREFIX = "__eg_";
    static const char* EG_INTERFACE_PARAMETER_TYPE = "__eg";
    
    static const char* EG_INSTANCE = "eg::Instance";
    static const char* EG_TYPE_ID = "eg::TypeID";
    static const char* EG_TIME_STAMP = "eg::TimeStamp";
    static const char* EG_ACTION_STATE = "eg::ActionState";
    static const char* EG_INVALID_TIMESTAMP = "eg::INVALID_TIMESTAMP";
    static const char* EG_INVALID_STATE = "eg::TOTAL_ACTION_STATES";
    static const char* EG_OBJECT_TRAITS = "eg::ObjectTraits";
    static const char* EG_DIMENSION_TRAITS = "eg::DimensionTraits";
    static const char* EG_INVOKE_IMPL_TYPE = "__invoke_impl";
    static const char* EG_REFERENCE_ITERATOR_TYPE = "__eg_ReferenceIterator";
    static const char* EG_REFERENCE_RAW_ITERATOR_TYPE = "__eg_ReferenceRawIterator";
    static const char* EG_MULTI_ITERATOR_TYPE = "__eg_MultiIterator";
    static const char* EG_RANGE_TYPE = "__eg_Range";
    static const char* EG_RESULT_TYPE = "eg::result_type";
    static const char* EG_TYPE_PATH_CANNON_TYPE = "eg::CanonicaliseTypePath";
    static const char* EG_RING_BUFFER_ALLOCATOR_TYPE = "eg::RingBufferAllocator";
    static const char* EG_REFERENCE_TYPE = "eg::reference";
    static const char* EG_IS_CONVERTIBLE_TYPE = "eg::is_convertible";
    static const char* EG_EVENT_TYPE = "eg::Event";
    static const char* EG_EVENT_LOG_EVENT_TYPE = "eg::_event";
    static const char* EG_EVENT_ITERATOR = "eg::event_iterator";
	static const char* EG_TRAITS_SIZE = "Size";
	static const char* EG_TRAITS_SIMPLE = "Simple";
	static const char* EG_ALLOCATOR_TYPE = "std::uint64_t";
    static const char* EG_RETURN_REASON_TYPE = "eg::ActionCoroutine";
    
    static const char* pszLine = 
    "//////////////////////////////////////////////////////////////////////////////////\n";
    
    class Identifiers;
    
    void generateIncludeGuard( std::ostream& os, const char* pszName );
    
    void printActionType( std::ostream& os, const InvocationSolution::Context& returnTypes );
    
    std::string getInterfaceType( const std::string& strType );
    std::string getBaseTraitType( std::size_t szIndex );
    std::string getInterfaceInstantiationType( const std::string& strType, int iDepth );
    
    void generateIncludeHeader( std::ostream& os, const interface::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser );
        
    std::string getStaticType( const interface::Element* pElement );
    
    void generateInterface( std::ostream& os, 
        const interface::Root* pRoot, 
        const Identifiers* pIdentifiers );

    namespace Input
    {
        class Context;
        class Export;
    }
        
    class TranslationUnit;
    struct FunctionBodyGenerator
    {
        virtual void printFunctionBody( const input::Context* pContext, std::ostream& os ) = 0;
        virtual void printExportBody( const input::Export* pContext, std::ostream& os ) = 0;
    };
    void generateOperationSource( std::ostream& os, 
        const interface::Root* pRoot, 
        const TranslationUnit& translationUnit,
        FunctionBodyGenerator& functionBodyGenerator );
    
    void generateBufferStructures( std::ostream& os, const ReadStage& program );

    std::string getDimensionAccessorFunctionName( const concrete::Dimension* pDimension );
    void generateAccessorFunctionForwardDecls( std::ostream& os, const Interface& program );
     
    void generateGenericsHeader( std::ostream& os, 
        const Interface& program );
        
    void generateMemberFunctions( std::ostream& os, 
        const Interface& program );
        
    void generateGenerics(std::ostream& os, 
        PrinterFactory& printerFactory,
        const ReadStage& program, 
        const TranslationUnit& translationUnit );
        
    class InstructionCodeGeneratorFactory;
        
    void generateImplementationSource( std::ostream& os, 
        InstructionCodeGeneratorFactory& instructionCodeGenFactory,
        PrinterFactory& printerFactory,
        const ReadStage& program, 
        const TranslationUnit& translationUnit,
        const std::vector< std::string >& additionalIncludes );
        
    void generateActionInstanceFunctionsForwardDecls( std::ostream& os, const std::vector< const concrete::Action* >& actions );
    void generateAccessorFunctionImpls( std::ostream& os, PrinterFactory& printerFactory, const ReadStage& program );
    void generateActionInstanceFunctions( std::ostream& os, PrinterFactory& printerFactory, const ReadStage& program );
            
    class DataMember;

    void generateDimensionType( std::ostream& os, const concrete::Dimension_User* pDimension );
    void generateDimensionType( std::ostream& os, const concrete::Dimension_Generated* pDimension );
    void generateDataMemberType( std::ostream& os, const DataMember* pDataMember );
    
    void generateAllocation( std::ostream& os, const DataMember* pDataMember, const std::string& strIndex );
    void generateDeallocation( std::ostream& os, const DataMember* pDataMember, const std::string& strIndex );
    
    void generateEncode( std::ostream& os, const DataMember* pDataMember, const std::string& strIndex );
    void generateDecode( std::ostream& os, const DataMember* pDataMember, const std::string& strIndex );
	
    class Layout;
	void generateInstructions( std::ostream& os, InstructionCodeGeneratorFactory& factory, 
        PrinterFactory& printerFactory, const RootInstruction* pRootInstruction, const Layout& layout );
        
        
        
    inline std::string generateName( char prefix, const std::vector< const concrete::Element* >& path )
    {
        std::ostringstream os;
        os << prefix;
        const concrete::Element* pIterLast = nullptr;
        for( const concrete::Element* pIter : path )
        {
            if( pIter->getParent() )
                os << "_" << pIter->getAbstractElement()->getIdentifier();
            pIterLast = pIter;
        }
        VERIFY_RTE( pIterLast );
        //os << "_" << pIterLast->getIndex();
        return os.str();
    }

    inline std::string generateName( char prefix, const concrete::Element* pElement, const concrete::Element* pFrom )
    {
        std::ostringstream os;
        os << prefix;
        std::vector< const concrete::Element* > path = concrete::getPath( pElement, pFrom );
        const concrete::Element* pIterLast = nullptr;
        for( const concrete::Element* pIter : path )
        {
            os << "_" << pIter->getAbstractElement()->getIdentifier();
            pIterLast = pIter;
        }
        VERIFY_RTE( pIterLast );
        //os << "_" << pIterLast->getIndex();
        return os.str();
    }

}

#endif //CODE_GEN_18_04_2019