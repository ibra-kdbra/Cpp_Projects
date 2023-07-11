// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef DIGEST_HPP_
#define DIGEST_HPP_

#include <cstring>
#include <cassert>
#include <openssl/evp.h>

namespace crypto {
class digest_t final {
public:
    inline explicit digest_t(const EVP_MD *md = EVP_sha256()) : ctx(EVP_MD_CTX_create()) {
        assert(md != nullptr);
        if(ctx && EVP_DigestInit_ex(ctx, md, nullptr) != 1) {
            EVP_MD_CTX_destroy(ctx);
            ctx = nullptr;
        }
    }

    inline explicit digest_t(const char *cp) : ctx(EVP_MD_CTX_create()) {
        if(ctx && EVP_DigestInit_ex(ctx, EVP_get_digestbyname(cp), nullptr) != 1) {
            EVP_MD_CTX_destroy(ctx);
            ctx = nullptr;
        }
    }

    inline digest_t(const digest_t& from) {
        if(from.ctx)
            ctx = EVP_MD_CTX_create();
        else
            ctx = nullptr;
        if(ctx && EVP_MD_CTX_copy_ex(ctx, from.ctx) != 1) {
            EVP_MD_CTX_destroy(ctx);
            ctx = nullptr;
        }
        if(ctx)
            size_ = from.size_;
        if(size_)
            memcpy(data_, from.data_, size_);   // FlawFinder: size valid
    }

    inline digest_t(digest_t&& from) : ctx(from.ctx), size_(from.size_) {
        if(size_)
            memcpy(data_, from.data_, size_);   // FlawFinder: size valid
        from.ctx = nullptr;
        from.size_ = 0;
    }

    inline ~digest_t() {
        if(ctx)
            EVP_MD_CTX_destroy(ctx);
    }

    inline auto operator=(const digest_t& from) -> digest_t& {
        if(this == &from)
            return *this;
        if(ctx) {
            EVP_MD_CTX_destroy(ctx);
            ctx = nullptr;
        }
        if(from.ctx)
            ctx = EVP_MD_CTX_create();
        if(ctx && EVP_MD_CTX_copy_ex(ctx, from.ctx) != 1) {
            EVP_MD_CTX_destroy(ctx);
            ctx = nullptr;
        }
        if(ctx)
            size_ = from.size_;
        if(size_)
            memcpy(data_, from.data_, size_);   // FlawFinder: size valid
        return *this;
    }

    inline operator bool() const {
        return ctx != nullptr;
    }

    inline auto operator!() const {
        return ctx == nullptr;
    }

    inline auto size() const {
        return size_;
    }

    inline auto data() const {
        return data_;
    }

    inline auto c_str() const {
        return reinterpret_cast<const char *>(data_);
    }

    inline auto update(const char *cp, size_t size) {
        return !ctx || size_ ? false : EVP_DigestUpdate(ctx, reinterpret_cast<unsigned char *>(const_cast<char *>(cp)), size) == 1;
    }

    inline auto finish() {
        if(!ctx || size_)
            return false;
        return EVP_DigestFinal_ex(ctx, data_, &size_) == 1;
    }

    inline void reinit() {
        if(ctx)
            EVP_DigestInit_ex(ctx, nullptr, nullptr);
        size_ = 0;
    }

private:
    EVP_MD_CTX *ctx;
    unsigned size_{0};
    unsigned char data_[EVP_MAX_MD_SIZE]{0};
};
} // end namespace

/*!
 * Digest functions and support.
 * \file digest.hpp
 */
#endif
