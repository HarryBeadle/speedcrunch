file(GLOB_RECURSE doc_RST *.rst)
file(GLOB_RECURSE doc_PO *.po)
list(APPEND doc_depends
    ${doc_RST}
    ${doc_PO}
    conf.py
    doc-tool.py)

# find Python
find_package(PythonInterp 3.4 QUIET)
if(NOT ${PYTHONINTERP_FOUND})
    # try 2.7 as a fallback
    find_package(PythonInterp 2.7 EXACT REQUIRED)
endif()

# get a path for qcollectiongenerator
find_package(Qt5Help)

if(TARGET Qt5::qcollectiongenerator)
    # The Qt5::qcollectiongenerator imported target was added in Qt 5.3, but it tells us exactly where
    # qcollectiongenerator is.
    get_target_property(qcg Qt5::qcollectiongenerator LOCATION)
    get_filename_component(qt_bin_dir ${qcg} DIRECTORY)
else()
    # "Next to qmake" is probably correct.
    get_target_property(qmake Qt5::qmake LOCATION)
    get_filename_component(qt_bin_dir ${qmake} DIRECTORY)
endif()

find_program(
    QCOLLECTIONGENERATOR_EXECUTABLE
    NAMES qcollectiongenerator
    HINTS ${qt_bin_dir})

# get a path for sphinx-build
if(WIN32)
    get_filename_component(python_dir ${PYTHON_EXECUTABLE} DIRECTORY)
    # on Windows, there's a good chance Sphinx is in the Python installation we found
    set(extra_sphinx_paths ${python_dir}/Scripts)
endif()

find_program(
    SPHINX_EXECUTABLE
    NAMES sphinx-build
    PATHS ${extra_sphinx_paths})

set(DOCTOOL_EXECUTABLE ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/doc-tool.py")

add_custom_command(
    OUTPUT manual.qrc
    COMMAND ${DOCTOOL_EXECUTABLE}
            --sphinx-build-binary="${SPHINX_EXECUTABLE}"
            --qcollectiongenerator-binary=${QCOLLECTIONGENERATOR_EXECUTABLE}
            --source-dir="${CMAKE_CURRENT_SOURCE_DIR}"
            build-bundled-docs --build-dir="${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS ${doc_depends})

add_custom_target(manual DEPENDS manual.qrc)
