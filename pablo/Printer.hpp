#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <cstddef>
#include <iostream>
#include <ostream>
#include <print>
#include <string>
#include <string_view>
#include <utility>

// clang-format off
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #define NODISCARD [[nodiscard]]
#else
    #define NODISCARD
#endif

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #define MAYBE_UNUSED [[maybe_unused]]
#else
    #define MAYBE_UNUSED
#endif

#if defined(_MSC_VER)
    #define NOALIAS __restrict
#elif defined(__GNUC__) || defined(__clang__)
    #define NOALIAS __restrict__
#else
    #define NOALIAS
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define PURE __attribute__((pure))
#else
    #define PURE
#endif

#if defined(__cplusplus) && __cplusplus >= 202002L
    #define LIKELY [[likely]]
#else
    #define LIKELY
#endif

#if defined(__cplusplus) && __cplusplus >= 201402L
    #define CONSTEXPR constexpr
#else
    #define CONSTEXPR
#endif
// clang-format on

// MARK: Printer class

/// A simple class to provide a friendly wrapper for streams.
template<class Stream>
class Printer
{
  private:
    std::string str_;
    Stream* NOALIAS thisStream_ = nullptr;

  public:
    // MARK: key methods

    /// Initialized with `text` and `std::cout`.
    CONSTEXPR explicit Printer<Stream>(std::string text) noexcept
      : str_(std::move(text)),
        thisStream_(&std::cout) {}

    /// Initialized with `stream` (`std::cout` by default) and no text.
    CONSTEXPR explicit Printer<Stream>(Stream& NOALIAS stream = std::cout) noexcept
      : thisStream_(&stream) {}

    CONSTEXPR Printer<Stream>(Printer<Stream> const & NOALIAS) = default;
    CONSTEXPR Printer<Stream>(Printer<Stream>&& NOALIAS) = default;
    CONSTEXPR auto operator= (Printer<Stream> const &) -> Printer<Stream>& = default;
    CONSTEXPR auto operator= (Printer<Stream>&&) -> Printer<Stream>& = default;

    CONSTEXPR ~Printer<Stream>() {
        show();
    };

    /// Initialized with `text` and `stream`.
    CONSTEXPR Printer<Stream>(std::string text, Stream& NOALIAS stream) noexcept
      : str_(std::move(text)),
        thisStream_(&stream) {}

    // MARK: rest

    /// Replace for a new stream. Returns the address of the previous one,
    /// which might be `nullptr` if there was no stream.
    CONSTEXPR auto set_stream(Stream& NOALIAS stream) noexcept -> Stream* NOALIAS {
        Stream* NOALIAS const res = thisStream_;
        thisStream_ = &stream;
        return res;
    }

    /// Create a copy of the current Printer with a different stream, but the same text.
    NODISCARD PURE CONSTEXPR auto with_stream(Stream& NOALIAS stream) const noexcept
      -> Printer<Stream> {
        return {str_, stream};
    }

    /// Removes all accumulated text. Returns its size.
    CONSTEXPR auto clear() noexcept -> size_t {
        size_t const res = str_.size();
        str_.clear();
        return res;
    }

    /// Removes `numberOfElements` elements from the end of the accumulated text. If less
    /// elements than expected, the Printer will remain empty. Returns the size of the
    /// text after removing elements.
    CONSTEXPR auto pop(size_t numberOfElements = 1) noexcept -> size_t {
        for (size_t i = 0; i < numberOfElements && !this->empty(); ++i) {
            str_.pop_back();
        }
        return str_.size();
    }

    /// Reserve some memory for the text. Returns the new capacity.
    CONSTEXPR auto reserve(size_t requestedCapacity) noexcept -> size_t {
        str_.reserve(requestedCapacity);
        return str_.capacity();
    }

    /// Whether the Printer is empty.
    NODISCARD PURE CONSTEXPR auto empty() const noexcept -> bool {
        return str_.empty();
    }

    /// The size of the text in the Printer.
    NODISCARD PURE CONSTEXPR auto size() const noexcept -> size_t {
        return str_.size();
    }

    /// The same as writing to this Printer.
    template<typename CanBePrinted>
    CONSTEXPR auto operator<< (CanBePrinted const & NOALIAS text) noexcept
      -> Printer<Stream>& NOALIAS {
        write(text);
        return *this;
    }

    /// The same as writing to this Printer.
    template<typename CanBePrinted>
    CONSTEXPR auto operator<< (CanBePrinted const && NOALIAS text) noexcept
      -> Printer<Stream>& NOALIAS {
        write(text);
        return *this;
    }

    NODISCARD PURE CONSTEXPR explicit operator std::string() const noexcept {
        return {str_};
    }

    NODISCARD PURE CONSTEXPR explicit operator std::string_view() const noexcept {
        return {str_};
    }

    NODISCARD PURE CONSTEXPR auto begin() const noexcept -> std::string::const_iterator {
        return str_.begin();
    }

    NODISCARD PURE CONSTEXPR auto end() const noexcept -> std::string::const_iterator {
        return str_.end();
    }

    NODISCARD CONSTEXPR auto begin() noexcept -> std::string::iterator {
        return str_.begin();
    }

    NODISCARD CONSTEXPR auto end() noexcept -> std::string::iterator {
        return str_.end();
    }

    NODISCARD PURE auto get_buffered_text() const noexcept -> std::string const & {
        return str_;
    }

    NODISCARD auto get_buffered_text() noexcept -> std::string& {
        return str_;
    }

    // MARK: write

