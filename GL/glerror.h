#pragma once
#ifndef GUID_5ECBD7F3_CF5A_413D_B481_BC1418132BF0
#define GUID_5ECBD7F3_CF5A_413D_B481_BC1418132BF0
/*******************************************************************************
 *******************************************************************************
 * Copyright (c) 2009-2023 ectotropic (ectotropic@gmail.com,                   *
 *                                     https://github.com/ectotropic)          *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation, either version 2.1 of the License, or (at     *
 * your option) any later version.                                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
 * for more details.                                                           *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************
 ******************************************************************************/

//--------------------------------------
//
#include "GL/glcore.h"
//--------------------------------------

//--------------------------------------
//
#ifndef SPDLOG_API
#   ifndef SPDLOG_ACTIVE_LEVEL
#       if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#           define SPDLOG_ACTIVE_LEVEL  SPDLOG_LEVEL_DEBUG
#       else
#           define SPDLOG_ACTIVE_LEVEL  SPDLOG_LEVEL_WARN
#       endif
#   endif

#   if defined(_WIN32) && (defined(UNICODE) || defined(_UNICODE))
#       define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#   endif

#   ifndef FMT_HEADER_ONLY
#       define FMT_HEADER_ONLY
#   endif

#   include <spdlog/spdlog.h>
#endif
//--------------------------------------

//--------------------------------------
//
#include <cassert>
//--------------------------------------

namespace OpenGL {
    //**************************************************************************
    // glGetErrorString
    //**************************************************************************
    inline [[nodiscard]]
    const char* const glGetErrorStringA(GLenum error = ::glGetError()) noexcept {
        switch (error) {
        case GL_NO_ERROR:			return "GL_NO_ERROR";
        case GL_INVALID_ENUM:		return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:		return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:	return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:		return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:	return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:		return "GL_OUT_OF_MEMORY";
        default:                    return "UNKNOWN";
        }
    }

#ifdef _WIN32
    inline [[nodiscard]]
    const wchar_t* const glGetErrorStringW(GLenum error = ::glGetError()) noexcept {
        switch (error) {
        case GL_NO_ERROR:			return L"GL_NO_ERROR";
        case GL_INVALID_ENUM:		return L"GL_INVALID_ENUM";
        case GL_INVALID_VALUE:		return L"GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:	return L"GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:		return L"GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:	return L"GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:		return L"GL_OUT_OF_MEMORY";
        default:                    return L"UNKNOWN";
        }
    }
#   if defined(UNICODE) || defined(_UNICODE)
#       define glGetErrorString     glGetErrorStringA
#   else
#       define glGetErrorString     glGetErrorStringW
#   endif
#else //#ifdef _WIN32
#   define glGetErrorString         glGetErrorStringA
#endif //#ifdef _WIN32
} // namespace OpenGL

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define DETAIL_OpenGLAssert_Message(_, msg, ...) msg

#   define DETAIL_OpenGLAssert_DoAppend(_, _1, ...) _1, ## __VA_ARGS__
#   define DETAIL_OpenGLAssert_Append(_1, ...) DETAIL_OpenGLAssert_DoAppend(__VA_ARGS__, _1)

#   define DETAIL_OpenGLAssert_LogError(code, msg, ...) \
            const auto glLastErrorMessage_{ ::OpenGL::glGetErrorString(code) }; \
            try {\
                SPDLOG_ERROR(\
                    "Assertion Failed (OpenGL): `" ## msg ## "` (OpenGL Error: \"{}\" [{}])", \
                    DETAIL_OpenGLAssert_Append(glLastErrorMessage_, ## __VA_ARGS__),\
                    code \
                ); \
            } catch(...) {} \
            assert(code == GL_NO_ERROR);

#   define DETAIL_OpenGLAssert(expr, msg, ...) \
        do { \
            if (expr) { break; } \
            const auto glLastError_{ ::glGetError() }; \
            DETAIL_OpenGLAssert_LogError(glLastError_, msg, ## __VA_ARGS__) \
        } while(0)

#   define DETAIL_OpenGLAssertNoError(msg, ...) \
        do { \
            const auto glLastError_{ ::glGetError() }; \
            if (glLastError_ == GL_NO_ERROR) { break; } \
            DETAIL_OpenGLAssert_LogError(glLastError_, msg, ## __VA_ARGS__) \
        } while(0)

#   define OpenGLAssertNoError(...)    \
        DETAIL_OpenGLAssertNoError(DETAIL_OpenGLAssert_Message(_, ## __VA_ARGS__, "glGetError() == GL_NO_ERROR"), ## __VA_ARGS__)

#   define OpenGLAssert(expr, ...)    \
        DETAIL_OpenGLAssert(expr, DETAIL_OpenGLAssert_Message(expr, ## __VA_ARGS__, #expr), ## __VA_ARGS__)

#   define	OpenGLVerify                OpenGLAssert
#else // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define	OpenGLAssertNoError(...)    ((void)0)
#   define	OpenGLAssert(...)           ((void)0)
#   define	OpenGLVerify(expr, ...)     ((void)(expr))
#endif // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
//--------------------------------------

#endif // GUID_5ECBD7F3_CF5A_413D_B481_BC1418132BF0
