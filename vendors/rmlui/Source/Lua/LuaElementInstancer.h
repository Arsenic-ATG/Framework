/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef RMLUI_LUA_LUAELEMENTINSTANCER_H
#define RMLUI_LUA_LUAELEMENTINSTANCER_H

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementInstancer.h>
#include <RmlUi/Lua/IncludeLua.h>

namespace Rml {
namespace Lua {

class LuaElementInstancer : public ::Rml::ElementInstancer
{
public:
    LuaElementInstancer(lua_State* L);
    /// Instances an element given the tag name and attributes.
	/// @param[in] parent The element the new element is destined to be parented to.
	/// @param[in] tag The tag of the element to instance.
	/// @param[in] attributes Dictionary of attributes.
	ElementPtr InstanceElement(Element* parent, const String& tag, const XMLAttributes& attributes) override;
	/// Releases an element instanced by this instancer.
	/// @param[in] element The element to release.
	void ReleaseElement(Element* element) override;

    int ref_InstanceElement;

    //Pushes on to the top of the stack the table named EVENTINSTNACERFUNCTIONS
    void PushFunctionsTable(lua_State* L);
};

} // namespace Lua
} // namespace Rml
#endif
