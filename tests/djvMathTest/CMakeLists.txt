set(header
    BBoxFuncTest.h
    BBoxTest.h
	FrameNumberFuncTest.h
	FrameNumberTest.h
    MathFuncTest.h
    MatrixFuncTest.h
    NumericValueModelsTest.h
	RangeFuncTest.h
	RangeTest.h
	RationalFuncTest.h
	RationalTest.h
    VectorFuncTest.h)
set(source
    BBoxFuncTest.cpp
    BBoxTest.cpp
	FrameNumberFuncTest.cpp
	FrameNumberTest.cpp
    MathFuncTest.cpp
    MatrixFuncTest.cpp
    NumericValueModelsTest.cpp
	RangeFuncTest.cpp
	RangeTest.cpp
	RationalFuncTest.cpp
	RationalTest.cpp
    VectorFuncTest.cpp)

add_library(djvMathTest ${header} ${source})
target_link_libraries(djvMathTest djvTestLib)
set_target_properties(
    djvMathTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

