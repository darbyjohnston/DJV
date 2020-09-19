set(header
    BBoxFunc.h
    BBoxFuncInline.h
    BBox.h
    BBoxInline.h
    FrameNumber.h
    FrameNumberFunc.h
    INumericValueModel.h
    INumericValueModelInline.h
    MathFunc.h
    MathFuncInline.h
    Math.h
    MatrixFunc.h
    MatrixFuncInline.h
    NumericValueModels.h
    RangeFunc.h
    RangeFuncInline.h
    Range.h
    RangeInline.h
    RationalFunc.h
    Rational.h
    RayFunc.h
    RayFuncInline.h
    Ray.h
    RayInline.h
    VectorFunc.h
    VectorFuncInline.h)
set(source
    BBoxFunc.cpp
    FrameNumber.cpp
    FrameNumberFunc.cpp
    NumericValueModels.cpp
    Rational.cpp
    RationalFunc.cpp
    VectorFunc.cpp)

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

