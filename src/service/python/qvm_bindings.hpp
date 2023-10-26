

    // maths
    using namespace boost::qvm;
    // clang-format off
#define QVM_PYBIND11_OPERATORS( T ) \
        .def( "__add__",      []( const T& v, const T& o )      { using namespace boost::qvm; return v + o; },      pybind11::is_operator() ) \
        .def( "__sub__",      []( const T& v, const T& o )      { using namespace boost::qvm; return v - o; },      pybind11::is_operator() ) \
        .def( "__mul__",      []( const T& v, const float& f )  { using namespace boost::qvm; return v * f; },      pybind11::is_operator() ) \
        .def( "__truediv__",  []( const T& v, const float& f )  { using namespace boost::qvm; return v / f; },      pybind11::is_operator() ) \
        .def( "__iadd__",     []( T& v, const T& o )            { using namespace boost::qvm; v += o; return v; },  pybind11::is_operator() ) \
        .def( "__isub__",     []( T& v, const T& o )            { using namespace boost::qvm; v -= o; return v; },  pybind11::is_operator() ) \
        .def( "__imul__",     []( T& v, const float& f )        { using namespace boost::qvm; v *= f; return v; },  pybind11::is_operator() ) \
        .def( "__itruediv__", []( T& v, const float& f )        { using namespace boost::qvm; v /= f; return v; },  pybind11::is_operator() ) \
        .def( "__repr__",     []( const T& v ) { return boost::qvm::to_string( v ); } )

#define QVM_PYBIND11_FUNCTIONS( T ) \
        .def( "norm",   []( T& v )          { boost::qvm::normalize( v ); },            "Normalize" )       \
        .def( "mag",    []( const T& v )    { return boost::qvm::mag( v ); },           "Magnitude" )       \
        .def( "dot",    []( T& v1, T& v2 )  { return boost::qvm::dot( v1, v2 ); },      "Dot Product" )

    // clang-format on
    pybind11::class_< F2 >( pythonModule, "F2" )
        .def( pybind11::init<>() )
        .def( pybind11::init< float, float >() )

        .def_property(
            "x", []( F2& v ) { return v.x(); }, []( F2& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( F2& v ) { return v.y(); }, []( F2& v, float f ) { return v.y( f ); }, "Y coordinate" )

            QVM_PYBIND11_OPERATORS( F2 ) QVM_PYBIND11_FUNCTIONS( F2 );

    pybind11::class_< F3 >( pythonModule, "F3" )
        .def( pybind11::init<>() )
        .def( pybind11::init< float, float, float >() )
        .def_property(
            "x", []( F3& v ) { return v.x(); }, []( F3& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( F3& v ) { return v.y(); }, []( F3& v, float f ) { return v.y( f ); }, "Y coordinate" )
        .def_property(
            "z", []( F3& v ) { return v.z(); }, []( F3& v, float f ) { return v.z( f ); }, "Z coordinate" )

        .def(
            "cross", []( F3& v1, F3& v2 ) { return boost::qvm::cross( v1, v2 ); }, "Cross Product" )

            QVM_PYBIND11_OPERATORS( F3 ) QVM_PYBIND11_FUNCTIONS( F3 );

    pybind11::class_< F4 >( pythonModule, "F4" )
        .def( pybind11::init<>() )
        .def( pybind11::init< float, float, float, float >() )
        .def_property(
            "x", []( F4& v ) { return v.x(); }, []( F4& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( F4& v ) { return v.y(); }, []( F4& v, float f ) { return v.y( f ); }, "Y coordinate" )
        .def_property(
            "z", []( F4& v ) { return v.z(); }, []( F4& v, float f ) { return v.z( f ); }, "Z coordinate" )
        .def_property(
            "w", []( F4& v ) { return v.w(); }, []( F4& v, float f ) { return v.w( f ); }, "W coordinate" )

            QVM_PYBIND11_OPERATORS( F4 ) QVM_PYBIND11_FUNCTIONS( F4 );

    pybind11::class_< Quat >( pythonModule, "Quat" ).def( pybind11::init<>() )
        /*.def( pybind11::init< float, float, float, float >() )
        .def( pybind11::init( []( const F3& axis, float angle ) { return boost::qvm::rot_quat( axis, angle ); } ) )
        .def_property(
            "x", []( Quat& v ) { return v.x(); }, []( Quat& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( Quat& v ) { return v.y(); }, []( Quat& v, float f ) { return v.y( f ); }, "Y coordinate" )
        .def_property(
            "z", []( Quat& v ) { return v.z(); }, []( Quat& v, float f ) { return v.z( f ); }, "Z coordinate" )
        .def_property(
            "w", []( Quat& v ) { return v.w(); }, []( Quat& v, float f ) { return v.w( f ); }, "W coordinate" )

        .def(
            "conjugate", []( Quat& v ) { return boost::qvm::conjugate( v ); }, "Conjugate" )
        .def(
            "inverse", []( Quat& v ) { return boost::qvm::inverse( v ); }, "Inverse" )
        .def(
            "slerp", []( Quat& v1, Quat& v2, float t ) { return boost::qvm::slerp( v1, v2, t ); }, "Slerp" )
        .def(
            "identity", []( Quat& v ) { return boost::qvm::set_identity( v ); }, "Set to indentity" )
        .def(
            "set_rot", []( Quat& v, const F3& axis, float angle ) { boost::qvm::set_rot( v, axis, angle ); },
            "Set rotation" )
        .def(
            "rotate", []( Quat& v, const F3& axis, float angle ) { boost::qvm::rotate( v, axis, angle ); },
            "Rotate" )
        .def(
            "set_rotx", []( Quat& v, float angle ) { boost::qvm::set_rotx( v, angle ); },
            "Set rotation in x axis" )
        .def(
            "set_roty", []( Quat& v, float angle ) { boost::qvm::set_roty( v, angle ); },
            "Set rotation in y axis" )
        .def(
            "set_rotz", []( Quat& v, float angle ) { boost::qvm::set_rotz( v, angle ); },
            "Set rotation in z axis" )
        .def(
            "rotate_x", []( Quat& v, float angle ) { boost::qvm::rotate_x( v, angle ); },
            "Rotate in x axis" )
        .def(
            "rotate_y", []( Quat& v, float angle ) { boost::qvm::rotate_y( v, angle ); },
            "Rotate in y axis" )
        .def(
            "rotate_z", []( Quat& v, float angle ) { boost::qvm::rotate_z( v, angle ); },
            "Rotate in z axis" )

            QVM_PYBIND11_OPERATORS( Quat ) QVM_PYBIND11_FUNCTIONS( Quat )*/
        ;

    pybind11::class_< F33 >( pythonModule, "F33", pybind11::buffer_protocol() )
        .def( pybind11::init<>() )
        .def_buffer(
            []( F33& m ) -> pybind11::buffer_info
            {
                return pybind11::buffer_info(
                    m.data.data(),                                  /* Pointer to buffer */
                    sizeof( float ),                                /* Size of one scalar */
                    pybind11::format_descriptor< float >::format(), /* Python struct-style format descriptor */
                    2,                                              /* Number of dimensions */
                    { 3, 3 },                                       /* Buffer dimensions */
                    { sizeof( float ) * 3,                          /* Strides (in bytes) for each index */
                      sizeof( float ) } );
            } );