/*
cod - text editor

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include "tiling_area.hpp"

#include <morda/widgets/button/tab.hpp>

namespace cod{

class file_opener{
    std::shared_ptr<tiling_area> base_tiling_area;

    std::map<std::string, std::shared_ptr<morda::tab>> open_files;
public:
    file_opener(std::shared_ptr<tiling_area> base_tiling_area) :
            base_tiling_area(std::move(base_tiling_area))
    {
        ASSERT(this->base_tiling_area)
    }
    
    void open(const std::string& file_name);
};

}
