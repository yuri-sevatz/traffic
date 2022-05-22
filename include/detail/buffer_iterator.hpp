#ifndef BUFFER_ITERATOR_HPP
#define BUFFER_ITERATOR_HPP

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace detail {

template<typename Parser>
class buffer_iterator {
public:
    typedef typename Parser::difference_type difference_type;
    typedef typename Parser::value_type value_type;
    typedef typename Parser::pointer pointer;
    typedef typename Parser::reference reference;
    typedef typename Parser::iterator_category iterator_category;

    buffer_iterator() = default;
    template<
        typename U,
        typename = std::enable_if_t<(
            std::is_constructible_v<Parser, U &&> &&
            !std::is_same_v<std::decay_t<U>, buffer_iterator>
        )>
    >
    buffer_iterator(U && value) : m_parser(std::forward<U>(value)) {}
    buffer_iterator(const buffer_iterator &) = default;
    buffer_iterator(buffer_iterator &&) = default;
    buffer_iterator & operator=(const buffer_iterator &) = default;
    buffer_iterator & operator=(buffer_iterator &&) = default;
    ~buffer_iterator() = default;

    reference operator*() const {
        return m_parser.dereference();
    }

    pointer operator->() const {
        return &operator*();
    }

    reference operator[](difference_type difference) {
        return buffer_iterator(*this).operator+=(difference).operator*();
    }

    buffer_iterator & operator++() {
        m_parser.increment();
        return *this;
    }
    buffer_iterator operator++(int) {
        const buffer_iterator iterator(*this);
        operator++();
        return iterator;
    }

    buffer_iterator & operator--() {
        m_parser.decrement();
        return *this;
    }
    buffer_iterator operator--(int) {
        const buffer_iterator iterator(*this);
        operator--();
        return iterator;
    }

    buffer_iterator & operator+=(difference_type difference) {
        m_parser.advance(difference);
        return *this;
    }

    buffer_iterator & operator-=(difference_type difference) {
        m_parser.advance(difference);
        return *this;
    }

    friend buffer_iterator operator+(const buffer_iterator & iterator, difference_type difference) {
        return buffer_iterator(iterator) += difference;
    }
    friend buffer_iterator operator+(difference_type difference, const buffer_iterator & iterator) {
        return buffer_iterator(iterator) += difference;
    }
    friend buffer_iterator operator-(const buffer_iterator & iterator, difference_type difference) {
        return buffer_iterator(iterator) -= difference;
    }
    friend difference_type operator-(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_event - rhs.m_event;
    }

    friend bool operator==(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_parser == rhs.m_parser;
    }
    friend bool operator!=(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_parser != rhs.m_parser;
    }
    friend bool operator<(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_parser < rhs.m_parser;
    }
    friend bool operator>(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_parser > rhs.m_parser;
    }
    friend bool operator<=(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_parser <= rhs.m_parser;
    }
    friend bool operator>=(const buffer_iterator & lhs, const buffer_iterator & rhs) {
        return lhs.m_parser >= rhs.m_parser;
    }

private:
    Parser m_parser;
};

} // namespace detail

#endif // BUFFER_ITERATOR_HPP
