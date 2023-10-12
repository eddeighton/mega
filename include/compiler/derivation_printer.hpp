
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

static void push_indent( std::string& str )
{
    str += "  ";
}

static void pop_indent( std::string& str )
{
    str.pop_back();
    str.pop_back();
}

static void printDerivationStep( Edge* pEdge, std::string& strIndent, bool bShowEliminated, std::ostream& os )
{
    if( !bShowEliminated && pEdge->get_eliminated() )
        return;

    Step* pStep = pEdge->get_next();
    {
        if( db_cast< And >( pStep ) )
        {
            os << strIndent << "AND (";
        }
        else if( db_cast< Or >( pStep ) )
        {
            os << strIndent << "OR (";
        }
        else
        {
            THROW_RTE( "Unknown step type" );
        }

        Concrete::printContextFullType( pStep->get_vertex(), os );
        if( pEdge->get_eliminated() )
        {
            os << " <eliminated>";
        }
        if( pEdge->get_backtracked() )
        {
            os << " <backtracked>";
        }
        os << "[" << pEdge->get_precedence() << "])\n";
    }

    push_indent( strIndent );
    for( auto pNextEdge : pStep->get_edges() )
    {
        printDerivationStep( pNextEdge, strIndent, bShowEliminated, os );
    }
    pop_indent( strIndent );
}

static void printDerivationStep( Root* pRoot, bool bShowEliminated, std::ostream& os )
{
    {
        os << "ROOT (";

        bool bFirst = true;
        for( auto pContext : pRoot->get_context() )
        {
            if( bFirst )
                bFirst = false;
            else
                os << ", ";
            Concrete::printContextFullType( pContext, os );
        }
        os << ")\n";
    }

    std::string strIndent;
    push_indent( strIndent );

    for( auto pEdge : pRoot->get_edges() )
    {
        printDerivationStep( pEdge, strIndent, bShowEliminated, os );
    }

    pop_indent( strIndent );
}
