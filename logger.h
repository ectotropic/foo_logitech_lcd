#pragma once
#ifndef GUID_FF815251_326B_4104_A611_464729F9A27A
#define GUID_FF815251_326B_4104_A611_464729F9A27A
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

#include <string>
#include <memory>  // shared_ptr
//--------------------------------------

//******************************************************************************
//******************************************************************************
// SPDLOG Wrappers and Helpers
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
namespace logger {
    //----------------------------------
    //----------------------------------

    using namespace ::spdlog;

    //----------------------------------
    //----------------------------------

    using logger_t     = ::spdlog::logger;
    using logger_ptr_t = ::std::shared_ptr<::logger::logger_t>;

    using sink_t     = ::spdlog::sinks::sink;
    using sink_ptr_t = ::std::shared_ptr<::logger::sink_t>;

    //----------------------------------
    //----------------------------------

    using string_t  = ::std::string;
    using wstring_t = ::std::wstring;

    //----------------------------------
    //----------------------------------

    using level_t = ::spdlog::level::level_enum;

    //----------------------------------
    //----------------------------------

    void initialise(::logger::string_t   name,
                    ::logger::level_t    level,
                    ::logger::sink_ptr_t sink,
                    ::logger::string_t   pattern = {});

    //----------------------------------

    void uninitialise() noexcept;
} // namespace logger
//--------------------------------------

