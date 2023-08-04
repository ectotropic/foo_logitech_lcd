#pragma once
#ifndef GUID_4BF89EDB_28FB_40BC_9589_F5304D654CB9
#define GUID_4BF89EDB_28FB_40BC_9589_F5304D654CB9
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
#include "Windows/UI/UI.h"
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#ifndef CDERR_SUCCESS
#   define CDERR_SUCCESS 0x0000
#endif
#ifndef CDERR_ABORTED
#   define CDERR_ABORTED 0xF001
#endif
//--------------------------------------

namespace Windows::UI {
    //**************************************************************************
    // CommDialog
    //**************************************************************************
    struct CommDialog final {
    public:
        //======================================================================
        // COMMDLGERROR
        //======================================================================
        using COMMDLGERROR = DWORD;

        //======================================================================
        // ChooseColor
        //======================================================================
        static COMMDLGERROR ChooseColor(_In_opt_ HWND hwndOwner,
                                        _Inout_ LPCOLORREF pColor,
                                        _In_ DWORD flags = CC_FULLOPEN) noexcept {
            static COLORREF s_CustColors[16] = {
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
            };

            assert(pColor);
            if (!pColor) { return CDERR_INITIALIZATION; }

            CHOOSECOLOR ccdata;
            ZeroMemory(&ccdata, sizeof(CHOOSECOLOR));
            ccdata.lStructSize = sizeof(CHOOSECOLOR);

            ccdata.lpCustColors = s_CustColors;
            ccdata.hwndOwner    = hwndOwner;
            ccdata.Flags        = flags | CC_RGBINIT;
            ccdata.rgbResult    = *pColor;
            ccdata.lpfnHook     = NULL;

            if (::ChooseColor(&ccdata)) {
                *pColor = ccdata.rgbResult;
                return CDERR_SUCCESS;
            } else {
                const auto err = LastError();
                return err == 0 ? CDERR_ABORTED : err;
            }
        } // ChooseColor

        //--------------------------------------------------

        static COMMDLGERROR ChooseColour(_In_opt_ HWND hwndOwner,
                                         _Inout_ LPCOLORREF pColor,
                                         _In_ DWORD flags = CC_FULLOPEN) noexcept {
            return ChooseColor(hwndOwner, pColor, flags);
        } // ChooseColor

        //======================================================================
        // ChooseFont
        //======================================================================
        static COMMDLGERROR ChooseFont(_In_opt_ HWND hwndOwner,
                                       _Inout_ LPLOGFONT pFont,
                                       _Inout_ LPCOLORREF pColor,
                                       _In_ DWORD flags = 0) noexcept {
            assert(pFont);
            if (!pFont) { return CDERR_INITIALIZATION; }

            CHOOSEFONT cfdata;
            ZeroMemory(&cfdata, sizeof(CHOOSEFONT));
            cfdata.lStructSize = sizeof(CHOOSEFONT);

            cfdata.hwndOwner = hwndOwner;
            cfdata.lpLogFont = pFont;
            cfdata.Flags     = flags | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT;

            if (pColor) {
                cfdata.Flags |= CF_EFFECTS;
                cfdata.rgbColors = *pColor;
            }

            if (::ChooseFont(&cfdata)) {
                pFont = cfdata.lpLogFont;
                if (pColor) { (*pColor) = cfdata.rgbColors; }
                return CDERR_SUCCESS;
            } else {
                const auto err = LastError();
                return err == 0 ? CDERR_ABORTED : err;
            }
        } // ChooseFont

        //======================================================================
        // GetOpenFileName
        //======================================================================
        static COMMDLGERROR GetOpenFileName(_In_opt_ HWND hwndOwner,
                                            _Inout_ LPTSTR szFile,
                                            _In_ DWORD nMaxFile,
                                            _In_opt_z_ LPCTSTR szFilter = NULL,
                                            _In_ DWORD flags = 0) noexcept {
            OPENFILENAME ofdata;
            ZeroMemory(&ofdata, sizeof(OPENFILENAME));
            ofdata.lStructSize = sizeof(OPENFILENAME);

            ofdata.hwndOwner   = hwndOwner;
            ofdata.lpstrFilter = szFilter;
            ofdata.lpstrFile   = szFile;
            ofdata.nMaxFile    = nMaxFile;
            ofdata.Flags       = flags | OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            if (::GetOpenFileName(&ofdata)) {
                return CDERR_SUCCESS;
            } else {
                const auto err = LastError();
                return err == 0 ? CDERR_ABORTED : err;
            }
        }

        //--------------------------------------------------

        static COMMDLGERROR GetOpenFileName(_In_opt_ HWND hwndOwner,
                                            _Inout_ CSimpleString& strFile,
                                            _In_opt_z_ LPCTSTR szFilter = NULL,
                                            _In_ DWORD flags = 0) {
            auto lpszBuf = strFile.GetBuffer(MAX_PATH);
            auto nLength = strFile.GetAllocLength();

            auto err = GetOpenFileName(hwndOwner,
                                       lpszBuf,
                                       MAX_PATH,
                                       szFilter,
                                       flags);
            if (err == FNERR_BUFFERTOOSMALL) {
                lpszBuf = strFile.GetBuffer(((LPWORD)lpszBuf)[0]);
                nLength = strFile.GetAllocLength();
                err = GetOpenFileName(hwndOwner,
                                      lpszBuf,
                                      nLength,
                                      szFilter,
                                      flags);
            }
            return err;
        }

