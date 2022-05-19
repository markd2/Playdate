

row_t&) noexcept { return PDMalloc(sz); }
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
