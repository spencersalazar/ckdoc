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

#include "Output.h"

class HTMLOutput : public Output
{
public:
    HTMLOutput(FILE * output = stdout) :
    m_output(output)
    { }
    
    void begin()
    {
        fprintf(m_output,
"<html>\n\
<link rel=\"stylesheet\" type=\"text/css\" href=\"ckdoc.css\" />\n\
<body>\n\
");
    }
    
    void end()
    {
        fprintf(m_output, "</body>\n</html>\n");
    }
    
    void begin_class(Chuck_Type * type)
    {
        fprintf(m_output, "<div class=\"class\">\n");
        fprintf(m_output, "<a name=\"%s\" /><h2 name=\"%s\">%s</h2>\n",
                type->name.c_str(), type->name.c_str(), type->name.c_str());
        
        // type heirarchy
        Chuck_Type * parent = type->parent;
        if(parent != NULL) fprintf(m_output, "<h4>");
        while(parent != NULL)
        {
            fprintf(m_output, ": <a href=\"#%s\">%s</a> ", parent->name.c_str(), parent->name.c_str());
            parent = parent->parent;
        }
        if(type->parent != NULL) fprintf(m_output, "</h4>\n");
    }
    
    void end_class()
    {
        fprintf(m_output, "</div>\n<hr />\n");
    }
    
    void begin_static_member_vars()
    {
        fprintf(m_output, "<h3>static member variables</h3>\n<div class=\"members\">\n");
    }
    
    void end_static_member_vars()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void begin_member_vars()
    {
        fprintf(m_output, "<h3>member variables</h3>\n<div class=\"members\">\n");
    }
    
    void end_member_vars()
    {
        fprintf(m_output, "</div>\n");
    }
    
    
    void begin_static_member_funcs()
    {
        fprintf(m_output, "<h3>static member functions</h3>\n<div class=\"members\">\n");
    }
    
    void end_static_member_funcs()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void begin_member_funcs()
    {
        fprintf(m_output, "<h3>member functions</h3>\n<div class=\"members\">\n");
    }
    
    void end_member_funcs()
    {
        fprintf(m_output, "</div>\n");
    }
    
    
    void static_member_var(Chuck_Value * var)
    {
        fprintf(m_output, "<p><span class=\"typename\">%s", var->type->name.c_str());
        for(int i = 0; i < var->type->array_depth; i++)
            fprintf(m_output, "[]");
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"name\">%s</span></p>", var->name.c_str());
    }
    
    void member_var(Chuck_Value * var)
    {
        fprintf(m_output, "<p><span class=\"typename\">%s", var->type->name.c_str());
        for(int i = 0; i < var->type->array_depth; i++)
            fprintf(m_output, "[]");
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"name\">%s</span></p>", var->name.c_str());
    }
    
    void begin_static_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "<p><span class=\"typename\">%s", func->def->ret_type->name.c_str());
        for(int i = 0; i < func->def->ret_type->array_depth; i++)
            fprintf(m_output, "[]");
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"name\">%s</span>(", S_name(func->def->name));
        
    }
    
    void end_static_member_func()
    {
        fprintf(m_output, ")</p>\n");
    }
    
    void begin_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "<p><span class=\"typename\">%s", func->def->ret_type->name.c_str());
        for(int i = 0; i < func->def->ret_type->array_depth; i++)
            fprintf(m_output, "[]");
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"name\">%s</span>(", S_name(func->def->name));
    }
    
    void end_member_func()
    {
        fprintf(m_output, ")</p>\n");
    }
    
    void func_arg(a_Arg_List arg)
    {
        // argument type
        fprintf(m_output, "<span class=\"typename\">%s", arg->type->name.c_str());
        for(int i = 0; i < arg->type->array_depth; i++)
            fprintf(m_output, "[]");
        fprintf(m_output, "</span> ");
        
        // argument name
        fprintf(m_output, "%s", S_name(arg->var_decl->xid));
        
        if(arg->next != NULL)
            fprintf(m_output, ", ");
    }
    
private:
    FILE * m_output;
};

