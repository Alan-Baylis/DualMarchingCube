//
//  utils.h
//  DMC
//
//  Created by Ye Kuang on 11/24/15.
//  Copyright © 2015 Ye Kuang. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <vector>
#include <iostream>
#include <cmath>
#include <functional>   // std::less, std::greater

namespace utils
{
    template <typename T>
    struct tuple2
    {
        tuple2() = default;
        tuple2(T _x, T _y) : x(_x), y(_y) { }
        
        T x, y;
    };
    
    using float2 = tuple2<float>;
    
    inline float2 make_float2(float x, float y) { return {x, y}; }
    
    template <typename T>
    struct tuple3
    {
        tuple3() = default;
        tuple3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) { }
        
        T x, y, z;
    };
    
    using uint3 = tuple3<unsigned>;
    
    class float3 : public tuple3<float>
    {
    public:
        float3() : tuple3() { }
        float3(float x, float y, float z) : tuple3(x, y, z) { }
        
        float2 xy() const { return make_float2(x, y); }
        float2 xz() const { return make_float2(x, z); }
        float2 yz() const { return make_float2(y, z); }
    };
    
    inline uint3 make_uint3(unsigned x, unsigned y, unsigned z)
    {
        return {x, y, z};
    }
    
    inline float3 make_float3(float x, float y, float z)
    {
        return {x, y, z};
    }
    
    // small 3D vector wrapper implemented using std::vector<T>
    template <typename T>
    class Array3D
    {
    public:
        typedef T value_type;
        typedef typename std::vector<value_type>::iterator iterator;
        typedef typename std::vector<value_type>::const_iterator const_iterator;
        
        Array3D(unsigned dim_x, unsigned dim_y, unsigned dim_z, const T& value = T())
        : m_dim_x(dim_x)
        , m_dim_y(dim_y)
        , m_dim_z(dim_z)
        , m_dim_xy(dim_x * dim_y)
        , m_data(std::vector<value_type>(m_dim_xy * dim_z, value)) { }
        
        const T& operator()(unsigned x, unsigned y, unsigned z) const
        {
            return m_data[z * m_dim_xy + y * m_dim_x + x];
        }
        
        T& operator()(unsigned x, unsigned y, unsigned z)
        {
            return m_data[z * m_dim_xy + y * m_dim_x + x];
        }
        
        unsigned dim_x() const { return m_dim_x; }
        unsigned dim_y() const { return m_dim_y; }
        unsigned dim_z() const { return m_dim_z; }
        
        iterator begin()                { return m_data.begin(); }
        iterator end()                  { return m_data.end(); }
        const_iterator cbegin() const   { return m_data.cbegin(); }
        const_iterator cend() const     { return m_data.cend(); }
        
    private:
        unsigned m_dim_x;
        unsigned m_dim_y;
        unsigned m_dim_z;
        unsigned m_dim_xy;
        
        std::vector<value_type> m_data;
    };
    
    inline void index3D_to_1D(unsigned i, unsigned j, unsigned k,
                              unsigned num_voxels_i, unsigned num_voxels_j, unsigned& index1D)
    {
        index1D = (k * num_voxels_j + j) * num_voxels_i + i;
    }
    
    inline void index3D_to_1D(const uint3& index3D, const uint3& num_voxels_dim, unsigned& index1D)
    {
        index3D_to_1D(index3D.x, index3D.y, index3D.z, num_voxels_dim.x, num_voxels_dim.y, index1D);
    }
    
    inline void index1D_to_3D(unsigned index1D, const uint3& num_voxels_dim, uint3& index3D)
    {
        unsigned num_voxels_xy = num_voxels_dim.x * num_voxels_dim.y;
        
        index3D.z = index1D / num_voxels_xy;
        index1D = index1D % num_voxels_xy;
        index3D.y = index1D / num_voxels_dim.x;
        index1D = index1D % num_voxels_dim.x;
        index3D.x = index1D;
    }
    
    inline float ijk_to_xyz(unsigned i, unsigned size, float f_range, float f_min)
    {
        return (float)i / size * f_range + f_min;
    }
    
    template <size_t, size_t, typename Op, typename T>
    size_t argmin_impl(size_t min_index, const T&, const Op&) { return min_index; }
    
    template <size_t Index, size_t Size, typename Op, typename T, typename ...Ts>
    size_t argmin_impl(size_t min_index, const T& cur_min, const Op& op, const T& head, const Ts&... rest)
    {
        if (op(head, cur_min))
        {
            return argmin_impl<Index + 1, Size>(Index, head, op, rest...);
        }
        return argmin_impl<Index + 1, Size>(min_index, cur_min, op, rest...);
    }
    
    template <typename T, typename ...Ts>
    size_t argmin(const T& head, const Ts&... rest)
    {
        return argmin_impl<0, sizeof...(Ts)>(0, head, std::less<T>(), head, rest...);
    }
    
    template <typename T, typename ...Ts>
    size_t argmax(const T& head, const Ts&... rest)
    {
        return argmin_impl<0, sizeof...(Ts)>(0, head, std::greater<T>(), head, rest...);
    }
}; // namespace utils

