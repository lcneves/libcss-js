/*
 * index.js
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Entry point for the libcss-js project.
 */

'use strict';

/*
 * These are the signatures of the C functions that need to be implemented:
 *
 * const char* js_node_name(const char* node);
 * const char* js_node_classes(const char* node);
 * const char* js_node_id(const char* node);
 * const char* js_named_ancestor_node(const char* node, const char* ancestor);
 * const char* js_named_parent_node(const char* node, const char* parent);
 * const char* js_named_sibling_node(const char* node, const char* sibling);
 * const char* js_named_generic_sibling_node(
 *   const char* node, const char* sibling);
 * const char* js_parent_node(const char* node);
 * const char* js_sibling_node(const char* node);
 * bool js_node_has_name(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_class(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_id(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_attribute(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_attribute_equal(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_dashmatch(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_includes(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_prefix(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_suffix(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_substring(
 *   const char* node, const char* search, const char* match);
 * bool js_node_is_root(
 *   const char* node, const char* empty_search, const char* empty_match);
 * int32_t js_node_count_siblings(const char* node, bool same_name, bool after);
 * bool js_node_is_empty(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_link(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_visited(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_hover(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_active(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_focus(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_enabled(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_disabled(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_checked(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_target(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_lang(
 *   const char* node, const char* search, const char* empty_match);
 *
 * const int js_ua_font_size();
 */

var lh = require('./libcss-handler.js');

var ch; //Client handler functions
module.exports.init = (options) => ch = options;

var exportFunctions = [
  function js_node_name (node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = ch.getTagName(nodeId);
    var ptr = allocate(
      ch.Module.intArrayFromString(results), 'i8', ch.Module.ALLOC_NORMAL);
    return ptr;
  },
  function js_node_classes(node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = ch.getAttributes(nodeId);
    var classes = results['class'] ? results['class'].split(' ') : [];
    classes = JSON.stringify(classes);
    var ptr = allocate(
      ch.Module.intArrayFromString(classes), 'i8', ch.Module.ALLOC_NORMAL);
    return ptr;
  },
  function js_node_id(node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = ch.getAttributes(nodeId);
    var id = results['id'] ? results['id'] : '';
    var ptr = allocate(
      ch.Module.intArrayFromString(id), 'i8', ch.Module.ALLOC_NORMAL);
    return ptr;
  },
  function js_named_ancestor_node(node, ancestor) {},
  function js_named_parent_node(node, parent) {},
  function js_named_sibling_node(node, sibling) {},
  function js_named_generic_sibling_node(node, sibling) {},
  function js_parent_node(node) {},
  function js_sibling_node(node) {},
  function js_node_has_name(node, search, empty_match) {},
  function js_node_has_class(node, search, empty_match) {},
  function js_node_has_id(node, search, empty_match) {},
  function js_node_has_attribute(node, search, empty_match) {},
  function js_node_has_attribute_equal(node, search, match) {},
  function js_node_has_attribute_dashmatch(node, search, match) {},
  function js_node_has_attribute_includes(node, search, match) {},
  function js_node_has_attribute_prefix(node, search, match) {},
  function js_node_has_attribute_suffix(node, search, match) {},
  function js_node_has_attribute_substring(node, search, match) {},
  function js_node_is_root(node, empty_search, empty_match) {},
  function js_node_count_siblings(node,  same_name,  after) {},
  function js_node_is_empty(node, empty_search, empty_match) {},
  function js_node_is_link(node, empty_search, empty_match) {},
  function js_node_is_visited(node, empty_search, empty_match) {},
  function js_node_is_hover(node, empty_search, empty_match) {},
  function js_node_is_active(node, empty_search, empty_match) {},
  function js_node_is_focus(node, empty_search, empty_match) {},
  function js_node_is_enabled(node, empty_search, empty_match) {},
  function js_node_is_disabled(node, empty_search, empty_match) {},
  function js_node_is_checked(node, empty_search, empty_match) {},
  function js_node_is_target(node, empty_search, empty_match) {},
  function js_node_is_lang(node, search, empty_match) {},
  function js_ua_font_size() {}
];


