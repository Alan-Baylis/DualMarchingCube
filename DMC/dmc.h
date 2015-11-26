//
//  dmc.h
//  DMC
//
//  Created by Ye Kuang on 11/24/15.
//  Copyright © 2015 Ye Kuang. All rights reserved.
//

#ifndef dmc_h
#define dmc_h

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include <bitset>

#include "utils.h"

namespace dmc
{
    using namespace utils;
    
    typedef utils::Array3D<float> scalar_grid_type;
    
    typedef uint8_t voxel_pt_index_type;
    typedef uint8_t voxel_edge_index_type;
    typedef uint8_t voxel_face_index_type;
    
    typedef uint8_t voxel_config_type;
    typedef unsigned voxel_index1D_type;
    typedef uint8_t vertex_index_type;
    typedef unsigned char flag_type;
    
    const unsigned INVALID_UINT8 = 0xff;
    const unsigned INVALID_UINT32 = 0xffffffff;
    
    const unsigned VOXEL_NUM_PTS = 8;
    const unsigned VOXEL_NUM_EDGES = 12;
    const unsigned VOXEL_NUM_FACES = 6;
    
    // For inactive voxel index_1D in full_voxel_index_map, this value is stored.
    const voxel_index1D_type INVALID_INDEX_1D = INVALID_UINT32;
    const voxel_config_type MAX_VOXEL_CONFIG_MASK = INVALID_UINT8;
    // Used in config_edge_lut1[2]
    const vertex_index_type NO_VERTEX = INVALID_UINT8;
    
