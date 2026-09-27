#ifndef JSON_NODE_H
#define JSON_NODE_H

#include <nlohmann/json.hpp>
#include <string>

namespace Poke
{
    class JsonNode
    {
    public:
        JsonNode() = default;
        JsonNode(const nlohmann::json &j) : m_json(j) {}

        bool HasKey(const std::string &key) const { return m_json.contains(key); }

        JsonNode operator[](const std::string& key) const
        {
            return m_json.contains(key) ? JsonNode(m_json[key]) : JsonNode();
        }

        JsonNode operator[](size_t index) const
        {
            return (m_json.is_array() && index < m_json.size()) ? JsonNode(m_json[index]) : JsonNode();
        }

        template <typename T>
        T Get(const std::string &key, const T &defaultValue = T()) const
        {
            if (m_json.contains(key) && !m_json[key].is_null())
            {
                return m_json[key].get<T>();
            }
            return defaultValue;
        }

        template <typename T>
        void Set(const std::string &key, const T &value)
        {
            m_json[key] = value;
        }

        nlohmann::json &GetInternal() { return m_json; }
        const nlohmann::json &GetInternal() const { return m_json; }

    private:
        nlohmann::json m_json;
    };
}

#endif