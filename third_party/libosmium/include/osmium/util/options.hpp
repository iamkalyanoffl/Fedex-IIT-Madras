#ifndef OSMIUM_UTIL_OPTIONS_HPP
#define OSMIUM_UTIL_OPTIONS_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2016 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <cstddef>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>

namespace osmium {

    namespace util {

        /**
         * Stores key=value type options. This class can be used stand-alone or
         * as a base class. Options are stored and retrieved by key using the
         * different set() and get() methods.
         *
         * Both keys and values are stored as strings. The values "true",
         * "yes", "false", and "no" are interpreted as boolean values in some
         * functions.
         *
         * You can iterate over all set options. Dereferencing an iterator
         * yields a std::pair of the key and value strings.
         */
        class Options {

            using option_map = std::map<std::string, std::string>;
            option_map m_options;

        public:

            using iterator = option_map::iterator;
            using const_iterator = option_map::const_iterator;
            using value_type = option_map::value_type;

            /**
             * Construct empty option set.
             */
            Options() = default;

            /**
             * Construct option set from initializer list:
             * @code
             *   Options options{ { "foo", "true" }, { "bar", "17" } };
             * @endcode
             */
            explicit Options(const std::initializer_list<value_type>& values) :
                m_options(values) {
            }

            /**
             * Set option 'key' to 'value'.
             */
            void set(const std::string& key, const std::string& value) {
                m_options[key] = value;
            }

            /**
             * Set option 'key' to 'value'.
             */
            void set(const std::string& key, const char* value) {
                m_options[key] = value;
            }

            /**
             * Set option 'key' to 'value'.
             */
            void set(const std::string& key, bool value) {
                m_options[key] = value ? "true" : "false";
            }

            /**
             * Set option from string in the form 'key=value'. If the string
             * contains no equal sign, the whole string is the key and it will
             * be set to "true".
             */
            void set(std::string data) {
                const size_t pos = data.find_first_of('=');
                if (pos == std::string::npos) {
                    m_options[data] = "true";
                } else {
                    std::string value = data.substr(pos+1);
                    data.erase(pos);
                    set(data, value);
                }
            }

            /**
             * Get value of "key" option. If not set, the default_value (or
             * empty string) is returned.
             */
            std::string get(const std::string& key, const std::string& default_value="") const noexcept {
                const auto it = m_options.find(key);
                if (it == m_options.end()) {
                    return default_value;
                }
                return it->second;
            }

            /**
             * Is this option set to a true value ("true" or "yes")?
             * Will return false if the value is unset.
             */
            bool is_true(const std::string& key) const noexcept {
                const std::string value = get(key);
                return (value == "true" || value == "yes");
            }

            /**
             * Is this option not set to a false value ("false" or "no")?
             * Will return true if the value is unset.
             */
            bool is_not_false(const std::string& key) const noexcept {
                const std::string value = get(key);
                return !(value == "false" || value == "no");
            }

            /**
             * The number of options set.
             */
            size_t size() const noexcept {
                return m_options.size();
            }

            /**
             * Returns an iterator to the beginning.
             */
            iterator begin() noexcept {
                return m_options.begin();
            }

            /**
             * Returns an iterator to the end.
             */
            iterator end() noexcept {
                return m_options.end();
            }

            /**
             * Returns an iterator to the beginning.
             */
            const_iterator begin() const noexcept {
                return m_options.cbegin();
            }

            /**
             * Returns an iterator to the end.
             */
            const_iterator end() const noexcept {
                return m_options.cend();
            }

            /**
             * Returns an iterator to the beginning.
             */
            const_iterator cbegin() const noexcept {
                return m_options.cbegin();
            }

            /**
             * Returns a iterator to the end.
             */
            const_iterator cend() const noexcept {
                return m_options.cend();
            }

        }; // class Options

    } // namespace util

} // namespace osmium

#endif // OSMIUM_UTIL_OPTIONS_HPP
