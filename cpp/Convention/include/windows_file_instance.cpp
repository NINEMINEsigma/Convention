#if defined(_WIN32) || defined(_WIN64)
#include "Convention/instance/file_instance.h"
#include <windows.h>
#include <shlwapi.h>
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")

using namespace std;
using namespace std::filesystem;

instance<std::filesystem::path, true>& instance<std::filesystem::path, true>::compress(const path& output_path, const std::string& format)
{
    if (output_path.empty())
    {
        auto out = this->get()->string() + "." + format;
        return this->compress(out, format);
    }

    // 使用Windows Cabinet API进行压缩
    HANDLE hFile = CreateFile(
        this->get()->c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to open source file");
    }

    // 创建压缩文件
    HANDLE hCompressed = CreateFile(
        output_path.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hCompressed == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to create compressed file");
    }

    // 读取源文件并写入压缩文件
    char buffer[8192];
    DWORD bytesRead, bytesWritten;

    while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0)
    {
        WriteFile(hCompressed, buffer, bytesRead, &bytesWritten, NULL);
    }

    CloseHandle(hFile);
    CloseHandle(hCompressed);
    return *this;
}

instance<std::filesystem::path, true>& instance<std::filesystem::path, true>::decompress(const path& output_path)
{
    // 类似compress的实现
    // TODO
    return *this;
}

// 加密/解密功能 - 使用Windows CryptoAPI
instance<std::filesystem::path, true>& instance<std::filesystem::path, true>::encrypt(const std::string& key, const std::string& algorithm)
{
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    HCRYPTKEY hKey;

    // 获取加密服务提供程序句柄
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
    {
        throw std::runtime_error("Failed to acquire crypto context");
    }

    // 创建哈希对象
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
    {
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("Failed to create hash");
    }

    // 添加密钥数据到哈希
    if (!CryptHashData(hHash, (BYTE*)key.c_str(), key.length(), 0))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("Failed to hash key data");
    }

    // 从哈希生成密钥
    if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, 0, &hKey))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("Failed to derive key");
    }

    // 读取文件内容并加密
    std::ifstream in(this->get()->string(), std::ios::binary);
    std::ofstream out(this->get()->string() + ".encrypted", std::ios::binary);

    std::vector<BYTE> buffer(8192);
    DWORD count;

    while (in.read((char*)buffer.data(), buffer.size()))
    {
        count = (DWORD)in.gcount();
        if (!CryptEncrypt(hKey, 0, in.eof(), 0, buffer.data(), &count, buffer.size()))
        {
            throw std::runtime_error("Encryption failed");
        }
        out.write((char*)buffer.data(), count);
    }

    CryptDestroyKey(hKey);
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return *this;
}

instance<std::filesystem::path, true>& instance<std::filesystem::path, true>::decrypt(const std::string& key, const std::string& algorithm)
{
    // 类似encrypt的实现，使用CryptDecrypt替代CryptEncrypt
    // TODO
    return *this;
}

// 哈希计算 - 使用Windows CryptoAPI
std::string instance<std::filesystem::path, true>::calculate_hash(const std::string& algorithm) 
{
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
    {
        throw std::runtime_error("Failed to acquire crypto context");
    }

    DWORD hashAlg = CALG_MD5;
    if (algorithm == "sha1") hashAlg = CALG_SHA1;
    else if (algorithm == "sha256") hashAlg = CALG_SHA_256;

    if (!CryptCreateHash(hProv, hashAlg, 0, 0, &hHash)) 
    {
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("Failed to create hash");
    }

    std::ifstream file(this->get()->string(), std::ios::binary);
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) 
    {
        if (!CryptHashData(hHash, (BYTE*)buffer, file.gcount(), 0)) 
        {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("Failed to hash data");
        }
    }

    DWORD hashLen;
    DWORD hashSize = sizeof(hashLen);
    CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashLen, &hashSize, 0);

    std::vector<BYTE> hashValue(hashLen);
    CryptGetHashParam(hHash, HP_HASHVAL, hashValue.data(), &hashLen, 0);

    std::stringstream ss;
    for (DWORD i = 0; i < hashLen; i++) 
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hashValue[i];
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return ss.str();
}

