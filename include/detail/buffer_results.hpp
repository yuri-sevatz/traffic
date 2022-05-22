#ifndef BUFFER_RESULTS_HPP
#define BUFFER_RESULTS_HPP

#include <cstddef>
#include <limits>
#include <utility>

namespace detail {

template<typename Iterator>
class buffer_results {
public:
    typedef typename Iterator::value_type value_type;
    typedef std::size_t size_type;

    typedef value_type & reference;
    typedef const value_type & const_reference;

    typedef Iterator iterator;
    typedef iterator const_iterator;

    buffer_results() = default;
    buffer_results(
        const const_iterator & begin,
        const const_iterator & end
    ) : m_value(begin, end) {}
    buffer_results(const buffer_results &) = default;
    buffer_results(buffer_results &&) = default;
    buffer_results & operator=(const buffer_results &) = default;
    buffer_results & operator=(buffer_results &&) = default;
    ~buffer_results() = default;

    const_iterator begin() const {
        return m_value.first;
    }

    const_iterator cbegin() const {
        return m_value.first;
    }

    const_iterator end() const {
        return m_value.second;
    }

    const_iterator cend() const {
        return m_value.second;
    }

    bool empty() const {
        return m_value.first == m_value.second;
    }

    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    void swap(buffer_results & results) {
        m_value.swap(results.m_value);
    }

private:
    std::pair<const_iterator, const_iterator> m_value;
};

} // namespace detail

#endif // BUFFER_RESULTS_HPP
