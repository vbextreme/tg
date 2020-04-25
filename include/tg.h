#ifndef __TG_H__
#define __TG_H__

#include <ef/type.h>
#include <ef/term.h>
#include <ef/err.h>
#include <ef/optex.h>
#include <ef/file.h>
#include <ef/str.h>
#include <ef/utf8.h>
#include <ef/ft.h>
#include <ef/vector.h>
#include <ef/os.h>
#include <ef/image.h>
#include <ef/imageFiles.h>
#include <ef/imageGif.h>
#include <ef/media.h>
#include <ef/delay.h>

#define TG_DEFAULT_FONT "monospace"
#define TGI_MAGICK      0xF1CA
#define TG_PATTERNS     " ▀▁▂▃▄▅▆▇█▉▊▋▌▍▎▏▐░▒▓▔▕▖▗▘▙▚▛▜▝▞▟"

typedef struct tg{
	char* inp;
	char* out;
	const char* fontName;
	const char* fontFallback;
	utf8_t* patterns;
	ftRender_s** vfcr;
	int* vfcrbits;
	ftFonts_s* fonts;
	unsigned fontSize;
	unsigned screenCol;
	unsigned screenRow;
	unsigned fontW;
	unsigned fontH;
	unsigned imgW;
	unsigned imgH;
}tg_s;

typedef struct tgImg{
	char* img;
	char* end;
	unsigned fps;
	size_t size;
	long delay;
}tgImg_s;


void tg_begin(tg_s* tg, const char* fontName, const char* fontFall, int size, const char* finput, const char* foutput, utf8_t* patterns);
void tg_end(tg_s* tg);
void tg_convert(tg_s* tg, int aspectRatio, int tgi);
void tg_view(tg_s* tg);


#endif