template <typename T>
std::ostream& operator<<(std::ostream& os, const utils::tuple2<T>& t)
{
    os << t.x << " " << t.y;
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const utils::tuple3<T>& t)
{
    os << t.x << " " << t.y << " " << t.z;
    return os;
}

inline utils::float2& operator+=(utils::float2& lhs, const utils::float2& rhs)
{
    lhs.x += rhs.x; lhs.y += rhs.y;
    return lhs;
}

inline utils::float2 operator+(const utils::float2& lhs, const utils::float2& rhs)
{
    utils::float2 result = lhs;
    result += rhs;
    return result;
}

inline utils::float2& operator-=(utils::float2& lhs, const utils::float2& rhs)
{
    lhs.x -= rhs.x; lhs.y -= rhs.y;
    return lhs;
}

inline utils::float2 operator-(const utils::float2& lhs, const utils::float2& rhs)
{
    utils::float2 result = lhs;
    result -= rhs;
    return result;
}

inline utils::float2& operator*=(utils::float2& lhs, float rhs)
{
    lhs.x *= rhs; lhs.y *= rhs;
    return lhs;
}

inline utils::float2 operator*(const utils::float2& lhs, float rhs)
{
    utils::float2 result = lhs;
    result *= rhs;
    return result;
}

inline utils::float2 operator*(float lhs, const utils::float2& rhs)
{
    return rhs * lhs;
}

inline utils::float2& operator/=(utils::float2& lhs, float rhs)
{
    lhs *= 1.0f / rhs;
    return lhs;
}

inline utils::float2 operator/(const utils::float2& lhs, float rhs)
{
    utils::float2 result = lhs;
    result /= rhs;
    return result;
}

inline float dot(const utils::float2& lhs, const utils::float2& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline void normalize(utils::float2& f)
{
    float len = sqrtf(dot(f, f));
    f /= len;
}

inline utils::float3& operator+=(utils::float3& lhs, const utils::float3& rhs)
{
    lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z;
    return lhs;
}

inline utils::float3 operator+(const utils::float3& lhs, const utils::float3& rhs)
{
    utils::float3 result = lhs;
    result += rhs;
    return result;
}

inline utils::float3& operator-=(utils::float3& lhs, const utils::float3& rhs)
{
    lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z;
    return lhs;
}

inline utils::float3 operator-(const utils::float3& lhs, const utils::float3& rhs)
{
    utils::float3 result = lhs;
    result -= rhs;
    return result;
}

inline utils::float3& operator*=(utils::float3& lhs, float rhs)
{
    lhs.x *= rhs; lhs.y *= rhs; lhs.z *= rhs;
    return lhs;
}

inline utils::float3 operator*(const utils::float3& lhs, float rhs)
{
    utils::float3 result = lhs;
    result *= rhs;
    return result;
}

inline utils::float3 operator*(float lhs, const utils::float3& rhs)
{
    return rhs * lhs;
}

inline utils::float3& operator/=(utils::float3& lhs, float rhs)
{
    lhs *= 1.0f / rhs;
    return lhs;
}

inline utils::float3 operator/(const utils::float3& lhs, float rhs)
{
    utils::float3 result = lhs;
    result /= rhs;
    return result;
}

inline float dot(const utils::float3& lhs, const utils::float3& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline utils::float3 cross(const utils::float3& lhs, const utils::float3& rhs)
{
    return utils::make_float3(lhs.y * rhs.z - lhs.z * rhs.y,
                              lhs.z * rhs.x - lhs.x * rhs.z,
                              lhs.x * rhs.y - lhs.y * rhs.x);
}

inline void normalize(utils::float3& f)
{
    float len = sqrtf(dot(f, f));
    f /= len;
}

#endif /* utils_h */
