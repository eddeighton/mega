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

#include "utilities/clang_format.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#ifdef DO_NOT_DO_THIS_BECAUSE_MASSIVE_CODE_BLOAT
#include "clang/Format/Format.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"
#endif

namespace mega::utilities
{

#ifdef DO_NOT_DO_THIS_BECAUSE_MASSIVE_CODE_BLOAT
void clang_format( std::string& strNewFileContents, std::optional< boost::filesystem::path > formatSpecPath )
{
    std::vector< clang::tooling::Range > ranges;
    ranges.push_back( clang::tooling::Range( 0U, strNewFileContents.size() ) );

    llvm::ArrayRef< clang::tooling::Range > rangesRef( ranges );

    llvm::StringRef strRef( strNewFileContents );

    clang::format::FormatStyle style = clang::format::getLLVMStyle();

    if ( formatSpecPath.has_value() )
    {
        const boost::filesystem::path filePath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( formatSpecPath.value() ) );
        VERIFY_RTE_MSG( boost::filesystem::exists( filePath ), "Could not locate format file: " << filePath );

        std::ostringstream os;
        std::string        str;
        boost::filesystem::loadAsciiFile( filePath, str );

        llvm::StringRef strRef( str );
        std::error_code error = clang::format::parseConfiguration( strRef, &style );
        VERIFY_RTE_MSG( error.value() == 0, "Encountered error parsing format file: " << filePath );
    }
    else
    {
        style.BreakBeforeBraces = clang::format::FormatStyle::BS_Allman;

        style.BraceWrapping.AfterClass            = true;
        style.BraceWrapping.AfterControlStatement = clang::format::FormatStyle::BWACS_Always;
        style.BraceWrapping.AfterEnum             = true;
        style.BraceWrapping.AfterFunction         = true;
        style.BraceWrapping.AfterNamespace        = true;
        style.BraceWrapping.AfterObjCDeclaration  = true;
        style.BraceWrapping.AfterStruct           = true;
        style.BraceWrapping.AfterUnion            = true;
        style.BraceWrapping.BeforeCatch           = true;
        style.BraceWrapping.BeforeElse            = true;
        style.BraceWrapping.IndentBraces          = true;

        style.AlignConsecutiveAssignments
            = clang::format::FormatStyle::AlignConsecutiveStyle{ true, true, true, true, true };
        style.AlignConsecutiveDeclarations
            = clang::format::FormatStyle::AlignConsecutiveStyle{ true, true, true, true, true };

        style.SpaceAfterCStyleCast           = false;
        style.SpaceAfterTemplateKeyword      = true;
        style.SpaceBeforeAssignmentOperators = true;
        style.SpaceBeforeParens              = clang::format::FormatStyle::SBPO_Never;
        style.SpaceInEmptyParentheses        = false;
        style.SpacesInAngles                 = clang::format::FormatStyle::SIAS_Always;
        style.SpacesInCStyleCastParentheses  = true;
        style.SpacesInParentheses            = true;
        style.SpacesInSquareBrackets         = true;

        style.KeepEmptyLinesAtTheStartOfBlocks = false;

        style.Language             = clang::format::FormatStyle::LK_Cpp;
        style.NamespaceIndentation = clang::format::FormatStyle::NI_All;
        style.TabWidth             = 4U;
        style.IndentWidth          = 4U;
        style.UseTab               = clang::format::FormatStyle::UT_Never;

        style.ColumnLimit         = 160U;
        style.BreakStringLiterals = true;
    }

    clang::tooling::Replacements replacements = clang::format::reformat( style, strRef, rangesRef );

    std::unique_ptr< llvm::MemoryBuffer > pMemBuffer = llvm::MemoryBuffer::getMemBuffer( strRef );

    auto pChangedCode = clang::tooling::applyAllReplacements( pMemBuffer->getBuffer(), replacements );
    if ( pChangedCode )
    {
        strNewFileContents = *pChangedCode;
    }
}
#else
void clang_format( std::string&, std::optional< boost::filesystem::path > )
{
    // do nothing...
}
#endif

} // namespace mega::utilities
