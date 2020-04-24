#include <tg.h>

__private utf8_t* defaultPatterns = (utf8_t*)TG_PATTERNS;

/*******************************************************************************/
/************************************** SECTION INIT ***************************/
/*******************************************************************************/

void tg_begin(tg_s* tg, const char* fontName, const char* fontFall, int size, const char* finput, const char* foutput, utf8_t* patterns){
	if( !finput ) err_fail("no input file, --help for usage");
	if( size < 6 ) size = 6;
	dbg_info("font:%s", fontName);
	dbg_info("fall:%s", fontFall);
	dbg_info("size:%d", size);
	dbg_info("finp:%s", finput);

	os_begin();
	utf_begin();
	term_begin();
	term_load(NULL, term_name());
	term_load(NULL, term_name_ef());
	term_buff_same_screen();
	ft_begin();

	tg->inp = path_resolve(finput);
	if( foutput ) tg->out = path_resolve(foutput);	
	
	dbg_info("fout:%s", foutput);

	winsize_s ws;
	if( term_winsize_get(&ws) ) err_fail("unable to read screen info");
	tg->screenCol = ws.ws_col - 1;
	tg->screenRow = ws.ws_row - 1;
	dbg_info("scrC:%u", tg->screenCol);
	dbg_info("scrR:%u", tg->screenRow);
	tg->fontSize = (unsigned)size;
	dbg_info("use %u font size", tg->fontSize);
	tg->fontName = fontName ? fontName : TG_DEFAULT_FONT;
	tg->fontFallback = fontFall ? fontFall : fontName;

	tg->fonts = ft_fonts_new();
	ftFont_s* font = ft_fonts_load(tg->fonts, tg->fontName, "main");
	if( !font ) err_fail("unable to load %s font", tg->fontName);
	ft_font_size(font, size, size);
	font = ft_fonts_load(tg->fonts, tg->fontFallback, "fallback");
	if( !font ) err_fail("unable to load %s font", tg->fontFallback);
	ft_font_size(font, size, size);
		
	tg->patterns = !patterns ? defaultPatterns : patterns;
	dbg_info("patt:%s", tg->patterns);
	utf8Iterator_s it = utf8_iterator(tg->patterns, 0);
	utf_t utf;
	size_t count = utf_width(tg->patterns);
	tg->vfcr = vector_new(ftRender_s*, count, 1);
	tg->vfcrbits = vector_new(int, count, 1);
	g2dCoord_s pos = {0};
	while( (utf=utf8_iterator_next(&it)) ){
		ftRender_s* render = ft_fonts_glyph_load(tg->fonts, utf, FT_RENDER_ANTIALIASED);
		if( !render ) err_fail("invalid utf 0x%X", utf);
		vector_push_back(tg->vfcr, render);
		pos.w = render->img->w;
		pos.h = render->img->h;
		unsigned bc = g2d_bitcount(render->img, &pos);
		vector_push_back(tg->vfcrbits, bc);
	}
	tg->fontW = tg->vfcr[0]->horiAdvance;
	tg->fontH = tg->vfcr[0]->img->h;

	tg->imgW = tg->fontW * tg->screenCol;
	tg->imgH = tg->fontH * tg->screenRow;

	dbg_info("fontW:%u", tg->fontW);
	dbg_info("fontH:%u", tg->fontH);
	dbg_info("imgW:%u", tg->imgW);
	dbg_info("imgH:%u", tg->imgH);

	err_clear();
}

void tg_end(tg_s* tg){
	ft_fonts_free(tg->fonts);
	if( tg->inp ) free(tg->inp);
	if( tg->out ) free(tg->out);
	
	vector_free(tg->vfcr);
	ft_end();
	term_buff_end();
	term_end();	
}


/*************/
/*** STAGE ***/
/******************************************************************************************************/
/* for each block, grab rgb, convert to gray scale, get min max gray, convert to bitmap, find pattern */
/******************************************************************************************************/

__private utf_t tg_pattern_find(tg_s* tg, g2dImage_s* bw, g2dCoord_s* ipos){
	int bestMatch = 0;
	int bestFontError  = INT_MAX;
	int bestImageError = INT_MAX;
	utf_t bestUtf = ' ';

	vector_foreach(tg->vfcr, i){
		g2dCoord_s ftpos = {
			.x = 0, 
			.y = 0, 
			.w = tg->vfcr[i]->img->w, 
			.h = tg->vfcr[i]->img->h 
		};
		int match = g2d_compare_similar(bw, ipos, tg->vfcr[i]->img, &ftpos);
		int errImag = g2d_bitcount(bw, ipos) - match;
		int errFont = tg->vfcrbits[i] - match;
		//dbg_info("utf:0x%X match:%d errImg:%d errFont:%d", tg->vfcr[i]->utf, match, errImag, errFont);
		if( match > bestMatch && errFont < bestFontError && errImag < bestImageError){
			bestMatch = match;
			bestFontError = errFont;
			bestImageError = errImag;
			bestUtf = tg->vfcr[i]->utf;
		}
	}
	if( !bestUtf ) err_fail("pattern not find");
	return bestUtf;
}


