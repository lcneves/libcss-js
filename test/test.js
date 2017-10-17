/*
 * test.js
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * This test uses the data file for NetSurf libcss's selection test.
 * Part of the libcss.js project.
 */

'use strict';

const path = require('path');
const fs = require('fs');
const libcss = require('../index.js');

const DEFAULT_FONT_SIZE = 12;

var root = null;
var count = 0;
var elements = {};

function getElementById (identifier) {
  if (typeof elements[identifier] === 'undefined')
    throw new Error('Unable to find element with identifier "' + identifier
      + '"!');

  return elements[identifier];
}

var handlers = {
  getTagName: function (identifier) {
    var element = getElementById(identifier);
    return element.tagName;
  },
  getAttributes: function (identifier) {
    var element = getElementById(identifier);
    return element.attributes;
  },
  getSiblings: function (identifier) {
    var element = getElementById(identifier);
    var siblings = [];
    if (element.parent && element.parent.tagName !== 'root') {
      for (let child of element.parent.children) {
        siblings.push({ tagName: child.tagName, identifier: child.id });
      }
    }
    return siblings;
  },
  getAncestors: function (identifier) {
    var element = getElementById(identifier);
    var ancestors = [];
    while (element.parent && element.parent.tagName !== 'root') {
      element = element.parent;
      ancestors.push({ tagName: element.tagName, identifier: element.id });
    }
    return ancestors;
  },
  isEmpty: function (identifier) {
    var element = getElementById(identifier);
    return (element.children[0] === undefined);
  },
  uaFontSize: function () {
    return DEFAULT_FONT_SIZE * 10;
  }
};

libcss.init(handlers);

function makeElement (tagName, parentElement, num) {
  var element = {
    id: num.toString(),
    tagName: tagName,
    children: [],
    attributes: []
  };
  element.parent = parentElement;
  if (parentElement && Array.isArray(parentElement.children)) {
    parentElement.children.push(element);
  }

  elements[element.id] = element;
  return element;
}

function parseExpected (data) {
  var resultsArr = data.split('\n');
  var resultsObj = {};
  for (let line of resultsArr) {
    let colon = line.indexOf(':');
    if (colon > 0) {
      let prop = line.substring(0, colon).trim();
      let value = line.substring(colon + 1).trim();
      resultsObj[prop] = value;
    }
  }

  return resultsObj;
}

function elementDepth (line) {
  var i = 0;
  while (line[i] === ' ') i++;
  return i;
}

function findParent(currentElement, currentDepth, newDepth) {
  var parentElement = currentElement;
  for (let i = newDepth; i - currentDepth < 1; i++) {
    parentElement = parentElement.parent;
  }
  
  return parentElement;
}

fs.readFile(
  path.join(__dirname,
    '..', 'src', 'libcss', 'test', 'data', 'select', 'tests1.dat'),
  'utf8',
  (err, data) => {
    if (err) throw err;

    var testDataArr = data.split('#reset').map((item) => item.trim());
    var testNum = 1;

    for (let testData of testDataArr) {
      if (!testData) continue;
      if (testData.indexOf('#expected') === -1) continue;

      let testParts = testData.split('#expected').map((item) => item.trim());

      let expectedResults = parseExpected(testParts[1]);

      root = makeElement('root', null, count++);
      let currentElement = root;
      let currentDepth = 0;
      let queryElement = null;

      let parsedCSS = [];

      let lines = testParts[0].split('\n').map((item) => item.trim());
      for (let line of lines) {
        if (!line || line.indexOf('#') === 0) continue;

        if (line.indexOf('|') === 0) {
          line = line.substring(1);

          if (line.indexOf('=') !== -1) {
            let params = line.split('=').map((item) => item.trim());
            let attribute = { attribute: params[0], value: params[1] };
            currentElement.attributes.push(attribute);
          }
          else {
            let depth = elementDepth(line);
            let parentElement = findParent(currentElement, currentDepth, depth);
            let query = false;
            let starIndex = line.indexOf('*');
            if (starIndex !== -1) {
              query = true;
              line = line.slice(0, -1);
            }

            let newElement = makeElement(line.trim(), parentElement, count++);
            if (query) {
              queryElement = newElement;
            }

            currentElement = newElement;
            currentDepth = depth;
          }
        }
        else {
          parsedCSS.push(line);
          libcss.addSheet(line);
        }
      }

      let results = libcss.getStyle(queryElement.id);
      let err = '';

      for (let property in expectedResults) {
        if (expectedResults.hasOwnProperty(property)) {
          if (results[property] !== expectedResults[property]) {
            err +=
              'Expected: ' + property + ': ' + expectedResults[property] + '\n'
              + 'Verified: ' + property + ': ' + results[property] + '\n';
          }
        }
      }
      if (err) {
        console.error('Test ' + testNum + ' FAIL!' + '\n' + err);
        console.dir(root, { depth: null });
        console.info('Query: ' + queryElement.id);
        console.dir(parsedCSS);
      } else {
        console.info('Test ' + testNum + ' PASS!');
      }
      testNum++;
      elements = {};
      libcss.reset();
    }
  }
);
