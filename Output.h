/*----------------------------------------------------------------------------
 ckdoc
 
 Copyright (c) 2012 Spencer Salazar.  All rights reserved.
 http://chuck.stanford.edu/
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 U.S.A.
 -----------------------------------------------------------------------------*/

#include "chuck_type.h"
#include <string>

class TextFilter;

class Output
{
public:

    Output() { m_doc_text_filter = NULL; }
    virtual ~Output() { }
    void set_doc_text_filter(TextFilter *filter) { m_doc_text_filter = filter; }

    virtual void begin(const std::string &title) = 0;
    virtual void heading() = 0;
    virtual void end() = 0;
    
    virtual void begin_body() = 0;
    virtual void end_body() = 0;
    
    virtual void begin_toc() = 0;
    virtual void toc_class(Chuck_Type * type) = 0;
    virtual void end_toc() = 0;
    
    virtual void begin_classes() = 0;
    virtual void end_classes() = 0;
    
    virtual void begin_class(Chuck_Type * type) = 0;
    
    virtual void begin_examples() = 0;
    virtual void example(const std::string &name, const std::string &url) = 0;
    virtual void end_examples() = 0;
    
    virtual void begin_static_member_vars() = 0;
    virtual void end_static_member_vars() = 0;
    virtual void begin_member_vars() = 0;
    virtual void end_member_vars() = 0;
    
    virtual void begin_static_member_funcs() = 0;
    virtual void end_static_member_funcs() = 0;
    virtual void begin_member_funcs() = 0;
    virtual void end_member_funcs() = 0;
    
    virtual void static_member_var(Chuck_Value * var) = 0;
    virtual void member_var(Chuck_Value * var) = 0;
    
    virtual void begin_static_member_func(Chuck_Func * func) = 0;
    virtual void end_static_member_func() = 0;
    virtual void begin_member_func(Chuck_Func * func) = 0;
    virtual void end_member_func() = 0;
    virtual void func_arg(a_Arg_List arg) = 0;
    
    virtual void end_class() = 0;
    
protected:
    TextFilter *m_doc_text_filter;
};


class TextFilter
{
public:
    virtual std::string filter(const std::string &) = 0;
};


std::string urlforclass(const std::string &cls);


