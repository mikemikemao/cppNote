/** @file:   hash_map_def.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  定义std::string为key的hash函数
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/08
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/08        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef __HASH_MAP_DEF_H__
#define __HASH_MAP_DEF_H__

#include<string>

#define INIT_HASH_BUCKET_SIZE    16384

#if (defined(_WIN32) || defined(_WIN64))
    #include<hash_map>
    using namespace stdext;

    template<> class hash_compare<std::string, std::less<std::string>>
    {
    public:
        enum 
        { 
            //parameters for hash table 
            bucket_size = 1, 
            min_buckets = INIT_HASH_BUCKET_SIZE
        }; 

        //SDBHash
        size_t operator ()(const std::string& key_value) const
        {
            const unsigned char * p = (const unsigned char *)key_value.c_str(); 
            unsigned int hash = 0;  

            while (*p)  
            {  
                hash = (*p++) + (hash << 6) + (hash << 16) - hash;  
            }  

            return (hash & 0x7FFFFFFF);
        }

        bool operator ()(const std::string& left_key, const std::string& right_key) const
        {
            return (left_key < right_key);
        }
    };
#elif (defined(__linux__))
    #include<ext/hash_map>
    //扩展__gnu_cxx命名空间
    namespace __gnu_cxx
    {
        template<> struct hash<std::string>
        {
            size_t operator()(const std::string& key_value) const
            {
                const unsigned char * p = (const unsigned char *)key_value.c_str(); 
                unsigned int hash = 0;  

                while (*p)  
                {  
                    hash = (*p++) + (hash << 6) + (hash << 16) - hash;  
                }  

                return (hash & 0x7FFFFFFF);
            }
        };
    }
#else 
    #error unsupport!
#endif

#endif //__HASH_MAP_DEF__
