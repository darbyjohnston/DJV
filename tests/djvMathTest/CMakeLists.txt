set(header
    BBoxTest.h
	FrameNumberTest.h
    MathTest.h
    MatrixTest.h
    NumericValueModelsTest.h
	RangeTest.h
	RationalTest.h
    VectorTest.h)
set(source
    BBoxTest.cpp
	FrameNumberTest.cpp
    MathTest.cpp
    MatrixTest.cpp
    NumericValueModelsTest.cpp
	RangeTest.cpp
	RationalTest.cpp
    VectorTest.cpp)

add_library(djvMathTest ${header} ${source})
target_link_libraries(djvMathTest djvTestLib)
set_target_properties(
    djvMathTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