//******************************************************************************
//******************************************************************************
// Helper Macros
//
// NOTE:
//  - `Safe` variants of log macros are wrapped in `try...catch` to avoid any
//     exceptions, making them safe to use in destructors, exception handlers,
//     etc. Other versions are **not** safe to use in those places. `Safe`
//     versions also allow for greater use of `noexcept` where applicable.
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#ifndef LOGGER_DETAIL_LogIf
#   define LOGGER_DETAIL_LogIf(logger, expr, ...) \
        do { if (expr) { logger ## (__VA_ARGS__); } } while(0)
#endif
#ifndef LOGGER_DETAIL_LogUnless
#   define LOGGER_DETAIL_LogUnless(logger, expr, ...) \
        do { if (!(expr)) { logger ## (__VA_ARGS__); } } while(0)
#endif
#ifndef LOGGER_DETAIL_SafeLog
#   define LOGGER_DETAIL_SafeLog(logger, ...) \
        do { try{ logger ## (__VA_ARGS__); } catch(...) { assert(false); } } while(0)
#endif
//--------------------------------------

//--------------------------------------
// LogDebug
#define LogDebug                        SPDLOG_DEBUG
#define LogDebugIf(expr, ...)           LOGGER_DETAIL_LogIf(LogDebug, expr, ## __VA_ARGS__)
#define LogDebugUnless(expr, ...)       LOGGER_DETAIL_LogUnless(LogDebug, expr, ## __VA_ARGS__)

#define SafeLogDebug(...)               LOGGER_DETAIL_SafeLog(LogDebug, ## __VA_ARGS__)
#define SafeLogDebugIf(expr, ...)       LOGGER_DETAIL_LogIf(SafeLogDebug, expr, ## __VA_ARGS__)
#define SafeLogDebugUnless(expr, ...)   LOGGER_DETAIL_LogUnless(SafeLogDebug, expr, ## __VA_ARGS__)
//--------------------------------------

//--------------------------------------
// LogTrace
#define LogTrace                        SPDLOG_TRACE
#define LogTraceIf(expr, ...)           LOGGER_DETAIL_LogIf(LogTrace, expr, ## __VA_ARGS__)
#define LogTraceUnless(expr, ...)       LOGGER_DETAIL_LogUnless(LogTrace, expr, ## __VA_ARGS__)

#define SafeLogTrace(...)               LOGGER_DETAIL_SafeLog(LogTrace, ## __VA_ARGS__)
#define SafeLogTraceIf(expr, ...)       LOGGER_DETAIL_LogIf(SafeLogTrace, expr, ## __VA_ARGS__)
#define SafeLogTraceUnless(expr, ...)   LOGGER_DETAIL_LogUnless(SafeLogTrace, expr, ## __VA_ARGS__)
//--------------------------------------

//--------------------------------------
// LogInfo
#define LogInfo                             SPDLOG_INFO
#define LogInfoIf(expr, ...)                LOGGER_DETAIL_LogIf(LogInfo, expr, ## __VA_ARGS__)
#define LogInfoUnless(expr, ...)            LOGGER_DETAIL_LogUnless(LogInfo, expr, ## __VA_ARGS__)

#define SafeLogInfo(...)                    LOGGER_DETAIL_SafeLog(LogInfo, ## __VA_ARGS__)
#define SafeLogInfoIf(expr, ...)            LOGGER_DETAIL_LogIf(SafeLogInfo, expr, ## __VA_ARGS__)
#define SafeLogInfoUnless(expr, ...)        LOGGER_DETAIL_LogUnless(SafeLogInfo, expr, ## __VA_ARGS__)

// Alias: LogMessage
#define LogMessage                          LogInfo
#define LogMessageIf                        LogInfoIf
#define LogMessageUnless                    LogInfoUnless

#define SafeLogMessage                      SafeLogInfo
#define SafeLogMessageIf                    SafeLogInfoIf
#define SafeLogMessageUnless                SafeLogInfoUnless

// Alias: LogMsg
#define LogMsg                              LogInfo
#define LogMsgIf                            LogInfoIf
#define LogMsgUnless                        LogInfoUnless

#define SafeLogMsg                          SafeLogInfo
#define SafeLogMsgIf                        SafeLogInfoIf
#define SafeLogMsgUnless                    SafeLogInfoUnless
//--------------------------------------

//--------------------------------------
// LogWarn
#define LogWarn                             SPDLOG_WARN
#define LogWarnIf(expr, ...)                LOGGER_DETAIL_LogIf(LogWarn, expr, ## __VA_ARGS__)
#define LogWarnUnless(expr, ...)            LOGGER_DETAIL_LogUnless(LogWarn, expr, ## __VA_ARGS__)

#define SafeLogWarn(...)                    LOGGER_DETAIL_SafeLog(LogWarn, ## __VA_ARGS__)
#define SafeLogWarnIf(expr, ...)            LOGGER_DETAIL_LogIf(SafeLogWarn, expr, ## __VA_ARGS__)
#define SafeLogWarnUnless(expr, ...)        LOGGER_DETAIL_LogUnless(SafeLogWarn, expr, ## __VA_ARGS__)

// Alias: LogWarning
#define LogWarning                          LogWarn
#define LogWarningIf                        LogWarnIf
#define LogWarningUnless                    LogWarnUnless

#define SafeLogWarning                      SafeLogWarn
#define SafeLogWarningIf                    SafeLogWarnIf
#define SafeLogWarningUnless                SafeLogWarnUnless
//--------------------------------------

//--------------------------------------
// Error
#define LogError                            SPDLOG_ERROR
#define LogErrorIf(expr, ...)               LOGGER_DETAIL_LogIf(LogError, expr, ## __VA_ARGS__)
#define LogErrorUnless(expr, ...)           LOGGER_DETAIL_LogUnless(LogError, expr, ## __VA_ARGS__)

#define SafeLogError(...)                   LOGGER_DETAIL_SafeLog(LogError, ## __VA_ARGS__)
#define SafeLogErrorIf(expr, ...)           LOGGER_DETAIL_LogIf(SafeLogError, expr, ## __VA_ARGS__)
#define SafeLogErrorUnless(expr, ...)       LOGGER_DETAIL_LogUnless(SafeLogError, expr, ## __VA_ARGS__)
//--------------------------------------

//--------------------------------------
// Critical
#define LogCritical                         SPDLOG_ERROR
#define LogCriticalIf(expr, ...)            LOGGER_DETAIL_LogIf(LogCritical, expr, ## __VA_ARGS__)
#define LogCriticalUnless(expr, ...)        LOGGER_DETAIL_LogUnless(LogCritical, expr, ## __VA_ARGS__)

#define SafeLogCritical(...)                LOGGER_DETAIL_SafeLog(LogCritical, ## __VA_ARGS__)
#define SafeLogCriticalIf(expr, ...)        LOGGER_DETAIL_LogIf(SafeLogCritical, expr, ## __VA_ARGS__)
#define SafeLogCriticalUnless(expr, ...)    LOGGER_DETAIL_LogUnless(SafeLogCritical, expr, ## __VA_ARGS__)
//--------------------------------------

//--------------------------------------
// Asserts
//
// NOTE:
//  - The use of `SafeLogError` here
//    avoids code analysis flagging
//    functions for `noexcept`
//    violations for code that will
//    not be in release builds. While
//    the macros could be disabled
//    during code analysis, other
//    analysis warnings can be useful.
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define LOGGER_DETAIL_LogAssert_Message(_, msg, ...) msg

#   define LOGGER_DETAIL_LogAssert(expr, msg, ...) \
        do { \
            if (expr) { break; } \
            SafeLogError("Assertion Failed: " ## msg, ## __VA_ARGS__); \
            assert(expr); \
        } while(0)

#   define LogAssertUnless(expr, ...)   \
        LOGGER_DETAIL_LogAssert(expr, LOGGER_DETAIL_LogAssert_Message(expr, ## __VA_ARGS__, #expr), ## __VA_ARGS__)

#   define LogAssertIf(expr, ...)       LogAssertUnless(!(expr), ## __VA_ARGS__)
#   define LogVerify                    LogAssertUnless
#else // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define LogAssertIf(expr, ...)       ((void)0)
#   define LogAssertUnless(expr, ...)   ((void)0)
#   define LogVerify(expr, ...)         ((void)(expr))
#endif // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
//--------------------------------------

#endif // GUID_FF815251_326B_4104_A611_464729F9A27A