
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

#include "chuck_def.h"
#include "chuck_compile.h"
#include "util_thread.h"
#include "chuck_vm.h"
#include "chuck_otf.h"
#include "chuck_errmsg.h"
#include "chuck_globals.h"
#include "util_string.h"
#include "chuck_symbol.h"

#include "HTMLOutput.h"


using namespace std;

static bool g_vm_on = false;

t_CKBOOL start_vm(int argc, const char *argv[]);
t_CKBOOL stop_vm();

bool comp_func(Chuck_Func *a, Chuck_Func *b)
{
    return a->name < b->name;
}

bool comp_value(Chuck_Value *a, Chuck_Value *b)
{
    return a->name < b->name;
}


bool skip(string &name)
{
    if(name == "void" ||
       name == "int" ||
       name == "float" ||
       name == "time" ||
       name == "dur" ||
       name == "complex" ||
       name == "polar" ||
       name == "Class" ||
       name == "Thread" ||
       name == "@function")
        return true;
    return false;
}

bool isopt(const char *arg, const char *opt, const char **param = NULL)
{
    size_t len = strlen(opt);
    if(strncmp(arg, opt, len) == 0)
    {
        if(param != NULL)
            *param = arg+len;
        
        return true;
    }
    
    return false;
}

int main(int argc, const char ** argv)
{
    list<string> type_args;
    string title;
    string examples_path = "http://chuck.stanford.edu/doc/examples/";
    
    const char *param;    
    for(int i = 1; i < argc; i++)
    {
        if( isopt(argv[i], "--title:", &param) )
        {
            title = param;
        }
        else if( isopt(argv[i], "-v", &param) )
        {
            t_CKINT log_level = atoi(param);
            if(log_level == 0) log_level = CK_LOG_INFO;
            EM_setlog(log_level);
        }
        else if(strncmp(argv[i], "-", 1) != 0 && strncmp(argv[i], "--", 2) != 0)
        {
            type_args.push_back(argv[i]);
        }
    }
    
    start_vm(argc, argv);
    
    Output * output = new HTMLOutput(stdout);
    
    // iterate through types
    
    output->begin(title);
    
    Chuck_Env * env = Chuck_Env::instance();
    vector<Chuck_Type *> types;
    env->global()->get_types(types);
    
    if(type_args.size())
    {
        vector<Chuck_Type *> newTypes;
        
        // filter args
        for(list<string>::iterator i = type_args.begin(); 
            i != type_args.end(); i++)
        {
            bool foundIt = false;
            
            for(vector<Chuck_Type *>::iterator t = types.begin(); 
                t != types.end(); t++)
            {
                if((*t)->name == *i)
                {
                    newTypes.push_back(*t);
                    foundIt = true;
                    break;
                }
            }
            
            if(!foundIt)
                fprintf(stderr, "ckdoc: warning: type '%s' not found\n", i->c_str());
        }
        
        types = newTypes;
    }
    
    output->begin_toc();
    
    for(vector<Chuck_Type *>::iterator t = types.begin(); t != types.end(); t++)
    {
        Chuck_Type * type = *t;
        
        if(skip(type->name)) continue;
        
        output->toc_class(type);
    }
    
    output->end_toc();
    
    output->begin_classes();
    
    for(vector<Chuck_Type *>::iterator t = types.begin(); t != types.end(); t++)
    {
        Chuck_Type * type = *t;
        
        if(skip(type->name)) continue;
        
        // class name
        // fprintf(stderr, "## %s\n", type->name.c_str());
        output->begin_class(type);
        
        if(type->examples.size())
        {
            output->begin_examples();
        
            for(vector<string>::iterator _ex = type->examples.begin(); _ex != type->examples.end(); _ex++)
            {
                string ex = *_ex;
                string name;
                string url;
                
                if(ex.find("http://") == 0 || ex.find("https://") == 0)
                {
                    // absolute URL
                    url = ex;
                    int pos = ex.rfind('/');
                    if(pos != string::npos)
                        name = string(ex, pos+1);
                    else
                        name = ex;
                }
                else
                {
                    // relative URL
                    url = examples_path + ex;
                    name = ex;
                }
                        
                output->example(name, url);
            }
        
            output->end_examples();
        }
        
        if(type->info)
        {
            map<string, int> func_names;
            vector<Chuck_Func *> funcs;
            type->info->get_funcs(funcs);
            vector<Chuck_Value *> vars;
            type->info->get_values(vars);
            
            vector<Chuck_Func *> mfuncs;
            vector<Chuck_Func *> sfuncs;
            vector<Chuck_Value *> mvars;
            vector<Chuck_Value *> svars;
            
            // fprintf(stderr, "### %lu vars\n", vars.size());
            
            for(vector<Chuck_Value *>::iterator v = vars.begin(); v != vars.end(); v++)
            {
                Chuck_Value * value = *v;
                                
                if(value == NULL) continue;
                
                if(value->name.length() == 0)
                    continue;
                if(value->name[0] == '@')
                    continue;
                if(value->type->name == "[function]")
                    continue;
                
                if(value->is_static)
                    svars.push_back(value);
                else
                    mvars.push_back(value);
            }
            
            for(vector<Chuck_Func *>::iterator f = funcs.begin(); f != funcs.end(); f++)
            {
                Chuck_Func * func = *f;
                
                if(func == NULL) continue;
                
                if(func_names.count(func->name))
                    continue;
                func_names[func->name] = 1;
                
                if(func->def->static_decl == ae_key_static)
                    sfuncs.push_back(func);
                else
                    mfuncs.push_back(func);
            }
            
            sort(svars.begin(), svars.end(), comp_value);
            sort(mvars.begin(), mvars.end(), comp_value);
            sort(sfuncs.begin(), sfuncs.end(), comp_func);
            sort(mfuncs.begin(), mfuncs.end(), comp_func);
            
            if(svars.size())
            {
                output->begin_static_member_vars();
                for(vector<Chuck_Value *>::iterator v = svars.begin(); v != svars.end(); v++)
                    output->static_member_var(*v);
                output->end_static_member_vars();
            }
            
            if(mvars.size())
            {
                output->begin_member_vars();
                for(vector<Chuck_Value *>::iterator v = mvars.begin(); v != mvars.end(); v++)
                    output->member_var(*v);
                output->end_member_vars();
            }
            
            if(sfuncs.size())
            {
                output->begin_static_member_funcs();
                
                for(vector<Chuck_Func *>::iterator f = sfuncs.begin(); f != sfuncs.end(); f++)
                {
                    Chuck_Func * func = *f;
                    
                    output->begin_static_member_func(func);
                    
                    // argument list
                    a_Arg_List args = func->def->arg_list;
                    while(args != NULL)
                    {
                        output->func_arg(args);
                        args = args->next;
                    }
                    
                    output->end_static_member_func();
                }
                
                output->end_static_member_funcs();
            }
        
            if(mfuncs.size())
            {
                output->begin_member_funcs();
                
                for(vector<Chuck_Func *>::iterator f = mfuncs.begin(); f != mfuncs.end(); f++)
                {
                    Chuck_Func * func = *f;
                    
                    output->begin_member_func(func);
                    
                    // argument list
                    a_Arg_List args = func->def->arg_list;
                    while(args != NULL)
                    {
                        output->func_arg(args);
                        args = args->next;
                    }
                    
                    output->end_member_func();
                }
                
                output->end_member_funcs();
            }
        }
    
        output->end_class();
    }
    
    output->end_classes();
    
    output->end();

    stop_vm();
    
    return 0;
}


