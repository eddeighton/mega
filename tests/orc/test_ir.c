

typedef int ( *funcPtr )();

void getFunction( int, funcPtr* );

static funcPtr pFunctionPtr;

typedef struct reference_
{
    int   instance;
    int   type;
    void* p;
} Pointer;

int test( Pointer* pReference )
{
    if ( !pFunctionPtr )
    {
        getFunction( pReference->instance, &pFunctionPtr );
    }

    switch( pReference->type )
    {
        case 0:
            return pFunctionPtr();
        default:
            return 0;
    }

}
