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
    m_output(output),
    m_func(NULL)
    { }
    
    void begin(const std::string &title)
    {
        m_title = title;
        if(m_title.length() == 0) m_title = "&nbsp;";
        
        fprintf(m_output, "<html>\n");
        fprintf(m_output, "<link rel=\"stylesheet\" type=\"text/css\" href=\"ckdoc.css\" />\n");
        fprintf(m_output, "<link rel=\"stylesheet\" type=\"text/css\" href=\"class.css\" />\n");
        fprintf(m_output, "<title>%s</title>\n", m_title.c_str());
        fprintf(m_output, "<body>\n");
    }
    
    void heading()
    {
        fprintf(m_output, "<a name=\"top\"/>\n");
        fprintf(m_output, "<div id=\"title\">\n");
        fprintf(m_output, "<div class=\"titleL\"><h1><a href=\"index.html\">ChucK Class Library Reference</a></h1></div>\n");
        fprintf(m_output, "<div class=\"titleR\"><h1>%s</h1></div>\n", m_title.c_str());
        fprintf(m_output, "</div>\n"); // #title
    }
    
    void end()
    {
        fprintf(m_output, "</html>\n");
    }
    
    void title(const std::string &_title)
    {
    }
    
    void begin_body()
    {
        fprintf(m_output, "<div id=\"body\">\n");
    }
    
    void end_body()
    {
        fprintf(m_output, "</div>\n"); // #body
        fprintf(m_output, "</body>\n");
    }
    
    void begin_toc()
    {
        fprintf(m_output, "<div class=\"toc\"><a id=\"toc\"/>\n");
    }
    
    void toc_class(Chuck_Type * type)
    {
        fprintf(m_output, "<p class=\"toc_class\"><a href=\"#%s\" class=\"%s\">%s</a></p>\n", 
                type->name.c_str(), class_for_type(type), type->name.c_str());
    }
    
    void end_toc()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void begin_classes()
    {
        fprintf(m_output, "<div class=\"classes\">\n");
    }
    
    void end_classes()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void begin_class(Chuck_Type * type)
    {
        fprintf(m_output, "<a name=\"%s\" /><div class=\"class\">\n", type->name.c_str());
        fprintf(m_output, "<h2 class=\"class_title\" name=\"%s\">%s</h2>\n",
                type->name.c_str(), type->name.c_str());
        
        // type heirarchy
        Chuck_Type * parent = type->parent;
        if(parent != NULL) fprintf(m_output, "<h4 class=\"class_hierarchy\">");
        while(parent != NULL)
        {
            fprintf(m_output, ": <a href=\"%s\" class=\"%s\">%s</a> ", 
                    urlforclass(parent->name).c_str(), class_for_type(parent), parent->name.c_str());
            parent = parent->parent;
        }
        if(type->parent != NULL) fprintf(m_output, "</h4>\n");
        
        if(type->doc.size() > 0)
        {
            std::string doc;
            if(m_doc_text_filter)
                doc = m_doc_text_filter->filter(type->doc);
            else
                doc = type->doc;
            fprintf(m_output, "<p class=\"class_description\">%s</p>\n", 
                    doc.c_str());
        }
        else
            fprintf(m_output, "<p class=\"empty_class_description\">No description available</p>\n");
    }
    
    void end_class()
    {
        fprintf(m_output, "<p class=\"top_link\">[ <a href=\"#top\">top</a> ]</p>\n");
        fprintf(m_output, "</div>\n<hr />\n");
    }
    
    void begin_examples()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">examples</h3><div class=\"examples\"><ul>\n");
    }
    
    void end_examples()
    {
        fprintf(m_output, "</ul></div>\n");
    }
    
    void begin_static_member_vars()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">static member variables</h3>\n<div class=\"members\">\n");
    }
    
    void end_static_member_vars()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void begin_member_vars()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">member variables</h3>\n<div class=\"members\">\n");
    }
    
    void end_member_vars()
    {
        fprintf(m_output, "</div>\n");
    }
    
    
    void begin_static_member_funcs()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">static member functions</h3>\n<div class=\"members\">\n");
    }
    
    void end_static_member_funcs()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void begin_member_funcs()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">member functions</h3>\n<div class=\"members\">\n");
    }
    
    void end_member_funcs()
    {
        fprintf(m_output, "</div>\n");
    }
    
    void example(const std::string &name, const std::string &url)
    {
        fprintf(m_output, "<li><a href=\"%s\">%s</a></li>\n", url.c_str(), name.c_str());
    }
    
    void static_member_var(Chuck_Value * var)
    {
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s", 
                class_for_type(var->type), var->type->name.c_str());
        if(var->type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < var->type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span></p>", var->name.c_str());
        
        if(var->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n", 
                    var->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");
        
        fprintf(m_output, "</div>\n");
    }
    
    void member_var(Chuck_Value * var)
    {
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s", 
                class_for_type(var->type), var->type->name.c_str());
        if(var->type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < var->type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span></p>", var->name.c_str());
        
        if(var->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n", 
                    var->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");
        
        fprintf(m_output, "</div>\n");
    }
    
    void begin_static_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s", 
                class_for_type(func->def->ret_type),
                func->def->ret_type->name.c_str());
        if(func->def->ret_type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < func->def->ret_type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span>(", S_name(func->def->name));        
        
        m_func = func;
    }
    
    void end_static_member_func()
    {
        fprintf(m_output, ")</p>\n");
        
        if(m_func->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n", 
                    m_func->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");
        
        fprintf(m_output, "</div>\n");
        
        m_func = NULL;
    }
    
    void begin_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s", 
                class_for_type(func->def->ret_type), 
                func->def->ret_type->name.c_str());
        if(func->def->ret_type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < func->def->ret_type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");
        
        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span>(", S_name(func->def->name));
        
        m_func = func;
    }
    
    void end_member_func()
    {
        fprintf(m_output, ")</p>\n");
        
        if(m_func->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n", 
                    m_func->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");
        
        fprintf(m_output, "</div>\n");
        
        m_func = NULL;
    }
    
    void func_arg(a_Arg_List arg)
    {
        // argument type
        fprintf(m_output, "<span class=\"%s\">%s", 
                class_for_type(arg->type), arg->type->name.c_str());            
        if(arg->type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < arg->type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");
        
        // argument name
        std::string varname = varnameclean(S_name(arg->var_decl->xid));
        fprintf(m_output, "%s", varname.c_str());
        
        if(arg->next != NULL)
            fprintf(m_output, ", ");
    }
    
private:
    FILE * m_output;
    Chuck_Func *m_func;
    std::string m_title;
    
    bool isugen(Chuck_Type *type) { return type->ugen_info != NULL; }
    
    std::string varnameclean(std::string vn)
    {
        // strip []
        vn.erase(std::remove(vn.begin(), vn.end(), '['), vn.end());
        vn.erase(std::remove(vn.begin(), vn.end(), ']'), vn.end());
        
        return vn;
    }
    
    std::string cssclean(std::string s)
    {
        for(int i = 0; i < s.length(); i++)
        {
            if(!isalnum(s[i])) s[i] = '_';
        }
        
        return s;
    }
    
    const char *class_for_type(Chuck_Type *type)
    {
        static char buf[1024];
        std::string name = cssclean(type->name);
        snprintf(buf, 1024, "typename type_%s", name.c_str());
        return buf;
    }
};

