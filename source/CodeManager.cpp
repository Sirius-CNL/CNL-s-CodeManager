#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <cstdlib>

using namespace std;
using namespace std::filesystem;

// 获取codes目录路径
path get_codes_dir()
{
    wchar_t exe_path[MAX_PATH];
    GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
    path exe_dir = path(exe_path).parent_path();
    return exe_dir / "codes";
}

// 确保codes目录存在
void ensure_codes_dir_exists()
{
    path codes_dir = get_codes_dir();
    if (!exists(codes_dir))
    {
        create_directory(codes_dir);
    }
}

// 添加题目
void add_problem(const wstring &name, const wstring &source_path)
{
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();
    path dest_path = codes_dir / (name + L".cpp");

    if (exists(dest_path))
    {
        wcerr << L"错误：已存在相同名称的题目，请更换名称\n";
        return;
    }

    try
    {
        copy_file(source_path, dest_path);
        wcout << L"题目 '" << name << L"' 添加成功\n";
    }
    catch (const filesystem_error &e)
    {
        wcerr << L"错误：无法添加题目 - " << e.what() << L"\n";
    }
}

// 列出所有题目
void list_problems()
{
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();

    wcout << L"已有题目列表:\n";
    try
    {
        for (const auto &entry : directory_iterator(codes_dir))
        {
            if (entry.is_regular_file() && entry.path().extension() == L".cpp")
            {
                wstring filename = entry.path().stem();
                wcout << L"  - " << filename << L"\n";
            }
        }
    }
    catch (const filesystem_error &e)
    {
        wcerr << L"错误：无法读取题目列表 - " << e.what() << L"\n";
    }
}

// 打开题目
void start_problem(const wstring &name)
{
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();
    path file_path = codes_dir / (name + L".cpp");

    if (!exists(file_path))
    {
        wcerr << L"错误：题目 '" << name << L"' 不存在\n";
        return;
    }

    wstring command = L"start \"\" \"" + file_path.wstring() + L"\"";
    _wsystem(command.c_str());
}

// 删除题目
void delete_problem(const wstring& name) {
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();
    path file_path = codes_dir / (name + L".cpp");
    
    if (!exists(file_path)) {
        wcerr << L"错误：题目 '" << name << L"' 不存在\n";
        return;
    }
    
    try {
        if (remove(file_path)) {
            wcout << L"题目 '" << name << L"' 已成功删除\n";
        } else {
            wcerr << L"错误：无法删除题目 '" << name << L"'\n";
        }
    } catch (const filesystem_error& e) {
        wcerr << L"错误：删除题目时出错 - " << e.what() << L"\n";
    }
}

// 显示帮助信息
void show_help()
{
    wcout << L"\nCodeManager (CM) 使用说明:\n"
          << L"> CM add <题目名称> <源文件路径>  - 添加题目\n"
          << L"> CM list                         - 列出所有题目\n"
          << L"> CM del                          - 删除题目\n"
          << L"> CM start <题目名称>             - 打开题目\n"
          << L"> CM help                         - 显示帮助\n"
          << L"   -------------by CNL 2025.7------------   \n";
}

void show_about()
{
    wcout << L"\n关于 CodeManager (CM) 1.0 \n"
          << L"作者：CNL - LEGENDrd@163.com\n"
          << L"这是我自己做的一个小程序，可能会有一些bug。\n"
          << L"Gitee项目链接：https://gitee.com/legendrd/cnl\n"
          << L"Bug提交：https://gitee.com/legendrd/cnl/issues（我能改就会改的）\n"
          << L"关注 https://space.bilibili.com/3546767803026063?spm_id_from=333.1007.0.0\n"
          << L"2025.7.24\n";

}

int wmain(int argc, wchar_t *argv[])
{
    setlocale(LC_ALL, "");
    if (argc < 2)
    {
        show_help();
        return 1;
    }

    wstring command = argv[1];

    if (command == L"add" && argc == 4)
    {
        add_problem(argv[2], argv[3]);
    }
    else if (command == L"list" && argc == 2)
    {
        list_problems();
    }
    else if (command == L"start" && argc == 3)
    {
        start_problem(argv[2]);
    }
    else if (command == L"del" && argc == 3) 
    {
        delete_problem(argv[2]);
    }
    else if (command == L"help" && argc == 2)
    {
        show_help();
    }
    else if (command == L"about")
    {
        show_about();
    }
    else
    {
        wcerr << L"错误：无效的命令或参数\n";
        show_help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
