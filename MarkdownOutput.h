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
#include <stdio.h>

class MarkdownOutput : public Output
{
public:
    
    MarkdownOutput(FILE * output = stdout) :
    m_output(output)
    { }
    
    void begin_class(Chuck_Type * type)
    {
        fprintf(m_output, "## %s\n", type->name.c_str());
        
        // type heirarchy
        Chuck_Type * parent = type->parent;
        if(parent != NULL) fprintf(m_output, "####");
        while(parent != NULL)
        {
            fprintf(m_output, "< %s ", parent->name.c_str());
            parent = parent->parent;
        }
        if(type->parent != NULL) fprintf(m_output, "\n");
    }
    
    void end_class()
    {
        fprintf(m_output, "- - -\n");
    }
    
    void begin_static_member_vars()
    {
        
    }
    
    void end_static_member_vars()
    {
        
    }
    
    void begin_member_vars()
    {
        
    }
    
    void end_member_vars()
    {
        
    }
    
    
    void begin_static_member_funcs()
    {
        fprintf(m_output, "### static member functions\n");
    }
    
    void end_static_member_funcs()
    {
        
    }
    
    void begin_member_funcs()
    {
        fprintf(m_output, "### member functions\n");
    }
    
    void end_member_funcs()
    {
        
    }
    
    
    void static_member_var(Chuck_Value * var)
    {
        
    }
    
    void member_var(Chuck_Value * var)
    {
        
    }
    
    void begin_static_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "*%s", func->def->ret_type->name.c_str());
        for(int i = 0; i < func->def->ret_type->array_depth; i++)
            fprintf(m_output, "\\[\\]");
        fprintf(m_output, "* ");
        
        // function name
        fprintf(m_output, "**%s**(", S_name(func->def->name));
        
    }
    
    void end_static_member_func()
    {
        fprintf(m_output, ")\n\n");
    }
    
    void begin_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "*%s", func->def->ret_type->name.c_str());
        for(int i = 0; i < func->def->ret_type->array_depth; i++)
            fprintf(m_output, "\\[\\]");
        fprintf(m_output, "* ");
        
        // function name
        fprintf(m_output, "**%s**(", S_name(func->def->name));
    }
    
    void end_member_func()
    {
        fprintf(m_output, ")\n\n");
    }
    
    void func_arg(a_Arg_List arg)
    {
        // argument type
        fprintf(m_output, "*%s", arg->type->name.c_str());
        for(int i = 0; i < arg->type->array_depth; i++)
            fprintf(m_output, "\\[\\]");
        fprintf(m_output, "* ");
        
        // argument name
        fprintf(m_output, "%s", S_name(arg->var_decl->xid));
        
        if(arg->next != NULL)
            fprintf(m_output, ", ");
    }
    
private:
    FILE * m_output;
};

