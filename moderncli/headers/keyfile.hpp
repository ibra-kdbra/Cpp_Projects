// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef KEYFILE_HPP_
#define KEYFILE_HPP_

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <string>
#include <list>

namespace tycho {
class keyfile {
public:
    using keys = std::unordered_map<std::string, std::string>;

    explicit inline keyfile(const std::initializer_list<std::string>& paths) noexcept :
    ptr_(std::make_shared<keyfile::data>()) {
        for(const auto& path : paths)
            ptr_->load(path);
    }

    inline keyfile() noexcept :
    ptr_(std::make_shared<keyfile::data>()) {};

    inline auto operator[](const std::string& id) -> keys& {
        return ptr_->fetch(id);
    }

    inline auto operator[](const std::string& id) const -> keys {
        return ptr_->fetch(id);
    }

    inline auto at(const std::string& id = "_") const -> keys {
        return ptr_->fetch(id);
    }

    inline auto exists(const std::string& id = "_") const -> bool {
        return ptr_ ? ptr_->exists(id) : false;
    }

    inline auto operator!() const -> bool {
        return !ptr_;
    }

    inline void remove(const std::string& id) {
        if(ptr_) ptr_->remove(id);
    }

    inline auto load(const std::string& path) -> bool {
        return ptr_ ? ptr_->load(path) : false;
    }

    inline void clear() {
        if(ptr_)
            ptr_.reset();
    }

    inline auto flatten(const std::string& id = "_") const {
        std::string result;
        auto list = ptr_->fetch(id);
        for(const auto& [key, value] : list)
            result += key + "='" + value + "' ";
        return result;
    }

    inline auto write(const std::string& path) const -> bool {
        return ptr_ ? ptr_->save(path) : false;
    }

    inline auto empty() const -> bool {
        return !ptr_ || ptr_->empty();
    }

private:
    class data final {
    public:
        data() = default;
        data(data const&) = delete;
        auto operator=(const data&) -> data& = delete;

        inline auto empty() const -> bool {
            return sections.empty();
        }

        inline auto exists(const std::string& id) const -> bool {
            return sections.count(id) > 0;
        }

        inline void remove(const std::string& id) {
            sections.erase(id);
        }

        inline auto fetch(const std::string& id) -> keys& {
            return sections[id];
        }

        inline auto load(const std::string& path) -> bool {
            const std::string_view whitespace(" \t\n\r");
            std::ifstream file(path);
            if(!file.is_open())
                return false;

            std::string buffer;
            std::string section = "_";

            while(std::getline(file, buffer)) {
                auto input = std::string_view(buffer);
                auto begin = input.find_first_not_of(whitespace);
                if(begin == std::string::npos)
                    continue;

                input.remove_prefix(begin);
                auto end = input.find_last_not_of(whitespace);
                if(end != std::string::npos)
                    input.remove_suffix(input.size() - end - 1);

                if(!input.empty() && input[0] == '[' && input.back() == ']') {
                    section = input.substr(1, input.size() - 2);
                    continue;
                }

                if(input.empty() || !isalnum(input[0]))
                    continue;

                auto pos = input.find_first_of('=');
                if(pos < 1 || pos == std::string::npos)
                    continue;

                auto key = input.substr(0, pos);
                auto value = input.substr(++pos);
                end = key.find_last_not_of(whitespace);
                if(end != std::string::npos)
                    key.remove_suffix(key.size() - end - 1);

                pos = value.find_first_not_of(whitespace);
                value.remove_prefix(pos);
                sections[section][std::string(key)] = std::string(value);
            }
            return true;
        }

        inline auto save(const std::string& path) -> bool {
            std::ofstream out(path, std::ios::binary);
            if(!out.is_open())
                return false;
            for(auto const& [key, value] : sections["_"]) {
                if(!value.empty())
                    out << key << " = " << value << std::endl;
            }
            if(!sections["_"].empty())
                out << std::endl;
            for(auto const& [id, keys] : sections) {
                if(id == "_")
                    continue;
                if(keys.empty())
                    continue;
                out << "[" << id << "]" << std::endl;
                for(auto const& [key, value] : keys) {
                    if(!value.empty())
                        out << key << " = " << value << std::endl;
                }
                out << std::endl;
            }
            return true;
        }

    private:
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections;
    };
    std::shared_ptr<data> ptr_;
};
} // end namespace

/*!
 * Keyfile config file loader.  The parsed config file can be shared
 * over multiple threads.
 * \file keyfile.hpp
 */
#endif
