//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef buffer_hpp
#define buffer_hpp

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstddef>

#include <type_traits>
namespace util {
    //=================================================================================
    /* General purpose buffer class.  This allows for reading/writing intergral types from a
     position in the buffer.
     
     */
    class buffer_t {
        //=================================================================================
        // variables/data
        //=================================================================================
        
        const std::uint8_t *read_data ; // Ptr to the data we read
        std::uint8_t *write_data ; //Ptr to data we write to
        mutable std::size_t current_position ; // Curent position into the data stream
        std::size_t length ; // Length of the data
        bool owning ; // Do we own the data or not?
        std::vector<std::uint8_t> data ;// If we own the data, this is storage
        bool is_expandable ;
    public:
        //=================================================================================
        // Constructors
        //=================================================================================
        buffer_t();
        buffer_t(std::uint8_t *ptr,std::size_t size, bool consume=false) ;
        buffer_t(const std::uint8_t *ptr,std::size_t size, bool consume=false) ;
        buffer_t(std::size_t size) ;
        
        //=================================================================================
        // Size/position related
        //=================================================================================
        auto size() const ->std::size_t ;
        auto remaining() const ->std::size_t ;
        auto at() const ->std::size_t;
        [[maybe_unused]] auto at(std::size_t position)  -> buffer_t&;
        [[maybe_unused]] auto resize(std::size_t size) ->buffer_t& ;
        
        [[maybe_unused]] auto expandable(bool value) ->buffer_t ;
        auto expandaable() const ->bool ;
        //=================================================================================
        // Access related
        //=================================================================================
        auto raw() ->std::uint8_t* ;
        auto raw() const ->const std::uint8_t* ;
        
        //=================================================================================
        // read
        //=================================================================================
        //=================================================================================
        template <typename T>
        inline typename std::enable_if<std::is_integral_v<T>,T>::type
        read(bool reverse=false){
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            
            auto bytesize = sizeof(T) ;
            if (current_position+bytesize >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            T value ;
            std::copy(read_data+current_position,read_data+current_position+bytesize,reinterpret_cast<std::uint8_t*>(&value));
            current_position+= bytesize;
            if (reverse) {
                std::reverse(reinterpret_cast<std::uint8_t*>(&value), reinterpret_cast<std::uint8_t*>(&value)+bytesize);
            }
            return value ;
        }
        //=================================================================================
        template <typename T>
        inline typename std::enable_if<std::is_same_v<T,std::string>,T>::type
        read(std::size_t amount){
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            
            auto bytesize = amount ;
            if (current_position+bytesize >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            T value ;
            auto buf = std::vector<char>(amount+1,0);
            std::copy(read_data+current_position,read_data+current_position+bytesize,reinterpret_cast<std::uint8_t*>(buf.data()));
            current_position+= bytesize;
            value = buf.data();
            return value ;
        }
        
        //=================================================================================
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_array_v<T>,buffer_t&>::type
        read(T &value, size_t amount){
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            
            auto bytesize = sizeof(std::remove_all_extents_t<T>) ;
            if (current_position+(bytesize*amount) >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            
            std::copy(read_data+current_position,read_data+current_position+(bytesize*amount),reinterpret_cast<std::uint8_t*>(value));
            current_position+= (bytesize*amount);
            return *this ;
        }
        
        //=================================================================================
        inline auto read(std::uint8_t *value,std::size_t amount) ->buffer_t& {
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            
            auto bytesize = 1 ;
            if (current_position+(bytesize*amount) >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            
            std::copy(read_data+current_position,read_data+current_position+(bytesize*amount),reinterpret_cast<std::uint8_t*>(value));
            current_position+= (bytesize*amount);
            return *this ;
            
        }
        
        //=================================================================================
        // write
        //=================================================================================
        //=================================================================================
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_integral_v<T>,buffer_t&>::type
        write(T value,bool reverse=false){
            if (write_data==nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto bytesize = sizeof(T) ;
            if (current_position+bytesize >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+bytesize);
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            if (reverse) {
                std::reverse(reinterpret_cast<std::uint8_t*>(&value), reinterpret_cast<std::uint8_t*>(&value)+bytesize);
            }
            
            std::copy(reinterpret_cast<std::uint8_t*>(&value),reinterpret_cast<std::uint8_t*>(&value)+bytesize,write_data+current_position);
            current_position+= bytesize;
            return *this ;
        }
        //=================================================================================
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_same_v<T,std::string>,buffer_t&>::type
        write(const T &value,std::size_t amount){
            if (write_data==nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto bytesize = amount ;
            if (current_position+bytesize >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+bytesize);
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            // What if we write more then the size of the string?
            auto writesize = amount ;
            if (writesize > value.size()){
                writesize = value.size();
            }
            std::copy(reinterpret_cast<const std::uint8_t*>(value.c_str()),reinterpret_cast<const std::uint8_t*>(value.c_str())+writesize,write_data+current_position);
            if (writesize < amount){
                auto nwritesize = amount - writesize;
                auto buf = std::vector<std::uint8_t>(nwritesize,0);
                std::copy(buf.data(),buf.data()+buf.size(),write_data+current_position+writesize);
            }
            current_position+= bytesize;
            return *this ;
        }
        //=================================================================================
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_array_v<T>,buffer_t&>::type
        write(T &value, std::size_t amount){
            if (write_data==nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto bytesize = sizeof(std::remove_all_extents_t<T>) ;
            if (current_position+(bytesize*amount) >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+(bytesize*amount));
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            
            std::copy(reinterpret_cast<std::uint8_t*>(value),reinterpret_cast<std::uint8_t*>(value)+(bytesize*amount),write_data+current_position);
            current_position+= (bytesize*amount);
            return *this ;
        }
        //=================================================================================
        template <typename T>
        [[maybe_unused]] inline buffer_t& write(std::uint8_t *value, std::size_t amount) {
            if (write_data==nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto bytesize = 1 ;
            if (current_position+(bytesize*amount) >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+(bytesize*amount));
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            
            std::copy(value,value+(bytesize*amount),write_data+current_position);
            current_position+= (bytesize*amount);
            return *this ;
        }
        
        
    };
}
#endif /* buffer_hpp */
