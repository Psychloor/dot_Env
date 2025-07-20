//
// Created by blomq on 2025-07-20.
//

#include "../include/env.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace dot_env
{
    [[nodiscard]] inline bool equals_case_insensitive(const std::string_view a,
                                                      const std::string_view b)
    {
        if (a.length() != b.length())
        {
            return false;
        }

        return std::ranges::equal(
            std::begin(a), std::end(a), std::begin(b), std::end(b),
            [](const uint8_t lhs, const uint8_t rhs)
            { return std::tolower(lhs) == std::tolower(rhs); });
    }

    bool env::load_env(const std::string_view filename)
    {
        const auto current_path = std::filesystem::current_path();
        std::filesystem::path env_path;
        for (const auto& entry :
             std::filesystem::directory_iterator(current_path))
        {
            if (entry.is_regular_file())
            {
                if (!filename.empty() && entry.path().filename() == filename)
                {
                    env_path = entry.path();
                    break;
                }
            }
        }

        if (env_path.empty())
        {
            return false;
        }

        parse_env_file(env_path);
        return true;
    }

    std::optional<std::string> env::get(const std::string_view& key)
    {
        if (const auto it = env_vars_.find(std::string(key));
            it != env_vars_.end())
        {
            return it->second;
        }

#ifdef _WIN32
        char* buffer = nullptr;
        size_t size = 0;
        if (_dupenv_s(&buffer, &size, key.data()) == 0 && buffer != nullptr)
        {
            std::string value(buffer);
            free(buffer);
            if (!value.empty())
            {
                return value;
            }
        }
#else
        if (const char* value = std::getenv(key.data()))
        {
            if (*value != '\0')
            {
                return std::string(value);
            }
        }
#endif


        return std::nullopt;
    }

    std::string env::require(const std::string_view key)
    {
        if (const auto val = get(key))
            return *val;

        throw std::runtime_error("Required environment variable missing: " +
                                 std::string(key));
    }

    void env::parse_env_file(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open env file: " << path << '\n';
            return;
        }

        std::string line;

        while (std::getline(file, line))
        {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty() || line[0] == '#')
                continue;

            const size_t eq_pos = line.find('=');
            if (eq_pos == std::string::npos)
                continue;

            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(eq_pos + 1);

            // Remove surrounding whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            if (!value.empty() && value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.length() - 2);
            }

            if (key.empty() || value.empty())
            {
                std::cerr << "Invalid line in env file: " << line << std::endl;
                continue;
            }

            if (auto [it, inserted] = env_vars_.emplace(key, value); !inserted)
            {
                std::cerr << "Duplicate env key: " << key << ", overwriting.\n";
                it->second = value; // overwrite in the map too
            }

            // Inject into actual environment
#ifndef _WIN32
            const char* existing_env = std::getenv(key.c_str());
#else
            char* existing_env = nullptr;
            size_t _size;
            _dupenv_s(&existing_env, &_size, key.c_str());
#endif

            if (!existing_env || std::string(existing_env).empty())
            {
                // Inject only if not already set
#ifdef _WIN32
                _putenv_s(key.c_str(), value.c_str());
                if (existing_env)
                    free(existing_env);
#else
                setenv(key.c_str(), value.c_str(), 1);
#endif
            }
        }
    }
} // namespace dot_env
