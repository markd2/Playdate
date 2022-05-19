// Thanks to @takuya from the Playdate discords for this coolness.

#if defined(NDEBUG) && defined(APPLE) && defined(arm64)
    // m1 simulator release環境でplacement newが動作しない
    // std::string / std::map などのアロケータが関連していそうだが、解決できなかった。
    // simulatorでは通常のnew/releaseが呼び出されても問題がないので現時点では無効にしている

    // placement new does not work in m1 simulator release environment
    // Allocators such as std :: string / std :: map seem to be related, but I couldn't solve it.
    // In the simulator, there is no problem even if normal new / release is called, so it is disabled at this time.

#else

    ///
    void* operator new(std::size_t sz) { return PDMalloc(sz); }
    void* operator new[](std::size_t sz) { return PDMalloc(sz); }
    void* operator new(std::size_t sz, const std::nothrow_t&) noexcept { return PDMalloc(sz); }
    void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept { return PDMalloc(sz); }

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)  __cplusplus >= 201703L)
    void* operator new(std::size_t sz, std::align_val_t) { return PDMalloc(sz); }
    void* operator new[](std::size_t sz, std::align_val_t) { return PDMalloc(sz); }
    void* operator new(std::size_t sz, std::align_val_t, const std::nothrow_t&) noexcept { return PDMalloc(sz); }
    void* operator new[](std::size_t sz, std::align_val_t, const std::nothrow_t&) noexcept { return PDMalloc(sz); }
    #endif

    void operator delete(void* p) noexcept { PDFree(p); }
    void operator delete[](void* p) noexcept { PDFree(p); }
    void operator delete(void* p, const std::nothrow_t&) noexcept { PDFree(p); }
    void operator delete[](void* p, const std::nothrow_t&) noexcept { PDFree(p); }
    void operator delete(void* p, std::size_t) noexcept { PDFree(p); }
    void operator delete[](void* p, std::size_t) noexcept { PDFree(p); }
    void operator delete(void* p, std::size_t, const std::nothrow_t&) noexcept { PDFree(p); }
    void operator delete[](void* p, std::size_t, const std::nothrow_t&) noexcept { PDFree(p); }

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)  __cplusplus >= 201703L)
    void operator delete(void* p, std::size_t, std::align_val_t) noexcept { PDFree(p); }
    void operator delete[](void* p, std::size_t, std::align_val_t) noexcept { PDFree(p); }
    #endif
#endif
