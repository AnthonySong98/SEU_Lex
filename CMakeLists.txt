cmake_minimum_required(VERSION 3.10)
project(Seu_Lex)

set(CMAKE_CXX_STANDARD 11)

add_executable(Seu_Lex SEU_Lex.cpp helper.h RE_Extraction.h RE_Standardization.h Re2NFA.h NFAToDFA.h DFAMinimization.h GenerateCCode.h)