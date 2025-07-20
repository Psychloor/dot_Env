//
// Created by blomq on 2025-07-20.
//

#ifndef ENV_HPP
#define ENV_HPP

#include <complex>
#include <optional>
#include <string>
#include <unordered_map>

namespace dot_env
{
    class env
    {
    public:
        env() = default;

        /**
         * Loads environment variables from a specified file.
         *
         * This function searches for the specified file in the current working
         * directory. If the file is found, it reads and parses the environment
         * variables contained within the file. The variables are stored
         * internally and injected into the system's environment variables.
         *
         * @param filename The name of the file to load environment variables
         * from. If not specified, defaults to ".env".
         * @param override_system If provided, overrides the compile-time default.
         *        If not provided, uses the value set by DOT_ENV_OVERRIDE_SYSTEM.
         * @return Returns true if the file was successfully found and parsed,
         *         otherwise returns false.
         */
        bool load_env(std::string_view filename = ".env",
                      std::optional<bool> override_system = std::nullopt);

        /**
         * Retrieves the value of the specified environment variable.
         *
         * This function checks both internal storage of loaded environment
         * variables and the system's environment variables for the given key.
         * If a match is found, the value of the variable is returned.
         *
         * @param key The name of the environment variable to retrieve.
         * @return Returns an optional containing the value of the environment
         *         variable if found; otherwise, returns std::nullopt.
         */
        std::optional<std::string> get(const std::string_view& key);

        template <typename T>
            requires std::is_arithmetic_v<T>
        /**
         * Retrieves the value of an environment variable and converts it to the
         * specified arithmetic type, assuming little-endian byte order.
         *
         * This function checks both the internal storage of loaded environment
         * variables and the system's environment variables for the given key.
         * If a match is found, it attempts to parse and convert the variable's
         * value to the specified type. The byte order of the returned value is
         * adjusted to little-endian if necessary.
         *
         * @tparam T The arithmetic type to which the environment variable's
         * value will be converted (e.g., int, float, double).
         * @param key The name of the environment variable to retrieve and
         * convert.
         * @return Returns an optional containing the converted value of the
         * environment variable if successful; otherwise, returns std::nullopt.
         */
        std::optional<T> get_le(const std::string_view& key);

        template <class T>
            requires std::is_arithmetic_v<T>
        /**
         * Retrieves the value of an environment variable and converts it to the
         * specified arithmetic type, assuming big-endian byte order.
         *
         * This function checks both the internal storage of loaded environment
         * variables and the system's environment variables for the given key.
         * If a match is found, it attempts to parse and convert the variable's
         * value to the specified type. The byte order of the returned value is
         * adjusted to big-endian if necessary.
         *
         * @tparam T The arithmetic type to which the environment variable's
         * value will be converted (e.g., int, float, double).
         * @param key The name of the environment variable to retrieve and
         * convert.
         * @return Returns an optional containing the converted value of the
         * environment variable if successful; otherwise, returns std::nullopt.
         */
        std::optional<T> get_be(const std::string_view& key);

        std::string require(std::string_view key);


    private:
        void parse_env_file(const std::filesystem::path& path,
                            bool override_system);


        std::unordered_map<std::string, std::string> env_vars_ = {};
    };

    template <typename T>
        requires std::is_arithmetic_v<T>
    std::optional<T> env::get_le(const std::string_view& key)
    {
        const auto val = get(key);
        if (!val.has_value())
            return std::nullopt;

        T value;
        auto [ptr, ec] =
            std::from_chars(val->data(), val->data() + val->size(), value);
        if (ec != std::errc{})
            return std::nullopt;

        if constexpr (std::endian::native == std::endian::little)
        {
            // ReSharper disable once CppDFAUnreachableCode
            return value;
        }
        else
        {
            // ReSharper disable once CppDFAUnreachableCode
            return std::byteswap(value);
        }
    }

    template <typename T>
        requires std::is_arithmetic_v<T>
    std::optional<T> env::get_be(const std::string_view& key)
    {
        const auto val = get(key);
        if (!val.has_value())
            return std::nullopt;

        T value;
        auto [ptr, ec] =
            std::from_chars(val->data(), val->data() + val->size(), value);
        if (ec != std::errc{})
            return std::nullopt;

        if constexpr (std::endian::native == std::endian::big)
        {
            // ReSharper disable once CppDFAUnreachableCode
            return value;
        }
        else
        {
            // ReSharper disable once CppDFAUnreachableCode
            return std::byteswap(value);
        }
    }

} // namespace dot_env

#endif // ENV_HPP