        //======================================================================
        // LastError
        //======================================================================
        static COMMDLGERROR LastError() noexcept {
            return ::CommDlgExtendedError();
        } // LastError

        //======================================================================
        // ErrorString
        //======================================================================
        static constexpr const TCHAR* const ErrorString(_In_ COMMDLGERROR error) noexcept {
            switch (error) {
            case CDERR_DIALOGFAILURE:    return TEXT("The dialog box could not be created. The common dialog box function's call to the DialogBox function failed. For example, this error occurs if the common dialog box call specifies an invalid window handle.");
            case CDERR_FINDRESFAILURE:   return TEXT("The common dialog box function failed to find a specified resource.");
            case CDERR_INITIALIZATION:   return TEXT("The common dialog box function failed during initialization. This error often occurs when sufficient util is not available.");
            case CDERR_LOADRESFAILURE:   return TEXT("The common dialog box function failed to load a specified resource.");
            case CDERR_LOADSTRFAILURE:   return TEXT("The common dialog box function failed to load a specified string.");
            case CDERR_LOCKRESFAILURE:   return TEXT("The common dialog box function failed to lock a specified resource.");
            case CDERR_MEMALLOCFAILURE:  return TEXT("The common dialog box function was unable to allocate util for internal structures.");
            case CDERR_MEMLOCKFAILURE:   return TEXT("The common dialog box function was unable to lock the util associated with a handle.");
            case CDERR_NOHINSTANCE:      return TEXT("The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding instance handle.");
            case CDERR_NOHOOK:           return TEXT("The ENABLEHOOK flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a pointer to a corresponding hook procedure.");
            case CDERR_NOTEMPLATE:       return TEXT("The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding template.");
            case CDERR_REGISTERMSGFAIL:  return TEXT("The RegisterWindowMessage function returned an error code when it was called by the common dialog box function.");
            case CDERR_STRUCTSIZE:       return TEXT("The lStructSize member of the initialization structure for the corresponding common dialog box is invalid.");
            case PDERR_CREATEICFAILURE:  return TEXT("The PrintDlg function failed when it attempted to create an information context.");
            case PDERR_DEFAULTDIFFERENT: return TEXT("You called the PrintDlg function with the DN_DEFAULTPRN flag specified in the wDefault member of the DEVNAMES structure, but the printer described by the other structure members did not match the current default printer. This error occurs when you store the DEVNAMES structure, and the user changes the default printer by using the Control Panel.");
            case PDERR_DNDMMISMATCH:     return TEXT("The data in the DEVMODE and DEVNAMES structures describes two different printers.");
            case PDERR_GETDEVMODEFAIL:   return TEXT("The printer driver failed to initialize a DEVMODE structure.");
            case PDERR_INITFAILURE:      return TEXT("The PrintDlg function failed during initialization, and there is no more specific extended error code to describe the failure. This is the generic default error code for the function.");
            case PDERR_LOADDRVFAILURE:   return TEXT("The PrintDlg function failed to load the device driver for the specified printer.");
            case PDERR_NODEFAULTPRN:     return TEXT("A default printer does not exist.");
            case PDERR_NODEVICES:        return TEXT("No printer drivers were found.");
            case PDERR_PARSEFAILURE:     return TEXT("The PrintDlg function failed to parse the strings in the [devices] section of the WIN.INI file.");
            case PDERR_PRINTERNOTFOUND:  return TEXT("The [devices] section of the WIN.INI file did not contain an entry for the requested printer.");
            case PDERR_RETDEFFAILURE:    return TEXT("The PD_RETURNDEFAULT flag was specified in the Flags member of the PRINTDLG structure, but the hDevMode or hDevNames member was not NULL.");
            case PDERR_SETUPFAILURE:     return TEXT("The PrintDlg function failed to load the required resources.");
            case CFERR_MAXLESSTHANMIN:   return TEXT("The size specified in the nSizeMax member of the CHOOSEFONT structure is less than the size specified in the nSizeMin member.");
            case CFERR_NOFONTS:          return TEXT("No fonts exist.");
            case FNERR_BUFFERTOOSMALL:   return TEXT("The buffer pointed to by the lpstrFile member of the OPENFILENAME structure is too small for the file name specified by the user. The first two bytes of the lpstrFile buffer contain an integer value specifying the size required to receive the full name, incharacters.");
            case FNERR_INVALIDFILENAME:  return TEXT("A file name is invalid.");
            case FNERR_SUBCLASSFAILURE:  return TEXT("An attempt to subclass a list box failed because sufficient util was not available.");
            case FRERR_BUFFERLENGTHZERO: return TEXT("A member of the FINDREPLACE structure points to an invalid buffer.");
            default: return TEXT("Unrecognised or invalid Common Dialog error.");
            }
        } // ErrorString
    }; // struct CommDialog final
} // namespace Windows::UI

#endif // GUID_4BF89EDB_28FB_40BC_9589_F5304D654CB9