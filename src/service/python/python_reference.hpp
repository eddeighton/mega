
#include "mega/reference.hpp"

#include <vector>

#include <pybind11/pybind11.h>

namespace megastructure
{
class PythonEGReferenceFactory;

class PythonEGReference
{
public:
    PythonEGReference( PythonEGReferenceFactory& pythonReferenceFactory, const mega::reference& ref );
    
    PyObject* get( void* pClosure );
    int set( void* pClosure, PyObject* pValue );
    PyObject* str() const;
    PyObject* call( PyObject *args, PyObject *kwargs );
    
    const mega::reference getEGReference() const { return m_reference; }
private:
    PythonEGReferenceFactory& m_pythonReferenceFactory;
    mega::reference m_reference;
    std::vector< mega::TypeID > m_type_path;
};

}
