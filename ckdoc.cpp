

extern "C" int chuck_main( int argc, const char ** argv );

int main(int argc, const char ** argv)
{
    const char ** ck_argv = { "chuck", "--loop" };
    return chuck_main(2, ck_argv);
}
