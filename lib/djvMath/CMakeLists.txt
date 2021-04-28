set(header
    BBox.h
    BBoxInline.h
    FrameNumber.h
    INumericValueModel.h
    INumericValueModelInline.h
    Math.h
    MathInline.h
    Matrix.h
    MatrixInline.h
    NumericValueModels.h
    Range.h
    RangeInline.h
    Rational.h
    RationalInline.h
    Ray.h
    RayInline.h
    Vector.h
    VectorInline.h)
set(source
    BBox.cpp
    FrameNumber.cpp
    NumericValueModels.cpp
    Rational.cpp
    Vector.cpp)

add_library(djvMath ${header} ${source})
set(LIBRARIES
    djvCore
    GLM)
target_link_libraries(djvMath ${LIBRARIES})
set_target_properties(
    djvMath
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvMath
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvMath)

