#include <iostream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <initializer_list>

namespace pod
{
    template <typename CharType>
    class my_class
    {
    private:
        size_t length;
        size_t capacity;
        CharType *data;

        void allocate(size_t n)
        {
            if (n > capacity)
            {
                CharType *new_data = new CharType[n];
                if (data)
                {
                    for (size_t i = 0; i < length; ++i)
                    {
                        new_data[i] = data[i];
                    }
                    delete[] data;
                }
                data = new_data;
                capacity = n;
            }
        }

        static const size_t npos = -1;

        size_t find(const my_class &substr, size_t pos = 0)
        {
            if (substr.length > length)
                return npos;
            for (size_t i = pos; i <= length - substr.length; ++i)
            {
                bool match = true;
                for (size_t j = 0; j < substr.length; ++j)
                {
                    if (data[i + j] != substr.data[j])
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                {
                    return i;
                }
            }
            return npos;
        }

        void erase(size_t pos, size_t count)
        {
            if (pos + count > length)
            {
                throw std::out_of_range("Erase out of range");
            }
            for (size_t i = pos; i < length - count; ++i)
            {
                data[i] = data[i + count];
            }
            length -= count;
        }

        void insert(size_t pos, const my_class &str)
        {
            if (pos > length)
            {
                throw std::out_of_range("Insert out of range");
            }
            reserve(length + str.length);
            for (size_t i = length; i > pos; --i)
            {
                data[i + str.length - 1] = data[i - 1];
            }
            for (size_t i = 0; i < str.length; ++i)
            {
                data[pos + i] = str.data[i];
            }
            length += str.length;
        }

    public:
        my_class() : data(nullptr), length(0), capacity(0) {}

        my_class(size_t n) : length(n), capacity(n)
        {
            data = new CharType[n]{};
        }

        my_class(const CharType *str)
        {
            length = 0;
            while (str[length] != CharType{})
                ++length;

            capacity = length;
            data = new CharType[length];
            for (size_t i = 0; i < length; ++i)
            {
                data[i] = str[i];
            }
        }

        my_class(const my_class &other) : length(other.length), capacity(other.length)
        {
            data = new CharType[other.length];
            for (size_t i = 0; i < length; ++i)
            {
                data[i] = other.data[i];
            }
        }

        my_class(my_class &&other) noexcept : data(other.data), length(other.length), capacity(other.capacity)
        {
            other.data = nullptr;
            other.length = 0;
            other.capacity = 0;
        }

        ~my_class()
        {
            delete[] data;
        }

        my_class &operator=(const my_class &other)
        {
            if (this != &other)
            {
                delete[] data;
                length = other.length;
                capacity = other.capacity;
                data = new CharType[capacity];
                for (size_t i = 0; i < length; ++i)
                {
                    data[i] = other.data[i];
                }
            }
            return *this;
        }

        my_class &operator=(my_class &&other) noexcept
        {
            if (this != &other)
            {
                delete[] data;
                data = other.data;
                length = other.length;
                capacity = other.capacity;
                other.data = nullptr;
                other.length = 0;
                other.capacity = 0;
            }
            return *this;
        }

        void fill(CharType c, size_t n)
        {
            allocate(n);
            for (size_t i = 0; i < n; ++i)
            {
                data[i] = c;
            }
            length = n;
        }

        static my_class filled(CharType c, size_t n)
        {
            my_class result;
            result.fill(c, n);
            return result;
        }

        void fill(CharType c, size_t start_idx, size_t count)
        {
            if (start_idx + count > length)
            {
                throw std::out_of_range("Index out of range");
            }
            for (size_t i = start_idx; i < start_idx + count; ++i)
            {
                data[i] = c;
            }
        }

        bool empty() const
        {
            return length == 0;
        }

        size_t size() const
        {
            return length;
        }

        void reserve(size_t n)
        {
            allocate(n);
        }

        void resize(size_t n)
        {
            if (n > capacity)
            {
                allocate(n);
            }
            length = n;
        }

        void shrink_to_fit()
        {
            if (length < capacity)
            {
                CharType *new_data = new CharType[length];
                for (size_t i = 0; i < length; ++i)
                {
                    new_data[i] = data[i];
                }
                delete[] data;
                data = new_data;
                capacity = length;
            }
        }

        void clear()
        {
            length = 0;
        }

        my_class operator+(const my_class &other)
        {
            my_class result(length + other.length);
            for (size_t i = 0; i < length; ++i)
            {
                result.data[i] = data[i];
            }
            for (size_t i = 0; i < other.length; ++i)
            {
                result.data[length + i] = other.data[i];
            }
            result.length = length + other.length;
            return result;
        }

        my_class &operator+=(const my_class &other)
        {
            reserve(length + other.length);
            for (size_t i = 0; i < other.length; ++i)
            {
                data[length + i] = other.data[i];
            }
            length += other.length;
            return *this;
        }

        void append(const CharType *str)
        {
            size_t str_len = 0;
            while (str[str_len] != CharType{})
            {
                ++str_len;
            }
            reserve(length + str_len);
            for (size_t i = 0; i < str_len; ++i)
            {
                data[length + i] = str[i];
            }
            length += str_len;
        }

        void push_back(CharType c)
        {
            reserve(length + 1);
            data[length++] = c;
        }

        CharType &at(size_t idx)
        {
            if (idx >= length)
            {
                throw std::out_of_range("Index out of range");
            }
            return data[idx];
        }

        CharType &operator[](size_t idx)
        {
            return data[idx];
        }

        bool operator==(const my_class &other) 
        {
            if (length != other.length)
                return false;
            for (size_t i = 0; i < length; ++i)
            {
                if (data[i] != other.data[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const my_class &other) 
        {
            return !(*this == other);
        }

        bool operator<(const my_class &other) 
        {
            size_t min_len = length < other.length ? length : other.length;
            for (size_t i = 0; i < min_len; ++i)
            {
                if (data[i] != other.data[i])
                {
                    return data[i] < other.data[i];
                }
            }
            return length < other.length;
        }

        bool operator<=(const my_class &other)
        {
            return !(*this > other);
        }

        bool operator>(const my_class &other) 
        {
            return other < *this;
        }

        bool operator>=(const my_class &other)
        {
            return !(*this < other);
        }

        void replace(const my_class &old_str, const my_class &new_str)
        {
            size_t pos = 0;
            while ((pos = find(old_str, pos)) != npos)
            {
                erase(pos, old_str.length);
                insert(pos, new_str);
                pos += new_str.length;
            }
        }

        void replace(CharType old_char, CharType new_char)
        {
            for (size_t i = 0; i < length; ++i)
            {
                if (data[i] == old_char)
                {
                    data[i] = new_char;
                }
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const my_class &str)
        {
            for (size_t i = 0; i < str.length; ++i)
            {
                os << str.data[i];
            }
            return os;
        }
    };

    using mys = my_class<char>;
    using mys_16 = my_class<char16_t>;
    using mys_32 = my_class<char32_t>;
    using wmys = my_class<wchar_t>;
}


int main()
{
    pod::mys s1 = "Hello, world!";
    pod::mys s2 = "Helo, world!";

    s1.append("dfs");

    std::cout << s1 << std::endl;

    return 0;
}