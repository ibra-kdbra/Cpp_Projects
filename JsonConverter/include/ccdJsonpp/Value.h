#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <initializer_list>

namespace ccd {
namespace Json {
	
class Value;

using Object = std::map<std::string,Value>;
using Array = std::vector<Value>;

class Value
{
public:
	enum class Type {
		Int = 0,
		Double,
		Bool,
		String,
		Object,
		Array,
		Null
	};

	Value();
	Value(const int value);
	Value(const double value);
	Value(const bool value);
	Value(const std::string& value);
	Value(const char* value);
	Value(const Array& array);
	Value(const Object& obj);
	Value(std::string&& value);
	Value(Array&& array);
	Value(Object&& obj);
	Value(std::nullptr_t np);
	Value(const std::initializer_list<Value> list);

	Value(const Value& other);
	Value(Value&& other);
	
	Value& operator=(const Value& other);
	Value& operator=(Value&& other);
	
	Value& operator[](const std::string& key);
	const Value& operator[](const std::string& key) const;
	Value& operator[](const int key);
	const Value& operator[](const int key) const;

	explicit operator double() const { return std::get<double>(m_value); }
	explicit operator int() const { return std::get<int>(m_value); }
	explicit operator bool() const { return std::get<bool>(m_value); }
	explicit operator const std::string& () const { return std::get<std::string>(m_value); }
	operator const Object& () const { return std::get<Object>(m_value); }
	operator const Array& () const { return std::get<Array>(m_value); }

	Type type() const;
	
	int toInt() const;
	double toDouble() const;
	const std::string& toString() const;
	bool toBool() const;
	
	const Array& toArray() const;
	const Object& toObject() const;
	
private:
	Type m_type = Type::Null;
	std::variant<int, double, bool, std::string, Object, Array> m_value {};

};

std::ostream& operator<<(std::ostream& os, const Value& val);
std::ostream& operator<<(std::ostream& os, const Object& val);
std::ostream& operator<<(std::ostream& os, const Array& val);


}
}
