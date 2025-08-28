cmake_minimum_required(VERSION 3.16)
project(MealManagementSystem VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Build Type Configuration (Debug/Release) ---
# Set a default build type if none was specified.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Set compiler flags for different build types.
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# --- Qt6 Configuration ---
# Find the Qt6 package and its components
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Automatically run moc, uic, and rcc as needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- Dependency Configuration ---
# Find MySQL Connector/C++
find_package(MySQL REQUIRED)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# --- Project Files ---
# Explicitly list all source files for clarity and reliability.
# This is better practice than using file(GLOB...).
set(HEADERS
    include/loginwindow.h
    include/mainwindow.h
    include/user.h
    include/database.h
    include/menu.h
    include/expense.h
    include/finance.h
    include/attendance.h
    include/period.h
    include/settings.h
    include/userprofilepage.h
    include/menumanagementpage.h
    include/expensetrackingpage.h
    include/mealattendancepage.h
    include/usermanagementpage.h
    include/financialoverviewpage.h
    include/dailymenupage.h
    include/menuhistorypage.h
)

set(SOURCES
    src/main.cpp
    src/loginwindow.cpp
    src/mainwindow.cpp
    src/user.cpp
    src/database.cpp
    src/menu.cpp
    src/expense.cpp
    src/finance.cpp
    src/attendance.cpp
    src/period.cpp
    src/settings.cpp
    src/userprofilepage.cpp
    src/menumanagementpage.cpp
    src/expensetrackingpage.cpp
    src/mealattendancepage.cpp
    src/usermanagementpage.cpp
    src/financialoverviewpage.cpp
    src/dailymenupage.cpp
    src/menuhistorypage.cpp
)

# --- Executable Target ---
# Define the executable and its source files
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY AUTOMOC_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Linking Libraries ---
# Link all required libraries to the executable
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        ${MySQL_LIBRARIES}
        mysqlcppconn
        OpenSSL::SSL OpenSSL::Crypto)