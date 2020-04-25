#include <tg.h> 

typedef enum{
	ARG_HELP,
	ARG_CONVERT,
	ARG_INP,
	ARG_OUT,
	ARG_FONTS,
	ARG_FALL,
	ARG_SIZE,
	ARG_PATT,
	ARG_ASPECT,
	ARG_RAW
}optarg_e;

long defaultSize = 12;
long defaultRatio = 0;
double seek = SNAN;

argdef_s args[] = {
	{0, 'h', "help",     ARGDEF_NOARG,  NULL,          "help/usage"},
	{0, 'c', "convert",  ARGDEF_NOARG,  NULL,          "convert input to output"},
	{0, 'i', "input",    ARGDEF_STR,    NULL,          "input file"},
	{0, 'o', "output",   ARGDEF_STR,    NULL,          "output file, default stdout"},
	{0, 'f', "fonts",    ARGDEF_STR,    NULL,          "select font name, default monospace"},
	{0, 'F', "fallback", ARGDEF_STR,    NULL,          "select fallback font, default == fonts"},
	{0, 's', "size",     ARGDEF_SIGNED, &defaultSize,  "select font size, deafut 12"},
	{0, 'p', "patterns", ARGDEF_STR,    NULL,          "select character use for display image"},
	{0, 'a', "aspect",   ARGDEF_SIGNED, &defaultRatio, "select aspet ratio, -1 no scale, 0 auto, 1 width, 2 height"},
	{0, 'r', "raw",      ARGDEF_NOARG,  NULL,          "save raw mode"},
	{0, 'S', "seek",     ARGDEF_DOUBLE, &seek,          "TODO NOT WORKING seeking video to seconds from start"},
	{0,  0 , NULL,       ARGDEF_NOARG,  NULL,          NULL}
};

int main(int argc, char** argv){
	err_begin();
	err_enable();

	tg_s tg = {0};

	if( opt_parse(args, argv, argc) < 0 ){
		opt_error(argc, argv);
		return -1;
	}

	if( opt_enabled(args, ARG_HELP) ){
		opt_usage(args, argv[0]);
		return 0;
	}

	tg_begin(
		&tg,
		opt_arg_str(args, ARG_FONTS),
		opt_arg_str(args, ARG_FALL),
		defaultSize,
		opt_arg_str(args, ARG_INP),
		opt_arg_str(args, ARG_OUT),
		(utf8_t*)opt_arg_str(args, ARG_PATT)
	);
	
	tg.seeking = seek;
	tg.aspectRatio = defaultRatio;

	if( opt_enabled(args, ARG_CONVERT) ){
		tg_convert(
			&tg,
			!opt_enabled(args, ARG_RAW)
		);
	}
	else{
		tg_view(&tg);
	}
	
	tg_end(&tg);
err_print();
	err_end();
}
