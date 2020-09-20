#pragma once

#include <utility>

namespace GOAT
{
    template <typename T>
    class optional
    {
    public:
        bool HasValue() const { return m_has_value; }
        T Value() const { return m_value; }

        bool operator==(T const & rhs) const { return m_has_value && m_value == rhs; }
        bool operator!=(T const & rhs) const { return !(this == rhs); }

        optional<T>& operator=(T const & rhs)
        {
            m_value = rhs;
            m_has_value = true;
            return *this;
        }

        optional<T>& operator=(T const && rhs)
        {
            m_value = std::move(rhs);
            m_has_value = true;
            return *this;
        }

    private:
        bool m_has_value = false;
        T m_value;
    };
}