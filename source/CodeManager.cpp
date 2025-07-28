/*
Copyright (c) [Year] [name of copyright holder]
[Software Name] is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
*/

/*
28th. July 2025
Code Manager 1.1
添加内容：
搜索
对所有语言文件的支持
*/
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
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

// 获取语言映射文件路径
path get_lang_map_path()
{
    return get_codes_dir().parent_path() / "language_map.vk";
}

// 获取文件名映射文件路径
path get_name_map_path()
{
    return get_codes_dir().parent_path() / "name_map.vk";
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

// 加载语言映射
map<wstring, wstring> load_language_map()
{
    map<wstring, wstring> lang_map;
    path lang_map_path = get_lang_map_path();
    
    // 默认语言映射
    lang_map[L".cpp"] = L"C++";
    lang_map[L".hpp"] = L"C++";
    lang_map[L".c++"] = L"C++";
    lang_map[L".h++"] = L"C++";
    lang_map[L".cc"] = L"C++";
    lang_map[L".hh"] = L"C++";
    lang_map[L".c"] = L"C";
    lang_map[L".h"] = L"C";
    lang_map[L".java"] = L"Java";
    lang_map[L".jav"] = L"Java";
    lang_map[L".py"] = L"Python";
    lang_map[L".html"] = L"HTML";
    lang_map[L".htm"] = L"HTML";
    lang_map[L".js"] = L"JavaScript";
    lang_map[L".css"] = L"CSS";
    lang_map[L".bat"] = L"Batch";
    
    // 如果存在外部语言映射文件，则覆盖默认值
    if (exists(lang_map_path))
    {
        wifstream fin(lang_map_path);
        wstring ext, lang;
        while (fin >> ext >> lang)
        {
            lang_map[ext] = lang;
        }
    }
    
    return lang_map;
}

// 加载文件名映射
map<wstring, wstring> load_name_map()
{
    map<wstring, wstring> name_map;
    path name_map_path = get_name_map_path();
    
    if (exists(name_map_path))
    {
        wifstream fin(name_map_path);
        wstring name, filename;
        while (fin >> name >> filename)
        {
            name_map[name] = filename;
        }
    }
    
    return name_map;
}

// 保存文件名映射
void save_name_map(const map<wstring, wstring>& name_map)
{
    path name_map_path = get_name_map_path();
    wofstream fout(name_map_path);
    
    for (const auto& pair : name_map)
    {
        fout << pair.first << L" " << pair.second << endl;
    }
}

// 获取文件语言类型
wstring get_file_language(const wstring& filename, const map<wstring, wstring>& lang_map)
{
    path p(filename);
    wstring ext = p.extension();
    
    auto it = lang_map.find(ext);
    if (it != lang_map.end())
    {
        return it->second;
    }
    
    return ext.empty() ? L"未知扩展名" : ext;
}

// 添加题目
void add_problem(const wstring &name, const wstring &source_path)
{
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();
    path source_file_path(source_path);
    wstring ext = source_file_path.extension();
    path dest_path = codes_dir / (name + ext);

    if (exists(dest_path))
    {
        wcerr << L"错误：已存在相同名称的题目，请更换名称\n";
        return;
    }

    try
    {
        copy_file(source_path, dest_path);
        
        // 更新文件名映射
        auto name_map = load_name_map();
        name_map[name] = (name + ext);
        save_name_map(name_map);
        
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
    auto lang_map = load_language_map();
    auto name_map = load_name_map();

    wcout << L"已有题目列表:\n";
    try
    {
        for (const auto &entry : directory_iterator(codes_dir))
        {
            if (entry.is_regular_file())
            {
                wstring filename = entry.path().filename();
                wstring stem = entry.path().stem();
                
                // 查找显示名称（如果有）
                wstring display_name = stem;
                for (const auto& pair : name_map)
                {
                    if (pair.second == filename)
                    {
                        display_name = pair.first;
                        break;
                    }
                }
                
                wstring lang = get_file_language(filename, lang_map);
                wcout << L"  - " << display_name << L" (" << lang << L")\n";
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
    auto name_map = load_name_map();
    
    // 查找实际文件名
    wstring filename;
    auto it = name_map.find(name);
    if (it != name_map.end())
    {
        filename = it->second;
    }
    else
    {
        // 如果没有找到映射，尝试直接匹配
        for (const auto &entry : directory_iterator(codes_dir))
        {
            if (entry.is_regular_file() && entry.path().stem() == name)
            {
                filename = entry.path().filename();
                break;
            }
        }
        
        if (filename.empty())
        {
            wcerr << L"错误：题目 '" << name << L"' 不存在\n";
            return;
        }
    }
    
    path file_path = codes_dir / filename;
    wstring command = L"start \"\" \"" + file_path.wstring() + L"\"";
    _wsystem(command.c_str());
}

// 删除题目
void delete_problem(const wstring& name) 
{
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();
    auto name_map = load_name_map();
    
    // 查找实际文件名
    wstring filename;
    auto it = name_map.find(name);
    if (it != name_map.end())
    {
        filename = it->second;
        name_map.erase(it);
        save_name_map(name_map);
    }
    else
    {
        // 如果没有找到映射，尝试直接匹配
        for (const auto &entry : directory_iterator(codes_dir))
        {
            if (entry.is_regular_file() && entry.path().stem() == name)
            {
                filename = entry.path().filename();
                break;
            }
        }
    }
    
    if (filename.empty())
    {
        wcerr << L"错误：题目 '" << name << L"' 不存在\n";
        return;
    }
    
    path file_path = codes_dir / filename;
    
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

// 搜索题目
void search_problems(const wstring& keyword)
{
    ensure_codes_dir_exists();
    path codes_dir = get_codes_dir();
    auto lang_map = load_language_map();
    auto name_map = load_name_map();

    wcout << L"搜索结果:\n";
    try
    {
        bool found = false;
        for (const auto &entry : directory_iterator(codes_dir))
        {
            if (entry.is_regular_file())
            {
                wstring filename = entry.path().filename();
                wstring stem = entry.path().stem();
                wstring lower_stem = stem;
                wstring lower_keyword = keyword;
                
                // 转换为小写以便不区分大小写比较
                transform(lower_stem.begin(), lower_stem.end(), lower_stem.begin(), ::tolower);
                transform(lower_keyword.begin(), lower_keyword.end(), lower_keyword.begin(), ::tolower);
                
                if (lower_stem.find(lower_keyword) != wstring::npos)
                {
                    found = true;
                    // 查找显示名称（如果有）
                    wstring display_name = stem;
                    for (const auto& pair : name_map)
                    {
                        if (pair.second == filename)
                        {
                            display_name = pair.first;
                            break;
                        }
                    }
                    
                    wstring lang = get_file_language(filename, lang_map);
                    wcout << L"  - " << display_name << L" (" << lang << L")\n";
                }
            }
        }
        
        if (!found)
        {
            wcout << L"  未找到包含 '" << keyword << L"' 的题目\n";
        }
    }
    catch (const filesystem_error &e)
    {
        wcerr << L"错误：搜索题目时出错 - " << e.what() << L"\n";
    }
}

// 显示帮助信息
void show_help()
{
    wcout << L"\nCodeManager (CM) 使用说明:\n"
          << L"> CM add <题目名称> <源文件路径>  - 添加题目\n"
          << L"> CM list                         - 列出所有题目\n"
          << L"> CM del <题目名称>               - 删除题目\n"
          << L"> CM start <题目名称>             - 打开题目\n"
          << L"> CM search <搜索字段>            - 搜索题目\n"
          << L"> CM help                         - 显示帮助\n"
          << L"> CM about                        - 显示关于信息\n"
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
    else if (command == L"about" && argc == 2)
    {
        show_about();
    }
    else if (command == L"search" && argc == 3)
    {
        search_problems(argv[2]);
    }
    else
    {
        wcerr << L"错误：无效的命令或参数\n";
        show_help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}