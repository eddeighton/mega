
#include <pybind11/pybind11.h>

#include <ostream>

namespace megastructure
{
    
class PythonEGReference;

class PythonEGReferenceFactory
{
public:
    static PythonEGReference* getReference( PyObject* pPyObject );

    PythonEGReferenceFactory( const char* pszModuleName, mega::EGRuntime& egRuntime, mega::RuntimeTypeInterop& runtimeInterop );
    ~PythonEGReferenceFactory();
    
    PyObject* create( mega::reference ref );
    
    void printType( mega::TypeID type, std::ostream& os );
    mega::TypeID getTypeID( const char* pszIdentity );
    PyObject* invoke( const mega::reference& reference, const std::vector< mega::TypeID >& typePath, PyObject *args, PyObject *kwargs );
    
    /*
    mega::TimeStamp getTimestamp( mega::TypeID type, mega::Instance instance );
    mega::ActionState getState( mega::TypeID type, mega::Instance instance );
    mega::TimeStamp getStopCycle( mega::TypeID type, mega::Instance instance );
    mega::TimeStamp cycle( mega::TypeID type );
        */
private:
    mega::EGRuntime& m_egRuntime;
    mega::RuntimeTypeInterop& m_runtimeInterop;
    PyTypeObject* m_pTypeObject;
    std::vector< PyGetSetDef > m_pythonAttributesData;
    std::vector< const char* > m_identities;
};



}