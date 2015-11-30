//
//  png_loader.h
//  MarchingTetrahedra
//
//  Created by Ye Kuang on 11/19/15.
//  Copyright © 2015 Ye Kuang. All rights reserved.
//

#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include <vector>
#include <string>

namespace png_load
{
    class PngLoader
    {
    public:
        typedef unsigned char data_type;
        
        PngLoader(const std::string& file_prefix, unsigned num_slices, const std::string& file_suffix = ".png");
        
        void load(unsigned slice_k, std::vector<data_type>& png_data, unsigned& width, unsigned& height) const;
        
        void load(unsigned slice_k, std::vector<data_type>& png_data) const;
        
        std::vector<data_type> load(unsigned slice_k) const;
        
    private:
        std::string m_file_prefix;
        unsigned m_num_slices;
        std::string m_file_suffix;
    };
}; // namespace png_load


#endif /* png_loader_h */
