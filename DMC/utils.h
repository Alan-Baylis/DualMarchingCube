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

namespace utils
{
    template <typename T>
    struct tuple3
    {
        tuple3() = default;
        tuple3(T _x, T _y, T _z)
        : x(_x)
        , y(_y)
        , z(_z) { }
        
        T x, y, z;
    };
    
    using uint3 = tuple3<unsigned>;
    using float3 = tuple3<float>;
    
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
}; // namespace utils

template <typename T>
std::ostream& operator<<(std::ostream& os, const utils::tuple3<T>& t)
{
    os << t.x << " " << t.y << " " << t.z;
    return os;
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


#endif /* utils_h */
