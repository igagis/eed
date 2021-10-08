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

#include "tiling_area.hpp"

#include <morda/context.hpp>

using namespace cod;

namespace{
const morda::real minimal_tile_size_dp = 100;
const morda::real dragger_size_dp = 5;
}

namespace{
class dragger : public morda::widget{
public:
    dragger(std::shared_ptr<morda::context> c) :
            morda::widget(std::move(c), treeml::forest())
    {}
};
}

tiling_area::tiling_area(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
        morda::widget(std::move(c), desc),
        morda::oriented_widget(this->context, treeml::forest(), false),
        morda::container(this->context, treeml::forest()),
        min_tile_size(this->context->units.dp_to_px(minimal_tile_size_dp)),
        dragger_size(this->context->units.dp_to_px(dragger_size_dp))
{
    for(const auto& p : desc){
        if(!morda::is_property(p)){
            continue;
        }

        if(p.value == "vertical"){
            this->set_vertical(morda::get_property_value(p).to_bool());
        }
    }

    this->content_container = std::make_shared<morda::container>(this->context, treeml::forest());
    this->morda::container::push_back(this->content_container);
    this->content_container->move_to({0, 0});

    this->content_container->push_back_inflate(desc);
}

void tiling_area::lay_out(){
    std::cout << "rect = " << this->rect() << std::endl;

    auto long_index = this->get_long_index();
    auto trans_index = this->get_trans_index();

    using std::max;

    // calculate current length of all tiles
    morda::real tiles_length = 0;

    for(const auto& t : *this->content_container){
        tiles_length += max(t->rect().d[long_index], this->min_tile_size);
    }

    const auto& content_dims = this->rect().d;

    using std::round;

    // arrange tiles
    if(content_dims[long_index] >= tiles_length){
        morda::vector2 pos{0, 0};
        for(auto& t : *this->content_container){
            morda::real tile_length = max(t->rect().d[long_index], this->min_tile_size);

            morda::vector2 dims;
            dims[trans_index] = content_dims[trans_index];
            dims[long_index] = content_dims[long_index] * (tile_length / tiles_length);
            dims = round(dims);
            std::cout << "dims = " << dims << std::endl;
            t->resize(dims);
            ASSERT(!t->is_layout_dirty())
            t->move_to(pos);
            pos[long_index] += dims[long_index];
        }
    }else{
        morda::real left_length = content_dims[long_index];

        morda::vector2 pos{0, 0};

        for(auto& t : *this->content_container){
            morda::real tile_length = max(t->rect().d[long_index], this->min_tile_size);

            morda::vector2 dims;
            dims[trans_index] = content_dims[trans_index];
            dims[long_index] = left_length * (tile_length / tiles_length);
            if(dims[long_index] <= this->min_tile_size){
                dims[long_index] = this->min_tile_size;
            }
            tiles_length -= tile_length;
            left_length -= dims[long_index];
            dims = round(dims);

            t->resize(dims);
            t->move_to(pos);
            pos[long_index] += dims[long_index];
        }
    }

    this->content_container->resize(content_dims);

    // lay out draggers

    ASSERT(this->size() >= 1)

    auto num_draggers = this->content_container->size() == 0 ? 0 : this->content_container->size() - 1;

    // remove redundant draggers
    while(this->size() - 1 > num_draggers){
        this->pop_back();
    }

    // add missing draggers
    while(this->size() - 1 < num_draggers){
        this->push_back(std::make_shared<dragger>(this->context));
    }

    // TODO:
}

morda::vector2 tiling_area::measure(const morda::vector2& quotum)const{
    auto long_index = this->get_long_index();

    morda::vector2 ret;

    for(size_t i = 0; i != quotum.size(); ++i){
        if(quotum[i] < 0){
            ret[i] = this->min_tile_size;

            if(i == long_index){
                ret[i] *= this->content_container->size();
            }
        }else{
            ret[i] = quotum[i];
        }
    }

    return ret;
}