__private void tg_convert_put(tgImg_s* ti, g2dImage_s* img, g2dColor_t* color, utf_t utf){
	char tmp[256];
	utf8_t pch[8] = {0};

	term_escapemk(tmp, "color24_fg", g2d_color_red(img, color[0]), g2d_color_green(img, color[0]), g2d_color_blue(img,color[0]));
	ti->end = str_cpy(ti->end, ti->size - (ti->end - ti->img), tmp); 
	term_escapemk(tmp, "color24_bk", g2d_color_red(img, color[1]), g2d_color_green(img, color[1]), g2d_color_blue(img,color[1]));
	ti->end = str_cpy(ti->end, ti->size - (ti->end - ti->img), tmp);
	utf_putch(pch, utf);
	ti->end = str_cpy(ti->end, ti->size - (ti->end - ti->img), (char*)pch);
}

__private void tg_convert_put_nl(tgImg_s* ti){
	char tmp[256];

	term_escapemk(tmp, "color_reset");
	ti->end = str_cpy(ti->end, ti->size - (ti->end - ti->img), tmp); 

	*ti->end++ = '\n';
	*ti->end = 0;
}

__private tgImg_s* tg_convert_g2d(tg_s* tg, g2dImage_s* img, unsigned cols, unsigned rows){ 
	tgImg_s* out = mem_new(tgImg_s);
	if( !out ) err_fail("eom");
	out->size = (256+4) * (cols+1) * rows;
	out->img = mem_many(char, out->size);
	if( !out->img ) err_fail("eom");
	out->end = out->img;
	*out->end = 0;
	out->delay = 0;

	__g2d_free g2dImage_s* gray = g2d_copy(img);
	g2d_luminance(gray);
	g2dColor_t dom[2];

	
	for( unsigned r = 0; r < rows; ++r ){
		g2dCoord_s pos = { 
			.x = 0, 
			.y = r * tg->fontH, 
			.w = tg->fontW, 
			.h = tg->fontH 
		};
		for( unsigned c = 0; c < cols; ++c){
			pos.x = c * pos.w;
			g2d_black_white(gray, &pos);
			g2d_black_white_dominant(dom, img, gray, &pos);
			utf_t utf = tg_pattern_find(tg, gray, &pos);
			tg_convert_put(out, img, dom, utf);
		}
		
		tg_convert_put_nl(out);
	}
	return out;
}

__private tgImg_s* tg_convert_image(tg_s* tg, int aspectRatio){
	unsigned rows, cols;
	unsigned w = tg->fontW * tg->screenCol;
	unsigned h = tg->fontH * tg->screenRow;

	__g2d_free g2dImage_s* img = g2d_load(tg->inp, w, h, aspectRatio);
	if( !img ) return NULL;

	cols = img->w / tg->fontW;
	rows = img->h / tg->fontH;
	if( cols > tg->screenCol ) err_fail("math cols %u > %u", cols, tg->screenCol);
	if( rows > tg->screenRow ) err_fail("math rows %u > %u", rows, tg->screenRow);

	return tg_convert_g2d(tg, img, cols, rows);	
}

__private void tg_display(tgImg_s* img){
	term_print_str(img->img);
	term_flush();
}

__private void tg_save_raw(tgImg_s* img, const char* fname){
	FILE* f = fopen(fname, "w");
	if( !f ){
		err_pushno("%s", fname);
		err_fail("save file %s", fname);
	}
	fwrite(img->img, sizeof(char), img->end - img->img, f);
	fclose(f);
}

__private void tg_save_frame(tgImg_s* img, FILE* fd){
	fwrite(&img->size, sizeof(size_t), 1, fd);
	fwrite(&img->delay, sizeof(size_t), 1, fd);
	fwrite(img->img, sizeof(char), img->end - img->img, fd);
}

__private FILE* tg_save_header(const char* fname){
	FILE* f = fopen(fname, "w");
	if( !f ){
		err_pushno("%s", fname);
		err_fail("save file %s", fname);
	}
	int magic = TGI_MAGICK;
	fwrite(&magic , sizeof(int), 1, f);
	return f;
}

__private void tg_img_free(tgImg_s* img){
	free(img->img);
	free(img);
}

void tg_convert(tg_s* tg, int aspectRatio, int tgi){
	tgImg_s* ret = tg_convert_image(tg, aspectRatio);
	if( ret ){
		if( tg->out ){
			if( tgi ){
				FILE* f = tg_save_header(tg->out);
				tg_save_frame(ret, f);
				fclose(f);
			}
			else{
				tg_save_raw(ret, tg->out);
			}
		}
		else{
			tg_display(ret);
		}
		tg_img_free(ret);
		return;
	}

	err_fail("unable convert image");
}

