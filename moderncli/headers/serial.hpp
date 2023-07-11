// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef SERIAL_HPP_
#define SERIAL_HPP_

#include <thread>
#include <chrono>
#include <string>
#include <cctype>
#include <cstring>
#include <csignal>

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#include <sys/ioctl.h>

namespace tycho {
class serial_t final {
public:
    inline serial_t() = default;

    inline explicit serial_t(const std::string& path) {
        open(path); // FlawFinder: safe
    }

    inline serial_t(const serial_t& from) :
    original_(from.original_), current_(from.current_) {
        if(from.device_ > -1)
            device_ = ::dup(from.device_);
    }

    inline serial_t(serial_t&& from) :
    device_(from.device_), original_(from.original_), current_(from.current_) {
        from.device_ = -1;
    }

    inline ~serial_t() {
        close();
    }

    inline auto operator!() const {
        return device_ < 0;
    }

    inline operator bool() const {
        return device_ > -1;
    }

    void open(const std::string& fname) {   // FlawFinder: safe
        close();
        device_ = ::open(fname.c_str(), O_RDWR | O_NDELAY); // FlawFinder: safe
        if(device_ < 0)
            return;

        if(!isatty(device_)) {
            ::close(device_);
            device_ = -1;
            return;
        }

        auto ioflags = fcntl(device_, F_GETFL);
        tcgetattr(device_, &current_);
        tcgetattr(device_, &original_);

        current_.c_oflag = current_.c_lflag = 0;
        current_.c_cflag = CLOCAL | CREAD | HUPCL;
        current_.c_iflag = IGNBRK;

        memset(&current_.c_cc, 0, sizeof(current_.c_cc));
        current_.c_cc[VMIN] = 1;

        current_.c_cflag |= original_.c_cflag & (CRTSCTS | CSIZE | PARENB | PARODD | CSTOPB);
        current_.c_iflag |= original_.c_iflag & (IXON | IXANY | IXOFF);

        tcsetattr(device_, TCSANOW, &current_);
        fcntl(device_, F_SETFL, ioflags & ~O_NDELAY);

#if defined(TIOCM_RTS) && defined(TIOCMODG)
        auto mcs = 0;
        ioctl(device_, TIOCMODG, &mcs);
        mcs |= TIOCM_RTS;
        ioctl(device_, TIOCMODS, &mcs);
#endif
    }

    void close() {
        if(device_ > -1) {
            tcsetattr(device_, TCSANOW, (struct termios *)&original_);
            ::close(device_);
        }
        device_ = -1;
    }

    auto wait(int msec = -1) const -> bool {
        if(device_ < 0)
            return false;

        int status{0};
        struct pollfd pfd{};
        pfd.fd = device_;
        pfd.revents = 0;
        pfd.events = POLLIN;

        status = ::poll(&pfd, 1, msec);
        if(status < 1)
            return false;

        return pfd.revents & POLLIN;
    }

    auto get() const {
        if(device_ > -1) {
            char buf{0};
            if(::read(device_, &buf, 1) < 1)    // FlawFinder: safe
                return EOF;
            return static_cast<int>(buf);
        }
        return EOF;
    }

    auto get(char *data, size_t size) const {
        if(!data || !size)
            return 0U;

        auto count = ::read(device_, data, size);   // FlawFinder: safe
        if(count > 0)
            return static_cast<unsigned>(count);
        return 0U;
    }

    auto put(char code) const {
        if(device_ > -1) {
            if(::write(device_, &code, 1) < 1)
                return EOF;
            return 1;
        }
        return EOF;
    }

    auto put(const char *data, size_t size) const {
        auto count = ::write(device_, data, size);
        if(count > 0)
            return static_cast<unsigned>(count);
        return 0U;
    }

    auto puts(std::string_view msg) const {
        return put(msg.data(), msg.size());
    }

    void reset() {
        if(device_ > -1) {
            current_ = original_;
            tcsetattr(device_, TCSANOW, &current_);
        }
    }

