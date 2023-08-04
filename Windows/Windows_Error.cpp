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
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#include <cctype>
#include <cwchar>
//--------------------------------------

namespace {
    //**************************************************************************
    // Helpers (mainly to assist code analysis which struggles with lpBuffer)
    //**************************************************************************
    _Success_(return != 0)
    decltype(auto) DoFormatMessage(_In_ DWORD dwFlags,
                                   _In_ DWORD dwMessageId,
                                   _When_(cchBuffer != 0, _At_(lpBuffer, _Outptr_result_z_))
                                   _When_(cchBuffer == 0, _At_(*lpBuffer, _Out_writes_z_(cchBuffer)))
                                   LPSTR * lpBuffer,
                                   _In_ DWORD cchBuffer) noexcept {
        return ::FormatMessageA(dwFlags,
                                /*lpSource*/ NULL,
                                dwMessageId,
                                /*dwLanguageId*/ 0,
                                cchBuffer > 0 ? *lpBuffer : (LPSTR)lpBuffer,
                                cchBuffer,
                                /*Arguments*/ NULL);
    }

    //--------------------------------------------------------------------------

    _Success_(return != 0)
    decltype(auto) DoFormatMessage(_In_ DWORD dwFlags,
                                   _In_ DWORD dwMessageId,
                                   _When_(cchBuffer != 0, _At_(lpBuffer, _Outptr_result_z_))
                                   _When_(cchBuffer == 0, _At_(*lpBuffer, _Out_writes_z_(cchBuffer)))
                                   LPWSTR* lpBuffer,
                                   _In_ DWORD cchBuffer) noexcept {
        return ::FormatMessageW(dwFlags,
                                /*lpSource*/ NULL,
                                dwMessageId,
                                /*dwLanguageId*/ 0,
                                cchBuffer > 0 ? *lpBuffer : (LPWSTR)lpBuffer,
                                cchBuffer,
                                /*Arguments*/ NULL);
    }
} // namespace <anonymous>

//==============================================================================

namespace Windows {
    //**************************************************************************
    // FormatMessageFromError
    //**************************************************************************
    _Success_(return != 0)
    BOOL FormatMessageFromError(_In_ DWORD dwError,
                                _When_(cchBuffer == 0, _At_(&lpBuffer, _Outptr_result_z_))
                                _When_(cchBuffer != 0, _Out_writes_z_(cchBuffer))
                                LPSTR& lpBuffer,
                                _In_ DWORD cchBuffer) noexcept {
        auto dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM |
                             FORMAT_MESSAGE_IGNORE_INSERTS;
        if (cchBuffer == 0) {
            dwFormatFlags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
        }
        const auto cchWritten = DoFormatMessage(dwFormatFlags, dwError,
                                                &lpBuffer, cchBuffer);
        assert(cchWritten > 0); //< Don't use complex asserts with functions used to implement those asserts!
        if (cchWritten > 0) {
            // Message is formatted with a trailing newline, remove it
            auto cch = cchWritten - 1;
            while (cch && std::isspace(lpBuffer[cch])) { lpBuffer[cch--] = '\0'; }
            return TRUE;
        }
        return FALSE;
    }

    //--------------------------------------------------------------------------

    _Success_(return != 0)
    BOOL FormatMessageFromError(_In_ DWORD dwError,
                                _When_(cchBuffer == 0, _At_(&lpBuffer, _Outptr_result_z_))
                                _When_(cchBuffer != 0, _Out_writes_z_(cchBuffer))
                                LPWSTR& lpBuffer,
                                _In_ DWORD cchBuffer) noexcept {
        auto dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM |
                             FORMAT_MESSAGE_IGNORE_INSERTS;
        if (cchBuffer == 0) {
            dwFormatFlags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
        }
        const auto cchWritten = DoFormatMessage(dwFormatFlags, dwError,
                                                &lpBuffer, cchBuffer);
        assert(cchWritten > 0); //< Don't use complex asserts with functions used to implement those asserts!
        if (cchWritten > 0) {
            // Message is formatted with a trailing newline, remove it
            auto cch = cchWritten;
            // VC seems not to include iswspace in the std namespace :/
            while (cch && /*std*/::iswspace(lpBuffer[cch])) { lpBuffer[cch--] = L'\0'; }
            return TRUE;
        }
        return FALSE;
    }
} // namespace Windows