    /// Adds text to output. Returns the size of the text after writing.
    template<typename CanBePrinted>
    CONSTEXPR auto write(CanBePrinted const & NOALIAS text) noexcept -> size_t {
        str_.append(text);
        return str_.size();
    }

    /// Adds text to output. Returns the size of the text after writing.
    template<typename CanBePrinted>
    CONSTEXPR auto write(CanBePrinted const && NOALIAS text) noexcept -> size_t {
        str_.append(text);
        return str_.size();
    }

    /// Adds text to output. Returns the size of the text after writing.
    CONSTEXPR auto write(unsigned long long text) noexcept -> size_t {
        str_.append(std::to_string(text));
        return str_.size();
    }

    /// Adds text to output. Returns the size of the text after writing.
    CONSTEXPR auto write(long long text) noexcept -> size_t {
        str_.append(std::to_string(text));
        return str_.size();
    }

    /// Adds text to output. Returns the size of the text after writing.
    CONSTEXPR auto write(char text) noexcept -> size_t {
        str_.push_back(text);
        return str_.size();
    }

    /// Adds text to output with a new line. Returns the size of the text after writing.
    template<typename CanBePrinted>
    CONSTEXPR auto writeln(CanBePrinted const & NOALIAS text) noexcept -> size_t {
        this->write(text);
        return this->write('\n');
    }

    /// Adds text to output with a new line. Returns the size of the text after writing.
    template<typename CanBePrinted>
    CONSTEXPR auto writeln(CanBePrinted const && NOALIAS text) noexcept -> size_t {
        this->write(text);
        return this->write('\n');
    }

    /// Adds new line to output. Returns the size of the text after writing.
    CONSTEXPR auto writeln() noexcept -> size_t {
        return this->write('\n');
    }

    // MARK: print

    /// Adds text and flushes. Returns the result of flushing.
    template<typename CanBePrinted>
    CONSTEXPR auto print(CanBePrinted const & NOALIAS text) noexcept -> size_t {
        write(text);
        return flush();
    }

    /// Adds text and flushes. Returns the result of flushing.
    template<typename CanBePrinted>
    CONSTEXPR auto print(CanBePrinted const && NOALIAS text) noexcept -> size_t {
        write(text);
        return flush();
    }

    /// Adds text, new line and flushes. Returns the result of flushing.
    template<typename CanBePrinted>
    CONSTEXPR auto println(CanBePrinted const & NOALIAS text) noexcept -> size_t {
        this->write(text);
        return print('\n');
    }

    /// Adds text, new line and flushes. Returns the result of flushing.
    template<typename CanBePrinted>
    CONSTEXPR auto println(CanBePrinted const && NOALIAS text) noexcept -> size_t {
        this->write(text);
        return print('\n');
    }

    /// Adds new line and flushes. Returns the result of flushing.
    CONSTEXPR auto println() noexcept -> size_t {
        return print('\n');
    }

    // MARK: show

    /// Sends the accumulated text to the given stream (such as `std::cout`). Returns the
    /// size of the text shown.
    CONSTEXPR auto show(MAYBE_UNUSED Stream& NOALIAS stream) const noexcept -> size_t {
        // clang-format off
#if __cplusplus >= 202302L
        std::print("{}", str_);
#else
        stream << str_;
#endif
        return str_.size();
        // clang-format on
    }

    /// Sends the accumulated text to the given stream (such as `std::cout`). Returns the
    /// size of the text shown.
    CONSTEXPR auto show(MAYBE_UNUSED Stream&& NOALIAS stream) const noexcept -> size_t {
        // clang-format off
#if __cplusplus >= 202302L
        std::print("{}", str_);
#else
        stream << str_;
#endif
        return str_.size();
        // clang-format on
    }

    /// Sends the accumulated text to output. Returns the size of the text shown. It is
    /// important that there is a stream, undefined behavior otherwise.
    CONSTEXPR void show() const noexcept {
        show(*thisStream_);
    }

    /// Sends the accumulated text to output. Returns at `size` the size of the text
    /// shown. It is important that there is a stream, undefined behavior otherwise.
    CONSTEXPR auto show(size_t* NOALIAS const size) const noexcept -> size_t {
        size_t const res = show(*thisStream_);
        if (size != nullptr)
            LIKELY {
                *size = res;
            }
        return res;
    }

    // MARK: flush

    /// Sends the accumulated text to the given stream (such as `std::cout`), and leaves
    /// this Printer empty. Returns the size of the whole text.
    CONSTEXPR auto flush(Stream& NOALIAS stream) noexcept -> size_t {
        size_t const res = show(stream);
        clear();
        return res;
    }

    /// Sends the accumulated text to output, and leaves the Printer empty. Returns the
    /// size of the whole text. It is important that there is a stream, undefined
    /// behavior otherwise.
    CONSTEXPR auto flush() noexcept -> size_t {
        return flush(*thisStream_);
    }

    /// Creates a Printer with `std::cout` as stream.
    NODISCARD
    static auto out() noexcept -> Printer<std::ostream> {
        return Printer<std::ostream>{std::cout};
    }
};

using PrinterOut = Printer<std::ostream>;

/// The overload of `operator<<` for some Printer.
inline auto operator<< (std::ostream& NOALIAS stream,
                        PrinterOut const & NOALIAS printer) noexcept -> std::ostream& {
    printer.show(stream);
    return stream;
}

#define NEW_OUT(name) PrinterOut name = PrinterOut::out(); // Created from macro NEW_OUT

#endif
