
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

#include <string>

using namespace std;

static bool g_vm_on = false;

t_CKBOOL start_vm();
t_CKBOOL stop_vm();

bool isugen(Chuck_Type *type) { return type->ugen_info != NULL; }

std::string cssclean(std::string s)
{
    for(int i = 0; i < s.length(); i++)
    {
        if(!isalnum(s[i])) s[i] = '_';
    }
    
    return s;
}

const char *css_for_type(Chuck_Type *type)
{
    static char buf[1024];
    
    std::string name = cssclean(type->name.c_str());
    
    if(isprim(type) || type == &t_void || (type->array_depth && isprim(type->array_type)))
        snprintf(buf, 1024, ".type_%s { color: blue; }", name.c_str());
    else if(isugen(type))
        snprintf(buf, 1024, ".type_%s { color: #A200EC; }", name.c_str());
    else
        snprintf(buf, 1024, ".type_%s { color: #800023; }", name.c_str());
    
    return buf;
}

int main(int argc, const char ** argv)
{
    start_vm();
    
    // iterate through types
    
    Chuck_Env * env = Chuck_Env::instance();
    vector<Chuck_Type *> types;
    env->global()->get_types(types);
    
    for(vector<Chuck_Type *>::iterator t = types.begin(); t != types.end(); t++)
    {
        Chuck_Type * type = *t;
        printf("%s\n", css_for_type(type));
    }

    stop_vm();
    
    return 0;
}


//-----------------------------------------------------------------------------
// name: start_vm()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL start_vm()
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
        
        // set watchdog
        g_do_watchdog = FALSE;
        
        // allocate the vm - needs the type system
        g_vm = new Chuck_VM;
        
        if( !g_vm->initialize( srate, output_channels, input_channels, buffer_size, vm_halt ) )
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
        
        
        // list of search pathes (added 1.3.0.0)
        std::list<std::string> library_paths;
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
        parse_path_list( initial_chugin_path, library_paths );
        
        std::list<std::string> named_chugins;
        // normalize paths
        for(std::list<std::string>::iterator i = library_paths.begin();
            i != library_paths.end(); i++)
            *i = expand_filepath(*i);
        for(std::list<std::string>::iterator j = named_chugins.begin();
            j != named_chugins.end(); j++)
            *j = expand_filepath(*j);
        
        // initialize the compiler
        g_compiler->initialize( g_vm, library_paths, named_chugins );
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
        
        // reset the parser
        reset_parse();
        
        Chuck_VM_Code * code = NULL;
        Chuck_VM_Shred * shred = NULL;
        
        // whether or not chug should be enabled (added 1.3.0.0)
        EM_log( CK_LOG_SEVERE, "pre-loading ChucK libs..." );
        EM_pushlog();
        
        // iterate over list of ck files that the compiler found
        for( std::list<std::string>::iterator j = g_compiler->m_cklibs_to_preload.begin();
            j != g_compiler->m_cklibs_to_preload.end(); j++)
        {
            // the filename
            std::string filename = *j;
            
            // log
            EM_log( CK_LOG_SEVERE, "preloading '%s'...", filename.c_str() );
            // push indent
            EM_pushlog();
            
            // SPENCERTODO: what to do for full path
            std::string full_path = filename;
            
            // parse, type-check, and emit
            if( g_compiler->go( filename, NULL, NULL, full_path ) )
            {
                // TODO: how to compilation handle?
                //return 1;
                
                // get the code
                code = g_compiler->output();
                // name it - TODO?
                // code->name += string(argv[i]);
                
                // spork it
                shred = g_vm->spork( code, NULL );
            }
            
            // pop indent
            EM_poplog();
        }
        
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