    void flush() const {
        if(device_ > -1)
            tcflush(device_, TCOFLUSH);
    }

    void drain() const {
        if(device_ > -1)
            tcflush(device_, TCIFLUSH);
    }

    void sync() const {
        if(device_ > -1)
            tcdrain(device_);
    }

    void hup() const {
        if(device_ >  -1)
            tcsendbreak(device_, 0);
    }

    void dtr(unsigned msec) {
        if(device_ < 0)
            return;

        struct termios temp{};
        tcgetattr(device_, &temp);
        cfsetospeed(&current_, 0);
        cfsetispeed(&current_, 0);
        tcsetattr(device_, TCSANOW, &current_);
        if(msec) {
            std::this_thread::sleep_for(std::chrono::milliseconds(msec));
            tcsetattr(device_, TCSANOW, &temp);
            current_ = temp;
        }
    }

    void flow(bool hw, bool sw) {
        if(device_ < 0)
            return;

        current_.c_cflag &= ~CRTSCTS;
        current_.c_iflag &= ~(IXON | IXANY | IXOFF);

        if(sw)
            current_.c_iflag |= (IXON | IXANY | IXOFF);

        if(hw)
            current_.c_iflag |= (IXON | IXANY | IXOFF);

        tcsetattr(device_, TCSANOW, &current_);
    }

    void format(const char *s) {
        unsigned bits = 8;
        unsigned stop = 1;
        unsigned parity = 'n';

        if(s && *s && isdigit(*s))
            bits = *(s++) - '0';

        if(s && *s && !isdigit(*s))
            parity = static_cast<unsigned>(tolower(*(s++)));

        if(s && *s && isdigit(*s))
            stop = *(s++) - '0';

        if(stop < 1 || stop > 2 || bits < 5 || bits > 8)
            return;

        if(device_ < 0)
            return;

        current_.c_cflag &= ~(PARENB | PARODD | CSTOPB | CSIZE);

        switch(bits) {
        case '5':
            current_.c_cflag |= CS5;
            break;
        case '6':
            current_.c_cflag |= CS6;
            break;
        case '7':
            current_.c_cflag |= CS7;
            break;
        case '8':
            current_.c_cflag |= CS8;
            break;
        }

        switch(stop) {
        case '1':
            break;
        case '2':
            current_.c_cflag |= CSTOPB;
            break;
        default:
            return;
        }

        switch(parity) {
        case 'o':
            current_.c_cflag |= (PARENB | PARODD);
            break;
        case 'e':
            current_.c_cflag |= PARENB;
            break;
        case 'n':
            break;
        default:
            return;
        }

        tcsetattr(device_, TCSANOW, &current_);
    }

    auto speed(unsigned long bytes) {
        unsigned long rate = 0;
        switch(bytes) {
#ifdef B256000
        case 256000:
            rate = B256000;
            break;
#endif
#ifdef B230400
        case 230400:
            rate = B230400;
            break;
#endif
#ifdef B128000
        case 128000:
            rate = B128000;
            break;
#endif
#ifdef B115200
        case 115200:
            rate = B115200;
            break;
#endif
#ifdef B57600
        case 57600:
            rate = B57600;
            break;
#endif
#ifdef B56000
        case 56000:
            rate = B56000;
            break;
#endif
#ifdef B38400
        case 38400:
            rate = B38400;
            break;
#endif
        case 19200:
            rate = B19200;
            break;
        case 9600:
            rate = B9600;
            break;
        case 4800:
            rate = B4800;
            break;
        case 2400:
            rate = B2400;
            break;
        case 1200:
            rate = B1200;
            break;
        case 600:
            rate = B600;
            break;
        case 300:
            rate = B300;
            break;
        case 110:
            rate = B110;
            break;
        default:
            return false;
        }

        if(device_ < 0)
            return false;

        cfsetispeed(&current_, rate);
        cfsetospeed(&current_, rate);
        tcsetattr(device_, TCSANOW, &current_);
        return true;
    }

private:
    int device_{-1};
    struct termios original_{}, current_{};
};
} // end namespace

#endif
