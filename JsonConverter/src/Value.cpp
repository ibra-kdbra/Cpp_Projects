#include "ccdJsonpp/Value.h"

#include <iostream>

using namespace std;

namespace ccd
{
namespace Json
{

Value::Value() : m_type ( Type::Null ) {}

Value::Value ( const int value ) : m_type ( Type::Int ), m_value ( value ) { }

Value::Value ( const double value ) : m_type ( Type::Double ), m_value ( value ) {}

Value::Value ( const bool value ) : m_type ( Type::Bool ), m_value ( value ) {}

Value::Value ( const std::string& value ) : m_type ( Type::String ), m_value ( value ) {}

Value::Value ( const char* value ) : m_type ( Type::String ), m_value {std::move(std::string{value})} {}

Value::Value ( const Array& array ) : m_type ( Type::Array ), m_value ( array ) {}

Value::Value ( const Object& obj ) : m_type ( Type::Object ), m_value ( obj ) {}

Value::Value ( std::string&& value ) : m_type ( Type::String ), m_value ( move ( value ) ) {}

Value::Value ( Array&& array ) : m_type ( Type::Array ), m_value ( move ( array ) ) {}

Value::Value ( Object&& obj ) : m_type ( Type::Object ), m_value ( move ( obj ) ) {}

Value::Value ( std::nullptr_t ) : m_type ( Type::Null ) {}

Value::Value ( const initializer_list<Value> list ) : m_type ( Type::Array ), m_value ( list ) {}

Value::Value ( const Value& other ) : 
	m_type { other.m_type },
	m_value { other.m_value }
{
}

Value::Value ( Value&& other ) :
	m_type { std::move ( other.m_type ) },
	m_value { std::move ( other.m_value ) }
{
}

Value& Value::operator = ( const Value& other )
{
	m_type = other.m_type;
	m_value = other.m_value;
	return *this;
}

Value& Value::operator = ( Value&& other )
{
	m_type = other.m_type;
	m_value = std::move ( other.m_value );
	return *this;
}

Value& Value::operator[] ( const std::string& key )
{
	if ( m_type == Type::Null ) {
		m_type = Type::Object;
	}

	if ( m_type != Type::Object )
		throw std::string ( "Item is no Object" );


	return std::get<Object>(m_value)[key];
}

const Value& Value::operator[] ( const std::string& key ) const
{
	if ( m_type != Type::Object )
		throw std::string ( "Item is no Object" );

	try {
		return std::get<Object>(m_value).at ( key );
	} catch ( const std::out_of_range& oor ) {
		//std::cerr << "Out of Range error (" << key << "): " << oor.what() << '\n';
		return *this;
	}

}

Value& Value::operator[] ( const int key )
{
	if ( m_type != Type::Array )
		throw std::string ( "Item is no Array" );
	return std::get<Array>(m_value)[key];
}

const Value& Value::operator[] ( const int key ) const
{
	if ( m_type != Type::Array )
		throw std::string ( "Item is no Array" );
	return std::get<Array>(m_value)[key];
}


Value::Type Value::type() const
{
	return m_type;
}

int Value::toInt() const
{
	return std::get<int>(m_value);
}

double Value::toDouble() const
{
	return std::get<double>(m_value);
}

const std::string& Value::toString() const
{
	return std::get<std::string>(m_value);
}

bool Value::toBool() const
{
	return std::get<bool>(m_value);
}

const Array& Value::toArray() const
{
	return std::get<Array>(m_value);
}

const Object& Value::toObject() const
{
	return std::get<Object>(m_value);
}

inline static void indent ( std::ostream& os, const int indentLvl )
{
	for ( auto indentIdx = 0u ; indentIdx < indentLvl; indentIdx++ ) {
		os << "\t";
	}
}

std::ostream& operator<< ( std::ostream& os, const Object& obj )
{
	static const int indentLvlIdx =  os.xalloc();
	auto& indentLvl = os.iword(indentLvlIdx);
	os << "{\n";
	indentLvl++;

	for ( auto item = obj.cbegin() ; item != obj.cend(); ++item ) {
		indent ( os, indentLvl );
		os << "\"" << item->first << "\":" << item->second;
		if ( std::next(item, 1) != obj.cend() )
			os << ",\n";
	}
	os << "\n";

	indentLvl--;
	indent ( os, indentLvl );
	os << "}";

	return os;
}

std::ostream & operator<< ( std::ostream & os, const Array& array )
{
	os << "[";
	for ( auto item = array.cbegin() ; item != array.cend(); ++item ) {
		os << *item;
		if ( std::next(item,1) != array.cend() ) {
			os << ", ";
		}
	}
	os << "]";

	return os;
}

std::ostream & operator<< ( std::ostream & os, const Value& val )
{
	switch ( val.type() ) {
	case Value::Type::Int:
		os << static_cast<int>(val);
		break;
	case Value::Type::Double:
		os << static_cast<double>(val);
		break;
	case Value::Type::Bool:
		os << ( static_cast<bool>(val)?"true":"false" );
		break;
	case Value::Type::String:
		os << "\"" << static_cast<const std::string&>(val) << "\"";
		break;
	case Value::Type::Object:
		os << static_cast<const Object&>(val);
		break;
	case Value::Type::Array:
		os << static_cast<const Array&>(val);
		break;
	case Value::Type::Null:
		os << "null";
		break;
	default:
		break;
	}
	return os;
}

}
}
