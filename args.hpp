
/*
 * MIT License
 *
 * Copyright (c) 2026 magistermaks
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

class ProgramArguments {

	private:

		std::string m_program;
		std::vector<std::string> m_positional;
		std::unordered_map<std::string, std::string> m_flags;

		static std::optional<int> to_int(const std::string& str) {
			int result = 0;
			auto [ptr, ec] {std::from_chars(str.data(), str.data() + str.size(), result)};

			if (ec == std::errc()) {
				return result;
			}

			return std::nullopt;
		}

		static std::optional<bool> to_bool(const std::string& str) {
			if (str == "1" || str == "true" || str == "on" || str == "yes") return true;
			if (str == "0" || str == "false" || str == "off" || str == "no") return false;

			return std::nullopt;
		}

	public:

		ProgramArguments(int argc, const char* argv[]) {
			m_program = argv[0];
			std::string key;

			for (int i = 1; i < argc; i ++) {
				std::string arg = argv[i];
				bool is_flag = arg.starts_with("--");

				if (!key.empty()) {
					m_flags[key] = is_flag ? "" : arg;
					key = is_flag ? arg : "";
				} else if (is_flag) {
					key = arg.substr(2);
				} else {
					m_positional.push_back(arg);
				}
			}

			if (!key.empty()) {
				m_flags[key] = "";
			}
		}

		const std::string& program() const {
			return m_program;
		}

		const std::vector<std::string>& positional() const {
			return m_positional;
		}

		std::optional<std::string> get_string(const std::string& flag) const {
			auto it = m_flags.find(flag);

			if (it == m_flags.end()) {
				return std::nullopt;
			}

			return {it->second};
		}

		std::optional<int> get_int(const std::string& flag) const {
			return get_string(flag).and_then(to_int);
		}

		std::optional<bool> get_bool(const std::string& flag) const {
			return get_string(flag).and_then(to_bool);
		}

};