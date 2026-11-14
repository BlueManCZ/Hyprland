#include "LuaConfigString.hpp"

#include "../../../macros.hpp"

using namespace Config;
using namespace Config::Lua;

CLuaConfigString::CLuaConfigString(Config::STRING def, std::function<std::expected<void, std::string>(const std::string&)>&& validator) :
    m_default(def), m_data(def), m_validator(std::move(validator)) {
    ;
}

SParseError CLuaConfigString::parse(lua_State* s) {
    if (!lua_isstring(s, -1))
        return {.errorCode = PARSE_ERROR_BAD_TYPE, .message = "string type requires a string"};

    std::string str = lua_tostring(s, -1);

    // an empty string means the default. Where the default is empty as well, keep the
    // string as given: rule fields like a monitor's mirror read it without the marker.
    if (str.empty()) {
        m_data       = m_default == STRVAL_EMPTY ? "" : m_default;
        m_bSetByUser = true;
        return {.errorCode = PARSE_ERROR_OK};
    }

    if (m_validator) {
        auto res = m_validator(str);
        if (!res)
            return {.errorCode = PARSE_ERROR_BAD_VALUE, .message = res.error()};
    }

    m_data       = std::move(str);
    m_bSetByUser = true;

    return {.errorCode = PARSE_ERROR_OK};
}

const std::type_info* CLuaConfigString::underlying() {
    return &typeid(decltype(m_data));
}

void const* CLuaConfigString::data() {
    return &m_data;
}

std::string CLuaConfigString::toString() {
    return m_data;
}

void CLuaConfigString::push(lua_State* s) {
    if (m_data == STRVAL_EMPTY)
        lua_pushliteral(s, "");
    else
        lua_pushstring(s, m_data.c_str());
}

Config::STRING CLuaConfigString::asString() {
    return m_data;
}

const Config::STRING& CLuaConfigString::parsed() {
    return m_data;
}

void CLuaConfigString::reset() {
    m_data = m_default;
}