    // For each voxel config and an edge index, return the associated iso vertex in DMC.
    // This is LUT 1. Voxel with 3B config with its adjacent voxel being 2B config CANNOT use this LUT.
    const vertex_index_type config_edge_lut1[256][VOXEL_NUM_EDGES] =
    {
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0, 0, 0, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 1, 0, 0, 1, 1, 0, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0, 1, 1, 1, 1, 0xff, 0xff, 1 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0xff },
        { 1, 1, 0, 0, 0, 1, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0, 0xff, 0, 0, 0, 0xff, 0xff },
        { 0, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0, 0, 0, 0, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 1, 0, 1, 1, 1, 1, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0, 1, 1, 0xff, 0, 1, 0xff, 0xff, 1, 0xff, 1 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0, 0, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0, 1, 1, 0, 0, 0xff, 0xff, 1, 0xff, 1, 0xff, 1 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0, 0, 1, 1, 0xff, 1, 0xff, 1 },
        { 1, 1, 0xff, 0xff, 0xff, 1, 0, 0, 0xff, 0, 0xff, 0 },
        { 1, 0xff, 0xff, 1, 1, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0, 0, 0xff },
        { 0, 1, 1, 0, 0xff, 0, 1, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0, 0xff, 1, 1, 0, 1, 0xff, 1, 1, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0 },
        { 0, 0, 0xff, 0xff, 0, 1, 0, 0xff, 1, 1, 0, 0 },
        { 0xff, 1, 0xff, 1, 0xff, 0, 1, 0xff, 0, 0, 1, 1 },
        { 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 0xff, 0xff, 0xff, 0xff, 0, 0, 1, 1 },
        { 1, 0xff, 1, 0xff, 1, 0, 0xff, 0xff, 0, 0, 1, 1 },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0, 0 },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0, 1, 0, 0, 1, 1 },
        { 1, 0xff, 1, 0xff, 0xff, 0xff, 1, 0, 1, 1, 0, 0 },
        { 0, 1, 1, 0, 0, 2, 1, 3, 2, 2, 3, 3 },
        { 0xff, 0, 0, 0xff, 0xff, 1, 0, 2, 1, 1, 2, 2 },
        { 0xff, 1, 0xff, 1, 1, 0xff, 0xff, 0, 1, 1, 0, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 1, 0, 0, 1, 1 },
        { 0, 0xff, 0xff, 0, 0, 2, 0xff, 1, 2, 2, 1, 1 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 1, 0xff, 0, 1, 1, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0, 1, 1, 1, 0, 0xff, 0xff, 1, 0xff, 1, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0, 0, 0, 0, 0xff, 0, 0xff },
        { 1, 0xff, 1, 0xff, 0, 1, 1, 0, 0, 0xff, 0, 0xff },
        { 1, 0, 0, 1, 0xff, 0xff, 0, 1, 1, 0xff, 1, 0xff },
        { 0xff, 1, 1, 0xff, 0, 0xff, 1, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff },
        { 1, 1, 0xff, 0xff, 0, 1, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 1, 1, 0, 0, 0xff, 1, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 1, 1, 1, 1, 1, 0, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 1, 0xff, 0xff, 1, 1 },
        { 1, 2, 2, 1, 1, 0xff, 2, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 1, 0xff, 0xff, 1, 1 },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 1, 0xff, 0xff, 1, 1, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0 },
        { 0, 0, 1, 1, 0, 0xff, 0, 1, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0, 0, 1, 1, 1, 0, 0xff, 0xff, 1, 1 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0xff, 0, 0, 0, 0xff, 0, 0xff },
        { 1, 1, 0, 0, 0xff, 0xff, 1, 0, 1, 0xff, 1, 0xff },
        { 0, 0xff, 0, 0xff, 0, 1, 1, 0, 1, 0xff, 1, 0xff },
        { 0xff, 0xff, 1, 1, 0xff, 0, 0, 1, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0, 1, 1, 0, 0, 1, 0xff, 0xff, 1, 0xff, 1, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 1, 0xff, 0xff, 1, 1, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0, 0 },
        { 0, 0xff, 0xff, 0, 1, 0, 0xff, 0, 1, 0, 0, 1 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0 },
        { 0xff, 1, 0xff, 1, 0, 0xff, 0xff, 1, 0, 1, 1, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0, 1, 0, 0, 1, 1, 0 },
        { 0, 0, 1, 1, 3, 0, 2, 1, 3, 2, 2, 3 },
        { 1, 0xff, 1, 0xff, 0xff, 0xff, 0, 1, 1, 0, 0, 1 },
        { 0xff, 0xff, 0, 0, 2, 0xff, 1, 0, 2, 1, 1, 2 },
        { 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0 },
        { 1, 0xff, 1, 0xff, 0, 1, 0xff, 0xff, 0, 1, 1, 0 },
        { 0, 1, 1, 0, 0xff, 0xff, 0xff, 0xff, 0, 1, 1, 0 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 0xff, 1, 0, 0, 1 },
        { 0xff, 1, 0xff, 1, 0xff, 1, 0, 0xff, 1, 0, 0, 1 },
        { 0, 0, 0xff, 0xff, 1, 0, 2, 0xff, 1, 2, 2, 1 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 1, 0xff, 0, 1, 1, 0 },
        { 0xff, 0xff, 0xff, 0xff, 1, 0xff, 0, 0xff, 1, 0, 0, 1 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 1, 1, 0xff, 1, 1, 0, 1, 0xff, 0, 0, 0xff },
        { 2, 2, 1, 1, 0xff, 2, 0, 1, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 1, 0, 0xff, 1, 1, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 1, 0, 0xff, 1, 1, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 1, 0, 0, 1, 1, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 1, 0xff, 0xff, 1, 1, 0xff },
        { 1, 1, 0xff, 0xff, 0xff, 1, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 1, 0xff, 1, 0, 0, 1, 1, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 1, 1, 0, 0, 1, 0xff, 0xff, 0, 0xff, 1, 0xff, 1 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 1, 1, 0, 0, 0xff, 1, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 1, 0, 0, 1, 0xff, 1, 0, 0xff, 0xff, 1, 0xff, 1 },
        { 0xff, 1, 1, 0xff, 0, 0, 1, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 1, 1, 0xff, 0xff, 1, 0, 1, 1, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 1, 0, 0, 1, 1, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0xff, 0xff },
        { 0, 0, 1, 1, 0xff, 0xff, 0xff, 1, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 1, 0, 0, 1, 1, 2, 0, 0xff, 2, 2, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 1, 0, 0xff, 1, 1, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 1, 0xff, 0xff, 1, 1, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0 },
        { 1, 0xff, 0xff, 1, 0, 1, 1, 1, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 1, 0xff, 0, 0, 1, 0xff, 0xff, 1 },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 2, 2, 0, 0, 1, 2, 0xff, 0, 1, 0xff, 0xff, 1 },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 1, 1, 0, 0xff, 0xff, 1, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 1, 0, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0, 1, 1, 0, 0xff, 0xff, 1, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 1, 0, 0xff, 0xff, 1, 0xff, 0xff, 1 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff }, 
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff }, 
        { 0, 0, 1, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 0xff, 0xff }, 
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff }, 
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff }, 
        { 0xff, 0xff, 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 1, 0, 0, 1, 1, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
    };
    // Number of iso vertices for DMC for each voxel config, this is LUT_1.
    // Voxel with 3B config with its adjacent voxel being 2B config CANNOT use this LUT.
    const unsigned num_vertex_lut1[256] =
    {
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1,
        1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1,
        1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1,
        1, 1, 2, 2, 1, 2, 2, 2, 2, 2, 4, 3, 2, 2, 3, 2,
        1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 1, 2, 1, 1,
        1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 3, 2, 2, 2, 2, 1,
        1, 1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 2, 1, 1, 1, 2, 1,
        1, 2, 1, 2, 2, 4, 2, 3, 1, 2, 2, 2, 2, 3, 2, 2,
        1, 1, 1, 1, 2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 1,
        1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 2, 1, 1, 1, 1,
        1, 1, 2, 2, 1, 2, 2, 2, 1, 1, 3, 2, 1, 1, 2, 1,
        1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 2, 2, 1, 2, 1, 1,
        1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0
    };
    
    const vertex_index_type config_edge_lut2[256][VOXEL_NUM_EDGES] =
    {
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 1, 0, 0, 1, 1, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 1, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 1, 0, 0xff, 0xff, 1, 0xff, 0xff, 1 },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0, 1, 1, 0, 0xff, 0xff, 1, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 1, 0, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 1, 1, 0, 0xff, 0xff, 1, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 1, 0, 0, 1, 1, 0, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 1, 0xff, 0, 0, 1, 0xff, 0xff, 1 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0, 1, 1, 1, 1, 0xff, 0xff, 1 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 1, 0xff, 0xff, 1, 1, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 1, 0, 0xff, 1, 1, 0xff, 0xff },
        { 1, 1, 0, 0, 0, 1, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0, 0, 1, 1, 0xff, 0xff, 0xff, 1, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 1, 0, 0, 1, 1, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 1, 0, 1, 1, 1, 1, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 1, 1, 0xff, 0, 0, 1, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0, 1, 1, 0xff, 0, 1, 0xff, 0xff, 1, 0xff, 1 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 1, 1, 0, 0, 0xff, 1, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0, 1, 1, 0, 0, 0xff, 0xff, 1, 0xff, 1, 0xff, 1 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0, 0, 1, 1, 0xff, 1, 0xff, 1 },
        { 1, 1, 0xff, 0xff, 0xff, 1, 0, 0, 0xff, 0, 0xff, 0 },
        { 1, 0xff, 0xff, 1, 1, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0xff },
        { 1, 1, 0xff, 0xff, 0xff, 1, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 1, 0xff, 0xff, 1, 1, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 1, 0, 0, 1, 1, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 1, 0, 0xff, 1, 1, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 1, 0, 0xff, 1, 1, 0xff },
        { 0, 1, 1, 0, 0xff, 0, 1, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0, 0xff, 1, 1, 0, 1, 0xff, 1, 1, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 1, 0xff, 0, 0xff, 1, 0, 0, 1 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 1, 0xff, 0, 1, 1, 0 },
        { 0, 0, 0xff, 0xff, 0, 1, 0, 0xff, 1, 1, 0, 0 },
        { 0xff, 1, 0xff, 1, 0xff, 0, 1, 0xff, 0, 0, 1, 1 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 0xff, 1, 0, 0, 1 },
        { 0, 0, 1, 1, 0xff, 0xff, 0xff, 0xff, 0, 0, 1, 1 },
        { 1, 0xff, 1, 0xff, 1, 0, 0xff, 0xff, 0, 0, 1, 1 },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0, 0 },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0, 1, 0, 0, 1, 1 },
        { 1, 0xff, 1, 0xff, 0xff, 0xff, 1, 0, 1, 1, 0, 0 },
        { 0, 1, 1, 0, 0, 2, 1, 3, 2, 2, 3, 3 },
        { 0xff, 0, 0, 0xff, 0xff, 1, 0, 2, 1, 1, 2, 2 },
        { 0xff, 1, 0xff, 1, 1, 0xff, 0xff, 0, 1, 1, 0, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 1, 0, 0, 1, 1 },
        { 0, 0xff, 0xff, 0, 0, 2, 0xff, 1, 2, 2, 1, 1 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 1, 0xff, 0, 1, 1, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 1, 0xff, 0xff, 1, 1, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0, 1, 1, 1, 0, 0xff, 0xff, 1, 0xff, 1, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 1, 1, 0xff, 0, 0, 1, 0, 0xff, 0, 0xff },
        { 1, 0xff, 1, 0xff, 0, 1, 1, 0, 0, 0xff, 0, 0xff },
        { 1, 0, 0, 1, 0xff, 0xff, 0, 1, 1, 0xff, 1, 0xff },
        { 0xff, 1, 1, 0xff, 0, 0xff, 1, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff },
        { 1, 1, 0xff, 0xff, 0, 1, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 1, 1, 0, 0, 0xff, 1, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 1, 1, 1, 1, 1, 0, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 1, 0xff, 0xff, 1, 1 },
        { 1, 2, 2, 1, 1, 0xff, 2, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 1, 0xff, 0xff, 1, 1 },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 1, 0xff, 0xff, 1, 1, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 1, 0xff, 0xff, 1, 1, 0xff, 0xff, 0, 0xff, 0xff, 0, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 1, 0xff, 0xff, 1, 1 },
        { 0, 0, 1, 1, 0, 0xff, 0, 1, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 1, 0xff, 0xff, 1, 1 },
        { 0xff, 0xff, 0, 0, 1, 1, 1, 0, 0xff, 0xff, 1, 1 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 1, 1, 0, 0, 0xff, 1, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff },
        { 1, 1, 0xff, 0xff, 0, 1, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff },
        { 0xff, 1, 1, 0xff, 0, 0xff, 1, 0, 0, 0xff, 0, 0xff },
        { 1, 1, 0, 0, 0xff, 0xff, 1, 0, 1, 0xff, 1, 0xff },
        { 0, 0xff, 0, 0xff, 0, 1, 1, 0, 1, 0xff, 1, 0xff },
        { 0xff, 0xff, 1, 1, 0xff, 0, 0, 1, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0, 1, 1, 0, 0, 1, 0xff, 0xff, 1, 0xff, 1, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff },
        { 1, 0xff, 0xff, 1, 1, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 1, 0xff, 0, 1, 1, 0, 0 },
        { 0, 0xff, 0xff, 0, 1, 0, 0xff, 0, 1, 0, 0, 1 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 1, 0, 0, 1, 1 },
        { 0xff, 1, 0xff, 1, 0, 0xff, 0xff, 1, 0, 1, 1, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0, 1, 0, 0, 1, 1, 0 },
        { 0, 0, 1, 1, 3, 0, 2, 1, 3, 2, 2, 3 },
        { 1, 0xff, 1, 0xff, 0xff, 0xff, 0, 1, 1, 0, 0, 1 },
        { 0xff, 0xff, 0, 0, 2, 0xff, 1, 0, 2, 1, 1, 2 },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0, 0 },
        { 1, 0xff, 1, 0xff, 0, 1, 0xff, 0xff, 0, 1, 1, 0 },
        { 0, 1, 1, 0, 0xff, 0xff, 0xff, 0xff, 0, 1, 1, 0 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 0xff, 1, 0, 0, 1 },
        { 0xff, 1, 0xff, 1, 0xff, 1, 0, 0xff, 1, 0, 0, 1 },
        { 0, 0, 0xff, 0xff, 1, 0, 2, 0xff, 1, 2, 2, 1 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 1, 0xff, 0, 1, 1, 0 },
        { 0xff, 0xff, 0xff, 0xff, 1, 0xff, 0, 0xff, 1, 0, 0, 1 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff },
        { 0xff, 1, 1, 0xff, 1, 1, 0, 1, 0xff, 0, 0, 0xff },
        { 2, 2, 1, 1, 0xff, 2, 0, 1, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 1, 0, 0xff, 1, 1, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 1, 0, 0xff, 1, 1, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 1, 0, 0, 1, 1, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 1, 0xff, 0xff, 1, 1, 0xff },
        { 1, 1, 0xff, 0xff, 0xff, 1, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0xff, 1, 1, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 1, 0xff, 0xff, 1, 1, 0xff, 0, 0, 0xff, 0, 0xff, 0 },
        { 1, 1, 0xff, 0xff, 0xff, 1, 0, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 1, 0xff, 1, 0, 0, 1, 1, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 1, 1, 0, 0, 1, 0xff, 0xff, 0, 0xff, 1, 0xff, 1 },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 1, 1, 0, 0, 0xff, 1, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0 },
        { 1, 0, 0, 1, 0xff, 1, 0, 0xff, 0xff, 1, 0xff, 1 },
        { 0xff, 1, 1, 0xff, 0, 0, 1, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff },
        { 1, 1, 0xff, 0xff, 1, 0, 1, 1, 0, 0, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 1, 0, 0, 1, 1, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0, 0xff, 0xff },
        { 0, 0, 1, 1, 0xff, 0xff, 0xff, 1, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 1, 0xff, 0, 1, 1, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff },
        { 1, 0, 0, 1, 1, 2, 0, 0xff, 2, 2, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 1, 0, 0xff, 1, 1, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 1, 0xff, 0xff, 1, 1, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0 },
        { 1, 0xff, 0xff, 1, 0, 1, 1, 1, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0xff, 0, 1, 0xff, 0, 0, 1, 0xff, 0xff, 1 },
        { 0xff, 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 2, 2, 0, 0, 1, 2, 0xff, 0, 1, 0xff, 0xff, 1 },
        { 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 1, 1, 0, 0xff, 0xff, 1, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0xff, 0, 0xff, 1, 0, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0, 1, 1, 0, 0xff, 0xff, 1, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0, 0, 0xff, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 1 },
        { 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0, 0, 0xff, 0xff, 1, 0, 0xff, 0xff, 1, 0xff, 0xff, 1 },
        { 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0 },
        { 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 1, 1, 0xff, 0, 0xff, 1, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 1, 0, 0, 1, 1, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
    };
    
    const unsigned num_vertex_lut2[256] =
    {
        0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
        1, 1, 2, 1, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
        1, 2, 1, 1, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 1, 1,
        1, 1, 1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1,
        1, 2, 2, 2, 1, 2, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 3, 2, 2, 3, 2,
        1, 2, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 1, 2, 1, 1,
        1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 3, 2, 2, 2, 2, 1,
        1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1,
        1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 2, 1,
        2, 2, 2, 2, 2, 4, 2, 3, 2, 2, 2, 2, 2, 3, 2, 2,
        1, 1, 1, 1, 2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 1,
        1, 2, 2, 2, 1, 2, 1, 2, 1, 1, 2, 2, 1, 1, 1, 1,
        1, 1, 2, 2, 1, 2, 2, 2, 1, 1, 3, 2, 1, 1, 2, 1,
        1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 2, 2, 1, 2, 1, 1,
        1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0 
    };
    
    // Stores the minimum required information for each voxel
    class _VoxelInfo
    {
        typedef uint8_t info_type;
        
        static const uint8_t EDGE_6_SHIFT = 0;
        static const uint8_t EDGE_9_SHIFT = 1;
        static const uint8_t EDGE_10_SHIFT = 2;
        static const uint8_t USE_LUT2_SHIFT = 7;
    public:

        _VoxelInfo() = default;
        _VoxelInfo(voxel_index1D_type index) : index1D(index) { }
        
        void encode_edge_is_bipolar(voxel_edge_index_type edge, bool is_bipolar)
        {
            uint8_t shift = get_edge_shift(edge);
            info_write_bit(shift, is_bipolar);
        }
        
        void encode_edge_bipolar_info(voxel_edge_index_type edge, bool is_bipolar, bool use_ccw)
        {
            uint8_t shift = get_edge_shift(edge);
            info_write_bit(shift, is_bipolar);
            
            if (is_bipolar)
            {
                shift += 3;
                info_write_bit(shift, use_ccw);
            }
        }
        
        bool is_edge_bipolar(voxel_edge_index_type edge) const
        {
            uint8_t shift = get_edge_shift(edge);
            return (bool)info_read_bit(shift);
        }
        // An edge that is 'CCW' means the polarization direction of the edge aligns with the positive axis.
        // [precondition] 'edge' must be bipolar
        bool is_edge_ccw(voxel_edge_index_type edge) const
        {
            assert(is_edge_bipolar(edge));
            
            uint8_t shift = get_edge_shift(edge) + 3;
            return (bool)info_read_bit(shift);
        }
        
        inline void encode_use_lut2(bool use_lut2) { info_write_bit(USE_LUT2_SHIFT, use_lut2); }
        
        inline bool use_lut2() const { return (bool)info_read_bit(USE_LUT2_SHIFT); }
        
        uint8_t num_bipolar_edges() const
        {
            uint8_t num = 0;
            num += info_read_bit(EDGE_6_SHIFT);
            num += info_read_bit(EDGE_9_SHIFT);
            num += info_read_bit(EDGE_10_SHIFT);
            return num;
        }
        
        inline uint8_t num_iso_vertices() const
        {
            // return use_lut2() ? num_vertex_lut2[config] : num_vertex_lut1[config];
            return num_vertices - num_bipolar_edges();
        }
        
        inline unsigned iso_vertex_begin() const { return vertex_begin; }
        
        inline unsigned iso_vertex_index(vertex_index_type iso_vertex_m) const
        {
            assert(iso_vertex_m < num_iso_vertices());
            return iso_vertex_begin() + iso_vertex_m;
        }
        
        inline unsigned edge_vertex_begin() const { return vertex_begin + num_iso_vertices(); }
        
        unsigned edge_vertex_index(voxel_edge_index_type edge) const
        {
            assert(is_edge_bipolar(edge));
            
            uint8_t offset = 0;
            for (uint8_t i = 0; i < get_edge_shift(edge); ++i)
            {
                offset += info_read_bit(i);
            }
            return edge_vertex_begin() + offset;
        }
        
        vertex_index_type iso_vertex_m_by_edge(voxel_edge_index_type edge) const
        {
            if (use_lut2())
            {
                return config_edge_lut2[config][edge];
            }
            else
            {
                return config_edge_lut1[config][edge];
            }
        }
        
        // Its index_1D
        voxel_index1D_type index1D = INVALID_INDEX_1D;
        // The beginning index of the vertices (both DMC iso_vertex and iso-surface edge
        // intersection point).
        unsigned vertex_begin = INVALID_UINT32;
        // The voxel config mask, each bit corresponds to one unique vertex corner point.
        // LSB (bit 0) represents corner pt 0, MSB (bit 7) represents corner pt 7
        voxel_config_type config = 0x00;
        // Compact bit vector:
        // bit 7: should use LUT2?
        // bit 5: is edge 10 using ccw?
        // bit 4: is edge 9 using ccw?
        // bit 3: is edge 6 using ccw?
        // bit 2: is edge 10 bipolar?
        // bit 1: is edge 9 bipolar?
        // bit 0: is edge 6 bipolar?
        // other bits: not used
        info_type info = 0x00;
        // Since this class will be enforeced aligned, we can use another 8 bit to store the
        // number of vertices, although we can fully retrieve this information under the help
        // of both 'info' and other 'LUT's. 8_bit is quite enough because each voxel will have
        // a maximum of 4 + 3 = 7 vertices. (4 for DMC iso-vertices, 3 for bipolar edge pts)
        uint8_t num_vertices = 0;
        
    private:
        uint8_t get_edge_shift(voxel_edge_index_type edge) const
        {
            uint8_t shift;
            switch (edge)
            {
                case 6:
                    shift = EDGE_6_SHIFT;
                    break;
                case 9:
                    shift = EDGE_9_SHIFT;
                    break;
                case 10:
                    shift = EDGE_10_SHIFT;
                    break;
                default:
                    assert(false);
                    break;
            }
            return shift;
        }
        
        void info_write_bit(uint8_t shift, bool flag)
        {
            info_type mask;
            if (flag)
            {
                mask = 0x01 << shift;
                info |= mask;
            }
            else
            {
                mask = ~(0x01 << shift);
                info &= mask;
            }
        }
        
        inline info_type info_read_bit(uint8_t shift) const
        {
            info_type shifted_info = info >> shift;
            return shifted_info & 0x01;
        }
    };
    
    // Calculate a voxel's config mask.
    voxel_config_type voxel_config_mask(const float* voxel_vals, float iso_value)
    {
        voxel_config_type mask = 0;
        for (unsigned i = 0; i < 8; ++i)
        {
            mask |= (voxel_vals[i] < iso_value) << i;
        }
        
        return mask;
    }
    
    // Scan through and flag out the active voxels according to its voxel config.
    void flag_active_voxels(std::vector<flag_type>& voxel_flags,
                            const utils::Array3D<float>& scalar_grid, float iso_value)
    {
        unsigned num_voxels_i = scalar_grid.dim_x() - 1;
        unsigned num_voxels_j = scalar_grid.dim_y() - 1;
        unsigned num_voxels_k = scalar_grid.dim_z() - 1;
        
        voxel_flags.clear();
        voxel_flags.resize(num_voxels_i * num_voxels_j * num_voxels_k);
        
        unsigned index1D;
        for (unsigned k = 0; k < num_voxels_k; ++k)
        {
            for (unsigned j = 0; j < num_voxels_j; ++j)
            {
                for (unsigned i = 0; i < num_voxels_i; ++i)
                {
                    float voxel_vals[8] =
                    {
                        scalar_grid(i,      j,      k       ),
                        scalar_grid(i + 1,  j,      k       ),
                        scalar_grid(i + 1,  j + 1,  k       ),
                        scalar_grid(i,      j + 1,  k       ),
                        scalar_grid(i,      j,      k + 1   ),
                        scalar_grid(i + 1,  j,      k + 1   ),
                        scalar_grid(i + 1,  j + 1,  k + 1   ),
                        scalar_grid(i,      j + 1,  k + 1   ),
                        
                    };
                    
                    voxel_config_type voxel_config = voxel_config_mask(voxel_vals, iso_value);
                    index3D_to_1D(i, j, k, num_voxels_i, num_voxels_j, index1D);
                    voxel_flags[index1D] = (voxel_config && voxel_config < MAX_VOXEL_CONFIG_MASK);
                }
            }
        }
    }
    
    inline void get_num_voxels_dim_from_scalar_grid(uint3& num_voxels_dim,
                                                    const scalar_grid_type& scalar_grid)
    {
        num_voxels_dim.x = scalar_grid.dim_x() - 1;
        num_voxels_dim.y = scalar_grid.dim_y() - 1;
        num_voxels_dim.z = scalar_grid.dim_z() - 1;
    }
    
    // Calculate the position of the isosurface's vertex
    float3 lerp_float3(const float3& p0, const float3& p1, float v0, float v1, float iso_value)
    {
        float interp = (iso_value - v0) / (v1 - v0);
        float one_minus_interp = 1.0f - interp;
        
        float3 iso_vertex = p0 * one_minus_interp + p1 * interp;
        
        return iso_vertex;
    }
    
    // Check if an edge is bipolar given its two endpoints' value
    bool is_edge_bipolar(float val0, float val1, float iso_value)
    {
        if (val0 == val1) return false;
        else if (val0 > val1) return is_edge_bipolar(val1, val0, iso_value);
        return !((val0 < iso_value && val1 < iso_value) || (val0 > iso_value && val1 > iso_value));
    }
    
    // Return an edge index given its two point indices
    voxel_edge_index_type pt_pair_edge_lut(voxel_pt_index_type p0, voxel_pt_index_type p1)
    {
        assert(p0 != p1);
        if (p0 > p1)
            return pt_pair_edge_lut(p1, p0);
        
        if      (p0 == 0 && p1 == 1) return 0;
        else if (p0 == 1 && p1 == 2) return 1;
        else if (p0 == 2 && p1 == 3) return 2;
        else if (p0 == 0 && p1 == 3) return 3;
        else if (p0 == 0 && p1 == 4) return 4;
        else if (p0 == 1 && p1 == 5) return 5;
        else if (p0 == 2 && p1 == 6) return 6;
        else if (p0 == 3 && p1 == 7) return 7;
        else if (p0 == 4 && p1 == 5) return 8;
        else if (p0 == 5 && p1 == 6) return 9;
        else if (p0 == 6 && p1 == 7) return 10;
        else if (p0 == 4 && p1 == 7) return 11;

        assert(false);
    }
    
    // Compact to get the active voxels, for each compacted voxel, store its index_1D.
    // [invariant] for 0 <= i < compact_voxel_info.size(),
    //                  full_voxel_index_map[compact_voxel_info[i].index1D] == i
    void compact_voxel_flags(std::vector<_VoxelInfo>& compact_voxel_info,
                             std::vector<voxel_index1D_type>& full_voxel_index_map,
                             const std::vector<flag_type>& flags)
    {
        compact_voxel_info.clear();
        full_voxel_index_map.clear();
        full_voxel_index_map.resize(flags.size());
        std::fill(full_voxel_index_map.begin(), full_voxel_index_map.end(), INVALID_UINT32);
        
        for (voxel_index1D_type index1D = 0; index1D < flags.size(); ++index1D)
        {
            if (flags[index1D])
            {
                full_voxel_index_map[index1D] = (unsigned)compact_voxel_info.size();
                compact_voxel_info.push_back(_VoxelInfo(index1D));
            }
        }
    }
    
    // Initialize the voxel info. During this stage we only store the voxel config and
    // the edges this voxel manages (edge 6, 9, 10) are bipolar. The possible situation
    // where voxels with 2B config and 3B config are adjacent are not resolved at this stage.
    void init_voxels_info(std::vector<_VoxelInfo>& compact_voxel_info,
                          const scalar_grid_type& scalar_grid, float iso_value)
    {
        
        uint3 num_voxels_dim;
        get_num_voxels_dim_from_scalar_grid(num_voxels_dim, scalar_grid);
        
        for (_VoxelInfo& vx_info : compact_voxel_info)
        {
            // index1D = compact_voxel_info[i].index1D;
            voxel_index1D_type index1D = vx_info.index1D;
            uint3 index3D;
            index1D_to_3D(index1D, num_voxels_dim, index3D);
            // Calculate the voxel config by eight voxel points value
            const float voxel_vals[8] =
            {
                scalar_grid(index3D.x,     index3D.y,     index3D.z    ),
                scalar_grid(index3D.x + 1, index3D.y,     index3D.z    ),
                scalar_grid(index3D.x + 1, index3D.y + 1, index3D.z    ),
                scalar_grid(index3D.x,     index3D.y + 1, index3D.z    ),
                scalar_grid(index3D.x,     index3D.y,     index3D.z + 1),
                scalar_grid(index3D.x + 1, index3D.y,     index3D.z + 1),
                scalar_grid(index3D.x + 1, index3D.y + 1, index3D.z + 1),
                scalar_grid(index3D.x,     index3D.y + 1, index3D.z + 1)
            };
            voxel_config_type voxel_config = voxel_config_mask(voxel_vals, iso_value);
            vx_info.config = voxel_config;
            // Calculate if the three edges 6, 9, 10 are bipolar
            auto encode_voxel_edge_info = [=, &vx_info](voxel_pt_index_type p0, voxel_pt_index_type p1)
            {
                voxel_edge_index_type edge_index = pt_pair_edge_lut(p0, p1);
                /*std::cout << "p0: " << (unsigned)p0 << ", p1: " << (unsigned)p1
                << "val0: " << voxel_vals[p0] << ", val1: " << voxel_vals[p1] << std::endl;
                */
                bool is_bipolar = is_edge_bipolar(voxel_vals[p0], voxel_vals[p1], iso_value);
                if (is_bipolar)
                {
                    bool use_ccw = voxel_vals[p0] <= iso_value;
                    if (use_ccw)
                    {
                        assert(voxel_vals[p1] >= iso_value);
                    }
                    else
                    {
                        assert(voxel_vals[p1] <= iso_value);
                    }
                    // assert(use_ccw ? voxel_vals[p1] >= iso_value : voxel_vals[p1] <= iso_value);

                    vx_info.encode_edge_bipolar_info(edge_index, is_bipolar, use_ccw);
                }
                else
                {
                    vx_info.encode_edge_is_bipolar(edge_index, is_bipolar);
                }
            };
            
            encode_voxel_edge_info(2, 6);   // edge 6
            encode_voxel_edge_info(5, 6);   // edge 9
            encode_voxel_edge_info(7, 6);   // edge 10
        }
    }
    
    const unsigned AMBIGUOUS_CONFIG_SIZE = 36;
    const voxel_config_type config_2B_3B_lut[AMBIGUOUS_CONFIG_SIZE] =
    {
        0xa0, 0x21, 0x42, 0x84, 0x05, 0x81, 0x48, 0x0a, 0x50, 0x12, 0x18, 0x24, // 2B
        0xc1, 0xc2, 0x83, 0x45, 0x86, 0x49, 0x8a, 0x51, 0x92, 0x43, 0x54, 0x15, // 3B
        0x16, 0x1c, 0x61, 0xa2, 0xa8, 0x29, 0x2a, 0x2c, 0x68, 0x34, 0x38, 0x94
    };
    
    const voxel_face_index_type config_2B_3B_ambiguous_face[AMBIGUOUS_CONFIG_SIZE] =
    {
        5,    1,    2,    3,    0,    4,    3,    0,    5,    1,    4,    2,    // 2B
        4,    2,    4,    0,    3,    3,    0,    5,    1,    2,    5,    0,    // 3B
        1,    4,    1,    5,    5,    1,    0,    2,    3,    2,    4,    3
    };
    
    const voxel_face_index_type opposite_face_lut[6] = {5, 3, 4, 1, 2, 0};
    
    enum class CHECK_DIR { PX, NX, PY, NY, PZ, NZ };
    
    const CHECK_DIR face_to_check_dir_lut[6] =
    {
        CHECK_DIR::NZ, CHECK_DIR::NY, CHECK_DIR::PX, CHECK_DIR::PY, CHECK_DIR::NX, CHECK_DIR::PZ
    };
    
    // Check if the given voxel config belongs to 2B or 3B ambiguous config category.
    bool is_ambiguous_config(voxel_config_type config, uint8_t& index)
    {
        auto result = std::find(config_2B_3B_lut, config_2B_3B_lut + AMBIGUOUS_CONFIG_SIZE, config);
        
        if (result != config_2B_3B_lut + AMBIGUOUS_CONFIG_SIZE)
        {
            index = (uint8_t)(result - config_2B_3B_lut);
            return true;
        }
        return false;
    }
    
    // Check if after we advance the 'index3D' according to 'dir', the new result will
    // exceed the boundary or not. Have to use this function because we are using unsigned
    // int instead of int.
    bool will_exceed_boundary(uint3 index3D, uint3 dims, CHECK_DIR dir)
    {
        switch (dir) {
            case CHECK_DIR::PX:
                return index3D.x + 1 >= dims.x;
                
            case CHECK_DIR::NX:
                return index3D.x == 0;
            
            case CHECK_DIR::PY:
                return index3D.y + 1 >= dims.y;
                
            case CHECK_DIR::NY:
                return index3D.y == 0;
                
            case CHECK_DIR::PZ:
                return index3D.z + 1 >= dims.z;
                
            case CHECK_DIR::NZ:
                return index3D.z == 0;
                
            default:
                return false;
        }
    }
    
    // Execute the 'dir' on 'index3D' to get the new result. It is the user's responsibility
    // to make sure that the result won't exceed the boundary.
    uint3 get_index3D_by_dir(uint3 index3D, CHECK_DIR dir)
    {
        switch (dir) {
            case CHECK_DIR::PX:
                return make_uint3(index3D.x + 1, index3D.y, index3D.z);
            case CHECK_DIR::NX:
                return make_uint3(index3D.x - 1, index3D.y, index3D.z);
            case CHECK_DIR::PY:
                return make_uint3(index3D.x, index3D.y + 1, index3D.z);
            case CHECK_DIR::NY:
                return make_uint3(index3D.x, index3D.y - 1, index3D.z);
            case CHECK_DIR::PZ:
                return make_uint3(index3D.x, index3D.y, index3D.z + 1);
            case CHECK_DIR::NZ:
                return make_uint3(index3D.x, index3D.y, index3D.z - 1);
        }
    }
    
    // Check if the active voxel indicated by 'cur_compact_index' has an adjacent voxel which has
    // an ambiguous config that will result in non-manifold situation.
    // [precondition] compact_voxel_info[cur_compact_index].config == config_2B_3B_lut[cur_config_index]
    bool is_adjacent_ambiguous_config(voxel_index1D_type& adjacent_compact_index,
                                      voxel_index1D_type cur_compact_index, uint8_t cur_config_index,
                                      const std::vector<_VoxelInfo>& compact_voxel_info,
                                      const std::vector<voxel_index1D_type>& full_voxel_index_map,
                                      const uint3& num_voxels_dim)
    {
        assert(compact_voxel_info[cur_compact_index].config == config_2B_3B_lut[cur_config_index]);
        // Get the 3D coordinate of the current active voxel
        uint3 cur_index3D;
        index1D_to_3D(compact_voxel_info[cur_compact_index].index1D, num_voxels_dim, cur_index3D);
        // Get the checking direction, or offset, according to 'cur_ambiguous_face'
        voxel_face_index_type cur_ambiguous_face = config_2B_3B_ambiguous_face[cur_config_index];
        CHECK_DIR dir = face_to_check_dir_lut[cur_ambiguous_face];
        
        if (will_exceed_boundary(cur_index3D, num_voxels_dim, dir))
        {
            return false;
        }
        
        // Compute the index of the voxel to be checked in 'compact_voxel_info'
        uint3 index3D_to_check = get_index3D_by_dir(cur_index3D, dir);
        voxel_index1D_type index1D_to_check;
        index3D_to_1D(index3D_to_check, num_voxels_dim, index1D_to_check);
        
        voxel_index1D_type adjc_compact_index_to_check = full_voxel_index_map[index1D_to_check];
        assert(adjc_compact_index_to_check != INVALID_INDEX_1D);
        
        uint8_t adj_config_index;
        if (is_ambiguous_config(compact_voxel_info[adjc_compact_index_to_check].config, adj_config_index))
        {
            voxel_face_index_type adj_ambiguous_face = config_2B_3B_ambiguous_face[adj_config_index];
            assert(opposite_face_lut[cur_ambiguous_face] == adj_ambiguous_face);
            adjacent_compact_index = adjc_compact_index_to_check;
            return true;
        }
        
        return false;
    }

    // Correct some of the voxels when it and its adjacent voxel are having ambiguous configs that will
    // result in non-manifold. Returns the actual number of vertices, including both iso-vertex and
    // intersection vertex between voxel bipolar edge and iso-surface.
    unsigned correct_voxels_info(std::vector<_VoxelInfo>& compact_voxel_info,
                                 const std::vector<voxel_index1D_type>& full_voxel_index_map,
                                 const uint3& num_voxels_dim)
    {
        for (unsigned compact_index = 0; compact_index < compact_voxel_info.size(); ++compact_index)
        {
            uint8_t ambiguous_config_index = INVALID_UINT8;
            
            if ((compact_voxel_info[compact_index].use_lut2()) ||
                (!is_ambiguous_config(compact_voxel_info[compact_index].config, ambiguous_config_index)))
            {
                continue;
            }
            
            voxel_index1D_type adjacent_compact_index;
            if (is_adjacent_ambiguous_config(adjacent_compact_index, compact_index, ambiguous_config_index,
                                             compact_voxel_info, full_voxel_index_map, num_voxels_dim))
            {
                compact_voxel_info[compact_index].encode_use_lut2(true);
                compact_voxel_info[adjacent_compact_index].encode_use_lut2(true);
            }
        }
        // At this moment, all the voxels' @info are properly set. We can calculate
        // how many points are needed for each active voxel.
        unsigned num_total_vertices = 0;
        for (_VoxelInfo& vx_info : compact_voxel_info)
        {
            uint8_t num_voxel_vertices = 0;
            vx_info.vertex_begin = num_total_vertices;
            
            if (vx_info.use_lut2())
            {
                num_voxel_vertices += num_vertex_lut2[vx_info.config];
            }
            else
            {
                num_voxel_vertices += num_vertex_lut1[vx_info.config];
            }
            num_voxel_vertices += vx_info.num_bipolar_edges();
            vx_info.num_vertices = num_voxel_vertices;
            num_total_vertices += num_voxel_vertices;
        }
        return num_total_vertices;
    }
    
    uint16_t LOCAL_EDGE_ENTRY = 0xffff;
    // local edges: 6, 9, 10
    uint16_t edge_belonged_voxel_lut[VOXEL_NUM_EDGES] =
    {
        ( 0x0000 | 0x3000 | 0x0c00 ) | 10,  // 0
        ( 0x0000 | 0x0000 | 0x0c00 ) | 9,   // 1
        ( 0x0000 | 0x0000 | 0x0c00 ) | 10,  // 2
        ( 0xc000 | 0x0000 | 0x0c00 ) | 9,   // 3
        ( 0xc000 | 0x3000 | 0x0000 ) | 6,   // 4
        ( 0x0000 | 0x3000 | 0x0000 ) | 6,   // 5
                LOCAL_EDGE_ENTRY,           // 6
        ( 0xc000 | 0x0000 | 0x0000 ) | 6,   // 7
        ( 0x0000 | 0x3000 | 0x0000 ) | 10,  // 8
                LOCAL_EDGE_ENTRY,           // 9
                LOCAL_EDGE_ENTRY,           // 10
        ( 0xc000 | 0x0000 | 0x0000 ) | 9,   // 11
    };
    
    void decode_edge_belong_voxel_entry(uint16_t entry,
                                        int8_t& x_offset, int8_t& y_offset, int8_t& z_offset,
                                        uint8_t& belonged_edge_index)
    {
        if (entry == 0xffff) return;
        // extract the edge
        belonged_edge_index = 0x00ff & entry;
        
        auto get_offset = [](uint16_t two_bits)
        {
            switch (two_bits) {
                case 0x0000:
                    return (int8_t)0;
                case 0x4000:
                    return (int8_t)1;
                case 0xc000:
                    return (int8_t)-1;
                default:
                    assert(false);
            }
        };
        
        uint16_t two_bits = entry & 0xc000;
        x_offset = get_offset(two_bits);
        
        entry <<= 2;
        two_bits = entry & 0xc000;
        y_offset = get_offset(two_bits);
        
        entry <<= 2;
        two_bits = entry & 0xc000;
        z_offset = get_offset(two_bits);
    }
    // Sample the intersection vertices positions between voxel bipolar edges and iso-surface.
    // Each voxel is only responsible for its local edges, namely 6, 9 and 10.
    void sample_edge_intersection_vertices(std::vector<float3>& compact_vertices,
                                           const std::vector<_VoxelInfo>& compact_voxel_info,
                                           const scalar_grid_type& scalar_grid,
                                           const float3& xyz_min, const float3& xyz_max, float iso_value)
    {
        float3 xyz_range = xyz_max - xyz_min;
        
        uint3 num_voxels_dim;
        get_num_voxels_dim_from_scalar_grid(num_voxels_dim, scalar_grid);
        
        
        
        for (const _VoxelInfo& vx_info : compact_voxel_info)
        {
            voxel_index1D_type index1D = vx_info.index1D;
            uint3 index3D;
            index1D_to_3D(index1D, num_voxels_dim, index3D);
            
            float x0 = ijk_to_xyz(index3D.x,     num_voxels_dim.x, xyz_range.x, xyz_min.x);
            float x1 = ijk_to_xyz(index3D.x + 1, num_voxels_dim.x, xyz_range.x, xyz_min.x);
            float y0 = ijk_to_xyz(index3D.y,     num_voxels_dim.y, xyz_range.y, xyz_min.y);
            float y1 = ijk_to_xyz(index3D.y + 1, num_voxels_dim.y, xyz_range.y, xyz_min.y);
            float z0 = ijk_to_xyz(index3D.z,     num_voxels_dim.z, xyz_range.z, xyz_min.z);
            float z1 = ijk_to_xyz(index3D.z + 1, num_voxels_dim.z, xyz_range.z, xyz_min.z);
            
            const float3 voxel_corner_pts[8] =
            {
                {x0, y0, z0},
                {x1, y0, z0},
                {x1, y1, z0},
                {x0, y1, z0},
                {x0, y0, z1},
                {x1, y0, z1},
                {x1, y1, z1},
                {x0, y1, z1}
            };
            
            const float voxel_vals[8] =
            {
                scalar_grid(index3D.x,     index3D.y,     index3D.z    ),
                scalar_grid(index3D.x + 1, index3D.y,     index3D.z    ),
                scalar_grid(index3D.x + 1, index3D.y + 1, index3D.z    ),
                scalar_grid(index3D.x,     index3D.y + 1, index3D.z    ),
                scalar_grid(index3D.x,     index3D.y,     index3D.z + 1),
                scalar_grid(index3D.x + 1, index3D.y,     index3D.z + 1),
                scalar_grid(index3D.x + 1, index3D.y + 1, index3D.z + 1),
                scalar_grid(index3D.x,     index3D.y + 1, index3D.z + 1)
            };
            
            unsigned vx_vertex_index = vx_info.edge_vertex_begin();
            auto calc_edge_vertex = [&](uint8_t edge_index, uint8_t pt0, uint8_t pt1)
            {
                if (vx_info.is_edge_bipolar(edge_index))
                {
                    compact_vertices[vx_vertex_index] = lerp_float3(voxel_corner_pts[pt0], voxel_corner_pts[pt1],
                                                                    voxel_vals[pt0], voxel_vals[pt1], iso_value);
                    vx_vertex_index += 1;
                }
            };
            
            calc_edge_vertex(6, 2, 6);      // edge 6, pt 2, 6
            calc_edge_vertex(9, 5, 6);      // edge 9, pt 5, 6
            calc_edge_vertex(10, 6, 7);     // edge 10, pt 6, 7
            
            assert(vx_vertex_index - vx_info.edge_vertex_begin() == vx_info.num_bipolar_edges());
        }
    }
    
    // Calculate the iso vertices positions in each voxel.
    void calc_iso_vertices(std::vector<float3>& compact_vertices, const std::vector<_VoxelInfo>& compact_voxel_info,
                           const std::vector<voxel_index1D_type>& full_voxel_index_map, const uint3& num_voxels_dim)
    {
        for (const _VoxelInfo& vx_info : compact_voxel_info)
        {
            voxel_index1D_type index1D = vx_info.index1D;
            uint3 index3D;
            index1D_to_3D(index1D, num_voxels_dim, index3D);
            
            // const vertex_index_type (*vx_config_edge_lut)[VOXEL_NUM_EDGES];
            // vx_config_edge_lut = vx_info.use_lut2() ? config_edge_lut2 : config_edge_lut1;
            uint8_t iso_vertex_num_incident[4] = {0, 0, 0, 0};
            
            for (voxel_edge_index_type edge = 0; edge < VOXEL_NUM_EDGES; ++edge)
            {
                int8_t x_offset = 0xff, y_offset = 0xff, z_offset = 0xff;
                
                // vx_config_edge_lut[vx_info.config][edge];
                vertex_index_type iso_vertex_m = vx_info.iso_vertex_m_by_edge(edge);

                if (iso_vertex_m == NO_VERTEX)
                {
                    continue;
                }
                
                // Find out the voxel which is responsible for 'edge'. From that voxel we can retrieve
                // the edge intersect vertex index.
                uint16_t entry = edge_belonged_voxel_lut[edge];
                voxel_edge_index_type belonged_edge = 0xff;
                voxel_index1D_type belonged_index1D = INVALID_INDEX_1D;
                
                if (entry == LOCAL_EDGE_ENTRY)
                {
                    // edge belongs to current voxel
                    belonged_index1D = index1D;
                    belonged_edge = edge;
                }
                else
                {
                    decode_edge_belong_voxel_entry(entry, x_offset, y_offset, z_offset, belonged_edge);
                    // here the voxel we want may actually exceed boundary, so we just ignore it.
                    bool exceed_boundary  = (x_offset < 0 && index3D.x == 0) ||
                                            (y_offset < 0 && index3D.y == 0) ||
                                            (z_offset < 0 && index3D.z == 0);
                    if (exceed_boundary)
                    {
                        continue;
                    }
                    
                    index3D_to_1D(index3D.x + x_offset, index3D.y + y_offset, index3D.z + z_offset,
                                  num_voxels_dim.x, num_voxels_dim.y, belonged_index1D);
                    assert(full_voxel_index_map[belonged_index1D] != INVALID_UINT32);
                }
                // Get the 'belonged_voxel' which manages 'belonged_edge'
                const _VoxelInfo& belonged_vx_info = compact_voxel_info[full_voxel_index_map[belonged_index1D]];
                unsigned edge_intersect_vertex_index = belonged_vx_info.edge_vertex_index(belonged_edge);
                
                unsigned iso_vertex_index = vx_info.iso_vertex_index(iso_vertex_m);
                if (iso_vertex_num_incident[iso_vertex_m] == 0)
                {
                    // If this is the first time we see 'iso_vertex_m', we just assign it
                    compact_vertices[iso_vertex_index] = compact_vertices[edge_intersect_vertex_index];
                }
                else
                {
                    // Otherwise we increase it
                    compact_vertices[iso_vertex_index] += compact_vertices[edge_intersect_vertex_index];
                }
                
                ++iso_vertex_num_incident[iso_vertex_m];
            }
            // For each iso-vertex managed by 'vx_info', calculate its new position by averaging its
            // associated edges intersection vertex positions.
            vertex_index_type iso_vertex_m = 0;
            for (; iso_vertex_m < vx_info.num_iso_vertices(); ++iso_vertex_m)
            {
                unsigned iso_vertex_index = vx_info.iso_vertex_index(iso_vertex_m);
                if (iso_vertex_num_incident[iso_vertex_m])
                {
                    compact_vertices[iso_vertex_index] /= (float)(iso_vertex_num_incident[iso_vertex_m]);
                }
            }
            // post check
            if (vx_info.use_lut2())
            {
                assert(iso_vertex_m == num_vertex_lut2[vx_info.config]);
            }
            else
            {
                assert(iso_vertex_m == num_vertex_lut1[vx_info.config]);
            }
        }
    }
    
    // Same edge shared by four voxels. Default in CCW order when looking align the positive
    // direction of the axis.
    voxel_edge_index_type circular_edge_lut[3][4] =
    {
        {6, 7, 4, 5},
        {9, 1, 3, 11},
        {10, 8, 0, 2}
    };
    uint8_t ccw_order[4] = {0, 1, 2, 3};
    uint8_t cw_order[4]  = {0, 3, 2, 1};
    
    class CircularEdgeRange
    {
    public:
        class CircularEdgeIterator
        {
        public:
            typedef CircularEdgeIterator iterator_type;
            
            CircularEdgeIterator(voxel_edge_index_type edge, bool ccw)
            : m_lut_index(get_lut_index_by_edge(edge)), m_cur_state(0), m_ccw(ccw) { }
            // For end iterator
            CircularEdgeIterator(voxel_edge_index_type edge)
            : m_lut_index(get_lut_index_by_edge(edge)), m_cur_state(4), m_ccw(true) { }
            
            // We are using CircularEdgeIterator itself, it does not represent any data underlying it. However,
            // for range object to work in c++11, we have to define dereference opreator*(). Therefore we let
            // it to dereference to itself.
            const CircularEdgeIterator& operator*() const { return *this; }
            
            // We've been lazy here and only compares 'm_lut_index' and 'm_cur_state'.
            // It's not absolutely safe, but we don't expect the client should use this class at all!
            bool operator==(const iterator_type& other) const
            {
                return (m_lut_index == other.m_lut_index) && (m_cur_state == other.m_cur_state);
            }
            
            bool operator!=(const iterator_type& other) const { return !(this->operator==(other)); }
            
            iterator_type& operator++()
            {
                if (m_cur_state < 4) ++m_cur_state;
                return (*this);
            }
            
            // Retrieve the information of the adjacent voxel that shares the edge, along with
            // the edge index in that voxel, in circular order
            void retrieve(uint3& circular_index3D, voxel_edge_index_type& circular_edge,
                         const uint3& src_index3D) const
            {
                if (m_ccw)
                {
                    circular_edge = circular_edge_lut[m_lut_index][ccw_order[m_cur_state]];
                }
                else
                {
                    circular_edge = circular_edge_lut[m_lut_index][cw_order[m_cur_state]];
                }
                // reverse calculate the adjacent voxel that shares the edge
                uint16_t entry = edge_belonged_voxel_lut[circular_edge];
                if (entry == LOCAL_EDGE_ENTRY)
                {
                    circular_index3D = src_index3D;
                }
                else
                {
                    int8_t x_offset, y_offset, z_offset;
                    voxel_edge_index_type src_edge;
                    decode_edge_belong_voxel_entry(entry, x_offset, y_offset, z_offset, src_edge);
                    assert(get_lut_index_by_edge(src_edge) == m_lut_index);
                
                    x_offset = -x_offset; y_offset = -y_offset; z_offset = -z_offset;
                    circular_index3D = src_index3D;
                    circular_index3D.x += x_offset;
                    circular_index3D.y += y_offset;
                    circular_index3D.z += z_offset;
                }
            }
            
        private:
            uint8_t get_lut_index_by_edge(voxel_edge_index_type edge) const
            {
                if (edge == 6) return 0;
                else if (edge == 9) return 1;
                else if (edge == 10) return 2;
                assert(false);
            }
            
            uint8_t m_lut_index;
            uint8_t m_cur_state;
            bool m_ccw;
            
        };
        
        CircularEdgeRange(voxel_edge_index_type edge, bool ccw = true)
        : m_edge(edge), m_ccw(ccw) { }
        
        CircularEdgeIterator begin() const { return {m_edge, m_ccw}; }
        CircularEdgeIterator end() const { return {m_edge}; }
        
    private:

        uint8_t m_edge;
        bool m_ccw;
    };
    
    // Check, when we want to retrieve all the four voxels sharing the same 'edge', if any of these voxels
    // will actually exceed the boundary. Notice that all the circular edges are carefully designed so that
    // the adjacent voxels will only increase their position along the positive axis direction.
    bool circular_edge_exceed_boundary(voxel_edge_index_type edge, const uint3& index3D, const uint3& num_voxels_dim)
    {
        switch (edge) {
            case 6:
                return (index3D.x + 1 >= num_voxels_dim.x) || (index3D.y + 1 >= num_voxels_dim.y);
            case 9:
                return (index3D.x + 1 >= num_voxels_dim.x) || (index3D.z + 1 >= num_voxels_dim.z);
            case 10:
                return (index3D.y + 1 >= num_voxels_dim.y) || (index3D.z + 1 >= num_voxels_dim.z);
            default:
                assert(false);
        }
    }
    
    // Genreate the actual triangles information of the mesh.
    void generate_triangles(std::vector<uint3>& compact_triangles,
                            const std::vector<float3>& compact_vertices,
                            const std::vector<_VoxelInfo>& compact_voxel_info,
                            const std::vector<voxel_index1D_type>& full_voxel_index_map,
                            const uint3& num_voxels_dim)
    {
        compact_triangles.clear();
        
        static const std::vector<voxel_edge_index_type> edges_vec = {6, 9, 10};
        
        for (const _VoxelInfo& vx_info : compact_voxel_info)
        {
            uint3 index3D;
            index1D_to_3D(vx_info.index1D, num_voxels_dim, index3D);
            /*
            std::cout << "index1D: " << vx_info.index1D
            << " index3D: " << index3D
            << " num vertices: " << (unsigned)vx_info.num_vertices
            << " num iso_vertices: " << (unsigned)vx_info.num_iso_vertices()
            << " config: " << std::bitset<8>(vx_info.config)
            << " info: " << std::bitset<8>(vx_info.info) << std::endl;
            */
            for (voxel_edge_index_type edge : edges_vec)
            {
                /*
                std::cout << "is edge " << (unsigned)edge << " bipolar: "
                << vx_info.is_edge_bipolar(edge) << std::endl;
                std::cout << "edge exceeds boundary: "
                << circular_edge_exceed_boundary(edge, index3D, num_voxels_dim) << std::endl;
                */
                if ((!vx_info.is_edge_bipolar(edge)) ||
                    circular_edge_exceed_boundary(edge, index3D, num_voxels_dim))
                {
                    continue;
                }
                
                std::vector<unsigned> iso_vertices;
                for (auto circular_edge_iter : CircularEdgeRange(edge, vx_info.is_edge_ccw(edge)))
                {
                    uint3 circular_index3D;
                    voxel_edge_index_type circular_edge;
                    circular_edge_iter.retrieve(circular_index3D, circular_edge, index3D);
                    
                    voxel_index1D_type circular_index1D;
                    index3D_to_1D(circular_index3D, num_voxels_dim, circular_index1D);
                    
                    assert(full_voxel_index_map[circular_index1D] != INVALID_INDEX_1D);
                    const _VoxelInfo& circular_vx_info = compact_voxel_info[full_voxel_index_map[circular_index1D]];
                    
                    vertex_index_type circular_iso_vertex_m = circular_vx_info.iso_vertex_m_by_edge(circular_edge);
                    assert(circular_iso_vertex_m != NO_VERTEX);
                    // iso_vertices.push_back(circular_vx_info.iso_vertex_begin() + circular_iso_vertex_m);
                    
                    unsigned circular_iso_vertex_index = circular_vx_info.iso_vertex_index(circular_iso_vertex_m);
                    iso_vertices.push_back(circular_iso_vertex_index);
                }
                assert(iso_vertices.size() == 4);
                
                uint3 tri1 = make_uint3(iso_vertices[0], iso_vertices[1], iso_vertices[2]);
                uint3 tri2 = make_uint3(iso_vertices[2], iso_vertices[3], iso_vertices[0]);
                compact_triangles.push_back(tri1);
                compact_triangles.push_back(tri2);
            }
        }
    }
    
    void run_dmc(std::vector<float3>& compact_vertices, std::vector<uint3>& compact_triangles,
                 const scalar_grid_type& scalar_grid, const float3& xyz_min, const float3& xyz_max, float iso_value)
    {
        uint3 num_voxels_dim;
        get_num_voxels_dim_from_scalar_grid(num_voxels_dim, scalar_grid);
        
        std::vector<flag_type> voxel_flags;
        flag_active_voxels(voxel_flags, scalar_grid, iso_value);
        
        std::vector<_VoxelInfo> compact_voxel_info;
        std::vector<voxel_index1D_type> full_voxel_index_map;
        compact_voxel_flags(compact_voxel_info, full_voxel_index_map, voxel_flags);
        
        init_voxels_info(compact_voxel_info, scalar_grid, iso_value);
        unsigned num_total_vertices = correct_voxels_info(compact_voxel_info, full_voxel_index_map, num_voxels_dim);
        
        compact_vertices.clear();
        compact_vertices.resize(num_total_vertices);
        sample_edge_intersection_vertices(compact_vertices, compact_voxel_info, scalar_grid,
                                          xyz_min, xyz_max, iso_value);
        calc_iso_vertices(compact_vertices, compact_voxel_info, full_voxel_index_map, num_voxels_dim);
        
        compact_triangles.clear();
        generate_triangles(compact_triangles, compact_vertices, compact_voxel_info,
                           full_voxel_index_map, num_voxels_dim);
    }
}; // namespace dmc

#endif /* dmc_h */
