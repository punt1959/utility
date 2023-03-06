//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef numinc_hpp
#define numinc_hpp

#include <cstdint>
#include <string>
#include <limits>
#include <algorithm>

namespace util{
    //=================================================================================
    template <typename T>
    class numinc_t {
        static_assert(std::is_integral_v<T>,
                      "numic_t requires integral types");
        T current_value ;
        T limit ;
        
    public:
        auto next() ->T {
            if ((current_value+1) == limit){
                throw std::runtime_error(std::string("Numeric limit has been reached: ") + std::to_string(limit));
            }
            return ++current_value;
            
        }
        auto used(T value) ->numinc_t&{
            current_value=std::max(current_value,value);
        }
        numinc_t(T initial=0):current_value(initial),limit(std::numeric_limits<T>::max()){};
        numinc_t(T initial, T max_value):current_value(initial),limit(max_value){};
        
    };
}
#endif /* numinc_hpp */
