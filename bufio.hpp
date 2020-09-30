#ifndef BUFIO_HPP_
#define BUFIO_HPP_

#include <string>

#include "resp.hpp"
#include "sockpp/tcp_acceptor.h"
#include "sockpp/tcp_connector.h"
#include "spdlog/spdlog.h"

#include "error.hpp"

#include <algorithm>

const int32_t MaxBufSize = 4096;
const int32_t MaxConsecutiveEmptyReads = 100;

const std::string ErrNegativeCount = "Negative count";
const std::string ErrNoProcess = "Multiple read calls return no data or error";
const std::string ErrFillFullBuffer = "Tried to fill full buffer";
const std::string ErrNegativeRead = "Reader returned negative count from read";

namespace bufio {

template <size_t Size>
class BufReader {
public:
    explicit BufReader(sockpp::tcp_socket* sock): sock_(sock) {
        reset(); 
    }
    explicit BufReader(sockpp::tcp_connector* sock) {
        sock_ = dynamic_cast<sockpp::tcp_socket*>(sock);
        reset();
    }

public:

    void fill();

    void reset();

    std::string read_err();

    std::string peek(size_t n);

    size_t discard(size_t n);

    size_t buffered();

    char read_byte();

    std::pair<char *, char *> read_until(char delim);

    std::pair<char *, char *> readline();

    std::pair<char *, char *> read_n(size_t n);

    bool read_n(size_t n, std::string &buf);

    size_t write(const std::string& str) {
        return sock_->write(str);
    }

    size_t write(char* buf, size_t n) {
        return sock_->write(buf, n); 
    }


private:
    sockpp::tcp_socket* sock_;
    char* reader_;
    char* writer_;
    int error_code_;
    char buffer_[Size];
};

template <size_t Size>
void BufReader<Size>::reset() {
    error_code_ = 0;
    reader_ = writer_ = buffer_;
}

template <size_t Size>
void BufReader<Size>::fill() {
    if (reader_ > buffer_) {
        int dist{static_cast<int>(writer_ - reader_)};
        if (dist > 0) {
            std::memcpy(buffer_, reader_, dist);
            writer_ -= dist;
            reader_ = buffer_;
        } else {
            reader_ = writer_ = buffer_;
        }
    }

    errors::Error err("BufReader", "fill", ErrFillFullBuffer);

    if (static_cast<size_t>(writer_ - buffer_) >= Size) {
        throw err;
    }

    for (auto i{MaxConsecutiveEmptyReads}; i > 0; --i) {
        auto n = sock_->read(writer_, Size - (writer_ - buffer_));
        if (n < 0) {
            err.detail_ = ErrNegativeCount;
            throw err;
        }
        writer_ += n;
        if (sock_->last_error() != 0) {
            error_code_ = sock_->last_error();
            return;
        }
        if (n > 0) {
            return;
        }
    }
    err.detail_ = ErrNoProcess;
    throw err;
}

template <size_t Size>
std::string BufReader<Size>::read_err() {
    if (error_code_ != 0) {
        return sockpp::tcp_socket::error_str(error_code_);
    }
    return "";
}

template <size_t Size>
std::pair<char*, char*> BufReader<Size>::read_until(char delim) {
    errors::Error err("BufReader", "read_until", "");
    std::pair<char*, char*> res;
    for (;;) {

        if (auto f{std::find(reader_, writer_, delim)}; *f = delim) {
            res = std::make_pair(reader_, f);
            reader_ = f + 1;
            return res;
        }
        // not found delim
        if (error_code_ != 0) {
            err.detail_ = sockpp::tcp_socket::error_str(error_code_);
            throw err;
        }

        if (buffered() >= Size) {
            err.detail_ = ErrFillFullBuffer;
            throw err;
        }
        fill();
    }
}

template <size_t Size>
size_t BufReader<Size>::buffered() {
    return writer_ - reader_;
}

template <size_t Size> 
char BufReader<Size>::read_byte() {
    errors::Error err("BufReader", "read_byte", "");
    while (reader_ == writer_) {
        if (error_code_ != 0) {
            err.detail_ = sockpp::tcp_socket::error_str(error_code_);
            throw err;
        }
        fill();
    }
    char buf = *reader_;
    ++reader_;
    return buf;
}


template<size_t Size>
bool BufReader<Size>::read_n(size_t n, std::string &buf) {
    auto remain{static_cast<int>(n)};
    for (; remain > 0;) {
        int buf_siz{static_cast<int>(buffered())};
        // buffer enough...
        if (buf_siz > remain) {
            buf += std::string(reader_, remain);
            reader_ += remain;
            break;
        }
        // buffer not enough
        if (buf_siz < remain && buffered() > 0) {
            remain -= buffered();
            buf += std::string(reader_, buffered());
            reader_ = writer_ = buffer_;
        }
        fill();
    }
    return true;
}

template<size_t Size>
std::string BufReader<Size>::peek(size_t n) {
    if (n < 0) {
        return std::string();
    }

    while (buffered() < n && buffered() < Size && error_code_ != 0) {
        fill();
    }
    std::string temp;

    if (auto avail = buffered();avail < n) {
        n = avail;
    }
    temp.clear();
    return temp.assign(reader_, reader_ + n);
}


} // end of namespace bufio


#endif