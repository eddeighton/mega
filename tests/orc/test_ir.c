

typedef int ( *funcPtr )();

void getFunction( int, funcPtr* );

static funcPtr pFunctionPtr;

typedef struct reference_
{
    int   instance;
    int   type;
    void* p;
} reference;

int test( reference* pReference )
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
