/*
 * libcss-js.h
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Handler functions for libcss.
 * Based on the source code of netsurf and libcss.
 * Base files available at http://source.netsurf-browser.org
 *
 * Part of the libcss-js project.
 */

#ifndef _LIBCSS_JS_H_
#define _LIBCSS_JS_H_

typedef enum css_js_error {
	CSS_JS_OK             =  0,
	CSS_JS_ELEMENT        =  1,
	CSS_JS_PSEUDO         =  2,
	CSS_JS_CREATE_CTX     =  3,
	CSS_JS_CREATE_SHEET   =  4,
	CSS_JS_CREATE_STYLE   =  5,
	CSS_JS_DESTROY_CTX    =  6,
	CSS_JS_DESTROY_SHEET  =  7,
	CSS_JS_DESTROY_STYLE  =  8,
	CSS_JS_LEVEL          =  9,
	CSS_JS_APPEND_DATA    = 10,
	CSS_JS_DATA_DONE      = 11,
	CSS_JS_APPEND_SHEET   = 12,
	CSS_JS_HANDLER_LENGTH = 13
} css_js_error;

/*
 * Resets the selection context (i.e. removes all added CSS stylesheets).
 */
css_js_error reset_ctx();

/*
 * Adds a CSS stylesheet to the selection context.
 * Parameters:
 * 	css_string: a string of CSS text to be added to the selection context.
 * 	level: a string that specifies the language level of the CSS code.
 * 		Accepted values are "1", "2", "2.1" and "3".
 * 	url: a string that specifies the base URL for the stylesheet.
 * 		If none, should be "" (empty string).
 */
css_js_error add_stylesheet (const char* css_string, const char* level,
		const char* url);

/*
 * Sets the Javascript handler functions.
 * Parameters:
 * 	arr: an array of function pointers cast to uint64_t*.
 * 	len: the count of function pointers. Currently, must be 33.
 *
 * The array of functions must contain the following pointers, in order:
 * char* (*js_node_name)(const char* node);
 * char* (*js_node_classes)(const char* node);
 * char* (*js_node_id)(const char* node);
 * char* (*js_named_ancestor_node)(const char* node, const char* ancestor);
 * char* (*js_named_parent_node)(const char* node, const char* parent);
 * char* (*js_named_sibling_node)(const char* node, const char* sibling);
 * char* (*js_named_generic_sibling_node)(
 * 		const char* node, const char* sibling);
 * char* (*js_parent_node)(const char* node);
 * char* (*js_sibling_node)(const char* node);
 * bool (*js_node_has_name)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_class)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_id)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_attribute)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_attribute_equal)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_dashmatch)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_includes)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_prefix)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_suffix)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_substring)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_is_root)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * int32_t (*js_node_count_siblings)(const char* node, bool same_name,
 * 		bool after);
 * bool (*js_node_is_empty)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_link)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_visited)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_hover)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_active)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_focus)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_enabled)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_disabled)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_checked)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_target)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_lang)(
 * 		const char* node, const char* search, const char* empty_match);
 * int32_t (*js_ua_font_size)(void);
 */ 
css_js_error set_handlers(uint64_t* arr, size_t len);

#endif