//-----------------------------------------------------------------------------
// name: start_vm()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL start_vm(int argc, const char *argv[])
{
    char buffer[1024];
    time_t t;
    
    time(&t);
    strncpy( buffer, ctime(&t), 24 );
    buffer[24] = '\0';
    
    // log
    EM_log( CK_LOG_SYSTEM, "-------( %s )-------", buffer );
    EM_log( CK_LOG_SYSTEM, "starting chuck virtual machine..." );
    // push log
    EM_pushlog();
    
    if( g_vm == NULL )
    {
        // log
        EM_log( CK_LOG_INFO, "allocating VM..." );
        t_CKBOOL enable_audio = FALSE;
        t_CKBOOL vm_halt = FALSE;
        t_CKUINT srate = 44100;
        t_CKUINT buffer_size = 512;
        t_CKUINT num_buffers = 1;
        t_CKUINT dac = 0;
        t_CKUINT adc = 0;
        t_CKBOOL set_priority = FALSE;
        t_CKBOOL block = FALSE;
        t_CKUINT output_channels = 2;
        t_CKUINT input_channels = 2;
        t_CKBOOL chugin_load = TRUE;
        
        // list of search pathes (added 1.3.0.0)
        std::list<std::string> dl_search_path;
        // initial chug-in path (added 1.3.0.0)
        std::string initial_chugin_path;
        // if set as environment variable (added 1.3.0.0)
        if( getenv( g_chugin_path_envvar ) )
        {
            // get it from the env var
            initial_chugin_path = getenv( g_chugin_path_envvar );
        }
        else
        {
            // default it
            initial_chugin_path = g_default_chugin_path;
        }
        // parse the colon list into STL list (added 1.3.0.0)
        parse_path_list( initial_chugin_path, dl_search_path );
        // list of individually named chug-ins (added 1.3.0.0)
        std::list<std::string> named_dls;
        
        const char *param;
        
        for(int i = 1; i < argc; i++)
        {
            if( isopt(argv[i], "--chugin-load:", &param) )
            {
                // get the rest
                string arg = param;
                if( arg == "off" ) chugin_load = 0;
                else if( arg == "auto" ) chugin_load = 1;
                else
                {
                    // error
                    fprintf( stderr, "[chuck]: invalid arguments for '--chugin-load'...\n" );
                    fprintf( stderr, "[chuck]: ... (looking for :auto or :off)\n" );
                    exit( 1 );
                }
            }
            else if( isopt(argv[i], "--chugin-path:", &param) )
            {
                // get the rest
                dl_search_path.push_back( param );
            }
            else if( isopt(argv[i], "-G", &param) )
            {
                // get the rest
                dl_search_path.push_back( param );
            }
            else if( isopt(argv[i], "--chugin:", &param) )
            {
                named_dls.push_back(param);
            }
            else if( isopt(argv[i], "-g", &param) )
            {
                named_dls.push_back(param);
            }
        }
        
        // set watchdog
        g_do_watchdog = FALSE;
        
        // allocate the vm - needs the type system
        g_vm = new Chuck_VM;
        
        if( !g_vm->initialize( enable_audio, vm_halt, srate, buffer_size,
                               num_buffers, dac, adc, output_channels,
                               input_channels, block ) )
        {
            fprintf( stderr, "[chuck]: %s\n", g_vm->last_error() );
            // pop
            EM_poplog();
            return FALSE;
        }
        
        // log
        EM_log( CK_LOG_INFO, "allocating compiler..." );
        
        // allocate the compiler
        g_compiler = new Chuck_Compiler;
        
        if(!chugin_load)
        {
            dl_search_path.clear();
            named_dls.clear();
        }
        else
        {
            // normalize paths
            for(std::list<std::string>::iterator i = dl_search_path.begin();
                i != dl_search_path.end(); i++)
                *i = expand_filepath(*i);
            for(std::list<std::string>::iterator j = named_dls.begin();
                j != named_dls.end(); j++)
                *j = expand_filepath(*j);
        }
                
        // initialize the compiler
        g_compiler->initialize( g_vm, dl_search_path, named_dls );
        // enable dump
        g_compiler->emitter->dump = FALSE;
        // set auto depend
        g_compiler->set_auto_depend( FALSE );
        
        // vm synthesis subsystem - needs the type system
        if( !g_vm->initialize_synthesis() )
        {
            fprintf( stderr, "[chuck]: %s\n", g_vm->last_error() );
            // pop
            EM_poplog();
            return FALSE;
        }
        
        // here we would normally preload chuck files
        // but that is disabled in doc mode
        
        // reset the parser
        reset_parse();
        
        Chuck_VM_Code * code = NULL;
        Chuck_VM_Shred * shred = NULL;
        
        // whether or not chug should be enabled (added 1.3.0.0)
        EM_log( CK_LOG_SEVERE, "pre-loading ChucK libs..." );
        EM_pushlog();
                
        // clear the list of chuck files to preload
        g_compiler->m_cklibs_to_preload.clear();
        
        // pop log
        EM_poplog();
    }
    
    g_vm_on = TRUE;
    // pop
    EM_poplog();
    
    return g_vm_on;
}

//-----------------------------------------------------------------------------
// name: stop_vm()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL stop_vm()
{
    // if it's there
    if( g_vm )
    {
        EM_log( CK_LOG_SYSTEM, "stopping chuck virtual machine..." );
        // get vm
        Chuck_VM * the_vm = g_vm;
        // flag the global one
        g_vm = NULL;
        // if not NULL
        if( the_vm && g_vm_on )
        {
            // flag
            g_vm_on = FALSE;
            
            // stop
            the_vm->stop();
            
            // wait a bit
            usleep( 100000 );
            
            // detach
            // all_detach();
            
#if !defined(__PLATFORM_WIN32__) && !defined(__WINDOWS_PTHREAD__)
            SAFE_DELETE( the_vm );
#endif
        }
        
        SAFE_DELETE( g_compiler );
        g_compiler = NULL;
    }
    
    return TRUE;
}

