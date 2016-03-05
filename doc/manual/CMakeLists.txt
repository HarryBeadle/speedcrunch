file(GLOB_RECURSE doc_RST *.rst)
file(GLOB_RECURSE doc_PO *.po)
list(APPEND doc_depends
    ${doc_RST}
    ${doc_PO}
    conf.py
    doc-tool.py)

set(py3_version 3.4)

# find Python
find_package(PythonInterp ${py3_version} QUIET)
if(NOT ${PYTHONINTERP_FOUND})
    # try 2.7 as a fallback
    find_package(PythonInterp 2.7 REQUIRED)
endif()

if(${PYTHON_VERSION_STRING} VERSION_LESS 2.7 OR (${PYTHON_VERSION_STRING} VERSION_GREATER 3.0 AND ${PYTHON_VERSION_STRING} VERSION_LESS ${py3_version}))
    message(SEND_ERROR "Need Python 2.7 or ${py3_version}+ (found: ${PYTHON_VERSION_STRING})")
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
