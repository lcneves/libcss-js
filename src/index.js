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
 *
 * The client must implement the following handler functions:
 *
 * getTagName(node)
 *   returns a string with the tag name (e.g. "div").
 *
 * getAttributes(node)
 *   returns an array of objects in the format:
 *   { attribute: <string>, value: <string> }
 *
 * getSiblings(node)
 *   returns an array of objects in the format:
 *   { tagName: <string>, identifier: <string> }
 *   Array must be in the document order and must include the node being
 *   queried (i.e. must include all of the parent's children in order).
 *
 * getAncestors(node)
 *   returns an array of objects in the format:
 *   { tagName: <string>, identifier: <string> }
 *   Object at index 0 must be the parent; at index 1, the parent's parent,
 *   until the root element is reached. If node is root, array must be empty.
 *
 * isEmpty(node)
 *   returns a boolean: true if node has no children or text; otherwise false.
 */

var lh = require('./libcss-handler.js');

var ch; //Client handler functions

const DEFAULT_FONT_SIZE = 16;

function getString (chFun) {
  var nodeId = lh.Pointer_stringify(node);
  var results = chFun(nodeId);
  var ptr = allocate(
    lh.Module.intArrayFromString(results), 'i8', lh.Module.ALLOC_NORMAL);
  return ptr;
}

function getSelfIndex(identifier, siblings) {
  var selfIndex = -1;
  for (let i = 0; i < siblings.length; i++) {
    if (siblings[i].identifier === identifier) {
      selfIndex = i;
    }
  }
  if (selfIndex === -1) {
    throw new Error(
      'The siblings array does not contain the node being queried!');
  }
  return selfIndex;
}

function getPrevSibling(identifier, siblings) {
  var index = getSelfIndex(identifier, siblings);
  if (index === 0) {
    return null;
  }
  else {
    return siblings[index - 1];
  }
}

function getClasses (node) {
  var attributes = ch.getAttributes(node);
  var classes = [];
  for (let attribute of attributes) {
    if (attribute.attribute === 'class') {
      classes = attribute.value.split(' ');
      break;
    }
  }
  return classes;
}

function optionalHandler (node, chFun) {
  if (typeof chFun !== 'function') {
    return false;
  }
  var nodeId = lh.Pointer_stringify(node);
  return chFun(nodeId);
}

function pointerize (results) {
  var ptr = allocate(
    ch.Module.intArrayFromString(results), 'i8', ch.Module.ALLOC_NORMAL);
  return ptr;
}

var exportFunctions = [
  function js_node_name (node) {
    return getString(ch.getTagName);
  },
  function js_node_classes(node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = ch.getAttributes(nodeId);
    var classes = JSON.Stringify(getClasses(nodeId));
    return pointerize(classes);
  },
  function js_node_id(node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = ch.getAttributes(nodeId);
    var id = results['id'] ? results['id'] : '';
    return pointerize(id);
  },
  function js_named_ancestor_node(node, ancestor) {
    var nodeId = lh.Pointer_stringify(node);
    var ancestorName = lh.Pointer_stringify(ancestor);
    var results = '';
    var ancestors = ch.getAncestors(nodeId);
    for (let ancestor of ancestors) {
      if (ancestor.tagName === ancestorName) {
        results = ancestor.identifier;
        break;
      }
    }
    return pointerize(results);
  },
  function js_named_parent_node(node, parent) {
    var nodeId = lh.Pointer_stringify(node);
    var parentName = lh.Pointer_stringify(parent);
    var results = '';
    var ancestors = ch.getAncestors(nodeId);
    if (ancestors[0] && ancestors[0].tagName === parentName) {
      results = ancestors[0].identifier;
    }
    return pointerize(results);
  },
  function js_named_sibling_node(node, sibling) {
    var nodeId = lh.Pointer_stringify(node);
    var siblingName = lh.Pointer_stringify(sibling);
    var results = '';
    var siblings = ch.getSiblings(nodeId);
    var prevSibling = getPrevSibling(nodeId, siblings);
    if (prevSibling && prevSibling.tagName === siblingName) {
      results = prevSibling.identifier;
    }
    return pointerize(results);
  },
  function js_named_generic_sibling_node(node, sibling) {
    var nodeId = lh.Pointer_stringify(node);
    var siblingName = lh.Pointer_stringify(sibling);
    var results = '';
    var siblings = ch.getSiblings(nodeId);
    for (let sibling of siblings) {
      if (sibling.identifier !== nodeId && sibling.tagName === siblingName) {
        results = sibling.identifier;
        break;
      }
    }
    return pointerize(results);
  },
  function js_parent_node(node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = '';
    var ancestors = ch.getAncestors(nodeId);
    if (ancestors[0]) {
      results = ancestors[0].identifier;
    }
    return pointerize(results);
  },
  function js_sibling_node(node) {
    var nodeId = lh.Pointer_stringify(node);
    var results = '';
    var siblings = ch.getSiblings(nodeId);
    var prevSibling = getPrevSibling(nodeId, siblings);
    if (prevSibling) {
      results = prevSibling.identifier;
    }
    return pointerize(results);
  },
  function js_node_has_name(node, search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search).toLowerCase();
    var results = ch.getTagName(nodeId).toLowerCase();
    return query === results;
  },
  function js_node_has_class(node, search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search).toLowerCase();
    var classes = getClasses(nodeId);
    for (let className of classes) {
      if (className.toLowerCase() === query) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_id(node, search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search).toLowerCase();
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === 'id' &&
        attribute.value.toLowerCase() === query) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute(node, search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search).toLowerCase();
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute_equal(node, search, match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search);
    var value = lh.Pointer_stringify(match);
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query &&
        attribute.value.toLowerCase() === value) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute_dashmatch(node, search, match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search);
    var value = lh.Pointer_stringify(match);
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query &&
        (attribute.value.toLowerCase() === value ||
          attribute.value.toLowerCase() === value.concat('-'))) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute_includes(node, search, match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search);
    var value = lh.Pointer_stringify(match);
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query &&
        attribute.value.toLowerCase().indexOf(value) !== -1) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute_prefix(node, search, match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search);
    var value = lh.Pointer_stringify(match);
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query &&
        attribute.value.toLowerCase().startsWith(value)) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute_suffix(node, search, match) {
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search);
    var value = lh.Pointer_stringify(match);
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query &&
        attribute.value.toLowerCase().endsWith(value)) {
        return true;
      }
    }
    return false;
  },
  function js_node_has_attribute_substring(node, search, match) {
    // I don't see the difference between this and has_attribute_includes
    var nodeId = lh.Pointer_stringify(node);
    var query = lh.Pointer_stringify(search);
    var value = lh.Pointer_stringify(match);
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.attribute.toLowerCase() === query &&
        attribute.value.toLowerCase().indexOf(value) !== -1) {
        return true;
      }
    }
    return false;
  },
  function js_node_is_root(node, empty_search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    var ancestors = ch.getAncestors(nodeId);
    return ancestors[0] === undefined;
  },
  function js_node_count_siblings(node, same_name, after) {
    var nodeId = lh.Pointer_stringify(node);
    var siblings = getSiblings(nodeId);
    var selfIndex = getSelfIndex(nodeId, siblings);
    if (same_name) {
      var count = 0;
      var tagName = siblings[selfIndex].tagName.toLowerCase();
      if (after) {
        for (let i = selfIndex + 1; i < siblings.length; i++) {
          if (siblings[i].tagName.toLowerCase() === tagName) {
            count++;
          }
        }
      }
      else {
        for (let i = 0; i < selfIndex; i++) {
          if (siblings[i].tagName.toLowerCase() === tagName) {
            count++;
          }
        }
      }
      return count;
    }
    else {
      return after ? siblings.length - selfIndex - 1 : selfIndex;
    }
  },
  function js_node_is_empty(node, empty_search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    return ch.isEmpty(nodeId);
  },
  function js_node_is_link(node, empty_search, empty_match) {
    var nodeId = lh.Pointer_stringify(node);
    if (ch.getTagName(nodeId).toLowerCase() !== 'a') {
      return false;
    }
    var attributes = ch.getAttributes(nodeId);
    for (let attribute of attributes) {
      if (attribute.tagName.toLowerCase() === 'href' &&
        attribute.value) {
        return true;
      }
    }
    return false;
  },
  function js_node_is_visited(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isVisited);
  },
  function js_node_is_hover(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isHover);
  },
  function js_node_is_active(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isActive);
  },
  function js_node_is_focus(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isFocus);
  },
  function js_node_is_enabled(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isEnabled);
  },
  function js_node_is_disabled(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isDisabled);
  },
  function js_node_is_checked(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isChecked);
  },
  function js_node_is_target(node, empty_search, empty_match) {
    return optionalHandler(node, ch.isTarget);
  },
  function js_node_is_lang(node, search, empty_match) {
    if (typeof ch.isLang !== 'function') {
      return false;
    }
    var nodeId = lh.Pointer_stringify(node);
    var language = lh.Pointer_stringify(search);
    return ch.isLang(nodeId, language);
  },
  function js_ua_font_size() {
    if (typeof ch.uaFontSize !== 'function') {
      return DEFAULT_FONT_SIZE;
    }
    return ch.uaFontSize();
  }
];

module.exports.init = (clientHandlers) => {
  ch = clientHandlers;
}
