#include "tagepch.h"
#include "TAGE/Utilities/Platform.h"
#include "TAGE/Application/Application.h"

#include <commdlg.h>
#include <shlobj.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define NOMINMAX
#include <GLFW/glfw3native.h>

namespace TAGE {
	namespace Platform {
		std::string FileDialog::OpenFile(const char* filter)
		{
            OPENFILENAMEA ofn;
            std::vector<char> szFile(4096, 0);

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = glfwGetWin32Window(Application::Get()->GetWindow()->GetGLFWWindow());
            ofn.lpstrFile = szFile.data();
            ofn.nMaxFile = static_cast<DWORD>(szFile.size());
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileNameA(&ofn) == TRUE)
                return std::string(ofn.lpstrFile);

            LOG_WARN("File dialog canceled or failed.");
            return std::string();
		}
		std::string FileDialog::SaveFile(const char* filter)
		{
            OPENFILENAMEA ofn;
            CHAR szFile[260] = { 0 };

            ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = glfwGetWin32Window(Application::Get()->GetWindow()->GetGLFWWindow());
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;
            ofn.lpstrDefExt = ".scene";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

            if (GetSaveFileNameA(&ofn) == TRUE)
                return std::string(ofn.lpstrFile);

            return std::string();
		}
        std::string FileDialog::OpenDirectory()
        {
            BROWSEINFO bi = { 0 };
            const wchar_t* title = L"Select a folder";
            bi.lpszTitle = title;
            bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

            PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&bi);
            if (pidl != nullptr)
            {
                CHAR path[MAX_PATH];
                if (SHGetPathFromIDListA(pidl, path))
                {
                    CoTaskMemFree(pidl);
                    return std::string(path);
                }
                CoTaskMemFree(pidl);
            }

            return std::string();
        }
	}
}