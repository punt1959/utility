//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef filemap_hpp
#define filemap_hpp

#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>
namespace util {
    //=================================================================================
    class filemap_t {
        std::filesystem::path path ;
    public:
        filemap_t():ptr(nullptr),length(0){}
        ~filemap_t() ;
        filemap_t(const std::filesystem::path &filepath);
        [[maybe_unused]] auto map(const std::filesystem::path &filepath) ->std::uint8_t* ;
        [[maybe_unused]] auto unmap(bool nothrow=false)->bool ;
        std::uint8_t *ptr ;
        std::size_t length ;
    };
}
#endif /* filemap_hpp */
