/*
 * example1.js
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Example client for libcss-js.
 */

'use strict';

const libcss = require('../index.js');

var nodes = [
  {
    id: 'idzero',
    tag: 'div',
    attributes: [
      { attribute: 'id', value: 'div-0' },
      { attribute: 'class', value: 'divs elements' },
      { attribute: 'style', value: 'justify-content: center;' }
    ],
    parent: null,
    children: ['idone']
  },
  {
    id: 'idone',
    tag: 'p',
    attributes: [],
    parent: 'idzero',
    children: []
  }
];

function getNodeById (identifier) {
  for (let node of nodes) {
    if (node.id === identifier) {
      return node;
    }
  }
}

function getProperty (identifier, property) {
  var node = getNodeById(identifier);
  if (!node)
    return null;

  return node[property];
}

var handlers = {
  getTagName: function (identifier) {
    return getProperty(identifier, 'tag');
  },
  getAttributes: function (identifier) {
    return getProperty(identifier, 'attributes');
  },
  getSiblings: function (identifier) {
    var node = getNodeById(identifier);
    var siblings = [];
    if (node.parent) {
      var parent = getNodeById(node.parent);
      for (let child of parent.children) {
        let childNode = getNodeById(child);
        siblings.push({ tagName: childNode.tag, identifier: childNode.id });
      }
    }
    else {
        siblings.push({ tagName: node.tag, identifier: node.id });
    }
    return siblings;
  },
  getAncestors: function (identifier) {
    var node = getNodeById(identifier);
    var ancestors = [];
    while (node.parent) {
      node = getNodeById(node.parent);
      ancestors.push({ tagName: node.tag, identifier: node.id });
    }
    return ancestors;
  },
  isEmpty (identifier) {
    var node = getNodeById(identifier);
    return (node.children[0] === undefined);
  }
};

libcss.init(handlers);

const css = '#div-0 { color: red; width: 70px; height: 50px; }';
const cssBis = 'div { align-content: center; }';
const cssTris = 'p { order: 1; display: block; }';
const cssQuat = 'div { flex: 3 3; }';

libcss.addSheet(css);
libcss.addSheet(cssBis);
libcss.addSheet(cssTris);
libcss.addSheet(cssQuat);

var computedStyleZero = libcss.getStyle('idzero');
var computedStyleOne = libcss.getStyle('idone');

console.dir(computedStyleZero);
console.log('\n');
console.dir(computedStyleOne);