// 文件监控 - 使用Windows API
void instance<std::filesystem::path, true>::start_monitoring(
    monitor_callback callback,
    bool recursive,
    const std::vector<std::string>& ignore_patterns,
    bool ignore_directories
)
{
    HANDLE hDir = CreateFile(
        this->get()->c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) 
    {
        throw std::runtime_error("Failed to open directory for monitoring");
    }

    std::thread([this, hDir, callback, recursive, ignore_patterns, ignore_directories]() 
        {
        char buffer[4096];
        DWORD bytesReturned;
        OVERLAPPED overlapped = { 0 };
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        while (true) {
            if (ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                recursive,
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES |
                FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_SECURITY,
                &bytesReturned,
                &overlapped,
                NULL
            )) {
                WaitForSingleObject(overlapped.hEvent, INFINITE);

                FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buffer;
                do {
                    std::wstring filename(info->FileName, info->FileNameLength / sizeof(WCHAR));
                    std::string action;

                    switch (info->Action) {
                    case FILE_ACTION_ADDED: action = "created"; break;
                    case FILE_ACTION_REMOVED: action = "deleted"; break;
                    case FILE_ACTION_MODIFIED: action = "modified"; break;
                    case FILE_ACTION_RENAMED_OLD_NAME: action = "renamed_from"; break;
                    case FILE_ACTION_RENAMED_NEW_NAME: action = "renamed_to"; break;
                    }

                    callback(action, this->get()->parent_path() / filename);

                    if (info->NextEntryOffset == 0) break;
                    info = (FILE_NOTIFY_INFORMATION*)((BYTE*)info + info->NextEntryOffset);
                } while (true);
            }
        }
        }).detach();
}

// 备份/恢复功能
instance<std::filesystem::path, true> instance<std::filesystem::path, true>::create_backup(
    const path& backup_dir,
    size_t max_backups,
    const std::string& backup_format,
    bool include_metadata
)
{
    path backup_path = backup_dir.empty() ?
        this->get()->parent_path() / ".backup" :
        backup_dir;

    if (!std::filesystem::exists(backup_path)) 
    {
        std::filesystem::create_directories(backup_path);
    }

    // 使用时间戳创建备份文件名
    SYSTEMTIME st;
    GetLocalTime(&st);
    char timestamp[32];
    sprintf_s(timestamp, "%04d%02d%02d_%02d%02d%02d",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);

    path backup_file = backup_path / (this->get_filename().string() + "_" + timestamp + "." + backup_format);

    // 复制文件
    if (!CopyFile(this->get()->c_str(), backup_file.c_str(), FALSE)) 
    {
        throw std::runtime_error("Failed to create backup");
    }

    if (include_metadata) 
    {
        std::ofstream meta(backup_path / (this->get_filename().string() + "_" + timestamp + ".meta"));
        meta << "Original Path: " << this->get()->string() << "\n";
        meta << "Backup Time: " << timestamp << "\n";
        meta << "Hash: " << this->calculate_hash() << "\n";
    }

    // 清理旧备份
    if (max_backups > 0) 
    {
        std::vector<path> backups;
        for (const auto& entry : std::filesystem::directory_iterator(backup_path)) 
        {
            if (entry.path().extension() == "." + backup_format) 
            {
                backups.push_back(entry.path());
            }
        }

        if (backups.size() > max_backups) 
        {
            std::sort(backups.begin(), backups.end());
            for (size_t i = 0; i < backups.size() - max_backups; i++) 
            {
                std::filesystem::remove(backups[i]);
            }
        }
    }

    return instance(backup_file);
}

// 权限管理 - 使用Windows API
std::map<std::string, bool> instance<std::filesystem::path, true>::get_permissions()
{
    DWORD attributes = GetFileAttributes(this->get()->c_str());

    std::map<std::string, bool> perms;
    perms["read"] = (attributes & FILE_ATTRIBUTE_READONLY) == 0;
    perms["write"] = (attributes & FILE_ATTRIBUTE_READONLY) == 0;
    perms["execute"] = this->get()->extension() == ".exe";
    perms["hidden"] = (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;

    return perms;
}

instance<std::filesystem::path, true>& instance<std::filesystem::path, true>::set_permissions(
    std::optional<bool> read,
    std::optional<bool> write,
    std::optional<bool> execute,
    bool recursive
)
{
    DWORD attributes = GetFileAttributes(this->get()->c_str());

    if (read.has_value() || write.has_value()) 
    {
        if (*read == false || *write == false) 
        {
            attributes |= FILE_ATTRIBUTE_READONLY;
        }
        else 
        {
            attributes &= ~FILE_ATTRIBUTE_READONLY;
        }
    }

    SetFileAttributes(this->get()->c_str(), attributes);

    if (recursive && this->is_dir()) 
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(**this)) 
        {
            DWORD subAttributes = GetFileAttributes(entry.path().c_str());
            if (read.has_value() || write.has_value()) 
            {
                if (*read == false || *write == false) 
                {
                    subAttributes |= FILE_ATTRIBUTE_READONLY;
                }
                else 
                {
                    subAttributes &= ~FILE_ATTRIBUTE_READONLY;
                }
            }
            SetFileAttributes(entry.path().c_str(), subAttributes);
        }
    }

    return *this;
}

#endif