/***************************************************************************
* Copyright (c) 2017, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XOBJECT_HPP
#define XOBJECT_HPP

#include <string>
#include <unordered_map>

#include "xtl/xoptional.hpp"

#include "xproperty/xobserved.hpp"

#include "xmaterialize.hpp"
#include "xtransport.hpp"

namespace xtl
{
    /***********************************************************
     * to_json and from_json specialization for xtl::xoptional *
     ***********************************************************/

    template <class D>
    void to_json(xeus::xjson& j, const xoptional<D>& o)
    {
        if (!o.has_value())
        {
            j = nullptr;
        }
        else
        {
            j = o.value();
        }
    }

    template <class D>
    void from_json(const xeus::xjson& j, xoptional<D>& o)
    {
        if (j.is_null())
        {
            o = missing<D>();
        }
        else
        {
            o = j.get<D>();
        }
    }
}

namespace xw
{

    /****************************
     * base xobject declaration *
     ****************************/

    template <class D>
    class xobject : public xp::xobserved<D>, public xtransport<D>
    {
    public:

        using base_type = xtransport<D>;
        using derived_type = D;

        using base_type::derived_cast;

        xeus::xjson get_state() const;
        void apply_patch(const xeus::xjson&);

        XPROPERTY(xtl::xoptional<std::string>, derived_type, _model_module, "@jupyter-widgets/base");
        XPROPERTY(xtl::xoptional<std::string>, derived_type, _model_module_version, "^1.0.0");
        XPROPERTY(xtl::xoptional<std::string>, derived_type, _model_name, "WidgetModel");
        XPROPERTY(xtl::xoptional<std::string>, derived_type, _view_module, "@jupyter-widgets/base");
        XPROPERTY(xtl::xoptional<std::string>, derived_type, _view_module_version, "^1.0.0");
        XPROPERTY(xtl::xoptional<std::string>, derived_type, _view_name, "WidgetView");

        using base_type::notify;

    protected:

        xobject();
        xobject(xeus::xcomm&&, bool owning = false);
        using concrete_type = xconcrete_type_t<D>; 

        concrete_type* self();
        const concrete_type* self() const;
    };

    /*******************************
     * base xobject implementation *
     *******************************/

    #define XOBJECT_SET_PROPERTY_FROM_PATCH(name, patch)                   \
    if (patch.find(#name) != patch.end())                                  \
    {                                                                      \
        name = patch.at(#name).get<typename decltype(name)::value_type>(); \
    }

    #define XOBJECT_SET_PATCH_FROM_PROPERTY(name, patch) \
    patch[#name] = this->name();

    template <class D>
    inline xeus::xjson xobject<D>::get_state() const
    {
        xeus::xjson state;

        XOBJECT_SET_PATCH_FROM_PROPERTY(_model_module, state);
        XOBJECT_SET_PATCH_FROM_PROPERTY(_model_module_version, state);
        XOBJECT_SET_PATCH_FROM_PROPERTY(_model_name, state);
        XOBJECT_SET_PATCH_FROM_PROPERTY(_view_module, state);
        XOBJECT_SET_PATCH_FROM_PROPERTY(_view_module_version, state);
        XOBJECT_SET_PATCH_FROM_PROPERTY(_view_name, state);

        return state;
    }

    template <class D>
    inline void xobject<D>::apply_patch(const xeus::xjson& patch)
    {
        XOBJECT_SET_PROPERTY_FROM_PATCH(_model_module, patch);
        XOBJECT_SET_PROPERTY_FROM_PATCH(_model_module_version, patch);
        XOBJECT_SET_PROPERTY_FROM_PATCH(_model_name, patch);
        XOBJECT_SET_PROPERTY_FROM_PATCH(_view_module, patch);
        XOBJECT_SET_PROPERTY_FROM_PATCH(_view_module_version, patch);
        XOBJECT_SET_PROPERTY_FROM_PATCH(_view_name, patch);
    }

    template <class D>
    inline xobject<D>::xobject()
        : base_type()
    {
    }

    template <class D>
    inline xobject<D>::xobject(xeus::xcomm&& com, bool owning)
        : base_type(std::move(com), owning)
    {
    }

    template <class D>
    inline auto xobject<D>::self() -> concrete_type*
    {
        return reinterpret_cast<concrete_type*>(this);
    }

    template <class D>
    inline auto xobject<D>::self() const -> const concrete_type*
    {
        return reinterpret_cast<const concrete_type*>(this);
    }
}

#endif
