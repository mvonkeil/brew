##
#
#  |_  _ _
#  |_)| (/_VV
#
#  Copyright 2015-2018 Marcus v. Keil
#
##

cmake_minimum_required(VERSION 3.6)

find_package(OpenGL QUIET)
if(OPENGL_FOUND)

project(BrewVideoGL)

brew_library(VideoGL
		GLCanvas.cpp
		GLContext.cpp
		GLGPUExecutionContext.cpp
		GLExtensions.cpp
		GLFrameBuffer.cpp
		GLIndexBuffer.cpp
		GLMesh.cpp
		GLObject.cpp
		GLShader.cpp
		GLShaderProgram.cpp
		GLShaderVariables.cpp
		GLTexture.cpp
		GLVertexBuffer.cpp
		)

brew_link_libraries(GL)

add_subdirectory(tests)

endif()
