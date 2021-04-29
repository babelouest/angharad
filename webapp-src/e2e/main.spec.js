'use strict';

describe('The main view', function () {
  var page;

  beforeEach(function () {
    browser.get('/');
    page = require('./main.po');
  });

  it('should include the title', function() {
    expect(page.titleEl.getText()).toBe('A super angular homepage');
  });
});
